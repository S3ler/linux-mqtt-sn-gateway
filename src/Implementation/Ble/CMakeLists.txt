cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(SimpleBluetoothLowEnergySocket)

add_library(libMqttSNBLESocket BLEDeviceStatistic.cpp BLEDeviceStatistic.h BLESocket.cpp BLESocket.h BLEConnectionAcceptor.cpp BLEConnectionAcceptor.h BLEConnection.cpp BLEConnection.h BLEMessage.cpp BLEMessage.h)
target_link_libraries(libMqttSNBLESocket libBLE)