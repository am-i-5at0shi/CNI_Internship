import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

data = pd.read_csv("ffmpeg_cfr_30fps.log", delimiter = " ")
#data.drop(data.columns[2], axis=1,inplace=True)

data.columns=["Frame","Megabits"]

#print(data)
step=3	
df = data.groupby(data["Megabits"].index // step).sum()
bitrate_multiplier = 10
df['Megabits'] = df['Megabits'].apply(lambda x: (x*bitrate_multiplier*8)/1000000)
print(df)

plt.plot(df['Megabits'],color="Red",label="abr")

'''data = pd.read_csv("abr_comparison.filtered", delimiter = " ")
#data.drop(data.columns[2], axis=1,inplace=True)

data.columns=["Frame","Megabits"]

#print(data)
step=3	
df = data.groupby(data["Megabits"].index // step).sum()
bitrate_multiplier = 10
df['Megabits'] = df['Megabits'].apply(lambda x: (x*bitrate_multiplier*8)/1000000)
print(df)

plt.plot(df['Megabits'],color="Blue", label="abr")'''

plt.xlabel("100mili-sec Intervals")
plt.ylabel("Bitrate")
plt.legend(loc="lower right")
plt.grid()
plt.show()
