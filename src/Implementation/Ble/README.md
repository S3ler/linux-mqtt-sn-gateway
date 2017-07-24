GattLib is a library used to access Generic Attribute Profile (GATT) protocol of BLE (Bluetooth Low Energy) devices.
It has been introduced to allow to build applications that could easily communicate with BLE devices.

Taken from: https://github.com/labapart/gattlib

Known limitations
-----------------

* **gattlib and BLE**: gattlib requires at least Bluez v4.100 to work with Bluetooth Low Energy (BLE) devices. Bluez does not allow to connect to BLE device prior to this version. But gattlib can still work with Bluetooth Classic (BR/EDR) prior to Bluez v4.100.  
Debian 7 "Wheezy" (supported until 31st of May 2018) relies on Bluez v4.99 while Debian 8 "Jessie" (supported until April/May 2020) uses Bluez v5.23.

TODO List
=========

- Complete `examples/gatttool` port to GattLib to demonstrate the completeness of GattLib.
- Remove GLib dependencies to GattLib (mainly replacing GLib IO Channels by Unix Domain Socket).


# bluez_nordic_uart_example
This is an example project to communicate over Nordic's NUS service between two Linux Computer with Bluez.
My initial idea was to program it purely in C/C++ but bluez makes it really hard.

For the Gatt Client (Central) I use gattlib (C/C++).
Fot the Gatt Server (Peripheral) I user bluez (python).

For the Gatt Client implementation use my gattlib fork: [Link](https://github.com/S3ler/gattlib).
I included some small fixes - you can also use the [original gattlib project](https://github.com/labapart/gattlib) too.
My computer runs with Ubuntu 16.04 TLS:

$ uname -a
Linux bele-desktop 4.8.0-56-generic #61~16.04.1-Ubuntu SMP Wed Jun 14 11:58:22 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux

And the default installed bluez version ist 5.37.
$ bluetoothd --version
5.37

I wanted to stick with the default version to make further applications as portable as possible (which is not easy with bluez).

I obtained the BlueZ source code from here: http://www.bluez.org/release-of-bluez-5-37/
For the beginning I modified the test/example-gatt-server python script.



## Futher development
If someone can implement the python Gatt Server in C/C++ embeable into my applications i will be very pleased.


