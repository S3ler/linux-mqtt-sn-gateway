cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Scanner.h Scanner.cpp ScanResult.cpp ScanResult.h ScannerCallbackInterface.h)
#set(MAIN_FILE main.cpp)

#add_executable(BLEScanner ${SOURCE_FILES} ${MAIN_FILE})
#target_link_libraries(BLEScanner pthread bluetooth)

add_library(libBLEScanner ${SOURCE_FILES})
target_link_libraries(libBLEScanner pthread bluetooth)