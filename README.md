# HIDDEV
Simple cross-platform library to handle HID devices

It implements both both device and host sides of the HID communication using the same interface.

However, HID descriptors and reports as complex beasts, and you must always provide manual assembling/parsing of those
(Don't worry, some of the most common device types are implemented on the library)

# Supported Platforms

# Device
  - Linux:
    - [x] Virtual device conected as [UHID](https://www.kernel.org/doc/Documentation/hid/uhid.txt)
    - [ ] Bluetooth
    - [ ] USB via gadgetFS
  - Android
    - Bluetooth via [BluetoothHidDevice](https://developer.android.com/reference/android/bluetooth/BluetoothHidDevice)
  - Arduino:
    - [x] [PlugableUSB](https://github.com/arduino/Arduino/wiki/PluggableUSB-and-PluggableHID-howto)
    - [ ] [I2C](http://download.microsoft.com/download/7/d/d/7dd44bb7-2a7a-4505-ac1c-7227d3d96d5b/hid-over-i2c-protocol-spec-v1-0.docx)
  - ESP32:
    - [ ] BluetoothLE via [ESP32 BLE for Arduino](https://github.com/espressif/arduino-esp32/tree/master/libraries/BLE)

# Host
  - ESP32:
    - [ ] BluetoothLE via [ESP32 BLE for Arduino](https://github.com/espressif/arduino-esp32/tree/master/libraries/BLE)
  - Arduinos:
    - [ ] [I2C](http://download.microsoft.com/download/7/d/d/7dd44bb7-2a7a-4505-ac1c-7227d3d96d5b/hid-over-i2c-protocol-spec-v1-0.docx)
