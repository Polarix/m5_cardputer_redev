#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "keypad.h"
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>

static void keypad_init_gpio(void);
static uint8_t keypad_write_row(uint8_t row);
#if 0
static uint8_t keypad_read_col(uint8_t col);
#endif
static void keypad_event_update(void);

static const gpio_num_t s_key_pad_col_pin[] = {GPIO_NUM_13, GPIO_NUM_15, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7};   /* input */
static const gpio_num_t s_key_pad_row_pin[] = {GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_11}; /* Use 74HC138 for scan 8 row with 3 gpios. */     /* ouput */
static keypad_state_t s_key_state_previous = {0x00, {0x00}};
static keypad_state_t s_key_state_last = {0x00, {0x00}};
static const char* TAG = {"keypad_drv"};

static void keypad_init_gpio(void)
{
    for(size_t col_idx=0; col_idx < (sizeof(s_key_pad_col_pin)/sizeof(s_key_pad_col_pin[0])); ++col_idx)
    {
        gpio_reset_pin(s_key_pad_col_pin[col_idx]);
        gpio_set_direction(s_key_pad_col_pin[col_idx], GPIO_MODE_INPUT);
        gpio_set_pull_mode(s_key_pad_col_pin[col_idx], GPIO_PULLUP_ONLY);
    }
    for(size_t row_idx=0; row_idx < (sizeof(s_key_pad_row_pin)/sizeof(s_key_pad_row_pin[0])); ++row_idx)
    {
        gpio_reset_pin(s_key_pad_row_pin[row_idx]);
        gpio_set_direction(s_key_pad_row_pin[row_idx], GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(s_key_pad_row_pin[row_idx], GPIO_PULLUP_PULLDOWN);
    }
}

static uint8_t keypad_write_row(uint8_t row)
{
    /* 0 for no select, 1-8 for each row, */
    gpio_set_level(s_key_pad_row_pin[0], (row & 0B00000001));
    gpio_set_level(s_key_pad_row_pin[1], (row & 0B00000010));
    gpio_set_level(s_key_pad_row_pin[2], (row & 0B00000100));
    return row % 8;
}
#if 0
static uint8_t keypad_read_col(uint8_t col)
{
    uint8_t col_idx = col % 7;
    uint8_t col_val = 0;

    /* Use 74HC183, 0 for no select, 1-8 for each row, */
    for(int row_idx=1; row_idx<=8; ++ row_idx)
    {
        keypad_write_row(row_idx);
        if(gpio_get_level(0 == s_key_pad_col_pin[col_idx]))
        {
            col_val |= 0x01;
        }
        col_val <<= 1;
    }
    return col_val;
}
#endif
void keypad_init(void)
{
    keypad_init_event_queue();
    keypad_init_gpio();
    keypad_write_row(0);
}

void keypad_scan(void)
{
    memcpy(&s_key_state_previous, &s_key_state_last, sizeof(keypad_state_t));
    memset(&s_key_state_last, 0x00, sizeof(keypad_state_t));
    for(int row_idx=0; row_idx<8; ++row_idx)
    {
        keypad_write_row(row_idx);
        for(int col_idx=0; col_idx<7; ++col_idx)
        {
            if(0 == gpio_get_level(s_key_pad_col_pin[col_idx]))
            {
                /* A key is pressed. */
                /* Convert to layout coordinate. */
                int row_coord = 3 - (row_idx % 4);
                int col_coord = col_idx * 2 + ((row_idx<4)?1:0);
                int key_index = row_coord * 14 + col_coord + 1;
                if(s_key_state_last.pressed_count < KEYPAD_REC_LEN_MAX)
                {
                    s_key_state_last.pressed_code[s_key_state_last.pressed_count] = key_index;
                    ++s_key_state_last.pressed_count;
                }
            }
        }
    }
    if(keypad_has_pressed())
    {
        // keypad_show_key_state();
    }
    keypad_event_update();
}

bool keypad_has_pressed(void)
{
    return (s_key_state_last.pressed_count > 0);
}

void keypad_show_key_state(void)
{
    ESP_LOGW(TAG, "Key recorder: %d, %d, %d, %d, %d, %d.", 
        s_key_state_last.pressed_code[0], s_key_state_last.pressed_code[1], s_key_state_last.pressed_code[2], s_key_state_last.pressed_code[3], s_key_state_last.pressed_code[4], s_key_state_last.pressed_code[5]);
}

void keypad_dump_key_rec(keypad_state_t* dest)
{
    if(dest)
    {
        memcpy(dest, &s_key_state_last, sizeof(keypad_state_t));
    }
}

static void keypad_event_update(void)
{
    /* Search for key release. */
    int release_chk_idx = 0;
    while(release_chk_idx < s_key_state_previous.pressed_count)
    {
        int comp_idx=0;
        while(comp_idx < s_key_state_last.pressed_count)
        {
            if(s_key_state_previous.pressed_code[release_chk_idx] == s_key_state_last.pressed_code[comp_idx])
            {
                /* Key code s_key_state_previous.pressed_code[release_chk_idx] is already pressed. */
                break;
            }
            else
            {
                ++comp_idx;
            }
        }
        if(comp_idx == s_key_state_last.pressed_count)
        {
            /* Key code s_key_state_previous.pressed_code[release_chk_idx] is not exist in last state, released. */
            /* Key released. */
            // ESP_LOGI(TAG, "key code %d, released.", s_key_state_previous.pressed_code[release_chk_idx]);
            keypad_queue_push_event(s_key_state_previous.pressed_code[release_chk_idx], KEYPAD_EVT_RELEASE);
        }
        ++release_chk_idx;
    }

    /* Search for key new pressed. */
    int pressed_chk_idx = 0;
    while(pressed_chk_idx < s_key_state_last.pressed_count)
    {
        int comp_idx=0;
        /* Loop for search key code in previous state. */
        while(comp_idx < s_key_state_previous.pressed_count)
        {
            if(s_key_state_last.pressed_code[pressed_chk_idx] == s_key_state_previous.pressed_code[comp_idx])
            {
                /* Key code s_key_state_last.pressed_code[pressed_chk_idx] is already pressed. */
                break;
            }
            else
            {
                ++comp_idx;
            }
        }
        if(comp_idx == s_key_state_previous.pressed_count)
        {
            /* Key code s_key_state_last.pressed_code[pressed_chk_idx] is not exist in previous state, new pressed. */
            /* Key pressed. */
            // ESP_LOGI(TAG, "key code %d, pressed.", s_key_state_last.pressed_code[pressed_chk_idx]);
            keypad_queue_push_event(s_key_state_last.pressed_code[pressed_chk_idx], KEYPAD_EVT_PRESS);
        }
        ++pressed_chk_idx;
    }
}
