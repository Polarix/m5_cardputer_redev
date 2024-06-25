#include "esp_keypad.h"
#include "keypad.h"

static keypad_state_t s_last_key_scan_rec = {0x00};

void esp_keypad_init(void)
{
    keypad_init();
}

void esp_keypad_scan(lv_indev_drv_t* indev_driver, lv_indev_data_t* data)
{

}
