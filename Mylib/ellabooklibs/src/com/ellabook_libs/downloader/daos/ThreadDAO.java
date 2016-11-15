package com.ellabook_libs.downloader.daos;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import com.ellabook_libs.downloader.cons.PublicCons;
import com.ellabook_libs.downloader.entities.DLInfo;
import com.ellabook_libs.downloader.entities.ThreadInfo;
import com.ellabook_libs.downloader.interfaces.DAO;

/**
 * 线程的DAO实现
 * DAO for thread.
 */
public class ThreadDAO extends DAO {
	public ThreadDAO(Context context) {
		super(context);
	}

	@Override
	public void insertInfo(DLInfo info) {
		ThreadInfo i = (ThreadInfo) info;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		db.execSQL("INSERT INTO " + PublicCons.DBCons.TB_THREAD + "(" +
				PublicCons.DBCons.TB_THREAD_BOOKID + ", " +
				PublicCons.DBCons.TB_THREAD_URL_BASE + ", " +
				PublicCons.DBCons.TB_THREAD_URL_REAL + ", " +
				PublicCons.DBCons.TB_THREAD_FILE_PATH + ", " +
				PublicCons.DBCons.TB_THREAD_START + ", " +
				PublicCons.DBCons.TB_THREAD_END + ", " +
				PublicCons.DBCons.TB_THREAD_ID + ") VALUES (?,?,?,?,?,?,?)",
				new Object[]{i.bookId,i.baseUrl, i.realUrl, i.dlLocalFile.getAbsolutePath(), i.start,
				i.end, i.id});
		db.close();
	}

	@Override
	public void deleteInfo(String id) {
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		db.execSQL("DELETE FROM " + PublicCons.DBCons.TB_THREAD + " WHERE " +
				PublicCons.DBCons.TB_THREAD_ID + "=?", new String[]{id});
		db.close();
	}

	public void deleteInfos(String url) {
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		db.execSQL("DELETE FROM " + PublicCons.DBCons.TB_THREAD + " WHERE " +
				PublicCons.DBCons.TB_THREAD_URL_BASE + "=?", new String[]{url});
		db.close();
	}

	@Override
	public void updateInfo(DLInfo info) {
		ThreadInfo i = (ThreadInfo) info;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		db.execSQL("UPDATE " + PublicCons.DBCons.TB_THREAD + " SET " +
				PublicCons.DBCons.TB_THREAD_START + "=? WHERE " +
				PublicCons.DBCons.TB_THREAD_BOOKID + "=? AND " +
				PublicCons.DBCons.TB_THREAD_ID + "=?", new Object[]{i.start, i.bookId, i.id});
		db.close();
	}

	@Override
	public DLInfo queryInfo(String id) {
		ThreadInfo info = null;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		Cursor c = db.rawQuery("SELECT " +
				PublicCons.DBCons.TB_THREAD_URL_BASE + ", " +
				PublicCons.DBCons.TB_THREAD_URL_REAL + ", " +
				PublicCons.DBCons.TB_THREAD_FILE_PATH + ", " +
				PublicCons.DBCons.TB_THREAD_START + ", " +
				PublicCons.DBCons.TB_THREAD_END +  ", " +
				PublicCons.DBCons.TB_THREAD_BOOKID+ " FROM " +
				PublicCons.DBCons.TB_THREAD + " WHERE " +
				PublicCons.DBCons.TB_THREAD_ID + "=?", new String[]{id});
		if (c.moveToFirst()) {
			info = new ThreadInfo(c.getString(5),new File(c.getString(2)), c.getString(0), c.getString(1),
					c.getInt(3), c.getInt(4), id);
		}
		c.close();
		db.close();
		return info;
	}

	public List<ThreadInfo> queryInfos(String bookid) {
		List<ThreadInfo> infos = new ArrayList<ThreadInfo>();
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		Cursor c = db.rawQuery("SELECT " +
				PublicCons.DBCons.TB_THREAD_URL_BASE + ", " +
				PublicCons.DBCons.TB_THREAD_URL_REAL + ", " +
				PublicCons.DBCons.TB_THREAD_FILE_PATH + ", " +
				PublicCons.DBCons.TB_THREAD_START + ", " +
				PublicCons.DBCons.TB_THREAD_END + ", " +
				PublicCons.DBCons.TB_THREAD_ID + ", " +
				PublicCons.DBCons.TB_THREAD_BOOKID+ " FROM " +
				PublicCons.DBCons.TB_THREAD + " WHERE " +
				PublicCons.DBCons.TB_THREAD_BOOKID + "=?", new String[]{bookid});
		while (c.moveToNext()) {
			infos.add(new ThreadInfo(c.getString(6),new File(c.getString(2)), c.getString(0),c.getString(1),
					c.getInt(3),  c.getInt(4), c.getString(5)));
		}
		c.close();
		db.close();
		return infos;
	}

	@Override
	public void deleteInfoByid(String bookid) {
		// TODO Auto-generated method stub

	}

	@Override
	public DLInfo queryInfoByid(String bookid) {
		// TODO Auto-generated method stub
		return null;
	}
}
