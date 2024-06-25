#include "esp_keypad.h"
#include "keypad.h"
#include "freertos/ringbuf.h"
#include <esp_err.h>
#include <esp_log.h>

#define KEY_CODE_RING_BUF_LEN        (8)
#define KEY_CODE_BUFFER_TYPE        RINGBUF_TYPE_NOSPLIT

static void esp_keypad_convert(int keycode, key_evt_t event, lv_indev_data_t* data);

static const char* TAG = {"esp_keypad"};

void esp_keypad_init(void)
{
    keypad_init();
}

void esp_keypad_scan(lv_indev_drv_t* indev_driver, lv_indev_data_t* data)
{
    if(keypad_queue_is_empty())
    {
        keypad_scan();
    }
    if(false == keypad_queue_is_empty())
    {
        int keycode;
        key_evt_t event;
        keypad_queue_pop_event(&keycode, &event);
        esp_keypad_convert(keycode, event, data);
    }
    if(keypad_queue_is_empty())
    {
        data->continue_reading = false;
    }
    else
    {
        data->continue_reading = true;
    }
}

static void esp_keypad_convert(int keycode, key_evt_t event, lv_indev_data_t* data)
{
    ESP_LOGW(TAG, "Key %d, %s.", keycode, event?"pressed":"released");
}