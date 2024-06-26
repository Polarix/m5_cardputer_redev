#include "wifi_conn_screen.h"
#include "screens.h"
#include "lvgl.h"
#include "esp_err.h"
#include "esp_log.h"

static void on_screen_unloaded(lv_event_t* event);
static void on_key_pressed(lv_event_t* event);
static void input_box_style_set(lv_obj_t* textbox);

LV_FONT_DECLARE(SOURCEHANSANS_18PX_M);

static lv_obj_t* s_screen_handle = NULL;
static lv_obj_t* s_ssid_input = NULL;
static lv_obj_t* s_passwd_input = NULL;

static const char* TAG = {"screen:wifi_conn"};

void wifi_conn_screen_create(void)
{
    s_screen_handle = lv_obj_create(NULL);
    if(s_screen_handle)
    {
        lv_obj_add_event_cb(s_screen_handle, on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_set_style_bg_opa(s_screen_handle, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(s_screen_handle, lv_color_black(), LV_STATE_DEFAULT);

        /* SSID input box. */
        s_ssid_input = lv_textarea_create(s_screen_handle);
        lv_obj_set_size(s_ssid_input, 200, 24);
        lv_obj_align(s_ssid_input, LV_ALIGN_TOP_MID, 0, 30);
        input_box_style_set(s_ssid_input);
        lv_obj_add_event_cb(s_ssid_input, on_key_pressed, LV_EVENT_KEY, NULL);
        lv_group_add_obj(lv_group_get_default(), s_ssid_input);

        /* Password input box. */
        s_passwd_input = lv_textarea_create(s_screen_handle);
        lv_obj_set_size(s_passwd_input, 200, 24);
        lv_obj_align(s_passwd_input, LV_ALIGN_TOP_MID, 0, 70);
        input_box_style_set(s_passwd_input);
        lv_obj_add_event_cb(s_passwd_input, on_key_pressed, LV_EVENT_KEY, NULL);
        lv_group_add_obj(lv_group_get_default(), s_passwd_input);

        lv_group_focus_obj(s_ssid_input);
    }
}

static void input_box_style_set(lv_obj_t* textbox)
{
    lv_obj_set_style_pad_all(textbox, 4, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(textbox, 4, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(textbox, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(textbox, LV_OPA_100, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(textbox, lv_color_black(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(textbox, LV_OPA_90, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(textbox, lv_color_black(), LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_set_style_border_opa(textbox, LV_OPA_0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(textbox, LV_OPA_80, LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(textbox, 1, LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_set_style_border_side(textbox, LV_BORDER_SIDE_LEFT, LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_set_style_anim_time(textbox, 500, LV_PART_CURSOR);
    lv_textarea_set_one_line(textbox, true);
}

void wifi_conn_screen_load(void)
{
    if(s_screen_handle)
    {
        // lv_scr_load(s_screen_handle);
        lv_scr_load_anim(s_screen_handle, LV_SCR_LOAD_ANIM_FADE_OUT, 500, 0, false);
        ESP_LOGD(TAG, "Demo screen loadup.");
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
        if(LV_KEY_ENTER == lv_event_get_key(event))
        {
            ESP_LOGW(TAG, "SSID: %s.", lv_textarea_get_text(s_ssid_input));
            ESP_LOGW(TAG, "Password: %s.", lv_textarea_get_text(s_passwd_input));
        }
        else if(LV_KEY_ESC == lv_event_get_key(event))
        {
            main_screen_create();
            main_screen_load();
        }
    }
}
