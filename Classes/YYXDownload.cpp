#include "YYXDownload.h"

YYXDownload* YYXDownload::instance = nullptr;
//	string statusKey = taskTag + "+status";//����״̬ 0=���� 1=׼������   2=��ͣ  3=���ؽ���
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

YYXDownload* YYXDownload::GetInstance()
{
	if (instance == nullptr)
		instance = new YYXDownload();
	return instance;
}

std::string YYXDownload::download(string url, string dir, string fileName, string beginstr, function<void(YYXStruct)> beginFunction,
	string downloadingstr, function<void(YYXStruct)> downloadingFunction, string endstr, function<void(YYXStruct)> endFunction)
{
	App::log("YYXDownload::download");
	if (!FileUtils::getInstance()->isDirectoryExist(dir))
	{
		App::log("YYXDownload::download / " + dir);
		FileUtils::getInstance()->createDirectory(dir);
	}
	//�ж��Ƿ��ظ�����
	auto urltag = YYXStruct::getMapString(data, url, "");
	auto pathtag = YYXStruct::getMapString(data, dir + "/" + fileName, "");
	string taskTag = "";
	if (urltag == "" && pathtag == "")
	{
		//�������������Ψһ��ʶ��
		taskTag = StringUtils::format("%s_downloadTag_%d", fileName.c_str(),(int)YYXLayer::getRandom());
		YYXStruct::initMapYYXStruct(data, url, -999, taskTag);
		YYXStruct::initMapYYXStruct(data, dir + "/" + fileName, -999, taskTag);
	}
	else
	{
		if (urltag == "")
			taskTag = pathtag;
		else
			taskTag = urltag;
	}	
	std::thread([=]() {
		setMapFunction(beginstr, beginFunction);
		setMapFunction(downloadingstr, downloadingFunction);
		setMapFunction(endstr, endFunction);
		string beginKey = taskTag + "+begin";
		YYXStruct::initMapYYXStruct(data, beginKey, -999, beginstr);
		string downloadingKey = taskTag + "+downloading";
		YYXStruct::initMapYYXStruct(data, downloadingKey, -999, downloadingstr);
		string endKey = taskTag + "+end";
		YYXStruct::initMapYYXStruct(data, endKey, -999, endstr);
		string urlKey = taskTag + "+url";
		YYXStruct::initMapYYXStruct(data, urlKey, -999, url);
		string pathKey = taskTag + "+path";
		YYXStruct::initMapYYXStruct(data, pathKey, -999, dir + "/" + fileName);
		if (!isExitDownloadQueue(taskTag))
		{
			deleteReadyQueue(taskTag);
			addReadyQueue(taskTag);
		}
		//autoFullDownload();
	}).detach();
	App::log("YYXDownload::download--END" + taskTag);
	return taskTag;
}

void YYXDownload::pause(string tag)
{
	App::log("YYXDownload::stop");
	//�����ض����Ƴ�
	//�ж��Ƿ������ض�����
	string statusKey = tag + "+status";
	YYXStruct::initMapYYXStruct(data, statusKey, 2);
	if (downloadQueue.find(tag) != downloadQueue.end())
	{
		deleteDownloadQueue(tag);
	}
	else
	{
		deleteReadyQueue(tag);
	}
	//autoFullDownload();
	App::log("YYXDownload::stop-END");
}

void YYXDownload::pauseAll()
{
	//ȫ�����
	while (!readyQueue.empty())
	{
		auto tag = readyQueue.front();
		string statusKey = tag + "+status";
		YYXStruct::initMapYYXStruct(data, statusKey, 2);
		readyQueue.pop();
	}
	readyQueueMap.clear();
	for (auto it = downloadQueue.begin(); it != downloadQueue.end(); it++)
	{
		App::log(it->first);
		string statusKey = it->first + "+status";
		YYXStruct::initMapYYXStruct(data, statusKey, 2);
	}
	downloadQueue.clear();
}

//void YYXDownload::autoFullDownload()
//{//�ж����ض����Ƿ�������
//	App::log("YYXDownload::autoFullDownload", downloadCount);
//	App::log("downqueue.size = ", downloadQueue.size());
//	App::log("readyqueue.size = ", readyQueue.size());
//	while (downloadQueue.size() < downloadCount)
//	{
//		getReadyQueueFrontPushDownloadQueue();
//		if (readyQueue.empty())
//			break;
//	}
//	startAll();
//	App::log("YYXDownload::autoFullDownload---END");
//	App::log("downqueue.size2 = ", downloadQueue.size());
//	App::log("readyqueue.size2 = ", readyQueue.size());
//}

//void YYXDownload::pauseAll()
//{
//	App::log("YYXDownload::pauseAll");
//	for (int i = 0; i < readyQueue.size();i++)
//	{
//		down_mtx.lock();
//		auto it = readyQueue.front();
//		string statusKey = it + "+status";
//		YYXStruct::initMapYYXStruct(data, statusKey, 2);
//		readyQueue.pop();
//		down_mtx.unlock();
//	}
//	for (auto it : downloadQueue)
//	{
//		auto key = it.first;
//		auto value = it.second;		
//		string statusKey = key + "+status";
//		YYXStruct::initMapYYXStruct(data, statusKey, 2);
//		if (downloadQueue.find(key) != downloadQueue.end())
//		{
//			deleteDownloadQueue(key);
//		}
//	}
//	App::log("YYXDownload::pauseAll---END");
//}

void YYXDownload::setDownloadMaxCount(int count)
{
	YYXLayer::logb("YYXDownload::setDownloadMaxCount");
	downloadCount = count;
	YYXLayer::loge("YYXDownload::setDownloadMaxCount");
}

int YYXDownload::getDownloadMaxCount()
{
	YYXLayer::logb("YYXDownload::getDownloadMaxCount");
	return downloadCount;
	YYXLayer::loge("YYXDownload::getDownloadMaxCount");
}

std::string YYXDownload::getTag(string path)
{
	auto pathtag = YYXStruct::getMapString(YYXDownload::GetInstance()->data, path, "");
	return pathtag;
}

void YYXDownload::setMapFunction(string key, function<void(YYXStruct)> runFunction)
{
	YYXLayer::logb("YYXDownload::setMapFunction");
	if (&key && runFunction && key != "")
		functionMap[key] = runFunction;
	YYXLayer::loge("YYXDownload::setMapFunction");
}

function<void(YYXStruct)> YYXDownload::getMapFunction(string key)
{
	//YYXLayer::logb("YYXDownload::getMapFunction / " + key);
	if (&key && key != "")
	{
		if (functionMap.find(key) != functionMap.end())
		{
			auto result = functionMap[key];
			//YYXLayer::loge("YYXDownload::getMapFunction" + key);
			return result;
		}
	}
	YYXLayer::loge("YYXDownload::getMapFunction  is nullptr "); 
	return nullptr;
}

int YYXDownload::addDownloadQueue(string task)
{
	YYXLayer::logb("YYXDownload::addDownloadQueue");
	down_mtx.lock();
	if (&task && task != "")
	{
		string pathKey = task + "+path";
		string path = YYXStruct::getMapString(data, pathKey, "");
		downloadQueue[task] = YYXStruct(-999, path, nullptr);
	}
	down_mtx.unlock();
	YYXLayer::loge("YYXDownload::addDownloadQueue");
	return downloadQueue.size();
}

int YYXDownload::deleteDownloadQueue(string task)
{
	down_mtx.lock();
	YYXLayer::logb("YYXDownload::deleteDownloadQueue");
	if (downloadQueue.find(task) != downloadQueue.end())
		downloadQueue.erase(task);
	YYXLayer::loge("YYXDownload::deleteDownloadQueue");
	down_mtx.unlock();
	return downloadQueue.size();
}

int YYXDownload::addReadyQueue(string task)
{
	down_mtx.lock();
	YYXLayer::logb("YYXDownload::addReadyQueue");
	string statusKey = task + "+status";
	YYXStruct::initMapYYXStruct(data, statusKey, 1);
	if (&task && task != "")
	{
		readyQueue.push(task);
		readyQueueMap[task] = "";
	}
	YYXLayer::loge("YYXDownload::addReadyQueue");
	down_mtx.unlock();
	App::log("addReadyQueue ---- readyQueue.size()", readyQueue.size());
	return readyQueue.size();
}

int YYXDownload::deleteReadyQueue(string task)
{
	down_mtx.lock();
	YYXLayer::logb("YYXDownload::deleteReadyQueue");
	string statusKey = task + "+status";
	YYXStruct::initMapYYXStruct(data, statusKey, 2);
	vector<string> data;
	while (!readyQueue.empty())
	{
		auto it = readyQueue.front();
		if (it != task)
			data.push_back(it);
		readyQueue.pop();
	}
	for (auto it : data)
	{
		readyQueue.push(it);
	}
	readyQueueMap.erase(task);
	YYXLayer::loge("YYXDownload::deleteReadyQueue");
	down_mtx.unlock();
	App::log("readyQueue.size() ", readyQueue.size());
	return readyQueue.size();
}

//void YYXDownload::startAll()
//{
//	YYXLayer::logb("YYXDownload::startAll");
//	for (auto it : downloadQueue)
//	{
//		string str = "";
//		str = it.first;
//		startTask(str);
//	}
//	YYXLayer::loge("YYXDownload::startAll");
//}

void YYXDownload::startAll()
{
	YYXLayer::logb("YYXDownload::startAll");
	for (int i = 0; i < getDownloadMaxCount();i++)
	{
		thread(&YYXDownload::downloadThreadRuning, this).detach();
	}
	YYXLayer::loge("YYXDownload::startAll");
}

bool YYXDownload::isExitDownloadQueue(string taskTag)
{
	if (downloadQueue.find(taskTag) == downloadQueue.end())
		return false;
	else
		return true;
}

//int YYXDownload::startTask(string taskTag)
//{
//	YYXLayer::logb("YYXDownload::startTask"+ taskTag );
//	string urlKey = taskTag + "+url";
//	string url = YYXStruct::getMapString(data, urlKey, "");
//	string pathKey = taskTag + "+path";
//	string path = YYXStruct::getMapString(data, pathKey, "");
//	string statusKey = taskTag + "+status";
//	int status = YYXStruct::getMapInt64(data, statusKey, 1);
//	if (url != "" && path != "") 
//	{//����״̬ 0=���� 1=׼������   2=��ͣ  3=���ؽ���
//		if (status == 1)
//		{
//			thread(&YYXDownload::downLoadFile, this, url, path, taskTag).detach();
//			//downLoadFile(url, path, taskTag);
//		}
//		else if (status == 2 || status == 3)
//		{
//			deleteDownloadQueue(taskTag);
//		}
//	}
//	YYXLayer::loge("******YYXDownload::startTask / path=   " + path);
//	return downloadQueue.size();
//}

void YYXDownload::taskBegin(string taskTag)
{
	YYXLayer::logb("YYXDownload::taskBegin");
	string beginKey = taskTag + "+begin";
	string beginstr = YYXStruct::getMapString(data, beginKey, "");
	auto run = getMapFunction(beginstr);
	if (run)
	{
		run(YYXStruct(-999, taskTag, nullptr));
	}
	addDownloadQueue(taskTag);
	YYXLayer::loge("YYXDownload::taskBegin");
}

void YYXDownload::taskDownloading(string taskTag, int progress)
{
	//YYXLayer::logb("YYXDownload::taskDownloading");
	string downloadingKey = taskTag + "+downloading";
	string downloadingstr = YYXStruct::getMapString(YYXDownload::GetInstance()->data, downloadingKey, "");
	auto run = YYXDownload::GetInstance()->getMapFunction(downloadingstr);
	if (run)
	{
		run(YYXStruct(progress, taskTag, nullptr));
	}
	//YYXLayer::loge("YYXDownload::taskDownloading");
}

void YYXDownload::taskEnd(string taskTag, int resultCode)
{
	YYXLayer::logb("YYXDownload::taskEnd");
	string endKey = taskTag + "+end";
	string endstr = YYXStruct::getMapString(data, endKey, "");
	auto run = getMapFunction(endstr);
	if (resultCode == -1)
		App::log("CURL init error");
	deleteDownloadQueue(taskTag);
	//autoFullDownload();
	if (run)
	{		
		run(YYXStruct(resultCode, taskTag, nullptr));
	}
	YYXLayer::loge("YYXDownload::taskEnd");
}

void YYXDownload::downLoadFile(string url, string path,  const string& taskTag)
{//����״̬ 0=���� 1=׼������   2=��ͣ  3=���ؽ���
	YYXLayer::logb("YYXDownload::downLoadFile");
	taskBegin(taskTag);
	CURL *pCurl;
	pCurl = curl_easy_init();//��ʼ��CURLȡ�ó�ʼ���ɹ����CURLָ��
	FILE *pFile = nullptr;
	App::log("YYXDownload::downLoadFile"+path);
	pFile = fopen(path.c_str(), "ab+");
	if (pFile == nullptr)
	{
		taskEnd(taskTag, -1);//��ʼ��ʧ��
		YYXLayer::loge("YYXDownload::downLoadFile");
		return;
	}
	string statusKey = string(taskTag).append("+status");	//��ǿ�ʼ���ص�״̬
	YYXStruct::initMapYYXStruct(data, statusKey, 0);
	curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(pCurl, CURLOPT_FILE, pFile);                  //ָ��д����ļ�ָ��
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, pWriteCallback);//����д���ݵĻص�����
	curl_easy_setopt(pCurl, CURLOPT_VERBOSE, true);                //��CURL����ÿһ�����������
	curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10000);                 //���ó�ʱʱ��
	curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, false);
	curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION, DownProgresss);//ָ����ʾ���ȵĻص�����
	curl_easy_setopt(pCurl, CURLOPT_RESUME_FROM, getLocalFileSize(path.c_str()));// �ӱ��ش�Сλ�ý�����������
	curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA, &taskTag);//DownProgresss�����ĵ�һ������	
	CURLcode nResCode = curl_easy_perform(pCurl);//ִ�������趨�Ķ���������״̬��
	curl_easy_cleanup(pCurl);//�ͷ������Դ
	fclose(pFile);
	if(!isPause(taskTag))
		YYXStruct::initMapYYXStruct(data, statusKey, 3);//��ǽ������ص�״̬	
	taskEnd(taskTag, nResCode);
	YYXLayer::loge("*****************YYXDownload::downLoadFile");
}
//zipName�޺�׺
bool YYXDownload::uncompress(string zipDir, string zipName, string unzipDirPath, function<void(string str)> successRun, function<void(string str)> failRun)
{
	YYXLayer::logb("YYXDownload::uncompress");
	App::ccsleep(1000);
	//��zip�ļ�
	string zipPath = zipDir + "/" + zipName + ".zip";
	unzFile zipfile = unzOpen(zipPath.c_str());
	if (!zipfile) {
		App::log("can not open downloaded zip file " + zipPath);
		//֪ͨ��ѹʧ��
		if (failRun)
			failRun(zipPath);
		return false;
	}

	//��ȡzip�ļ���Ϣ
	unz_global_info global_info;
	if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK) {
		App::log("can not read file global info of " + zipPath);
		//֪ͨ��ѹʧ��
		if (failRun)
			failRun(zipPath);
		unzClose(zipfile);
		return false;
	}

	//������ļ���ȡ�����ݵĻ�����
	char readBuffer[8192];

	App::log("start uncompressing");

	//ѭ����ȡ�����ļ�
	uLong i;
	for (i = 0; i < global_info.number_entry; ++i) {
		//��ȡ��ǰ�ļ���Ϣ
		unz_file_info fileInfo;
		char fileName[512];
		if (unzGetCurrentFileInfo(zipfile, &fileInfo, fileName, 512, nullptr, 0, nullptr, 0) != UNZ_OK) {//��ȡ�ļ���Ϣʧ��
			unzClose(zipfile);
			//֪ͨ��ѹʧ��
			App::log("ѭ����ȡ�����ļ�notifyUpressFailed " + zipPath);
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
			//������ļ��У��򴴽�һ���ļ��У�����ļ����Ѵ����򲻻��ٴδ���
			if (!FileUtils::getInstance()->createDirectory(fullPath)) {
				unzClose(zipfile);
				//֪ͨ��ѹʧ��
				App::log("������ļ��У��򴴽�һ���ļ��У�����ļ����Ѵ����򲻻��ٴδ���notifyUpressFailed " + zipName);
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
						//֪ͨ��ѹʧ��
						App::log("1111111111notifyUpressFailed " + zipName);
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

			// ��ǰ�����һ���ļ�, ���Գ�ȡ�ļ�
			// �򿪵�ǰ�ļ�
			if (unzOpenCurrentFile(zipfile) != UNZ_OK) {
				unzClose(zipfile);
				//֪ͨ��ѹʧ��
				App::log("222222222 notifyUpressFailed" + zipName);
				if (failRun)
					failRun(zipPath);
				return false;
			}

			FILE *out = fopen(FileUtils::getInstance()->getSuitableFOpen(fullPath).c_str(), "wb");
			if (!out) {
				unzCloseCurrentFile(zipfile);
				unzClose(zipfile);
				//֪ͨ��ѹʧ��
				App::log("333333333333 notifyUpressFailed" + zipName);
				if (failRun)
					failRun(zipPath);
				return false;
			}
			//����ǰ�ļ�������д���ļ�
			int error = UNZ_OK;
			do {
				error = unzReadCurrentFile(zipfile, readBuffer, BUFFER_SIZE);
				if (error < 0) {
					App::log("can not read zip file " + string(fileName) + ", error code is ", error);
					unzCloseCurrentFile(zipfile);
					unzClose(zipfile);
					//֪ͨ��ѹʧ��
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

		//ǰ����һ�����
		if ((i + 1) < global_info.number_entry) {
			if (unzGoToNextFile(zipfile) != UNZ_OK) {
				unzClose(zipfile);
				//֪ͨ��ѹʧ��
				App::log("444444444444 notifyUpressFailed" + zipName);
				if (failRun)
					failRun(zipPath);
				return false;
			}
		}
	}

	App::log("end uncompressing");
	unzClose(zipfile);
	//��ѹ�ɹ�֪ͨ
	if(successRun)
		successRun(zipPath);
	//��ѹ��ɺ�ɾ��zip�ļ�
	remove(zipPath.c_str());
	YYXLayer::loge("YYXDownload::uncompress");
	return true;
}

bool YYXDownload::isExitReadyQueue(string taskTag)
{
	//down_mtx.lock();
	//bool result = false;
	//vector<string> data;
	//for (int i = 0; i < readyQueue.size(); i++)
	//{
	//	auto it = readyQueue.front();
	//	data.push_back(it);
	//	readyQueue.pop();
	//	if (it == taskTag)
	//		result= true;
	//	else
	//		result= false;
	//}
	//for (auto it : data)
	//{
	//	readyQueue.push(it);
	//}
	//down_mtx.unlock();
	if (readyQueueMap.find(taskTag) != readyQueueMap.end())
	{
		return true;
	}
	else
	{
		return false;
	}
	//return result;
}

bool YYXDownload::isPause(string taskTag)
{
	string statusKey = taskTag + "+status";
	//int status = YYXStruct::getMapInt64(data, statusKey, 1);//����״̬ 0=���� 1=׼������   2=��ͣ  3=���ؽ���
	int status = YYXStruct::getMapInt64(YYXDownload::GetInstance()->data, statusKey, 0);
	if (status == 2)
	{		
		return true;
	}
	else
	{
		return false;
	}
}
//
//void YYXDownload::getReadyQueueFrontPushDownloadQueue()
//{
//	YYXLayer::logb("YYXDownload::getReadyQueueFrontPushDownloadQueue");
//	if (downloadQueue.size() < downloadCount)
//	{
//		if (!readyQueue.empty())
//		{
//			down_mtx.lock();
//			App::log("readyQueue = ", readyQueue.size());
//			auto it = readyQueue.front();
//			readyQueue.pop();
//			down_mtx.unlock();
//			if (it != "")
//			{
//				string statusKey = it + "+status";
//				int status = YYXStruct::getMapInt64(data, statusKey, 1);//����״̬ 0=���� 1=׼������   2=��ͣ  3=���ؽ���
//				if (status == 1)
//					addDownloadQueue(it);
//			}
//		}
//	}
//	YYXLayer::loge("YYXDownload::getReadyQueueFrontPushDownloadQueue");
//}

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

int YYXDownload::DownProgresss(void* clientp, double fDownLoadTotal, double fDownLoaded, double fUpTotal, double fUpLoaded)
{
	//YYXLayer::logb("YYXDownload::DownProgresss");
	auto tag = (string*)clientp;
	int pragress = fDownLoaded / fDownLoadTotal *100.0;
	int pausePint = YYXStruct::getMapInt64(YYXDownload::GetInstance()->data, *tag + "+pause", 0);
	if (pragress < pausePint)
	{
		pragress = pausePint;
	}	
	taskDownloading(*tag, pragress);
	string statusKey = *tag + "+status";
	int status = YYXStruct::getMapInt64(YYXDownload::GetInstance()->data, statusKey , 0);//����״̬ 0=���� 1=׼������   2=��ͣ  3=���ؽ���
	if (status == 2 )
	{
		int pausePint = YYXStruct::getMapInt64(YYXDownload::GetInstance()->data, *tag + "+pause", 0);
		if (pragress > pausePint)
			YYXStruct::initMapYYXStruct(YYXDownload::GetInstance()->data, *tag + "+pause", pragress);
		return 1;
	}
	else if (status == 3)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

//1.�������֮��ɾ��taskTag
//2.��ʼ�Զ�ȫ��������, ��׼�����е������ȡ����, �����ض�������������
//3.��ͣһ������ ֻ��Ҫstring statusKey = tag + "+status";//����״̬ 1=׼������  0=����
//YYXStruct::initMapYYXStruct(data, statusKey, 1);


/*
1.�����̳߳ط�ʽ����
2.������������߳�����
3.ÿ���̵߳ȴ�����
*/
void YYXDownload::downloadThreadRuning()
{
	while (true)
	{
		//App::log("              void YYXDownload::downloadThreadRuning()");
		App::ccsleep(5000);
		//1.�ж��Ƿ���������Ҫ����
		string taskTag = "";
		down_mtx.lock();
		if (!readyQueue.empty())
		{
			App::log("cong readyQueue qu chu yi ge ren wu              readyQueue.size() =", readyQueue.size());
			taskTag = readyQueue.front();
			readyQueue.pop();
			readyQueueMap.erase(taskTag);
		}
		down_mtx.unlock();
		if (taskTag != "")
		{
			string urlKey = taskTag + "+url";
			string url = YYXStruct::getMapString(data, urlKey, "");
			string pathKey = taskTag + "+path";
			string path = YYXStruct::getMapString(data, pathKey, "");
			string statusKey = taskTag + "+status";
			int status = YYXStruct::getMapInt64(data, statusKey, 1);//����״̬ 0=���� 1=׼������   2=��ͣ  3=���ؽ���
			if (status == 1)
			{
				//thread(&YYXDownload::downLoadFile, this, url, path, taskTag).detach();				
				downLoadFile(url, path, taskTag);
			}
		}
	}
}