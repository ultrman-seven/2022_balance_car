from img_analyze import getData, getXYZ
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

K_TH =  -1.2624850533711323
if __name__ == '__main__':
    fig = plt.figure(0)
    ax = Axes3D(fig)
    x, y, z = getXYZ('img_posi01.txt')
    ax.scatter3D(z, y, x)
    t = np.array([5.38045091e3, - 1.17231739e6, 6.71794421e7])
    one = np.ones(len(x))
    X = np.c_[one,(z-K_TH*y)**-1,(z-K_TH*y)**-2]
    r = X@t
    ax.scatter3D(z, y, r)
    plt.show()

