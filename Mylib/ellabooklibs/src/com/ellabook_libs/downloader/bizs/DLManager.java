package com.ellabook_libs.downloader.bizs;

import android.content.Context;
import android.util.Log;

import com.ellabook_libs.downloader.cons.HttpConnPars;
import com.ellabook_libs.downloader.cons.PublicCons;
import com.ellabook_libs.downloader.entities.TaskInfo;
import com.ellabook_libs.downloader.entities.ThreadInfo;
import com.ellabook_libs.downloader.interfaces.DLTaskListener;
import com.ellabook_libs.downloader.interfaces.IDLThreadListener;
import com.ellabook_libs.downloader.utils.FileUtil;
import com.ellabook_libs.downloader.utils.NetUtil;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.zip.ZipException;

import org.apache.http.HttpStatus;

/**
 * 下载管理器
 * Download manager
 * 执行具体的下载操作
 *
 *         开始一个下载任务只需调用{@link #dlStart}方法即可
 *         停止某个下载任务需要调用{@link #dlStop}方法 停止下载任务仅仅会将对应下载任务移除下载队列而不删除相应数据 下次启动相同任务时会自动根据上一次停止时保存的数据重新开始下载
 *         取消某个下载任务需要调用{@link #dlCancel}方法 取消下载任务会删除掉相应的本地数据库数据但文件不会被删除
 *         相同url的下载任务视为相同任务
 *         对不支持断点下载的文件直接使用单线程下载 该操作将不会插入数据库
 */
public final class DLManager {

	private static DLManager sManager;
	private static Hashtable<String, DLTask> sTaskDLing;
	private List<String> bookIdList = new ArrayList<String>();
	private static DBManager sDBManager;

	private ExecutorService mExecutor;
	private Context dlcontext;

	private boolean canDownInNoWifi;
	public DLManager(Context context ,int THREAD_POOL_SIZE,boolean canDownInNoWifi) {
		this.dlcontext = context;
		this.mExecutor = Executors.newFixedThreadPool(THREAD_POOL_SIZE);
		this.canDownInNoWifi=canDownInNoWifi;
		sDBManager = DBManager.getInstance(context);
		sTaskDLing = new Hashtable<String, DLTask>();

	}

	public static DLManager getInstance(Context context ,int THREAD_POOL_SIZE,boolean canDownInNoWifi) {
		if (null == sManager) {
			sManager = new DLManager(context,THREAD_POOL_SIZE,canDownInNoWifi);
		}
		return sManager;
	}

	public void dlStart(String bookid ,String url, String dirPath,String unZipPath, DLTaskListener listener) {
		DLPrepare dlPrepare = new DLPrepare(bookid,url, dirPath,unZipPath, listener);
		mExecutor.execute(dlPrepare);
	}

	public void dlStop(String bookid) {
		if (sTaskDLing.containsKey(bookid)) {
			DLTask task = sTaskDLing.get(bookid);
			task.setStop(true);
		}
	}

	//		public void dlCancel(String bookid) {
	//			dlStop(bookid);
	//			if (null != sDBManager.queryTaskInfoByBookId(bookid)) {
	//				sDBManager.deleteTaskInfo(bookid);
	//				List<ThreadInfo> infos = sDBManager.queryThreadInfos(bookid);
	//				if (null != infos && infos.size() != 0) {
	//					sDBManager.deleteThreadInfos(bookid);
	//				}
	//			}
	//		}

	private class DLPrepare implements Runnable {
		private String url, dirPath,bookid,unZipPath;// 下载路径和保存目录
		private DLTaskListener listener;// 下载监听器

		private DLPrepare(String bookid ,String url, String dirPath,String unZipPath , DLTaskListener listener) {
			this.bookid = bookid;
			this.url = url;
			this.dirPath = dirPath;
			this.listener = listener;
			this.unZipPath=unZipPath;
		}

		@Override
		public void run() {
			HttpURLConnection conn = null;
			try {
				String realUrl = url;
				conn = NetUtil.buildConnection(url);
				conn.setInstanceFollowRedirects(false);
				conn.setRequestProperty(HttpConnPars.REFERER.content, url);
				if (conn.getResponseCode() == HttpStatus.SC_MOVED_TEMPORARILY ||
						conn.getResponseCode() == HttpStatus.SC_MOVED_PERMANENTLY) {
					realUrl = conn.getHeaderField(HttpConnPars.LOCATION.content);
				}
				// 如果文件正在下载
				if (sTaskDLing.containsKey(bookid)) {
					// 文件正在下载 File is downloading
				} else {
					TaskInfo info = sDBManager.queryTaskInfoByBookId(bookid);
					String fileName = FileUtil.getFileNameFromUrl(realUrl).replace("/", "");
					if (null != listener) listener.onStart(fileName, realUrl);
					File file = new File(dirPath, fileName);
					if (null == info || !file.exists()) {
						File filezip =FileUtil.createFile(dirPath, fileName);
						info = new TaskInfo(bookid,filezip, url, realUrl, 0, 0 ,0);
					}
					DLTask task = new DLTask(info,unZipPath, listener);
					mExecutor.execute(task);
				}
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				if (null != conn) {
					conn.disconnect();
				}
			}
		}
	}

	private class DLTask implements Runnable, IDLThreadListener {
		private static final int LENGTH_PER_THREAD = 2097152;
		private String unZipPath;
		private TaskInfo info;
		private DLTaskListener mListener;

		private int totalProgress, fileLength;
		private int totalProgressIn100;
		private boolean isResume;
		private boolean isStop;
		private boolean isExists;
		private boolean isConnect = true;

		private List<ThreadInfo> mThreadInfos;

		private DLTask(TaskInfo info , String unZipPath, DLTaskListener listener) {
			this.info = info;
			this.mListener = listener;
			this.totalProgress = info.progress;
			this.fileLength = info.length;
			this.unZipPath=unZipPath;

			if (null != sDBManager.queryTaskInfoByBookId(info.bookId)) {
				if (!info.dlLocalFile.exists()) {
					sDBManager.deleteTaskInfo(info.bookId);
				}
				mThreadInfos = sDBManager.queryThreadInfos(info.bookId);
				if (null != mThreadInfos && mThreadInfos.size() != 0) {
					isResume = true;
				} else {
					sDBManager.deleteTaskInfo(info.bookId);
				}
			}
		}

		public void setStop(boolean isStop) {
			this.isStop = isStop;
		}

		@Override
		public void run() {
			if (NetUtil.getNetWorkType(dlcontext) == PublicCons.NetType.INVALID) {
				if (null != mListener)
					mListener.onConnect(PublicCons.NetType.INVALID, "无网络连接");
				isConnect = false;
			}else if (NetUtil.getNetWorkType(dlcontext) == PublicCons.NetType.NO_WIFI) {
				if(canDownInNoWifi){
					if (null != mListener)
						isConnect = mListener.onConnect(PublicCons.NetType.NO_WIFI, "正在使用非WIFI网络下载");
				}else{
					if (null != mListener)
						isConnect = mListener.onConnect(PublicCons.NetType.NO_WIFI, "正在使用非WIFI网络下载");
					isConnect = false;
				}

			}
			if (isConnect) {
				bookIdList.add(info.bookId);
				sTaskDLing.put(info.bookId, this);
				if (isResume) {
					for (ThreadInfo i : mThreadInfos) {
						mExecutor.execute(new DLThread(i, this));
					}
				} else {
					HttpURLConnection conn = null;
					try {
						conn = NetUtil.buildConnection(info.realUrl);
						conn.setRequestProperty("Range", "bytes=" + 0 + "-" + Integer.MAX_VALUE);
						if (conn.getResponseCode() == HttpStatus.SC_PARTIAL_CONTENT) {
							fileLength = conn.getContentLength();
							if (info.dlLocalFile.exists() && info.dlLocalFile.length() == fileLength) {
								isExists = true;
								sTaskDLing.remove(info.bookId);
								bookIdList.remove(info.bookId);
								if (null != mListener) mListener.onFinish(info);
							}
							if (!isExists) {
								info.length = fileLength;
								sDBManager.insertTaskInfo(info);
								int threadSize;
								int length = LENGTH_PER_THREAD;
								if (fileLength <= LENGTH_PER_THREAD) {
									threadSize = 3;
									length = fileLength / threadSize;
								} else {
									threadSize = fileLength / LENGTH_PER_THREAD;
								}
								int remainder = fileLength % length;
								for (int i = 0; i < threadSize; i++) {
									int start = i * length;
									int end = start + length - 1;
									if (i == threadSize - 1) {
										end = start + length + remainder;
									}
									String id = UUID.randomUUID().toString();
									ThreadInfo ti = new ThreadInfo(info.bookId,info.dlLocalFile,
											info.baseUrl, info.realUrl, start, end, id);

									mExecutor.execute(new DLThread(ti, this));
								}
							}
						} else if (conn.getResponseCode() == HttpStatus.SC_OK) {
							fileLength = conn.getContentLength();
							if (info.dlLocalFile.exists() && info.dlLocalFile.length() == fileLength) {
								sTaskDLing.remove(info.bookId);
								bookIdList.remove(info.bookId);
								if (null != mListener) mListener.onFinish(info);
							} else {
								ThreadInfo ti = new ThreadInfo(info.bookId,info.dlLocalFile, info.baseUrl,
										info.realUrl, 0, fileLength, UUID.randomUUID().toString());
								mExecutor.execute(new DLThread(ti, this));
							}
						}
					} catch (Exception e) {
						if (null != sDBManager.queryTaskInfoByBookId(info.bookId)) {
							info.progress = totalProgress;
							sDBManager.updateTaskInfo(info);
							sTaskDLing.remove(info.bookId);
							bookIdList.remove(info.bookId);
						}
						if (null != mListener) mListener.onError(e.getMessage());
					} finally {
						if (conn != null) {
							conn.disconnect();
						}
					}
				}
			}
		}

		@Override
		public void onThreadProgress(int progress) {
			synchronized (this) {
				totalProgress += progress;
				int tmp = (int) (totalProgress * 1.0 / fileLength * 100);
				if (null != mListener && tmp != totalProgressIn100) {
					mListener.onProgress(tmp , "下载中");
					totalProgressIn100 = tmp;
				}
				if (fileLength == totalProgress) {

					sDBManager.deleteTaskInfo(info.bookId);
					sTaskDLing.remove(info.bookId);
					bookIdList.remove(info.bookId);

					info.isfinish = 1;
					sDBManager.finishTaskInfo(info);
					/*
					 * 解压方法，参数一，压缩包路径，参数二，解压路径
					 */
					try {
						mListener.onProgress(tmp , "解压中");
						ZipUtil.upZipFile(info.dlLocalFile,unZipPath);
						mListener.onProgress(tmp , "解压完成");
						if (null != mListener) mListener.onFinish(info);
					} catch (ZipException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
						Log.d("ZipUtil", "zip文件错误");
						mListener.onProgress(tmp , "zip文件错误,解压失败");
						mListener.onUnZipFail("zip文件错误,解压失败");
						String s = info.dlLocalFile.getPath();
						

					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
						Log.d("ZipUtil", "io错误");
						mListener.onProgress(tmp , "解压出错啦");
						mListener.onUnZipFail("解压出错啦");
						String s = info.dlLocalFile.getPath();
						
					}
					
				}
				if (isStop) {
					info.progress = totalProgress;
					sDBManager.updateTaskInfo(info);
					sTaskDLing.remove(info.bookId);
					bookIdList.remove(info.bookId);
				}
			}
		}

		private class DLThread implements Runnable {
			private ThreadInfo info;
			private IDLThreadListener mListener;

			private int progress;

			public DLThread(ThreadInfo info, IDLThreadListener listener) {
				this.info = info;
				this.mListener = listener;
			}

			@Override
			public void run() {
				HttpURLConnection conn = null;
				RandomAccessFile raf = null;
				InputStream is = null;
				try {
					conn = NetUtil.buildConnection(info.realUrl);
					conn.setRequestProperty("Range", "bytes=" + info.start + "-" + info.end);

					raf = new RandomAccessFile(info.dlLocalFile,
							PublicCons.AccessModes.ACCESS_MODE_RWD);
					if (conn.getResponseCode() == HttpStatus.SC_PARTIAL_CONTENT) {
						if (!isResume) {
							sDBManager.insertThreadInfo(info);
						}
						is = conn.getInputStream();
						raf.seek(info.start);
						int total = info.end - info.start;
						byte[] b = new byte[1024];
						int len;
						while (!isStop && (len = is.read(b)) != -1) {
							raf.write(b, 0, len);
							progress += len;
							mListener.onThreadProgress(len);
							if (progress >= total) {
								sDBManager.deleteThreadInfoById(info.id);
							}
						}
						if (isStop && null != sDBManager.queryThreadInfoById(info.id)) {
							info.start = info.start + progress;
							sDBManager.updateThreadInfo(info);
						}
					} else if (conn.getResponseCode() == HttpStatus.SC_OK) {
						is = conn.getInputStream();
						raf.seek(info.start);
						byte[] b = new byte[1024];
						int len;
						while (!isStop && (len = is.read(b)) != -1) {
							raf.write(b, 0, len);
							mListener.onThreadProgress(len);
						}
					}
				} catch (Exception e) {
					if (null != sDBManager.queryThreadInfoById(info.id)) {
						info.start = info.start + progress;
						sDBManager.updateThreadInfo(info);
					}
				} finally {


					try {
						if (null != is) {
							is.close();
						}
						if (null != raf) {
							raf.close();
						}
					} catch (IOException e) {
						e.printStackTrace();
					}
					if (null != conn) {
						conn.disconnect();
					}
				}
			}
		}
	}


}
