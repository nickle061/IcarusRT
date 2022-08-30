import socket

HEADER_SIZE = 10

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("192.168.99.12", 1250))
print("Connection Successful")

while True:
    full_msg = ''
    new_msg = True
    while True:
        msg = s.recv(64)
        if new_msg:
            print(f"new message length: {msg[:HEADER_SIZE]}") # give us the length of the msg
            msg_len = int(msg[:HEADER_SIZE])
            new_msg = False
        full_msg += msg.decode("utf-8")
        if len(full_msg) - HEADER_SIZE == msg_len:
            print("full message received")
            print(full_msg[HEADER_SIZE:])
            new_msg = True
            full_msg = ''

    print(full_msg)