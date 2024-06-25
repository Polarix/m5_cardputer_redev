#include "keypad_event.h"
#include <string.h>

static keypad_event_queue_t s_keypad_event_queue;

bool keypad_queue_is_full(void)
{
    return ((s_keypad_event_queue.rear+1) % KEYPAD_EVENT_QUEUE_SIZE == s_keypad_event_queue.front);
}

bool keypad_queue_is_empty(void)
{
    return (s_keypad_event_queue.rear == s_keypad_event_queue.front);
}

void keypad_init_event_queue(void)
{
    memset(&s_keypad_event_queue, 0x00, sizeof(keypad_event_queue_t));
}

bool keypad_queue_push_event(int keycode, key_evt_t event)
{
    bool result;
    if(keypad_queue_is_full())
    {
        result = false;
    }
    else
    {
        result = true;
        s_keypad_event_queue.rear = (s_keypad_event_queue.rear + 1) % KEYPAD_EVENT_QUEUE_SIZE;
        s_keypad_event_queue.event[s_keypad_event_queue.rear].code = keycode;
        s_keypad_event_queue.event[s_keypad_event_queue.rear].event = event;        
    }
	return result;
}

bool keypad_queue_pop_event(int* keycode, key_evt_t* event)
{
    bool result;
    if(keypad_queue_is_empty())
    {
        result = false;
    }
    else
    {
        result = true;
        s_keypad_event_queue.front = (s_keypad_event_queue.front + 1) % KEYPAD_EVENT_QUEUE_SIZE;
        *keycode = s_keypad_event_queue.event[s_keypad_event_queue.front].code;
        *event = s_keypad_event_queue.event[s_keypad_event_queue.front].event;
        memset(&s_keypad_event_queue.event[s_keypad_event_queue.front], 0x00, sizeof(keypad_event_t));
    }
    return result;
}
