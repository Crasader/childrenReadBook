package com.ellabook_libs.downloader.daos;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import com.ellabook_libs.downloader.cons.PublicCons;
import com.ellabook_libs.downloader.entities.DLInfo;
import com.ellabook_libs.downloader.entities.TaskInfo;
import com.ellabook_libs.downloader.entities.ThreadInfo;
import com.ellabook_libs.downloader.interfaces.DAO;

/**
 * 下载任务的DAO实现
 * DAO for download task.
 *
 */
public class TaskDAO extends DAO {
	public TaskDAO(Context context) {
		super(context);
	}

	@Override
	public void insertInfo(DLInfo info) {
		TaskInfo i = (TaskInfo) info;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		db.execSQL("INSERT INTO " + PublicCons.DBCons.TB_TASK + "(" +
				PublicCons.DBCons.TB_TASK_BOOKID + ", " +
				PublicCons.DBCons.TB_TASK_URL_BASE + ", " +
				PublicCons.DBCons.TB_TASK_URL_REAL + ", " +
				PublicCons.DBCons.TB_TASK_FILE_PATH + ", " +
				PublicCons.DBCons.TB_TASK_PROGRESS + ", " +
				PublicCons.DBCons.TB_TASK_FILE_LENGTH+ ", " +PublicCons.DBCons.TB_TASK_ISFINISH+ ") values (?,?,?,?,?,?,?)",
				new Object[]{i.bookId,i.baseUrl, i.realUrl, i.dlLocalFile.getAbsolutePath(), i.progress,
				i.length ,i.isfinish});
		db.close();
	}

	@Override
	public void deleteInfo(String url) {
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		db.execSQL("DELETE FROM " + PublicCons.DBCons.TB_TASK + " WHERE " +
				PublicCons.DBCons.TB_TASK_URL_BASE + "=?", new String[]{url});
		db.close();
	}

	@Override
	public void updateInfo(DLInfo info) {
		TaskInfo i = (TaskInfo) info;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		db.execSQL("UPDATE " + PublicCons.DBCons.TB_TASK + " SET " +
				PublicCons.DBCons.TB_TASK_PROGRESS + "=? WHERE " +
				PublicCons.DBCons.TB_TASK_BOOKID + "=?", new Object[]{i.progress, i.bookId});
		db.close();
	}

	@Override
	public DLInfo queryInfo(String url) {
		TaskInfo info = null;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		Cursor c = db.rawQuery("SELECT " +
				PublicCons.DBCons.TB_TASK_URL_BASE + ", " +
				PublicCons.DBCons.TB_TASK_URL_REAL + ", " +
				PublicCons.DBCons.TB_TASK_FILE_PATH + ", " +
				PublicCons.DBCons.TB_TASK_PROGRESS + ", " +
				PublicCons.DBCons.TB_TASK_FILE_LENGTH+ ", " +
				PublicCons.DBCons.TB_TASK_BOOKID+ ", " +
				PublicCons.DBCons.TB_TASK_ISFINISH+ 
				" FROM " +
				PublicCons.DBCons.TB_TASK + " WHERE " +
				PublicCons.DBCons.TB_TASK_URL_BASE + "=?", new String[]{url});
		if (c.moveToFirst()) {
			info = new TaskInfo(c.getString(5),new File(c.getString(2)), c.getString(0),c.getString(1),
					c.getInt(3), c.getInt(4) ,c.getInt(6));
		}
		c.close();
		db.close();
		return info;
	}

	@Override
	public void deleteInfoByid(String bookid) {
		// TODO Auto-generated method stub
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		db.execSQL("DELETE FROM " + PublicCons.DBCons.TB_TASK + " WHERE " +
				PublicCons.DBCons.TB_TASK_BOOKID + "=?", new String[]{bookid});
		db.close();
	}

	@Override
	public DLInfo queryInfoByid(String bookid) {
		// TODO Auto-generated method stub
		TaskInfo info = null;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		Cursor c = db.rawQuery("SELECT " +
				PublicCons.DBCons.TB_TASK_URL_BASE + ", " +
				PublicCons.DBCons.TB_TASK_URL_REAL + ", " +
				PublicCons.DBCons.TB_TASK_FILE_PATH + ", " +
				PublicCons.DBCons.TB_TASK_PROGRESS + ", " +
				PublicCons.DBCons.TB_TASK_FILE_LENGTH+ ", " +
				PublicCons.DBCons.TB_TASK_BOOKID+ ", " +
				PublicCons.DBCons.TB_TASK_ISFINISH+ 
				" FROM " +
				PublicCons.DBCons.TB_TASK + " WHERE " +
				PublicCons.DBCons.TB_TASK_BOOKID + "=?", new String[]{bookid});
		if (c.moveToFirst()) {
			info = new TaskInfo(c.getString(5),new File(c.getString(2)), c.getString(0),c.getString(1),
					c.getInt(3), c.getInt(4) ,c.getInt(6));
		}
		c.close();
		db.close();
		return info;
	}

	public void finishInfo(TaskInfo info) {
		// TODO Auto-generated method stub
		TaskInfo i = (TaskInfo) info;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		db.execSQL("UPDATE " + PublicCons.DBCons.TB_TASK + " SET " +
				PublicCons.DBCons.TB_TASK_ISFINISH + "=? WHERE " +
				PublicCons.DBCons.TB_TASK_BOOKID + "=?", new Object[]{i.isfinish, i.bookId});
		db.close();
	}

	public List<TaskInfo> queryInfos() {
		// TODO Auto-generated method stub
		List<TaskInfo> infos = new ArrayList<TaskInfo>();
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		Cursor cursor = db.rawQuery("select * from " +PublicCons.DBCons.TB_TASK , null);
		while (cursor.moveToNext()) {
			infos.add(new TaskInfo(cursor.getString(cursor.getColumnIndex(PublicCons.DBCons.TB_TASK_BOOKID)),
					new File(cursor.getString(cursor.getColumnIndex(PublicCons.DBCons.TB_TASK_FILE_PATH))),
					cursor.getString(cursor.getColumnIndex(PublicCons.DBCons.TB_TASK_URL_BASE)),
					cursor.getString(cursor.getColumnIndex(PublicCons.DBCons.TB_TASK_URL_REAL)),
					cursor.getInt(cursor.getColumnIndex(PublicCons.DBCons.TB_TASK_PROGRESS)), 
					cursor.getInt(cursor.getColumnIndex(PublicCons.DBCons.TB_TASK_FILE_LENGTH)) ,
					cursor.getInt(cursor.getColumnIndex(PublicCons.DBCons.TB_TASK_ISFINISH))));
		}
		cursor.close();
		db.close();
		return infos;
	}
}
