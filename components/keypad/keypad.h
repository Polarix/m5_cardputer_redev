#ifndef _INCLUDE_KEY_PAD_DRV_H_
#define _INCLUDE_KEY_PAD_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#define KEYPAD_REC_LEN_MAX      (6)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _st_key_coord_
{
    uint8_t     row;
    uint8_t     col;
}key_coord_t;

typedef enum _e_keypad_evt_
{
    KEYPAD_EVT_RELEASE,
    KEYPAD_EVT_PRESS,
}key_evt_t;

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
