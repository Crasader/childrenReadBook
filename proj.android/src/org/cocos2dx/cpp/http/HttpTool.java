package org.cocos2dx.cpp.http;

import org.cocos2dx.cpp.ImageTool;
import org.cocos2dx.cpp.MacroCode;
import org.cocos2dx.cpp.sqlite.MySQLiteOpenHelper;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.android.volley.RequestQueue;
import com.android.volley.Response.ErrorListener;
import com.android.volley.Response.Listener;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.ImageRequest;
import com.android.volley.toolbox.Volley;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.math.BigDecimal;
import java.net.URL;
import java.net.URLConnection;

public class HttpTool {
	//
	public static SQLiteDatabase openDatabase(Context context) {
		SQLiteDatabase db = null;
		try {
			db = new MySQLiteOpenHelper(context, "data").getWritableDatabase();
		} catch (Exception e) {
			Log.e("201605121948", e.toString());
		}
		return db;
	}

	// 获取网络管理队列
	public static RequestQueue getRequestQueue(Context context) {
		RequestQueue mQueue = Volley.newRequestQueue(context);
		mQueue.start();
		return mQueue;
	}

	public static void stopRequest(RequestQueue mQueue, int RequestID) {
		mQueue.cancelAll(RequestID);
	}

	// ---------------------------解析json--------------------------------------------------
	public static boolean GetBool(JSONObject response, String key,
			String ExceptionTag) {
		boolean result = false;
		try {
			result = response.getBoolean(key);
		} catch (Exception e) {
			Log.e(ExceptionTag, e.toString());
		}
		return result;
	}

	public static String GetString(JSONObject response, String key,
			String ExceptionTag) {
		String result = "";
		try {
			result = response.getString(key);
		} catch (Exception e) {
			Log.e(ExceptionTag, e.toString());
		}
		return result;
	}

	public static long GetLong(JSONObject response, String key,
			String ExceptionTag) {
		long result = -999;
		try {
			result = response.getLong(key);
		} catch (Exception e) {
			Log.e(ExceptionTag, e.toString());
			result = GetInt(response, key, ExceptionTag);
		}
		return result;
	}

	public static int GetInt(JSONObject response, String key,
			String ExceptionTag) {
		int result = -999;
		try {
			result = response.getInt(key);
		} catch (Exception e) {
			Log.e(ExceptionTag, e.toString());
		}
		return result;
	}

	public static double GetDouble(JSONObject response, String key,
			String ExceptionTag) {
		double result = -999;
		try {
			result = response.getDouble(key);
		} catch (Exception e) {
			Log.e(ExceptionTag, e.toString());
		}
		return result;
	}

	public static JSONObject GetJSONObject(JSONObject response, String key,
			String ExceptionTag) {
		try {
			JSONObject result = response.getJSONObject(key);
			return result;
		} catch (Exception e) {
			Log.e(ExceptionTag, e.toString());
			return new JSONObject();
		}
	}

	public static JSONArray GetJSONArray(JSONObject response, String key,
			String ExceptionTag) {
		try {
			JSONArray result = response.getJSONArray(key);
			return result;
		} catch (Exception e) {
			Log.e(ExceptionTag, e.toString());
			return new JSONArray();
		}
	}

	// ---------------------------解析jsonEND--------------------------------------------------

	// ---------------------------包装成json--------------------------------------------------
	public static void Put(JSONObject json, String key, Object value) {
		try {
			json.put(key, value);
		} catch (JSONException e) {
			Log.e("201605122058", e.toString());
		}
	}

	// ---------------------------包装成jsonEND--------------------------------------------------

	/**
	 * 获得指定文件的byte数组
	 */
	public static byte[] getBytes(String filePath) {
		byte[] buffer = null;
		try {
			File file = new File(filePath);
			FileInputStream fis = new FileInputStream(file);
			ByteArrayOutputStream bos = new ByteArrayOutputStream(1000);
			byte[] b = new byte[1000];
			int n;
			while ((n = fis.read(b)) != -1) {
				bos.write(b, 0, n);
			}
			fis.close();
			bos.close();
			buffer = bos.toByteArray();
		} catch (FileNotFoundException e) {
			Log.e("201605121757", e.toString());
		} catch (IOException e) {
			Log.e("201605121756", e.toString());
		}
		return buffer;
	}

	/**
	 * 获取文件夹大小
	 * 
	 * @param file
	 *            File实例
	 * @return long
	 */
	public static long getFolderSize(File file) {
		long size = 0;
		try {
			File[] fileList = file.listFiles();
			for (int i = 0; i < fileList.length; i++) {
				if (fileList[i].isDirectory()) {
					size = size + getFolderSize(fileList[i]);

				} else {
					size = size + fileList[i].length();
				}
			}
		} catch (Exception e) {
			Log.e("201512081703", e.toString());
		}
		// return size/1048576;
		return size;
	}

	/**
	 * 删除指定目录下文件及目录
	 * 
	 * @param deleteThisPath
	 * @param filepath
	 * @return
	 */
	public static void deleteFolderFile(String filePath, boolean deleteThisPath) {
		if (!TextUtils.isEmpty(filePath)) {
			try {
				File file = new File(filePath);
				if (file.isDirectory()) {// 处理目录
					File files[] = file.listFiles();
					for (int i = 0; i < files.length; i++) {
						deleteFolderFile(files[i].getAbsolutePath(), true);
					}
				}
				if (deleteThisPath) {
					if (!file.isDirectory()) {// 如果是文件，删除
						file.delete();
					} else {// 目录
						if (file.listFiles().length == 0) {// 目录下没有文件或者目录，删除
							file.delete();
						}
					}
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				Log.e("201512081702", e.toString());
			}
		}
	}

	/**
	 * 格式化单位
	 * 
	 * @param size
	 * @return
	 */
	public static String getFormatSize(double size) {
		double kiloByte = size / 1024;
		if (kiloByte < 1) {
			return size + "Byte(s)";
		}

		double megaByte = kiloByte / 1024;
		if (megaByte < 1) {
			BigDecimal result1 = new BigDecimal(Double.toString(kiloByte));
			return result1.setScale(2, BigDecimal.ROUND_HALF_UP)
					.toPlainString() + "KB";
		}

		double gigaByte = megaByte / 1024;
		if (gigaByte < 1) {
			BigDecimal result2 = new BigDecimal(Double.toString(megaByte));
			return result2.setScale(2, BigDecimal.ROUND_HALF_UP)
					.toPlainString() + "MB";
		}

		double teraBytes = gigaByte / 1024;
		if (teraBytes < 1) {
			BigDecimal result3 = new BigDecimal(Double.toString(gigaByte));
			return result3.setScale(2, BigDecimal.ROUND_HALF_UP)
					.toPlainString() + "GB";
		}
		BigDecimal result4 = new BigDecimal(teraBytes);
		return result4.setScale(2, BigDecimal.ROUND_HALF_UP).toPlainString()
				+ "TB";
	}

	public void downloadNet(final String downloadneturl, final String path) {
		// 下载网络文件
		boolean IsFinish = false;
		try {
			// 判断文件大小,是否多线程下载
			// 读取下载文件总大小
			int fileSize = new URL(downloadneturl).openConnection()
					.getContentLength();
			if (fileSize > 1024 * 1024) {
				downloadTask thread = new downloadTask(downloadneturl, 5, path);
				thread.start();
				while (!IsFinish) {
					Thread.sleep(1000);
					if (thread.isfinished) {
						IsFinish = true;
						break;
					}
				}
			} else {
				DownLoadFile downLoadFile = new DownLoadFile(path,
						downloadneturl, fileSize);
				downLoadFile.start();
				while (!IsFinish) {
					Thread.sleep(1000);
					if (downLoadFile.isFinish()) {
						IsFinish = true;
						break;
					}
				}
			}
		} catch (Exception e) {
			Log.e("201510201530", e.toString());
		}
	}

	// 单线程下载
	class DownLoadFile extends Thread {
		private boolean IsFinish = false;
		private String downloadUrl;// 下载链接地址
		private String filePath;// 保存文件路径地址
		private int fileSize;

		public DownLoadFile(String filePath, String downloadUrl, int fileSize) {
			this.filePath = filePath;
			this.downloadUrl = downloadUrl;
			this.fileSize = fileSize;
		}

		public boolean isFinish() {
			return IsFinish;
		}

		@Override
		public void run() {
			try {
				int byteread;
				int bytesum = 0;
				URL url = new URL(downloadUrl);
				URLConnection conn = url.openConnection();
				InputStream inStream = conn.getInputStream();
				FileOutputStream fs = new FileOutputStream(filePath);
				byte[] buffer = new byte[1024 * 1024];
				while ((byteread = inStream.read(buffer)) != -1) {
					bytesum += byteread;
					fs.write(buffer, 0, byteread);
				}
				if (bytesum == fileSize) {
					IsFinish = true;
				}
			} catch (Exception e) {
				Log.e("201510271455", e.toString());
			}
		}
	}

	// 多线程下载
	/**
	 * 多线程文件下载
	 * 
	 * @2014-8-7
	 */
	class downloadTask extends Thread {
		private String downloadUrl;// 下载链接地址
		private int threadNum;// 开启的线程数
		private String filePath;// 保存文件路径地址
		private int blockSize;// 每一个线程的下载量
		public boolean isfinished = false;

		public downloadTask(String downloadUrl, int threadNum, String fileptah) {
			this.downloadUrl = downloadUrl;
			this.threadNum = threadNum;
			this.filePath = fileptah;
		}

		@Override
		public void run() {
			FileDownloadThread[] threads = new FileDownloadThread[threadNum];
			try {
				URL url = new URL(downloadUrl);
				Log.i("show", "download file=" + downloadUrl);
				URLConnection conn = url.openConnection();
				// 读取下载文件总大小
				int fileSize = conn.getContentLength();
				if (fileSize <= 0) {
					System.out.println("读取文件失败");
					return;
				}
				// // 设置ProgressBar最大的长度为文件Size
				// mProgressbar.setMax(fileSize);
				// 计算每条线程下载的数据长度
				blockSize = (fileSize % threadNum) == 0 ? fileSize / threadNum
						: fileSize / threadNum + 1;

				Log.i("show", "fileSize:" + fileSize + "  blockSize:"
						+ blockSize);
				File file = new File(filePath);
				for (int i = 0; i < threads.length; i++) {
					// 启动线程，分别下载每个线程需要下载的部分
					threads[i] = new FileDownloadThread(url, file, blockSize,
							(i + 1));
					threads[i].setName("Thread:" + i);
					threads[i].start();
				}
				int downloadedAllSize = 0;
				while (!isfinished) {
					isfinished = true;
					// 当前所有线程下载总量
					downloadedAllSize = 0;
					for (int i = 0; i < threads.length; i++) {
						downloadedAllSize += threads[i].getDownloadLength();
						if (!threads[i].isCompleted()) {
							isfinished = false;
						}
					}
					// // 通知handler去更新视图组件
					// Message msg = new Message();
					// msg.getData().putInt("size", downloadedAllSize);
					// mHandler.sendMessage(msg);
					// // Log.d(TAG, "current downloadSize:" +
					// downloadedAllSize);
					// Thread.sleep(1000);// 休息1秒后再读取下载进度
				}
				Log.i("show", " all of downloadSize:" + downloadedAllSize);
			} catch (Exception e) {
				Log.e("异常201510270913", e.toString());
			}
		}
	}

	/**
	 * 文件下载类
	 * 
	 * @2014-5-6
	 */
	public class FileDownloadThread extends Thread {

		/** 当前下载是否完成 */
		private boolean isCompleted = false;
		/** 当前下载文件长度 */
		private int downloadLength = 0;
		/** 文件保存路径 */
		private File file;
		/** 文件下载路径 */
		private URL downloadUrl;
		/** 当前下载线程ID */
		private int threadId;
		/** 线程下载数据长度 */
		private int blockSize;

		/**
		 * // * @param url:文件下载地址 // * @param file:文件保存路径 // * @param
		 * blocksize:下载数据长度 // * @param threadId:线程ID
		 */
		public FileDownloadThread(URL downloadUrl, File file, int blocksize,
				int threadId) {
			this.downloadUrl = downloadUrl;
			this.file = file;
			this.threadId = threadId;
			this.blockSize = blocksize;
		}

		@Override
		public void run() {
			BufferedInputStream bis = null;
			RandomAccessFile raf = null;
			try {
				URLConnection conn = downloadUrl.openConnection();
				conn.setAllowUserInteraction(true);
				int startPos = blockSize * (threadId - 1);// 开始位置
				int endPos = blockSize * threadId - 1;// 结束位置
				// 设置当前线程下载的起点、终点
				conn.setRequestProperty("Range", "bytes=" + startPos + "-"
						+ endPos);
				System.out.println(Thread.currentThread().getName()
						+ "  bytes=" + startPos + "-" + endPos);
				byte[] buffer = new byte[1024];
				bis = new BufferedInputStream(conn.getInputStream());

				raf = new RandomAccessFile(file, "rwd");
				raf.seek(startPos);
				int len;
				while ((len = bis.read(buffer, 0, 1024)) != -1) {
					raf.write(buffer, 0, len);
					downloadLength += len;
				}
				isCompleted = true;
				Log.i("show", "current thread task has finished,all size:"
						+ downloadLength);

			} catch (Exception e) {
				e.printStackTrace();
			} finally {
				if (bis != null) {
					try {
						bis.close();
					} catch (Exception e) {
						Log.e("异常201510271336", e.toString());
					}
				}
				if (raf != null) {
					try {
						raf.close();
					} catch (Exception e) {
						Log.e("异常201510271337", e.toString());
					}
				}
			}
		}

		/**
		 * 线程文件是否下载完毕
		 */
		public boolean isCompleted() {
			return isCompleted;
		}

		/**
		 * 线程下载文件长度
		 */
		public int getDownloadLength() {
			return downloadLength;
		}
	}

	/**
	 * 使用volley框架 下载图片
	 * 
	 * @2015-11-16
	 */
	public static void DownLoadImage(RequestQueue mQueue, final int requestID,
			final String URL, final String fileName, final String dir,
			final CallBackFunction.DownLoadPortart callback,
			final JSONObject json) 
	{
		try{
		ImageRequest request = new ImageRequest(URL, new Listener<Bitmap>() {
			@Override
			public void onResponse(Bitmap response) {
				ImageTool.saveFile(response, fileName, dir);
				Log.i("show", dir + "/" + fileName + "下载文件成功");
				callback.doSomething(json);
			}
		}, 0, 0, Config.ARGB_8888, new ErrorListener() {
			@Override
			public void onErrorResponse(VolleyError error) {
				Log.e("201605151245", error.toString());
			}
		});
		if (!URL.equals("") || !URL.equals("null")) {
			Log.i("show", "下载文件进入队列");
			request.setTag(requestID);
			mQueue.add(request);
		}
		}catch(Exception e)
		{
			Log.e("201606071657",e.toString());
		}
	}

	
	public static void DownLoadImage(RequestQueue mQueue, final String URL,
			final String dir, final String fileName,
			final CallBackFunction callback,
			final CallBackFunction.DownLoadImage downloadcallback) {
		try {
			ImageRequest request = new ImageRequest(URL,
					new Listener<Bitmap>() {
						@Override
						public void onResponse(Bitmap response) {
							ImageTool.saveFile(response, fileName, dir);
							Log.e("show", dir + "/" + fileName + "下载文件成功");
							if(downloadcallback != null)
								downloadcallback.doSomething(dir + "/" + fileName);
							callback.sendMessage(dir + "/" + fileName, MacroCode.SUCCESS);
						}
					}, 0, 0, Config.ARGB_8888, new ErrorListener() {
						@Override
						public void onErrorResponse(VolleyError error) {
							Log.e("201608041654", error.toString());
							Log.e("show", dir + "/" + fileName + "下载文件失败");
							callback.sendMessage("", MacroCode.FAIL);
						}
					});
			if (!URL.equals("") || !URL.equals("null")) {
				mQueue.add(request);
				Log.e("show","开始下载"+URL);
			}
		} catch (Exception e) {
			Log.e("201608041653", e.toString());
		}
	}
	
	// 当网络异常,返回界面出错
	public static void ReturnExceptionJSON(CallBackFunction callback,
			int msgType) {
		String str = "{}";
		try {
			JSONObject json = new JSONObject();
			json.put("result", false);
			json.put("code", -999);
			str = json.toString();
		} catch (JSONException e1) {
			Log.e("201605141324", e1.toString());
		}
		callback.sendMessage(str, msgType);
	}
}
