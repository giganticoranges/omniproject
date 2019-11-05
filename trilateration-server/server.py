import socket
import sys

from io import StringIO

import pandas as pd

s = socket.socket() #create a new socket object
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('0.0.0.0', 8090)) #bind socket to all ip addresses on LAN and por 8090

s.listen(0)

while True:
 
    client, addr = s.accept() #get client object and address

    while True:
        content = client.recv(1000)
 
        if len(content) == 0:
           break
 
        else:
            #print(content)
            DATA = StringIO(content.decode("utf-8")) #content stream is bytes, so cast to string
            df = pd.read_csv(DATA, sep=",")
            df.columns = df.columns.str.strip()
            df = df.sort_values(by = 'RSSI', ascending = False)
            print(df)

    #print("Client disconnectted - Closing connection") #if the client disconnects
    client.close()