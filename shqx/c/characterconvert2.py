#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import codecs
import chardet

filenumbers = 0
abnormals = 0
def list_folders_files(path):
    """
    返回 "文件夹" 和 "文件" 名字
    :param path: "文件夹"和"文件"所在的路径
    :return:  (list_folders, list_files)
            :list_folders: 文件夹
            :list_files: 文件
    """
    list_folders = []
    list_files = []
    for file in os.listdir(path):
        file_path = os.path.join(path, file)
        if os.path.isdir(file_path):
            list_folders.append(file)
        else:
            list_files.append(file)
    return (list_folders, list_files)

def convert(file, in_enc="GBK", out_enc="UTF-8"):
    """
    该程序用于将目录下的文件从指定格式转换到指定格式，默认的是GBK转到UTF-8
    :param file:    文件路径
    :param in_enc:  输入文件格式
    :param out_enc: 输出文件格式
    :return:
    """
    global filenumbers
    global abnormals
    try:
        in_enc = in_enc.upper()
        out_enc = out_enc.upper()
        if (in_enc == out_enc):
            print('Consistent with the original character set(%s)' % out_enc)
            return 0;
    except AttributeError as err:
        in_enc = "ASCII"
        print('%s character set is NoneType' % file.split('/')[-1])
        abnormals += 1
    try:
        print("convert [ " + file.split('/')[-1] + " ].....From " + in_enc + " --> " + out_enc) #若为Windows系统, 请将'/'修改为'\\'
        filenumbers += 1
        f = codecs.open(file, 'r', in_enc, "ignore")
        new_content = f.read()
        codecs.open(file, 'w', out_enc).write(new_content)
    except IOError as err:
        print("I/O error: {0}".format(err))
# 将路径下面的所有文件，从原来的格式变为UTF-8的格式
if __name__ == "__main__":
    path = r'/ProDir/public/freecplus-1.0.0/freecplus'  #只要满足形式，一般只需改变文件夹的路径即可
    (list_folders, list_files) = list_folders_files(path)

    print("Path: " + path)
    for fileName in list_files:
        filePath = path + '/' + fileName   #若为Windows系统, 请将'/'修改为'\\'
        with open(filePath, "rb") as f:
            data = f.read()
            codeType = chardet.detect(data)['encoding']
            convert(filePath, codeType, 'UTF-8')
    print('The encoding format of the unknown exception is automatically considered to be ASCCI encoding, and is not unprocessed.')
    print('total = %d, unnormals = %d' % (filenumbers, abnormals))
