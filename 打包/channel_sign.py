import zipfile
import os 
import os.path 
import shutil 
import time,  datetime

version= "1.7.1"
FristDir = "E:/output/"+version+".apk"
NextDir = "E:/output_rel"


def copyfile(file,path):
    shutil.copyfile(file, path)


list =("c360","baidu","wandoujia","xiaomi","sanxing","yingyonghui","haixin","huawei","flyme","meizu","ppzushou","anzhi","lenovo","sogou","jinli","gfan","iqiyi","oppo","vivo","qq","ellabook","inapp","wo")
N = len(list)
i = 0
while i < N:
    copyfile(FristDir,NextDir+"/"+version+"_"+list[i]+".apk")
    empty_channel_file = "META-INF/ellabookchannel_{channel}".format(channel=list[i])
    zipped = zipfile.ZipFile(NextDir+"/"+version+"_"+list[i]+".apk", 'a', zipfile.ZIP_DEFLATED) 
    zipped.write(r'E:/output/null', empty_channel_file)
    zipped.close
    print(NextDir+"/"+version+"_"+list[i]+".apk"+"/tDone")
    i += 1



