# -*- coding: gbk -*-
#============================================================
# TEST1.PY                     -- by Dr. ZhuoQing 2022-07-19
#
# Note:
#============================================================

import math
from math import pi,sqrt
import matplotlib.pyplot as plt

#------------------------------------------------------------
posdim = [(100,325), (75, 200), (75,75), (425,200)]
start1 = (250, 22.5)

def routearg(r, s):
    global posdim
    lenall = 0
    angleall = 0

    lastangle = 0
    lastr = s
    for id,rr in enumerate(r):
        rrr = posdim[rr-1]
        deltax = rrr[0] - lastr[0]
        deltay = rrr[1] - lastr[1]
        lenall += sqrt(deltax**2 + deltay**2)

        angle = math.atan2(deltay, deltax)

        lastr = rrr

        if id > 0:
            deltaangle = angle - lastangle

            while deltaangle < -pi:
                deltaangle += pi
            while deltaangle > pi:
                deltaangle -= pi

            angleall += abs(deltaangle)

        lastangle = angle
    return lenall + angleall * 180/pi/4

#------------------------------------------------------------
'''
r = tspload('route18', 'r')

rscore = []

for id,rr in enumerate(r):
    rscore.append(routearg(rr[1:], start1))

    if id % 10000 == 0:
        printf(id)

tspsave('score', rscore=rscore)
'''
#------------------------------------------------------------

# rscore = tspload('score', 'rscore')

# plt.hist(rscore, 100)
# plt.xlabel("路径难易")
# plt.ylabel("出现频次")
plt.grid(True)
plt.tight_layout()
plt.show()

# exit()



#------------------------------------------------------------
def routerandall(r, endp, num):
    global routeall

#    printff(r, endp, num)
    if len(r) >= num - 1:
        rc = r.copy()
        rc.append(endp)
        routeall.append(rc)
        return


    rr = list(range(1,5))

    if r[-1] in rr:
        rr.remove(r[-1])

    if r[-1] == 1:
        if 2 in rr: rr.remove(2)

    if r[-1] == 2:
        if 1 in rr: rr.remove(1)
        if 3 in rr: rr.remove(3)

    if r[-1] == 3:
        if 2 in rr: rr.remove(2)

    if len(r) >= num - 2:
        if endp in rr:
            rr.remove(endp)

    for rrr in rr:
        rc = r.copy()
        rc.append(rrr)
        routerandall(rc, endp, num)


#------------------------------------------------------------
routeall = []
r = [-1]
routerandall(r, 4, 19)
# tspsave('route18', r = routeall)

#------------------------------------------------------------
print(len(routeall))
i=0
for r in routeall:
    if (1 in r and 2 in r and 3 in r and 4 in r):
        i+=1
        print(r)
        if i>=200:
            break


#------------------------------------------------------------
#        END OF FILE : TEST1.PY
#============================================================
