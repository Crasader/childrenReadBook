#include "YYXDownloadImages.h"
#include "CrossPlatform.h"
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
	auto data = YYXDownloadImagesData::create();
	string path = dir + "/" + filename;
	string taskTag = getTaskTag(path);
	data->setUrl(url)->setPath(path)->Priority(priority)->setDir(dir)->setFileName(filename)->Task(taskTag)->setCallback(callback)->setCallbackerror(callbackerror);
	auto it = m_Tasks.find(taskTag);
	if (it != m_Tasks.end())
	{
		YYXDownloadImagesData::del(data);
		return;
	}
	else
		m_Tasks[taskTag] = data;
	App::log("YYXDownloadImages::newDownloadImage ===>>>" + path);
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
	//if (m_Tag.count(path) > 0)
	//{
		//taskTag = m_Tag[path];
	//}
	//else
	//{
		taskTag = /*StringUtils::format("%d_downloadImage_", YYXTime::getInstance()->getRandomL()) +*/ path;
	//}
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
	if (c>=0)
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
	for (auto it : lowList)
	{
		auto readytask = m_Tasks.find(it);
		if (readytask != m_Tasks.end())
		{
			auto delData = readytask->second;
			YYXDownloadImagesData::del(delData);
			m_Tasks[it] = nullptr;
			m_Tasks.erase(it);
		}
	}
	for (auto he : normalList)
	{
		auto task = m_Tasks.find(he);
		if (task != m_Tasks.end())
		{
			auto taskData = task->second;
			YYXDownloadImagesData::del(taskData);
			m_Tasks[he] = nullptr;
			m_Tasks.erase(he);
		}
	}
	m.unlock();
	normalList.clear();
	lowList.clear();
}

YYXDownloadImagesData * YYXDownloadImages::getData(string taskTag)
{
	YYXDownloadImagesData* data = nullptr;
	auto it = m_Tasks.find(taskTag);
	if (it != m_Tasks.end())
	{
		data = it->second;
	}
	return data;
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
		if (data->getRuning())
			return;
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
	downDate->setRuning(true);
	CURL *pCurl = nullptr;
	string url = downDate->getUrl();
	function<void(string)> callback = [](string downpath) {
		auto download = YYXDownloadImages::GetInstance();
		string taskTag = download->getTaskTag(downpath);
		auto downDate = download->getData(taskTag);
		if (downDate)
		{
			auto call = downDate->getCallback();
			if (call)
				call(downpath);
			auto nowtime = YYXTime::getInstance()->getNowTime4S();
			auto httptime = nowtime - downDate->StartTime();
			if (httptime < download->outTime)
				download->change4GoodTime();
		}
		download->deleteTask(taskTag);
	};
	function<void(string)> callbackerror = [](string downpath) {
		auto download = YYXDownloadImages::GetInstance();
		string taskTag = download->getTaskTag(downpath);
		auto downDate = download->getData(taskTag);
		if (downDate)
		{
			auto call = downDate->getCallbackerror();
			if (call)
				call(downpath);
			YYXDownloadImages::GetInstance()->change4OutTime();
		}		
		YYXDownloadImages::GetInstance()->deleteTask(taskTag);
	}; 
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
	string str = "download over  v|v|v|v __________________  " + path+"\n";
	YYXLayer::writeFilepp(str, FileUtils::getInstance()->getWritablePath()+"temp/OOM.txt");
	if (nResCode == CURLcode::CURLE_OK)
	{
		auto size = FileUtils::getInstance()->getFileSize(path);
		if (size == downDate->getNetFileSize() && App::initImage(path))
		{
			if (callback)
				callback(path);
			return;
		}
	}
	if (FileUtils::getInstance()->isFileExist(path))
		FileUtils::getInstance()->removeFile(path);
	if (callbackerror)
		callbackerror(path);
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
	YYXDownloadImagesData::del(data);
	m_Tasks[taskTag] = nullptr;
	downloadList[taskTag] = nullptr;
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

YYXDownloadImagesData * YYXDownloadImagesData::create()
{
	return new YYXDownloadImagesData();
}

void YYXDownloadImagesData::del(YYXDownloadImagesData * data)
{
	if (data)
		delete data;
	data = nullptr;
}

string YYXDownloadImagesData::ToString()
{
	map<string, string>data;
	data["m_task"] = m_task;
	data["m_url"] = m_url;
	data["m_path"] = m_path;
	data["m_dir"] = m_dir;
	data["m_fileName"] = m_fileName;
	return	YYXLayer::getStringFormMap(data);;
}
