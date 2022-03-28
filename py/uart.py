import serial
import keyboard

def openCom(comName:str, bodeRate:int)->serial.Serial:
    com = serial.Serial(comName,bodeRate,timeout=0.5)
    if com.isOpen():
        print("serial open success")
        # data = com.readline()
        return com

# mm32 = openCom('com17',115200)
# print(mm32.read(10).hex())

# if __name__ == '__main__':
#     readFlag=False
#     runFlag=True
#     mm32 = openCom('com6',115200)
#     print("按空格输出串口数据，按Esc退出")
#     while runFlag:
#         if(keyboard.is_pressed(' ')):
#             readFlag=not readFlag
#             while(keyboard.is_pressed(' ')):
#                 pass
#         if(keyboard.is_pressed('esc')):
#             runFlag=False
#         if readFlag:
#             data = mm32.readline()
#             print(data.decode())

#     mm32.close()
