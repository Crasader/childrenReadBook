#include "YYXDownloadImages.h"
#include "NetIntface.h"
#include "YYXTime.h"

YYXDownloadImages* YYXDownloadImages::instance = nullptr;

YYXDownloadImages::YYXDownloadImages()
{
}

YYXDownloadImages::~YYXDownloadImages()
{
}

YYXDownloadImages* YYXDownloadImages::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new YYXDownloadImages();
	}
	return instance;
}

void YYXDownloadImages::newDownloadImage(string url, string dir, string filename, YYXDownloadImagesPriority priority /*= normal*/, int delayTime /*= 0*/, function<void(string)> callback /*= nullptr*/, function<void(string)> callbackerror /*= nullptr*/)
{
	auto data = new YYXDownloadImagesData();
	string path = dir + "/" + filename;
	App::log("YYXDownloadImages::newDownloadImage ===>>>" + path);
	string taskTag = getTaskTag(path);
	data->setUrl(url)->setPath(path)->Priority(priority)->setDir(dir)->setFileName(filename)->Task(taskTag)->setCallback([=](string downpath) {
		if (callback)
			callback(downpath);
		auto nowtime = YYXTime::getInstance()->getNowTime4S();
		auto httptime = nowtime - data->StartTime();
		if (httptime < outTime)
		{
			change4GoodTime();
		}
		deleteTask(taskTag);
	})->setCallbackerror([=](string error) {		
		if (callbackerror)
			callbackerror(error);
		change4OutTime();
		deleteTask(taskTag);
	});
	data->retain();
	data->autorelease();
	m_Tasks[taskTag] = data;
	switch (priority)
	{
	case high:
		highList.push_back(taskTag);
		break;
	case normal:
		normalList.push_back(taskTag);
		break;
	case low:
		lowList.push_back(taskTag);
		break;
	}
	App::GetInstance()->addMessage();
}


std::string YYXDownloadImages::getTaskTag(string path)
{
	string taskTag = "";
	if (m_Tag.count(path) > 0)
	{
		taskTag = m_Tag[path];
	}
	else
	{
		taskTag = StringUtils::format("%d_downloadImage_", YYXTime::getInstance()->getRandomL()) + path;
	}
	return taskTag;
}

void YYXDownloadImages::getList()
{
	for (auto it : downloadList)
	{
		string name = it.first;
		string path = it.second->getPath();
		App::log(name + "===>>" + path);
	}
}

void YYXDownloadImages::startTask()
{
	if (downloadList.size() > 0)
	{
		vector<string> data;
		for (auto it : downloadList)
		{
			auto task = it.second;
			if (task)
			{
				auto timestart = task->StartTime();
				if (timestart <= 0)
				{
					timestart = YYXLayer::getCurrentTime4Second();
					task->StartTime(timestart);
				}
				auto nowtime = YYXLayer::getCurrentTime4Second() - timestart;
				if (nowtime > outTime)
				{
					data.push_back(task->Task());
					change4OutTime();
				}
			}
			else
				data.push_back(it.first);
		}
		for (auto it : data)
		{
			deleteTask(it);
			//cocos2d::network::HttpClient::getInstance()->destroyInstance();
			App::log(it+"  ==> deleteTask outTime ", outTime);
		}
	}
	if (downloadList.size() < m_concurrence)
	{
		bool full = addDownloadListFormHighList();
		if (!full)
		{
			bool isFull = addDownloadListFormNormalList();
			if (!isFull)
			{
				addDownloadListFormLowList();
			}
		}
	}
	if (downloadList.size() > 0)
	{
		setStart(true);
	}
	else
	{
		setStart(false);
	}
	auto c = downloadList.size();
	if (c>0)
	{
		App::log("YYXDownloadImages::startTask() =========================>>> downloadList.size() =",c );
		App::log("YYXDownloadImages::startTask() =========================>>> outTime =", outTime);
		App::log("YYXDownloadImages::startTask() =========================>>>  m_concurrence(bingfa)=", m_concurrence);
	}
}

void YYXDownloadImages::clearAllReadyTask()
{
	m.lock();
	vector<string> dels;
	for (auto it : m_Tasks)
	{
		string key = it.first;
		auto data = it.second;
		auto res = downloadList.find(key);
		if (res == downloadList.end() && data)
		{
			data->release();
			data = nullptr;
			dels.push_back(key);
		}
	}
	for (auto item : dels)
	{
		m_Tasks.erase(item);
	}
	m.unlock();
	highList.clear();
	normalList.clear();
	lowList.clear();
}

bool YYXDownloadImages::addDownloadListFormHighList()
{
	int count = m_concurrence - downloadList.size();
	for (int i = 0; i < count; i++)
	{
		if (!highList.empty())
		{
			auto deat = highList.back();
			auto data = m_Tasks[deat];
			downloadImage(data);
			downloadList[deat] = data;
			if(!highList.empty())
				highList.pop_back();			
			App::log("YYXDownloadImages::addDownloadListFormHighList() ==>>"+ deat);
		}
	}
	if (downloadList.size() < m_concurrence)
		return false;
	else
		return true;
}

bool YYXDownloadImages::addDownloadListFormNormalList()
{
	int count = m_concurrence - downloadList.size();
	for (int i = 0; i < count; i++)
	{
		if (!normalList.empty())
		{
			auto deat = normalList.back();
			auto data = m_Tasks[deat];
			downloadImage(data);
			downloadList[deat] = data;
			normalList.pop_back();
			App::log("YYXDownloadImages::addDownloadListFormNormalList() ==>>" + deat);
		}
	}
	if (downloadList.size() < m_concurrence)
		return false;
	else
		return true;
}

void YYXDownloadImages::addDownloadListFormLowList()
{
	int count = m_concurrence - downloadList.size();
	for (int i = 0; i < count; i++)
	{
		if (!lowList.empty())
		{
			auto deat = lowList.back();
			auto data = m_Tasks[deat];
			downloadImage(data);
			downloadList[deat] = data;
			lowList.pop_back();
			App::log("YYXDownloadImages::addDownloadListFormLowList() ==>>" + deat);
		}
	}
}

void YYXDownloadImages::downloadImage(YYXDownloadImagesData * data)
{
	if (data)
	{
		thread mythread([=]() {
			App::ccsleep(data->DelayTime());
			data->StartTime(YYXTime::getInstance()->getNowTime4S());
			//NetIntface::DownLoadImage(data->getUrl(), data->Dir(), data->getFileName(), "", data->getCallback(), "", data->getCallbackerror());
			downloadImage_Curl(data);
		});
		mythread.detach();
	}
}

void YYXDownloadImages::downloadImage_Curl(YYXDownloadImagesData *downDate)
{
	if (downDate == nullptr)
		return;
	CURL *pCurl = nullptr;
	string url = downDate->getUrl();
	auto callback = downDate->getCallback();
	auto callbackerror = downDate->getCallbackerror();
	string path = downDate->getPath();

	pCurl = curl_easy_init();//初始化CURL取得初始化成功后的CURL指针
	if (pCurl == nullptr)
	{
		if (callbackerror)
			callbackerror("");
		return;
	}
	FILE *pFile = nullptr;
	pFile = fopen(path.c_str(), "ab+");
	if (pFile == nullptr || url.empty())
	{
		if (callbackerror)
			callbackerror("");
		return;
	}
	curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(pCurl, CURLOPT_FILE, pFile);                  //指定写入的文件指针
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, pWriteCallback);//设置写数据的回调函数
	curl_easy_setopt(pCurl, CURLOPT_VERBOSE, true);                //让CURL报告每一件意外的事情
	curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10000);                 //设置超时时间
	curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, false);
	curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION, DownProgresss);//指定显示进度的回调函数
	curl_easy_setopt(pCurl, CURLOPT_RESUME_FROM, getLocalFileSize(path.c_str()));// 从本地大小位置进行请求数据
	curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA, downDate);//DownProgresss函数的第一个参数	
	CURLcode nResCode = curl_easy_perform(pCurl);//执行上面设定的动作并返回状态码
	curl_easy_cleanup(pCurl);//释放相关资源
	fclose(pFile);
	App::GetInstance()->addMessage();
	if (nResCode == CURLcode::CURLE_OK)
	{
		auto size = FileUtils::getInstance()->getFileSize(path);
		if (size == downDate->getNetFileSize())
		{
			if (callback)
				callback(path);
			return;
		}
	}
	if (FileUtils::getInstance()->isFileExist(path))
		FileUtils::getInstance()->removeFile(path);
	if (callbackerror)
		callbackerror(Value(nResCode).asString());
}

size_t YYXDownloadImages::pWriteCallback(void *pData, size_t n, size_t nDataSize, FILE *stream)
{
	size_t nWritten = fwrite(pData, n, nDataSize, (FILE *)stream);
	return nWritten;
}

int YYXDownloadImages::DownProgresss(void* clientp, double fDownLoadTotal, double fDownLoaded, double fUpTotal, double fUpLoaded)
{
	auto downDate = (YYXDownloadImagesData*)clientp;//传过来的自定义的参数
	downDate->setNetFileSize(fDownLoadTotal);
	int pragress = fDownLoaded / fDownLoadTotal *100.0;
	return 0;
}

long YYXDownloadImages::getLocalFileSize(const char *filePath)
{
	if (!FileUtils::getInstance()->isFileExist(filePath))
		return 0;
	FILE * pFile = fopen(filePath, "rb");
	fseek(pFile, 0, SEEK_END);
	long size = ftell(pFile);
	fclose(pFile);
	return size;
}

void YYXDownloadImages::deleteTask(string taskTag)
{
	m.lock();
	auto data = m_Tasks[taskTag];
	if (data)
	{
		data->release();
		data = nullptr;
	}
	downloadList[taskTag] = nullptr;
	m_Tasks[taskTag] = nullptr;
	downloadList.erase(taskTag);
	m_Tasks.erase(taskTag);
	m.unlock();
}

void YYXDownloadImages::addMaxTask()
{
	m_concurrence += 1;
	if (m_concurrence > 6)
	{
		m_concurrence = 6;
	}
}

void YYXDownloadImages::decreaseMaxTask()
{
	m_concurrence = 3;
}

void YYXDownloadImages::change4OutTime()
{
	decreaseMaxTask();
	//addOutTime();
	clearAllReadyTask();
}

void YYXDownloadImages::change4GoodTime()
{
	addMaxTask();
	//decreaseOutTime();
}

void YYXDownloadImages::addOutTime()
{
	outTime = outTime * 2.0;
	if (outTime>120)
	{
		outTime = 120;
	}
	else if (outTime <15)//减小到非常小的时候 突然增加时间 恢复初始值
	{
		outTime = 40;
	}
}

void YYXDownloadImages::decreaseOutTime()
{
	outTime = outTime / 2.0;
	if (outTime < 5)
	{
		outTime = 5;
	}
	else if (outTime >100)//增大到非常大的时候 突然正常下载了 恢复初始值
	{
		outTime = 40;
	}
}
