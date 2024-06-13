#include "wifi_list_screen.h"
#include "wifi_info_screen.h"
#include <esp_err.h>
#include <esp_log.h>
#include <lvgl.h>
#include "wifi_scan.h"

static void wifi_list_item_paint_proc(lv_event_t* event);
static void wifi_list_screen_create_new_table(void);
static void wifi_list_on_selection_change(lv_event_t* event);
static void wifi_list_on_select_proc(lv_event_t* event);
static void wifi_list_on_screen_unloaded(lv_event_t* event);
static void wifi_list_show_exec_cb(void* obj, int32_t val);
static void wifi_list_start_auto_refresh(void);
static void wifi_list_refresh_timer_cb(lv_timer_t* timer);

static lv_obj_t* s_screen_handle = nullptr;
static lv_timer_t* s_refresh_timer = nullptr;
static const char* TAG = {"wifi list screen"};

LV_FONT_DECLARE(UNIFONT_16PX);

void wifi_list_screen_create(void)
{
    if(!s_screen_handle)
    {
        s_screen_handle = lv_obj_create(nullptr);
        assert(s_screen_handle);
        lv_obj_set_style_bg_color(s_screen_handle, lv_color_black(), LV_STATE_DEFAULT);
        /* Creater spinner. */
        lv_obj_t* load_spinner = lv_spinner_create(s_screen_handle, 1000, 80);
        assert(load_spinner);
        lv_obj_set_style_arc_width(load_spinner, 5, LV_PART_MAIN);
        lv_obj_set_style_arc_width(load_spinner, 5, LV_PART_INDICATOR);
        lv_obj_set_style_pad_all(load_spinner, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(load_spinner, 0, LV_PART_INDICATOR);
        lv_obj_set_style_arc_rounded(load_spinner, true, LV_PART_INDICATOR);
        lv_obj_set_size(load_spinner, 50, 50);
        lv_obj_center(load_spinner);

        wifi_list_screen_create_new_table();
        lv_obj_add_event_cb(s_screen_handle, wifi_list_on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, nullptr);
        
    }
}

static void wifi_list_on_selection_change(lv_event_t* event)
{
    lv_obj_t* table_obj = lv_event_get_target(event);
    uint16_t col, row;
    lv_table_get_selected_cell(table_obj, &row, &col);
    // ESP_LOGI(TAG, "Item %u selected.", row);
}

static void wifi_list_on_select_proc(lv_event_t* event)
{
    uint16_t col, row;
    lv_obj_t* table_obj = lv_event_get_target(event);
    lv_table_get_selected_cell(table_obj, &row, &col);
    ESP_LOGI(TAG, "Select %u confirmed.", row);
    wifi_info_screen_create();
    wifi_info_screen_load(row);
}

static void wifi_list_on_screen_unloaded(lv_event_t* event)
{
    wifi_list_screen_destroy();
}

static void wifi_list_item_paint_proc(lv_event_t* event)
{
    lv_obj_t* table_obj = lv_event_get_target(event);
    lv_obj_draw_part_dsc_t* dsc = lv_event_get_draw_part_dsc(event);
    if(dsc->part == LV_PART_ITEMS)
    {
        lv_table_cell_ctrl_t cell_flag[] = {
            LV_TABLE_CELL_CTRL_CUSTOM_1, LV_TABLE_CELL_CTRL_CUSTOM_2, LV_TABLE_CELL_CTRL_CUSTOM_3, LV_TABLE_CELL_CTRL_CUSTOM_4,
        };
        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_color_hex(0x015183);
        rect_dsc.bg_opa = LV_OPA_100;

        lv_area_t draw_area;
        draw_area.x1 = dsc->draw_area->x2 - 40;
        draw_area.x2 = draw_area.x1 + 10;
        draw_area.y1 = dsc->draw_area->y1 + lv_area_get_height(dsc->draw_area) / 2 - 10;
        draw_area.y2 = draw_area.y1 + 20;

        for(uint16_t i=0; i<(sizeof(cell_flag)/sizeof(lv_table_cell_ctrl_t)); ++i)
        {
            lv_draw_rect(dsc->draw_ctx, &rect_dsc, &draw_area);
        }

        // T.B.D
    }
}

void wifi_list_screen_load(void)
{
    if(s_screen_handle)
    {
        lv_obj_t* table_obj = lv_obj_get_child(s_screen_handle, 1);
        assert(table_obj);
        assert(lv_obj_get_class(table_obj) == &lv_table_class);

        lv_scr_load_anim(s_screen_handle, LV_SCR_LOAD_ANIM_FADE_IN, 400, 0, false);
        wifi_scan_task_exec();
    }
}

void wifi_list_screen_destroy(void)
{
    if(s_screen_handle)
    {
        lv_obj_del(s_screen_handle);
        s_screen_handle = nullptr;
        lv_timer_del(s_refresh_timer);
        s_refresh_timer = nullptr;
    }
}

static void wifi_list_screen_create_new_table(void)
{
    if(s_screen_handle)
    {
        lv_obj_t* wifi_table = lv_table_create(s_screen_handle);
        lv_obj_set_size(wifi_table, CONFIG_LV_DISP_SCREEN_WIDTH, LV_SIZE_CONTENT);
        // lv_obj_add_event_cb(wifi_table, wifi_list_item_paint_proc, LV_EVENT_DRAW_PART_END, NULL);
        lv_obj_add_event_cb(wifi_table, wifi_list_on_selection_change, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_event_cb(wifi_table, wifi_list_on_select_proc, LV_EVENT_SHORT_CLICKED, NULL);
        lv_table_set_col_cnt(wifi_table, 3);
        lv_obj_set_size(wifi_table, CONFIG_LV_DISP_SCREEN_WIDTH, CONFIG_LV_DISP_SCREEN_HEIGHT);
        lv_obj_set_style_border_width(wifi_table, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(wifi_table, lv_color_hex(0x7F7F7F), LV_PART_ITEMS);
        lv_obj_set_style_pad_hor(wifi_table, 5, LV_PART_MAIN | LV_PART_ITEMS );
        lv_obj_set_style_pad_ver(wifi_table, 3, LV_PART_MAIN | LV_PART_ITEMS );
        lv_obj_set_style_bg_color(wifi_table, lv_color_hex(0x080808), LV_PART_MAIN );
        lv_obj_set_style_bg_color(wifi_table, lv_color_hex(0x080808), LV_PART_ITEMS);
        lv_obj_set_style_text_color(wifi_table, lv_color_hex(0xD0D0D0), LV_PART_ITEMS);
        lv_obj_set_scroll_dir(wifi_table, LV_DIR_VER);
        lv_table_set_col_width(wifi_table, 0, CONFIG_LV_DISP_SCREEN_WIDTH - 80);
        lv_table_set_col_width(wifi_table, 1, 40);
        lv_table_set_col_width(wifi_table, 1, 40);
        lv_obj_set_style_opa(wifi_table, LV_OPA_0, LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(wifi_table, &UNIFONT_16PX, LV_STATE_DEFAULT);
    }
}

void wifi_list_screen_clear(void)
{
    lv_obj_t* table_obj = lv_obj_get_child(s_screen_handle, 1);
    assert(table_obj);
    assert(lv_obj_get_class(table_obj) == &lv_table_class);
    // Delete old table object.
    lv_obj_del(table_obj);
    // Create a new table.
    wifi_list_screen_create_new_table();
}

void wifi_list_screen_set_item_count(uint16_t item_cnt)
{
    lv_obj_t* table_obj = lv_obj_get_child(s_screen_handle, 1);
    assert(table_obj);
    assert(lv_obj_get_class(table_obj) == &lv_table_class);

    lv_table_set_row_cnt(table_obj, item_cnt);
}

void wifi_list_screen_set_item(uint16_t item_idx, const char* ssid, int rssi, int channel)
{
    lv_obj_t* table_obj = lv_obj_get_child(s_screen_handle, 1);
    assert(table_obj);
    assert(lv_obj_get_class(table_obj) == &lv_table_class);

    lv_table_set_cell_value(table_obj, item_idx, 0, ssid);
    lv_table_set_cell_value_fmt(table_obj, item_idx, 1, "%d", rssi);
    lv_table_set_cell_value_fmt(table_obj, item_idx, 2, "%d", channel);
}

void wifi_list_screen_add_item(const char* ssid, int rssi)
{
    lv_obj_t* table_obj = lv_obj_get_child(s_screen_handle, 1);
    assert(table_obj);
    assert(lv_obj_get_class(table_obj) == &lv_table_class);

    int new_index = lv_table_get_row_cnt(table_obj);
    lv_table_set_cell_value(table_obj, new_index, 0, ssid);
    lv_table_set_cell_value_fmt(table_obj, new_index, 1, "%d", rssi);
}

void wifi_list_screen_show(void)
{
    lv_obj_t* table_obj = lv_obj_get_child(s_screen_handle, 1);
    assert(table_obj);
    assert(lv_obj_get_class(table_obj) == &lv_table_class);
    if(LV_OPA_0 == lv_obj_get_style_opa(table_obj, LV_STATE_DEFAULT))
    {
        // lv_obj_set_style_opa(table_obj, LV_OPA_0, LV_STATE_DEFAULT);
        lv_anim_t table_show_anim;
        lv_anim_init(&table_show_anim);
        lv_anim_set_exec_cb(&table_show_anim, wifi_list_show_exec_cb);
        lv_anim_set_values(&table_show_anim, LV_OPA_0, LV_OPA_100);
        lv_anim_set_time(&table_show_anim, 500);
        lv_anim_set_var(&table_show_anim, table_obj);
        lv_anim_start(&table_show_anim);
    }
    wifi_list_start_auto_refresh();
}

static void wifi_list_show_exec_cb(void* var, int32_t val)
{
    lv_obj_t* table_obj = (lv_obj_t*)(var);
    if(table_obj)
    {
        lv_obj_set_style_opa(table_obj, val, LV_STATE_DEFAULT);
    }
}

static void wifi_list_start_auto_refresh(void)
{
    s_refresh_timer = lv_timer_create(wifi_list_refresh_timer_cb, 30000, nullptr);
    assert(s_refresh_timer);
    s_refresh_timer->repeat_count = 1;
}

static void wifi_list_refresh_timer_cb(lv_timer_t* timer)
{
    wifi_scan_task_exec();
}