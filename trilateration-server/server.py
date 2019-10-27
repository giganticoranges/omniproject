import socket

s = socket.socket() #create a new socket object
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('0.0.0.0', 8090)) #bind socket to all ip addresses on LAN and por 8090

s.listen(0)

while True:
 
    client, addr = s.accept() #get client object and address

    while True:
        content = client.recv(1024)
 
        if len(content) ==0:
           break
 
        else:
            print(content)

    print("Client disconnectted - Closing connection") #if the client disconnects
    client.close()