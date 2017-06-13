add_subdirectory(gattlib)

# passende folders fon gattlib hinzufügen

find_package(PkgConfig REQUIRED)

pkg_search_module(GATTLIB REQUIRED gattlib)

#include_directories(gattlib/bluez)
#include_directories(gattlib/dbus)
#include_directories(gattlib/include)

set(SOURCE_FILES
        Scanner.h
        Scanner.cpp
        Queue.h
        LinuxBluetoothLowEnergySocket.h
        LinuxBluetoothLowEnergySocket.cpp
        PeripherapConnectionCreator.h
        PeripherapConnectionCreator.cpp
        PerpheralConnection.h
        PerpheralConnection.cpp
        BluetoothLowEnergyMessage.h
        BluetoothLowEnergyMessage.cpp
        BluetoothLowEnergyAdvertise.h
        BluetoothLowEnergyAdvertise.cpp
        BlacklistEntry.h
        BlacklistEntry.cpp
        )

add_library(BluetoothLowEnergy ${SOURCE_FILES})
target_link_libraries(BluetoothLowEnergy ${GATTLIB_LIBRARIES})

set(BLE_MAIN_FILE blesocketmain.cpp)

add_executable(ble ${SOURCE_FILES} ${BLE_MAIN_FILE})
target_link_libraries(ble ${GATTLIB_LIBRARIES} pthread)

#set(discover_SRCS discover.c)
#add_executable(discover ${discover_SRCS})
