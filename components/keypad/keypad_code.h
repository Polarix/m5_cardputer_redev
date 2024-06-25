#ifndef _INCLUDE_KEYPAD_CODE_DEF_H_
#define _INCLUDE_KEYPAD_CODE_DEF_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_CTRL_CODE_NULL      (0x00)
#define KEY_CTRL_CODE_HOME      (0x02)
#define KEY_CTRL_CODE_END       (0x03)
#define KEY_CTRL_CODE_BACKSPACE (0x08)
#define KEY_CTRL_CODE_TAB       (0x09)
#define KEY_CTRL_CODE_LF        (0x0A)
#define KEY_CTRL_CODE_ENTER     (KEY_CTRL_CODE_LF)
#define KEY_CTRL_CODE_CR        (0x0D)
#define KEY_CTRL_CODE_SHIFT_ON  (0x0E)
#define KEY_CTRL_CODE_SHIFT_OFF (0x0F)
#define KEY_CTRL_CODE_UP        (0x11)
#define KEY_CTRL_CODE_DOWN      (0x12)
#define KEY_CTRL_CODE_LEFT      (0x13)
#define KEY_CTRL_CODE_RIGHT     (0x14)
#define KEY_CTRL_CODE_ESC       (0x1B)
#define KEY_CTRL_CODE_SPACE     (0x20)
#define KEY_CTRL_CODE_DEL       (0x7F)
#define KEY_CTRL_CODE_FN        (0xA0)
#define KEY_CTRL_CODE_CAPS      (0xA1)
#define KEY_CTRL_CODE_CTRL      (0xA2)
#define KEY_CTRL_CODE_ALT       (0xA3)
#define KEY_CTRL_CODE_OPT       (0xA4)

typedef struct _st_key_char_
{
    int         key_code;
    char        key_char;
    char        key_shift_char;
    char        key_caps_char;
    char        key_fn_char;
}key_char_t;

const key_char_t* keypad_get_char_map(int key_code);

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_KEYPAD_CODE_DEF_H_
