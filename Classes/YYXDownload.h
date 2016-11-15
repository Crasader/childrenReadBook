#ifndef __YYXDownload_HH__
#define __YYXDownload_HH__
#include "cocos2d.h"
#include <stdio.h>
#include <sys/stat.h>
#include <thread> 
#include <mutex>
#include "FileNames.h"
#include "external/unzip/unzip.h"
//#include "SqliteManager.h"
#include "App.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "curl/include/win32/curl/curl.h"
#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "curl/include/android/curl/curl.h"
#endif

USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

class YYXDownload:public Ref
{
public:
	YYXDownload();
	~YYXDownload();
	//-------------------------------------------------------

	//对外应用接口
	//获取一个实例
	static YYXDownload* GetInstance();
	static YYXDownload* instance;
	//下载一个URL, 返回全局唯一的标记字符串
	string download(string url, string dir, string fileName, string beginstr, function<void(YYXStruct)> beginFunction, string downloadingstr, function<void(YYXStruct)> downloadingFunction, string endstr, function<void(YYXStruct)> endFunction);
	//停止下载一个任务, 传一个唯一的标记字符串
	void pause(string tag);
	//开始自动全负荷下载
	//void autoFullDownload();
	//暂停所有下载任务
	void pauseAll();
	//获取和设置下载队列长度
	void setDownloadMaxCount(int count);
	int getDownloadMaxCount();
	//获取任务标记字符串
	static string getTag(string path);
	//解压一个文件
	static bool uncompress(string zipDir, string zipName, string unzipDirPath, function<void(string str)> successRun, function<void(string str)> failRun);
	//查询任务是否在准备队列中
	bool isExitReadyQueue(string taskTag);
	//判断任务是否被暂停
	bool isPause(string taskTag);
	//开始所有下载线程
	void startAll();
	//判断是否在下载队列中
	bool isExitDownloadQueue(string taskTag);
	//线程池运行
	void downloadThreadRuning();
private:
	std::mutex down_mtx;
	//线程容器
	map<string, thread::id> threadData;
	//数据容器
	map<string, YYXStruct> data;
	//代码容器
	map<string, function<void(YYXStruct)>> functionMap;
	void setMapFunction(string key, function<void(YYXStruct)> runFunction);
	function<void(YYXStruct)> getMapFunction(string key);
	//有一个下载队列
	map<string, YYXStruct> downloadQueue;
	//有一个准备队列
	queue<string> readyQueue;
	map<string, string> readyQueueMap;
	//可以设置下载队列长度
	int downloadCount = 1;
	//准备队列无限长度
	//下载队列添加一个任务
	int addDownloadQueue(string task);
	//下载队列删除一个任务
	int deleteDownloadQueue(string task);
	//准备队列添加一个任务
	int addReadyQueue(string task);
	//准备队列删除一个任务
	int deleteReadyQueue(string task);
	//开始所有下载任务
	//void startAll();
	//开始一个下载任务,返回下载队列的数量
	//int startTask(string task);
	//一个任务下载前的动作
	void taskBegin(string tag);
	//一个任务下载进度中的动作
	static void taskDownloading(string taskTag, int progress);
	//一个任务下载结束后的动作
	void taskEnd(string taskTag, int resultCode);
	//下载一个文件
	void downLoadFile(string url, string path, const string& tag);
	//从准备队列取出第一个进入下载队列
	//void getReadyQueueFrontPushDownloadQueue();
	//下载文件写文件的回调
	static size_t pWriteCallback(void *pData, size_t n, size_t nDataSize, FILE *stream);
	//获取本地文件的大小（断点续载的需要）
	long getLocalFileSize(const char *filePath);
	//下载中的进度
	static int DownProgresss(void* clientp, double fDownLoadTotal, double fDownLoaded, double fUpTotal, double fUpLoaded);

	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||	
};
#endif //__YYXDownload_HH__