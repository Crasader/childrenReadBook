#include "YYXDownloadImages.h"
#include "NetIntface.h"

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
		instance = new YYXDownloadImages();
	return instance;
}

void YYXDownloadImages::newDownloadImage(string url, string dir, string filename, YYXDownloadImagesPriority priority /*= normal*/, int delayTime /*= 0*/, function<void(string)> callback /*= nullptr*/, function<void(string)> callbackerror /*= nullptr*/)
{
	auto data = new YYXDownloadImagesData();
	string path = dir + "/" + filename;
	App::log("YYXDownloadImages::newDownloadImage ===>>>" + path);
	string taskTag = getTaskTag(path);
	data->Url(url)->Path(path)->Priority(priority)->Dir(dir)->FileName(filename)->Task(taskTag)->Callback([=](string downpath) {
		if (callback)
			callback(downpath);
		deleteTask(taskTag);
		startTask();
	})->Callbackerror([=](string error) {
		if (callbackerror)
			callbackerror(error);
		deleteTask(taskTag);
		startTask();
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
	startTask();
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
		taskTag = StringUtils::format("%d_downloadImage_", (int)YYXLayer::getRandom()) + path;
	}
	return taskTag;
}

void YYXDownloadImages::getList()
{
	App::log("*****************************************************************************************************************");
	for (auto it : downloadList)
	{
		string name = it.first;
		string path = it.second->Path();
		App::log(name + "===>>" + path);
	}
	App::log("*****************************************************************************************************************");
}

void YYXDownloadImages::startTask()
{
	m.lock();
	if (downloadList.size() > 0)
	{
		vector<string> data;
		for (auto it : downloadList)
		{
			auto task = it.second;
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
			}
		}
		for (auto it : data)
		{
			deleteTask(it);
			App::log("deleteTask out 10s ==>>" + it);
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
		Start(true);
	}
	else
	{
		Start(false);
	}
	App::log("YYXDownloadImages::startTask() =========================>>> downloadList.size() =", downloadList.size());
	m.unlock();
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
		thread([=]() {
			App::ccsleep(data->DelayTime());
			NetIntface::DownLoadImage(data->Url(), data->Dir(), data->FileName(), "", data->Callback(), "", data->Callbackerror());
			data->StartTime(YYXLayer::getCurrentTime4Second());
		}).detach();
	}
}

void YYXDownloadImages::deleteTask(string taskTag)
{
	downloadList.erase(taskTag);
	auto data = m_Tasks[taskTag];
	if (data)
		data->release();
	m_Tasks.erase(taskTag);	
}
