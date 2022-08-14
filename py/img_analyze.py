import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


def getData(name, date='8013/'):
    with open('py/dat/'+date+name) as f:
        # ix,iy,px,py,p=[],[],[],[],[]
        data = [[], [], [], [], []]
        for l in f.readlines()[:-1]:
            # ixn,iyn,pxn,pyn,pn = l.split(',')
            # ix.append(float(ixn.split('=')[-1]))
            n = 0
            for d in l.split(','):
                data[n].append(float(d.split('=')[-1]))
                n += 1
    return data


def getXYZ(*name):
    '''
    **参数**:    文件名

    **返回值**:
    1. x: 距离
    2. y: 俯仰角
    3. z: 灯在图像中的纵坐标
    '''
    ixs, iys, ps, pys = [], [], [], []
    for n in name:
        ix, iy, px, py, p = getData(n)
        ixs += px
        iys += py
        ps += p
        pys += iy
    x0 = np.array(ixs)
    y0 = np.array(iys)

    x = np.sqrt(x0**2+y0**2)

    y = np.array(ps)
    z = np.array(pys)
    return [x, y, z]


def least_square(X: np.array, Y: np.array):
    A = (X.T)@X
    B = (X.T)@Y
    theta = np.linalg.solve(A, B)
    return theta


def sim_pitch(p, y, ax):
    p = np.array(p)
    y = np.array(y)
    one = np.ones(len(p))
    X = np.c_[one, p]
    theta = least_square(X, y)
    r = X@theta
    ax.scatter(p, r)
    return theta


if __name__ == '__main__':
    # plt.rcParams['font.sans-serif']=['SimHei'] #用来正常显示中文标签
    # plt.rcParams['axes.unicode_minus'] = False #用来正常显示负号
    # ix, iy, px, py, p = getData('img_posi05.txt')
    # x0 = np.array(ix)
    # y0 = np.array(iy)

    # x = np.sqrt(x0**2+y0**2)
    # y = np.array(p)
    # z = np.array(py)

    fig = plt.figure(0)
    ax = Axes3D(fig)
    x, y, z = getXYZ('img_posi06.txt')
    # ax.scatter3D(x, y, z)
    ax.scatter3D(z, y, x)
    # x, y, z = getXYZ('img_posi05.txt', 'img_posi07.txt', 'img_posi08.txt')
    x, y, z = getXYZ('img_posi05.txt')
    ax.scatter3D(z, y, x)
    x, y, z = getXYZ('img_posi07.txt')
    ax.scatter3D(z, y, x)
    x, y, z = getXYZ('img_posi08.txt')
    ax.scatter3D(z, y, x)

    f1 = plt.figure(1)
    # plt.title(u'距离恒定时, $\\theta$与灯坐标关系')
    # plt.title('distance is const, theta is variable')
    x, y, z = getXYZ('img_posi06.txt')
    ax1 = f1.add_subplot(3, 1, 1)
    ax1.scatter(y, z)
    plt.xlabel('pitch')
    plt.ylabel('y')
    ax2 = f1.add_subplot(3, 1, 2)
    ax2.scatter(x, y)
    plt.xlabel('distance')
    plt.ylabel('pitch')
    ax3 = f1.add_subplot(3, 1, 3)
    ax3.scatter(x, z)
    plt.xlabel('distance')
    plt.ylabel('y')
    z1, z2, z3, z4, y1, y2, y3, y4 = [], [], [], [], [], [], [], []
    for i in range(len(x)):
        if(x[i] > 400 and x[i] < 600):
            z1.append(z[i])
            y1.append(y[i])
            continue
        if(x[i] > 600 and x[i] < 800):
            z2.append(z[i])
            y2.append(y[i])
            continue
        if(x[i] > 1000 and x[i] < 1200):
            z3.append(z[i])
            y3.append(y[i])
            continue
        if(x[i] > 1200 and x[i] < 1400):
            z4.append(z[i])
            y4.append(y[i])
            continue
    t1 = sim_pitch(y1, z1, ax1)
    t2 = sim_pitch(y2, z2, ax1)
    t3 = sim_pitch(y3, z3, ax1)
    t4 = sim_pitch(y4, z4, ax1)
    print('t1:', t1)
    print('t2:', t2)
    print('t3:', t3)
    print('t4:', t4)
    k_theta = ((t1+t2+t3+t4)/4)[-1]
    print('pitch-y 平均斜率:', k_theta)

    f2 = plt.figure(2)
    # plt.title('$\\theta$恒定时, 距离与灯坐标关系')
    # plt.title('θ is const, distance is variable')
    x, y, z = getXYZ('img_posi05.txt')
    ax1 = f2.add_subplot(1, 3, 1)
    ax1.scatter(y, z)
    plt.xlabel('pitch')
    plt.ylabel('y')
    ax2 = f2.add_subplot(1, 3, 2)
    ax2.scatter(y, x)
    plt.ylabel('distance')
    plt.xlabel('pitch')
    ax3 = f2.add_subplot(1, 3, 3)
    ax3.scatter(x, z)
    plt.xlabel('distance')
    plt.ylabel('y')
    one = np.ones(len(x))
    ave_pitch = sum(y)/len(y)
    print('平均俯仰角: ', ave_pitch)
    # X = np.c_[one,z**-1,z**-2, z, z**2, z**3,z**4]*math.cos(ave_pitch*math.pi/180)
    X = np.c_[one, (z-k_theta*ave_pitch)**-1, (z-k_theta*ave_pitch)**-2]
    theta = least_square(X, x)
    print('距离：', theta)
    t1 = theta
    r = X@theta
    ax3.scatter(r, z)
    ax.scatter(z, y, r)

    f3 = plt.figure(3)
    # plt.title('$\\theta$恒定时, 距离与灯坐标关系')
    # plt.title('θ is const, distance is variable')
    x, y, z = getXYZ('img_posi07.txt')
    ax1 = f3.add_subplot(1, 3, 1)
    ax1.scatter(y, z)
    plt.xlabel('pitch')
    plt.ylabel('y')
    ax2 = f3.add_subplot(1, 3, 2)
    ax2.scatter(y, x)
    plt.ylabel('distance')
    plt.xlabel('pitch')
    ax3 = f3.add_subplot(1, 3, 3)
    ax3.scatter(x, z)
    plt.xlabel('distance')
    plt.ylabel('y')
    one = np.ones(len(x))
    ave_pitch = sum(y)/len(y)
    print('平均俯仰角2: ', ave_pitch)
    # X = np.c_[one,z**-1,z**-2, z, z**2, z**3,z**4]*math.cos(ave_pitch*math.pi/180)
    X = np.c_[one, (z-k_theta*ave_pitch)**-1, (z-k_theta*ave_pitch)**-2]
    theta = least_square(X, x)
    t2 = theta
    print('距离2:', theta)
    r = X@theta
    ax3.scatter(r, z)
    ax.scatter(z, y, r)

    f4 = plt.figure(4)
    # plt.title('$\\theta$恒定时, 距离与灯坐标关系')
    # plt.title('θ is const, distance is variable')
    x, y, z = getXYZ('img_posi08.txt')
    ax1 = f4.add_subplot(1, 3, 1)
    ax1.scatter(y, z)
    plt.xlabel('pitch')
    plt.ylabel('y')
    ax2 = f4.add_subplot(1, 3, 2)
    ax2.scatter(y, x)
    plt.ylabel('distance')
    plt.xlabel('pitch')
    ax3 = f4.add_subplot(1, 3, 3)
    ax3.scatter(x, z)
    plt.xlabel('distance')
    plt.ylabel('y')
    one = np.ones(len(x))
    ave_pitch = sum(y)/len(y)
    print('平均俯仰角3: ', ave_pitch)
    # X = np.c_[one,z**-1,z**-2, z, z**2, z**3,z**4]*math.cos(ave_pitch*math.pi/180)
    X = np.c_[one, (z-k_theta*ave_pitch)**-1, (z-k_theta*ave_pitch)**-2]
    theta = least_square(X, x)
    t3 = theta
    print('距离3:', theta)
    r = X@theta
    ax3.scatter(r, z)
    ax.scatter(z, y, r)

    t = (t1+t2+t3)/3
    print('平均 t: ', t)
    # x, y, z = getXYZ('img_posi08.txt')
    # ave_pitch = sum(y)/len(y)
    # print('平均俯仰角3: ',ave_pitch)
    # X = np.c_[one,(z-k_theta*ave_pitch)**-1,(z-k_theta*ave_pitch)**-2]
    # r = X@theta
    # ax.scatter(z,y,r)

    x, y, z = getXYZ('img_posi06.txt', 'img_posi05.txt',
                     'img_posi07.txt', 'img_posi08.txt')
    one = np.ones(len(x))
    X = np.c_[one, z, z**2, z**-1, z**-2, z**-3, y]
    Y = x
    theta = least_square(X, Y)
    # print(theta)

    # tz = []
    # ty = []
    # for i in range(len(z)):
    #     if(i%10 == 0):
    #         tz.append(z[i])
    #         ty.append(y[i])
    # z = np.array(tz)
    # y = np.array(ty)
    z = np.arange(31.0, 120.0, 0.8)[:99]
    y = np.arange(0.5, 36, 0.3)[:99]
    px, py = np.meshgrid(z, y)
    x = px[0]
    one = np.ones(len(z))
    x = np.c_[one, x, x**2, x**-1, x**-2, x**-3]
    r = []
    for ey in py[0]:
        y = one*ey
        X = np.c_[x, y]
        r.append(X@theta)
    r = np.array(r)
    # ax.scatter(r, py, px)
    # print(r.shape)
    # print(px.shape)

    x = px[0]
    r = []
    for ey in py[0]:
        y = one*ey
        X = np.c_[one,  (x-k_theta*y)**-1, (x-k_theta*y)**-2]
        r.append(X@t)
    r = np.array(r)
    # ax.scatter(px,py, r)
    plt.show()
