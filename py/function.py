from math import exp
import matplotlib.pyplot as plt
import numpy as np


def f(x):
    return(exp(50/(x))-1)/(exp(50/(x*x))+1)


F_NUM = 0.08


def getBellFunc(val):
    if (val > -6 and val < 6):
        return 1
    if (val > 0):
        val = -val
    return (exp(F_NUM * val) - 1) / (exp(F_NUM * val) + 1) + 1


x = np.arange(-61, 60, 1)
y = []
for i in x:
    y.append(getBellFunc(i))
y = np.array(y)
plt.plot(x, y)
plt.show()
