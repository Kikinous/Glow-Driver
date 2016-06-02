#!/usr/bin/env python
import pandas as pd
from scipy.optimize import curve_fit
import matplotlib.pylab as pypl
import numpy as np

DEBUG = False

df = pd.read_csv('varistor_10k.txt',sep=",",header=1,dtype={'T': np.float32, 'RsurR25': np.float32,'alpha': np.float32})
print "data frame loaded : "
print df

T = df.iloc[:,0].values
T = T + 273.15
R = df.iloc[:,1].values
if DEBUG == True:
    print "T"
    print T
    print "R"
    print R



def func(x,T0,B):  # x est (R / R25C)
    UNsurT = 1./T0 + 1./B * np.log(x)
    return 1./(UNsurT)

T_debut     = 0
T_frontiere = T.size  - 3
print "T_debut     = " + str(T[T_debut]     - 273.15)
print "T_frontiere = " + str(T[T_frontiere] - 273.15)
T1 = T[T_debut:T_frontiere]
R1 = R[T_debut:T_frontiere]

popt1, pcov1 = curve_fit(func,R1,T1,bounds=([295,100], [305, 10000]))


r1= np.arange( R1[0] , R1[T1.size-1]-0.01 , -0.001 ) 
t1 = func(r1,popt1[0],popt1[1]) 
print "Steinhart coefficients T0 et B" 
print popt1

pypl.scatter(T-273.15,R,color='black')
pypl.xlabel('T')
pypl.ylabel('R')
pypl.semilogy(t1-273.15,r1,color="red")

if False:
    pypl.savefig('fit_polaire.pdf',format='pdf',bbox_inches='tight')
pypl.grid()
pypl.show()

exit()

# open the file again for writing
f = open('CLCDfit.csv', 'w')
f.write(str(popt[0]))
f.write(",")
f.write(str(popt[1]))
f.write(",")
f.write(str(popt[2]))
f.write(",")
f.write(str(popt[3]))
f.write(",")
f.write(str(popt[4]))
f.write("\n")
#f.write("REX fuselag cone 35mm\n")
# write the original contents
f.close()
