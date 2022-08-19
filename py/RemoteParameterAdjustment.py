import os
import time
from uart import openCom

CMDs = ['H', 'mpu', 'pic', 'm', 'pid', 'ang', 'turn',
        'balance', 'g_pid', 'f', 'v', 'gf', 'gv', 'run', 'stop']

PIDs = ['b', 's', 't']

Fs = ['lamp', 'speed', 'para']
Vs = ['balance', 'speed', 'pamul', 'ypara', 'ykill', 'ydie', 'ycs', 'sdie', 'basemul',
      'decmul', 'spmul', 'tdie', 'tmiss', 'tre', 'sre', 'smiss', 'scen', 'skill','imul','xmodi']
helpList = {'cmd': CMDs, 'pid': PIDs, 'f': Fs,
            'fuzzy': Fs, 'v': Vs, 'variable': Vs}


def main():
    car = openCom('com3', 115200)
    f_name = './paraLog.txt'
    t = time.localtime()
    if os.path.exists(f_name):
        f_log = open(f_name, 'a', encoding='utf8')
    else:
        f_log = open(f_name, 'w', encoding='utf8')
    f_log.write(
        f'\n记录追加时间: {t.tm_year}年{t.tm_mon}月{t.tm_mday}日;\t{t.tm_hour}:{t.tm_min}:{t.tm_sec}\n')
    while True:
        cmd = input().split('--')
        c = cmd[0]
        if c == 'q':
            break
        if c == 'h':
            print(helpList[cmd[-1]])
            continue
        if c in CMDs:
            idx = CMDs.index(c)
        else:
            break
        if c == 'run' or c == 'stop' or c == 'H':
            dat = [0x97, idx, 0xf0, 0xa5]
            car.write(bytes(dat))
            continue
        f_log.write('--'.join(cmd))
        f_log.write('\n')
        if c == 'pid':
            dat = [0x97, idx]
            pid = PIDs.index(cmd[1])
            dat.append(pid)

            v = cmd[-1]
            for k in v.split(','):
                val = int(k) & 0xffff
                dat.append(val >> 8)
                dat.append(val & 0x00ff)
            dat.append(0xf0)
            dat.append(0xa5)
            print(dat)
            car.write(bytes(dat))
        elif c == 'f':
            dat = [0x97, idx]
            if cmd[1] == 'save':
                dat += [0xff, 0xf0, 0xa5]
                car.write(bytes(dat))
                continue
            if cmd[1] == 'load':
                dat += [0xfe, 0xf0, 0xa5]
                car.write(bytes(dat))
                continue
            f = Fs.index(cmd[1])
            dat.append(f)

            v = cmd[-1]
            v = v.split(',')
            dat.append(len(v))
            for ev in v:
                val = int(ev) & 0xffffffff
                dat.append((val >> 24) & 0x000000ff)
                dat.append((val >> 16) & 0x000000ff)
                dat.append((val >> 8) & 0x000000ff)
                dat.append(val & 0x000000ff)
            dat.append(0xf0)
            dat.append(0xa5)
            print(dat)
            car.write(bytes(dat))
        elif c == 'v':
            dat = [0x97, idx]
            if cmd[1] == 'save':
                dat += [0xff, 0xf0, 0xa5]
                car.write(bytes(dat))
                continue
            if cmd[1] == 'load':
                dat += [0xfe, 0xf0, 0xa5]
                car.write(bytes(dat))
                continue
            f = Vs.index(cmd[1])
            dat.append(f)

            val = int(cmd[-1]) & 0xffffffff
            dat.append((val >> 24) & 0x000000ff)
            dat.append((val >> 16) & 0x000000ff)
            dat.append((val >> 8) & 0x000000ff)
            dat.append(val & 0x000000ff)
            dat.append(0xf0)
            dat.append(0xa5)
            print(dat)
            car.write(bytes(dat))
    f_log.close()


if __name__ == '__main__':
    main()
