idf_component_register(SRCS "esp_lcd_st7789.c" esp_lvgl.c
                       INCLUDE_DIRS "./"
                       REQUIRES "driver" "esp_lcd" "esp_timer" "lvgl")

