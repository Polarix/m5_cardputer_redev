#include "fw_info_screen.h"
#include "screens.h"
#include "lvgl.h"
#include "esp_err.h"
#include "esp_log.h"

static void on_screen_unloaded(lv_event_t* event);
static void on_key_pressed(lv_event_t* event);

LV_FONT_DECLARE(SOURCEHANSANS_18PX_M);
LV_IMG_DECLARE(LOGO)

static lv_obj_t* s_screen_handle = NULL;

void fw_info_screen_create(void)
{
    s_screen_handle = lv_obj_create(NULL);
    if(s_screen_handle)
    {
        lv_obj_add_event_cb(s_screen_handle, on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_add_event_cb(s_screen_handle, on_key_pressed, LV_EVENT_KEY, NULL);
        lv_obj_set_style_bg_opa(s_screen_handle, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(s_screen_handle, lv_color_black(), LV_STATE_DEFAULT);
        lv_group_add_obj(lv_group_get_default(), s_screen_handle);

        /* LOGO image. */
        lv_obj_t* logo_img = lv_img_create(s_screen_handle);
        lv_obj_set_size(logo_img, 240, 35);
        lv_img_set_src(logo_img, &LOGO);
        lv_obj_align(logo_img, LV_ALIGN_TOP_MID, 0, 0);

        /* Info text. */
        lv_obj_t* info_text = lv_label_create(s_screen_handle);
        lv_obj_set_size(info_text, 200, LV_SIZE_CONTENT);
        lv_obj_align(info_text, LV_ALIGN_TOP_MID, 0, 35);
        lv_obj_set_style_bg_opa(info_text, LV_OPA_0, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(info_text, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(info_text, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(info_text, &SOURCEHANSANS_18PX_M, LV_STATE_DEFAULT);
        lv_label_set_text(info_text, "ReCardputer\n powered by LVGL and ESP-IDF");
        lv_label_set_long_mode(info_text, LV_LABEL_LONG_WRAP);
    }
}

void fw_info_screen_load(void)
{
    if(s_screen_handle)
    {
        lv_scr_load_anim(s_screen_handle, LV_SCR_LOAD_ANIM_FADE_OUT, 100, 0, false);
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
