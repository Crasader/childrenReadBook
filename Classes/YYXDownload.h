#ifndef __YYXDownload_HH__
#define __YYXDownload_HH__
#include "cocos2d.h"
#include <stdio.h>
#include <sys/stat.h>
#include <thread> 
#include <mutex>
#include "FileNames.h"
#include "external/unzip/unzip.h"
#include "App.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "curl/include/win32/curl/curl.h"
#pragma  comment(lib,"libcurl_imp.lib")
#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "curl/include/android/curl/curl.h"
#endif
#include <atomic>

//下载状态 0=下载 1=准备下载   2=暂停  3=下载结束 -1=提前创建任务标示
USING_NS_CC_EXT;
USING_NS_CC;
using namespace std;

enum YYXDownloadStatus
{
	_download = 0,
	_ready = 1,
	_pause=2,
	_over=3,
	_null=-1
};

class DownLoadFileData
{
private:
	string m_Url = "";
	string m_Dir = "";
	string m_fileName = "";
	int m_beginKey = -999;
	int m_downloadingKey = -999;
	int m_endKey = -999;
	YYXDownloadStatus m_status = _null;
	std::atomic_bool m_threadRuning;
	int m_pausePint = 0;//暂停的百分比
	Value m_useData = Value(-999);
public:
	DownLoadFileData();
	~DownLoadFileData();

	static DownLoadFileData* create();
	static void del(DownLoadFileData* data);

	std::string getUrl() const { return m_Url; }
	DownLoadFileData* setUrl(std::string val) {
		m_Url = val;
		return this;
	}

	std::string getDir() const { return m_Dir; }
	DownLoadFileData* setDir(std::string val) {
		m_Dir = val;
		return this;
	}

	std::string getFileName() const { return m_fileName; }
	DownLoadFileData* setFileName(std::string val) {
		m_fileName = val;
		return this;
	}

	YYXDownloadStatus getStatus() const { return m_status; }
	DownLoadFileData* setStatus(YYXDownloadStatus val) {
		m_status = val;
		return this;
	}

	std::string getTag() const { return m_Dir + "/" + m_fileName; }

	std::string getPath() const { return m_Dir + "/" + m_fileName; }

	function<void(DownLoadFileData*)> getBeginFunc();
	void setBeginFunc(const function<void(DownLoadFileData*)>& val);

	function<void(DownLoadFileData*)> getDownloadingFunc();
	void setDownloadingFunc(const function<void(DownLoadFileData*)>& val);

	function<void(DownLoadFileData*)> getEndFunc();
	void setEndFunc(const function<void(DownLoadFileData*)>& val);

	bool getThreadRuning() { return m_threadRuning; }
	void setThreadRuning(bool val) { m_threadRuning = val; }
	int getPausePint() const { return m_pausePint; }
	void setPausePint(int val) { m_pausePint = val; }
	cocos2d::Value getUseData() const { return m_useData; }
	void setUseData(cocos2d::Value val) { m_useData = val; }
};

class YYXDownload:public Ref
{
public:
	YYXDownload();
	~YYXDownload();
	//获取一个实例
	static YYXDownload* getInstance();
	static YYXDownload* instance;

private:
	std::unordered_map<int, function<void(DownLoadFileData*)>> m_callbackFuncs;
	std::unordered_map<string, DownLoadFileData*> m_allTasks;
	queue<string> m_readyQueue;
	std::unordered_map<string, int> m_downloadList;
	//可以设置下载队列长度
	int downloadCount = 1;

	//m_callbackFuncs同步
	mutex m_Mcallback;
	//下载队列的增 pop 数量
	mutex m_Mdownloadlist;
	void addDownloadList(string tag);
	void delDownloadList(string tag);
	int getDownloadingCount();

	//准备队列的增 pop
	mutex m_Mreadylist;
	void addReadyList(string tag);
	string popReadyList();
	int getReadyCount();

	//从任务详情表m_allTsks增删改查任务
	mutex m_Malllist;
	void addTask(DownLoadFileData*);
	void delTask(string tag);

	//下载
	void curlDownloadFile(DownLoadFileData* data);
	static int DownProgress(void* clientp, double fDownLoadTotal, double fDownLoaded, double fUpTotal, double fUpLoaded);
	//下载文件写文件的回调
	static size_t pWriteCallback(void *pData, size_t n, size_t nDataSize, FILE *stream);
	//获取本地文件的大小（断点续载的需要）
	long getLocalFileSize(const char *filePath);
public:
	void newDownloadFile(DownLoadFileData* data);
	//开始所有下载线程
	void startAll();
	//停止下载一个任务, 传一个唯一的标记字符串
	void pause(string tag);
	//暂停所有下载任务
	void pauseAll();
	//删除所有任务
	void deleteAll();
	//获取和设置下载队列长度
	void setDownloadMaxCount(int count);
	int getDownloadMaxCount();
	//解压一个文件
	static bool uncompress(string zipDir, string zipName, string unzipDirPath, function<void(string str)> successRun, function<void(string str)> failRun);

	void addFunc(int, const function<void(DownLoadFileData*)>&);
	function<void(DownLoadFileData*)> getFunc(int);
	void delFunc(int);

	DownLoadFileData* getTask(string tag);
};
#endif //__YYXDownload_HH__