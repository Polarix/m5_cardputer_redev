#include "wifi_info_screen.h"
#include "wifi_list_screen.h"
#include <esp_err.h>
#include <esp_log.h>
#include <lvgl.h>
#include "wifi_scan.h"

static void wifi_info_screen_on_screen_unloaded(lv_event_t* event);
static void wifi_info_screen_on_key_press(lv_event_t* event);

static lv_obj_t* s_screen_handle = nullptr;
static const char* TAG = {"wifi_info_scr"};
static const char* s_btnmtx_map[] = {"back", "attack", NULL};

LV_FONT_DECLARE(UNIFONT_16PX);

void wifi_info_screen_create(void)
{
    if(!s_screen_handle)
    {
        s_screen_handle = lv_btn_create(nullptr);
        assert(s_screen_handle);
        lv_obj_t* info_label = lv_label_create(s_screen_handle);
        assert(info_label);
        lv_obj_set_pos(info_label, 0, 0);
        lv_obj_set_size(info_label, 160, LV_SIZE_CONTENT);

        lv_obj_set_style_bg_color(s_screen_handle, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(s_screen_handle, &UNIFONT_16PX, LV_STATE_DEFAULT);
        lv_obj_add_event_cb(s_screen_handle, wifi_info_screen_on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, nullptr);
        lv_obj_add_event_cb(s_screen_handle, wifi_info_screen_on_key_press, LV_EVENT_SHORT_CLICKED, nullptr);
    }
}

void wifi_info_screen_load(uint16_t ap_idx)
{
    if(s_screen_handle)
    {
        wifi_scan_show_info(ap_idx);
        lv_scr_load_anim(s_screen_handle, LV_SCR_LOAD_ANIM_FADE_IN, 100, 900, false);
        ESP_LOGI(TAG, "Wifi info screen load.");
    }
}

void wifi_info_screen_destroy(void)
{
    if(s_screen_handle)
    {
        ESP_LOGI(TAG, "Destroy screen.");
        lv_obj_del(s_screen_handle);
        s_screen_handle = nullptr;
    }
}

static void wifi_info_screen_on_screen_unloaded(lv_event_t* event)
{
    wifi_info_screen_destroy();
}

static void wifi_info_screen_on_key_press(lv_event_t* event)
{
    ESP_LOGI(TAG, "Key code: %lu.", lv_indev_get_key(lv_indev_get_act()));
    wifi_list_screen_create();
    wifi_list_screen_load();
}

void wifi_info_screen_fill(const char* ssid, int8_t rssi, uint8_t channel)
{
    if(s_screen_handle)
    {
        lv_obj_t* info_lab = lv_obj_get_child(s_screen_handle, 0);
        assert(info_lab);
        assert(lv_obj_get_class(info_lab) == &lv_label_class);
        lv_label_set_text_fmt(info_lab, "SSID: %s\nRSSI: %d\nCH: %u.", ssid, rssi, channel);
    }
}