import numpy as np

def main():
  print("this code will calculate the x and y values of a point to use for trilateration")
  print("enter the coordinates of the 3 beacons and the range from each beacon to use seperated by commas, like this: x1,y1,r1,x2,y2,r2,x3,y3,r2")
  #user_input = input()
  #user_data = user_input.split(",")
  beacon_data=[2,2,2,4,3,2,3,7,3] #the beacons with the strongest 3 signals, fromcsv file. also includd is the calculated sitance from each
  xy = get_pqr(beacon_data)
  print(xy)

#this function finds the p q and r values to trilaterate using a given 3 beacons
def get_pqr(beacon_data):
    x1 = beacon_data[0] #these beeacon coords will normally be read in from a .csv file
    y1 = beacon_data[1]
    x2 = beacon_data[3]
    y2 = beacon_data[4]
    x3 = beacon_data[6]
    y3 = beacon_data[7]
    r1 = beacon_data[2]
    r2 = beacon_data[5]
    r3 = beacon_data[8]

    '''
    calculate p, q and r. Assume beacon (x1,y1) is origin, (x2, y2) is at (0,p) and beacon (x3, y3) is at (q,r)
    which means that the coordinates found for x and y willl need to be added to coordinates (x1 y1) since x and y found are relative
    '''

    p = np.sqrt((x2-x1)**2 + (y2-y1)**2) 
    m = np.sqrt((y3-y1)**2 + (x3-x1)**2) 
    n = np.sqrt((y3-y2)**2 + (x3-x2)**2)
    q = (n**2 - m**2 - p**2)/(-2*p)
    r = np.sqrt(m**2 - q**2)
    #print("m is",m)
    #print("n is",n)
    #print("p is:",p)
    #print("q is:",q)
    #print("r is:", r)

    #find the x and y relative to point (x1,y1)
    x = (r1**2 - r2**2 + p**2)/(2*p)
    y = ((r1**2 - r2**2 + q**2 + r**2)/(2*r))-((q/r)*x)

    #now add x1 and y1 to get overall coordinates
    x = x + x1
    y = y + y1

    return(x,y)



if __name__== "__main__":
  main()