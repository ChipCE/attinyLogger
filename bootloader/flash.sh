#!/bin/bash
adafruit-nrfutil --verbose dfu serial --package feather_nrf52840_express_bootloader-0.5.0_s140_6.1.1.zip -p /dev/ttyACM0 -b 115200 --singlebank --touch 1200