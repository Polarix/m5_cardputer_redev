#ifndef _INCLUDE_ESP_KEYPAD_H_
#define _INCLUDE_ESP_KEYPAD_H_

#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

#define KEY_CHAR_NULL       (0x00)
#define KEY_CHAR_HOME       (0x02)
#define KEY_CHAR_END        (0x03)
#define KEY_CHAR_BS         (LV_KEY_BACKSPACE)  /* 0x08 */
#define KEY_CHAR_TAB        (LV_KEY_NEXT)       /* 0x09 */
#define KEY_CHAR_LF         (LV_KEY_ENTER)      /* 0x0A */
#define KEY_CHAR_ENTER      (LV_KEY_ENTER)      /* 0x0A */
#define KEY_CHAR_CR         (0x0D)
#define KEY_CHAR_SHIFT_ON   (0x0E)
#define KEY_CHAR_SHIFT_OFF  (0x0F)
#define KEY_CHAR_UP         (LV_KEY_UP)         /* 0x11 */
#define KEY_CHAR_DOWN       (LV_KEY_DOWN)       /* 0x12 */
#define KEY_CHAR_LEFT       (LV_KEY_LEFT)       /* 0x14 */
#define KEY_CHAR_RIGHT      (LV_KEY_RIGHT)      /* 0x13 */
#define KEY_CHAR_ESC        (LV_KEY_ESC)        /* 0x1B */
#define KEY_CHAR_SP         (0x20)
#define KEY_CHAR_DEL        (LV_KEY_DEL)        /* 0x7F */
#define KEY_CHAR_FN         (0xA0)
#define KEY_CHAR_CAPS       (0xA1)
#define KEY_CHAR_CTRL       (0xA2)
#define KEY_CHAR_ALT        (0xA3)
#define KEY_CHAR_OPT        (0xA4)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _st_lv_key_char_
{
    char    shift_off;
    char    shift_on;
}lv_key_char_t;

typedef struct _st_lv_key_map_
{
    lv_key_char_t   caps_off;
    lv_key_char_t   caps_on;
    char            fn_on;
}lv_key_map_t;

void esp_keypad_init(void);
void esp_keypad_scan(lv_indev_drv_t* indev_driver, lv_indev_data_t* data);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_ESP_KEYPAD_H_ */