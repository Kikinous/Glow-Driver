#!/usr/bin/env python
import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

'''
    commande
    ./plot_logs.py log.csv    
'''
fichier = sys.argv[1]

resultats = pd.read_csv(fichier,delimiter=',',engine='python',header=0)
resultats.convert_objects(convert_numeric=True)

temperature = resultats['Temperature'].dropna().as_matrix()
courant     = resultats['Courant'].dropna().as_matrix()
t = np.arange(0,temperature.size,1)     

plt.plot(t[0:200],temperature[0:200])
plt.savefig("log.png",format='png',bbox_inches='tight')
plt.show()
