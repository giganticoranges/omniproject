import sys
from io import StringIO
import pylab
import pandas as pd
import pandas


x = 0.6
y = 0.8

df = pandas.read_csv('beacon_data.csv')
print(df)

for index, row in df.iterrows():
    pylab.plot(row['x'], row['y'], 'rs')

pylab.plot(x, y, 'bs')
    
pylab.show()
