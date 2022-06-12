from uart import openCom
import serial
import numpy as np
import cv2

PIC_LINE = 60
PIC_COL = 94
pic_FLAG_BYTES=[0xff,0xf0,0xf0,0xf0, 0x00,0xff]
def judgeStart(com:serial.Serial)->bool:
    for i in pic_FLAG_BYTES:
        tmp = com.read().hex()
        if len(tmp)>1 and (int(tmp,16) != i):
            return False
    global PIC_LINE, PIC_COL
    tmp = com.read().hex()
    while(len(tmp)<=1):
        tmp = com.read().hex()
    PIC_LINE = (int(tmp,16))
    tmp = com.read().hex()
    while(len(tmp)<=1):
        tmp = com.read().hex()
    PIC_COL = (int(tmp,16))
    return True
        
    
if __name__=='__main__':
    mm32 = openCom('com13',115200)
    mm32.write(bytes([0x97, 0x02, 0xf0,0xa5]))
    while(True):
        if (judgeStart(mm32)):
            data = []
            for i in range(PIC_COL*PIC_LINE):
                tmp = mm32.read().hex()
                while(len(tmp)<=1):
                    tmp = mm32.read().hex()
                data.append(int(tmp,16))
            pic=[]
            for i in range(0,PIC_LINE*PIC_COL,PIC_COL):
                pic.append(data[i:i+PIC_COL])
            pic = np.array(pic,dtype=np.uint8)
            pic = cv2.flip(pic,1)
            pic = cv2.resize(pic,dsize=None,fx=4,fy=4,interpolation=cv2.INTER_LINEAR)
            cv2.imshow("img",pic)
            ch = cv2.waitKey(2)
            if ch == ord('q'):
                break
