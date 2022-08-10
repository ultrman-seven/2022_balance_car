import os
from shutil import copyfile

fileTypes = {'cpp': 8, 's': 2, 'c': 1}

# location = '../../'
# dirL = './examples/'

project_target_location_dirs = []
dirL = './'
location = '../'
for t in project_target_location_dirs:
    dirL += t + '/'
    location += '../'


def addIncPath(f, n: str):
    if os.path.isdir(n):
        f.write(location + n + ';')
        print(location + n)
        for d in os.listdir(n):
            addIncPath(f, n+'/'+d)


def addFiles(f, f_type, f_dir: str):
    if (os.path.isdir(f_dir)):
        for dat in os.listdir(f_dir):
            if(os.path.isdir(f_dir+'/'+dat)):
                addFiles(f, f_type, f_dir+'/'+dat)
            elif (f_type == dat.split('.')[-1]):
                f.write(' '*12+'<File>')
                f.write(' '*14+'<FileName>'+dat+'</FileName>')
                f.write(' '*14+'<FileType>' +
                        str(fileTypes[f_type])+'</FileType>')
                f.write(' '*14+'<FilePath>'+location +
                        f_dir+'/'+dat+'</FilePath>')
                f.write(' '*12+'</File>')


def addGroup(f, g_name, f_info: dict):
    f.write(' '*8+'<Group>\n')
    f.write(' '*10+'<GroupName>' + g_name + '</GroupName>\n')
    f.write(' '*10+'<Files>')
    for fdir in f_info:
        addFiles(f, f_info[fdir], fdir)
    f.write(' '*10+'</Files>')
    f.write(' '*8+'</Group>\n')


def addFileContent(f_name, f):
    with open(f_name) as file:
        data = file.read()
        f.write(data)


def create_project(name):
    if not os.path.exists(dirL + name):
        os.mkdir(dirL + name)
        os.mkdir(dirL + name+'/src')
        # os.system('copy build/mainTemplate.cpp '+dirL + name+'/src/main.cpp')
        copyfile('build/mainTemplate.cpp', dirL + name+'/src/main.cpp')
        copyfile('build/generalInitTemplate.cpp', dirL + name+'/src/generalInit.cpp')
        copyfile('build/genObjTemplate.hpp', dirL + name+'/src/genObj.hpp')

    # with open(dirL + name+'/src/main.cpp', 'w') as f:
    #     f.write('#include "common.h"\n\n')
    #     f.write('void main(void)\n{\n\twhile(1);\n}\n')

    with open(dirL + name+'/'+name+'.uvprojx', 'w') as f:
        addFileContent('./build/start2name.txt', f)
        f.write(name)

        addFileContent('./build/name2inc.txt', f)
        print('include path:')
        addIncPath(f, 'CMSIS')
        addIncPath(f, 'MM32F327x')
        addIncPath(f, 'WyCppHal')
        addIncPath(f, 'WyLib')
        f.write('./;')
        f.write('./src')

        addFileContent('./build/inc2group.txt', f)
        addGroup(f, 'MM_HAL', {'MM32F327x/HAL_lib': 'cpp'})
        addGroup(f, 'START_UP', {
                 'MM32F327x/Source': 'cpp', 'MM32F327x': 's'})
        addGroup(f, 'WY_HAL', {'WyCppHal': 'cpp'})
        addGroup(f, 'WY_Hardware', {'WyLib': 'cpp'})
        addGroup(f, 'USER', {dirL + name+'/src/': 'cpp'})

        addFileContent('./build/group2end.txt', f)
        # f.write(name)

        addFileContent('./build/end.txt', f)


if __name__ == '__main__':
    n = input("请输入项目名称: ")
    create_project(n)
