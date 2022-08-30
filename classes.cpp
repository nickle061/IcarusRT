#include "classes.hpp"
#include <cstdint>

bitParser::bitParser(){

}
bitParser::bitParser(const bitParser &aBit){

}
bitParser::~bitParser(){

}
bitParser& bitParser::operator=(const bitParser &aBit){
    return *this;
}
bool bitParser::bitbybit(std::string theByte, uint8_t co){
    std::stringstream str;
    if(start == true){
        if(msgcounter == 1){
            msgSizeBuffer.append(theByte);
            msgcounter++;
            return false;
        }
        else if(msgcounter == 2){
            msgSizeBuffer.append(theByte);
            msgSize = std::stoi (msgSizeBuffer,nullptr,16);
            //strtol(theByte.c_str(), NULL, 16);
            // std::cout<<"I see the size to be " << std::to_string(msgSize) <<std::endl;
            msgcounter++;
            return false;
        }
        else if(3 <= msgcounter && msgcounter < 8){
            msgcounter++;
            return false;
        }
        else if(msgcounter >= 8 && msgcounter < 13){
            address.append(theByte);
            msgcounter++;
            return false;
        }
        else if(msgcounter >= 12 && msgcounter <= 15){
            msgcounter++;
            return false;
        }
        else if(msgcounter > 15 && msgcounter < msgSize + 4){
            message.push_back(co);
            msgcounter++;
            return false;
        }
        else if(msgcounter == msgSize + 4){
            //write function to send strings out
            //std::cout<<"I see the end"<<std::endl;
            msgcounter = 0;
            msgSize = 0;
            start = false;
            msgSizeBuffer.clear();
            return true;
        }
    }
    if(theByte=="7e"){
        if(cleaned == false){
            std::cout << "\n error has occured" <<std::endl;
            return false;
        }
        start = true;
        msgcounter++;
        cleaned = false;
        //std::cout<<"I see the starting bit"<<std::endl;
        return false;
    }
    std::cout<<"\nerror has occured in the bitparser"<<std::endl;
    return false;
}

bool bitParser::popOutAddressAndMessage(std::string &aAddress, std::vector<uint8_t> &aMessage){
    if(start == true){
        return false;
    }
    aAddress = address;
    aMessage = message;
    address.clear();
    message.clear();
    cleaned = true;
    return true;
}

bitMaker::bitMaker(){
}
bitMaker::bitMaker(const bitMaker &aBit){

}
bitMaker::~bitMaker(){

}
bitMaker& bitMaker::operator=(const bitMaker &aBit){
    return *this;
}

std::vector<uint8_t> bitMaker::sendMaker(std::vector<uint8_t> &aMessage){
    std::vector<uint8_t> wrappedMessage;
    wrappedMessage.push_back(0x7e);
    uint16_t size;
    if(aMessage.size()>50){
        size = 50 + 14;
    }
    else{
     size = aMessage.size() + 14;
    }   
    wrappedMessage.push_back(size/0x100);
    wrappedMessage.push_back(size%0x100);
    wrappedMessage.push_back(0x10);
    wrappedMessage.push_back(0x01);
    for(int i =0; i < sizeof(address); i++){
        wrappedMessage.push_back(address[i]);
    }
    wrappedMessage.push_back(0xff);
    wrappedMessage.push_back(0xfe);
    wrappedMessage.push_back(0x00);
    wrappedMessage.push_back(0x00);
    int i = 0;
    while(aMessage.size()>0){
        wrappedMessage.push_back(aMessage[0]);
        aMessage.erase(aMessage.begin());
        i++;
        if(i == 50) break;
    }
    uint32_t sum = 0;
    for(int i = 3; i <wrappedMessage.size();i++ ){
        sum += wrappedMessage[i];
    }
    uint8_t checksum = 0xff - (sum%0x100);
    wrappedMessage.push_back(checksum);
    return wrappedMessage;
}

bool bitMaker::changeAddress(uint8_t anAddress[]){
    for(int i = 0; i< sizeof(address); i++){
        address[i] = anAddress[i];
    }
    return true;
}