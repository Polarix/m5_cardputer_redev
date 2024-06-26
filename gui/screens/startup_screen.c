#include "startup_screen.h"
#include "screens.h"
#include "esp_err.h"
#include "esp_log.h"

static void startup_timer_callback(lv_timer_t* timer);
static void on_screen_unloaded(lv_event_t* event);

LV_IMG_DECLARE(LOGO);

static lv_obj_t* s_screen_handle = NULL;
static const char* TAG = {"screen:startup"};

void startup_screen_create(void)
{
    if(!s_screen_handle)
    {
        s_screen_handle = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(s_screen_handle, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(s_screen_handle, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_add_event_cb(s_screen_handle, on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);

        lv_obj_t* logo_img = lv_img_create(s_screen_handle);
        lv_obj_set_size(logo_img, 240, 35);
        lv_img_set_src(logo_img, &LOGO);
        lv_obj_center(logo_img);
    }
}

void startup_screen_load(void)
{
    if(s_screen_handle)
    {
        lv_scr_load(s_screen_handle);
        lv_timer_create(startup_timer_callback, 2000, NULL);
    }
}

static void startup_timer_callback(lv_timer_t* timer)
{
    if(timer)
    {
        lv_timer_del(timer);
        main_screen_create();
        main_screen_load();
    }
}

static void on_screen_unloaded(lv_event_t* event)
{
    if(s_screen_handle)
    {
        lv_obj_del(s_screen_handle);
        s_screen_handle = NULL;
    }
}
