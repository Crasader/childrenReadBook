package com.ellabook_libs.downloader.daos;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.content.res.AssetManager;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

public class CopyDBToSDCard {  
Context context ;
	//数据库存储路径  
//	String filePath = "data/data/com.ellabook/databases/data.db";  

	SQLiteDatabase database;   
	public  SQLiteDatabase openDatabase(Context context){  
//		System.out.println("filePath:"+filePath);  
		this.context=context;
		File jhPath=new File(context.getFilesDir().getParentFile(),"databases/data.db");  
		//查看数据库文件是否存在  
		if(jhPath.exists()){  
			Log.i("20160630.13:45", "存在数据库");
			//存在则直接返回打开的数据库  
			return SQLiteDatabase.openOrCreateDatabase(jhPath, null);  
		}else{  
			//不存在先创建文件夹  
			File path=new File(context.getFilesDir().getParentFile(),"databases");  
			Log.i("20160630.13:45", "pathStr="+path);
			if (path.mkdir()){  
				Log.i("20160630.13:45", "创建成功"); 
			}else{  
				Log.i("20160630.13:45", "创建失败");
			};  
			try {  
				//得到资源  
				AssetManager am= context.getAssets();  
				//得到数据库的输入流  
				InputStream is=am.open("data.db");  
				Log.i("20160630.13:45", is+"");
				//用输出流写到SDcard上面	
				FileOutputStream fos=new FileOutputStream(jhPath);  
				Log.i("20160630.13:45", "fos="+fos);
				Log.i("20160630.13:45", "jhPath="+jhPath);
				//创建byte数组  用于1KB写一次  
				byte[] buffer=new byte[1024];  
				int count = 0;  
				while((count = is.read(buffer))>0){  
					Log.i("20160630.13:45", "得到");
					fos.write(buffer,0,count);  
				}  
				//最后关闭就可以了  
				fos.flush();  
				fos.close();  
				is.close();  
			} catch (IOException e) {  
				// TODO Auto-generated catch block  
				Log.i("2016063016", e.toString());
				return null;
			}  
			//如果没有这个数据库  我们已经把他写到SD卡上了，然后在执行一次这个方法 就可以返回数据库了  
			return openDatabase(context);  
		}  
	}  
	public static  void UpgradedVersion(Context context) {
		CopyDBToSDCard copy = new CopyDBToSDCard();
		copy.openDatabase(context);

	}
}
