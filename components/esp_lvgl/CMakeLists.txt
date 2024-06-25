idf_component_register(SRCS esp_lvgl.c esp_lcd_st7789.c esp_keypad.c
                       INCLUDE_DIRS "./"
                       REQUIRES "driver" "esp_lcd" "esp_timer" "esp_ringbuf" "lvgl" "keypad")

