# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

idf_component_register(
        SRCS lv_port_disp.c lv_port_indev.c
        INCLUDE_DIRS "."
        REQUIRES lvgl lvgl_esp32_drivers
)