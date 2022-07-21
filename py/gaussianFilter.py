import cv2

def getGaussianFilterKernel(size=3, sigma=1):
    a = cv2.getGaussianKernel(size, sigma)
    return a*a.T

def putGaussianCore2D():
    data = getGaussianFilterKernel(3, 1.4)*10000
    with open('./tmp/g.cpp','w') as g:
        g.write('#include "common.h"\nuint16_t gaussian[][3] = {\n\t')
        for l,j in zip(data,range(len(data))):
            g.write('\t{')
            for d,i in zip(l,range(len(l))):
                g.write(str(int(d)))
                if(i<len(l)-1):
                    g.write(', ')
            if(j<len(l)-1):
                g.write('},\n')
            else:
                g.write('}\n')
            
        g.write('};')

def putGaussianCore1D():
    print(cv2.getGaussianKernel(3,1.4))

if __name__ == "__main__":
    putGaussianCore1D()
