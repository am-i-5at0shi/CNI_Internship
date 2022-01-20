import matplotlib.pyplot as plt
# the file must be processed using "sort| uniq -c" to get cdf plot using this script

plt.rcParams.update({'font.size': 18})
plt.rcParams.update({'font.weight': 'bold'})

plt.xlabel("Delay in mS.")
plt.ylabel("Percentage")

with open('60fps/exp_4_60fps_filtered.out') as f:
	arr = [[float(x) for x in line.split()] for line in f]
	sum = 0
	for i in arr:
		sum = sum + i[0]
	cdf = []
	pkt_size = []
	cum = 0
	for i in arr:
		cum = cum + i[0]
		cdf.append(cum*100/sum)
		pkt_size.append(i[1])


plt.step(pkt_size, cdf, label = "ffmpeg_10M_nobuf_tc_10")

with open('60fps/exp_5_60fps_filtered.out') as f:
	arr = [[float(x) for x in line.split()] for line in f]
	sum = 0
	for i in arr:
		sum = sum + i[0]
	cdf = []
	pkt_size = []
	cum = 0
	for i in arr:
		cum = cum + i[0]
		cdf.append(cum*100/sum)
		pkt_size.append(i[1])

plt.step(pkt_size, cdf, label = "ffmpeg_10M_buf_5M_tc_10")

with open('60fps/exp_6_60fps_filtered.out') as f:
	arr = [[float(x) for x in line.split()] for line in f]
	sum = 0
	for i in arr:
		sum = sum + i[0]
	cdf = []
	pkt_size = []
	cum = 0
	for i in arr:
		cum = cum + i[0]
		cdf.append(cum*100/sum)
		pkt_size.append(i[1])

plt.step(pkt_size, cdf, label = "ffmpeg_10M_buf_10M_tc_10")


with open('60fps/exp_7_60fps_filtered.out') as f:
	arr = [[float(x) for x in line.split()] for line in f]
	sum = 0
	for i in arr:
		sum = sum + i[0]
	cdf = []
	pkt_size = []
	cum = 0
	for i in arr:
		cum = cum + i[0]
		cdf.append(cum*100/sum)
		pkt_size.append(i[1])

plt.step(pkt_size, cdf, label = "ffmpeg_10M_buf_100M_tc_10")


# giving a title to my graph
plt.title('CDF of Percentage of inter-frame delay', fontsize=18, fontweight='bold')

plt.grid()
# show a legend on the step 
plt.legend(loc = "lower right")
 
plt.xlim(0, 200)
# function to show the step
plt.show()
