#include "CrossPlatform.h"
#include "YYXLayer.h"
 #if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <windows.h>
#include <io.h>
#include <direct.h>
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif
#include <stdio.h>
#include "YYXTime.h"
#include "cocos2d.h"
#include "WeixinPay.h"
#include "HttpWaiting.h"

USING_NS_CC;
using namespace std;

CrossPlatform* CrossPlatform::m_pInstance = nullptr;
map<string, function<void(string)>> CrossPlatform::m_functionMap;

CrossPlatform * CrossPlatform::getInstance()
{
	if (m_pInstance == nullptr)
		m_pInstance = new CrossPlatform();
	return m_pInstance;
}

CallBack_String CrossPlatform::getFunc(int key)
{
	m_MallFunc.lock();
	CallBack_String func = nullptr;
	auto it = m_allFunc.find(key);
	if (it != m_allFunc.end())
	{
		func = it->second;
	}
	m_MallFunc.unlock();
	return func;
}

void CrossPlatform::addFunc(int key, const CallBack_String & val)
{
	m_MallFunc.lock();
	m_allFunc[key] = val;
	m_MallFunc.unlock();
}

void CrossPlatform::delFunc(int key)
{
	m_MallFunc.lock();
	auto it = m_allFunc.find(key);
	if (it != m_allFunc.end())
	{
		m_allFunc.erase(it);
	}
	m_MallFunc.unlock();
}

CrossPlatformData* CrossPlatform::getTask(int key)
{
	m_MTask.lock();
	CrossPlatformData* data = nullptr;
	auto it = m_allTask.find(key);
	if (it != m_allTask.end())
	{
		data = it->second;
	}
	m_MTask.unlock();
	return data;
}

void CrossPlatform::addTask(CrossPlatformData* data)
{
	m_MTask.lock();
	if (data)
	{
		int key = data->getTaskName();
		auto it = m_allTask.find(key);
		if (it != m_allTask.end())
		{
			CrossPlatformData::del(data);
		}
		else
			m_allTask[key] = data;
	}
	m_MTask.unlock();
}

void CrossPlatform::delTask(int key)
{
	m_MTask.lock();
	auto it = m_allTask.find(key);
	if (it != m_allTask.end())
	{
		auto data = it->second;
		CrossPlatformData::del(data);
		m_allTask.erase(key);
	}
	m_MTask.unlock();
}

function<void(string)> CrossPlatform::getMapFunction(string key)
{
	function<void(string)> result = nullptr;
	if (&key && !key.empty())
	{
		auto it = CrossPlatform::m_functionMap.find(key);
		if ( it != CrossPlatform::m_functionMap.end())
		{
			result =it->second;
		}
	}
	//App::log("NetIntface::getMapFunction find not" + key);
	return result;
}

void CrossPlatform::setMapFunction(string key, function<void(string)> runFunction)
{
	if (&key && runFunction && !key.empty())
	{
		//App::log("set" + key);
		CrossPlatform::m_functionMap[key] = runFunction;
	}
}

void CrossPlatform::deleteMapFunction(string key)
{
	if (&key && !key.empty())
	{
		auto value = m_functionMap.find(key);
		if (value != m_functionMap.end())
		{
			m_functionMap.erase(value);
			App::log("==============NetIntface::deleteMapFunction(key);" + key);
		}
	}
}

bool CrossPlatform::IsNetConnect(bool hint)
{
	bool result = true;
	setMapFunction("showNetConnectError", [](string stt) {
		Toast::create(App::getString("WEIJIANCEDAOWANGLUO"), false);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//App::log("WIN32--This function(NetIntface::IsNetConnect()) is not implemented");
	result = true;
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	result = CocosAndroidJni::IsNetConnect(hint, "showNetConnectError");
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	//App::log("IOS--This function(NetIntface::IsNetConnect()) is not implemented");
	result = true;
#endif
	if (!result)
		App::log("NetIntface::IsNetConnect = false");
	return result;
}

long long CrossPlatform::getMillsTime()
{
	long long result=0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	result = timeGetTime();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	result = CocosAndroidJni::getMillsTime();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	result = 0;
#endif
	return result;
}

//网络请求上传文件接口
void CrossPlatform::httpUpFile(string url, string jsonparater, string filepath, function<void(string)> runFunction, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect())
		return;
	auto runKey = App::getOnlyKey();
	auto errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);	
		App::log(" ( httpUpFile OK ) " + url + "<" + jsonparater + "> => " + json);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = url + "(" + jsonparater + ")>>> httpUpFile error => " + error;
		errorstr = App::replaceChar(errorstr, "&", "___");
		errorstr = App::replaceChar(errorstr, ",", "___");
		errorstr = App::replaceChar(errorstr, ":", "=");
		App::log(" ( httpUpFile error ) " + errorstr);
		App::addErrorLog(errorstr, StringUtils::format("httpUpFile_%d.dat", YYXTime::getInstance()->getIntNumber()), 1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::httpUpFile(url.c_str(), jsonparater.c_str(), filepath.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#endif
}

void CrossPlatform::TraversingFiles(string path, function<void(string filePath, string name)> fileRun, function<void(string fileDir, string name)> dirRun)
{
	if (!FileUtils::getInstance()->isDirectoryExist(path))
		return;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	struct dirent* ent = NULL;
	DIR *pDir;
	pDir = opendir(path.c_str());
	char FileName[512];
	char FolderName[512];
	strcpy(FolderName, path.c_str());
	strcat(FolderName, "/%s");
	if (pDir == NULL) {
		App::log(path + "can not match the folder path");
		return;
	}
	while (NULL != (ent = readdir(pDir))) {
		if (ent->d_type == 8) {
			//文件-删除
			sprintf(FileName, FolderName, ent->d_name);
			if (fileRun)
				fileRun(FileName, ent->d_name);
		}
		else {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
				continue;
			}
			//文件夹-递归
			sprintf(FileName, FolderName, ent->d_name);
			if (dirRun)
				dirRun(FileName, ent->d_name);
		}
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_finddata_t FileInfo;
	string strfind = path + "/*";

	long Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L) {
		App::log(path + "can not match the folder path");
		return;
	}
	do {
		//判断是否有子目录
		if (FileInfo.attrib & _A_SUBDIR)
		{
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{//文件夹-递归
				string newPath = path + "/" + FileInfo.name;
				if (dirRun)
					dirRun(newPath, FileInfo.name);
			}
		}
		else
		{
			//文件-删除
			string filename = (path + "/" + FileInfo.name);
			if (fileRun)
				fileRun(filename, FileInfo.name);
		}
	} while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);
#endif
}

//判断wifi是否存在
bool CrossPlatform::isWifi()
{
	bool dir = true;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	dir = CocosAndroidJni::IsThereWifi();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#endif
	return dir;
}

//获取下载的目录
string CrossPlatform::getDownloadDir()
{
	string dir = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::GetDownloadPath(dir);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	dir = FileUtils::getInstance()->getWritablePath() + "temp";
#endif
	return dir;
}

//获取当前APP的版本号
string CrossPlatform::getAppVersion()
{
	string version;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::getAppVersion(version);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	version = App::GetInstance()->version;
#endif
	return version;
}

//安装安装包
void CrossPlatform::installInstallationPackage(string path)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::InstallApk(path.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	App::log("install " + path);
#endif
}

//VIP
void CrossPlatform::httpVIPPay(int memberId, int rechargeCount, int payMoney, string payType, string payinfo, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect(true))
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		App::log("VIP支付成功的回调");
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = payinfo + ": httpVIPPay error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("httpVIPPay_%d.dat", YYXTime::getInstance()->getIntNumber()), 4);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::httpGetVIPOrderID(memberId, rechargeCount, payMoney, payType.c_str(), payinfo.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#endif
}

//获取本地相册的绝对路径
string CrossPlatform::getAlbumAbsolutePath()
{
	string path = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::GetPhotoPath(path);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	path = FileUtils::getInstance()->getWritablePath() + "temp";
#endif
	return path;
}

//打开相册选中图片,图片保存到指定位置
void CrossPlatform::openPhotoAlbumSelectImage(string fileName,string dir, long width, long height, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		App::log(" ( selcete OK ) headImage = " + dir + "/" + fileName);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = dir+"/"+ fileName + "error: openPhotoAlbumSelectImage => " + error;
		App::addErrorLog(errorstr, StringUtils::format("photo_%d.dat", YYXTime::getInstance()->getIntNumber()), 3);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::openPhotoAlbumSelectImage(fileName.c_str(), dir.c_str(), width, height, runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Toast::create("win32 have not openPhotoAlbum");
#endif
}

//上传头像
void CrossPlatform::httpUpImage(int childID, string ImageFullPath, int memberId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect(true))
		return; 
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		App::log(" ( uploading OK ) " + ImageFullPath);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = ImageFullPath + ": httpUpImage error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getIntNumber()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::UpImage(childID, ImageFullPath.c_str(),  memberId,  runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Toast::create("win32 have not upImage");
#endif
}

//拍照, 保存到指定图片路径
void CrossPlatform::photograph(string fileName, string dir, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		App::log(" ( photograph OK ) headImage = " + dir + "/" + fileName);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = dir + "/" + fileName + "error: photograph => " + error;
		App::addErrorLog(errorstr, StringUtils::format("photo_%d.dat", YYXTime::getInstance()->getIntNumber()),3);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::photographAlbumSelectImage(fileName.c_str(), dir.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//Toast::create("win32 have not photographAlbumSelectImage");
	auto paht = FileUtils::getInstance()->fullPathForFilename(IAMGE_BABYCENTER_CHANGECHILD_CHILD_HEADPORTRAIT);
	if (!FileUtils::getInstance()->isDirectoryExist(dir))
		FileUtils::getInstance()->createDirectory(dir);
	auto res = CopyFileA(paht.c_str(), (dir + "/" + fileName).c_str(), FALSE);
	if (res == TRUE)
	{
		if (runFunction)
		{
			runFunction(dir + "/" + fileName);
		}
	}
	else
	{
		if (errorRunFunction)
		{
			errorRunFunction("");
		}
	}
#endif
}

//图片切成圆形
void CrossPlatform::cutTheRounded(string path, string savePath, long width, long height, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		App::log(" ( cutTheRounded OK )  headImage : " + path+" => "+savePath);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = path + ": cutTheRounded  savePath error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("httpPay_%d.dat", YYXTime::getInstance()->getIntNumber()),3);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::cutTheRounded(path.c_str(), savePath.c_str(), width, height, runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if (CopyFileA(path.c_str(), savePath.c_str(), FALSE))
	{
		if (runFunction)
			runFunction(savePath);
	}
	else
	{
		if (errorRunFunction)
			errorRunFunction("");
	}
#endif
}

//下载
void CrossPlatform::DownLoadFile(string url, string dir, string fileName, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect(true))
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string path) {
		runFunction(path);
		App::log(" ( OK )  " + url + " : DownLoad = > " + path);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = url + ": DownLoadFile " + dir + "/" + fileName + " error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getIntNumber()),2);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	std::thread thisthread(&CrossPlatform::WIN32_DownLoad, url, dir, fileName, runKey, errorKey);
	thisthread.detach();
}

//下载图片---发送消息的回调可以调用本方法下载,如果不是消息进行回调的需要自行书写回调
void CrossPlatform::DownLoadImage(string url, string dir, string fileName, string runKey, function<void(string)> runFunction,string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect())
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string path) {
		runFunction(path);
		App::log(" ( OK ) "+url + " : DownLoad = > " + path);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = url + ": DownLoadImage "+ dir +"/"+fileName+" error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getIntNumber()),2);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//CocosAndroidJni::DownLoadImage(url.c_str(), dir.c_str(), fileName.c_str(), runKey.c_str(), errorKey.c_str());
	std::thread thisthread(&CrossPlatform::WIN32_DownLoad, url, dir, fileName, runKey, errorKey);
	thisthread.detach();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::thread thisthread(&CrossPlatform::WIN32_DownLoad, url, dir, fileName, runKey, errorKey);
	thisthread.detach();
#endif
}

void CrossPlatform::WIN32_DownLoad(string url, string dir, string fileName, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	if (!&url)
	{
		App::log(fileName + " error : DownLoad = > 400");
		if (errorRunable)
			errorRunable("");
		return;
	}
	string str = "\n( download ) fileName = < " + fileName + " >   @@@@@@@@@@ " + "\n";
	App::log(str);
	YYXLayer::writeFilepp(str, FileUtils::getInstance()->getWritablePath() + "temp/log.txt");
	HttpRequest* pRequest = new HttpRequest();
	pRequest->setUrl(url.c_str());
	pRequest->setRequestType(HttpRequest::Type::GET);
	pRequest->setResponseCallback([=](HttpClient* client, HttpResponse* response)
	{
		if (!response)
		{
			App::log(fileName + " error : DownLoad = > 401");
			if (errorRunable)
				errorRunable("");
			return;
		}
		if (!response->isSucceed())
		{
			App::log(fileName + " error : DownLoad = > 402");
			if (errorRunable)
				errorRunable("");			
			return;
		}
		if (!FileUtils::getInstance()->isDirectoryExist(dir))
		{
			FileUtils::getInstance()->createDirectory(dir);
		}
		auto path = dir + "/" + fileName;
		std::vector<char> *buffer = response->getResponseData();
		if (buffer == nullptr)
		{
			App::log(fileName + " error : DownLoad = > 403");
			if (errorRunable)
				errorRunable("");			
			return;
		}
		auto buf = new char[buffer->size()];
		App::log(fileName + " error : DownLoad = > 404");
		std::copy(buffer->begin(), buffer->end(), buf);
		App::log(url + " : DownLoadFile.Size() = ",  buffer->size());
		FILE *fp = fopen(path.c_str(), "wb+");
		App::log(fileName + " error : DownLoad = > 405");
		fwrite(buf, 1, buffer->size(), fp);
		App::log(fileName + " error : DownLoad = > 406");
		fclose(fp);
		string str = "\n( download OK ) fileName = < " + fileName + " >   Size = < "+Value((int)buffer->size() ).asString()+" > @@@@@@@@@@ " + "\n";
		App::log(str);
		YYXLayer::writeFilepp(str, FileUtils::getInstance()->getWritablePath() + "temp/log.txt");
		if (buf)
			delete[] buf;
		if (runable)
			runable(path);		
	});
	HttpClient::getInstance()->send(pRequest);
	pRequest->release();
}

//分享
void CrossPlatform::share(string filePath, string bookName, string targetUrl, string headUrl, string title, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect(true))
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		App::log(" ( share OK )   " + bookName);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = bookName + ": share " + " error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getIntNumber()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::Share(filePath.c_str(), bookName.c_str() , targetUrl.c_str(), headUrl.c_str(), title.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Toast::create("windows have not share");
#endif
}

//获取用户有效红包列表
void CrossPlatform::httpGetUserRedPacketsCallBack(string json, const function<void()> runBegin,const function<void(int, int, string)> itemRun, const function<void(int)> runable, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	auto result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		result = YYXLayer::getBool4Json(false, doc, "result");
		if (result)
		{
			if (runBegin)
				runBegin();
			int  expiring_coupon_count = YYXLayer::getIntForJson(0, doc, "expiring_coupon_count");
			rapidjson::Value arraylist;
			if (YYXLayer::getJsonArray4Json(arraylist, doc, "data"))
			{
				YYXLayer::getDataForJsonArray(arraylist, [=](rapidjson::Value & item, int i) {
					int coupon_id = YYXLayer::getInt4Json(-999, item, "coupon_id");
					int coupon_amount100 = YYXLayer::getDouble4Json(-999.00, item, "coupon_amount") * 100;
					string coupon_expire_time = YYXLayer::getString4Json("", item, "coupon_expire_time");
					if (itemRun)
					{
						itemRun(coupon_id, coupon_amount100, coupon_expire_time);
					}
				});
				if (runable)
					runable(expiring_coupon_count);
			}
		}
	}
	if (!result)
	{
		if (errorRunable)
			errorRunable();
	}
}

//发表语音评论(带界面)
void CrossPlatform::goToSendRecording(int bookId, int memberId, int types, string membername, string orderid, 
	string runKey, function<void(string)> runFunction, //语音成功回调
	string errorKey, function<void(string)> errorRunFunction,//语音失败回调
	string closeKey, function<void(string)> closeFunction)//关闭按钮回调
{
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	closeKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		App::log(" ( VoiceComment OK )   " + StringUtils::format("bookId = %d, memberid = %d,types = %d, membername = ",bookId, memberId, types)+ membername);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
		deleteMapFunction(closeKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
	});
	setMapFunction(closeKey, [=](string closestr) {
		closeFunction(closestr);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
		deleteMapFunction(closeKey);
});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::commentTheRecording(bookId, memberId, types, membername.c_str(), orderid.c_str(), runKey.c_str(), errorKey.c_str(), closeKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Toast::create("windows have not goToSendRecording");
#endif
}

//邀请注册送红包
void CrossPlatform::inviteRegister(int memberId, string url , string runKey, function<void(string)> runFunction,string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect(true))
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		App::log(" ( inviteRegister OK )   " + StringUtils::format("memberid = %d, url = ", memberId) + url);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = url + ": inviteRegister " + " error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getIntNumber()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::inviteRegister(memberId, url.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Toast::create("windows have not inviteRegister");
#endif
}

string CrossPlatform::getPhoneModel(int um)
{//0=机型 1=sdk版本 2=android版本 3=设备码
	string str = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::getPhoneInfo(um, str);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	str = "X2P5T15C26001580";
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	str = "ios";
#endif	
	App::log("PhoneModel = " + str);
	return str;
}

CrossPlatformData::CrossPlatformData()
{

}

CrossPlatformData::~CrossPlatformData()
{
	if(m_call1Key > 0)
		CrossPlatform::getInstance()->delFunc(m_call1Key);
	if(m_call2Key > 0)
		CrossPlatform::getInstance()->delFunc(m_call2Key);
}

CrossPlatformData* CrossPlatformData::create()
{
	auto data = new CrossPlatformData();
	data->setTaskName(YYXTime::getInstance()->getIntNumber());
	return data;
}

void CrossPlatformData::del(CrossPlatformData* data)
{
	if (data)
	{
		delete data;
	}
	data = nullptr;
}

CallBack_String CrossPlatformData::getCall1Key()
{
	CallBack_String call = nullptr;
	if(m_call1Key > 0)
		call = CrossPlatform::getInstance()->getFunc(m_call1Key);
	return call;
}

void CrossPlatformData::setCall1Key(const CallBack_String& val)
{
	m_call1Key = YYXTime::getInstance()->getIntNumber();
	CrossPlatform::getInstance()->addFunc(m_call1Key, val);
}

CallBack_String CrossPlatformData::getCall2Key()
{
	CallBack_String call = nullptr;
	if (m_call2Key > 0)
		call = CrossPlatform::getInstance()->getFunc(m_call2Key);
	return call;
}

void CrossPlatformData::setCall2Key(const CallBack_String& val)
{
	m_call2Key = YYXTime::getInstance()->getIntNumber();
	CrossPlatform::getInstance()->addFunc(m_call2Key, val);
}

//最新的支付宝支付接口
void CrossPlatform::pay(int memberId, int payObjectId, string card_or_recharge, string payPlatform, const CallBack_String & call1, const CallBack_String & call2)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	thread([=]() {
		Toast::create(payPlatform.c_str());
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([]() {
			auto layer = HttpWaiting::getInstance()->newWaitingLayer();
			if (layer)
			{
				Director::getInstance()->getRunningScene()->addChild(layer, 1000);
			}
		});
		auto cData = CrossPlatformData::create();
		if (call1)
		{
			cData->setCall1Key(call1);
		}
		if (call2)
		{
			cData->setCall2Key(call2);
		}
		addTask(cData);
		App::ccsleep(5000);
		YYXLayer::sendNotify(TAG_REMOVEWAITING);
		auto taskName = cData->getTaskName();
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([taskName]() {
			auto laer2 = YYXLayer::MyMessageBox("pay", "yes", [taskName]() {
				auto data = CrossPlatform::getInstance()->getTask(taskName);
				if (data)
				{
					CallBack_String func = data->getCall1Key();
					if (func)
						func("");
				}
				CrossPlatform::getInstance()->delTask(taskName);
			}, "no", [taskName]() {
				auto data = CrossPlatform::getInstance()->getTask(taskName);
				if (data)
				{
					CallBack_String func = data->getCall2Key();
					if (func)
						func("");
				}
				CrossPlatform::getInstance()->delTask(taskName);
			});
			if (laer2)
				Director::getInstance()->getRunningScene()->addChild(laer2, 1000);
		});
	}).detach();
#endif
	if (payPlatform == "wxpay")
	{
		WeixinPay::getInstance()->newPay(memberId, payObjectId, card_or_recharge, call1, call2);
	}
	else if (payPlatform == "alipay") {
		map<string, string> p;
		p["memberid"] = Value(memberId).asString();
		p["paytype"] = Value(payObjectId).asString();//购买对象的ID
		p["contenttpye"] = card_or_recharge;
		p["resource"] = "android";
		p["version"] = App::GetInstance()->version;
		auto cData = CrossPlatformData::create();
		if (call1)
		{
			cData->setCall1Key(call1);
		}
		if (call2)
		{
			cData->setCall2Key(call2);
		}
		p["task"] = Value(cData->getTaskName()).asString();
		auto strJson = YYXLayer::getStringFormMap(p);
		addTask(cData);
		string result = "";
		CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "newMyAliPay", strJson, result);
	}
}

//新的找相册换头像
void CrossPlatform::newAlbumChangeHead(int childId, string oldHead, string newHead, const CallBack_String & call1, const CallBack_String & call2)
{
	map<string, string> p;
	p["childId"] = Value(childId).asString();
	p["oldHead"] = oldHead;
	p["newHead"] = newHead;
	auto cData = CrossPlatformData::create();
	if (call1)
		cData->setCall1Key(call1);
	if (call2)
		cData->setCall2Key(call2);
	p["task"] = Value(cData->getTaskName()).asString();
	auto strJson = YYXLayer::getStringFormMap(p);
	addTask(cData);
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "replaceavatarActivity", strJson, result);
}

//新的绑定手机
void CrossPlatform::newBindPhone(string uid, string platform, const CallBack_String & call1, const CallBack_String & call2)
{
	App::log("newBindPhone-----------uid="+uid);
	map<string, string> p;
	p["uid"] = uid;
	p["platform"] = platform;
	auto cData = CrossPlatformData::create();
	if (call1)
		cData->setCall1Key(call1);
	if (call2)
		cData->setCall2Key(call2);
	p["task"] = Value(cData->getTaskName()).asString();
	auto strJson = YYXLayer::getStringFormMap(p);
	addTask(cData);
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "bindPhoneActivity", strJson, result);
}

//新的绑定第三方
void CrossPlatform::newBindThird(string phone, string uid, int memberId, string platform, const CallBack_String & call1, const CallBack_String & call2)
{
	map<string, string> p;
	p["phone"] = phone;
	p["uid"] = uid;
	p["memberid"] = Value(memberId).asString();
	p["platform"] = platform;
	auto cData = CrossPlatformData::create();
	if (call1)
		cData->setCall1Key(call1);
	if (call2)
		cData->setCall2Key(call2);
	p["task"] = Value(cData->getTaskName()).asString();
	auto strJson = YYXLayer::getStringFormMap(p);
	addTask(cData);
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "bindThirdActivity", strJson, result);
}

//注销
void CrossPlatform::newAccountCancel(string memberAccount, int memberId,  const CallBack_String & call1, const CallBack_String & call2)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	map<string, string> p;
	p["memberid"] = Value(memberId).asString();
	p["account"] = memberAccount;
	auto cData = CrossPlatformData::create();
	if (call1)
		cData->setCall1Key(call1);
	if (call2)
		cData->setCall2Key(call2);
	p["task"] = Value(cData->getTaskName()).asString();
	auto strJson = YYXLayer::getStringFormMap(p);
	addTask(cData);
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "offLineActivity", strJson, result);
#endif
}

//新的解绑第三方
void CrossPlatform::newUnBindThird(int memberId, const CallBack_String & call1, const CallBack_String & call2)
{
	map<string, string> p;
	p["memberid"] = Value(memberId).asString();
	auto cData = CrossPlatformData::create();
	if (call1)
		cData->setCall1Key(call1);
	if (call2)
		cData->setCall2Key(call2);
	p["task"] = Value(cData->getTaskName()).asString();
	auto strJson = YYXLayer::getStringFormMap(p);
	addTask(cData);
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "bindThirdActivity", strJson, result);
}

//新的修改密码
void CrossPlatform::newChangePassword(const CallBack_String & call1, const CallBack_String & call2)
{
	map<string, string> p;
	auto cData = CrossPlatformData::create();
	if (call1)
		cData->setCall1Key(call1);
	if (call2)
		cData->setCall2Key(call2);
	p["task"] = Value(cData->getTaskName()).asString();
	auto strJson = YYXLayer::getStringFormMap(p);
	addTask(cData);
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "ChangePasswordActivity", strJson, result);
}

//新的踢下线
void CrossPlatform::newOffLine()
{
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "OffLineMessageActivity", "", result);
}

void CrossPlatform::newSetMemberId(string memberid)
{
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "setUser4MemberId", memberid, result);
}

void CrossPlatform::ShareWeb(int memberid, int bookid)
{
	string result = "";
	map<string, string> p;
	p["memberid"] = Value(memberid).asString();
	p["bookid"] = Value(bookid).asString();
	auto strJson = YYXLayer::getStringFormMap(p);
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "ShareWeb", strJson, result);
}

void CrossPlatform::deletequdao()
{
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "deletequdao", "", result);
}

void CrossPlatform::gotoQQ(string  qq)
{
	string url = "mqqwpa://im/chat?chat_type=wpa&uin=" + qq + "&version=1";
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "gotoQQ", url, result);
}

void CrossPlatform::CtrlC(string  qq)
{
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "CtrlC", qq, result);
}