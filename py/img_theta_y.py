from img_analyze import getData,getXYZ,least_square
import numpy as np
import matplotlib.pyplot as plt

if __name__ == '__main__':
    ix, iy, px, py, p = getData('img_posi01.txt','0815/')
    iy = np.array(iy)
    p = np.array(p)
    plt.figure()
    plt.scatter(iy,p)

    one = np.ones(len(p))
    X = np.c_[one,p]
    theta = least_square(X,iy)
    print(theta)
    r = X@theta

    plt.scatter(r,p)
    plt.show()
