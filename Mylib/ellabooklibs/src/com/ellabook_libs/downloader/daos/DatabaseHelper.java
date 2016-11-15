package com.ellabook_libs.downloader.daos;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import com.ellabook_libs.downloader.cons.PublicCons;

import android.content.Context;
import android.content.pm.PackageManager.NameNotFoundException;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteDatabase.CursorFactory;
import android.util.Log;

/**
 * 数据库管理
 * 
 * */
public class DatabaseHelper extends SQLiteOpenHelper {

	private Context context;
	@SuppressWarnings("unused")
	private DatabaseHelper(Context context, String dbName,
			CursorFactory factory, int version) {
		super(context, dbName, factory, version);
		this.context=context;
		
	}

	@Override
	public SQLiteDatabase getWritableDatabase() {
		SQLiteDatabase db=null;
        try {
        	String dir = context.getApplicationContext().getFilesDir().getAbsolutePath();
            String path =dir+ "/"+DbFromJSONEX.DB_NAME;
            File dbp=new File(dir);
            File dbf=new File(path);
            if(!dbp.exists()){
                dbp.mkdir();
            }
            Log.i("show","获取数据库文件");
            //数据库文件是否创建成功
            boolean isFileCreateSuccess=false;
            if(!dbf.exists()){            	
                isFileCreateSuccess=copyDataBase(context,dir,path);
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

	public DatabaseHelper(Context context, String dbName, int version ) {
		this(context, dbName, null, version );

	}
	public DatabaseHelper(Context context) throws NameNotFoundException {
		
		this(context,DbFromJSONEX.DB_NAME, null, context.getPackageManager().getPackageInfo(context.getPackageName(), 0).versionCode);
		System.out.println("DBHelper isused");
	}
	@Override
	public void onCreate(SQLiteDatabase db) {
		System.out.println("DBHelper onCreate");
		db.execSQL(PublicCons.DBCons.TB_TASK_SQL_CREATE);
        db.execSQL(PublicCons.DBCons.TB_THREAD_SQL_CREATE);
	}

	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
		System.out.println("DBHelper onUpgrade");
		db.execSQL(PublicCons.DBCons.TB_TASK_SQL_UPGRADE);
        db.execSQL(PublicCons.DBCons.TB_THREAD_SQL_UPGRADE);
        onCreate(db);
		try {
			for (int i = oldVersion; i < newVersion; i++) {
				switch (i) {
				case 1:
					CopyDBToSDCard.UpgradedVersion(context);
					break;

				default:
					break;
				}
			}

		} catch (Exception e) {
			Log.i("2016063016", e.toString());
		}
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
}