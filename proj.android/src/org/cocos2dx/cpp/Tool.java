package org.cocos2dx.cpp;

import java.io.FileOutputStream;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.sql.Timestamp;
import java.util.Enumeration;
import java.util.Map;

import org.apache.http.conn.util.InetAddressUtils;
import org.cocos2dx.cpp.http.HttpTool;
import org.cocos2dx.cpp.sqlite.MySQLiteOpenHelper;
import org.json.JSONArray;
import org.json.JSONObject;


import android.app.Activity;
import android.content.ContentValues;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

public class Tool {

	// 判断是否连接wifi
	public static boolean isWifi(Activity context) {
		ConnectivityManager connectivityManager = (ConnectivityManager) context
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo activeNetInfo = connectivityManager.getActiveNetworkInfo();
		if (activeNetInfo != null
				&& activeNetInfo.getType() == ConnectivityManager.TYPE_WIFI) {
			return true;
		}
		return false;
	}

	// 判断是否联网
	public static boolean isNetConnect(Activity context) {
		ConnectivityManager cwjManager = (ConnectivityManager) context
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo info = cwjManager.getActiveNetworkInfo();
		if (info != null && info.isAvailable()) {
			return true;
		} else {
			return false;
		}
	}

	// 获取毫秒级时间戳
	public static long getTime() {
		Timestamp now = new Timestamp(System.currentTimeMillis());// 获取系统当前时间
		return now.getTime();
	}

	// 增加记录 数据库
	public static void WriteSqlite(SQLiteDatabase db, String tableName,
			ContentValues value, String whereClause, String[] whereArgs,
			int count) {
		if (db == null)
			return;
		try {
			db.delete(tableName, whereClause, whereArgs);
		} catch (Exception e) {
			Log.e("201605271141", e.toString());
			String dir = AppActivity.m_context.getApplicationContext().getFilesDir().getAbsolutePath();
            String path =dir+ "/data.db";
            MySQLiteOpenHelper.copyDataBase(AppActivity.m_context, dir, path);
		}
		try {
			long result = db.insert(tableName, null, value);
			if (result == count)
				Log.i("show", "insert success" + result);
		} catch (Exception e) {
			Log.e("201605271142", e.toString());
		}
	}

	// 查询信息
	public static JSONArray ReadSqlite(SQLiteDatabase db, String tableName,
			String[] str_columns, String[] long_columns, String selection,
			String[] selectionArgs) {
		Cursor cursor = db.query(tableName, null, selection, selectionArgs,
				null, null, null);
		JSONArray array = new JSONArray();
		while (cursor.moveToNext()) {
			JSONObject userinfo = new JSONObject();
			for (int i = 0; i < str_columns.length; i++) {
				String key = str_columns[i];
				HttpTool.Put(userinfo, key,
						cursor.getString(cursor.getColumnIndex(key)));
			}
			for (int i = 0; i < long_columns.length; i++) {
				String key = long_columns[i];
				HttpTool.Put(userinfo, key,
						cursor.getLong(cursor.getColumnIndex(key)));
			}
			array.put(userinfo);
		}
		return array;
	}

	public static String GetString4Map(String key, Map map) {
		if (map == null)
			return "";
		try {
			if (map.containsKey(key))
				return (String) map.get(key);
			else
				return "";
		} catch (Exception e) {
			Log.e("201605231452", e.toString());
			return "";
		}
	}

	public static Long GetLong4Map(String key, Map map) {
		if (map == null)
			return (long) -999;
		try {
			if (map.containsKey(key))
				return (Long) map.get(key);
			else
				return (long) -999;
		} catch (Exception e) {
			Log.e("201605231451", e.toString());
			return (long) -999;
		}
	}
	
	/**
	 * 获取版本号
	 * @return 当前应用的版本号
	 */
	public static String getVersion(Context context) {
	    try {
	        PackageManager manager = context.getPackageManager();
	        PackageInfo info = manager.getPackageInfo(context.getPackageName(), 0);
	        String version = info.versionName;
	        return version;
	    } catch (Exception e) {
	        Log.e("201606081432",e.toString());
	        return "";
	    }
	}
	
	public static void writeFileData(String fileName, String message, Context context) {
		try {
			FileOutputStream fout = context.openFileOutput(fileName,
					Context.MODE_PRIVATE);
			byte[] bytes = message.getBytes();
			fout.write(bytes);
			fout.close();
			Log.i("show","已购列表写入文件");
		} catch (Exception e) {
			Log.e("201606131724", e.toString());
		}
	}  
	
	public static String getLocalIpAddress() { 
		  try { 
		    for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();) { 
		      NetworkInterface intf = en.nextElement(); 
		      for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();) { 
		        InetAddress inetAddress = enumIpAddr.nextElement(); 
		        if (!inetAddress.isLoopbackAddress() && InetAddressUtils.isIPv4Address(inetAddress  
                        .getHostAddress())) { 
		        return inetAddress.getHostAddress(); 
		      } 
		    } 
		  } 
		  } catch (SocketException ex) { 
		      Log.e("201606161815", ex.toString()); 
		  } 
		  return null; 
		} 
}
