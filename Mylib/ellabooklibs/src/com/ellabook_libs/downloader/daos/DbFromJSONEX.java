package com.ellabook_libs.downloader.daos;


import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;


import android.content.ContentValues;
import android.content.Context;
import android.content.pm.PackageManager.NameNotFoundException;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

/**
 * Us业务操作
 * 
 * */
public class DbFromJSONEX{
	public static String DB_NAME = "data.db";
	public static SQLiteDatabase db = null;
	private DatabaseHelper dbHelper = null;	
	private static String TABLENAME = null;
	private String json;
	private Context context;
	private int dbVersion = 1;
	public DbFromJSONEX(Context context,String json) {
		this.context = context;
		this.json = json;
		JSONObject jsonObj;
		try {
			jsonObj = new JSONObject(json);
			TABLENAME = jsonObj.getString("tableName");
		} catch (JSONException e) {
			Log.e("2016063011", e.toString());
		}

	}


	public void Add() {
		try {
			JSONObject jsonObj = new JSONObject(json);
			ContentValues values = new ContentValues();
			String[] dataString = new String[jsonObj.getJSONArray("addLong").length()+jsonObj.getJSONArray("addString").length()];
			for (int i = 0; i < dataString.length; i++) {
				if(i<jsonObj.getJSONArray("addLong").length()){
					dataString[i]=jsonObj.getJSONArray("addLong").getString(i);
				}else{
					dataString[i]=jsonObj.getJSONArray("addString").getString(i-jsonObj.getJSONArray("addLong").length());
				}
			}
			JSONObject data = jsonObj.getJSONObject("data");
			for (int i = 0; i < dataString.length; i++) {
				values.put(dataString[i], data.getString(dataString[i]));
			}
			getDb().insert(TABLENAME, null, values);
		} catch (Exception e) {
			e.printStackTrace();
		} finally {

		}
	}

	@SuppressWarnings("null")
	public void Update() {
		try {
			JSONObject jsonObj = new JSONObject(json);
			ContentValues values = new ContentValues();
			String whereClause=null;
			String[] whereArgs =null;
			String[] dataString = new String[jsonObj.getJSONArray("addLong").length()+jsonObj.getJSONArray("addString").length()];
			for (int i = 0; i < dataString.length; i++) {
				if(i<jsonObj.getJSONArray("addLong").length()){
					dataString[i]=jsonObj.getJSONArray("addLong").getString(i);
				}else{
					dataString[i]=jsonObj.getJSONArray("addString").getString(i-jsonObj.getJSONArray("addLong").length());
				}
			}
			JSONObject data = jsonObj.getJSONObject("data");
			for (int i = 0; i < dataString.length; i++) {
				values.put(dataString[i], data.getString(dataString[i]));
			}
			whereClause = jsonObj.getString("whereClause");
			for (int i = 0; i <  jsonObj.getJSONArray("whereArgs").length(); i++) {
				whereArgs[i]  = jsonObj.getJSONArray("whereArgs").getString(i);
			}
			getDb().update(TABLENAME, values, whereClause, whereArgs);

		} catch (Exception e) {
			Log.e("2016063016", e.toString());
		} finally {
		}
	}

	@SuppressWarnings("null")
	public void Delete() {
		try {
			JSONObject jsonObj = new JSONObject(json);
			String whereClause=null;
			String[] whereArgs =null;
			whereClause = jsonObj.getString("whereClause");
			for (int i = 0; i <  jsonObj.getJSONArray("whereArgs").length(); i++) {
				whereArgs[i]  = jsonObj.getJSONArray("whereArgs").getString(i);
			}
			getDb().delete(TABLENAME, whereClause, whereArgs);
		} catch (Exception e) {
			Log.e("201608191433", e.toString());
		} finally {
		}
	}

	public Cursor Query() {
		Cursor cursor = null;
		try {
			openDBConnect();
			JSONObject jsonObj;
			String[] columns =null;
			String selection=null;
			String[] selectionArgs =null;
			try {
				jsonObj = new JSONObject(json);
				TABLENAME = jsonObj.getString("tableName");
				JSONArray longArray = jsonObj.getJSONArray("needLong");
				JSONArray stringArray = jsonObj.getJSONArray("needString");
				columns= new String[longArray.length()+stringArray.length()];
				for (int i = 0; i < columns.length; i++) {
					if(i<longArray.length()){
						columns[i]=longArray.getString(i);
					}else{
						columns[i]=stringArray.getString(i-longArray.length());
					}
				}
				selection = jsonObj.getString("selection");
				JSONArray selectionArgsArray = jsonObj.getJSONArray("selectionArgs");
				selectionArgs= new String[selectionArgsArray.length()];
				for (int i = 0; i < selectionArgs.length; i++) {
					selectionArgs[i]=selectionArgsArray.getString(i);
				}
			} catch (JSONException e) {				
				Log.e("2016063011", e.toString());
			}
			cursor = getDb().query(TABLENAME, columns, selection,
					selectionArgs, null, null, null);

		} catch (Exception e) {
			Log.i("2016063016", e.toString());
		} finally {
		}

		return cursor;
	}

	public boolean openDBConnect() {
		try {
			if (db == null){
			this.dbVersion = context.getPackageManager().getPackageInfo(
					context.getPackageName(), 0).versionCode;
			dbHelper = new DatabaseHelper(context, DB_NAME, dbVersion);
			db = dbHelper.getWritableDatabase();
			}
			return true;
		} catch (NameNotFoundException e) {
			Log.e("201608191434", e.toString());
			return false;
		}		
	}

	public boolean closeDBConnect() {
		try {
			if (db != null) {
				db.close();
				db = null;
			}
			if (dbHelper != null) {
				dbHelper.close();
				dbHelper = null;
			}
			return true;
		} catch (Exception e) {
			Log.e("201608191435", e.toString());
			return false;
		}
	}

	public DatabaseHelper getDbHelper() {
		return dbHelper;
	}

	public SQLiteDatabase getDb() {
		return db;
	}		
}
