#ifndef _INCLUDE_KEY_PAD_DRV_H_
#define _INCLUDE_KEY_PAD_DRV_H_

#include <stdint.h>
#include <stdbool.h>
#include "keypad_event.h"

#define KEYPAD_REC_LEN_MAX      (6)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _st_key_state_
{
    int         pressed_count;
    int         pressed_code[KEYPAD_REC_LEN_MAX];
}keypad_state_t;

void keypad_init(void);
void keypad_scan(void);
bool keypad_has_pressed(void);
void keypad_show_key_state(void);
void keypad_dump_key_rec(keypad_state_t* dest);

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_KEY_PAD_DRV_H_
