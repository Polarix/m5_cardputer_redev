#include "bt_scan.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/task.h>
#include <nvs_flash.h>

#define BT_SCAN_TASK_STACK_SIZE (4 * 1024)   // 定义蓝牙扫描任务的堆栈大小（字节）
#define BT_SCAN_TASK_PRIORITY   2            // 定义蓝牙扫描任务的优先级

static void bt_scan_task(void *arg);
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

static const char *TAG = {"bt_scan"};

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};

uint8_t *adv_name;
uint8_t adv_name_len;
uint8_t *part_uuid;
uint8_t part_uuid_len;
uint8_t *cmpl_uuid;
uint8_t cmpl_uuid_len;
uint8_t *user_data;
uint8_t user_data_len;

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;

    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        //the unit of the duration is second, 0 means scan permanently
        uint32_t duration = 1;
        esp_ble_gap_start_scanning(duration);
        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if ((err = param->scan_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Scan start failed: %s", esp_err_to_name(err));
        }
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            ESP_LOGI(TAG, "----------Device Found----------");
            esp_log_buffer_hex("Device address:", scan_result->scan_rst.bda, ESP_BD_ADDR_LEN );
            ESP_LOGI(TAG, "searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
            esp_log_buffer_hex("searched Adv Data:", scan_result->scan_rst.ble_adv, scan_result->scan_rst.adv_data_len + scan_result->scan_rst.scan_rsp_len);
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv, ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            if(adv_name_len != 0){
                ESP_LOGI(TAG, "searched Device Name Len %d", adv_name_len);
                esp_log_buffer_char(TAG, (char *)adv_name, adv_name_len);
            }
            part_uuid = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv, ESP_BLE_AD_TYPE_16SRV_PART, &part_uuid_len);
            if(part_uuid_len != 0)
            {
                esp_log_buffer_hex("searched part uuid", part_uuid, part_uuid_len);
            } 
            cmpl_uuid = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv, ESP_BLE_AD_TYPE_16SRV_CMPL, &cmpl_uuid_len);
            if(cmpl_uuid_len != 0)
            {
                esp_log_buffer_hex("searched cmpl uuid", cmpl_uuid, cmpl_uuid_len);
            }
            user_data = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv, ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE, &user_data_len);
            if(user_data_len != 0)
            {
                esp_log_buffer_hex("searched user data:", user_data, user_data_len);
            }
            ESP_LOGI(TAG, "RSSI of packet:%d dbm", scan_result->scan_rst.rssi);
            break;
        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if ((err = param->scan_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(TAG, "Scan stop failed: %s", esp_err_to_name(err));
        }
        else {
            ESP_LOGI(TAG, "Stop scan successfully");
        }
        break;

    default:
        break;
    }
}

void ble_ibeacon_appRegister(void)
{
    esp_err_t status;

    ESP_LOGI(TAG, "register callback");

    //register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        ESP_LOGE(TAG, "gap register error: %s", esp_err_to_name(status));
        return;
    }

}

void ble_ibeacon_init(void)
{
    esp_bluedroid_init();
    esp_bluedroid_enable();
    ble_ibeacon_appRegister();
}

static void bt_scan_task(void *arg)
{
    (void) arg;
    ESP_LOGI(TAG, "Bluetooth scan task start on CPU%u", xPortGetCoreID());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bt_dev_set_device_name("Marauder Remix");

    ble_ibeacon_init();

    /* set scan parameters */
    esp_ble_gap_set_scan_params(&ble_scan_params);

    vTaskDelete(NULL);
}

void bt_scan_task_exec(void)
{
    xTaskCreatePinnedToCore(bt_scan_task, "bt_scan", BT_SCAN_TASK_STACK_SIZE, nullptr, BT_SCAN_TASK_PRIORITY, nullptr, 0);
    ESP_LOGI(TAG, "Create bluetooth scan task...");
}
