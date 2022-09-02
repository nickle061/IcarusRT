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
// Create a BufferedSerial object with a default bault rate, this is to communicate with the Xbee module
static BufferedSerial serial_port(ARDUINO_UNO_D1, ARDUINO_UNO_D0); // tx, rx
//this is the default address that will be loaded in, this can be changed by the user
uint8_t theSAddress [8]= {0x00, 0x13, 0xa2, 0x00, 0x41, 0xe4, 0xb2, 0xd0};




//this is a function that converts uin8_t data to string hex, i.e. 0x7e --> "7e"
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
    //this establishes the ip of the board
    net.set_network("192.168.99.12", "255.255.255.0", "192.168.99.1");
    net.connect();
    TCPSocket socket;
    //this establishes the ip that it will attempt to send data to
    SocketAddress a {"192.168.99.254"};
    a.set_port(1252);
    socket.open(&net);
    socket.connect(a);
    //the wrappers, these are messages that will be sent at the beginning and end of each message since we except messages larger than the 50 byte packets that the XBee wants
    //thus it will use these beginning and end messages to reconstruct the message
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
    // Set desired properties of the serial connection to the xbee module
    serial_port.set_baud(115200);
    serial_port.set_format(
        /* bits */ 8, // 8bits a byte
        /* parity */ SerialBase::None,
        /* stop bit */ 1
    );


    // Code to receieve data, the earlier portions are simple various variables needed as well as declaring the send and recieve objects
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
    theMaker.changeAddress(theSAddress);//initializes the default address
    vector<uint8_t> message;
    uint8_t theSend [70];
    uint8_t rbuff[64];
    ssize_t theSize = 0;
    socket.set_blocking(false);//this is important, this prevents the socket from blocking the loop, since by default the socket read blocks the system from continuing unless it reads a byte
    uint8_t theOutMsg [640];
    uint8_t temp;
    std::cout<<"looping"<<std::endl;

    while(true){
        //this reads the contents of what the socket recieves into a buffer
        theSize = socket.recv(rbuff, sizeof rbuff);
        //if the socket receive nothing it returns a zero, else it returns the size of the message
        if(theSize>0){
            for(int i = 0; i < theSize; i++){
                message.push_back(rbuff[i]);
            }
        }
        
        //checks if the xbee is ready to recieve serial data
        if (serial_port.writable()) {
            //checks if there is message
            if(message.size()>0){
                //calls function from object to create the new message message (wrapper)
                std::vector<uint8_t> theSMessage = theMaker.sendMaker(newMessage);
                std::cout<<"New Message Created"<<std::endl;
                //puts the vector into an array because mbed serial can only use array
                for(int i = 0; i < theSMessage.size(); i++){
                    theSend[i] = theSMessage[i];

                }
                //sends out the "new message" message via serial
                serial_port.write(theSend , theSMessage.size());
                //this creates and sends the message for the actual intended message in groups of max 50 bytes
                while(message.size()>0){
                    theSMessage = theMaker.sendMaker(message);
                    for(int i = 0; i < theSMessage.size(); i++){
                        theSend[i] = theSMessage[i];
                    }
                    serial_port.write(theSend , theSMessage.size());
                }
                //this create the end of the wrapper so the reciever knows that the message ends here
                theSMessage = theMaker.sendMaker(endMessage);
                for(int i = 0; i < theSMessage.size(); i++){
                    theSend[i] = theSMessage[i];
                }
                //sends out the "end message" message
                serial_port.write(theSend , theSMessage.size());
            }
        }
        
        //keeps reading as long as there is input from the xbee
        while(serial_port.readable()){
            serial_port.read(&c, 1); // reads in buffer size of 1 byte
            to_hex(c2, sizeof(c2), &c, sizeof(c)); //converts to hex character array
            mystr = c2[0];
            mystr += c2[1]; //puts the hex characters into a string variable
            //begin reading, note that the function is nested in a if statement to check when it ends
            if(abitparser.bitbybit(mystr, c)){
                //if true, pull out message and address of sender
                abitparser.popOutAddressAndMessage(theAddress, theMessage);

                if(theMessage == newMessage){//checks to see when the message starts and create a boolean so that we know that the message has started
                    onGoingMessage = true;
                    continue;
                 }
                else if(theMessage == endMessage){//if message has ended set boolean to false
                    onGoingMessage = false;
                    //send out the Final

                    socket.send(theOutMsg, theFinal.size()); //sends the message out via the ip socket
                    theFinal.clear();
                    continue;
                 }
                if(onGoingMessage == true){ //this puts the message into an array which the mbed system functions prefers
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
        
    }
    


