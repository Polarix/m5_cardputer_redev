idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES "driver" "esp_lcd" "lvgl" "esp_lvgl" "gui" "bat_volt" "sdcard_fs"
)
add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)