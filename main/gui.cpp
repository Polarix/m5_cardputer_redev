#include "gui.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "startup_screen.h"

#define LVGL_TICK_PERIOD_MS    10           // 定义LVGL tick的周期（毫秒）
#define LVGL_TASK_MAX_DELAY_MS 500          // 定义LVGL任务的最大延迟（毫秒）
#define LVGL_TASK_MIN_DELAY_MS 1            // 定义LVGL任务的最小延迟（毫秒）
#define LVGL_TASK_STACK_SIZE   (4 * 1024)   // 定义LVGL任务的堆栈大小（字节）
#define LVGL_TASK_PRIORITY     2            // 定义LVGL任务的优先级

static void lvgl_ticks_task(void *arg);
static void lvgl_increase_ticks(void *arg);

static SemaphoreHandle_t s_lvgl_mux = nullptr;
static const char* TAG = {"gui"};

void gui_init(void)
{
    s_lvgl_mux = xSemaphoreCreateRecursiveMutex();
    if (s_lvgl_mux == nullptr) {
        ESP_LOGE(TAG, "Create LVGL mutex failed!");
    }
    assert(s_lvgl_mux);
    ESP_LOGI(TAG, "Initialize LVGL.");
    lv_init();
    lv_port_disp_init();
    // lv_port_indev_init();
}

void gui_start(void)
{
    ESP_LOGI(TAG, "Create and load startup screen.");
    /* Start */
    startup_screen_create();
    startup_screen_load();

    xTaskCreatePinnedToCore(lvgl_ticks_task, "LVGL", LVGL_TASK_STACK_SIZE, nullptr, LVGL_TASK_PRIORITY, nullptr, 0);

    ESP_LOGI(TAG, "Create LVGL ticks timer.");
    const esp_timer_create_args_t lvgl_tick_timer_args = {
            .callback = &lvgl_increase_ticks, // 设置定时器回调函数为 increase_lvgl_tick
            .arg = nullptr, // 传递给回调函数的参数，如果不需要额外的参数，可以设置为 nullptr
            .dispatch_method = ESP_TIMER_TASK, // 选择调用回调函数的方式，ESP_TIMER_TASK 表示在任务中调用，ESP_TIMER_ISR 表示在中断中调用
            .name = "lvgl_tick", // 定时器的名称，用于在调试和日志中标识定时器
            .skip_unhandled_events = false // 对于周期性定时器，设置为 true 表示跳过未处理的事件，false 表示不跳过
    };

    esp_timer_handle_t lvgl_tick_timer = nullptr;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));
}

bool gui_lock(void)
{
    return xSemaphoreTakeRecursive(s_lvgl_mux, portMAX_DELAY) == pdTRUE;
}

void gui_unlock(void)
{
    xSemaphoreGiveRecursive(s_lvgl_mux);
}

static void lvgl_increase_ticks(void *arg)
{
    (void) arg;
    /* 告诉LVGL经过了多少毫秒 */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static void lvgl_ticks_task(void *arg)
{
    (void) arg;
    ESP_LOGI(TAG, "LVGL task start on CPU%u", xPortGetCoreID());

    uint32_t task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
    while (true)
    {
        gui_lock();
        task_delay_ms = lv_timer_handler();
        gui_unlock();// 释放互斥锁

        // 限制任务延迟时间在最大和最小延迟之间
        if (task_delay_ms > LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
        } else if (task_delay_ms < LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}