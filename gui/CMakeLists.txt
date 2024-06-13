idf_component_register(
    SRCS "gui.cpp" "startup_screen.cpp" "wifi_list_screen.cpp" "wifi_info_screen.cpp" "LOGO_50PX.c" "UNIFONT_16PX.c"
    PRIV_REQUIRES driver lvgl lvgl_porting
    INCLUDE_DIRS "." "../functions"
)

add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)
