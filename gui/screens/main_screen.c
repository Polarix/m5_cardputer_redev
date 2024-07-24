/**
 * @file lv_pantum_demo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "main_screen.h"
#include "screens.h"
#include "lvgl.h"
#include "esp_err.h"
#include "esp_log.h"
#include "keypad.h"

static void on_screen_loaded(lv_event_t* event);
static void on_screen_unloaded(lv_event_t* event);
static void on_roller_mask_paint(lv_event_t* event);
static void on_roller_select(lv_event_t* event);
static void on_roller_confirm(lv_event_t* event);

LV_FONT_DECLARE(unifont_16px);
LV_IMG_DECLARE(SYS_MENU_ICON_INFO);
LV_IMG_DECLARE(SYS_MENU_ICON_FILES);
LV_IMG_DECLARE(SYS_MENU_ICON_WIFI);
LV_IMG_DECLARE(SYS_MENU_ICON_BT);
LV_IMG_DECLARE(SYS_MENU_ICON_USB);
LV_IMG_DECLARE(SYS_MENU_ICON_ABOUT);

LV_FONT_DECLARE(SOURCEHANSANS_18PX_M);
LV_FONT_DECLARE(SOURCEHANSANS_24PX_M);

static lv_obj_t* s_screen_handle = NULL;
static lv_obj_t* s_top_bar = NULL;
static lv_obj_t* s_bottom_bar = NULL;
static lv_obj_t* s_icon_image = NULL;
static lv_obj_t* s_sel_roller = NULL;

static const char* TAG = {"screen:main"};

static const lv_img_dsc_t* menu_icons[] = {
    &SYS_MENU_ICON_INFO,
    &SYS_MENU_ICON_FILES,
    &SYS_MENU_ICON_WIFI,
    &SYS_MENU_ICON_BT,
    &SYS_MENU_ICON_USB,
    &SYS_MENU_ICON_ABOUT
};

void main_screen_create(void)
{
    s_screen_handle = lv_obj_create(NULL);
    if(s_screen_handle)
    {
        lv_obj_add_event_cb(s_screen_handle, on_screen_loaded, LV_EVENT_SCREEN_LOADED, NULL);
        lv_obj_add_event_cb(s_screen_handle, on_screen_unloaded, LV_EVENT_SCREEN_UNLOADED, NULL);
        lv_obj_set_style_bg_opa(s_screen_handle, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(s_screen_handle, lv_color_black(), LV_STATE_DEFAULT);

        /* Top bar. */
        s_top_bar = lv_obj_create(s_screen_handle);
        lv_obj_set_size(s_top_bar, 240, 24);
        lv_obj_align(s_top_bar, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_set_style_bg_color(s_top_bar, lv_color_hex(0x0079DB), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(s_top_bar, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(s_top_bar, lv_color_hex(0x003763), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(s_top_bar, LV_GRAD_DIR_VER, LV_STATE_DEFAULT);


        /* Bottom bar. */
        s_bottom_bar = lv_obj_create(s_screen_handle);
        lv_obj_set_size(s_bottom_bar, 240, 24);
        lv_obj_align(s_bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_style_bg_color(s_bottom_bar, lv_color_hex(0x0079DB), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(s_bottom_bar, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(s_bottom_bar, lv_color_hex(0x003763), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(s_bottom_bar, LV_GRAD_DIR_VER, LV_STATE_DEFAULT);

        /* Icon image. */
        s_icon_image = lv_img_create(s_screen_handle);
        lv_img_set_src(s_icon_image, menu_icons[0]);
        lv_obj_set_style_img_opa(s_icon_image, LV_OPA_100, LV_STATE_DEFAULT);
        //lv_obj_set_style_radius(s_icon, 10, LV_STATE_DEFAULT);
        //lv_obj_set_style_border_color(s_icon, lv_color_hex(0x72C4FF), LV_STATE_DEFAULT);
        //lv_obj_set_style_border_opa(s_icon, LV_OPA_100, LV_STATE_DEFAULT);
        //lv_obj_set_style_border_side(s_icon, LV_BORDER_SIDE_FULL, LV_STATE_DEFAULT);
        //lv_obj_set_style_border_width(s_icon, 1, LV_STATE_DEFAULT);
        lv_obj_set_size(s_screen_handle, 64, 64);
        lv_obj_align(s_icon_image, LV_ALIGN_LEFT_MID, 30, 0);

        /* Roller */
        s_sel_roller = lv_roller_create(s_screen_handle);
        lv_obj_set_size(s_sel_roller, 130, 70);
        lv_obj_set_style_text_font(s_sel_roller, &SOURCEHANSANS_18PX_M, LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(s_sel_roller, &SOURCEHANSANS_24PX_M, LV_PART_SELECTED);
        lv_obj_set_style_text_opa(s_sel_roller, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(s_sel_roller, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(s_sel_roller, 5, LV_STATE_DEFAULT);
        lv_obj_set_style_anim_time(s_sel_roller, 100, LV_STATE_DEFAULT);
        lv_roller_set_options(s_sel_roller,
                              "Info\n"
                              "Files\n"
                              "Wifi\n"
                              "BlueTooth\n"
                              "USB\n"
                              "About",
                              LV_ROLLER_MODE_INFINITE);
        lv_obj_align(s_sel_roller, LV_ALIGN_LEFT_MID, 110, 0);
        lv_obj_add_event_cb(s_sel_roller, on_roller_mask_paint, LV_EVENT_COVER_CHECK, NULL);
        lv_obj_add_event_cb(s_sel_roller, on_roller_mask_paint, LV_EVENT_DRAW_MAIN_BEGIN, NULL);
        lv_obj_add_event_cb(s_sel_roller, on_roller_mask_paint, LV_EVENT_DRAW_POST_END, NULL);
        lv_obj_add_event_cb(s_sel_roller, on_roller_select, LV_EVENT_KEY, NULL);
        lv_obj_add_event_cb(s_sel_roller, on_roller_confirm, LV_EVENT_VALUE_CHANGED, NULL);
        lv_group_add_obj(lv_group_get_default(), s_sel_roller);
#if 0
        lv_obj_t* text_area = lv_textarea_create(s_screen_handle);
        lv_group_add_obj(lv_group_get_default(), text_area);
        lv_obj_set_size(text_area, 160, 30);
        lv_obj_set_style_bg_color(text_area, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(text_area, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(text_area, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_opa(text_area, LV_OPA_90, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(text_area, lv_color_black(), LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_set_style_border_opa(text_area, LV_OPA_80, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(text_area, 1, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_set_style_border_side(text_area, LV_BORDER_SIDE_LEFT, LV_PART_CURSOR | LV_STATE_FOCUSED);
        lv_obj_set_style_anim_time(text_area, 500, LV_PART_CURSOR);
        lv_textarea_add_text(text_area, "demo");
#endif
        lv_group_focus_obj(s_sel_roller);
    }
}

void main_screen_load(void)
{
    if(s_screen_handle)
    {
        // lv_scr_load(s_screen_handle);
        lv_scr_load_anim(s_screen_handle, LV_SCR_LOAD_ANIM_FADE_OUT, 100, 0, false);
    }
}

static void on_screen_loaded(lv_event_t* event)
{
    keypad_force_fn(true);
}

static void on_screen_unloaded(lv_event_t* event)
{
    if(s_screen_handle)
    {
        lv_obj_del(s_screen_handle);
        s_screen_handle = NULL;
    }
}

static void on_roller_mask_paint(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * obj = lv_event_get_target(event);

    static int16_t mask_top_id = -1;
    static int16_t mask_bottom_id = -1;

    if (code == LV_EVENT_COVER_CHECK)
    {
        lv_event_set_cover_res(event, LV_COVER_RES_MASKED);
    }
    else if (code == LV_EVENT_DRAW_MAIN_BEGIN)
    {
        /* add mask */
        const lv_font_t* font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t font_h = lv_font_get_line_height(font);

        lv_area_t roller_coords;
        lv_obj_get_coords(obj, &roller_coords);

        lv_area_t rect_area;
        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;
        rect_area.y1 = roller_coords.y1;
        rect_area.y2 = roller_coords.y1 + (lv_obj_get_height(obj) - font_h - line_space) / 2;

        lv_draw_mask_fade_param_t* fade_mask_top = (lv_draw_mask_fade_param_t*)lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_top, &rect_area, LV_OPA_TRANSP, rect_area.y1, LV_OPA_COVER, rect_area.y2);
        mask_top_id = lv_draw_mask_add(fade_mask_top, NULL);

        rect_area.y1 = rect_area.y2 + font_h + line_space - 1;
        rect_area.y2 = roller_coords.y2;

        lv_draw_mask_fade_param_t* fade_mask_bottom = (lv_draw_mask_fade_param_t*)lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_bottom, &rect_area, LV_OPA_COVER, rect_area.y1, LV_OPA_TRANSP, rect_area.y2);
        mask_bottom_id = lv_draw_mask_add(fade_mask_bottom, NULL);

    }
    else if (code == LV_EVENT_DRAW_POST_END)
    {
        lv_draw_mask_fade_param_t* fade_mask_top = (lv_draw_mask_fade_param_t*)lv_draw_mask_remove_id(mask_top_id);
        lv_draw_mask_fade_param_t* fade_mask_bottom = (lv_draw_mask_fade_param_t*)lv_draw_mask_remove_id(mask_bottom_id);
        lv_draw_mask_free_param(fade_mask_top);
        lv_draw_mask_free_param(fade_mask_bottom);
        lv_mem_buf_release(fade_mask_top);
        lv_mem_buf_release(fade_mask_bottom);
        mask_top_id = -1;
        mask_bottom_id = -1;
    }
}

static void on_roller_select(lv_event_t* event)
{
    int selection = lv_roller_get_selected(s_sel_roller);
    ESP_LOGI(TAG, "New value sel: %d.", selection);
    lv_img_set_src(s_icon_image, menu_icons[selection]);
}

static void on_roller_confirm(lv_event_t* event)
{
    int selection = lv_roller_get_selected(s_sel_roller);
    ESP_LOGI(TAG, "Confirm selection: %d.", selection);
    switch(selection)
    {
        case 0:
        {
            sys_info_screen_create();
            sys_info_screen_load();
            keypad_force_fn(false);
            break;
        }
        case 1:
        {
            file_screen_create();
            file_screen_load();
            keypad_force_fn(false);
            break;
        }
        case 2:
        {
            wifi_scan_screen_create();
            wifi_scan_screen_load();
            keypad_force_fn(false);
            break;
        }
        case 5:
        {
            fw_info_screen_create();
            fw_info_screen_load();
            keypad_force_fn(false);
            break;
        }
        default:
        {
            /* Do nothing. */
        }
    }
}
