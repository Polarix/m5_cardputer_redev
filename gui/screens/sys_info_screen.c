#include "sys_info_screen.h"
#include "screens.h"
#include "lvgl.h"
#include "esp_err.h"
#include "esp_log.h"

static void on_screen_unloaded(lv_event_t* event);
static void on_key_pressed(lv_event_t* event);
static void info_label_style_set(lv_obj_t* textbox);
static void update_sys_info(void);
static void update_info_timer_callback(lv_timer_t* timer);

LV_FONT_DECLARE(SOURCEHANSANS_18PX_M);

static lv_obj_t* s_screen_handle = NULL;
static lv_obj_t* s_mem_label = NULL;
static lv_obj_t* s_bat_volt_label = NULL;

static const char* TAG = {"screen:sys_info"};

void sys_info_screen_create(void)
{
    s_screen_handle = lv_obj_create(NULL);
    if(s_screen_handle)
    {
        lv_obj_add_event_cb(s_screen_handle, on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_add_event_cb(s_screen_handle, on_key_pressed, LV_EVENT_KEY, NULL);
        lv_obj_set_style_bg_opa(s_screen_handle, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(s_screen_handle, lv_color_black(), LV_STATE_DEFAULT);
        lv_group_add_obj(lv_group_get_default(), s_screen_handle);

        /* Memory info. */
        s_mem_label = lv_label_create(s_screen_handle);
        lv_obj_set_size(s_mem_label, 200, 25);
        lv_obj_align(s_mem_label, LV_ALIGN_TOP_MID, 0, 10);
        info_label_style_set(s_mem_label);

        /* Bat volt. */
        s_bat_volt_label = lv_label_create(s_screen_handle);
        lv_obj_set_size(s_bat_volt_label, 200, 25);
        lv_obj_align(s_bat_volt_label, LV_ALIGN_TOP_MID, 0, 45);
        info_label_style_set(s_bat_volt_label);

        update_sys_info();
    }
}

static void info_label_style_set(lv_obj_t* label)
{
    lv_obj_set_style_bg_opa(label, LV_OPA_0, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label, LV_OPA_100, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &SOURCEHANSANS_18PX_M, LV_STATE_DEFAULT);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
}

static void update_info_timer_callback(lv_timer_t* timer)
{
    if(s_screen_handle)
    {
        update_sys_info();
    }
    else
    {
        lv_timer_del(timer);
    }
}

void sys_info_screen_load(void)
{
    if(s_screen_handle)
    {
        // lv_scr_load(s_screen_handle);
        lv_scr_load_anim(s_screen_handle, LV_SCR_LOAD_ANIM_FADE_OUT, 500, 0, false);

        lv_timer_create(update_info_timer_callback, 5000, NULL);
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

static void on_key_pressed(lv_event_t* event)
{
    if(LV_EVENT_KEY == lv_event_get_code(event))
    {
        if(LV_KEY_ESC == lv_event_get_key(event))
        {
            main_screen_create();
            main_screen_load();
        }
    }
}

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "bat_volt.h"

static void update_sys_info(void)
{
    uint32_t flash_size;
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        ESP_LOGE(TAG, "Get flash size failed.");
        flash_size = 0;
    }
    uint32_t mem_size = esp_get_minimum_free_heap_size();

    lv_label_set_text_fmt(s_mem_label, "Flash: %luM bytes.\nRam: %lu bytes.", flash_size / 1024 / 1024, mem_size);

    uint32_t bat_volt_mv = (uint32_t)bat_volt_read_mv();

    lv_label_set_text_fmt(s_bat_volt_label, "Bat volt: %lu.%03luv.", bat_volt_mv/1000, bat_volt_mv%1000);
}