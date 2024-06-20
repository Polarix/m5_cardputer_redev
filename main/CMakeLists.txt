idf_component_register(
    SRCS "main.cpp" "startup_screen.cpp" "logo.c" "BASIC_ASCII.c"
    INCLUDE_DIRS "." "../gui"
    REQUIRES "driver" "esp_lcd" "lvgl" "esp_lvgl"
)
add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)