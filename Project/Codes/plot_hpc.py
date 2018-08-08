fo1 = open("logs_up.csv")
dic = {}
while True:
    line1 = fo1.readline() 
    temp1 = line1.split(',')
    if len(temp1) == 1:
        break
    size = temp1[0]
    core = temp1[1]
    time = temp1[3]
    if core not in dic.keys():
        dic[core] = {}
    temp = dic[core]
    temp[size] = time
    dic[core] = temp

speedUp = {}
karpFlatt = {}
efficiency ={}
for core in dic.keys():
    if int(core)<2:
        continue
    if core not in karpFlatt.keys():
        karpFlatt[core] = {}
    temp = karpFlatt[core]
    temp2 = dic[core]
    for size in temp2.keys():
        tempSpeedUp = float(dic['0'][size])/float(temp2[size])
        a = float(1/tempSpeedUp)
        b = float(1/float(core))
        c  = a-b
        d = 1-b;        
        tempKF = float(c/d)
        temp[size] = tempKF

import matplotlib.pyplot as plt

fig = plt.figure(1)

import numpy as np

for core in karpFlatt.keys():
        n = karpFlatt[core].keys()
        temp = []
        temp2 = []
        for i in n:
            temp.append(int(i))
            temp2.append(float(karpFlatt[core][i]))
        line, = plt.semilogx(temp,temp2,label='$%i Cores$'%int(core));

plt.legend(loc='best', bbox_to_anchor=(1.04,1))
plt.xlabel('Problem Size')
plt.ylabel('Karp-Flatt')
plt.title('Problem Size vs Karp-Flatt')
plt.show()

"""fig = plt.figure(2)
ax1 = plt.subplot(111)

for i in xrange(len(p)):
	line1, = ax1.semilogx(n,karpFlatt[i][1:4],label='$%i Cores$'%p[i]);

ax1.legend(loc='best', bbox_to_anchor=(1.04,1))
plt.xlabel('Problem Size')
plt.ylabel('karpFlatt')
plt.title('Problem Size vs karpFlatt')
#plt.show()

fig = plt.figure(3)
ax1 = plt.subplot(111)

for i in xrange(len(p)):
	line1, = ax1.semilogx(n,efficiency[i][1:4],label='$%i Cores$'%p[i]);

ax1.legend(loc='best', bbox_to_anchor=(1.04,1))
plt.xlabel('Problem Size')
plt.ylabel('Efficiency')
plt.title('Problem Size vs efficiency')
plt.show()
"""
