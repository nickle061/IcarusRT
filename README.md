# IcarusRT
Codebase for mesh communication between server and client Nucleo development boards. The server board will receive TCP/IP packets through an ethernet port and broadcast them through an attached XBee module, which will then be received by another XBee module.

# Client and Server Model with Python
To set up a test TCP/IP socket for communication between a client and server, run the server.py and client.py files. These two file can be used to test whether our nucleo boards can send and receive TCP/IP packets. To test the client functionality of our nucleo board, first connect your nucleo board to your personal computer or device's ethernet port. Then start up the server.py file, and have your server.py file binds to your personal computer's IP address. Then in the C++ client.cpp script for your client nucleo board, have your nucleo board connected to your computer's IP address. Also ensure that both your server.py file and nucleo board script are interacting on the same port. Once everything is setup, your nucleo board should be able to print out the packet that it receives from your personal computer/device. 

# Forwarding packets received through ethernet port to analog TX/RX pins on Nucleo board
In the main.cpp file, it retains both server and client functionality of our nucleo board, while also forwarding the received packets to the TX/RX analog pins, so that the packets can be broadcasted wirelessly through our attached XBee moudle. The main.cpp script also handles de-structing the Zigbee headers that our XBee module includes in our packet.
