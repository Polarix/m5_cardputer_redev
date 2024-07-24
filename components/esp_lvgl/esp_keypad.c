#include "esp_keypad.h"
#include "keypad.h"
#include "freertos/ringbuf.h"
#include <esp_err.h>
#include <esp_log.h>

static void esp_keypad_convert(int keycode, key_evt_t event, lv_indev_data_t* data);
static uint32_t esp_keypad_get_keychar(int keycode, bool caps, bool shift, bool fn);

static const lv_key_map_t s_lv_key_map[] = 
{
              /* For capslock off, lower.   */  /* For capslock on, upper.    */  /* Fn on.  */
    {/*  0 */ {KEY_CHAR_NULL,  KEY_CHAR_NULL }, {KEY_CHAR_NULL,  KEY_CHAR_NULL }, KEY_CHAR_NULL},
    /* Row 1 */
    {/*  1 */ {'`',            '~'           }, {'`',            '~'           }, KEY_CHAR_ESC},
    {/*  2 */ {'1',            '!'           }, {'1',            '!'           }, KEY_CHAR_NULL},
    {/*  3 */ {'2',            '@'           }, {'2',            '@'           }, KEY_CHAR_NULL},
    {/*  4 */ {'3',            '#'           }, {'3',            '#'           }, KEY_CHAR_NULL},
    {/*  5 */ {'4',            '$'           }, {'4',            '$'           }, KEY_CHAR_NULL},
    {/*  6 */ {'5',            '%'           }, {'5',            '%'           }, KEY_CHAR_NULL},
    {/*  7 */ {'6',            '^'           }, {'6',            '^'           }, KEY_CHAR_NULL},
    {/*  8 */ {'7',            '&'           }, {'7',            '&'           }, KEY_CHAR_NULL},
    {/*  9 */ {'8',            '*'           }, {'8',            '*'           }, KEY_CHAR_NULL},
    {/* 10 */ {'9',            '('           }, {'9',            '('           }, KEY_CHAR_NULL},
    {/* 11 */ {'0',            ')'           }, {'-',            '_'           }, KEY_CHAR_NULL},
    {/* 12 */ {'-',            '_'           }, {'-',            '_'           }, KEY_CHAR_NULL},
    {/* 13 */ {'=',            '+'           }, {'=',            '+'           }, KEY_CHAR_NULL},
    {/* 14 */ {KEY_CHAR_BS,    KEY_CHAR_BS   }, {KEY_CHAR_BS,    KEY_CHAR_BS   }, KEY_CHAR_DEL},
    /* Row 2 */
    {/* 15 */ {KEY_CHAR_TAB,   KEY_CHAR_TAB  }, {KEY_CHAR_TAB,   KEY_CHAR_TAB  }, KEY_CHAR_TAB},
    {/* 16 */ {'q',            'Q'           }, {'Q',            'q'           }, KEY_CHAR_NULL},
    {/* 17 */ {'w',            'W'           }, {'W',            'w'           }, KEY_CHAR_NULL},
    {/* 18 */ {'e',            'E'           }, {'E',            'e'           }, KEY_CHAR_NULL},
    {/* 19 */ {'r',            'R'           }, {'R',            'r'           }, KEY_CHAR_NULL},
    {/* 20 */ {'t',            'T'           }, {'T',            't'           }, KEY_CHAR_NULL},
    {/* 21 */ {'y',            'Y'           }, {'Y',            'y'           }, KEY_CHAR_NULL},
    {/* 22 */ {'u',            'U'           }, {'U',            'u'           }, KEY_CHAR_NULL},
    {/* 23 */ {'i',            'I'           }, {'I',            'i'           }, KEY_CHAR_NULL},
    {/* 24 */ {'o',            'O'           }, {'O',            'o'           }, KEY_CHAR_NULL},
    {/* 25 */ {'p',            'P'           }, {'P',            'p'           }, KEY_CHAR_NULL},
    {/* 26 */ {'[',            '{'           }, {'[',            '{'           }, KEY_CHAR_NULL},
    {/* 27 */ {']',            '}'           }, {']',            '}'           }, KEY_CHAR_NULL},
    {/* 28 */ {'\\',           '|'           }, {'\\',           '|'           }, KEY_CHAR_NULL},
    /* Row 3 */
    {/* 29 */ {KEY_CHAR_FN,    KEY_CHAR_FN   }, {KEY_CHAR_FN,    KEY_CHAR_FN   }, KEY_CHAR_NULL},
    {/* 30 */ {KEY_CHAR_CAPS,  KEY_CHAR_CAPS }, {KEY_CHAR_CAPS,  KEY_CHAR_CAPS }, KEY_CHAR_NULL},
    {/* 31 */ {'a',            'A'           }, {'A',            'a'           }, KEY_CHAR_NULL},
    {/* 32 */ {'s',            'S'           }, {'S',            's'           }, KEY_CHAR_NULL},
    {/* 33 */ {'d',            'D'           }, {'D',            'd'           }, KEY_CHAR_NULL},
    {/* 34 */ {'f',            'F'           }, {'F',            'f'           }, KEY_CHAR_NULL},
    {/* 35 */ {'g',            'G'           }, {'G',            'g'           }, KEY_CHAR_NULL},
    {/* 36 */ {'h',            'H'           }, {'H',            'h'           }, KEY_CHAR_NULL},
    {/* 37 */ {'j',            'J'           }, {'J',            'j'           }, KEY_CHAR_NULL},
    {/* 38 */ {'k',            'K'           }, {'K',            'k'           }, KEY_CHAR_NULL},
    {/* 39 */ {'l',            'L'           }, {'L',            'l'           }, KEY_CHAR_NULL},
    {/* 40 */ {';',            ':'           }, {';',            ':'           }, KEY_CHAR_UP},
    {/* 41 */ {'\'',           '"'           }, {'\'',           '"'           }, KEY_CHAR_NULL},
    {/* 42 */ {KEY_CHAR_ENTER, KEY_CHAR_ENTER}, {KEY_CHAR_ENTER, KEY_CHAR_ENTER}, KEY_CHAR_ENTER},
    /* Row 4 */
    {/* 43 */ {KEY_CHAR_CTRL,  KEY_CHAR_CTRL }, {KEY_CHAR_CTRL,  KEY_CHAR_CTRL }, KEY_CHAR_CTRL},
    {/* 44 */ {KEY_CHAR_OPT,   KEY_CHAR_OPT  }, {KEY_CHAR_OPT,   KEY_CHAR_OPT  }, KEY_CHAR_OPT},
    {/* 45 */ {KEY_CHAR_ALT,   KEY_CHAR_ALT  }, {KEY_CHAR_ALT,   KEY_CHAR_ALT  }, KEY_CHAR_ALT},
    {/* 46 */ {'z',            'Z'           }, {'Z',            'z'           }, KEY_CHAR_NULL},
    {/* 47 */ {'x',            'X'           }, {'X',            'x'           }, KEY_CHAR_NULL},
    {/* 48 */ {'c',            'C'           }, {'C',            'c'           }, KEY_CHAR_NULL},
    {/* 49 */ {'v',            'V'           }, {'V',            'v'           }, KEY_CHAR_NULL},
    {/* 50 */ {'b',            'B'           }, {'B',            'b'           }, KEY_CHAR_NULL},
    {/* 51 */ {'n',            'N'           }, {'N',            'n'           }, KEY_CHAR_NULL},
    {/* 52 */ {'m',            'M'           }, {'M',            'm'           }, KEY_CHAR_NULL},
    {/* 53 */ {',',            '<'           }, {',',            '<'           }, KEY_CHAR_LEFT},
    {/* 54 */ {'.',            '>'           }, {'.',            '>'           }, KEY_CHAR_DOWN},
    {/* 55 */ {'/',            '?'           }, {'/',            '?'           }, KEY_CHAR_RIGHT},
    {/* 56 */ {KEY_CHAR_SP,    KEY_CHAR_SP   }, {KEY_CHAR_SP,    KEY_CHAR_SP   }, KEY_CHAR_NULL}
};

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
    else
    {
        int keycode;
        key_evt_t event;
        if(keypad_queue_pop_event(&keycode, &event))
        {
            esp_keypad_convert(keycode, event, data);
            data->continue_reading = true;
        }
        else
        {
            data->continue_reading = false;
        }
    }
}

static void esp_keypad_convert(int keycode, key_evt_t event, lv_indev_data_t* data)
{
    // ESP_LOGW(TAG, "Key %d, %s.", keycode, event?"pressed":"released");
    data->key = esp_keypad_get_keychar(keycode, keypad_caps_on(), keypad_shift_on(), keypad_fn_on());
    data->state = (KEYPAD_EVT_PRESS == event)?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;
    // ESP_LOGW(TAG, "LVGL key %d, %s.", (int)(data->key), event?"pressed":"released");
}

static uint32_t esp_keypad_get_keychar(int keycode, bool caps, bool shift, bool fn)
{
    int keymap_index = keycode % 57;
    uint32_t key_char = KEY_CHAR_NULL;
    if(fn)
    {
        key_char = s_lv_key_map[keymap_index].fn_on;
    }
    else
    {
        if(caps)
        {
            if(shift)
            {
                key_char = s_lv_key_map[keymap_index].caps_on.shift_on;
            }
            else
            {
                key_char = s_lv_key_map[keymap_index].caps_on.shift_off;
            }
        }
        else
        {
            if(shift)
            {
                key_char = s_lv_key_map[keymap_index].caps_off.shift_on;
            }
            else
            {
                key_char = s_lv_key_map[keymap_index].caps_off.shift_off;
            }
        }
    }
    return key_char;
}