Bluetooth low energy HID Keyboard+mouse example on ESP32 chip using Apache Mynewt NimBLE stack.

Inspired by Apache NimBLE peripheral role example (ESP-IDF v4.1 examples/bluetooth/nimble/blehr)
and ESP-IDF (v4.1) HID device example from examples/bluetooth/bluedroid/ble/ble_hid_device_demo.

This example creates GATT server and then starts advertising, waiting to be connected
to a GATT client. It supports several GPIO buttons, that can trigger image to send any
keyboard scan codes to client (as well as mouse clicks and moves).
Also this image switches LED when receives "CAPSLOCK on" event from GATT client.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

What is the point of using NimBLE stack on ESP32 instead of old Bluedroid stack?
1. First of all - size of the image. Following data was gained on ESP-IDF v4.1 with
default log level INFO and framework default component settings.
  - ble_hid_device_demo on Bluedroid has size 698208 bytes without gpio buttons
  - this example has size 473696 bytes (x1.47 smaller), and it has gpio buttons support
When I added gpio buttons to ble_hid_device_demo to gain an equal functionality as this demo,
its image size has grown to 816416 bytes, which is x1.7 bigger than this demo has.
2. Second benefit is the time to start stack. We are not talking about speed of stack in common, I haven't
done any research about it. But IMHO NimBLE should be a bit faster, because it is newer and BLE-only.
Here are some logs from these to images on same ESP32 chip.
  - ble_hid_device_demo on Bluedroid has started to advertise at 1915 milliseconds after boot.
      I (1915) HID_DEMO: Advertising started.
  - this example on NimBLE started to advertise at 1087 milliseconds after boot. (x1.75 faster)
      I (1087) NimBLEKBD_BLEFUNC: Device Address: fc:f5:c4:0e:24:1e
      GAP procedure initiated: advertise; disc_mode=2 adv_channel_map=0 ...
3. Thirdly, I formed an opinion for myself that it is more convenient to describe services and
characteristics of BLE device using NumBLE stack, than Bluedroid.

All BLE-psecific numbers (such as service and characteristics UUIDs) are taken from
ESP32 ble_hid_device_demo, some data was changed, some data was taken as is (for example: report map).

Documentation sources used:
1. "Getting Started with Bluetooth Low Energy" by Kevin Townsend, Carles Cufí, Akiba, and Robert Davidson
2. From bluetooth.org:
  a. Bluetooth Core Specification - Core_v5.2.pdf
  b. HID SERVICE SPECIFICATION - HIDS_SPEC_V10.pdf
  c. HID OVER GATT PROFILE SPECIFICATION - HOGP_SPEC_V10.pdf
  d. BLUETOOTH ASSIGNED NUMBERS - bluetooth_app_c8.pdf
  e. DEVICE INFORMATION SERVICE - DIS_SPEC_V11r00.pdf
3. From usb.org
  a. Device Class Definition for Human Interface Devices (HID) - hid1_11.pdf
  b. HID Usage Tables - hut1_12v2.pdf
4. ESP-IDF official documentation
  https://docs.espressif.com/projects/esp-idf/en/release-v4.1/api-reference/bluetooth/nimble/index.html
4. Apache NimBLE https://mynewt.apache.org/latest/network/ble_hs/ble_hs.html

Tested with ESP-IDF v4.1 and v3.3.4

How to use example (with ESP-IDF v4.1).

First, install ESP-IDF and tools according to ESP-IDF documentation.
See https://docs.espressif.com/projects/esp-idf/en/release-v4.1/get-started/index.html

### Configure the project

```
make menuconfig
'''

Enter "Example configuration menu" and change parameters if you want.
Bonding method (I/O Capability) "DISPLAY ONLY" allows you to set sercret number used on computer to pair.
Also set "Blink GPIO number for CAPSLOCK" to pin with led attached to it to indicate CAPSLOCK ON mode.

After changing config save new skdconfig and run

```
make && make flash && make monitor
'''

Dive into sources to know how to change buttons GPIO, send mouse moves and clicks or other keyboard keys.
Have fun with your new BLE Keyboard!
