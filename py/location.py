from matplotlib import pyplot as plt
from uart import openCom
import serial
import numpy as np

def getData(com):
    data = com.read(5000)
    while data[0] != 120:
        data = data[1:]
    while data[-1] != 10:
        data = data[:-1]

    xs,ys=[],[]
    data = data.decode('ascii')
    for l in data.split('\n')[:-1]:
        print(l)
        x,y = l.split('y')
        xs.append(float(x[1:]))
        ys.append(float(y))
    # print(xs,ys)
    return [xs,ys]


def main():
    mm32 = openCom('com20', 115200)
    plt.figure()
    plt.ion()
    while True:
        xs,ys = getData(mm32)
        xs = np.array(xs)
        ys = np.array(ys)
        plt.cla
        plt.scatter(xs,ys)
        plt.pause(0.01)
    plt.ioff()
    plt.show()
    

if __name__ == '__main__':
    main()
# def judgeStart(com: serial.Serial) -> bool:
#     tmp = com.read()
#     if tmp.isascii():
#         print(tmp.decode('ascii'))
#     if not tmp.isalpha():
#         return False
#     if (tmp.decode('ascii') != 'x'):
#         return False
#     return True


# if __name__ == '__main__':
#     mm32 = openCom('com5', 115200)
#     # plt.ion()
#     # plt.figure()
#     xs = []
#     ys = []
#     while(True):
#         if (judgeStart(mm32)):
#             # if True:
#             data = b''
    # n = 0
    # # x
    # while True:
    #     tmp = mm32.read()
    #     if tmp.isdigit():
    #         data += tmp
    #     elif tmp.isalpha():
    #         if tmp.decode('ascii') == '.':
    #             data += tmp
    #         elif tmp.decode('ascii') == 'y':
    #             break
    # xs.append(int(data.decode('ascii')))
    # while True:
    #     tmp = mm32.read()
    #     if tmp.isdigit():
    #         data += tmp
    #     elif tmp.isalpha():
    #         if tmp.decode('ascii') == '.':
    #             data += tmp
    #         elif tmp.decode('ascii') == '\n':
    #             break
    # ys.append(int(data.decode('ascii')))
    # print(xs[-1], ys[-1])
