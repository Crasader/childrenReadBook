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
class YYXDownloadImagesData :public Ref
{
	string m_task = "";
	string m_url = "";
	string m_path = "";
	string m_dir = "";
	string m_fileName = "";
	long long m_startTime = -1;
	YYXDownloadImagesPriority m_priority;
	int m_delayTime = 0;
	function<void(string)>  m_callback = nullptr;
	function<void(string)>  m_callbackerror = nullptr;
public:
	std::string Url() const { return m_url; }
	YYXDownloadImagesData* Url(std::string val)
	{
		m_url = val;
		return this;
	}

	std::string Path() const { return m_path; }

	YYXDownloadImagesData* Path(std::string val)
	{
		m_path = val;
		return this;
	}

	function<void(string)> Callback() const { return m_callback; }
	YYXDownloadImagesData* Callback(function<void(string)> val)
	{
		m_callback = val;
		return this;
	}

	function<void(string)> Callbackerror() const { return m_callbackerror; }

	YYXDownloadImagesData* Callbackerror(function<void(string)> val)
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
	std::string Dir() const { return m_dir; }
	YYXDownloadImagesData* Dir(std::string val) 
	{
		m_dir = val; 	
		return this;
	}
	std::string FileName() const { return m_fileName; }
	YYXDownloadImagesData* FileName(std::string val)
	{
		m_fileName = val; 
		return this;
	}
	long long StartTime() const { return m_startTime; }
	void StartTime(long long val) { m_startTime = val; }
};

class YYXDownloadImages:public Ref
{
public:
	mutex m;//同步
	int m_concurrence = 4;//并发数
	int outTime = 40;//超时时间
	int Concurrence() const { return m_concurrence; }
	void Concurrence(int val) { m_concurrence = val; }
	bool m_start;//运行中
	bool Start() const { return m_start; }
	void Start(bool val) { 
		m_start = val; 
	}
	YYXDownloadImages();
	~YYXDownloadImages();
	static YYXDownloadImages* GetInstance();
	static YYXDownloadImages* instance;
	void newDownloadImage(string url, string dir, string filename,  YYXDownloadImagesPriority priority = normal,int delayTime = 0,  function<void(string)> callback = nullptr, function<void(string)> callbackerror = nullptr);
	string getTaskTag(string path);
	void getList();

private:
	map<string, YYXDownloadImagesData*> m_Tasks;
	map<string, string> m_Tag;//任务标示存储
	//3条准备队列
	vector<string> highList;
	vector<string> normalList;
	vector<string> lowList;
	//1条正在下载队列
	map<string, YYXDownloadImagesData*> downloadList;
	void startTask();
	bool addDownloadListFormHighList();
	bool addDownloadListFormNormalList();
	void addDownloadListFormLowList();
	void downloadImage(YYXDownloadImagesData * data);
	void deleteTask(string taskTag);
};
#endif 