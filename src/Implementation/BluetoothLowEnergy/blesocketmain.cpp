//
// Created by bele on 13.06.17.
//

#include "LinuxBluetoothLowEnergySocket.h"

int main(){
    LinuxBluetoothLowEnergySocket socket;
    if (socket.begin()) {
        while (true) {
            socket.loop();
        }
    }

}