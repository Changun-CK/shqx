#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import codecs
import chardet

'''
此程序实现将目标目录下及其子目录下的全部文件转换为指定的编码格式, 转换之前尽量确保目录中全部为文本文件, 异常时放弃转换
请注意, 此转换可能会损坏二进制文件, 请谨慎使用(已经优化部分二进制文件或者库文件的字符集, 如: WINDOWS-和TIS-).
Please note that this conversion may damage the binary file. Please use it with caution.
'''
'全局变量'
filenumbers = 0 #成功转换的个数

def list_folders_files(path):
    """
    返回 "文件夹" 和 "文件" 名字
    :param path: "文件夹"和"文件"所在的路径
    :return: list_files
    :list_files: 文件
    """
    list_files = []
    for maindir, subdir, file_name_list in os.walk(path):
        for filename in file_name_list:
            file_path = os.path.join(maindir, filename)
            list_files.append(file_path)

    return list_files

def convert(file, in_enc="GBK", out_enc="UTF-8"):
    """
    该程序用于将指定目录及其子目录下的文件从指定格式转换到指定格式，默认为GBK转到UTF-8
    :param file:    文件路径
    :param in_enc:  输入文件格式
    :param out_enc: 输出文件格式
    :return:
    """
    global filenumbers
    try:
        in_enc = in_enc.upper()
		#若为WINDOWS和TIS开头的字符集放弃转换, 这是可执行文件和库文件的字符集
        if ((in_enc.split('-')[0] == "WINDOWS") or (in_enc.split('-')[0] == "TIS")):
            return 0
        out_enc = out_enc.upper()
        if (in_enc == out_enc):
            print('Consistent with the original character set(%s)' % out_enc)
            return 0
    except AttributeError as err:
        return 0
    try:
        print("convert [ " + file.split('/')[-1] + " ]...From " + in_enc + " --> " + out_enc) #若为Windows系统, 请将'/'修改为'\\'
        filenumbers += 1
        f = codecs.open(file, 'r', in_enc, "ignore")
        new_content = f.read()
        codecs.open(file, 'w', out_enc).write(new_content)
    except IOError as err:
        print("I/O error: {0}".format(err))

# 将路径下面的所有文件，从原来的格式变为UTF-8的格式
if __name__ == "__main__":
    path = r'/ProDir/public/c'  #只要满足形式, 只需改变文件夹的路径即可
    list_files = list_folders_files(path)

    print("Please note that this conversion may damage the binary file. Please use it with caution")
    print("Path: " + path)
    for fileName in list_files:
        with open(fileName, "rb") as f:
            data = f.read()
            codeType = chardet.detect(data)['encoding']
            convert(fileName, codeType, 'UTF-8')
    print('The encoding format of the unknown exception is automatically considered to be ASCCI encoding, and is not unprocessed.')
    print('total = %d' % filenumbers)
