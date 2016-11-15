package com.ellabook_libs.downloader.interfaces;

import com.ellabook_libs.downloader.daos.DatabaseHelper;
import com.ellabook_libs.downloader.entities.DLInfo;

import android.content.Context;
import android.content.pm.PackageManager.NameNotFoundException;

/**
 * DAO抽象类
 * Abstract class of DAO.
 */
public abstract class DAO {
    protected DatabaseHelper dbHelper;

    public DAO(Context context) {
			try {
				dbHelper = new DatabaseHelper(context);
			} catch (NameNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    }

    public abstract void insertInfo(DLInfo info);

    public abstract void deleteInfo(String url);
    
    public abstract void deleteInfoByid(String bookid);

    public abstract void updateInfo(DLInfo info);

    public abstract DLInfo queryInfo(String str);

    public abstract DLInfo queryInfoByid(String bookid);
    
    public void close() {
        dbHelper.close();
    }
}
