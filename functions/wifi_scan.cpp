#include "wifi_scan.h"
#include <esp_wifi.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include "wifi_scan_screen.h"
#include "esp_lvgl.h"

#define WIFI_SCAN_TASK_STACK_SIZE   (4 * 1024)   // 定义WIFI扫描任务的堆栈大小（字节）
#define WIFI_SCAN_TASK_PRIORITY     2            // 定义WIFI扫描任务的优先级

static void wifi_scan_task(void *arg);
static void wifi_scan_cleanup_recorder(void);
static void wifi_scan_read_recorder(uint16_t ap_num);

static const char* TAG = {"wifi_scan"};
static struct
{
    uint16_t    count;
    wifi_ap_record_t* rec;
}s_wifi_scan_recorder = {0, NULL};

void wifi_scan_init(void)
{
    nvs_flash_init(); // Enable this line if WIFI NVS Flash is enabled.
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
}

void wifi_func_deinit(void)
{
    esp_wifi_stop();
    esp_wifi_deinit();
}

static void wifi_scan_task(void *arg)
{
    (void) arg;
    ESP_LOGI(TAG, "Wifi scan task start on CPU%u", xPortGetCoreID());
 
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    };
    esp_wifi_scan_start(&scan_config, true);
    esp_wifi_scan_stop();
 
    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    ESP_LOGI(TAG, "Found %d APs", ap_count);
    if(ap_count)
    {
        wifi_scan_read_recorder(ap_count);
        esp_lvgl_lock(-1);
        wifi_scan_screen_set_item_count(ap_count);
        for (uint16_t i = 0; i < ap_count; i++)
        {
            // ESP_LOGI(TAG, "SSID: %s RSSI: %d", s_wifi_scan_recorder.rec[i].ssid, s_wifi_scan_recorder.rec[i].rssi);
            wifi_scan_screen_set_item(i, (char*)s_wifi_scan_recorder.rec[i].ssid, s_wifi_scan_recorder.rec[i].rssi, s_wifi_scan_recorder.rec[i].primary);
        }
        wifi_scan_screen_scan_done();
        esp_lvgl_unlock();
    }
    vTaskDelete(NULL);
}

void wifi_scan_task_exec(void)
{
    xTaskCreatePinnedToCore(wifi_scan_task, "WIFI_SCAN", WIFI_SCAN_TASK_STACK_SIZE, nullptr, WIFI_SCAN_TASK_PRIORITY, nullptr, 0);
    ESP_LOGI(TAG, "Create wifi scan task...");
}

static void wifi_scan_cleanup_recorder(void)
{
    if(s_wifi_scan_recorder.count)
    {
        delete[] s_wifi_scan_recorder.rec;
        s_wifi_scan_recorder.count = 0;
    }
}

static void wifi_scan_read_recorder(uint16_t ap_num)
{
    if(ap_num)
    {
        wifi_scan_cleanup_recorder();
        s_wifi_scan_recorder.count = ap_num;
        s_wifi_scan_recorder.rec = new wifi_ap_record_t[ap_num];
        esp_wifi_scan_get_ap_records(&ap_num, s_wifi_scan_recorder.rec);
    }
}

void wifi_scan_show_info(uint16_t ap_idx)
{
    if(ap_idx < s_wifi_scan_recorder.count)
    {
        // wifi_info_screen_fill((char*)(s_wifi_scan_recorder.rec[ap_idx].ssid), s_wifi_scan_recorder.rec[ap_idx].rssi, s_wifi_scan_recorder.rec[ap_idx].primary);
    }
}