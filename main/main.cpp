#include <rom/ets_sys.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lvgl.h"
#include "startup_screen.h"
#include "bat_volt.h"

static const char* TAG = {"main"};
void platform_init(void);

extern "C" [[maybe_unused]] void app_main(void)
{
    /* For init filesystem. */
    // sdcard_mount();
    // ESP_LOGI(TAG, "SD card inited.");
    // sdcard_show_root();
    /* For init GUI. */
    bat_volt_monitor_init();

    esp_lvgl_adapter_init();
    ESP_LOGI(TAG, "GUI Inited.");
    platform_init();
    while(true)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


void platform_init(void)
{
    startup_screen_create();
    startup_screen_load();
}