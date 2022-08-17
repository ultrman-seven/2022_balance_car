from uart import openCom

CMDs = ['H', 'mpu', 'pic', 'm', 'pid', 'ang',
        'turn', 'balance', 'g_pid', 'f', 'v', 'gf', 'gv', 'run']

PIDs = ['b', 's', 't']

Fs = ['lamp', 'speed', 'para']
Vs = ['balance', 'speed', 'pamul', 'ypara', 'ylamp', 'ydie', 'ycs',
      'sdie', 'basemul', 'decmul', 'spmul', 'tdie', 'tmiss', 'tre', 'sre']


def main():
    car = openCom('com3', 115200)
    while True:
        cmd = input().split('--')
        c = cmd[0]
        if c == 'q':
            break
        if c in CMDs:
            idx = CMDs.index(c)
        else:
            break
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
        elif c == 'run':
            dat = [0x97, idx, 0xf0, 0xa5]
            car.write(bytes(dat))
        elif c == 'f':
            dat = [0x97, idx]
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


if __name__ == '__main__':
    main()
