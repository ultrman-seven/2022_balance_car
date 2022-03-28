from uart import openCom
import serial
import numpy as np
import cv2

PIC_LINE = 60
PIC_COL = 94
pic_FLAG_BYTES=[0xff,0xf0,0xf0,0xf0, 0x00,0xff,0x00,0xff]
def judgeStart(com:serial.Serial)->bool:
    for i in range(8):
        tmp = com.read().hex()
        if len(tmp)>1 and (int(tmp,16) != pic_FLAG_BYTES[i]):
            return False
    return True
        
    
if __name__=='__main__':
    mm32 = openCom('com17',115200)
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
            cv2.imshow("img",pic)
            ch = cv2.waitKey(2)
            if ch == ord('q'):
                break
