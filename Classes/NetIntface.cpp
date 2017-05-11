#include "NetIntface.h"
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

USING_NS_CC;
using namespace std;

NetIntface* NetIntface::m_pInstance = nullptr;
map<string, function<void(string)>> NetIntface::m_functionMap;

NetIntface * NetIntface::getInstance()
{
	if (m_pInstance == nullptr)
		m_pInstance = new NetIntface();
	return m_pInstance;
}

function<void(string)> NetIntface::getMapFunction(string key)
{
	function<void(string)> result = nullptr;
	if (&key && !key.empty())
	{
		auto it = NetIntface::m_functionMap.find(key);
		if ( it != NetIntface::m_functionMap.end())
		{
			result =it->second;
		}
	}
	//App::log("NetIntface::getMapFunction find not" + key);
	return result;
}

void NetIntface::setMapFunction(string key, function<void(string)> runFunction)
{
	if (&key && runFunction && !key.empty())
	{
		//App::log("set" + key);
		NetIntface::m_functionMap[key] = runFunction;
	}
}

void NetIntface::deleteMapFunction(string key)
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

bool NetIntface::IsNetConnect(bool hint)
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

long long NetIntface::getMillsTime()
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
void NetIntface::httpUpFile(string url, string jsonparater, string filepath, function<void(string)> runFunction, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("httpUpFile_%d.dat", YYXTime::getInstance()->getRandomL()), 1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::httpUpFile(url.c_str(), jsonparater.c_str(), filepath.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#endif
}

void NetIntface::WIN32_httpPost(string url, map<string, string> parameter,string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	if (!IsNetConnect())
	{
		if (errorRunable)
			errorRunable("");
		return;
	}
	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 30, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

//网络请求GET方法
//void NetIntface::httpGet(string url , string runKey1, function<void(string)> runFunction, string errorKey1, function<void(string)> errorRunFunction)
//{
//	if (!IsNetConnect())
//		return;
//	string runKey = App::getOnlyKey();
//	string errorKey = App::getOnlyKey();
//	setMapFunction(runKey, [=](string json) {
//		if(runFunction)
//			runFunction(json);
//		App::log(" ( httpGet OK ) "+url + " => "+json);
//		deleteMapFunction(runKey);
//		deleteMapFunction(errorKey);
//	});
//	setMapFunction(errorKey, [=](string error) {
//		if (errorRunFunction)
//			errorRunFunction(error);
//		string errorstr = url + ">>> httpGet error => " + error;
//		App::log(" ( post error ) " + errorstr);
//		//errorstr = App::replaceChar(errorstr, "&", "___");
//		//errorstr = App::replaceChar(errorstr, ",", "___");
//		//errorstr = App::replaceChar(errorstr, ":", "=");
//		//App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getRandomL()),1);
//		deleteMapFunction(runKey);
//		deleteMapFunction(errorKey);
//	});
//	//App::log(url);
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//	//CocosAndroidJni::httpGet(url.c_str(), runKey.c_str(), errorKey.c_str());
//	std::thread thisthread(&NetIntface::WIN32_httpGet, url, runKey, errorKey);
//	thisthread.detach();
//#endif
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//	std::thread thisthread(&NetIntface::WIN32_httpGet, url, runKey, errorKey);
//	thisthread.detach();
//#endif
//}

void NetIntface::WIN32_httpGet(string url, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	if (!IsNetConnect())
	{
		if (errorRunable)
			errorRunable("");
		return;
	}
	stringHttpRequest(HttpRequest::Type::GET, url, map<string, string>(), 30, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::TraversingFiles(string path, function<void(string filePath, string name)> fileRun, function<void(string fileDir, string name)> dirRun)
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
bool NetIntface::isWifi()
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
string NetIntface::getDownloadDir()
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
string NetIntface::getAppVersion()
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

void NetIntface::WIN32_httpAppVersion(string resourst, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = std::string(IP).append(NET_ELLAVERSION);
	auto parameter = map<string, string>();
	parameter["versionResource"] = resourst;
	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::httpAppVersionCallBack(string json, function<void(string)> runable, function<void(string)> errorRun)
{
	rapidjson::Document doc;
	auto result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		int code = YYXLayer::getIntForJson(1, doc, "code");
		if (code == 0)
		{
			string versionNum = YYXLayer::getStringForJson("", doc, "data", "versionNum");
			if (runable)
			{
				runable(versionNum);
			}			
		}
		else
		{
			if (errorRun)
				errorRun("");
		}
	}
}


//安装安装包
void NetIntface::installInstallationPackage(string path)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::InstallApk(path.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	App::log("install " + path);
#endif
}

//支付
void NetIntface::httpPay(int memberId,  int rechargeCount, int payMoney, string payType, string payinfo, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect(true))
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		App::log("支付成功的回调");
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = " ( "+ payType+" ) "+payinfo + ": httpPay error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("httpPay_%d.dat", YYXTime::getInstance()->getRandomL()),4);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::httpGetRechargeOrderID(memberId, rechargeCount, payMoney, payType.c_str(), payinfo.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#endif
}
//VIP
void NetIntface::httpVIPPay(int memberId, int rechargeCount, int payMoney, string payType, string payinfo, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("httpVIPPay_%d.dat", YYXTime::getInstance()->getRandomL()), 4);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::httpGetVIPOrderID(memberId, rechargeCount, payMoney, payType.c_str(), payinfo.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#endif
}

void NetIntface::WIN32_httpShareWithCoupon(int memberID,  string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	auto url = std::string(IP).append(NET_GETSHAREREDPACKET).append(StringUtils::format("?memberId=%d", memberID)).append("&resource=").append(App::m_resource);
	stringHttpRequest(HttpRequest::Type::GET, url, map<string, string>(), 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::WIN32_httpBookStoreSceneCurrentlyPageBookListInfo(int memberID, int BookStoreId, int pageIndex, string m_visitFrom, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	auto url = std::string(IP).append(NET_BOOKSID).append(StringUtils::format("%d", BookStoreId))
		.append("?page=").append(StringUtils::format("%d", pageIndex))
		.append("&pagesize=").append("8")
		.append("&resource=").append(App::m_resource)
		.append("&memberId=").append(StringUtils::format("%d", memberID))
		.append("&visitFrom=").append(m_visitFrom);
	stringHttpRequest(HttpRequest::Type::GET, url, map<string, string>(), 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::WIN32_httpGetUserBuyBooks(int memberID, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = std::string(IP).append(NET_BOOKROOMBOOKLIST).append("memberId=").append
		(StringUtils::format("%d", memberID)).append("&page=1").append("&pagesize=1000").append("&resource=").append(App::GetInstance()->m_resource);
	App::log(url);
	stringHttpRequest(HttpRequest::Type::GET, url, map<string, string>(), 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::WIN32_httpGetUserBalance(int memberID, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = std::string(IP).append(NET_GETRECHARGE) .append("?memberId=").append
		(StringUtils::format("%d", memberID)).append("&resource=").append(App::GetInstance()->m_resource);
	stringHttpRequest(HttpRequest::Type::GET, url, map<string, string>(), 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::WIN32_httpAccountRegiste(string memberName, string memberPasswd, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = string() + IP + NET_REGISTER + "?memberName=" + memberName
		+ "&memberPasswd=" + memberPasswd + "&resource=" + App::m_resource;
	auto parameter = map<string, string>();
	stringHttpRequest(HttpRequest::Type::GET, url, parameter, 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::WIN32_httpAmendUserInfo(int memberId, int sex, string city, string province, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = std::string(IP).append(NET_MODIFY_CITY);
	auto parameter = map<string, string>();
	parameter["memberId"] = StringUtils::format("%d", memberId);
	parameter["city"] = city;
	parameter["sex"] = StringUtils::format("%d", sex);
	parameter["province"] = province;
	parameter["resource"] = App::GetInstance()->m_resource;
	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

//获取本地相册的绝对路径
string NetIntface::getAlbumAbsolutePath()
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

void NetIntface::WIN32_httpAmendBabyInfo(int childrenId, string childrenName, int childrenSex, string childrenBirth, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = std::string(IP).append(NET_MODIFY_CHILD_INFO);
	auto parameter = map<string, string>();
	parameter["childrenId"] = StringUtils::format("%d", childrenId);
	parameter["childrenName"] = childrenName;
	parameter["childrenSex"] = StringUtils::format("%d", childrenSex);
	parameter["childrenBirth"] = childrenBirth;
	parameter["resource"] = App::GetInstance()->m_resource;
	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

//打开相册选中图片,图片保存到指定位置
void NetIntface::openPhotoAlbumSelectImage(string fileName,string dir, long width, long height, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("photo_%d.dat", YYXTime::getInstance()->getRandomL()), 3);
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
void NetIntface::httpUpImage(int childID, string ImageFullPath, int memberId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getRandomL()),1);
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
void NetIntface::photograph(string fileName, string dir, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("photo_%d.dat", YYXTime::getInstance()->getRandomL()),3);
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
void NetIntface::cutTheRounded(string path, string savePath, long width, long height, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("httpPay_%d.dat", YYXTime::getInstance()->getRandomL()),3);
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

void NetIntface::WIN32_httpLogIn(string account, string password, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = string(IP).append(NET_LOGIN);
	auto parameter = map<string, string>();
	parameter["username"] = account;
	parameter["password"] = password;
	parameter["resource"] = App::GetInstance()->m_resource;
	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::WIN32_httpDeleteChild(int memberId, int childrenId, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = std::string(IP).append(NET_DELETECHILD);
	auto parameter = map<string, string>();
	parameter["memberId"] = StringUtils::format("%d", memberId);
	parameter["childrenId"] = StringUtils::format("%d", childrenId);
	parameter["resource"] = App::GetInstance()->m_resource;
	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::WIN32_httpAddChild(int memberId, string childrenName, int childrenSex, string childrenBirth, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = std::string(IP).append(NET_ADDCHILD);
	auto parameter = map<string, string>();
	parameter["memberId"] = StringUtils::format("%d", memberId);
	parameter["childrenName"] = childrenName;
	parameter["childrenSex"] = StringUtils::format("%d", childrenSex);
	parameter["childrenBirth"] = childrenBirth;
	parameter["resource"] = App::GetInstance()->m_resource;
	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

//下载
void NetIntface::DownLoadFile(string url, string dir, string fileName, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getRandomL()),2);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	std::thread thisthread(&NetIntface::WIN32_DownLoad, url, dir, fileName, runKey, errorKey);
	thisthread.detach();
}

//下载图片---发送消息的回调可以调用本方法下载,如果不是消息进行回调的需要自行书写回调
void NetIntface::DownLoadImage(string url, string dir, string fileName, string runKey, function<void(string)> runFunction,string errorKey, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getRandomL()),2);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//CocosAndroidJni::DownLoadImage(url.c_str(), dir.c_str(), fileName.c_str(), runKey.c_str(), errorKey.c_str());
	std::thread thisthread(&NetIntface::WIN32_DownLoad, url, dir, fileName, runKey, errorKey);
	thisthread.detach();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::thread thisthread(&NetIntface::WIN32_DownLoad, url, dir, fileName, runKey, errorKey);
	thisthread.detach();
#endif
}

void NetIntface::WIN32_DownLoad(string url, string dir, string fileName, string runKey, string errorKey)
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

//获取孩子列表
//void NetIntface::httpGetChildDetails(int memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
//{
//	string url = std::string(IP).append(NET_CHILD_DETAILS);
//	map<string, string>  parameter;
//	parameter["memberId"] = StringUtils::format("%d", memberID);
//	parameter["resource"] = App::GetInstance()->m_resource;
//	httpPost(url, parameter, runKey, runFunction, errorKey, errorRunFunction);
//}
//
//void NetIntface::WIN32_httpGetChildDetails(int memberID, string runKey, string errorKey)
//{
//	auto runable = getMapFunction(runKey);
//	auto errorRunable = getMapFunction(errorKey);
//	string url = std::string(IP).append(NET_CHILD_DETAILS);
//	auto parameter = map<string, string>();
//	parameter["memberId"] = StringUtils::format("%d", memberID);
//	parameter["resource"] = App::GetInstance()->m_resource;
//	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 3, [=](string json) {
//		if (runable)
//			runable(json);
//	}, [=]() {
//		if (errorRunable)
//			errorRunable("");
//	});
//}
//
//void NetIntface::httpGetChildDetailsCallBack(string json, const std::function<void(int, int, int, string, string, string, long long)> runable, const std::function<void(int)> resultRunBegin)
//{
//	rapidjson::Document doc;
//	YYXLayer::getJsonObject4Json(doc, json);
//	auto code = YYXLayer::getIntForJson(-999, doc, "code");
//	if (code == 0)
//	{
//		if (resultRunBegin)
//		{
//			resultRunBegin(0);
//		}
//		rapidjson::Value arrayData;
//		YYXLayer::getJsonArray4Json(arrayData, doc, "data");
//		for (rapidjson::SizeType i = 0; i < arrayData.Size(); i++)
//		{
//			rapidjson::Value & item = arrayData[i];
//			auto childrenId = YYXLayer::getInt4Json(-999, item, "childrenId");
//			auto childrenSex = YYXLayer::getInt4Json(-999, item, "childrenSex");
//			auto childrenName = YYXLayer::getString4Json("", item, "childrenName");
//			auto childrenBirth = YYXLayer::getString4Json("", item, "childrenBirth");
//			auto url = YYXLayer::getString4Json("", item, "avatarUrl");
//			auto uptime = App::getCurrentTime();
//			if (runable)
//				runable(i, childrenId, childrenSex, childrenName, childrenBirth, url, uptime);
//		}
//		if (resultRunBegin)
//			resultRunBegin(1);
//	}
//	else
//	{
//		if (resultRunBegin)
//			resultRunBegin(2);
//	}
//}

void NetIntface::WIN32_httpGetFirstChildHeadPortrait(int memberID,  string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = string(IP).append(NET_GETFIRSTCHILDHEADPORTRAIT).append("?memberId=").append(StringUtils::format("%d", memberID));
	stringHttpRequest(HttpRequest::Type::GET, url, map<string, string>(), 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::WIN32_httpGetBookInfo(int bookInfoID, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	auto url = std::string(IP).append(NET_BOOKINFO).append(StringUtils::format("%d", bookInfoID))
		.append("?memberId=").append(App::getMemberID()).append("&resource=").append(App::GetInstance()->m_resource);
	stringHttpRequest(HttpRequest::Type::GET, url, map<string, string>(), 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

//分享
void NetIntface::share(string filePath, string bookName, string targetUrl, string headUrl, string title, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getRandomL()),1);
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
void NetIntface::httpGetUserRedPacketsCallBack(string json, const function<void()> runBegin,const function<void(int, int, string)> itemRun, const function<void(int)> runable, const function<void()> errorRunable)
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
void NetIntface::goToSendRecording(int bookId, int memberId, int types, string membername, string orderid, 
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
void NetIntface::inviteRegister(int memberId, string url , string runKey, function<void(string)> runFunction,string errorKey, function<void(string)> errorRunFunction)
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
		App::addErrorLog(errorstr, StringUtils::format("http_%d.dat", YYXTime::getInstance()->getRandomL()),1);
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

string NetIntface::getPhoneModel(int um)
{//0=机型 1=sdk版本 2=android版本
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