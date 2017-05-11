#include "YYXDownload.h"
#include "YYXTime.h"

YYXDownload* YYXDownload::instance = nullptr;
//	string statusKey = taskTag + "+status";//下载状态 0=下载 1=准备下载   2=暂停  3=下载结束
YYXDownload::YYXDownload()
{
	App::log("YYXDownload::YYXDownload()");
	App::log("YYXDownload::YYXDownload()---END");
}

YYXDownload::~YYXDownload()
{
	App::log("~YYXDownload()");
	App::log("~YYXDownload()---END");
}

YYXDownload* YYXDownload::getInstance()
{
	if (instance == nullptr)
		instance = new YYXDownload();
	return instance;
}

void YYXDownload::pause(string tag)
{
	auto task = getTask(tag);
	if (task)
	{
		task->setStatus(_pause);
	}
	delDownloadList(tag);	
}

void YYXDownload::pauseAll()
{
	m_Mdownloadlist.lock();
	for (auto it : m_downloadList)
	{
		auto tag = it.first;
		auto data = getTask(tag);
		if (data)
		{
			data->setStatus(_pause);
		}
	}
	m_downloadList.clear();
	m_Mdownloadlist.unlock();
}

void YYXDownload::deleteAll()
{
	m_Malllist.lock();
	for (auto it : m_allTasks)
	{
		string key = it.first;
		auto data = it.second;
		if (data)
		{
			if (!data->getThreadRuning())
			{
				DownLoadFileData::del(data);
			}
			else
			{
				thread([=]() {
					string tag = data->getTag();
					data->setStatus(_pause);
					while (data->getThreadRuning())
					{
						data->setStatus(_pause);
						App::ccsleep(3000);
						if (!data->getThreadRuning())
						{
							DownLoadFileData::del(data);
							break;
						}
						App::log(tag+"  YYXDownload::deleteAll() ------------------------------------>>> runing");
					}
				}).detach();
			}
		}
	}
	m_allTasks.clear();
	m_Malllist.unlock();
}

void YYXDownload::setDownloadMaxCount(int count)
{
	downloadCount = count;
}

int YYXDownload::getDownloadMaxCount()
{
	return downloadCount;
}

void YYXDownload::startAll()
{
	int count = 0;
	int dc = getDownloadingCount();
	if (dc < downloadCount)
		count = downloadCount - dc;
	for (size_t i = 0; i < count; i++)
	{
		if (getReadyCount() > 0)
		{
			thread([]() {
				auto taskTag = YYXDownload::getInstance()->popReadyList();
				if (!taskTag.empty())
				{
					auto data = YYXDownload::getInstance()->getTask(taskTag);
					//重复任务 会出现的情况：1.有任务tag 但是任务data已经释放
					if (data)
					{
						if (data->getStatus() != _pause) {
							App::log("startAll-----------" + data->getTag());
							YYXDownload::getInstance()->addDownloadList(taskTag);
							YYXDownload::getInstance()->curlDownloadFile(data);
						}
					}
				}
			}).detach();
		}
	}
}

//zipName无后缀
bool YYXDownload::uncompress(string zipDir, string zipName, string unzipDirPath, function<void(string str)> successRun, function<void(string str)> failRun)
{
	YYXLayer::logb("YYXDownload::uncompress");
	App::ccsleep(1000);
	//打开zip文件
	string zipPath = zipDir + "/" + zipName + ".zip";
	unzFile zipfile = unzOpen(zipPath.c_str());
	if (!zipfile) {
		string errorstr = zipPath + " error : Unable to open the zip file";
		App::log(errorstr);
		App::addErrorLog(errorstr, StringUtils::format("uncompress_%d.dat",YYXTime::getInstance()->getRandomL()),2);
		//通知解压失败
		if (failRun)
			failRun(zipPath);
		return false;
	}

	//获取zip文件信息
	unz_global_info global_info;
	if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK) {
		string errorstr = zipPath + " error : Unable to get the zip file information";
		App::log(errorstr);
		App::addErrorLog(errorstr, StringUtils::format("uncompress_%d.dat", YYXTime::getInstance()->getRandomL()),2);
		//通知解压失败
		if (failRun)
			failRun(zipPath);
		unzClose(zipfile);
		return false;
	}

	//保存从文件读取的数据的缓冲区
	char readBuffer[8192];

	App::log("start uncompressing");

	//循环抽取整个文件
	uLong i;
	for (i = 0; i < global_info.number_entry; ++i) {
		//获取当前文件信息
		unz_file_info fileInfo;
		char fileName[512];
		if (unzGetCurrentFileInfo(zipfile, &fileInfo, fileName, 512, nullptr, 0, nullptr, 0) != UNZ_OK) {//读取文件信息失败
			unzClose(zipfile);
			//通知解压失败
			string errorstr = zipPath + " error : "+ string(fileName) +" =>Read the current file information failure";
			App::log(errorstr);
			App::addErrorLog(errorstr, StringUtils::format("uncompress_%d.dat", YYXTime::getInstance()->getRandomL()),2);
			if (failRun)
				failRun(zipPath);
			return false;
		}

		const size_t filenameLength = strlen(fileName);

		for (size_t i = 0; i < filenameLength; i++) {
			if (fileName[i] == '\\')
				fileName[i] = '/';
		}

		string _storagePath = unzipDirPath + "/" + zipName + "/";
		const string fullPath = _storagePath + fileName;

		if (fileName[filenameLength - 1] == '/' || fileName[filenameLength - 1] == '\\') {
			//如果是文件夹，则创建一个文件夹，如果文件夹已存在则不会再次创建
			if (!FileUtils::getInstance()->createDirectory(fullPath)) {
				unzClose(zipfile);
				//通知解压失败
				string errorstr = zipPath + " error : "+ fullPath +"=>Failed to create the folder";
				App::log(errorstr);
				App::addErrorLog(errorstr, StringUtils::format("uncompress_%d.dat", YYXTime::getInstance()->getRandomL()),2);
				if (failRun)
					failRun(zipPath);
				return false;
			}
		}
		else {
			const string fileNameStr(fileName);

			size_t startIndex = 0;

			size_t index = fileNameStr.find("/", startIndex);

			while (index != std::string::npos) {
				const string dir = _storagePath + fileNameStr.substr(0, index);
				FILE *out = fopen(FileUtils::getInstance()->getSuitableFOpen(dir).c_str(), "r");

				if (!out) {
					if (!FileUtils::getInstance()->createDirectory(dir)) {
						unzClose(zipfile);
						//通知解压失败
						string errorstr = zipPath + " error : "+ dir +"=>Failed to create the folder";
						App::log(errorstr);
						App::addErrorLog(errorstr, StringUtils::format("uncompress_%d.dat", YYXTime::getInstance()->getRandomL()),2);
						if (failRun)
							failRun(zipPath);
						return false;
					}
				}
				else
					fclose(out);

				startIndex = index + 1;
				index = fileNameStr.find("/", startIndex);
			}

			// 当前入口是一个文件, 所以抽取文件
			// 打开当前文件
			if (unzOpenCurrentFile(zipfile) != UNZ_OK) {
				unzClose(zipfile); 
				//通知解压失败
				string errorstr = zipPath + " error : " + string(fileName) + " =>unzOpenCurrentFile is Failed";
				App::log(errorstr);
				App::addErrorLog(errorstr, StringUtils::format("uncompress_%d.dat", YYXTime::getInstance()->getRandomL()),2);
				if (failRun)
					failRun(zipPath);
				return false;
			}

			FILE *out = fopen(FileUtils::getInstance()->getSuitableFOpen(fullPath).c_str(), "wb");
			if (!out) {
				unzCloseCurrentFile(zipfile);
				unzClose(zipfile);
				//通知解压失败
				string errorstr = zipPath + " error : "+ fullPath +" =>Failed to open the file";
				App::log(errorstr);
				App::addErrorLog(errorstr, StringUtils::format("uncompress_%d.dat", YYXTime::getInstance()->getRandomL()),2);
				if (failRun)
					failRun(zipPath);
				return false;
			}
			//将当前文件的内容写入文件
			int error = UNZ_OK;
			do {
				error = unzReadCurrentFile(zipfile, readBuffer, BUFFER_SIZE);
				if (error < 0) {
					App::log("can not read zip file " + string(fileName) + ", error code is ", error);
					unzCloseCurrentFile(zipfile);
					unzClose(zipfile);
					//通知解压失败
					string errorstr = zipPath + " error : " + string(fileName) + " =>unzReadCurrentFile is Failed";
					App::log(errorstr);
					App::addErrorLog(errorstr, StringUtils::format("uncompress_%d.dat", YYXTime::getInstance()->getRandomL()),2);
					if (failRun)
						failRun(zipPath);
					return false;
				}
				if (error > 0)
					fwrite(readBuffer, error, 1, out);
			} while (error > 0);

			fclose(out);
		}

		unzCloseCurrentFile(zipfile);

		//前往下一个入口
		if ((i + 1) < global_info.number_entry) {
			if (unzGoToNextFile(zipfile) != UNZ_OK) {
				unzClose(zipfile);
				//通知解压失败
				string errorstr = zipPath + " error : " + string(fileName) + " =>unzGoToNextFile is Failed";
				App::log(errorstr);
				App::addErrorLog(errorstr, StringUtils::format("uncompress_%d.dat", YYXTime::getInstance()->getRandomL()),2);
				if (failRun)
					failRun(zipPath);
				return false;
			}
		}
	}

	App::log("end uncompressing");
	unzClose(zipfile);
	//解压成功通知
	if(successRun)
		successRun(zipPath);
	//解压完成后删除zip文件
	remove(zipPath.c_str());
	YYXLayer::loge("YYXDownload::uncompress");
	return true;
}

size_t YYXDownload::pWriteCallback(void *pData, size_t n, size_t nDataSize, FILE *stream)
{
	//YYXLayer::logb("YYXDownload::pWriteCallback");
	size_t nWritten = fwrite(pData, n, nDataSize, (FILE *)stream);
	//YYXLayer::loge("YYXDownload::pWriteCallback");
	return nWritten;
}

long YYXDownload::getLocalFileSize(const char *filePath)
{
	YYXLayer::logb("YYXDownload::getLocalFileSize");
	if (!FileUtils::getInstance()->isFileExist(filePath))
		return 0;
	FILE * pFile = fopen(filePath, "rb");
	fseek(pFile, 0, SEEK_END);
	long size = ftell(pFile);
	fclose(pFile);
	YYXLayer::loge("YYXDownload::getLocalFileSize");
	return size;
}

void YYXDownload::addFunc(int key, const function<void(DownLoadFileData*)>& func)
{
	m_Mcallback.lock();
	if (key > 0 && func)
	{
		m_callbackFuncs[key] = func;
	}
	m_Mcallback.unlock();
}

function<void(DownLoadFileData*)> YYXDownload::getFunc(int key)
{
	m_Mcallback.lock();
	function<void(DownLoadFileData*)> call = nullptr;
	auto it = m_callbackFuncs.find(key);
	if (it != m_callbackFuncs.end())
	{
		call = it->second;
	}
	m_Mcallback.unlock();
	return call;
}

void YYXDownload::delFunc(int key)
{
	m_Mcallback.lock();
	if (key > 0)
	{
		auto it = m_callbackFuncs.find(key);
		if (it != m_callbackFuncs.end())
		{
			m_callbackFuncs.erase(it);
		}
	}
	m_Mcallback.unlock();
}

void YYXDownload::addDownloadList(string tag)
{
	m_Mdownloadlist.lock();
	if (!tag.empty())
		m_downloadList[tag] = 0;
	m_Mdownloadlist.unlock();
}

void YYXDownload::delDownloadList(string tag)
{
	m_Mdownloadlist.lock();
	m_downloadList.erase(tag);
	m_Mdownloadlist.unlock();
}

int YYXDownload::getDownloadingCount()
{
	m_Mdownloadlist.lock();
	int cou = m_downloadList.size();
	m_Mdownloadlist.unlock();
	return cou;
}

void YYXDownload::addReadyList(string tag)
{
	m_Mreadylist.lock();
	m_readyQueue.push(tag);
	m_Mreadylist.unlock();
}

string  YYXDownload::popReadyList()
{
	m_Mreadylist.lock();
	string tag = "";
	if (!m_readyQueue.empty())
	{
		tag = m_readyQueue.front();
		m_readyQueue.pop();
	}
	m_Mreadylist.unlock();
	return tag;
}

int YYXDownload::getReadyCount()
{
	m_Mreadylist.lock();
	int cou = m_readyQueue.size();
	m_Mreadylist.unlock();
	return cou;
}

void YYXDownload::addTask(DownLoadFileData * data)
{
	//去任务列表中的重复
	m_Malllist.lock();
	string tag = data->getTag();
	auto it = m_allTasks.find(tag);
	if (it != m_allTasks.end())
	{
		if(it->second != data)
			DownLoadFileData::del(data);
	}
	else
	{
		m_allTasks[tag] = data;
	}
	m_allTasks[tag]->setStatus(_ready);
	m_Malllist.unlock();
}

void YYXDownload::delTask(string tag)
{
	m_Malllist.lock();
	auto it = m_allTasks.find(tag);
	if (it != m_allTasks.end())
	{
		auto data = it->second;
		DownLoadFileData::del(data);
		m_allTasks.erase(tag);
	}
	m_Malllist.unlock();
}

DownLoadFileData * YYXDownload::getTask(string tag)
{
	m_Malllist.lock();
	DownLoadFileData* data = nullptr;
	auto it = m_allTasks.find(tag);
	if (it != m_allTasks.end())
	{
		data = it->second;
	}
	m_Malllist.unlock();
	return data;
}

void YYXDownload::curlDownloadFile(DownLoadFileData* data)
{
	while (data->getThreadRuning())
	{
		data->setStatus(_pause);
		App::ccsleep(5000);
	}
	string url = data->getUrl();
	string path = data->getPath();
	auto beginFun = data->getBeginFunc();
	function<void(DownLoadFileData*)> EndFunc = [](DownLoadFileData* dat) {
		auto endFun = dat->getEndFunc();
		if (endFun)
			endFun(dat);
		dat->setThreadRuning(false);
	};
	if (beginFun)
		beginFun(data);
	data->setThreadRuning(true);

	CURL *pCurl = nullptr;
	pCurl = curl_easy_init();//初始化CURL取得初始化成功后的CURL指针
	FILE *pFile = nullptr;
	pFile = fopen(path.c_str(), "ab+");
	if (pFile == nullptr)
	{
		if (EndFunc)
			EndFunc(data);
		return;
	}
	data->setStatus(_download);
	curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(pCurl, CURLOPT_FILE, pFile);                  //指定写入的文件指针
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, pWriteCallback);//设置写数据的回调函数
	curl_easy_setopt(pCurl, CURLOPT_VERBOSE, true);                //让CURL报告每一件意外的事情
	curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10000);                 //设置超时时间
	curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, false);
	curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION, DownProgress);//指定显示进度的回调函数
	curl_easy_setopt(pCurl, CURLOPT_RESUME_FROM, getLocalFileSize(path.c_str()));// 从本地大小位置进行请求数据
	curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA, data);//DownProgresss函数的第一个参数	
	CURLcode nResCode = curl_easy_perform(pCurl);//执行上面设定的动作并返回状态码
	curl_easy_cleanup(pCurl);//释放相关资源
	fclose(pFile);
	if (data->getStatus() != _pause)
	{
		if (nResCode == CURLE_OK)
			data->setStatus(_over);
		else
			data->setStatus(_null);
	}
	auto endFun = data->getEndFunc();
	if (endFun)
		endFun(data);
	data->setThreadRuning(false);
	if (data->getStatus() != _pause)
	{
		data->setStatus(_over);
		delDownloadList(data->getTag());
		delTask(data->getTag());
	}	
	startAll();
}

int YYXDownload::DownProgress(void* clientp, double fDownLoadTotal, double fDownLoaded, double fUpTotal, double fUpLoaded)
{
	auto date = (DownLoadFileData*)clientp;
	string taskTag = date->getTag();
	YYXLayer::controlTouchTime(5, taskTag + "Time", [=]() {
		YYXLayer::setFileValue(taskTag, Value(fDownLoadTotal).asString(), FileUtils::getInstance()->getWritablePath() + "temp");
	});
	int pragress = fDownLoaded / fDownLoadTotal *100.0;
	auto pausePint = date->getPausePint();
	if (pragress < pausePint)
	{
		pragress = pausePint;
	}
	int code = 0;//1=下载中断 0=下载流畅
	auto status = date->getStatus();
	switch (status)
	{
	case _pause:
		code = 1;
		break;
	case _over:
		code = 1;
		break;
	}
	date->setPausePint(pragress);
	auto fun = date->getDownloadingFunc();
	if (fun)
	{
		fun(date);
	}
	return code;
}

void YYXDownload::newDownloadFile(DownLoadFileData * data)
{
	string dir = data->getDir();
	string fileName = data->getFileName();
	string taskTag = data->getTag();

	if (!FileUtils::getInstance()->isDirectoryExist(dir))
		FileUtils::getInstance()->createDirectory(dir);
	std::thread mythread([=]() {
		auto zhuangtai = data->getStatus();
		switch (zhuangtai)
		{
		case _null:
			data->setStatus(_ready);
			break;
		case _ready:
			return;
			break;
		case _download:
			return;
			break;
		case _pause:
			data->setStatus(_ready);
			break;
		case _over:
			data->setStatus(_ready);
			break;
		}
		if (data)
			addTask(data);
		addReadyList(taskTag);
		startAll();
	});
	mythread.detach();
	App::log("YYXDownload::download--END" + taskTag);
}

DownLoadFileData::DownLoadFileData()
{
	m_threadRuning = false;
}

DownLoadFileData::~DownLoadFileData()
{
	YYXDownload::getInstance()->delFunc(m_beginKey);
	YYXDownload::getInstance()->delFunc(m_downloadingKey);
	YYXDownload::getInstance()->delFunc(m_endKey);
}

DownLoadFileData * DownLoadFileData::create()
{
	return new DownLoadFileData();
}

void DownLoadFileData::del(DownLoadFileData * data)
{
	if (data)
		delete data;
	data = nullptr;
}

function<void(DownLoadFileData*)> DownLoadFileData::getBeginFunc()
{
	if (m_beginKey > 0)
		return YYXDownload::getInstance()->getFunc(m_beginKey);
	else
		return nullptr;
}

void DownLoadFileData::setBeginFunc(const function<void(DownLoadFileData*)>& val)
{
	m_beginKey = YYXTime::getInstance()->getRandomL();
	YYXDownload::getInstance()->addFunc(m_beginKey, val);
}

function<void(DownLoadFileData*)> DownLoadFileData::getDownloadingFunc()
{
	if (m_downloadingKey > 0)
		return YYXDownload::getInstance()->getFunc(m_downloadingKey);
	else
		return nullptr;
}

void DownLoadFileData::setDownloadingFunc(const function<void(DownLoadFileData*)>& val)
{
	m_downloadingKey = YYXTime::getInstance()->getRandomL();
	YYXDownload::getInstance()->addFunc(m_downloadingKey, val);
}


function<void(DownLoadFileData*)> DownLoadFileData::getEndFunc()
{
	if (m_endKey > 0)
		return YYXDownload::getInstance()->getFunc(m_endKey);
	else
		return nullptr;
}

void DownLoadFileData::setEndFunc(const function<void(DownLoadFileData*)>& val)
{
	m_endKey = YYXTime::getInstance()->getRandomL();
	YYXDownload::getInstance()->addFunc(m_endKey, val);
}
