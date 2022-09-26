import matplotlib.pyplot as plt
import numpy as np

file_out = open("output.txt",  "r", encoding="utf-8")

time = 0
l_time = []
l_vel_des = []
l_vel_cur = []
l_err = []


for raw in file_out:
	 raw = raw.strip().split(",")
	 try:
		 l_err.append(int(raw[3]))
		 l_vel_des.append(int(raw[2]))
		 l_vel_cur.append(int(raw[1]))
		 l_time.append(time)
		 time += 1
	 except:
			continue
	 
xpoints = np.array(l_time)
ypoints = np.array(l_vel_cur)
ypoints1 = np.array(l_vel_des)
ypoints2 = np.array(l_err)

plt.plot(xpoints, ypoints, 'o')
plt.plot(xpoints, ypoints1, 'o')
plt.plot(xpoints, ypoints2, 'o')
plt.show()
	 
	 

	 	
	
