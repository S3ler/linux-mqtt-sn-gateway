#!/bin/bash
sudo hciconfig hci0 down & \
sudo hciconfig hci0 up & \
sudo gdbserver localhost:8889 cmake-build-debug//src/Implementation/BluetoothLowEnergy/ble