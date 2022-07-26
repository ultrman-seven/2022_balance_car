from matplotlib import pyplot as plt
import numpy as np


# 摄像头畸变矫正表计算

def getDat(name):
    dat = []
    with open(name) as f:
        for a in f.readlines():
            dat.append(float(a))
    return dat

PIC_COL = 94
MAX_PIC_COL_NUM = 47
if __name__ == '__main__':
    pixel = getDat('tmp/pix.txt')
    distance = getDat('tmp/dis.txt')

    tmp = zip(pixel, distance)
    tmp = sorted(tmp, key=lambda x: x[0])
    pixel = []
    distance = []
    for p, d in tmp:
        pixel.append(p)
        distance.append(d)

    x = []
    y = []
    for p in pixel:
        # x.append([a**(-6), a**(-5), a**(-4), a**(-3), a**(-2), a**(-1), 1])
        x.append([1, p, p**2, p**3, p**4])

    for d in distance:
        y.append([d])

    y = np.array(y)
    x = np.array(x)
    # print(x.shape)

    distance = np.array(distance)
    pixel = np.array(pixel)

    A = (x.T)@x
    B = (x.T)@y
    theta = np.linalg.solve(A, B)

    x = np.arange(0,MAX_PIC_COL_NUM)
    x = np.c_[x**0,x,x**2,x**3,x**4]
    # x = x.T
    result = x@theta
    print(list(theta))
    # plt.plot(area, bright)
    plt.scatter(pixel, distance)
    plt.plot(np.arange(0,MAX_PIC_COL_NUM), result, 'r')
    plt.show()

    with open('tmp/jb.cpp','w') as f:
        f.write(f'''#include "common.h"

const uint8_t distortionList[{MAX_PIC_COL_NUM}] = {'{'}
    ''')
        for r in result:
            f.write(f'{r[0]*10:.2}')
            if(r != result[-1]):
                f.write(',')
        f.write('\r\n};')