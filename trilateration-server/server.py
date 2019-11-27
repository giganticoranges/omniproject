import socket
import sys

from io import StringIO

import pandas as pd
import math
import numpy as np
import matplotlib.pyplot as plt
from itertools import islice



def get_intersections(x0, y0, r0, x1, y1, r1):
    # circle 1: (x0, y0), radius r0
    # circle 2: (x1, y1), radius r1

    d=math.sqrt((x1-x0)**2 + (y1-y0)**2)

    # non intersecting
    if d > r0 + r1 :
        return None
    # One circle within other
    if d < abs(r0-r1):
        return None
    # coincident circles
    if d == 0 and r0 == r1:
        return None
    else:
        a=(r0**2-r1**2+d**2)/(2*d)
        h=math.sqrt(r0**2-a**2)
        x2=x0+a*(x1-x0)/d   
        y2=y0+a*(y1-y0)/d   
        x3=x2+h*(y1-y0)/d     
        y3=y2-h*(x1-x0)/d 

        x4=x2-h*(y1-y0)/d
        y4=y2+h*(x1-x0)/d

        return (x3, y3, x4, y4)

def distance_calc(rssi):
    
    distance = 0.0153 * np.exp(-0.06 * rssi)
    
    return distance

def centroid_three_lines(m1, b1, m2, b2, m3, b3):
    
    int_x1 = (b2 - b1) / (m1 - m2)
    int_x2 = (b3 - b2) / (m2 - m3)
    int_x3 = (b3 - b1) / (m1 - m3)
    
    int_y1 = m1 * int_x1 + b1
    int_y2 = m2 * int_x2 + b2
    int_y3 = m3 * int_x3 + b3
    
    centroid_x = (int_x1 + int_x2 + int_x3) / 3
    centroid_y = (int_y1 + int_y2 + int_y3) / 3
    
    return int_x1, int_y1, int_x2, int_y2, int_x3, int_y3, centroid_x, centroid_y

def gen_map(x0, y0, r0, x1, y1, r1, x2, y2, r2):

    # create circles
    circle1 = plt.Circle((x0, y0), r0, color='b', fill=False)
    circle2 = plt.Circle((x1, y1), r1, color='b', fill=False)
    circle3 = plt.Circle((x2, y2), r2, color='b', fill=False)

    # create plot
    fig, ax = plt.subplots() 
    ax.set_xlim((-2, 2))
    ax.set_ylim((-2, 2))
    ax.add_artist(circle1)
    ax.add_artist(circle2)
    ax.add_artist(circle3)

    # intersections with circle 1 and circle 2
    intersections = get_intersections(x0, y0, r0, x1, y1, r1)
    if intersections is not None:
        i_x3, i_y3, i_x4, i_y4 = intersections 
        plt.plot([i_x3, i_x4], [i_y3, i_y4], 'o', color='r')
    tempx1 = i_x3
    tempx2 = i_x4
    tempy1 = i_y3
    tempy2 = i_y4
    # tangent between intersection points
    if tempx1 == tempx2:
        M1 = 1000
        B1 = 0
    elif tempx1 > tempx2:
        M1 = (tempy1 - tempy2) / (tempx1 - tempx2)
        B1 = tempy1 - M1 * tempx1
    else:
        M1 = (tempy2 - tempy1) / (tempx2 - tempx1)
        B1 = tempy1 - M1 * tempx1


    # intersections with circle 1 and circle 3
    intersections = get_intersections(x0, y0, r0, x2, y2, r2)
    if intersections is not None:
        i_x3, i_y3, i_x4, i_y4 = intersections 
        plt.plot([i_x3, i_x4], [i_y3, i_y4], 'o', color='g')
    tempx1 = i_x3
    tempx2 = i_x4
    tempy1 = i_y3
    tempy2 = i_y4
    # tangent between intersection points
    if tempx1 == tempx2:
        M2 = 1000
        B2 = 0
    elif tempx1 > tempx2:
        M2 = (tempy1 - tempy2) / (tempx1 - tempx2)
        B2 = tempy1 - M2 * tempx1
    else:
        M2 = (tempy2 - tempy1) / (tempx2 - tempx1)
        B2 = tempy1 - M2 * tempx1


    # intersections with circle 2 and circle 3    
    intersections = get_intersections(x1, y1, r1, x2, y2, r2)
    if intersections is not None:
        i_x3, i_y3, i_x4, i_y4 = intersections 
        plt.plot([i_x3, i_x4], [i_y3, i_y4], 'o', color='b')
    tempx1 = i_x3
    tempx2 = i_x4
    tempy1 = i_y3
    tempy2 = i_y4
    # tangent between intersection points
    if tempx1 == tempx2:
        M3 = 1000
        B3 = 0
    elif tempx1 > tempx2:
        M3 = (tempy1 - tempy2) / (tempx1 - tempx2)
        B3 = tempy1 - M3 * tempx1
    else:
        M3 = (tempy2 - tempy1) / (tempx2 - tempx1)
        B3 = tempy1 - M3 * tempx1    


    nx1, ny1, nx2, ny2, nx3, ny3, cx, cy = centroid_three_lines(M1, B1, M2, B2, M3, B3)
    plt.plot(cx, cy, 'o', color = 'purple')
    plt.gca().set_aspect('equal', adjustable='box')


    plt.show()


def gen_rssi(x1, y1, rssi1, x2, y2, rssi2, x3, y3, rssi3):
    
    if((x1 != x2 and y1 != y2) or (x1 != x3 and y1 != y3)):
        r1 = distance_calc(rssi1)
        r2 = distance_calc(rssi2)
        r3 = distance_calc(rssi3)
        gen_map(x1, y1, r1, x2, y2, r2, x3, y3, r3)
    else:
        print('Overlapping points!')


# Main
if __name__=="__main__":
    beacons = pd.read_csv('beacon_data.csv') #read in beacons info
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
                counter = 0
                x = []
                y = []
                rssi = []
                DATA = StringIO(content.decode("utf-8")) #content stream is bytes, so cast to string
                df = pd.read_csv(DATA, sep=",")
                df.columns = df.columns.str.strip()
                df = df.sort_values(by = 'RSSI', ascending = False)
                print(df)
                for index, row in islice(df.iterrows(), 3):
                    counter += 1
                    current_row = beacons.loc[beacons['Beacon'] == row['Beacon']]
                    #print(row['Beacon'], ', ', row['RSSI'], 'location: ', current_row['x'], ',',  current_row['y'])
                    #print('x,y,rssi: ', current_row.iloc[0, 1], current_row.iloc[0, 2], row['RSSI'])
                    x.append(current_row.iloc[0, 1])
                    y.append(current_row.iloc[0, 2])
                    rssi.append(row['RSSI'])
                print('sending to function: ', x[0], ',', y[0], ',', rssi[0], ',', x[1], ',', y[1], ',', rssi[1], ',', x[2], ',', y[2], ',', rssi[2])
                gen_rssi(x[0], y[0], rssi[0]-4, x[1], y[1], rssi[1]-4, x[2], y[2], rssi[2]-4)
                #gen_rssi(1.0,0.0,-65,1.0,1.0,-65,0.0,0.0,-65)

                

        #print("Client disconnectted - Closing connection") #if the client disconnects
        client.close()