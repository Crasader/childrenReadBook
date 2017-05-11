//1.并发图片数量,同时几张图片同时下载
//2.先获取优先级高的
//3.延迟下载
//4.发送之后不暂停 不取消
#ifndef __YYXDownloadImages_HH__
#define __YYXDownloadImages_HH__
#include "App.h"

enum YYXDownloadImagesPriority
{
	high,
	normal,
	low
};
class YYXDownloadImagesData
{
private:
	string m_task = "";
	string m_url = "";
	string m_path = "";
	string m_dir = "";
	string m_fileName = "";
	long long m_startTime = -1;
	double m_netFileSize = 0;
	YYXDownloadImagesPriority m_priority;
	int m_delayTime = 0;
	function<void(string)>  m_callback = nullptr;
	function<void(string)>  m_callbackerror = nullptr;
	bool runing = false;
public:
	static YYXDownloadImagesData* create();
	static void del(YYXDownloadImagesData* data);

	std::string getUrl() const { return m_url; }
	YYXDownloadImagesData* setUrl(std::string val)
	{
		m_url = val;
		return this;
	}

	std::string getPath() const { return m_path; }

	YYXDownloadImagesData* setPath(std::string val)
	{
		m_path = val;
		return this;
	}

	function<void(string)> getCallback() const { return m_callback; }
	YYXDownloadImagesData* setCallback(const function<void(string)>& val)
	{
		m_callback = val;
		return this;
	}

	function<void(string)> getCallbackerror() const { return m_callbackerror; }

	YYXDownloadImagesData* setCallbackerror(const function<void(string)>& val)
	{
		m_callbackerror = val;
		return this;
	}
	int DelayTime() const { return m_delayTime; }

	YYXDownloadImagesData* DelayTime(int val)
	{
		m_delayTime = val;
		return this;
	}
	YYXDownloadImagesPriority Priority() const { return m_priority; }
	YYXDownloadImagesData* Priority(YYXDownloadImagesPriority val) 
	{
		m_priority = val;
		return this;
	}
	std::string Task() const { return m_task; }
	YYXDownloadImagesData* Task(std::string val)
	{
		m_task = val; 
		return this;
	}
	std::string getDir() const { return m_dir; }
	YYXDownloadImagesData* setDir(std::string val) 
	{
		m_dir = val; 	
		if (!FileUtils::getInstance()->isDirectoryExist(m_dir))
			FileUtils::getInstance()->createDirectory(m_dir);
		return this;
	}
	std::string getFileName() const { return m_fileName; }
	YYXDownloadImagesData* setFileName(std::string val)
	{
		m_fileName = val; 
		return this;
	}
	long long StartTime() const { return m_startTime; }
	void StartTime(long long val) { m_startTime = val; }
	double getNetFileSize() const { return m_netFileSize; }
	void setNetFileSize(double val) { m_netFileSize = val; }
	string ToString();
	bool getRuning() const { return runing; }
	void setRuning(bool val) { runing = val; }
};

class YYXDownloadImages:public Ref
{
public:
	mutex m;//同步

	bool m_start = false;//运行中
	bool getStart() const { return m_start; }
	void setStart(bool val) { 
		m_start = val; 
	}
	YYXDownloadImages();
	~YYXDownloadImages();
	static YYXDownloadImages* GetInstance();
	static YYXDownloadImages* instance;
	void newDownloadImage(string url, string dir, string filename,  YYXDownloadImagesPriority priority = normal,int delayTime = 0,  function<void(string)> callback = nullptr, function<void(string)> callbackerror = nullptr);
	string getTaskTag(string path);
	void getList();
	void startTask();
	int Concurrence() const { return m_concurrence; }
	void Concurrence(int val) { m_concurrence = val; }
	void clearAllReadyTask();
	YYXDownloadImagesData* getData(string taskTag);
private:
	map<string, YYXDownloadImagesData*> m_Tasks;
	//map<string, string> m_Tag;//任务标示存储 <path, taskname>
	//3条准备队列
	vector<string> highList;
	vector<string> normalList;
	vector<string> lowList;
	//1条正在下载队列
	map<string, YYXDownloadImagesData*> downloadList;
	float outTime =60;//超时时间
	int m_concurrence = 3;//并发数

	bool addDownloadListFormHighList();
	bool addDownloadListFormNormalList();
	void addDownloadListFormLowList();
	void downloadImage(YYXDownloadImagesData * data);
	void downloadImage_Curl(YYXDownloadImagesData *data);
	static size_t pWriteCallback(void *pData, size_t n, size_t nDataSize, FILE *stream);
	static int DownProgresss(void* clientp, double fDownLoadTotal, double fDownLoaded, double fUpTotal, double fUpLoaded);
	long getLocalFileSize(const char *filePath);
	void deleteTask(string taskTag);
	void addMaxTask();//增加并发
	void decreaseMaxTask();//减少并发
	void addOutTime();//增加超时时间
	void decreaseOutTime();//减少超时时间

	void change4OutTime();//超时一次
	void change4GoodTime();//快速下载一次
};
#endif 