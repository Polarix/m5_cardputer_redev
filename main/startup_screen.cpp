#include "startup_screen.h"
#include <esp_err.h>
#include <esp_log.h>

#include "wifi_list_screen.h"

static void startup_timer_callback(lv_timer_t* timer);
static void on_screen_unloaded(lv_event_t* event);

LV_IMG_DECLARE(logo);
LV_FONT_DECLARE(BASIC_ASCII);

static lv_obj_t* screen_handle = nullptr;
static lv_obj_t* demo_label = nullptr;
static const char* TAG = {"startup_scr"};

void startup_screen_create(void)
{
    if(!screen_handle)
    {
        screen_handle = lv_obj_create(nullptr);
        assert(screen_handle);
        lv_obj_set_style_bg_color(screen_handle, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_add_event_cb(screen_handle, on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, nullptr);
#if 0
        lv_obj_t* logo_img = lv_img_create(screen_handle);
        lv_img_set_src(logo_img, &logo);
        lv_obj_align(logo_img, LV_ALIGN_TOP_MID, 0, 5);
#else
        demo_label = lv_label_create(screen_handle);
        lv_obj_set_style_text_font(demo_label, &BASIC_ASCII, LV_STATE_DEFAULT);
        lv_label_set_text(demo_label, "Hello ReCardputer!");
        lv_obj_set_style_text_opa(demo_label, LV_OPA_100, LV_STATE_DEFAULT);
#endif
    }
}

void startup_screen_load(void)
{
    if(screen_handle)
    {
        assert(lv_timer_create(startup_timer_callback, 1000, nullptr));
        ESP_LOGW(TAG, "Start timer.");
        lv_scr_load(screen_handle);
    }
}

void destroy_startup_screen(void)
{
    if(screen_handle)
    {
        lv_obj_del(screen_handle);
        screen_handle = nullptr;
        demo_label = nullptr;
    }
}

static void startup_timer_callback(lv_timer_t* timer)
{
    if(timer)
    {
        lv_timer_del(timer);
        ESP_LOGW(TAG, "Timer triggered.");
        if(demo_label)
        {
            lv_obj_set_pos(demo_label, 5, 5);
        }
        // wifi_list_screen_create();
        // wifi_list_screen_load();
    }
}

static void on_screen_unloaded(lv_event_t* event)
{
    destroy_startup_screen();
}