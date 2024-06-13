#include <rom/ets_sys.h>
#include "esp_err.h"
#include "esp_log.h"
#include "gui.h"
#include "sd_card.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = {"main"};

extern "C" [[maybe_unused]] void app_main(void)
{
    /* For init filesystem. */
    sdcard_mount();
    ESP_LOGI(TAG, "SD card inited.");
    // sdcard_show_root();
    /* For init GUI. */
    gui_init();
    ESP_LOGI(TAG, "GUI Inited.");
    gui_start();
    ESP_LOGI(TAG, "GUI Started.");
}
