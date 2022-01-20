import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

data1 = pd.read_csv("t_50ms.out")
data1.columns=['Bitrate']

data2 = pd.read_csv("t_50ms.changed.out")
data2.columns=['Bitrate']


data1['New_ID'] = range(0,len(data1))
data2['New_ID'] = range(len(data1),len(data1)+len(data2))



plt.xlabel("50ms intervals")
plt.ylabel("Bitrate measured")
plt.plot(data1['New_ID'],data1['Bitrate'],color='Blue')
plt.plot(data2['New_ID'],data2['Bitrate'],color='Red')
plt.show()
