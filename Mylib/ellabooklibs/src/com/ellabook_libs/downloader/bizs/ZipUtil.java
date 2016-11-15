package com.ellabook_libs.downloader.bizs;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

import android.util.Log;

public class ZipUtil {
	public ZipUtil(){
		
	}
    /** 
* 解压缩一个文件 
*  
* @param zipFile 
*            压缩文件 
* @param folderPath 
*            解压缩的目标目录 
* @throws IOException 
*             当解压缩过程出错时抛出 
*/  

public static void upZipFile(File zipFile, String folderPath)  
     throws ZipException, IOException {  
 File desDir = new File(folderPath);  
 if (!desDir.exists()) {  
     desDir.mkdirs();  
 }  
 ZipFile zf = new ZipFile(zipFile);  
 for (Enumeration<?> entries = zf.entries(); entries.hasMoreElements();) {  
     ZipEntry entry = ((ZipEntry) entries.nextElement());  
     InputStream in = zf.getInputStream(entry);  
     String str = folderPath;  
     // str = new String(str.getBytes("8859_1"), "GB2312");  
     File desFile = new File(str, java.net.URLEncoder.encode(  
             entry.getName(), "UTF-8"));  

     if (!desFile.exists()) {  
         File fileParentDir = desFile.getParentFile();  
         if (!fileParentDir.exists()) {  
             fileParentDir.mkdirs();  
         }  
     }  

     OutputStream out = new FileOutputStream(desFile);  
     byte buffer[] = new byte[1024 * 1024];  
     int realLength = in.read(buffer);  
     while (realLength != -1) {  
         out.write(buffer, 0, realLength);  
         realLength = in.read(buffer);  
     }  

     out.close();  
     in.close();  

 }  
}  
}
