package org.cocos2dx.cpp.sqlite;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import org.cocos2dx.cpp.http.HttpTool;
import org.json.JSONArray;
import org.json.JSONObject;

public class MySQLiteOpenHelper extends SQLiteOpenHelper {
    private String dbName;
    private String packageName;
    private Context m_context;

    public MySQLiteOpenHelper(Context context, String name, SQLiteDatabase.CursorFactory factory, int version)
    {
        super(context, name, factory, version);
    }

    public MySQLiteOpenHelper(Context context,String dbName)
    {
        super(context, dbName+".db", null, 1);
        this.dbName=dbName;
        this.packageName = context.getPackageName();
        this.m_context = context;
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        
    }

    @Override
    //version 版本参数改变时 被调用,用来修改表结构
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        //db.execSQL("ALTER TABLE person ADD COLUMN accounts integer");
    	String dir = m_context.getApplicationContext().getFilesDir().getAbsolutePath();
    	Log.i("show",dir);
        String path =dir+ "/data.db";
    	File file = new File(path);
    	if(file.isFile()&& file.exists())
    	{
    		file.delete();
    	}
    }

    @Override
    public SQLiteDatabase getWritableDatabase() {
        SQLiteDatabase db=null;
        try {
//            String dir=android.os.Environment.getExternalStorageDirectory().getAbsolutePath()+"/Pictures";
        	String dir = m_context.getApplicationContext().getFilesDir().getAbsolutePath();
        	Log.i("show",dir);
            String path =dir+ "/"+dbName+".db";
            File dbp=new File(dir);
            File dbf=new File(path);
            if(!dbp.exists()){
                dbp.mkdir();
            }
            Log.i("show","获取数据库文件");
            //数据库文件是否创建成功
            boolean isFileCreateSuccess=false;
            if(!dbf.exists()){            	
                isFileCreateSuccess=copyDataBase(m_context,dir,path);
                if(!isFileCreateSuccess)
                	Log.i("show","复制数据库失败");
            }
            else{
                isFileCreateSuccess=true;
            }
            if(isFileCreateSuccess)           
                db = SQLiteDatabase.openOrCreateDatabase(dbf, null);
        }catch (Exception e)
        {
            Log.e("201605121820",e.toString());
        }        
        return db;        
    }
    public static boolean copyDataBase(Context context,String dirs,String path) {
        try
        {
        	Log.i("show","复制数据库");
            File dir = new File(dirs);
            if (!dir.exists())
                dir.mkdir();
            FileOutputStream os = new FileOutputStream(path);
            InputStream is = context.getAssets().open("data.db");
            byte[] buffer = new byte[1024*1024];
            int count = 0;
            while ((count = is.read(buffer)) > 0)
            {
                os.write(buffer, 0, count);
                os.flush();
            }
            is.close();
            os.close();
            return true;
        }
        catch (Exception e)
        {
            Log.e("201511241051", e.toString());
            return false;
        }
    }
       
    /*public static String getDataBaseVersion(SQLiteDatabase db) throws Exception
    {
    	Cursor cursor = db.query("dbinfo", null, null, new String[]{},
				null, null, null);
    	String version = "";
		while (cursor.moveToNext()) {
			long versionA = cursor.getLong(cursor.getColumnIndex("appversionA"));
			long versionB = cursor.getLong(cursor.getColumnIndex("appversionB"));
			long versionC = cursor.getLong(cursor.getColumnIndex("appversionC"));
			version = Long.toString(versionA) +"."+ Long.toString(versionB) +"."+ Long.toString(versionC);
		}
		return version;
    }*/
    
    public static long getDataBaseVersion(SQLiteDatabase db) throws Exception
    {
    	Cursor cursor = db.query("dbinfo", null, null, new String[]{},
				null, null, null);
    	long version = 0;
		while (cursor.moveToNext()) {
//			long versionA = cursor.getLong(cursor.getColumnIndex("appversionA"));
//			long versionB = cursor.getLong(cursor.getColumnIndex("appversionB"));
			long versionC = cursor.getLong(cursor.getColumnIndex("appversionC"));
//			version = Long.toString(versionA) +"."+ Long.toString(versionB) +"."+ Long.toString(versionC);
			version = versionC;
		}
		return version;
    }
    
    public static boolean OutDataBase(String inPath,String outPath)
    {
    	try
        {
        	Log.i("show","输出数据库");
            FileOutputStream os = new FileOutputStream(outPath);
            InputStream is = new FileInputStream(inPath);
            byte[] buffer = new byte[1024*1024];
            int count = 0;
            while ((count = is.read(buffer)) > 0)
            {
                os.write(buffer, 0, count);
                os.flush();
            }
            is.close();
            os.close();
            return true;
        }
        catch (Exception e)
        {
            Log.e("201606281943", e.toString());
            return false;
        }
    }
}
