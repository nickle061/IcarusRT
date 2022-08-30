import socket

HEADER_SIZE = 10

# SOCK_STREAM --> TCP/IP
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(("192.168.99.11", 1252))
# Server listening with a queue of 5
s.listen(5)

while True:
    client_socket, address = s.accept()
    print(f"Connection from {address} has been established")

    msg = "Temperature : 79.9923"
    msg = f'{len(msg) : < {HEADER_SIZE}}' + msg

    client_socket.send(bytes(msg, "utf-8"))  # sending information to the client socket



