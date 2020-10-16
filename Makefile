#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := ble_kbdhid

EXTRA_CFLAGS := -DMYNEWT_VAL_BLE_SVC_GAP_CENTRAL_ADDRESS_RESOLUTION=0

include $(IDF_PATH)/make/project.mk

SPIFFS_IMAGE_FLASH_IN_PROJECT := 1
