file(GLOB_RECURSE RES_IMG_SRCS
    ./resource/images/*.c
)

file(GLOB_RECURSE RES_FONT_SRCS
    ./resource/fonts/*.c
)

file(GLOB_RECURSE SCREENS_SRCS
    ./screens/*.c
)

idf_component_register(
    SRCS ${RES_IMG_SRCS} ${RES_FONT_SRCS} ${SCREENS_SRCS}
    PRIV_REQUIRES lvgl bat_volt spi_flash
    INCLUDE_DIRS "./screens"
)

add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)
