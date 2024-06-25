#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lvgl.h"
#include "esp_lcd_st7789.h"
#include "esp_keypad.h"
#include "lvgl.h"

static const char *TAG = "esp_lvgl";

// Using SPI2 in the example
#define LCD_SPI_HOST  SPI2_HOST

#define LCD_SPI_CLK_FREQ    (20 * 1000 * 1000)
#define LCD_BL_ON_LEVEL     1
#define LCD_BL_OFF_LEVEL    !LCD_BL_ON_LEVEL
#define LCD_PIN_CLK         GPIO_NUM_36
#define LCD_PIN_DAT         GPIO_NUM_35
#define LVLCD_PIN_DC        GPIO_NUM_34
#define LVLCD_PIN_RST       GPIO_NUM_33
#define LVLCD_PIN_CS        GPIO_NUM_37
#define LVLCD_PIN_BL        GPIO_NUM_38


#define LVGL_DISP_H_RES     240
#define LVGL_DISP_V_RES     135
#define LVGL_DISP_BUF_LINE  40

// Bit number used to represent command and parameter
#define LCD_SPI_CMD_BITS    8
#define LCD_SPI_PARAM_BITS  8

#define ESP_LVGL_TICK_PERIOD_MS     2
#define ESP_LVGL_TASK_MAX_DELAY_MS  500
#define ESP_LVGL_TASK_MIN_DELAY_MS  1
#define ESP_LVGL_TASK_STACK_SIZE    (4 * 1024)
#define ESP_LVGL_TASK_PRIORITY      2

#if 0
static void esp_lvgl_disp_driver_update_cb(lv_disp_drv_t *drv)
#endif
static void esp_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static bool esp_lvgl_flush_done_cb(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);
static void esp_lvgl_tick_timer_cb(void *arg);
static void esp_lvgl_task_handler(void *arg);
static void esp_lvgl_key_read_cb(lv_indev_drv_t* indev_driver, lv_indev_data_t* data);


static SemaphoreHandle_t lvgl_mux = NULL;

#if 0
/* Rotate display and touch, when rotated screen in LVGL. Called when driver parameters are updated. */
static void esp_lvgl_disp_driver_update_cb(lv_disp_drv_t *drv)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    ESP_LOGI(TAG, "Display rotated: %d.", drv->rotated);
    switch (drv->rotated) 
    {
    case LV_DISP_ROT_90:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, true);
        esp_lcd_panel_mirror(panel_handle, true, true);
        break;
    case LV_DISP_ROT_180:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, false);
        esp_lcd_panel_mirror(panel_handle, false, true);
        break;
    case LV_DISP_ROT_270:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, true);
        esp_lcd_panel_mirror(panel_handle, false, false);
        break;
//  case LV_DISP_ROT_NONE:
    default: 
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, false);
        esp_lcd_panel_mirror(panel_handle, true, false);
        break;
    }
}
#endif

static void esp_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

static bool esp_lvgl_flush_done_cb(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}

static void esp_lvgl_tick_timer_cb(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(ESP_LVGL_TICK_PERIOD_MS);
}

static void esp_lvgl_task_handler(void *arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t task_delay_ms = ESP_LVGL_TASK_MAX_DELAY_MS;
    while (1)
    {
        // Lock the mutex due to the LVGL APIs are not thread-safe
        if (esp_lvgl_lock(-1)) {
            task_delay_ms = lv_timer_handler();
            // Release the mutex
            esp_lvgl_unlock();
        }
        if (task_delay_ms > ESP_LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = ESP_LVGL_TASK_MAX_DELAY_MS;
        } else if (task_delay_ms < ESP_LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = ESP_LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

void esp_lvgl_adapter_init(void)
{
    static lv_disp_draw_buf_t   disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t        disp_drv;      // contains callback functions
    static lv_indev_drv_t       keypad_drv;

    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_reset_pin(LVLCD_PIN_BL);
    gpio_set_direction(LVLCD_PIN_BL, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(LVLCD_PIN_BL, GPIO_PULLUP_ONLY);
    gpio_set_level(LVLCD_PIN_BL, LCD_BL_OFF_LEVEL);

    ESP_LOGI(TAG, "Turn off LCD backlight");
    esp_keypad_init();

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = LCD_PIN_CLK,
        .mosi_io_num = LCD_PIN_DAT,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LVGL_DISP_H_RES * LVGL_DISP_BUF_LINE * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LVLCD_PIN_DC,
        .cs_gpio_num = LVLCD_PIN_CS,
        .pclk_hz = LCD_SPI_CLK_FREQ,
        .lcd_cmd_bits = LCD_SPI_CMD_BITS,
        .lcd_param_bits = LCD_SPI_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = esp_lvgl_flush_done_cb,
        .user_ctx = &disp_drv,
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_HOST, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LVLCD_PIN_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

    ESP_LOGI(TAG, "Install ST7789 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, true));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 40, 53));

    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(LVLCD_PIN_BL, LCD_BL_ON_LEVEL);

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    // alloc draw buffers used by LVGL
    // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    lv_color_t *buf1 = (lv_color_t*)heap_caps_malloc(LVGL_DISP_H_RES * LVGL_DISP_BUF_LINE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    lv_color_t *buf2 = (lv_color_t*)heap_caps_malloc(LVGL_DISP_H_RES * LVGL_DISP_BUF_LINE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LVGL_DISP_H_RES * LVGL_DISP_BUF_LINE);
    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LVGL_DISP_H_RES;
    disp_drv.ver_res = LVGL_DISP_V_RES;
    disp_drv.flush_cb = esp_lvgl_flush_cb;
#if 0
    disp_drv.drv_update_cb = esp_lvgl_disp_driver_update_cb;
#endif
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    // lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
    (void)lv_disp_drv_register(&disp_drv);

    // Initialize LVGL keypad driver.
    lv_indev_drv_init(&keypad_drv);
    keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    keypad_drv.long_press_time = 2000;
    keypad_drv.read_cb = esp_lvgl_key_read_cb;
    lv_group_set_default(lv_group_create());
    lv_indev_set_group(lv_indev_drv_register(&keypad_drv), lv_group_get_default());
    ESP_LOGI(TAG, "Keypad driver initialize done.");

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &esp_lvgl_tick_timer_cb,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, ESP_LVGL_TICK_PERIOD_MS * 1000));

    lvgl_mux = xSemaphoreCreateRecursiveMutex();
    assert(lvgl_mux);

    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreate(esp_lvgl_task_handler, "LVGL", ESP_LVGL_TASK_STACK_SIZE, NULL, ESP_LVGL_TASK_PRIORITY, NULL);
}

bool esp_lvgl_lock(int timeout_ms)
{
    // Convert timeout in milliseconds to FreeRTOS ticks
    // If `timeout_ms` is set to -1, the program will block until the condition is met
    const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(lvgl_mux, timeout_ticks) == pdTRUE;
}

void esp_lvgl_unlock(void)
{
    xSemaphoreGiveRecursive(lvgl_mux);
}

void esp_lvgl_key_read_cb(lv_indev_drv_t* indev_driver, lv_indev_data_t* data)
{

}