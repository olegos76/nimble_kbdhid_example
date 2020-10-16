# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(SUPPORTED_TARGETS esp32)
set(ENV{EXTRA_CFLAGS} "-DMYNEWT_VAL_BLE_SVC_GAP_CENTRAL_ADDRESS_RESOLUTION=0")
project(ble_kbdhid)
