#include "SocketAddress.h"
#include "TCPSocket.h"
#include "mbed.h"
#include "EthernetInterface.h"
#include <cstdint>
#include "BufferedSerial.h"
#include <iostream>


#define MAXIMUM_BUFFER_SIZE 40

// main() runs in its own thread in the OS
static BufferedSerial serial_port(ARDUINO_UNO_D1, ARDUINO_UNO_D0); // tx, rx

EthernetInterface net;
// 68.27.19.cc.22.57
int main()
{
    serial_port.set_baud(9600);
    serial_port.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );
    printf("Ethernet socket example\n");
    // set static client ip
    net.set_network("192.168.99.13", "255.255.255.0", "192.168.99.1");
    net.connect();


    TCPSocket socket;
    SocketAddress a {"192.168.99.11"}; // server IP
    a.set_port(1250);
    socket.open(&net);
    socket.connect(a);
    socket.set_blocking(false);
    char rbuff[64];


    while(true){
        ssize_t receive = socket.recv(rbuff, sizeof rbuff);
        if(receive > 0)
            std::cout << rbuff << std::endl;

        serial_port.write(rbuff, sizeof rbuff);
    }
        



}

