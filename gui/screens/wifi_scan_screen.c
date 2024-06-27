#include "wifi_scan_screen.h"
#include "screens.h"
#include "lvgl.h"
#include "esp_err.h"
#include "esp_log.h"
#include "wifi_scan.h"

static void on_screen_unloaded(lv_event_t* event);
static void on_key_pressed(lv_event_t* event);
static void on_select_proc(lv_event_t* event);
static void on_refresh_timer(lv_timer_t* timer);

LV_FONT_DECLARE(UNIFONT_16PX);

static lv_obj_t* s_screen_handle = NULL;
static lv_obj_t* s_wifi_table = NULL;
static lv_obj_t* s_timeout_bar = NULL;

static const char* TAG = {"screen:wifi_scan"};

void wifi_scan_screen_create(void)
{
    s_screen_handle = lv_obj_create(NULL);
    if(s_screen_handle)
    {
        lv_obj_add_event_cb(s_screen_handle, on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_set_style_bg_opa(s_screen_handle, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(s_screen_handle, lv_color_black(), LV_STATE_DEFAULT);

        /* SSID list. */
        s_wifi_table = lv_table_create(s_screen_handle);
        // lv_obj_add_event_cb(s_wifi_table, on_selection_change, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_event_cb(s_wifi_table, on_select_proc, LV_EVENT_SHORT_CLICKED, NULL);
        lv_obj_add_event_cb(s_wifi_table, on_key_pressed, LV_EVENT_KEY, NULL);
        lv_obj_set_size(s_wifi_table, 230, 120);
        lv_obj_align(s_wifi_table, LV_ALIGN_TOP_MID, 0, 5);
        lv_table_set_col_cnt(s_wifi_table, 3);
        lv_table_set_col_width(s_wifi_table, 0, 150);
        lv_table_set_col_width(s_wifi_table, 1, 40);
        lv_table_set_col_width(s_wifi_table, 2, 40);
        lv_obj_set_scroll_dir(s_wifi_table, LV_DIR_VER);
        lv_obj_set_style_opa(s_wifi_table, LV_OPA_100, LV_STATE_DEFAULT);
        /* For border */
        lv_obj_set_style_border_side(s_wifi_table, LV_BORDER_SIDE_FULL, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(s_wifi_table, 1, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(s_wifi_table, lv_color_hex(0x0079DB), LV_STATE_DEFAULT);
        /* For items style */
        lv_obj_set_style_border_width(s_wifi_table, 0, LV_PART_ITEMS);
        lv_obj_set_style_border_color(s_wifi_table, lv_color_hex(0x7F7F7F), LV_PART_ITEMS);
        lv_obj_set_style_border_opa(s_wifi_table, LV_OPA_50, LV_PART_ITEMS);
        lv_obj_set_style_bg_color(s_wifi_table, lv_color_hex(0x080808), LV_PART_ITEMS | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(s_wifi_table, lv_color_hex(0x003763), LV_PART_ITEMS | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(s_wifi_table, LV_OPA_40, LV_PART_ITEMS);
        lv_obj_set_style_text_color(s_wifi_table, lv_color_hex(0xD0D0D0), LV_PART_ITEMS);
        lv_obj_set_style_text_opa(s_wifi_table, LV_OPA_100, LV_PART_ITEMS);
        lv_obj_set_style_text_font(s_wifi_table, &UNIFONT_16PX, LV_PART_ITEMS);
        /* For item pad */
        lv_obj_set_style_pad_hor(s_wifi_table, 5, LV_PART_ITEMS );
        lv_obj_set_style_pad_ver(s_wifi_table, 3, LV_PART_ITEMS );

        /* Timeout bar */
        s_timeout_bar = lv_bar_create(s_screen_handle);
        lv_obj_set_size(s_timeout_bar, 230, 5);
        lv_obj_align_to(s_timeout_bar, s_wifi_table, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        lv_bar_set_range(s_timeout_bar, 0, 30);
        lv_obj_set_style_border_opa(s_timeout_bar, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(s_timeout_bar, 1, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(s_timeout_bar, lv_color_hex(0x0079DB), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(s_timeout_bar, lv_color_hex(0x003763), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(s_timeout_bar, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(s_timeout_bar, 2, LV_STATE_DEFAULT);
        // lv_obj_set_style_bg_color(s_timeout_bar, lv_color_hex(0x0776D0), LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(s_timeout_bar, lv_color_white(), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(s_timeout_bar, LV_OPA_100, LV_PART_INDICATOR);
        lv_obj_set_style_anim_time(s_timeout_bar, 100, LV_STATE_DEFAULT);
        lv_bar_set_value(s_timeout_bar, 30, LV_ANIM_OFF);

        lv_group_add_obj(lv_group_get_default(), s_wifi_table);
        lv_group_focus_obj(s_wifi_table);
    }
}

void wifi_scan_screen_load(void)
{
    if(s_screen_handle)
    {
        wifi_scan_init();
        lv_scr_load_anim(s_screen_handle, LV_SCR_LOAD_ANIM_FADE_OUT, 100, 0, false);
        wifi_scan_init();
        wifi_scan_task_exec();
#if 0
        wifi_scan_screen_set_item(0, "SSID1", -10, 1);
        wifi_scan_screen_set_item(1, "SSID2", -10, 2);
        wifi_scan_screen_set_item(2, "SSID3", -10, 3);
        wifi_scan_screen_set_item(3, "SSID4", -10, 4);
        wifi_scan_screen_set_item(4, "SSID4", -10, 10);
        wifi_scan_screen_set_item(5, "SSID4", -10, 15);
        wifi_scan_screen_set_item(6, "SSID4", -10, 8);
        wifi_scan_screen_set_item(7, "SSIDPT", -10, 11);
        wifi_scan_screen_scan_done();
#endif
    }
}

static void on_screen_unloaded(lv_event_t* event)
{
    if(s_screen_handle)
    {
        lv_obj_del(s_screen_handle);
        s_screen_handle = NULL;
        s_wifi_table = NULL;
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

static void on_select_proc(lv_event_t* event)
{
    uint16_t col, row;
    lv_obj_t* table_obj = lv_event_get_target(event);
    lv_table_get_selected_cell(table_obj, &row, &col);
    ESP_LOGI(TAG, "Select %u confirmed.", row);
    wifi_conn_screen_create(lv_table_get_cell_value(table_obj, row, 0));
    wifi_conn_screen_load();
}

static void on_refresh_timer(lv_timer_t* timer)
{
    if(timer)
    {
        if(s_screen_handle)
        {
            int bar_val = lv_bar_get_value(s_timeout_bar);
            if(bar_val > 0)
            {
                --bar_val;
                lv_bar_set_value(s_timeout_bar, bar_val, LV_ANIM_OFF);
            }
            else
            {
                lv_timer_del(timer);
                /* refresh SSID. */
                wifi_scan_task_exec();
            }
        }
        else
        {
            lv_timer_del(timer);
        }
    }
}

void wifi_scan_screen_set_item_count(int count)
{
    if(s_wifi_table)
    {
        lv_table_set_row_cnt(s_wifi_table, count);
    }
}

void wifi_scan_screen_set_item(int index, const char* ssid, int rssi, int channel)
{
    if(s_wifi_table)
    {
        lv_table_set_cell_value(s_wifi_table, index, 0, ssid);
        lv_table_set_cell_value_fmt(s_wifi_table, index, 1, "%d", rssi);
        lv_table_set_cell_value_fmt(s_wifi_table, index, 2, "%d", channel);
    }
}

void wifi_scan_screen_scan_done(void)
{
    if(s_screen_handle)
    {
        ESP_LOGI(TAG, "Scan done.");
        lv_bar_set_value(s_timeout_bar, 30, LV_ANIM_ON);
        lv_timer_create(on_refresh_timer, 1000, NULL);
    }
}
