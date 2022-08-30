#include "mbed.h"
#include "classes.hpp"
#include <cstdint>
#include <iostream>
#include "SocketAddress.h"
#include "TCPSocket.h"
#include "mbed.h"
#include "EthernetInterface.h"
#include <cstdint>
#include "BufferedSerial.h"




EthernetInterface net;


#define MAXIMUM_BUFFER_SIZE 40

// Create a BufferedSerial object with a default bault rate
static BufferedSerial serial_port(ARDUINO_UNO_D1, ARDUINO_UNO_D0); // tx, rx
//static BufferedSerial Ethernet_port(ETH_TXD1, ETH_RXD1);
uint8_t theSAddress [8]= {0x00, 0x13, 0xa2, 0x00, 0x41, 0xe4, 0xb2, 0xd0};





bool to_hex(char* dest, size_t dest_len, const uint8_t* values, size_t val_len) {
    if(dest_len < (val_len*2+1)) /* check that dest is large enough */
        return false;
    *dest = '\0'; /* in case val_len==0 */
    while(val_len--) {
        /* sprintf directly to where dest points */
        sprintf(dest, "%02x", *values);
        dest += 2;
        ++values;
    }
    return true;
}




int main() {
    std::cout<<"program starting..."<<std::endl;
    net.set_network("192.168.99.12", "255.255.255.0", "192.168.99.1");
    net.connect();
    TCPSocket socket;
    SocketAddress a {"192.168.99.254"};
    a.set_port(1252);
    socket.open(&net);
    socket.connect(a);
    //the wrappers
    std::vector<uint8_t> newMessage;
    newMessage.push_back(0x6e);
    newMessage.push_back(0x65);
    newMessage.push_back(0x77);
    newMessage.push_back(0x6e);
    newMessage.push_back(0x65);
    newMessage.push_back(0x77);
    std::vector<uint8_t>endMessage;
    endMessage.push_back(0x65);
    endMessage.push_back(0x6e);
    endMessage.push_back(0x64);
    endMessage.push_back(0x65);
    endMessage.push_back(0x6e);
    endMessage.push_back(0x64);
    // Set desired properties 
    serial_port.set_baud(115200);
    serial_port.set_format(
        /* bits */ 8,
        /* parity */ SerialBase::None,
        /* stop bit */ 1
    );
    // Starting up the ethernet interface
    // printf("Establish Ethernet Socket\n");
    // net.connect();
    // //show network address
    // SocketAddress a;
    // net.get_ip_address(&a);
    // printf("IP address: %s\n", a.get_ip_address() ? a.get_ip_address() : "None");
    //Open a TCP Socket
    // TCPSocket socket;
    // socket.open(&net);

    // net.gethostbyname("<NETWORK IP ADDRESS, MUST BE ON THE NETWORK>", &a); // host name can also be an ip address
    // a.set_port(80); // set port
    // socket.connect(a);

    // defining a receiving buffer
    // Recieve a simple http response and print out the response line
    // char rbuffer[64]; // receiving buffer
    // int rcount = socket.recv(rbuffer, sizeof rbuffer);
    // printf("recv %d [%.*s]\n", rcount, strstr(rbuffer, "\r\n") - rbuffer, rbuffer);
    

    // Code to receieve data
    uint8_t c = 0;
    char c2[2*sizeof(c)+1];
    std::stringstream ss;
    std::string mystr;
    bitParser abitparser;
    std::string theAddress;
    vector<uint8_t> theMessage;
    std::cout<<"initializing"<<"\n"<<std::endl;
    bool onGoingMessage = false;
    bool endofMessage = false;
    vector<uint8_t> theFinal;

    bitMaker theMaker;
    theMaker.changeAddress(theSAddress);
    vector<uint8_t> message;
    uint8_t theSend [70];
    uint8_t rbuff[64];
    ssize_t theSize = 0;
    socket.set_blocking(false);
    uint8_t theOutMsg [640];
    uint8_t temp;
    std::cout<<"looping"<<std::endl;

    while(true){
        theSize = socket.recv(rbuff, sizeof rbuff);
        
        if(theSize>0){
            for(int i = 0; i < theSize; i++){
                message.push_back(rbuff[i]);
                std::cout<<rbuff[i];
            }
            std::cout<<std::endl;
        }
        
        if (serial_port.writable()) {
            //std::fill(theSend[0], theSend[69], 0);
            if(message.size()>0){
                std::vector<uint8_t> theSMessage = theMaker.sendMaker(newMessage);
                std::cout<<"New Message Created"<<std::endl;
                for(int i = 0; i < theSMessage.size(); i++){
                    theSend[i] = theSMessage[i];
                    //std::cout<<theMessage[i]<<std::endl;
                    //std::cout<<theSend[i]<<std::endl;
                    // check1 = theSend[i];
                    // to_hex(check, sizeof(check), &check1, sizeof(check1));
                    // std::cout<<check<<std::endl;

                }
                std::cout<<std::endl;
                std::cout<<sizeof(theSend)<<std::endl;
                //std::cout<<theSend<<std::endl;
                serial_port.write(theSend , theSMessage.size());
                //std::fill(theSend[0], theSend[69], 0);
                while(message.size()>0){
                    theSMessage = theMaker.sendMaker(message);
                    for(int i = 0; i < theSMessage.size(); i++){
                        theSend[i] = theSMessage[i];
                    }
                    serial_port.write(theSend , theSMessage.size());
                    //std::fill(theSend[0], theSend[69], 0);
                }
                theSMessage = theMaker.sendMaker(endMessage);
                for(int i = 0; i < theSMessage.size(); i++){
                    theSend[i] = theSMessage[i];
                }
                serial_port.write(theSend , theSMessage.size());
            }
        }
    }
        while(serial_port.readable()){
            serial_port.read(&c, 1); // read from receiving buffer
            to_hex(c2, sizeof(c2), &c, sizeof(c));
            mystr = c2[0];
            mystr += c2[1];
            if(abitparser.bitbybit(mystr, c)){
                abitparser.popOutAddressAndMessage(theAddress, theMessage);

                if(theMessage == newMessage){
                    onGoingMessage = true;
                    continue;
                 }
                else if(theMessage == endMessage){
                    onGoingMessage = false;
                    //send out the Final

                    socket.send(theOutMsg, theFinal.size());
                    theFinal.clear();
                    continue;
                 }
                if(onGoingMessage == true){
                    for(int i = 0; i < theMessage.size(); i++){
                        theOutMsg[i] = theMessage[i];
                        theFinal.push_back(theMessage[i]);
                    }
                }


                


                std::cout<<"The Address is: "<<theAddress<<std::endl;
                std::cout<<"The Message is: ";
                for(int i = 0; i<theMessage.size(); i++){
                    std::cout<<theMessage[i];
                }
                //checks address and sends accordingly most likely via some kind of for loop
                //append header? but is it necessary?
                std::cout<<std::endl;
            }
            
        }
        
    }
    


