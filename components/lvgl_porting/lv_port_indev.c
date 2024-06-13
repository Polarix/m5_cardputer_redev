/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "lvgl.h"
// #include "touch_driver.h"
#include "driver/gpio.h"
#include "esp_log.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum _e_key_id_
{
    lv_indrv_key_id_none = 0,
    lv_indrv_key_id_up,
    lv_indrv_key_id_down,
    lv_indrv_key_id_left,
    lv_indrv_key_id_right,
    lv_indrv_key_id_ok,
}lv_indev_key_id_t;

typedef struct _st_key_io_rec_
{
    lv_indev_key_id_t   key_id;
    gpio_num_t          io_num;
}lv_indev_esp_key_rec;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

static void mouse_init(void);
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static lv_indev_key_id_t keypad_get_key(void);

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void encoder_handler(void);

static void button_init(void);
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;
lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
lv_indev_t * indev_encoder;
lv_indev_t * indev_button;

static int32_t encoder_diff;
static lv_indev_state_t encoder_state;



static lv_indev_esp_key_rec s_key_rec[] = 
{
    {lv_indrv_key_id_up,    GPIO_NUM_8},
    {lv_indrv_key_id_down,  GPIO_NUM_10},
    {lv_indrv_key_id_left,  GPIO_NUM_0},
    {lv_indrv_key_id_right, GPIO_NUM_12},
    {lv_indrv_key_id_ok,    GPIO_NUM_11},
};

static const char* TAG = {"lv_indev_drv"};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
#if 0
    /*------------------
     * Touchpad
     * -----------------*/
    static lv_indev_drv_t indev_drv_touchpad;
    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv_touchpad);
    indev_drv_touchpad.type = LV_INDEV_TYPE_POINTER;
    indev_drv_touchpad.read_cb = touch_driver_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv_touchpad);
#endif
#if 0
    /*------------------
     * Mouse
     * -----------------*/
    static lv_indev_drv_t indev_drv_mouse;
    /*Initialize your mouse if you have*/
    mouse_init();

    /*Register a mouse input device*/
    lv_indev_drv_init(&indev_drv_mouse);
    indev_drv_mouse.type = LV_INDEV_TYPE_POINTER;
    indev_drv_mouse.read_cb = mouse_read;
    indev_mouse = lv_indev_drv_register(&indev_drv_mouse);

    /*Set cursor. For simplicity set a HOME symbol now.*/
    lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
    lv_img_set_src(mouse_cursor, LV_SYMBOL_HOME);
    lv_indev_set_cursor(indev_mouse, mouse_cursor);
#endif
#if 0
    /*------------------
     * Keypad
     * -----------------*/
    static lv_indev_drv_t indev_drv_keypad;
    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    lv_indev_drv_init(&indev_drv_keypad);
    indev_drv_keypad.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_keypad.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&indev_drv_keypad);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_keypad, group);`*/
#endif
    static lv_indev_drv_t indev_key_drv;
    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    lv_indev_drv_init(&indev_key_drv);
    indev_key_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_key_drv.long_press_time = 500;
    indev_key_drv.long_press_repeat_time = 100;
    indev_key_drv.read_cb = keypad_read;

    indev_keypad = lv_indev_drv_register(&indev_key_drv);

    lv_group_t* default_group = lv_group_create();
    if(default_group)
    {
        lv_group_set_default(default_group);
        lv_indev_set_group(indev_keypad, default_group);
    }
#if 0
    /*------------------
     * Encoder
     * -----------------*/
    static lv_indev_drv_t indev_drv_encoder;
    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv_encoder);
    indev_drv_encoder.type = LV_INDEV_TYPE_ENCODER;
    indev_drv_encoder.read_cb = encoder_read;
    indev_encoder = lv_indev_drv_register(&indev_drv_encoder);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_encoder, group);`*/
#endif
#if 0
    /*------------------
     * Button
     * -----------------*/
    static lv_indev_drv_t indev_drv_button;
    /*Initialize your button if you have*/
    button_init();

    /*Register a button input device*/
    lv_indev_drv_init(&indev_drv_button);
    indev_drv_button.type = LV_INDEV_TYPE_BUTTON;
    indev_drv_button.read_cb = button_read;
    indev_button = lv_indev_drv_register(&indev_drv_button);

    /*Assign buttons to points on the screen*/
    static const lv_point_t btn_points[2] = {
        {10, 10},   /*Button 0 -> x:10; y:10*/
        {40, 100},  /*Button 1 -> x:40; y:100*/
    };
    lv_indev_set_button_points(indev_button, btn_points);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    // touch_driver_init();
    /*Your code comes here*/
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /*Save the pressed coordinates and the state*/
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;
}

/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    /*Your code comes here*/

    return false;
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}

/*------------------
 * Mouse
 * -----------------*/

/*Initialize your mouse*/
static void mouse_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the mouse*/
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the mouse button is pressed or released*/
    if(mouse_is_pressed()) {
        data->state = LV_INDEV_STATE_PR;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }
}

/*Return true is the mouse button is pressed*/
static bool mouse_is_pressed(void)
{
    /*Your code comes here*/

    return false;
}

/*Get the x and y coordinates if the mouse is pressed*/
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}

/*------------------
 * Keypad
 * -----------------*/

/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/

    // gpio_config_t gpio_cfg = {.intr_type = GPIO_INTR_DISABLE, .mode = GPIO_MODE_DEF_INPUT, .pull_up_en = GPIO_PULLUP_ENABLE, .pull_up_en = GPIO_PULLDOWN_DISABLE};
    // gpio_cfg.pin_bit_mask
    for(uint16_t i=0; i<(sizeof(s_key_rec)/sizeof(lv_indev_esp_key_rec)); ++i)
    {
        gpio_reset_pin(s_key_rec[i].io_num);
        gpio_set_direction(s_key_rec[i].io_num, GPIO_MODE_INPUT);
        gpio_pulldown_dis(s_key_rec[i].io_num);
        gpio_pullup_en(s_key_rec[i].io_num);
    }
}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;
    if(LV_INDEV_TYPE_KEYPAD == indev_drv->type)
    {
        /*Get whether the a key is pressed and save the pressed key*/
        uint32_t act_key = 0;
        switch(keypad_get_key())
        {
            case lv_indrv_key_id_up:
                act_key = LV_KEY_UP;
                data->state = LV_INDEV_STATE_PRESSED;
                break;
            case lv_indrv_key_id_down:
                act_key = LV_KEY_DOWN;
                data->state = LV_INDEV_STATE_PRESSED;
                break;
            case lv_indrv_key_id_left:
                act_key = LV_KEY_PREV; // LV_KEY_LEFT;
                data->state = LV_INDEV_STATE_PRESSED;
                break;
            case lv_indrv_key_id_right:
                act_key = LV_KEY_NEXT; // LV_KEY_RIGHT;
                data->state = LV_INDEV_STATE_PRESSED;
                break;
            case lv_indrv_key_id_ok:
                act_key = LV_KEY_ENTER;
                data->state = LV_INDEV_STATE_PRESSED;
                break;
            default:
            {
                // Donothing.
            }
        }
        last_key = act_key;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
    data->key = last_key;
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static lv_indev_key_id_t keypad_get_key(void)
{
    /*Your code comes here*/
    lv_indev_key_id_t key_id = lv_indrv_key_id_none;

    for(uint16_t i=0; i<(sizeof(s_key_rec)/sizeof(lv_indev_esp_key_rec)); ++i)
    {
        if(0 == gpio_get_level(s_key_rec[i].io_num))
        {
            // ESP_LOGW(TAG, "IO %d.", s_key_rec[i].io_num);
            key_id = s_key_rec[i].key_id;
            break;
        }
    }
    return key_id;
}

/*------------------
 * Encoder
 * -----------------*/

/*Initialize your keypad*/
static void encoder_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    data->enc_diff = encoder_diff;
    data->state = encoder_state;
}

/*Call this function in an interrupt to process encoder events (turn, press)*/
static void encoder_handler(void)
{
    /*Your code comes here*/

    encoder_diff += 0;
    encoder_state = LV_INDEV_STATE_REL;
}

/*------------------
 * Button
 * -----------------*/

/*Initialize your buttons*/
static void button_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if(btn_act >= 0) {
        data->state = LV_INDEV_STATE_PR;
        last_btn = btn_act;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
    for(i = 0; i < 2; i++) {
        /*Return the pressed button's ID*/
        if(button_is_pressed(i)) {
            return i;
        }
    }

    /*No button pressed*/
    return -1;
}

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

    /*Your code comes here*/

    return false;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
