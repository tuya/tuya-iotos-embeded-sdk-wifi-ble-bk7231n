#!/usr/bin/python
# -*- coding: UTF-8 -*-
import sys
import json
import os
import hashlib

# $1 - 产品目录，如： apps/product1
# $2 - 产品名称，如： product1
# $3 - 产品版本，如： 1.0.0
# $4 - 产物包路径，如： output/dist/product1_1.0.0
outputPath = sys.argv[4]
productName = sys.argv[2]
productVersion = sys.argv[3]

# 计算MD5
def getMD5(file_path):
    md5 = None
    if os.path.isfile(file_path):
        f = open(file_path,'rb')
        md5_obj = hashlib.md5()
        md5_obj.update(f.read())
        hash_code = md5_obj.hexdigest()
        f.close()
        md5 = str(hash_code).lower()

    return md5

def main():
    if not productName:
        print("please input the product name!")
        return
    
    if not outputPath: 
        print("please input the path of product output")
        return
    
    packageInfoFile  = productName + '_' + productVersion +'.json'        
    packageInfo = []
    
    # 搜索目录，构造json字符串 
    files = os.listdir(outputPath)
    for f in files:
        binFile = {}
        binFileInfo = {}
        name, ext = os.path.splitext(f)
        if ext == ".bin" or ext == "":
            binFileInfo["name"] = f
            binFileInfo["MD5"] = getMD5(outputPath + '/' + f)
            if name.find("QIO") != -1:
                binFile["fwQIO"] = binFileInfo
                packageInfo.append(binFile)
                continue
            if name.find("QOUT") != -1:
                binFile["fwQOUT"] = binFileInfo
                packageInfo.append(binFile)
                continue
            if name.find("DIO") != -1:
                binFile["fwDIO"] = binFileInfo
                packageInfo.append(binFile)
                continue
            if name.find("DOUT") != -1:
                binFile["fwDOUT"] = binFileInfo
                packageInfo.append(binFile)
                continue
            if name.find("UA") != -1:
                binFile["fwUserArea"] = binFileInfo
                packageInfo.append(binFile)
                continue
            if name.find("UG") != -1:
                binFile["fwUpgrade"] = binFileInfo
                packageInfo.append(binFile)
                continue            
    
    print(packageInfo)
    jsonStr = json.dumps(packageInfo, indent=2, ensure_ascii=False)
    print(jsonStr)

    # 写入文件
    fp = open(outputPath + '/' + packageInfoFile, 'w')
    fp.write(jsonStr)
    fp.close()

    return
        
            
if __name__ == '__main__':
    main()
