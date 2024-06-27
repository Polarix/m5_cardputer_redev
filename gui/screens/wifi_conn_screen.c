#include "wifi_conn_screen.h"
#include "screens.h"
#include "lvgl.h"
#include "esp_err.h"
#include "esp_log.h"

static void on_screen_unloaded(lv_event_t* event);
static void on_key_pressed(lv_event_t* event);

LV_FONT_DECLARE(UNIFONT_16PX);

static lv_obj_t* s_screen_handle = NULL;
static lv_obj_t* s_passwd_input = NULL;

static const char* TAG = {"screen:wifi_conn"};
static char s_wifi_ssid[64];

void wifi_conn_screen_create(const char* ssid)
{
    s_screen_handle = lv_obj_create(NULL);
    if(s_screen_handle)
    {
        strncpy(s_wifi_ssid, ssid, 63);
        ESP_LOGD(TAG, "Create WiFi connection screen.");
        lv_obj_add_event_cb(s_screen_handle, on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_set_style_bg_opa(s_screen_handle, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(s_screen_handle, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_add_event_cb(s_screen_handle, on_key_pressed, LV_EVENT_KEY, NULL);
        lv_group_add_obj(lv_group_get_default(), s_screen_handle);

        /* Top bar. */
        lv_obj_t* top_bar = lv_obj_create(s_screen_handle);
        lv_obj_set_size(top_bar, 240, 24);
        lv_obj_align(top_bar, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_set_style_bg_color(top_bar, lv_color_hex(0x0079DB), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(top_bar, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(top_bar, lv_color_hex(0x003763), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(top_bar, LV_GRAD_DIR_VER, LV_STATE_DEFAULT);

        /* Bottom bar. */
        lv_obj_t* bottom_bar = lv_obj_create(s_screen_handle);
        lv_obj_set_size(bottom_bar, 240, 24);
        lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_style_bg_color(bottom_bar, lv_color_hex(0x0079DB), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(bottom_bar, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(bottom_bar, lv_color_hex(0x003763), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(bottom_bar, LV_GRAD_DIR_VER, LV_STATE_DEFAULT);

        /* Notice label. */
        lv_obj_t* notice = lv_label_create(s_screen_handle);
        lv_obj_set_style_text_color(notice, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(notice, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(notice, &UNIFONT_16PX, LV_STATE_DEFAULT);
        lv_obj_set_size(notice, 200, LV_SIZE_CONTENT);
        lv_obj_align(notice, LV_ALIGN_TOP_MID, 0, 35);
        lv_label_set_text(notice, "Input password:");

        /* Password input box. */
        s_passwd_input = lv_textarea_create(s_screen_handle);
        lv_obj_set_size(s_passwd_input, 200, 24);
        lv_obj_align_to(s_passwd_input, notice, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        lv_obj_set_style_pad_all(s_passwd_input, 4, LV_STATE_DEFAULT);
        lv_obj_set_style_radius(s_passwd_input, 4, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(s_passwd_input, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(s_passwd_input, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(s_passwd_input, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(s_passwd_input, LV_OPA_90, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(s_passwd_input, lv_color_black(), LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(s_passwd_input, LV_OPA_0, LV_PART_CURSOR | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(s_passwd_input, LV_OPA_80, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(s_passwd_input, 1, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_set_style_border_side(s_passwd_input, LV_BORDER_SIDE_LEFT, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_set_style_anim_time(s_passwd_input, 500, LV_PART_CURSOR);
        lv_textarea_set_one_line(s_passwd_input, true);
        lv_obj_add_event_cb(s_passwd_input, on_key_pressed, LV_EVENT_KEY, NULL);
        lv_group_add_obj(lv_group_get_default(), s_passwd_input);

        lv_group_focus_obj(s_passwd_input);
    }
}

void wifi_conn_screen_load(void)
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
        uint32_t keycode = lv_event_get_key(event);
        if(LV_KEY_ENTER == keycode)
        {
            ESP_LOGD(TAG, "SSID: %s.", s_wifi_ssid);
            ESP_LOGD(TAG, "Password: %s.", lv_textarea_get_text(s_passwd_input));
        }
        else if(LV_KEY_ESC == keycode)
        {
#if 0
            main_screen_create();
            main_screen_load();
#else
            wifi_scan_screen_create();
            wifi_scan_screen_load();
#endif
        }
    }
}
