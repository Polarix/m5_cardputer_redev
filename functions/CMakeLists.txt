idf_component_register(
    SRCS "wifi_scan.cpp"
    PRIV_REQUIRES driver bt esp_wifi nvs_flash
    INCLUDE_DIRS "." "../gui"
)

add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)
