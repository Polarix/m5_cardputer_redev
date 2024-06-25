#ifndef _INCLUDE_KEYPAD_QUEUE_H_
#define _INCLUDE_KEYPAD_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>
#include <sdkconfig.h>

#define KEYPAD_EVENT_QUEUE_SIZE     (6)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _e_keypad_evt_
{
    KEYPAD_EVT_RELEASE,
    KEYPAD_EVT_PRESS,
}key_evt_t;

typedef struct _st_keypad_evt_
{
    int                 code;
    key_evt_t           event;
}keypad_event_t;

typedef struct _st_keypad_evt_queue_
{
    keypad_event_t      event[KEYPAD_EVENT_QUEUE_SIZE];
    int                 front;
    int                 rear;
}keypad_event_queue_t;

void keypad_init_event_queue(void);
bool keypad_event_queue_lock(void);
void keypad_event_queue_unlock(void);
bool keypad_queue_is_full(void);
bool keypad_queue_is_empty(void);
bool keypad_queue_push_event(int keycode, key_evt_t event);
bool keypad_queue_pop_event(int* keycode, key_evt_t* event);

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_KEYPAD_QUEUE_H_
