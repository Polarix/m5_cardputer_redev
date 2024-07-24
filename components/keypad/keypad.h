#ifndef _INCLUDE_KEY_PAD_DRV_H_
#define _INCLUDE_KEY_PAD_DRV_H_

#include <stdint.h>
#include <stdbool.h>
#include "keypad_event.h"

#define KEYPAD_REC_LEN_MAX      (6)

#define KEYPAD_CODE_FN_KEY      (29)
#define KEYPAD_CODE_SHIFT_KEY   (30)
#define KEYPAD_CODE_CTRL_KEY    (43)
#define KEYPAD_CODE_OPT_KEY     (44)
#define KEYPAD_CODE_ALT_KEY     (45)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _st_key_state_
{
    int         pressed_count;
    int         pressed_code[KEYPAD_REC_LEN_MAX];
}kaypad_press_state_t;

typedef struct _st_keypad_state_
{
    bool        shift;
    bool        fn;
    bool        caps;
    bool        ctrl;
    bool        opt;
    bool        alt;
}keypad_state_t;

void keypad_init(void);
void keypad_scan(void);
bool keypad_has_pressed(void);
void keypad_show_key_state(void);
void keypad_dump_key_rec(kaypad_press_state_t* dest);
bool keypad_shift_on(void);
bool keypad_fn_on(void);
bool keypad_caps_on(void);
bool keypad_ctrl_on(void);
bool keypad_opt_on(void);
bool keypad_alt_on(void);
void keypad_force_fn(bool set);

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_KEY_PAD_DRV_H_
