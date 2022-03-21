import serial
import keyboard

COM = 'COM5'
bodeRate = 9600
mm32 = serial.Serial(COM,bodeRate,timeout=0.5)
if mm32.isOpen():
    print("serial open success")
data = mm32.readline()

if __name__ == '__main__':
    readFlag=False
    runFlag=True
    print("按空格输出串口数据，按Esc退出")
    while runFlag:
        if(keyboard.is_pressed(' ')):
            readFlag=not readFlag
            while(keyboard.is_pressed(' ')):
                pass
        if(keyboard.is_pressed('esc')):
            runFlag=False
        if readFlag:
            data = mm32.readline()
            print(data.decode())

    mm32.close()
