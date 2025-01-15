# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
#./extra_components/esp_driver_i2c
cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS
    ./extra_components/am2320
    ./extra_components/ssd1306
    ./extra_components/stemma_soil
    )
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(project_display)
