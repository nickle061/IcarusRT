#include "BufferedSerial.h"
#include "ThisThread.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include "EthernetInterface.h"
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h> 

class bitParser{
    public:
    bitParser();
    bitParser(const bitParser &aBit);
    ~bitParser();
    bitParser& operator=(const bitParser &aBit);
    bool bitbybit(std::string theByte, uint8_t co);
    bool popOutAddressAndMessage(std::string &aAddress, std::vector<uint8_t> &aMessage);
    std::string address;
    std::vector<uint8_t> message;
    private:
    bool cleaned = true;
    int msgcounter = 0;
    int msgSize = 0;
    std::string msgSizeBuffer;
    bool start = false;
};

class bitMaker{
    public:
    bitMaker();
    bitMaker(const bitMaker &aBit);
    ~bitMaker();
    bitMaker& operator=(const bitMaker &aBit);
    std::vector<uint8_t> sendMaker(std::vector<uint8_t> &aMessage);
    bool changeAddress(uint8_t anAddress[]);
    uint8_t address[8];
    std::vector<uint8_t> message;
    private:
    bool cleaned = true;
    int msgcounter = 0;
    int msgSize = 0;
    std::string msgSizeBuffer;
    bool start = false;
};