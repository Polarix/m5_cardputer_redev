#include "file_screen.h"
#include "screens.h"
#include "lvgl.h"
#include "esp_err.h"
#include "esp_log.h"

#define FILE_PATH_LEN_MAX       (512)

static void on_screen_unloaded(lv_event_t* event);
static void on_item_select(lv_event_t* event);
static void on_key_pressed(lv_event_t* event);
static void file_screen_show_dir(const char* path);
static void create_new_list(void);

LV_FONT_DECLARE(UNIFONT_16PX);
LV_IMG_DECLARE(FILE_LIST_ICON_FILE);
LV_IMG_DECLARE(FILE_LIST_ICON_FOLDER);

static lv_obj_t* s_screen_handle = NULL;
static lv_obj_t* s_top_bar = NULL;
static lv_obj_t* s_bottom_bar = NULL;
static lv_obj_t* s_path_label = NULL;
static lv_obj_t* s_list = NULL;

static const char* TAG = {"screen:file_list"};
static char s_current_path_buf[FILE_PATH_LEN_MAX] = {'\0'};

void file_screen_create(void)
{
    s_screen_handle = lv_obj_create(NULL);
    if(s_screen_handle)
    {
        ESP_LOGD(TAG, "Create file screen.");
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

        /* Path label */
        s_path_label = lv_label_create(s_top_bar);
        lv_obj_set_size(s_path_label, 230, 20);
        lv_obj_set_style_text_font(s_path_label, &UNIFONT_16PX, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(s_path_label, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_opa(s_path_label, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_center(s_path_label);
        lv_label_set_long_mode(s_path_label, LV_LABEL_LONG_SCROLL);

        /* Bottom bar. */
        s_bottom_bar = lv_obj_create(s_screen_handle);
        lv_obj_set_size(s_bottom_bar, 240, 24);
        lv_obj_align(s_bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_style_bg_color(s_bottom_bar, lv_color_hex(0x0079DB), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(s_bottom_bar, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(s_bottom_bar, lv_color_hex(0x003763), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(s_bottom_bar, LV_GRAD_DIR_VER, LV_STATE_DEFAULT);

        /* List */
        create_new_list();
    }
}

void file_screen_load(void)
{
    if(s_screen_handle)
    {
        // lv_scr_load(s_screen_handle);
        lv_scr_load_anim(s_screen_handle, LV_SCR_LOAD_ANIM_FADE_OUT, 500, 0, false);
        ESP_LOGD(TAG, "Load main screen.");
        strcpy(s_current_path_buf, "S:");
        file_screen_show_dir(s_current_path_buf);
    }
}

static void create_new_list(void)
{
    if(s_list)
    {
        lv_obj_del(s_list);
    }
    s_list = lv_list_create(s_screen_handle);

    /* Font */
    lv_obj_set_style_text_font(s_list, &UNIFONT_16PX, LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(s_list, LV_OPA_100, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(s_list, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(s_list, 5, LV_STATE_DEFAULT);
    /* Border */
    lv_obj_set_style_border_opa(s_list, LV_OPA_100, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(s_list, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(s_list, lv_color_hex(0x0079DB), LV_STATE_DEFAULT);
    /* Items */
    lv_obj_set_style_bg_opa(s_list, LV_OPA_100, LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_style_bg_color(s_list, lv_color_hex(0x080808), LV_STATE_DEFAULT | LV_PART_ITEMS);
    lv_obj_set_scrollbar_mode(s_list,LV_SCROLLBAR_MODE_ON);

    lv_obj_add_event_cb(s_list, on_key_pressed, LV_EVENT_KEY, NULL);

    lv_group_add_obj(lv_group_get_default(), s_list);
    lv_group_focus_obj(s_list);
}

static void set_list_item_style(lv_obj_t* list_item)
{
    if(list_item)
    {
        lv_obj_set_style_bg_opa(list_item, LV_OPA_100, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(list_item, lv_color_hex(0x080808), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(list_item, lv_color_hex(0x003763), LV_STATE_PRESSED);
        lv_obj_set_style_bg_color(list_item, lv_color_hex(0x003763), LV_STATE_FOCUSED);
        lv_obj_add_event_cb(list_item, on_item_select, LV_EVENT_CLICKED, NULL);
    }
}

static void file_screen_show_dir(const char* path)
{
    if(s_path_label)
    {
        lv_label_set_text(s_path_label, path);
    }
    if(s_list)
    {
        lv_fs_dir_t dir_handle;
        lv_fs_res_t fs_res = lv_fs_dir_open(&dir_handle, path);
        if(LV_FS_RES_OK == fs_res)
        {
            char read_buffer[128];
            while(LV_FS_RES_OK == lv_fs_dir_read(&dir_handle, read_buffer))
            {
                ESP_LOGD(TAG, "Read item %s.", read_buffer);
                if('\0' == read_buffer[0])
                {
                    break;
                }
                else
                {
                    const lv_img_dsc_t* item_icon = NULL;
                    if('/' == read_buffer[0])
                    {
                        size_t len = strlen(read_buffer);
                        memcpy(&read_buffer[0], &read_buffer[1], len);
                        item_icon = &FILE_LIST_ICON_FOLDER;
                    }
                    else
                    {
                        item_icon = &FILE_LIST_ICON_FILE;
                    }
                    set_list_item_style(lv_list_add_btn(s_list, item_icon, read_buffer));
                }
            }
            lv_fs_dir_close(&dir_handle);
        }
        else
        {
            ESP_LOGE(TAG, "Open path %s error(%d).", path, fs_res);
        }
        lv_obj_set_size(s_list, 230, 85);
        lv_obj_center(s_list);
    }
}

static void on_screen_unloaded(lv_event_t* event)
{
    if(s_screen_handle)
    {
        lv_obj_del(s_screen_handle);
        s_screen_handle = NULL;
        s_list = NULL;
    }
}

static void on_key_pressed(lv_event_t* event)
{
    if(LV_EVENT_KEY == lv_event_get_code(event))
    {
        if(LV_KEY_BACKSPACE == lv_event_get_key(event))
        {
            lv_fs_up(s_current_path_buf);
            create_new_list();
            file_screen_show_dir(s_current_path_buf);
        }
        else if(LV_KEY_ESC == lv_event_get_key(event))
        {
            main_screen_create();
            main_screen_load();
        }
    }
}

static void on_item_select(lv_event_t* event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t* item = lv_event_get_target(event);
    if(code == LV_EVENT_CLICKED)
    {
        const char* item_text = lv_list_get_btn_text(s_list, item);
        ESP_LOGI(TAG, "Clicked: %s", item_text);
        if(item_text)
        {
            size_t old_len = strlen(s_current_path_buf);
            size_t item_text_len = strlen(item_text);
            size_t new_len = old_len + item_text_len + 1;
            if(new_len < FILE_PATH_LEN_MAX)
            {
                strcat(s_current_path_buf, "/");
                strcat(s_current_path_buf, item_text);
                create_new_list();
                file_screen_show_dir(s_current_path_buf);
            }
        }
    }
}
