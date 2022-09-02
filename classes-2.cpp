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

//this function reads a message bit by bit and keeps track of it automatically
//the function returns true once it reads the full message
//it is important to note to let it return true before doing anything with the message or address
bool bitParser::bitbybit(std::string theByte, uint8_t co){
    std::stringstream str;
    if(start == true){//this variable is set to true if the starting bit is read, which is later on for code efficiency sake
        //for the 1st and 2nd bytes (after the 0th byte) the xbee reads the length of the message
        //here it appends the 2 byte length information into a string and converts that into an integer
        if(msgcounter == 1){
            msgSizeBuffer.append(theByte);
            msgcounter++;
            return false;
        }
        else if(msgcounter == 2){
            msgSizeBuffer.append(theByte);
            msgSize = std::stoi (msgSizeBuffer,nullptr,16);
            msgcounter++;
            return false;
        }
        //these bytes are relevant only to the xbee modules itself and some details of how the message is sent
        //it is irrelevant to reading the message 
        else if(3 <= msgcounter && msgcounter < 8){
            msgcounter++;
            return false;
        }
        //these bytes are the address of the origin of the message, thus it is read and put into a vector that holds onto the sender's address
        else if(msgcounter >= 8 && msgcounter < 13){
            address.append(theByte);
            msgcounter++;
            return false;
        }
        //these are additional bytes that does not give us information on the message itself
        else if(msgcounter >= 12 && msgcounter <= 15){
            msgcounter++;
            return false;
        }
        //this is the main message, who's lenght is decided by the messagesize that was read earlier
        else if(msgcounter > 15 && msgcounter < msgSize + 4){
            message.push_back(co);
            msgcounter++;
            return false;
        }
        //this is the end of the message, there is no need to read the checksum since the Xbee module itself would have done this check
        //all relevant variables are cleared and reset except for the message and address, thus the pop out function must be called after bit by bit
        else if(msgcounter == msgSize + 4){
            //write function to send strings out
            msgcounter = 0;
            msgSize = 0;
            start = false;
            msgSizeBuffer.clear();
            return true;
        }
    }
    if(theByte=="7e"){ //this is the code that checks for the starting byte, the rest of the logic will not begin unless it sees the starting code
        if(cleaned == false){//this is the check to make sure that everything has been removed from the memory of the object
            std::cout << "\n error has occured" <<std::endl;
            return false;
        }
        start = true;
        msgcounter++;
        cleaned = false;
        return false;
    }
    std::cout<<"\nerror has occured in the bitparser"<<std::endl;
    return false;
}


//this function simply returns the address and message from the object and deletes them
//the address and message are public variables, so if there is a situation where one would like to read without deleting the existing variable, they can reference the variables directly
//this must be called at some point after bitbybit return true to prevent an error
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


//this function create a vector that contains a made message that can be sent through the Xbee
//note that the message passed in is a reference pointer, this is because this function deletes the message passed in as it adds the data to the wrapped message
std::vector<uint8_t> bitMaker::sendMaker(std::vector<uint8_t> &aMessage){
    std::vector<uint8_t> wrappedMessage;
    wrappedMessage.push_back(0x7e);//adding the starting byte
    uint16_t size;
    //due to xbee constraints, packets must be limited to 50 bytes, thus this here is to keep it to 50bytes
    if(aMessage.size()>50){
        size = 50 + 14;
    }
    else{
     size = aMessage.size() + 14;
    }   
    //the next two lines converts the length data into the correct format of two one byte data segments
    wrappedMessage.push_back(size/0x100);
    wrappedMessage.push_back(size%0x100);
    //these are the default xbee transmission settings
    wrappedMessage.push_back(0x10);
    wrappedMessage.push_back(0x01);
    //this adds the target address that the message is meant for, this can be changed in the changeAddress function
    for(int i =0; i < sizeof(address); i++){
        wrappedMessage.push_back(address[i]);
    }
    //additional default settings
    wrappedMessage.push_back(0xff);
    wrappedMessage.push_back(0xfe);
    wrappedMessage.push_back(0x00);
    wrappedMessage.push_back(0x00);
    int i = 0;
    //this adds the message to the stack
    while(aMessage.size()>0){
        wrappedMessage.push_back(aMessage[0]);
        aMessage.erase(aMessage.begin());
        i++;
        if(i == 50) break;
    }
    //this is the XBee checksum byte, it is the sum of the bytes of the message (excluding starting byte and length)
    //the last two hex digits (i.e. the last byte) of that sum is then taken to be the check sum
    uint32_t sum = 0;
    for(int i = 3; i <wrappedMessage.size();i++ ){
        sum += wrappedMessage[i];
    }
    uint8_t checksum = 0xff - (sum%0x100);
    wrappedMessage.push_back(checksum);
    return wrappedMessage;
}

//this changes the address stored in the object
bool bitMaker::changeAddress(uint8_t anAddress[]){
    for(int i = 0; i< sizeof(address); i++){
        address[i] = anAddress[i];
    }
    return true;
}