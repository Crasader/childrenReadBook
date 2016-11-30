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
	if (&key && key != "")
	{
		if (NetIntface::m_functionMap.find(key) != NetIntface::m_functionMap.end())
		{
			auto result = NetIntface::m_functionMap[key];
			return result;
		}
	}
	App::log("NetIntface::getMapFunction find not" + key);
	return nullptr;
}

void NetIntface::setMapFunction(string key, function<void(string)> runFunction)
{
	if (&key && runFunction && key != "")
	{
		App::log("set" + key);
		NetIntface::m_functionMap[key] = runFunction;
	}
}

void NetIntface::deleteMapFunction(string key)
{
	if (&key && key != "")
	{
		NetIntface::m_functionMap.erase(key);
	}
}

bool NetIntface::IsNetConnect(bool hint)
{
	bool result = true;
	setMapFunction("showNetConnectError", [](string stt) {
		Toast::create(App::getString("WEIJIANCEDAOWANGLUO"), false);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	App::log("WIN32--This function(NetIntface::IsNetConnect()) is not implemented");
	result = true;
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	result = CocosAndroidJni::IsNetConnect(hint, "showNetConnectError");
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	App::log("IOS--This function(NetIntface::IsNetConnect()) is not implemented");
	result = true;
#endif
	if (result)
		App::log("NetIntface::IsNetConnect = true");
	else
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

//网络请求POST方法
void NetIntface::httpPost(string url, map<string, string> parameter, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect())
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = url+": httpPost error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.txt", (int)YYXLayer::getRandom()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	string p = YYXLayer::getStringFormMap(parameter);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::httpPost(url.c_str(), p.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::thread thisthread(&NetIntface::WIN32_httpPost, url, parameter, runKey, errorKey);
	thisthread.detach();
#endif
}

void NetIntface::WIN32_httpPost(string url, map<string, string> parameter,string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 30, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		string path = FileUtils::getInstance()->getWritablePath() + "error.txt";
		YYXLayer::writeFilepp(url+ "\n\r", path);
		YYXLayer::writeFilepp(YYXLayer::getStringFormMap(parameter) + "\n\r\n\r", path);
		if (errorRunable)
			errorRunable("");
	});
}

//网络请求GET方法
void NetIntface::httpGet(string url , string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect())
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = url + ": httpGet error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.txt", (int)YYXLayer::getRandom()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	//App::log(url);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::httpGet(url.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::thread thisthread(&NetIntface::WIN32_httpGet, url, runKey, errorKey);
	thisthread.detach();
#endif
}

void NetIntface::WIN32_httpGet(string url, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	stringHttpRequest(HttpRequest::Type::GET, url, map<string, string>(), 30, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		string path = FileUtils::getInstance()->getWritablePath() + "error.txt";
		YYXLayer::writeFilepp(url + "\n\r\n\r", path);
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

//上传下载记录
void NetIntface::AddDownLoadRecord(int memberId, int bookId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = string(IP).append(NET_ADDDOWNLOADRECODE);
	map<string, string> paramter;
	paramter["memberId"] = StringUtils::format("%d", memberId);
	paramter["bookId"] = StringUtils::format("%d", bookId);
	paramter["resource"] = App::m_resource;
	httpPost(url, paramter, runKey, runFunction, errorKey, errorRunFunction);
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

//获取服务器App版本
void NetIntface::httpAppVersion(string resourst, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = std::string(IP).append(NET_ELLAVERSION);
	map<string, string> parameter;
	parameter["versionResource"] = resourst;
	httpPost(url, parameter, runKey, runFunction, errorKey, errorRunFunction);
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//#endif
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//	std::thread thisthread(&NetIntface::WIN32_httpAppVersion, resourst, runKey, errorKey);
//	thisthread.detach();
//#endif
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

//获取评论
void NetIntface::httpGetComments(int bookid, string runKey, function<void(string )> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	auto strUrl = std::string(IP).append(NET_BOOKCOMMENT);
	map<string, string > p;
	p["bookId"] = StringUtils::format("%d", bookid);
	p["memberId"] = App::getMemberID();
	p["resource"] = App::GetInstance()->m_resource;
	p["page"] = "1";
	p["pageSize"] = "20";
	//评论
	NetIntface::httpPost(strUrl, p, runKey, runFunction, errorKey, errorRunFunction);
}

void NetIntface::httpBookCommentsCallback(std::string json, const function<void(int, string, string, string, string, string, string, string, string, string, string, string)> itemRun, const function<void()> runable, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	auto result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		int code = YYXLayer::getIntForJson(-999, doc, "code");
		if (code == 0)
		{
			rapidjson::Value arrayData;
			YYXLayer::getJsonArray4Json(arrayData, doc, "data");
			YYXLayer::getDataForJsonArray(arrayData, [=](rapidjson::Value & item, int index)
			{
				string gevalId = YYXLayer::getString4Json("", item, "gevalId");//评论ID
				string gevalType = YYXLayer::getString4Json("", item, "gevalType");//评论类型 0=文字 1=语音
				string score = YYXLayer::getString4Json("", item, "score");//评星
				string memberName = YYXLayer::getString4Json("", item, "memberName");//用户名
				string gevalState = YYXLayer::getString4Json("", item, "gevalState");//0=只评论文字 1=只打分 2=评论文字并打分
				string memberId = YYXLayer::getString4Json("", item, "memberId");
				string commentTime = YYXLayer::getString4Json("", item, "commentTime");//评论时间
				string title = YYXLayer::getString4Json("", item, "title");//标题
				string content = YYXLayer::getString4Json("", item, "content");//内容
				string url = YYXLayer::getString4Json("", item, "url");//语音下载
				string gevalTime = YYXLayer::getString4Json("", item, "gevalTime");//语音时间
				if (itemRun)
				{
					itemRun(index, gevalId, gevalType, score, memberName, gevalState, memberId, commentTime, title, content, url, gevalTime);
				}
			});
			if (runable)
			{
				runable();
			}
		}
		else
			result = false;
	}
	if (!result)
	{
		if (errorRunable)
		{
			errorRunable();
		}
	}
}

//获取充值送红包活动
void NetIntface::httpGetRechargeActivityCallBack(string json, const function<void(int, int)> itemRun, const function<void()> runable, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	auto result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		result = YYXLayer::getBoolForJson(false, doc, "result");
		if (result)
		{
			rapidjson::Value arrayData;
			YYXLayer::getJsonArray4Json(arrayData, doc, "Coupon");
			YYXLayer::getDataForJsonArray(arrayData, [=](rapidjson::Value & item, int index) {
				int coupon_amount = YYXLayer::getInt4Json(-999, item, "coupon_amount");
				if (itemRun)
					itemRun(index, coupon_amount);
			});
			if (runable)
				runable();
		}
	}
	if (!result)
	{
		if (errorRunable)
			errorRunable();
	}
}
/*{
	"result": true,
	"totalCount": 0,
	"data": [],
	"Coupon": [
		{
			"coupon_id": 0,
			"member_id": null,
			"coupon_event_id": null,
			"src_member_id": null,
			"coupon_type": 0,
			"coupon_amount": 0,
			"coupon_discount": 0,
			"coupon_receive_time": null,
			"coupon_consume_time": null,
			"coupon_expire_time": null,
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 1,
			"member_id": null,
			"coupon_event_id": null,
			"src_member_id": null,
			"coupon_type": 0,
			"coupon_amount": 0,
			"coupon_discount": 0,
			"coupon_receive_time": null,
			"coupon_consume_time": null,
			"coupon_expire_time": null,
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 2,
			"member_id": null,
			"coupon_event_id": null,
			"src_member_id": null,
			"coupon_type": 0,
			"coupon_amount": 0,
			"coupon_discount": 0,
			"coupon_receive_time": null,
			"coupon_consume_time": null,
			"coupon_expire_time": null,
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 3,
			"member_id": null,
			"coupon_event_id": null,
			"src_member_id": null,
			"coupon_type": 0,
			"coupon_amount": 5,
			"coupon_discount": 0,
			"coupon_receive_time": null,
			"coupon_consume_time": null,
			"coupon_expire_time": null,
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 4,
			"member_id": null,
			"coupon_event_id": null,
			"src_member_id": null,
			"coupon_type": 0,
			"coupon_amount": 10,
			"coupon_discount": 0,
			"coupon_receive_time": null,
			"coupon_consume_time": null,
			"coupon_expire_time": null,
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 5,
			"member_id": null,
			"coupon_event_id": null,
			"src_member_id": null,
			"coupon_type": 0,
			"coupon_amount": 30,
			"coupon_discount": 0,
			"coupon_receive_time": null,
			"coupon_consume_time": null,
			"coupon_expire_time": null,
			"coupon_state": 0,
			"order_id": 0
		}
	],
	"CouponCode": 0
}*/
//支付
void NetIntface::httpPay(int memberId,  int rechargeCount, int payMoney, string payType, string payinfo, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect())
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = payinfo + ": httpPay error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("httpPay_%d.txt", (int)YYXLayer::getRandom()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::httpGetRechargeOrderID(memberId, rechargeCount, payMoney, payType.c_str(), payinfo.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#endif
}

// 获取书城信息, json解析函数
void NetIntface::httpBookCityInfoCallBack(string json, const function<void(int, int, int, int, int, int, string, string, string, string)> itemRun, const function<void(int)> runable, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	auto result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{		
		int code = YYXLayer::getIntForJson(-999, doc, "code");
		if (code == 0)
		{
			int totalPage = YYXLayer::getIntForJson(-999, doc, "totalPage");
			rapidjson::Value arrayData;
			YYXLayer::getJsonArray4Json(arrayData, doc, "data");
			YYXLayer::getDataForJsonArray(arrayData, [=](rapidjson::Value & item, int index) {
				int castleId = YYXLayer::getInt4Json(-999, item, "castleId");
				int castleType = YYXLayer::getInt4Json(-999, item, "castleType");
				int sort = YYXLayer::getInt4Json(-999, item, "sort");
				int storeBorder = YYXLayer::getInt4Json(-999, item, "storeBorder");
				int hasNewBook = YYXLayer::getInt644Json(-999, item, "hasNewBook");
				string castleName = YYXLayer::getString4Json("", item, "castleName");
				string borderUrl = YYXLayer::getString4Json("", item, "borderUrl");
				string bgUrl = YYXLayer::getString4Json("", item, "bgUrl");
				string bagUrl = YYXLayer::getString4Json("", item, "bagUrl");
				if (itemRun)
				{
					itemRun(index, castleId, castleType, sort, storeBorder, hasNewBook, castleName, borderUrl, bgUrl, bagUrl);
				}
			});	
			if (runable)
			{
				runable(totalPage);
			}
		}
		else
			result = false;
	}
	if (!result)
	{
		if (errorRunable)
			errorRunable();
	}
}

// 获取分享成功的红包
void NetIntface::httpShareWithCoupon(int memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	auto url = std::string(IP).append(NET_GETSHAREREDPACKET).append(StringUtils::format("?memberId=%d", memberID)).append("&resource=").append(App::m_resource);
	httpGet(url, runKey, runFunction, errorKey, errorRunFunction);
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
//{"errorMessage":"",
//	"id":"",
//	"result":true,
//	"data":{
//		"coupon_id":986,
//		"member_id":4627,
//		"coupon_event_id":1,
//		"src_member_id":-999,
//		"coupon_type":1,
//		"coupon_amount":2.00,
//		"coupon_discount":0.0,
//		"coupon_receive_time":"2016-08-26 16:42:09.0",
//		"coupon_consume_time":null,
//		"coupon_expire_time":"2016-09-02 16:42:48.0",
//		"coupon_state":0,"order_id":0},
//	"code":0,
//	"toURL":""}

void NetIntface::httpShareWithCouponCallBack(string json, const function<void(int)> itemRun, const function<void()> runable, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	auto result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		int code = YYXLayer::getIntForJson(-999, doc, "code");
		if (code == 0)
		{
			int coupon_amount = YYXLayer::getDoubleForJson(-999, doc, "data", "coupon_amount") *100;
			if (itemRun)
				itemRun(coupon_amount);
		}
		else
			result = false;
	}
	if (!result)
	{
		if (errorRunable)
			errorRunable();
	}
}

//获取书店当前页的书籍列表信息
void NetIntface::httpBookStoreSceneCurrentlyPageBookListInfo(int memberID, int BookStoreId, int pageIndex, string visitFrom, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	auto url = std::string(IP).append(NET_BOOKSID).append(StringUtils::format("%d", BookStoreId))
		.append("?page=").append(StringUtils::format("%d", pageIndex))
		.append("&pagesize=").append("8")
		.append("&resource=").append(App::m_resource)
		.append("&memberId=").append(StringUtils::format("%d", memberID))
		.append("&visitFrom=").append(visitFrom);
	httpGet(url, runKey, runFunction, errorKey, errorRunFunction);
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
/*{
	"code": 0,
	"data": [
		{
			"goodsId": 100385,
			"bookId": 431,
			"bookName": "伊娃和丽萨",
			"bookCoverUrl": "http://book.ellabook.cn/d50f243589384a65a83793409920a6ef?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=74hzXWoB2039gyogsdrJemqMwTI%3D",
			"bookListenUrl": "",
			"bookReadUrl": "",
			"bookPlayUrl": "http://book.ellabook.cn/05dfab660e6a4f69a2708aba0cc97da0?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=KB/YVreyIFWsjn2ViDA74BpKyEw%3D",
			"bookMakeUrl": "",
			"bookPrice": 3.99,
			"downloadNum": 16,
			"downloadTime": 0,
			"hashBuy": 1,
			"orderId": 0,
			"bookAuthor": "封雨",
			"goods_addtime": 1471603450,
			"goods_state": 1,
			"goods_verify": 1,
			"ossbucket": null,
			"osskey": "05dfab660e6a4f69a2708aba0cc97da0",
			"bookDownloadSize": 0,
			"coverKey": "d50f243589384a65a83793409920a6ef",
			"bookPress": "暖绘本",
			"payMent": 47.88,
			"bookScore": 0,
			"bookmarketPrice": 30,
			"remainTime": 629092,
			"isNewEllaBook": 1
		},
		{
			"goodsId": 100387,
			"bookId": 433,
			"bookName": "魔法药水",
			"bookCoverUrl": "http://book.ellabook.cn/afd0a5e6e19e44409736673c3ec51eda?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=ie3PzFCAaJNyspfaHS%2BvUsmzXpQ%3D",
			"bookListenUrl": "",
			"bookReadUrl": "",
			"bookPlayUrl": "http://book.ellabook.cn/2ff6375aa29c4f20a59021fe7bb8a99e?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=WRXsE3Sog9yhRqvuQoZicrt%2B%2B00%3D",
			"bookMakeUrl": "",
			"bookPrice": 2.99,
			"downloadNum": 31,
			"downloadTime": 0,
			"hashBuy": 1,
			"orderId": 0,
			"bookAuthor": "宋雨",
			"goods_addtime": 1471864849,
			"goods_state": 1,
			"goods_verify": 1,
			"ossbucket": null,
			"osskey": "2ff6375aa29c4f20a59021fe7bb8a99e",
			"bookDownloadSize": 0,
			"coverKey": "afd0a5e6e19e44409736673c3ec51eda",
			"bookPress": "蒲蕾英",
			"payMent": 62.84,
			"bookScore": 0,
			"bookmarketPrice": 24,
			"remainTime": 631553,
			"isNewEllaBook": 1
		},
		{
			"goodsId": 100383,
			"bookId": 429,
			"bookName": "二郎担山赶太阳",
			"bookCoverUrl": "http://book.ellabook.cn/7deabe83ee95458a8c2e96f3d65e2cb4?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=J2lxjFAbxtprPDYAj55VaMPw07E%3D",
			"bookListenUrl": "",
			"bookReadUrl": "",
			"bookPlayUrl": "http://book.ellabook.cn/456d061f049f43f9804ff488a16f6410?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=OUd1SzAksRr1OzaAu/TlkVg%2BskE%3D",
			"bookMakeUrl": "",
			"bookPrice": 2.99,
			"downloadNum": 36,
			"downloadTime": 0,
			"hashBuy": 1,
			"orderId": 0,
			"bookAuthor": "咿啦看书",
			"goods_addtime": 1471491103,
			"goods_state": 1,
			"goods_verify": 1,
			"ossbucket": null,
			"osskey": "456d061f049f43f9804ff488a16f6410",
			"bookDownloadSize": 0,
			"coverKey": "7deabe83ee95458a8c2e96f3d65e2cb4",
			"bookPress": "新蕾书店",
			"payMent": 86.71,
			"bookScore": 0,
			"bookmarketPrice": 10,
			"remainTime": 602050,
			"isNewEllaBook": 1
		},
		{
			"goodsId": 100380,
			"bookId": 426,
			"bookName": "字母小课堂（上）",
			"bookCoverUrl": "http://book.ellabook.cn/078506949e1c4193a41a4e092e3cc9a4?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=ELkrt83qzOBdYO7ijliCT472gdk%3D",
			"bookListenUrl": "",
			"bookReadUrl": "",
			"bookPlayUrl": "http://book.ellabook.cn/1fe928006f6846dbb421aa452436a2db?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=XSY2FQbhXA%2BtaOtlwnQ6H5CLBBU%3D",
			"bookMakeUrl": "",
			"bookPrice": 0.99,
			"downloadNum": 28,
			"downloadTime": 0,
			"hashBuy": 1,
			"orderId": 0,
			"bookAuthor": "孙瑾慧",
			"goods_addtime": 1471242158,
			"goods_state": 1,
			"goods_verify": 1,
			"ossbucket": null,
			"osskey": "1fe928006f6846dbb421aa452436a2db",
			"bookDownloadSize": 0,
			"coverKey": "078506949e1c4193a41a4e092e3cc9a4",
			"bookPress": "康轩书店",
			"payMent": 21.78,
			"bookScore": 0,
			"bookmarketPrice": 10,
			"remainTime": 612767,
			"isNewEllaBook": 1
		},
		{
			"goodsId": 100381,
			"bookId": 427,
			"bookName": "我想要个红帽子",
			"bookCoverUrl": "http://book.ellabook.cn/372f851c71814e1a8b65720e94211721?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=fi1NizclznAusUtsSXbkhZlSzv8%3D",
			"bookListenUrl": "",
			"bookReadUrl": "",
			"bookPlayUrl": "http://book.ellabook.cn/5af24b2ef7394e6ab5d622691e15fe34?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=H42MvBgb7GaPYkwcD1M94hpbLFs%3D",
			"bookMakeUrl": "",
			"bookPrice": 3.99,
			"downloadNum": 43,
			"downloadTime": 0,
			"hashBuy": 1,
			"orderId": 0,
			"bookAuthor": "杨冰洋",
			"goods_addtime": 1471242216,
			"goods_state": 1,
			"goods_verify": 1,
			"ossbucket": null,
			"osskey": "5af24b2ef7394e6ab5d622691e15fe34",
			"bookDownloadSize": 0,
			"coverKey": "372f851c71814e1a8b65720e94211721",
			"bookPress": "爱心河精品绘本馆",
			"payMent": 107.67,
			"bookScore": 0,
			"bookmarketPrice": 28,
			"remainTime": 611809,
			"isNewEllaBook": 1
		},
		{
			"goodsId": 100377,
			"bookId": 423,
			"bookName": "女娲补天",
			"bookCoverUrl": "http://book.ellabook.cn/e2f4f636658649e99a371f7485c60ca9?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=mHWSybXK%2BiHejKXLes7/ensOtTE%3D",
			"bookListenUrl": "",
			"bookReadUrl": "",
			"bookPlayUrl": "http://book.ellabook.cn/934a8cdcf7054245be918ed86a65d95b?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=uzDvH6GfsKIKEkUI7GqH7IJDE80%3D",
			"bookMakeUrl": "",
			"bookPrice": 2.99,
			"downloadNum": 18,
			"downloadTime": 0,
			"hashBuy": 1,
			"orderId": 0,
			"bookAuthor": "咿啦看书",
			"goods_addtime": 1470997079,
			"goods_state": 1,
			"goods_verify": 1,
			"ossbucket": null,
			"osskey": "934a8cdcf7054245be918ed86a65d95b",
			"bookDownloadSize": 0,
			"coverKey": "e2f4f636658649e99a371f7485c60ca9",
			"bookPress": "新蕾书店",
			"payMent": 68.77,
			"bookScore": 0,
			"bookmarketPrice": 10,
			"remainTime": 627634,
			"isNewEllaBook": 1
		},
		{
			"goodsId": 100376,
			"bookId": 422,
			"bookName": "鲧禹治水",
			"bookCoverUrl": "http://book.ellabook.cn/4008708cf8784923b91721acec2e7f65?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=mdQt8yoyou4Nc6fpp3uNFVQL16w%3D",
			"bookListenUrl": "",
			"bookReadUrl": "",
			"bookPlayUrl": "http://book.ellabook.cn/123d0e3ef7234cc68c2f87f2c0f2db56?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=A6XoI4k%2BzvObS/HOl1zKuvvYncY%3D",
			"bookMakeUrl": "",
			"bookPrice": 2.99,
			"downloadNum": 28,
			"downloadTime": 0,
			"hashBuy": 1,
			"orderId": 0,
			"bookAuthor": "咿啦看书",
			"goods_addtime": 1470971175,
			"goods_state": 1,
			"goods_verify": 1,
			"ossbucket": null,
			"osskey": "123d0e3ef7234cc68c2f87f2c0f2db56",
			"bookDownloadSize": 0,
			"coverKey": "4008708cf8784923b91721acec2e7f65",
			"bookPress": "新蕾书店",
			"payMent": 73.74,
			"bookScore": 0,
			"bookmarketPrice": 10,
			"remainTime": 600330,
			"isNewEllaBook": 1
		},
		{
			"goodsId": 100375,
			"bookId": 421,
			"bookName": "学画入门建筑风景篇一",
			"bookCoverUrl": "http://book.ellabook.cn/8a4b9fc80b75456b881ee8bc0f8f67a0?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=L8eQrSTJkK4ioZnrip1zZw6CDzE%3D",
			"bookListenUrl": "",
			"bookReadUrl": "",
			"bookPlayUrl": "http://book.ellabook.cn/e2122d70baae4bee8fdb083d54c3dd47?Expires=1473738378&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=bhM4WBmLNhCDcY5YKMvy97iq9oc%3D",
			"bookMakeUrl": "",
			"bookPrice": 0.99,
			"downloadNum": 105,
			"downloadTime": 0,
			"hashBuy": 1,
			"orderId": 0,
			"bookAuthor": "樊慧",
			"goods_addtime": 1470807603,
			"goods_state": 1,
			"goods_verify": 1,
			"ossbucket": null,
			"osskey": "e2122d70baae4bee8fdb083d54c3dd47",
			"bookDownloadSize": 0,
			"coverKey": "8a4b9fc80b75456b881ee8bc0f8f67a0",
			"bookPress": "河南科技",
			"payMent": 10.89,
			"bookScore": 0,
			"bookmarketPrice": 7.8,
			"remainTime": 591234,
			"isNewEllaBook": 1
		}
	],
	"totalCount": 132,
	"desc": "success"
}*/
void NetIntface::httpBookStoreSceneCurrentlyPageBookListInfoCallBack(string json, const function<void(int, int, int, int, int, int, int, int, string, string, string, string, string)> itemRun, const function<void()> runable, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	auto result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		int code = YYXLayer::getIntForJson(-999, doc, "code");
		if (code == 0)
		{
			result = true;
			int totalCount = YYXLayer::getIntForJson(-999, doc, "totalCount");
			rapidjson::Value arrayData;
			YYXLayer::getJsonArray4Json(arrayData, doc, "data");
			YYXLayer::getDataForJsonArray(arrayData, [=](rapidjson::Value & item, int index) {
				int bookId = YYXLayer::getInt4Json(-999, item, "bookId");
				int hashBuy = YYXLayer::getInt4Json(-999, item, "hashBuy");
				int bookPrice = YYXLayer::getDouble4Json(-999, item, "bookPrice")*100;
				if (bookPrice == -99900)
					bookPrice = YYXLayer::getInt4Json(-999, item, "bookPrice") * 100;
				int isNewEllaBook = YYXLayer::getInt4Json(-999, item, "isNewEllaBook");
				auto remainTime = YYXLayer::getInt644Json(-999, item, "remainTime");
				int bookmarketPrice = YYXLayer::getDouble4Json(-999, item, "bookmarketPrice")*100;
				string bookCoverUrl = YYXLayer::getString4Json("", item, "bookCoverUrl");
				string bookPlayUrl = YYXLayer::getString4Json("", item, "bookPlayUrl");
				string bookAuthor = YYXLayer::getString4Json("", item, "bookAuthor");
				string bookPress = YYXLayer::getString4Json("", item, "bookPress");
				string bookName = YYXLayer::getString4Json("", item, "bookName");
				if (itemRun)
				{
					itemRun(totalCount, index, bookId, isNewEllaBook, hashBuy, bookPrice, bookmarketPrice, remainTime, bookName, bookAuthor, bookPress, bookCoverUrl, bookPlayUrl);
				}
			});
			if (runable)
			{
				runable();
			}
		}
		else
			result = false;
	}
	if (!result)
	{
		if (errorRunable)
			errorRunable();
	}
}

//用户书籍的已购列表
void NetIntface::httpGetUserBuyBooks(long memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = std::string(IP).append(NET_BOOKROOMBOOKLIST).append("memberId=").append
		(StringUtils::format("%d", memberID)).append("&page=1").append("&pagesize=1000").append("&resource=").append(App::GetInstance()->m_resource);
	httpGet(url, runKey, runFunction, errorKey, errorRunFunction);
}
/*{"data":[{"goodsId":100290,"bookMakeUrl":"","bookId":336,"goods_verify":0,"hashBuy":1,"bookAuthor":"孙瑾慧  张霞","remainTime":0,"osskey":"2adbb845e5c14f138c4120260ada449b","downloadTime":1471344212,"isNewEllaBook":0,"bookPrice":0,"bookmarketPrice":0,"orderId":2050,"bookName":"点点乐园  识字（五）","downloadNum":4,"bookDownloadSize":0,"ossbucket":"ellabook-book","payMent":0,"bookPress":null,"bookReadUrl":"","goods_addtime":0,"bookScore":0,"goods_state":0,"coverKey":"464359ffa47d4aaf83564dc868c5e299","bookCoverUrl":"","bookPlayUrl":"","bookListenUrl":""},{"goodsId":13,"bookMakeUrl":"","bookId":13,"goods_verify":0,"hashBuy":1,"bookAuthor":"朱文迪 刘晓博","remainTime":0,"osskey":"b9a1e5892bae4de9a9f039c04ca06d22","downloadTime":1471344212,"isNewEllaBook":0,"bookPrice":0,"bookmarketPrice":0,"orderId":2035,"bookName":"动态阅读8-3文字游乐园","downloadNum":3,"bookDownloadSize":0,"ossbucket":"ellabook-book","payMent":0,"bookPress":null,"bookReadUrl":"","goods_addtime":0,"bookScore":0,"goods_state":0,"coverKey":"1a8c9db1fbcd4331aba67686226fb7d0","bookCoverUrl":"","bookPlayUrl":"","bookListenUrl":""},{"goodsId":7,"bookMakeUrl":"","bookId":7,"goods_verify":0,"hashBuy":1,"bookAuthor":"樊慧  陈桐","remainTime":0,"osskey":"ae651eaeb1524ebe9dcb8e9a1b0d6e24","downloadTime":1471249126,"isNewEllaBook":0,"bookPrice":0,"bookmarketPrice":0,"orderId":2034,"bookName":"动态阅读2-1来，抱抱","downloadNum":3,"bookDownloadSize":0,"ossbucket":"ellabook-book","payMent":0,"bookPress":null,"bookReadUrl":"","goods_addtime":0,"bookScore":0,"goods_state":0,"coverKey":"cc3d0c5068e140f28d8c076f24e64af3","bookCoverUrl":"","bookPlayUrl":"","bookListenUrl":""},{"goodsId":100277,"bookMakeUrl":"","bookId":323,"goods_verify":0,"hashBuy":1,"bookAuthor":"霍君尧 马文静","remainTime":0,"osskey":"e4b6e6bfa31b4213b238d7187f25ee01","downloadTime":1471240154,"isNewEllaBook":0,"bookPrice":0,"bookmarketPrice":0,"orderId":2029,"bookName":"点点乐园  识字（六）","downloadNum":2,"bookDownloadSize":0,"ossbucket":"ellabook-book","payMent":0,"bookPress":null,"bookReadUrl":"","goods_addtime":0,"bookScore":0,"goods_state":0,"coverKey":"ac2d873ad14e42419b93cdafd16469b0","bookCoverUrl":"","bookPlayUrl":"","bookListenUrl":""},{"goodsId":100283,"bookMakeUrl":"","bookId":329,"goods_verify":0,"hashBuy":1,"bookAuthor":"刘晓博","remainTime":0,"osskey":"1b41b32ef9934ace95139d5a74777063","downloadTime":1471344211,"isNewEllaBook":0,"bookPrice":0,"bookmarketPrice":0,"orderId":2028,"bookName":"卖火柴的小女孩","downloadNum":3,"bookDownloadSize":0,"ossbucket":"ellabook-book","payMent":0,"bookPress":null,"bookReadUrl":"","goods_addtime":0,"bookScore":0,"goods_state":0,"coverKey":"4028e1116b994a9e91dd618ed357a527","bookCoverUrl":"","bookPlayUrl":"","bookListenUrl":""},{"goodsId":100302,"bookMakeUrl":"","bookId":348,"goods_verify":0,"hashBuy":1,"bookAuthor":"张宇飞","remainTime":0,"osskey":"a54601a9607a4bd5b35cfbeca27bb422","downloadTime":1471240149,"isNewEllaBook":0,"bookPrice":0,"bookmarketPrice":0,"orderId":2025,"bookName":"如果没有其他小朋友的话","downloadNum":2,"bookDownloadSize":0,"ossbucket":"ellabook-book","payMent":0,"bookPress":null,"bookReadUrl":"","goods_addtime":0,"bookScore":0,"goods_state":0,"coverKey":"d37b8fcfc6bf42b6921d1c4da35ecb26","bookCoverUrl":"","bookPlayUrl":"","bookListenUrl":""},{"goodsId":14,"bookMakeUrl":"","bookId":14,"goods_verify":0,"hashBuy":1,"bookAuthor":"李朋   张宇飞","remainTime":0,"osskey":"dd8bcb2c66d348d18b6632c92aa1ac74","downloadTime":1471245886,"isNewEllaBook":0,"bookPrice":0,"bookmarketPrice":0,"orderId":2024,"bookName":"动态阅读4-2春天在哪里","downloadNum":9,"bookDownloadSize":0,"ossbucket":"ellabook-book","payMent":0,"bookPress":null,"bookReadUrl":"","goods_addtime":0,"bookScore":0,"goods_state":0,"coverKey":"101e19249d9c4b7c98a726970c2398d9","bookCoverUrl":"","bookPlayUrl":"","bookListenUrl":""},{"goodsId":2,"bookMakeUrl":"","bookId":2,"goods_verify":0,"hashBuy":1,"bookAuthor":"曹珑","remainTime":0,"osskey":"67281fe28af04b9cbce87ed*/
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

void NetIntface::httpGetUserBuyBooksCallBack(string json,function<void()> beginable, const function<void(int, int, string, string, string)> itemable, function<void()> runableSuccessOver, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	bool result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		auto code = YYXLayer::getIntForJson(-1, doc, "code");
		if (code == 0 )
		{
			if (beginable)
				beginable();
			rapidjson::Value  jsonarray;
			if (YYXLayer::getJsonArray4Json(jsonarray, doc, "data"))
			{
				map<string, YYXStruct> itemMap;
				YYXLayer::getData4JsonArray(jsonarray, itemMap, [=](rapidjson::Value & item, map<string, YYXStruct>& mapresult) {
					auto bookId = YYXLayer::getInt4Json(-999, item, "bookId");
					auto orderId = YYXLayer::getInt4Json(-999, item, "orderId");
					auto bookCoverUrl = YYXLayer::getString4Json("", item, "bookCoverUrl");
					auto bookPlayUrl = YYXLayer::getString4Json("", item, "bookPlayUrl");
					auto bookName= YYXLayer::getString4Json("", item, "bookName");
					if (itemable)
						itemable(bookId, orderId, bookCoverUrl, bookPlayUrl, bookName);
				});
				if (runableSuccessOver)
					runableSuccessOver();
			}
			else
			{
				if (errorRunable)
					errorRunable();
			}
		}
		else
		{
			if (errorRunable)
				errorRunable();
		}
	}
	else
	{
		if (errorRunable)
			errorRunable();
	}
}

//获取用户安卓的账户余额
void NetIntface::httpGetUserBalance(long memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = std::string(IP).append(NET_GETRECHARGE).append("?memberId=").append
		(StringUtils::format("%d", memberID)).append("&resource=").append(App::GetInstance()->m_resource);
	NetIntface::httpGet(url, runKey, runFunction, errorKey, errorRunFunction);
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
/*{
	"result": true,
	"data": {
		"balance_id": 13093,
		"balance_person_type": 1,
		"balance_person_id": 17167,
		"balance_person_name": "0",
		"balance_amount": 34,
		"pe_updatetime": 1470128217,
		"member_id": 17167,
		"balance_apple_amount": 0
	}
}*/
void NetIntface::httpGetUserBalanceCallBack(string json, const function<void(int, int, long long )> runable, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	bool result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		result = YYXLayer::getBoolForJson(false, doc, "result");
		if (result)
		{
			auto id = YYXLayer::getIntForJson(-999, doc, "data", "member_id");
			int userBalance = YYXLayer::getDoubleForJson(0, doc, "data","balance_amount") *100;
			auto uptime = YYXLayer::getInt64ForJson(0, doc, "data", "pe_updatetime");
			if (runable)
				runable(id, userBalance, uptime);
		}
		else 
		{
			if (errorRunable)
				errorRunable();
		}
	}
	else
	{
		if (errorRunable)
			errorRunable();
	}
}

//注册
void NetIntface::httpAccountRegiste(string memberName, string memberPasswd, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = string() + IP + NET_REGISTER + "?memberName=" + memberName
		+ "&memberPasswd=" + memberPasswd + "&resource=" + App::m_resource;
	httpGet(url, runKey, runFunction, errorKey, errorRunFunction);
}

void NetIntface::WIN32_httpAccountRegiste(string memberName, string memberPasswd, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = string() + IP + NET_REGISTER + "?memberName=" + memberName
		+ "&memberPasswd=" + memberPasswd + "&resource=" + App::m_resource;
	auto parameter = map<string, string>();
	//parameter["memberName"] = memberName;
	//parameter["memberPasswd"] = memberPasswd;
	//parameter["resource"] = App::GetInstance()->m_resource;
	//stringHttpRequest(HttpRequest::Type::POST, url, parameter, 3, [=](string json) {
	stringHttpRequest(HttpRequest::Type::GET, url, parameter, 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}
/*{
	"errorMessage": "",
	"id": "",
	"result": true,
	"data": {
		"memberSex": "1",
		"memberId": 17203,
		"memberCity": null,
		"memberName": "18888702708",
		"memberProvince": null
	},
	"code": 0,
	"coupon": [
		{
			"coupon_id": 473,
			"member_id": 17203,
			"coupon_event_id": 4,
			"src_member_id": -999,
			"coupon_type": 1,
			"coupon_amount": 3,
			"coupon_discount": 0,
			"coupon_receive_time": "2016-09-14 14:35:47",
			"coupon_consume_time": null,
			"coupon_expire_time": "2016-09-21 14:35:47",
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 474,
			"member_id": 17203,
			"coupon_event_id": 4,
			"src_member_id": -999,
			"coupon_type": 1,
			"coupon_amount": 2,
			"coupon_discount": 0,
			"coupon_receive_time": "2016-09-14 14:35:47",
			"coupon_consume_time": null,
			"coupon_expire_time": "2016-09-21 14:35:47",
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 475,
			"member_id": 17203,
			"coupon_event_id": 4,
			"src_member_id": -999,
			"coupon_type": 1,
			"coupon_amount": 2,
			"coupon_discount": 0,
			"coupon_receive_time": "2016-09-14 14:35:47",
			"coupon_consume_time": null,
			"coupon_expire_time": "2016-09-21 14:35:47",
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 476,
			"member_id": 17203,
			"coupon_event_id": 4,
			"src_member_id": -999,
			"coupon_type": 1,
			"coupon_amount": 1,
			"coupon_discount": 0,
			"coupon_receive_time": "2016-09-14 14:35:47",
			"coupon_consume_time": null,
			"coupon_expire_time": "2016-09-21 14:35:47",
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 477,
			"member_id": 17203,
			"coupon_event_id": 4,
			"src_member_id": -999,
			"coupon_type": 1,
			"coupon_amount": 1,
			"coupon_discount": 0,
			"coupon_receive_time": "2016-09-14 14:35:47",
			"coupon_consume_time": null,
			"coupon_expire_time": "2016-09-21 14:35:47",
			"coupon_state": 0,
			"order_id": 0
		},
		{
			"coupon_id": 478,
			"member_id": 17203,
			"coupon_event_id": 4,
			"src_member_id": -999,
			"coupon_type": 1,
			"coupon_amount": 1,
			"coupon_discount": 0,
			"coupon_receive_time": "2016-09-14 14:35:47",
			"coupon_consume_time": null,
			"coupon_expire_time": "2016-09-21 14:35:47",
			"coupon_state": 0,
			"order_id": 0
		}
	],
	"success": "/member/login?username=18888702708",
	"couponSum": "10",
	"toURL": ""
}*/
void NetIntface::httpAccountRegisteCallBack(string json, const function<void(int, int, string, string, string, string)> runable, const function<void(string errorstr)> errorRunable)
{
	rapidjson::Document doc;
	bool result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		result = YYXLayer::getBoolForJson(false, doc, "result");
		auto errorstr = YYXLayer::getStringForJson("", doc, "fail");
		if (result)
		{
			auto memberId = YYXLayer::getIntForJson(-999, doc, "data", "memberId");
			auto memberSex = __String(YYXLayer::getStringForJson("1", doc, "data", "memberSex")).intValue();
			auto memberName = YYXLayer::getStringForJson("", doc, "memberName");
			auto memberProvince = YYXLayer::getStringForJson("", doc, "memberProvince");
			auto memberCity = YYXLayer::getStringForJson("", doc, "memberCity");
			string couponSum = YYXLayer::getStringForJson("", doc, "couponSum");
			if (memberId != -999)
			{
				if (runable)
					runable(memberId, memberSex, memberName, memberProvince, memberCity, couponSum);
			}
			else
			{
				if (errorRunable)
					errorRunable(errorstr);
			}
		}
		else
		{
			if (errorRunable)
				errorRunable(errorstr);
		}
	}
	else
	{
		if (errorRunable)
			errorRunable("");
	}
}

//修改用户信息
void NetIntface::httpAmendUserInfo(int memberId, int sex, string city,  string province, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	url = std::string(IP).append(NET_MODIFY_CITY_ANDROID);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	url = std::string(IP).append(NET_MODIFY_CITY);
#endif
	map<string, string> parameter;
	parameter["memberId"] = StringUtils::format("%d", memberId);
	parameter["city"] = city;
	parameter["sex"] = StringUtils::format("%d", sex);
	parameter["province"] = province;
	parameter["resource"] = App::GetInstance()->m_resource;
	httpPost(url, parameter, runKey, runFunction, errorKey, errorRunFunction);
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

//解析函数
void NetIntface::httpAmendUserInfoCallBack(string json, const function<void()> runable, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	bool result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		auto code = YYXLayer::getIntForJson(-1, doc, "code");
		if (code == 0)
		{
			result = true;
			if (runable)
				runable();
		}
		else
			result = false;
	}
	if (!result)
	{
		if (errorRunable)
			errorRunable();
	}
}

//UTF-8转GBK
string NetIntface::UTF82GBK(string UTF)
{
	string GBK = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::UTF82GBK(GBK.c_str(), UTF.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	GBK = UTF;
#endif
	return GBK;
}
//GBK转UTF-8
string NetIntface::GBK2UTF8(string GBK)
{
	string UTF = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::UTF82GBK(GBK.c_str(), UTF.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	UTF = GBK;
#endif
	return UTF;
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

//修改宝贝信息
void NetIntface::httpAmendBabyInfo(int childrenId, string childrenName, int childrenSex, string childrenBirth, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = std::string(IP).append(NET_MODIFY_CHILD_INFO);
	map<string, string> parameter;
	parameter["childrenId"] = StringUtils::format("%d", childrenId);
	parameter["childrenName"] = childrenName;
	parameter["childrenSex"] = StringUtils::format("%d", childrenSex);
	parameter["childrenBirth"] = childrenBirth;
	parameter["resource"] = App::GetInstance()->m_resource;
	httpPost(url, parameter, runKey, runFunction, errorKey, errorRunFunction);
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
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = dir+"/"+ fileName + "error: openPhotoAlbumSelectImage => " + error;
		App::addErrorLog(errorstr, StringUtils::format("photo_%d.txt", (int)YYXLayer::getRandom()), 3);
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
	if (!IsNetConnect())
		return; 
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = ImageFullPath + ": httpUpImage error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.txt", (int)YYXLayer::getRandom()),1);
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
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = dir + "/" + fileName + "error: photograph => " + error;
		App::addErrorLog(errorstr, StringUtils::format("photo_%d.txt", (int)YYXLayer::getRandom()),3);
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
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = path + ": cutTheRounded  savePath error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("httpPay_%d.txt", (int)YYXLayer::getRandom()),3);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::cutTheRounded(path.c_str(), savePath.c_str(), width, height, runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	CopyFileA(path.c_str(), savePath.c_str(), FALSE);
#endif
}

//登录
void NetIntface::httpLogIn(string account, string password, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = string(IP).append(NET_LOGIN);
	map<string, string> parameter;
	parameter["username"] = account;
	parameter["password"] = password;
	parameter["resource"] = App::GetInstance()->m_resource;
	httpPost(url, parameter, runKey, runFunction, errorKey, errorRunFunction);
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
/*{
	"result": true,
	"code": 0,
	"id": null,
	"errorMessage": null,
	"toURL": "/member",
	"data": {
		"memberId": 17167,
		"memberName": "18888702717",
		"memberSex": 2,
		"memberGrade": 0,
		"memberCity": "",
		"memberProvince": "",
		"qq": "",
		"weixin": "",
		"balanceAppleAmount": "0.00"
	}
}*/
//解析函数
void NetIntface::httpLogInCallBack(string json,const function<void(int,int,int ,string ,string)> runable, const function<void(string)> errorRunable)
{
	string errorMessage = "";
	rapidjson::Document doc;
	bool result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		result = YYXLayer::getBoolForJson(false, doc, "result");
		if (result)
		{
			auto memberId = YYXLayer::getIntForJson(-999, doc, "data", "memberId");
			auto memberSex = YYXLayer::getIntForJson(-999, doc, "data", "memberSex");
			auto memberGrade = YYXLayer::getIntForJson(-999, doc, "data", "memberGrade");
			auto memberCity = YYXLayer::getStringForJson("", doc, "data", "memberCity");
			auto memberProvince = YYXLayer::getStringForJson("", doc, "data", "memberProvince");
			if (runable)
				runable(memberId, memberSex, memberGrade, memberCity, memberProvince);
		}
		else
		{
			errorMessage = YYXLayer::getStringForJson("", doc, "errorMessage");
		}
	}
	if(!result)
	{
		if (errorRunable)
			errorRunable(errorMessage);
	}
}

//删除孩子
void NetIntface::httpDeleteChild(int memberId, int childrenId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = std::string(IP).append(NET_DELETECHILD);
	map<string, string>  parameter;
	parameter["memberId"] = StringUtils::format("%d", memberId);
	parameter["childrenId"] = StringUtils::format("%d", childrenId);
	parameter["resource"] = App::GetInstance()->m_resource;
	httpPost(url, parameter, runKey, runFunction, errorKey, errorRunFunction);
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

bool NetIntface::httpDeleteChildCallBack(string json)
{
	rapidjson::Document doc;
	YYXLayer::getJsonObject4Json(doc, json);
	auto code = YYXLayer::getIntForJson(-999, doc, "code");
	if (code == 0)
		return true;
	else
		return false;
}

//添加孩子
void NetIntface::httpAddChild(int memberId, string childrenName, int childrenSex, string childrenBirth, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = std::string(IP).append(NET_ADDCHILD);
	map<string, string>  parameter;
	parameter["memberId"] = StringUtils::format("%d", memberId);
	parameter["childrenName"] = childrenName;
	parameter["childrenSex"] = StringUtils::format("%d", childrenSex);
	parameter["childrenBirth"] = childrenBirth;
	parameter["resource"] = App::GetInstance()->m_resource;
	httpPost(url, parameter, runKey, runFunction, errorKey, errorRunFunction);
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

void NetIntface::httpAddChildCallBack(string json)
{
	rapidjson::Document doc;
	YYXLayer::getJsonObject4Json(doc, json);
	auto code = YYXLayer::getIntForJson(-999, doc, "code");
}


//下载
void NetIntface::DownLoadFile(string url, string dir, string fileName, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect())
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = url + ": DownLoadFile " + dir + "/" + fileName + " error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.txt", (int)YYXLayer::getRandom()),1);
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
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = url + ": DownLoadImage "+ dir +"/"+fileName+" error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.txt", (int)YYXLayer::getRandom()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::DownLoadImage(url.c_str(), dir.c_str(), fileName.c_str(), runKey.c_str(), errorKey.c_str());
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
	App::log("DownLoad = >" + url);
	if (!&url)
	{
		App::log("DownLoad = >paramter is error");
		if (errorRunable)
			errorRunable("");
		return;
	}
	HttpRequest* pRequest = new HttpRequest();
	pRequest->setUrl(url.c_str());
	pRequest->setRequestType(HttpRequest::Type::GET);
	pRequest->setResponseCallback([errorRunable, runable, dir, fileName](HttpClient* client, HttpResponse* response)
	{
		if (!response)
		{
			if (errorRunable)
				errorRunable("");
			App::log("DownLoad = > 404");
			return;
		}
		if (!response->isSucceed())
		{
			if (errorRunable)
				errorRunable("");
			App::log("DownLoad = >404");
			return;
		}
		if (!FileUtils::getInstance()->isDirectoryExist(dir))
		{
			FileUtils::getInstance()->createDirectory(dir);
		}
		auto path = dir + "/" + fileName;
		std::vector<char> *buffer = response->getResponseData();
		auto buf = new char[buffer->size()];
		std::copy(buffer->begin(), buffer->end(), buf);
		App::log("DownLoad = > ", buffer->size());
		FILE *fp = fopen(path.c_str(), "wb+");
		fwrite(buf, 1, buffer->size(), fp);
		fclose(fp);
		if (buf)
			delete[] buf;
		if (runable)
			runable(path);
		App::log("DownLoad = > OK" + path);
	});
	HttpClient::getInstance()->send(pRequest);
	pRequest->release();
}

//获取孩子列表
void NetIntface::httpGetChildDetails(int memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = std::string(IP).append(NET_CHILD_DETAILS);
	map<string, string>  parameter;
	parameter["memberId"] = StringUtils::format("%d", memberID);
	parameter["resource"] = App::GetInstance()->m_resource;
	httpPost(url, parameter, runKey, runFunction, errorKey, errorRunFunction);
}

void NetIntface::WIN32_httpGetChildDetails(int memberID, string runKey, string errorKey)
{
	auto runable = getMapFunction(runKey);
	auto errorRunable = getMapFunction(errorKey);
	string url = std::string(IP).append(NET_CHILD_DETAILS);
	auto parameter = map<string, string>();
	parameter["memberId"] = StringUtils::format("%d", memberID);
	parameter["resource"] = App::GetInstance()->m_resource;
	stringHttpRequest(HttpRequest::Type::POST, url, parameter, 3, [=](string json) {
		if (runable)
			runable(json);
	}, [=]() {
		if (errorRunable)
			errorRunable("");
	});
}

void NetIntface::httpGetChildDetailsCallBack(string json, const std::function<void(int, int, int, string, string, string, long long)> runable, const std::function<void(int)> resultRunBegin)
{
	rapidjson::Document doc;
	YYXLayer::getJsonObject4Json(doc, json);
	auto code = YYXLayer::getIntForJson(-999, doc, "code");
	if (code == 0)
	{
		if (resultRunBegin)
		{
			resultRunBegin(0);
		}
		rapidjson::Value arrayData;
		YYXLayer::getJsonArray4Json(arrayData, doc, "data");
		for (rapidjson::SizeType i = 0; i < arrayData.Size(); i++)
		{
			rapidjson::Value & item = arrayData[i];
			auto childrenId = YYXLayer::getInt4Json(-999, item, "childrenId");
			auto childrenSex = YYXLayer::getInt4Json(-999, item, "childrenSex");
			auto childrenName = YYXLayer::getString4Json("", item, "childrenName");
			auto childrenBirth = YYXLayer::getString4Json("", item, "childrenBirth");
			auto url = YYXLayer::getString4Json("", item, "avatarUrl");
			auto uptime = App::getCurrentTime();
			if (runable)
				runable(i, childrenId, childrenSex, childrenName, childrenBirth, url, uptime);
		}
		if (resultRunBegin)
			resultRunBegin(1);
	}
	else
	{
		if (resultRunBegin)
			resultRunBegin(2);
	}
}

//获取第一个孩子的头像
void NetIntface::httpGetFirstChildHeadPortrait(int memberID,  string runKey, function<void(string)> runFunction, string errorKey,function<void(string)> errorRunFunction)
{
	string url = string(IP).append(NET_GETFIRSTCHILDHEADPORTRAIT).append("?memberId=").append(StringUtils::format("%d", memberID));
	NetIntface::httpGet(url, runKey, runFunction, errorKey, errorRunFunction);
}

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

//解析函数
void NetIntface::httpGetFirstChildHeadPortraitCallBack(string json,function<void(int , string)> runable,function<void()> errorRunable)
{
	rapidjson::Document doc;
	bool result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		result = YYXLayer::getBoolForJson("", doc, "result");
		if (result)
		{
			int id = YYXLayer::getIntForJson(-999, doc, "id");
			string toURL = YYXLayer::getStringForJson("", doc, "toURL");
			if (runable)
				runable(id,toURL);
		}
	}
	if (!result)
	{
		if (errorRunable)
			errorRunable();
	}
}

//获取书籍详情
void NetIntface::httpGetBookInfo(int bookInfoID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	auto url = std::string(IP).append(NET_BOOKINFO).append(StringUtils::format("%d", bookInfoID))
		.append("?memberId=").append(App::getMemberID()).append("&resource=").append(App::GetInstance()->m_resource);
	NetIntface::httpGet(url, runKey, runFunction, errorKey, errorRunFunction);
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
/*{"desc":"success","data":{"bookId":347,"bookAuthor":"霍君尧","bookPage":16,"bookSize":"22.24M","bookPress":"郑州大学出版社","bookAge":-3,"bookIntroduction":"丑小鸭一出生就与别的兄弟姐妹不一样，它们都嫌弃他，不愿意和他玩玩。丑小鸭伤心地离开了家。他后来都遭遇了哪些人、哪些事呢？他能找到让自己变“漂亮”的办法吗？让我们一起看看这本书，一探究竟吧！","avgScore":5.0,"bookPrice":1.00,"bookmarketPrice":8.00,"bookName":"丑小鸭","orderId":0,"bookListenUrl":null,"bookReadUrl":null,"bookPlayUrl":"http://book.ellabook.cn/08c463ba5bf24d19a6c6e378890b6ff1?Expires=1475896136&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=6BmjmzCltO9eGsfDt9hoogJLMxw%3D","bookMakeUrl":null,"bookCoverUrl":"http://book.ellabook.cn/3ed744e2cbb14f3e9933dff5b6e9c35a?Expires=1475896136&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=LL%2BxiGmKt2JaDDGTYniCaA4SsGI%3D","bookViewUrl":"http://book.ellabook.cn/36bee3ec9d80402ea7fc9ce20b111d65?Expires=1475896136&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=BNcAzS80PhczpujxXA6KblJ%2BCj0%3D","goodsState":1,"goodsVerify":1,"osskeyList":{},"ossbucket":"ellabook-book","osskey":"08c463ba5bf24d19a6c6e378890b6ff1","iosPriceId":"com.diandu.ellabook.bookprice_347_3","bookDownloadSize":23319420,"pressId":47,"gcId":1109,"remainTime":-14727871},"code":0}*/
void NetIntface::httpGetBookInfoCallBack(string json, const function<void(bool,int, int, int, int, int, string, string, string, string, string, string, string, string)> runable,  const function<void()> errorRunable)
{
	rapidjson::Document doc;
	auto result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		auto code = YYXLayer::getIntForJson(1, doc, "code");
		if (code == 0)
		{
			bool isvip = YYXLayer::getBoolForJson(false, doc, "isVip");
			auto bookId = YYXLayer::getIntForJson(-999, doc ,"data","bookId");
			auto bookAuthor = YYXLayer::getStringForJson("", doc, "data", "bookAuthor");
			auto bookPage = YYXLayer::getIntForJson(-999, doc, "data", "bookPage");
			auto bookSize = YYXLayer::getStringForJson("", doc, "data", "bookSize");
			auto bookPress = YYXLayer::getStringForJson("", doc, "data", "bookPress");
			auto bookIntroduction = YYXLayer::getStringForJson("", doc, "data", "bookIntroduction");
			int bookPrice100 = YYXLayer::getDoubleForJson(-999, doc, "data", "bookPrice")*100;
			if (bookPrice100 == -99900)
				bookPrice100 = YYXLayer::getIntForJson(-999, doc, "data", "bookPrice") * 100;
			int bookmarketPrice100 = YYXLayer::getDoubleForJson(-999, doc, "data", "bookmarketPrice") * 100;
			auto bookName = YYXLayer::getStringForJson("", doc, "data", "bookName");
			auto bookPlayUrl = YYXLayer::getStringForJson("", doc, "data", "bookPlayUrl");
			auto bookCoverUrl = YYXLayer::getStringForJson("", doc, "data", "bookCoverUrl");
			auto bookViewUrl = YYXLayer::getStringForJson("", doc, "data", "bookViewUrl");
			auto remainTime = YYXLayer::getIntForJson(-999, doc, "data", "remainTime");
			if (runable)
			{
				runable(isvip, bookId, bookPage, bookPrice100, bookmarketPrice100, remainTime, bookAuthor, bookSize, bookPress, bookIntroduction, bookName, bookPlayUrl, bookCoverUrl, bookViewUrl);
			}
		}
		else
		{
			if (errorRunable)
				errorRunable();
		}
	}
}

/*{
	"desc": "success",
		"data" : {
		"bookId": 329,
			"bookAuthor" : "刘晓博",
			"bookPage" : 15,
			"bookSize" : "26.81M",
			"bookPress" : "郑州大学出版社",
			"bookAge" : -3,
			"bookIntroduction" : "冬天，有一个穿着补丁衣服的卖火柴的小女孩，在寒风中瑟瑟发抖，她又冷又饿，只能划着火柴来取暖。微弱的光亮下，小女孩仿佛看到了希望。她到底看到了什么？等待她的命运是怎样的呢？让我们一起来读一读这个故事吧。",
			"avgScore" : 5,
			"bookPrice" : 1,
			"bookmarketPrice" : 8,
			"bookName" : "卖火柴的小女孩",
			"orderId" : 0,
			"bookListenUrl" : null,
			"bookReadUrl" : null,
			"bookPlayUrl" : "http://book.ellabook.cn/1b41b32ef9934ace95139d5a74777063?Expires=1472633779&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=wfWCBdQEdWeONdeUY6PnuHhK4Ks%3D",
			"bookMakeUrl" : null,
			"bookCoverUrl" : "http://book.ellabook.cn/4028e1116b994a9e91dd618ed357a527?Expires=1472633779&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=%2Bgpxt0hom0aGb9V/REbliiO62zg%3D",
			"bookViewUrl" : "http://book.ellabook.cn/a81467bc40d04cd789feca7061f68ee7?Expires=1472633779&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=nd6D18TseVyTQo7xqFos0k1snwQ%3D",
			"goodsState" : 1,
			"goodsVerify" : 1,
			"osskeyList" : {},
			"ossbucket" : "ellabook-book",
			"osskey" : "1b41b32ef9934ace95139d5a74777063",
			"iosPriceId" : "com.diandu.ellabook.bookprice_329_1",
			"bookDownloadSize" : 28111537,
			"pressId" : 47,
			"gcId" : 1109,
			"remainTime" : -11465780
	},
		"code": 0
}
*/
//分享
void NetIntface::share(string filePath, string bookName, string targetUrl, string headUrl, string title, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	if (!IsNetConnect())
		return;
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = bookName + ": share " + " error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.txt", (int)YYXLayer::getRandom()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	SimpleAudioEngine::getInstance()->end();
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

//使用红包购书
void NetIntface::httpBuyBookWithRedPacket(int memberId, int couponId, int bookId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = string(IP).append(NET_BUYBOOKWITHREDPACKET);
	map<string, string> paramter;
	paramter["memberId"] = StringUtils::format("%d", memberId);
	paramter["couponId"] = StringUtils::format("%d", couponId);
	paramter["bookId"] = StringUtils::format("%d", bookId);
	paramter["resource"] = App::m_resource;
	paramter["chargePlatformType"] = "0";
	paramter["chargePlatformName"] = "android";
	httpPost(url, paramter, runKey, runFunction, errorKey, errorRunFunction);
}
//红包购书的回调
void NetIntface::httpBuyBookWithRedPacketCallback(string json, function<void(string str)> runable, function<void(string error)> errorRun)
{
	rapidjson::Document doc;
	YYXLayer::getJsonObject4Json(doc, json);
	auto result = YYXLayer::getBoolForJson(false, doc, "result");
	auto code = YYXLayer::getIntForJson(-1, doc, "code");
	auto errorMessage = YYXLayer::getStringForJson("", doc, "errorMessage");
	if (result)
	{
		//购书成功
		if (runable)
		{
			runable("");
		}
	}
	else
	{
		if (errorRun)
		{
			errorRun(errorMessage);
		}
	}
}

void NetIntface::httpBuyBookWithOutRedPacket(int memberId, int bookId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = string(IP).append(NET_RECHARG_BUYBOOK);
	map<string, string> paramter ;
	paramter["bookId"] = StringUtils::format("%d", bookId);
	paramter["memberId"] = StringUtils::format("%d", memberId);
	paramter["resource"] = App::m_resource;
	paramter["chargePlatformName"] = App::m_resource;
	paramter["chargePlatformType"] = "0";
	paramter["goodsType"] = "1";
	paramter["promotionsId"] = "";
	paramter["orderMessage"] ="";
	paramter["payType"] = "2";
	NetIntface::httpPost(url, paramter, runKey, runFunction, errorKey, errorRunFunction);
}

//纯余额购书的回调
void NetIntface::httpBuyBookWithOutRedPacketCallback(string json, function<void()> runable, function<void()> yuebuzu, function<void()> errorRun)
{
	rapidjson::Document doc;
	YYXLayer::getJsonObject4Json(doc, json);
	auto code = YYXLayer::getIntForJson(-1, doc, "code");
	if (code == 0)
	{
		//购书成功
		if (runable)
		{
			runable();
		}
	}
	else if(code == -1)
	{//余额不足
		if (yuebuzu)
		{
			yuebuzu();
		}
	}
	else
	{
		if (errorRun)
		{
			errorRun();
		}
	}
}

//发表文字评论
void NetIntface::httpSendComment(int types, int bookId, int memberId, int score, string orderId, string memberName, string title, string comment, string runKey, function<void(string str)> runFunction, string errorKey, function<void(string  strs)> errorRunFunction)
{
	string url = string(IP).append(NET_SEND_COMMENT);
	map<string, string> paramter;
	paramter["memberId"] = StringUtils::format("%d", memberId);
	paramter["memberName"] = memberName;
	paramter["orderId"] = orderId;
	paramter["bookId"] = StringUtils::format("%d", bookId);
	paramter["type"] = StringUtils::format("%d",types);
	paramter["comment"] = comment;
	paramter["title"] = title;
	paramter["score"] = StringUtils::format("%d", score);
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	paramter["resource"] = App::m_resource;
	#endif
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	paramter["resource"] = "iphone6";
	#endif
	NetIntface::httpPost(url, paramter, runKey, runFunction, errorKey, errorRunFunction);
}

//获取当前书籍的订单情况
void NetIntface::httpGetBookIsBuy(int bookId, int memberId, int orderId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = string(IP).append(NET_ISBUY);
	map<string, string> paramter;
	paramter["bookid"] = StringUtils::format("%d", bookId);
	paramter["memberId"] = StringUtils::format("%d", memberId);
	paramter["resource"] = App::m_resource;
	paramter["orderid"] = StringUtils::format("%d", orderId);
	NetIntface::httpPost(url, paramter, runKey, runFunction, errorKey, errorRunFunction);
}

/*{ {
	"result": true,
	"code": "0",
	"type": -1, 0=买书 1=租书
	"order_id": "23774"
}*/
void NetIntface::httpGetBookIsBuyCallBack(string json, function<void(string order, int types)> runable)
{
	rapidjson::Document doc;
	if (YYXLayer::getJsonObject4Json(doc, json))
	{
		auto result = YYXLayer::getBoolForJson(false, doc, "result");
		if (result)
		{
			auto type = YYXLayer::getIntForJson(-1, doc, "type");
			if (type == 0 || type == 1)
			{
				auto order_id = YYXLayer::getString4Json("", doc, "order_id");
				if (runable)
					runable(order_id, type);
			}
		}
	}
}

//发表语音评论(带界面)
void NetIntface::goToSendRecording(int bookId, int memberId, int types, string membername, string orderid, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	runKey = App::getOnlyKey();
	errorKey = App::getOnlyKey();
	setMapFunction(runKey, [=](string json) {
		runFunction(json);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = membername + ": goToSendRecording " + " error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.txt", (int)YYXLayer::getRandom()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::commentTheRecording(bookId, memberId, types, membername.c_str(), orderid.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Toast::create("windows have not goToSendRecording");
#endif
}

//void NetIntface::WIN32_httpBuyBookWithRedPacket(int memberId, int couponId, int bookId, string runKey, string errorKey)
//{
//	string url = string(IP).append(NET_BUYBOOKWITHREDPACKET);/*.append("?memberId=").append
//		(StringUtils::format("%d", memberId)).append("&couponId=").append(StringUtils::format("%d", couponId)).append
//		("&bookId=").append(StringUtils::format("%d", bookId)).append("&resource=").append(App::m_resource);*/
//	auto paramter = map<string, string>();
//	paramter["memberId"] = StringUtils::format("%d", memberId);
//	paramter["couponId"] = StringUtils::format("%d", couponId);
//	paramter["bookId"] = StringUtils::format("%d", bookId);
//	paramter["resource"] = App::m_resource;
//	paramter["chargePlatformType"] = "0";
//	paramter["chargePlatformName"] = "android";
//	YYXLayer::stringHttpRequest(HttpRequest::Type::POST, url, paramter, 3, [=](string json) {
//		YYXLayer::sendNotify4YYXStruct(runKey, -999, json);
//	}, [=]() {
//		YYXLayer::sendNotify4YYXStruct(errorKey);
//	});
//}

void NetIntface::saveReadRecord(long memberId, long childrenId, long bookId, string readStartTime, string readEndTime, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction)
{
	string url = string(IP).append(NET_SAVE_READHISTORY);
	map<string, string> paramter;
	paramter["memberId"] = StringUtils::format("%d", memberId);
	paramter["childrenId"] = StringUtils::format("%d", childrenId);
	paramter["bookId"] = StringUtils::format("%d", bookId);
	paramter["readStartTime"] = readStartTime;
	paramter["readEndTime"] = readEndTime;
	paramter["resource"] = App::m_resource;
	NetIntface::httpPost(url, paramter, runKey, runFunction, errorKey, errorRunFunction);
}

void NetIntface::saveReadRecordCallBack(string json, const function<void()> runable, const function<void()> errorRunable)
{
	rapidjson::Document doc;
	bool result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		auto code = YYXLayer::getIntForJson(-1, doc, "code");
		if (code == 0)
		{
				if (runable){ 
					runable();
				}
		}
		else
		{
			if (errorRunable)
				errorRunable();
		}
	}
	else
	{
		if (errorRunable)
			errorRunable();
	}
}

//删除评论
void NetIntface::httpDeleteComment(int id, function<void(int commentID)> runable, function<void(string errorStr)> errorRun)
{
	string url = string(IP).append(NET_DELETECOMMENT);
	map<string, string> parameter;
	parameter["gevalId"] = StringUtils::format("%d", id);
	parameter["memberId"] = App::getMemberID();
	parameter["resource"] = App::m_resource;
	NetIntface::httpPost(url, parameter, "httpDeleteCommentSuccess", [=](string json) {
		/*{
		"result": 1,
		"success": "删除评论成功"
		}*/
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			auto result = YYXLayer::getIntForJson(0, doc, "result");
			auto success = YYXLayer::getStringForJson("", doc, "success");
			if (result == 1)
			{
				if (runable)
					runable(id);
			}
			else
			{
				if (errorRun)
					errorRun(success);
			}
		}
	}, "httpDeleteCommentFail", [](string str) {
		Toast::create(App::getString("NETEXCEPTION"));
	});
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
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	setMapFunction(errorKey, [=](string error) {
		errorRunFunction(error);
		string errorstr = url + ": inviteRegister " + " error => " + error;
		App::addErrorLog(errorstr, StringUtils::format("http_%d.txt", (int)YYXLayer::getRandom()),1);
		deleteMapFunction(runKey);
		deleteMapFunction(errorKey);
	});
	if (YYXLayer::getBoolFromXML(MUSIC_KEY))
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	else
		SimpleAudioEngine::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CocosAndroidJni::inviteRegister(memberId, url.c_str(), runKey.c_str(), errorKey.c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Toast::create("windows have not inviteRegister");
#endif
}

void NetIntface::getPhoneModel()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	App::GetInstance()->phoneModel = "android";
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	App::GetInstance()->phoneModel = "win32";
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	App::GetInstance()->phoneModel = "ios";
#endif
}
