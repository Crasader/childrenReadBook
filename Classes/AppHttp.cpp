#include "AppHttp.h"
#include "FileNames.h"
#include "MyBook.h"
#include "BookCache.h"
#include "User.h"
#include "YYXDownloadImages.h"
#include "YYXTime.h"
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
#include <iostream>
#include "CrossPlatform.h"
#include "YYXVisitor.h"
#include "YYXBuyBook.h"
#include "BookCityCache.h"
#include "BookStoreCache.h"
#include "BuyVip.h"
#include "vipNotifyLayer.h"
#include "XZLayer.h"
#include "Charger.h"

USING_NS_CC;
using namespace std;

#define WriteLOG if(App::m_debug ==0 ) YYXLayer::writeFilepp(str, FileUtils::getInstance()->getWritablePath() + "temp/log.txt")

AppHttp* AppHttp::Instance = nullptr;

void AppHttp::addTask(HttpData* data)
{
	m_Mtasks.lock();
	int httpId = data->getTaskId();
	m_Tasks[httpId] = data;
	m_Mtasks.unlock();
}

void AppHttp::delTask(int id)
{
	m_Mtasks.lock();
	m_Tasks.erase(id);
	m_Mtasks.unlock();
}

HttpData* AppHttp::getTask(int id)
{
	m_Mtasks.lock();
	HttpData* httpdata = nullptr;
	auto it = m_Tasks.find(id);
	if (it != m_Tasks.end())
		httpdata = it->second;
	m_Mtasks.unlock();
	return httpdata;
}

void AppHttp::delAllTask()
{
	m_Mtasks.lock();
	m_Tasks.clear();
	m_Mtasks.unlock();
}

int AppHttp::getTaskSize()
{
	int count = 0;
	m_Mtasks.lock();
	count = m_Tasks.size();
	m_Mtasks.unlock();
	return count;
}

void AppHttp::pushHighList(int taskID)
{
	m_MhighList.lock();
	highList.push_back(taskID);
	m_MhighList.unlock();
}

int AppHttp::popHighList()
{
	int taskid = 0;
	m_MhighList.lock();
	taskid = highList.back();
	highList.pop_back();
	m_MhighList.unlock();
	return taskid;
}

int AppHttp::getHighListSize()
{
	int count = 0;
	m_MhighList.lock();
	count = highList.size();
	m_MhighList.unlock();
	return count;
}

void AppHttp::pushNormalList(int taskID)
{
	m_MnormalList.lock();
	normalList.push_back(taskID);
	m_MnormalList.unlock();
}

int AppHttp::popNormalList()
{
	int taskid = 0;
	m_MnormalList.lock();
	taskid = normalList.back();
	normalList.pop_back();
	m_MnormalList.unlock();
	return taskid;
}

int AppHttp::getNormalListSize()
{
	int count = 0;
	m_MnormalList.lock();
	count = normalList.size();
	m_MnormalList.unlock();
	return count;
}

void AppHttp::pushLowList(int taskID)
{
	m_MlowList.lock();
	lowList.push_back(taskID);
	m_MlowList.unlock();
}

int AppHttp::popLowList()
{
	int id = 0;
	m_MlowList.lock();
	id = lowList.back();
	lowList.pop_back();
	m_MlowList.unlock();
	return id;
}

int AppHttp::getLowListSize()
{
	int count = 0;
	m_MlowList.lock();
	count = lowList.size();
	m_MlowList.unlock();
	return count;
}

void AppHttp::addHttpList(int id)
{
	m_MhttpList.lock();
	httpList[id] = YYXTime::getInstance()->getNowTime4S();
	m_MhttpList.unlock();
}

void AppHttp::delHttpList(int id)
{
	m_MhttpList.lock();
	auto it = httpList.find(id);
	if(it != httpList.end())
		httpList.erase(id);
	m_MhttpList.unlock();
}

int AppHttp::getHttpListSize()
{
	int count = 0;
	m_MhttpList.lock();
	count = httpList.size();
	m_MhttpList.unlock();
	return count;
}

void AppHttp::httpListControl()
{
	vector<int> outTimeData;

	if (httpList.size() > 0)
	{
		for (auto it : httpList)
		{
			int id = it.first;
			long long startTime = it.second;
			auto time = YYXTime::getInstance()->getNowTime4S() - startTime;
			if (time > outTime)
			{
				outTimeData.push_back(id);
				App::log("\nid = < " + Value(id).asString() + " > AppHttp ==> deleteTask outTime ", time);
			}
		}
		for (auto it : outTimeData)
		{
			httpList.erase(it);
		}
	}
	if (httpList.size() < m_concurrence)
	{
		bool full = pushDownloadListFromHighList();
		if (!full)
		{
			bool isFull = pushDownloadListFromNormalList();
			if (!isFull)
			{
				pushDownloadListFromLowList();
			}
		}
	}
	int taskSize = getTaskSize();
	if (taskSize > maxTask)
		clearAllReadyTask();
	if (taskSize > 0)
		setRuning(true);
	else
		setRuning(false);
	if (taskSize > 0)
	{
		App::log("\nAppHttp::httpListControl() =========================>>> m_Tasks.size() =", taskSize);
		App::log("\nAppHttp::httpListControl() =========================>>> httpList.size() =", httpList.size());
		App::log("\nAppHttp::httpListControl() =========================>>> outTime =", outTime);
		App::log("\nAppHttp::httpListControl() =========================>>>  m_concurrence(bingfa)=", m_concurrence);
	}
	if(outTimeData.size() > 0)
		change4OutTime();
	App::log("--BBBBBBBBBBBBBBBBB");
}

bool AppHttp::pushDownloadListFromHighList()
{
	int count = m_concurrence - getHttpListSize();
	for (int i = 0; i < count; i++)
	{
		if (getHighListSize() > 0)
		{
			int id = popHighList();
			auto data = getTask(id);
			addHttpList(id);
			http(data);
		}
	}
	if (getHttpListSize() < m_concurrence)
		return false;
	else
		return true;
}

bool AppHttp::pushDownloadListFromNormalList()
{
	int count = m_concurrence - getHttpListSize();
	for (int i = 0; i < count; i++)
	{
		if (getNormalListSize() > 0)
		{
			int id = popNormalList();
			auto data = getTask(id);
			addHttpList(id);
			http(data);
		}
	}
	if (getHttpListSize() < m_concurrence)
		return false;
	else
		return true;
}

void AppHttp::pushDownloadListFromLowList()
{
	int count = m_concurrence - getHttpListSize();
	for (int i = 0; i < count; i++)
	{
		if (getLowListSize() > 0)
		{
			int id = popLowList();
			auto data = getTask(id);
			addHttpList(id);
			http(data);
		}
	}
}

void AppHttp::addMaxTask()
{
	m_concurrence += 1;
	if (m_concurrence > 1)
	{
		m_concurrence = 1;
	}
}

void AppHttp::decreaseMaxTask()
{
	m_concurrence = 1;
}

void AppHttp::addOutTime()
{
	outTime = outTime +5;
	if (outTime > 60)
	{
		outTime = 60;
	}
	else if (outTime <= 3)//减小到非常小的时候 突然增加时间 恢复初始值
	{
		outTime = 10;
	}
}

void AppHttp::decreaseOutTime()
{
	outTime = outTime / 2.0;
	if (outTime <= 1)
	{
		outTime = 2;
	}
	else if (outTime >50)//增大到非常大的时候 突然正常下载了 恢复初始值
	{
		outTime = 10;
	}
}

void AppHttp::change4OutTime()
{
	if (getTaskSize() > 10)
	{
		clearAllReadyTask();
		YYXLayer::controlTouchTime(60, "change4OutTime", []() {
			Toast::create(App::getString("YINGXINQUANWU"), false);
		});
	}
}

void AppHttp::change4GoodTime()
{
	//addMaxTask();
	//decreaseOutTime();
}

int AppHttp::createId()
{
	return ++m_endId;
}

HttpCallBack AppHttp::getMycallback(string key)
{
	m_Mmycallback.lock();
	HttpCallBack call = nullptr;
	auto it = m_mycallback.find(key);
	if (it != m_mycallback.end())
	{
		call = it->second;
	}
	m_Mmycallback.unlock();
	return call;
}

void AppHttp::addMycallback(string key, const HttpCallBack& callback1)
{
	m_Mmycallback.lock();
	m_mycallback[key] = callback1;
	m_Mmycallback.unlock();
}

void AppHttp::deleteMycallback(string key)
{
	m_Mmycallback.lock();
	m_mycallback.erase(key);
	m_Mmycallback.unlock();
}

void AppHttp::clearMycallback()
{
	m_Mmycallback.lock();
	m_mycallback.clear();
	m_Mmycallback.unlock();
}

void AppHttp::http(HttpData *data)
{
	if (!controlRun)
		return;
	if (data == nullptr || data->getUrl().empty())
	{
		App::log("stringHttpRequest URL is error");
		return;
	}
	string str = "\n( send ) id = < " + Value(data->getTaskId()).asString() + " >   @@@@@@@@@@ " + data->getUrl() + "\n";
	App::log(str);
	WriteLOG;
	string url = data->getUrl();
	HttpRequest::Type type = data->getHttpType();
	map<string, string> paramter = data->getParamter();
	bool sendImmediate = data->getSendImmediate();

	HttpRequest* request = new HttpRequest();
	request->setRequestType(type);
	string postData;
	request->setUrl(url.c_str());
	if (type == HttpRequest::Type::POST)
	{
		if (paramter.size() > 0)
		{
			for (auto it : paramter)
			{
				auto key = it.first;
				auto value = it.second;
				postData.append(StringUtils::format("&%s=%s", key.c_str(), value.c_str()));
			}
			postData.erase(0, 1);
			string str = "\n( send ) id = < " + Value(data->getTaskId()).asString() + " > postData = < " + postData + " >  @@@@@ \n";
			App::log(str);
			WriteLOG;
			request->setRequestData(postData.c_str(), postData.length());
		}
	}
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {

		function<void(HttpData *)> call1 = [](HttpData *_data) {
			if (_data == nullptr)
				return;
			auto id = _data->getTaskId();
			if (id <= 0 || id > 100000)
				return;
			auto nowtime = YYXTime::getInstance()->getNowTime4S();
			auto httptime = nowtime - _data->getStartTime();
			if (App::m_debug == 0) {
				string str = "\n( send OK ) id = < " + Value(id).asString() + " >  Time = < " + StringUtils::format("%lld", httptime) + " > @@@@@@@@@@ " + _data->getJson() + "\n";
				App::log(str);
				WriteLOG;
			}
			if (httptime < AppHttp::getInstance()->outTime)
			{
				AppHttp::getInstance()->change4GoodTime();
			}
			function<void(HttpData *)> callback2 = _data->getCallback();
			if (callback2)
				callback2(_data);
			AppHttp::getInstance()->deleteTask(id);
			App::GetInstance()->addMessage();
			if (App::m_debug == 0) {
				string str = "\n( send OK ) id = < " + Value(id).asString() + " >  CALLBACK END  @@@@@@@@@@ ";
				App::log(str);
				WriteLOG;
			}
		};
		function<void(HttpData *)> call2 = [](HttpData *_data) {
			if (_data == nullptr)
				return;
			auto id = _data->getTaskId();
			if (id <= 0 || id >100000)
				return;
			auto nowtime = YYXTime::getInstance()->getNowTime4S();
			auto httptime = nowtime - _data->getStartTime();
			if (App::m_debug == 0) {
				string str = "\n( ERROR ) id = < " + Value(id).asString() + " > Time = < " + StringUtils::format("%lld", httptime) + " >  @@@@@@@@@@ " + "\n";
				App::log(str);
				WriteLOG;
			}
			function<void(HttpData*)> callbackerror2 = _data->getCallbackerror();
			if (callbackerror2)
				callbackerror2(_data);
			AppHttp::getInstance()->deleteTask(id);
			App::GetInstance()->addMessage();
			if (App::m_debug == 0) {
				string str = "\n( ERROR) id = < " + Value(id).asString() + " >  CALLBACKERROR END  @@@@@@@@@@ ";
				App::log(str);
				WriteLOG;
			}
		};

		if (!response || !response->isSucceed())
		{
			if (call2)
				call2(data);
			return;
		}
		auto dot = response->getResponseData();
		if (dot->empty())
		{
			if (call1)
				call1(data);
		}
		else
		{
			std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
			if (data)
				data->setJson(str);
			else
				App::log(" ( error ) http callback HttpData == nullptr");
			if (call1)
				call1(data);
		}
	});
	if (sendImmediate)
		cocos2d::network::HttpClient::getInstance()->sendImmediate(request);
	else
		cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
	data->setStartTime(YYXTime::getInstance()->getNowTime4S());
}

AppHttp::AppHttp()
{
}

AppHttp::~AppHttp()
{
}

AppHttp * AppHttp::getInstance()
{
	if (Instance == nullptr)
	{
		Instance = new AppHttp();
	}
	cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(60);
	return Instance;
}

std::string AppHttp::createSign(map<string, string> p)
{
	return "123";
}

map<string, string> AppHttp::signPostData(string methodName,map<string, string> pContent,string version)
{
	string content = YYXLayer::getStringFormMap(pContent);
	map<string, string> parameter;
	parameter["method"] = methodName;
	parameter["format"] = "json";
	parameter["app_key"] = m_appKey;
	parameter["v"] = version;
	parameter["sign_method"] = "md5";
	parameter["timestamp"] = YYXTime::getInstance()->getNowTimeString();
	parameter["content"] = content;
	parameter["sign"] = createSign(parameter);
	return parameter;
}

void AppHttp::deleteTask(int taskTag, bool del)
{
	if (del)
	{
		auto data = getTask(taskTag);
		HttpData::del(data);
	}
	delHttpList(taskTag);
	delTask(taskTag);
	App::log(" ( delete ) AppHttp::deleteTask " , taskTag);
}

void AppHttp::clearAllReadyTask()
{
	vector<int> dels;
	for (int i = 0; i < getNormalListSize(); i++)
	{
		int id = popNormalList();
		auto normalData = getTask(id);
		HttpData::del(normalData);
		delTask(id);
	}
	for (int i = 0; i < getLowListSize(); i++)
	{
		int id = popLowList();
		auto lowData = getTask(id);
		HttpData::del(lowData);
		delTask(id);
	}
	string str = "\n( clearAllReadyHTTP ) highList = < " + Value(getHighListSize()).asString() + " > @@@@@ \n";
	str += "\n( clearAllReadyHTTP ) normalList = < " + Value(getNormalListSize()).asString() + " > @@@@@ \n";
	str += "\n( clearAllReadyHTTP ) lowList = < " + Value(getLowListSize()).asString() + " > @@@@@ \n";
	str += "AppHttp::httpListControl() =========================>>> m_Tasks.size() ="+ Value(getTaskSize()).asString()+ "  @@@@@";
	App::log(str);
	WriteLOG;
}

void AppHttp::newHttp(HttpData* data)
{
	thread([=]() {
		if (data == nullptr)
			return;
		if (getTaskSize() > maxTask)
		{
			auto call = data->getCallbackerror();
			if (call)
				call(data);
			HttpData::del(data);
			App::GetInstance()->addMessage();
			return;//累计的网络请求太多 不再接网络请求
		}
		int httpId = data->getTaskId();
		addTask(data);
		switch (data->getPriority())
		{
		case _high:
			if (httpList.size() <= m_MaxThread)
			{
				http(data);
				httpList[data->getTaskId()] = YYXTime::getInstance()->getNowTime4S();
			}
			else
				pushHighList(httpId);
			break;
		case _normal:
			pushNormalList(httpId);
			break;
		case _low:
			pushLowList(httpId);
			break;
		}
		App::GetInstance()->addMessage();
	}).detach();
}

HttpData::HttpData()
{
	m_taskId = AppHttp::getInstance()->createId();
}

HttpData::~HttpData()
{
}

HttpData* HttpData::create()
{
	HttpData* book = nullptr;
	book = new HttpData();
	return book;
}

void HttpData::del(HttpData* book)
{
	if (book)
		delete book;
	book = nullptr;
}

string HttpData::toString()
{
	return string("m_taskId = ")+Value(m_taskId).asString()+"\nm_url = "+ m_url +"\nm_startTime = "+StringUtils::format("%lld", m_startTime)
		+"\nm_priority = "+( (m_priority == _high)?"_high":"_normal") ;
}

HttpCallBack HttpData::getCallback()
{
	if (&m_callbackKey && !m_callbackKey.empty())
	{
		auto call = AppHttp::getInstance()->getMycallback(m_callbackKey);
		return call;
	}
	else
		return nullptr;
}

void HttpData::setCallback(const HttpCallBack& val)
{
	if (!m_callbackKey.empty())
		AppHttp::getInstance()->deleteMycallback(m_callbackKey);
	m_callbackKey = StringUtils::format("%lld", YYXTime::getInstance()->getRandom());
	AppHttp::getInstance()->addMycallback(m_callbackKey, val);
}

HttpCallBack HttpData::getCallbackerror()
{
	if (&m_callbackerrorKey && !m_callbackerrorKey.empty())
	{
		auto call = AppHttp::getInstance()->getMycallback(m_callbackerrorKey);
		return call;
	}
	else
		return nullptr;
}

void HttpData::setCallbackerror(const HttpCallBack& val)
{
	if (!m_callbackerrorKey.empty())
		AppHttp::getInstance()->deleteMycallback(m_callbackerrorKey);
	m_callbackerrorKey = StringUtils::format("%lld", YYXTime::getInstance()->getRandom());
	AppHttp::getInstance()->addMycallback(m_callbackerrorKey, val);
}

void AppHttp::httpVipAndBuyBooks()
{
	string url = string(IP) + NET_USERBUYANDRENTBOOKS + "?memberId=" + StringUtils::format("%d", App::getMemberId()) + "&resource=" + App::m_resource;
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setPriority(HttpPriority::_high);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			App::GetInstance()->myBookURLMap.clear();
			App::GetInstance()->VIPbook.clear();
			App::GetInstance()->myBuyBook.clear();
			MyBook::getInstance()->clearBook();
			rapidjson::Value array;
			YYXLayer::getJsonArray4Json(array, doc, "data");
			YYXLayer::getDataForJsonArray(array, [](rapidjson::Value& item, int idx) {
				auto bookId = String(YYXLayer::getString4Json("0", item, "bookId")).intValue();
				auto bookName = YYXLayer::getString4Json("", item, "bookName");
				auto bookPlayUrl = YYXLayer::getString4Json("", item, "bookPlayUrl");
				auto bookCoverUrl = YYXLayer::getString4Json("", item, "bookCoverUrl");
				auto bookRelationState = String(YYXLayer::getString4Json("1", item, "bookRelationState")).intValue();//1代表买书 2代表租书
				auto isNewEllaBook = String(YYXLayer::getString4Json("0", item, "isNewEllaBook")).intValue();//是否新书 0不是 1是
				auto orderId = String(YYXLayer::getString4Json("0", item, "orderId")).intValue();
				auto time = String(YYXLayer::getString4Json("0", item, "time")).intValue();
				MyBook::getInstance()->addBook(bookId, time, false);
				App::GetInstance()->myBookURLMap[bookId] = bookPlayUrl;
				if (bookRelationState == 1)
				{
					App::GetInstance()->myBuyBook[bookId] = time;
				}
				else
				{
					App::GetInstance()->VIPbook[bookId] = time;
				}
				BookCache::getInstance()->addBook(Book::create()->setCoverURL(bookCoverUrl)->setBookId(bookId));
			});
			thread mythread([]() {
				MyBook::getInstance()->writeDownXml();
				map<string, string> buybooks;
				for (auto it : App::GetInstance()->myBuyBook)
				{
					string key = Value( it.first).asString();
					buybooks[key] = Value(it.second).asString();
				}
				string json_buybooks = YYXLayer::getStringFormMap(buybooks);
				string filename_buybooks = StringUtils::format("buyBook/buybook_%d.json", App::GetInstance()->getMemberId());
				YYXLayer::writeFile(json_buybooks, FileUtils::getInstance()->getWritablePath() + filename_buybooks);

				map<string, string> VIPbooks;
				for (auto it : App::GetInstance()->VIPbook)
				{
					string key = Value(it.first).asString();
					VIPbooks[key] = Value(it.second).asString();
				}
				string json_VIPbooks = YYXLayer::getStringFormMap(VIPbooks);
				string filename_VIPbooks = StringUtils::format("vipBook/vipbook_%d.json", App::GetInstance()->getMemberId());
				YYXLayer::writeFile(json_VIPbooks, FileUtils::getInstance()->getWritablePath() + filename_VIPbooks);
			});
			mythread.detach();
			YYXLayer::sendNotify(TAG_BOOKROOMBOOKISEXIT);
		}
	});
	newHttp(httpData);
}

void AppHttp::httpChildren()
{
	string url = std::string(IP).append(NET_CHILD_DETAILS);
	map<string, string>  parameter;
	int httpMemberId = User::getInstance()->getMemberId();
	string memerstr = StringUtils::format("%d", httpMemberId);
	YYXLayer::setFileValue("httpChildren_MemberId", memerstr);//切换账号 判断请求的时候 和回调的时候 是不是一个用户
	parameter["memberId"] = memerstr;
	parameter["resource"] = App::m_resource;

	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		auto resulut = YYXLayer::getJsonObject4Json(doc, json);
		if (!resulut)
			return;
		auto code = YYXLayer::getIntForJson(-999, doc, "code");
		if (code == 0)
		{
			rapidjson::Value arrayData;
			YYXLayer::getJsonArray4Json(arrayData, doc, "data");
			auto memberid = YYXLayer::getFileValue("httpChildren_MemberId", "-999");
			if (User::getInstance()->getMemberId() != Value(memberid).asInt())
				return;
			for (rapidjson::SizeType i = 0; i < arrayData.Size(); i++)
			{
				rapidjson::Value & item = arrayData[i];
				auto childrenId = YYXLayer::getInt4Json(-999, item, "childrenId");
				auto childrenSex = YYXLayer::getInt4Json(-999, item, "childrenSex");
				auto childrenName = YYXLayer::getString4Json("", item, "childrenName");
				auto childrenBirth = YYXLayer::getString4Json("", item, "childrenBirth");
				auto url = YYXLayer::getString4Json("", item, "avatarUrl");
				auto child = ChildInfo::create();
				child->setChildrenId(childrenId);
				child->setChildrenSex(childrenSex);
				child->setChildrenName(childrenName);
				child->setChildrenBirth(childrenBirth);
				child->setUrl(url);
				User::getInstance()->addChild(child);
				//头像更新
					YYXDownloadImages::GetInstance()->newDownloadImage(url, child->getDir(), 
						YYXTime::getInstance()->getRandomFileName()+".png",
						high, 
						0, 
						[childrenId](string downpath)
					{
						auto memberid = YYXLayer::getFileValue("httpChildren_MemberId", "-999");
						if (User::getInstance()->getMemberId() != Value(memberid).asInt())
							return;
						auto child = User::getInstance()->getChild(childrenId);
						User::getInstance()->addChild(child);//getchild中可能是new出来的，需要释放，addchild中可以管理或者释放
						string savepath = child->getDir() + "/" + YYXTime::getInstance()->getRandomFileName() + ".png";
						CrossPlatform::cutTheRounded(downpath, savepath, 300, 300, "", [childrenId](string _savepath)
						{
							if (FileUtils::getInstance()->isFileExist(_savepath))
							{
								auto child = User::getInstance()->getChild(childrenId);
								User::getInstance()->addChild(child);
								child->setPath(_savepath);
							}
							YYXLayer::sendNotify(TAG_BABYCENTERSCENECHANGECHILDREN);
						}, "", nullptr);
					});
				ChildInfo* child2 = User::getInstance()->getChild(User::getInstance()->getChildId());
				if (child2->getChildrenId() <= 0 && i == 0 )
				{
					User::getInstance()->setChildId(childrenId);
				}
			}
			YYXLayer::sendNotify(TAG_BABYCENTERSCENESHOWCHILD);
			YYXLayer::sendNotify(TAG_BABYCENTERSCENEREFERHEAD);
			YYXLayer::sendNotify(TAG_INDEXSCENEDOWNLOADCHILDHEAD);
			YYXLayer::sendNotify(TAG_REMOVEWAITING);
		}
	});
	newHttp(httpData);
}

void AppHttp::httpAddChild(ChildInfo* child)
{
	string url = std::string(IP).append(NET_ADDCHILD);
	map<string, string>  parameter;
	parameter["memberId"] = App::getMemberID();
	parameter["childrenName"] = child->getChildrenName();
	parameter["childrenSex"] = Value(child->getChildrenSex()).asString();
	parameter["childrenBirth"] = child->getChildrenBirth();
	parameter["resource"] = App::m_resource;

	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			auto childrenId = YYXLayer::getIntForJson(-999, doc, "childrenId");
			if (childrenId > 0)
				result = true;
			else
				result = false;
		}
		if (result)
		{
			Toast::create(App::getString("MESAAGEBOX_ADDCHILD_SUCCESS"));
			YYXLayer::sendNotify(TAG_BABYCENTERSCENECHANGECHILDREN);
			AppHttp::getInstance()->httpChildren();
		}
		else
		{
			Toast::create(App::getString("MESAAGEBOX_ADDCHILD_ERROR"));
		}
	});
	httpData->setCallbackerror([=](HttpData *data) {
		Toast::create(App::getString("MESAAGEBOX_ADDCHILD_ERROR"));
	});
	ChildInfo::del(child);
	newHttp(httpData);
}

void AppHttp::httpDeleteChild(int childId)
{
	string url = std::string(IP).append(NET_DELETECHILD);
	map<string, string>  parameter;
	parameter["memberId"] = App::getMemberID();
	parameter["childrenId"] = Value(childId).asString();
	parameter["resource"] = App::m_resource;

	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([=](HttpData * data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			auto code = YYXLayer::getIntForJson(-999, doc, "code");
			if (code == 0)
			{
				User::getInstance()->delChild(childId);
			}
			else
			{
				result = false;
			}
		}
		if (result)
		{
			Toast::create(App::getString("SHANCHUBAOBEICHENGGONG"));
			YYXLayer::sendNotify(TAG_BABYCENTERSCENECHANGECHILDREN);
		}
		else
		{
			Toast::create(App::getString("SHANCHUBAOBEISHIBAI"));
		}
	});
	httpData->setCallbackerror([](HttpData * data) {
		Toast::create(App::getString("SHANCHUBAOBEISHIBAI"));
	});
	newHttp(httpData);
}

void AppHttp::httpAmendBabyInfo(int id, ChildInfo* child)
{
	string url = std::string(IP).append(NET_MODIFY_CHILD_INFO);
	map<string, string> parameter;
	parameter["childrenId"] = Value(id).asString();
	parameter["childrenName"] = child->getChildrenName();
	parameter["childrenSex"] = Value(child->getChildrenSex()).asString();
	parameter["childrenBirth"] = child->getChildrenBirth();
	parameter["resource"] ="iphone6";

	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([=](HttpData * data) {
		string json = data->getJson();
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			auto code = YYXLayer::getIntForJson(-999, doc, "code");
			if (code == 0)
			{
				child->setChildrenId(id);
				child->writeFile();
				auto _child2 = User::getInstance()->getChild(User::getInstance()->getChildId());
				if (_child2)
				{
					_child2->setChildrenName(child->getChildrenName())->setChildrenBirth(child->getChildrenBirth())->setChildrenSex(child->getChildrenSex());
				}
				ChildInfo::del(child);
				YYXLayer::sendNotify(TAG_BABYCENTERSCENEAMENDBABYINFO);
				YYXLayer::sendNotify(TAG_REMOVEWAITING);
				Toast::create(App::getString("XIUGAIXINXICHENGGONG"));
			}
			else {
				YYXLayer::sendNotify(TAG_REMOVEWAITING);
				Toast::create(App::getString("MODIFY_INFO_FAILED"));
				ChildInfo::del(child);
			}
		}
	});
	httpData->setUserData(child);
	httpData->setCallbackerror([=](HttpData* data) {
		Toast::create(App::getString("MODIFY_INFO_FAILED"));
		YYXLayer::sendNotify(TAG_REMOVEWAITING);
		auto _child = (ChildInfo*)data->getUserData();
		ChildInfo::del(_child);
	});
	newHttp(httpData);
}

void AppHttp::httpUploadDownloadRecord(int bookId)
{
	string url = string(IP).append(NET_ADDDOWNLOADRECODE);
	map<string, string> parameter;
	parameter["memberId"] = App::GetInstance()->getMemberID();
	parameter["bookId"] = Value(bookId).asString();
	parameter["resource"] = App::m_resource;
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setHttpType(HttpRequest::Type::POST);
	newHttp(httpData);
}

void AppHttp::httpAppVersion() {
	string url = string(IP).append(NET_ELLAVERSION);
	map<string, string> parameter;
	parameter["versionResource"] = "android";
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setPriority(_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			int code = YYXLayer::getIntForJson(1, doc, "code");
			if (code == 0)
			{
				string versionNum = YYXLayer::getStringForJson("", doc, "data", "versionNum");
				if (versionNum != "")
				{
					App::log("App Version:   " + versionNum);
					auto index1 = versionNum.find_first_of(".");
					auto index2 = versionNum.find_last_of(".");
					int A = atoi(versionNum.substr(0, 1).c_str()) * 10000;
					int B = atoi(versionNum.substr(index1 + 1, 1).c_str()) * 1000;
					int C = atoi(versionNum.substr(index2 + 1).c_str());
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "netVersion", A + B + C, versionNum);
				}
			}
		}
	});
	newHttp(httpData);
}

void AppHttp::httpComments(int bookid, const function<void()>& runFunction)
{
	auto strUrl = std::string(IP).append(NET_BOOKCOMMENT);
	map<string, string > p;
	p["bookId"] = StringUtils::format("%d", bookid);
	p["memberId"] = App::getMemberID();
	p["resource"] = App::m_resource;
	p["page"] = "1";
	p["pageSize"] = "20";
	HttpData* httpData = HttpData::create();
	httpData->setUrl(strUrl);
	httpData->setParamter(p);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setUserValue(Value(bookid));
	httpData->setCallbackerror([=](HttpData* data) {//回调函数存入callbackerror中（callbackerror没有使用到）
		if (runFunction)
			runFunction();
	});
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		int bookid = data->getUserValue().asInt();
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
					int bookid = data->getUserValue().asInt();
					string gevalId = YYXLayer::getString4Json("", item, "gevalId");//评论ID
					string gevalType = YYXLayer::getString4Json("", item, "gevalType");//评论类型 0=文字 1=语音
					string score = YYXLayer::getString4Json("", item, "score");//评星
					string memberName = YYXLayer::getString4Json("", item, "memberName");//用户名
					string gevalState = YYXLayer::getString4Json("", item, "gevalState");//0=只评论文字 1=只打分 2=评论文字并打分
					string memberId = YYXLayer::getString4Json("", item, "memberId");
					string AvatarUrl = YYXLayer::getString4Json("", item, "AvatarUrl");//头像
					string commentTime = YYXLayer::getString4Json("", item, "commentTime");//评论时间
					string title = YYXLayer::getString4Json("", item, "title");//标题
					string content = YYXLayer::getString4Json("", item, "content");//内容
					string url = YYXLayer::getString4Json("", item, "url");//语音下载
					string gevalTime = YYXLayer::getString4Json("", item, "gevalTime");//语音时间
					int id = atoi(gevalId.c_str());//评论ID
					int xingji = atoi(score.c_str());//评星
					time_t icommentTime = atoi(commentTime.c_str());//评论时间
					int memnerid = atoi(memberId.c_str());//用户ID
					int voiceLength = atoi(gevalTime.c_str());//声音时长
					if (!content.empty())
					{//文字评论
						string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);//bookid + 下标顺序
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, idKey, id, "TEXT");//评论id + 类型
						string contentKey = StringUtils::format("comment_gevalId=%d+score+content", id);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, contentKey, xingji, content);
						string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", id);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, timeKey, icommentTime, memberName, (Ref*)memnerid);
						string titleKey = StringUtils::format("comment_gevalId=%d+title", id);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, titleKey, -999, title);
						string AvatarUrlKey = StringUtils::format("comment_gevalId=%d+memberId+AvatarUrl", id);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, AvatarUrlKey, memnerid, AvatarUrl);
					}
					else if (!url.empty())
					{//语音评论
						string dir = FileUtils::getInstance()->getWritablePath() + "voiceComment";
						string filename = gevalId + ".mp3";
						string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);//bookid + 下标顺序
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, idKey, id, "VOICE");//评论id + 类型
						string voiceKey = StringUtils::format("comment_gevalId=%d+score+url+gevalTime", id);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, voiceKey, xingji, url, (Ref*)voiceLength);
						string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", id);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, timeKey, icommentTime, memberName, (Ref*)memnerid);
						string AvatarUrlKey = StringUtils::format("comment_gevalId=%d+memberId+AvatarUrl", id);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, AvatarUrlKey, memnerid, AvatarUrl);
						if (!FileUtils::getInstance()->isFileExist(dir + "/" + filename))
						{
							CrossPlatform::DownLoadFile(url, dir, filename, "", [](string path) {}, "", [](string str) {});
						}
					}
				});
				int count = YYXLayer::getIntForJson(-999, doc, "count");
				string commentCountKey = StringUtils::format("comment_bookid=%d", bookid);//book评论的数量
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, commentCountKey, count);
				auto runFunction = data->getCallbackerror();
				if (runFunction)
					runFunction(data);
			}
			else
				result = false;
		}
	});
	newHttp(httpData);
}

void AppHttp::httpShareHongBao()
{
	auto url = std::string(IP).append(NET_GETSHAREREDPACKET).append(StringUtils::format("?memberId=%d", User::getInstance()->getMemberId())).append("&resource=").append(App::m_resource);
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			int code = YYXLayer::getIntForJson(-999, doc, "code");
			if (code == 0)
			{
				int coupon_amount = YYXLayer::getDoubleForJson(-999, doc, "data", "coupon_amount") * 100;
				XZLayer::safe_ShowShareRedPacket(StringUtils::format("%d", coupon_amount / 100) + App::getString("YUAN"));
			}
		}
	});
	newHttp(httpData);
}

void AppHttp::httpBookStoreSceneCurrentlyPageBookListInfo(int BookStoreId, int pageIndex, string visitFrom)
{
	auto url = std::string(IP).append(NET_BOOKSID).append(StringUtils::format("%d", BookStoreId))
		.append("?page=").append(StringUtils::format("%d", pageIndex+1))
		.append("&pagesize=").append("8")
		.append("&resource=").append(App::m_resource)
		.append("&memberId=").append(App::GetInstance()->getMemberID())
		.append("&visitFrom=").append(visitFrom);
	HttpData* httpData = HttpData::create();
	if (pageIndex == 0)
		httpData->setPriority(_high);
	httpData->setUrl(url);
	httpData->setUserValue(Value(BookStoreId));
	httpData->setUserData((void*)pageIndex);
	httpData->setCallback([](HttpData *data) {
		string json = data->getJson();
		auto BookStoreId = data->getUserValue().asInt();
		auto pageIndex =(int)(data->getUserData());
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			int code = YYXLayer::getIntForJson(-999, doc, "code");
			if (code == 0)
			{
				result = true;
				int totalCount = YYXLayer::getIntForJson(-999, doc, "totalCount");
				BookStoreCache::getInstance()->netCallback(BookStoreId, totalCount);
				rapidjson::Value arrayData;
				YYXLayer::getJsonArray4Json(arrayData, doc, "data");
				StorePage* spData = new StorePage();
				spData->setStoreId(BookStoreId);
				spData->setPageNumber(pageIndex);
				YYXLayer::getDataForJsonArray(arrayData, [=](rapidjson::Value & item, int index) {
					int bookId = YYXLayer::getInt4Json(-999, item, "bookId");
					int hashBuy = YYXLayer::getInt4Json(-999, item, "hashBuy");
					int bookPrice = YYXLayer::getDouble4Json(-999, item, "bookPrice") * 100;
					if (bookPrice == -99900)
						bookPrice = YYXLayer::getInt4Json(-999, item, "bookPrice") * 100;
					int isNewEllaBook = YYXLayer::getInt4Json(-999, item, "isNewEllaBook");
					auto remainTime = YYXLayer::getInt644Json(-999, item, "remainTime");
					int bookmarketPrice = YYXLayer::getDouble4Json(-999, item, "bookmarketPrice") * 100;
					string bookCoverUrl = YYXLayer::getString4Json("", item, "bookCoverUrl");
					string bookPlayUrl = YYXLayer::getString4Json("", item, "bookPlayUrl");
					string bookAuthor = YYXLayer::getString4Json("", item, "bookAuthor");
					string bookPress = YYXLayer::getString4Json("", item, "bookPress");
					string bookName = YYXLayer::getString4Json("", item, "bookName");

					spData->addBook(index, bookId);
					auto bookData = Book::create();
					bookData->setBookId(bookId)->setCoverURL(bookCoverUrl);
					bookData->setBookAuthor(bookAuthor);
					bookData->setBookmarketPrice(bookmarketPrice);
					bookData->setBookPlayUrl(bookPlayUrl);
					bookData->setBookPress(bookPress);
					bookData->setBookPrice(bookPrice);
					bookData->setHashBuy(hashBuy);
					bookData->setName(bookName);
					bookData->setIsNewEllaBook(isNewEllaBook);
					BookCache::getInstance()->addBook(bookData);

					//下载封面
					string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
					string fileName = StringUtils::format("%d", bookId) + ".png";
					auto path = dir + "/" + fileName;
					if (!FileUtils::getInstance()->isFileExist(path) || !App::initImage(path))
					{
						FileUtils::getInstance()->removeFile(path);
						YYXDownloadImages::GetInstance()->newDownloadImage(bookCoverUrl, dir, fileName, high, 0, [](string downPath) {
							auto temp = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreNeedRefresh", -999);
							if (temp > 0)
								temp += 1;
							else
								temp = 1;
							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreNeedRefresh", temp);
						}, [](string str) {
							Toast::create(App::getString("FENGMIANXIAZAISHIBAI"), false);
						});
					}
				});
				BookStoreCache::getInstance()->netCallBack(BookStoreId, pageIndex, spData);
				//解析完毕
				auto temp = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreNeedRefresh", -999);
				if (temp > 0)
					temp += 1;
				else
					temp = 1;
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreNeedRefresh", temp);
				YYXLayer::sendNotify("showPageNumber");
			}
			else
				result = false;
		}
		if (!result)
		{
			//解析错误
			Toast::create(App::getString("SHUJUYICHANG"));
			YYXLayer::sendNotify(TAG_REMOVEWAITING);
		}
	});
	httpData->setCallbackerror([](HttpData *data) {
		//网络连接错误
		Toast::create(App::getString("NETEXCEPTION"));
		YYXLayer::sendNotify(TAG_REMOVEWAITING);
	});
	newHttp(httpData);
}

//获取用户安卓的账户余额
void AppHttp::httpUserBalance(const function<void(int)>& runable)
{
	string url = std::string(IP).append(NET_GETRECHARGE).append("?memberId=").append
		(App::getMemberID()).append("&resource=").append(App::m_resource);
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setCallback([=](HttpData *data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			result = YYXLayer::getBoolForJson(false, doc, "result");
			if (result)
			{
				auto id = YYXLayer::getIntForJson(-999, doc, "data", "member_id");
				int userBalance = YYXLayer::getDoubleForJson(0, doc, "data", "balance_amount") * 100;
				auto uptime = YYXLayer::getInt64ForJson(0, doc, "data", "pe_updatetime");
				if (YYXVisitor::getInstance()->getVisitorMode())
					YYXVisitor::getInstance()->setMoney(userBalance);
				else
					User::getInstance()->setMoney(userBalance);
				if (runable)
					runable(userBalance);
			}
		}
	});
	httpData->setCallbackerror([](HttpData *data) {
		Toast::create(App::getString("NETEXCEPTION"), false);
	});
	newHttp(httpData);
}

//注册
void AppHttp::httpAccountRegiste(string memberName, string memberPasswd, const function<void(string hongbao)>& callback1)
{
	string url = string() + IP + NET_REGISTER + "?memberName=" + memberName
		+ "&memberPasswd=" + memberPasswd + "&resource=" + App::m_resource;
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setPriority(_high);
	httpData->setCallback([=](HttpData *data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		string errorstr = "";
		if (result)
		{
			result = YYXLayer::getBoolForJson(false, doc, "result");
			errorstr = YYXLayer::getStringForJson("", doc, "fail");
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
					Toast::create(App::getString("MESAAGEBOX_REGISTER_SUCCESS"));
					User::getInstance()->setMemberId(memberId);
					User::getInstance()->setUserAccount(memberName);
					User::getInstance()->setSex(memberSex);
					User::getInstance()->setUserCity(memberCity);
					User::getInstance()->setUserProvince(memberProvince);
					AppHttp::getInstance()->httpUserRedPackets();
					if (callback1)
						callback1(couponSum);
					result = true;
				}
			}
		}
		if (!result)
		{
			if (errorstr == "")
				Toast::create(App::getString("ZHUCESHIBAI"));
			else
				Toast::create(errorstr.c_str());
			YYXLayer::sendNotify("LoginScenehttpAccountRegisteOver");
		}
	});
	httpData->setCallbackerror([](HttpData *data) {
		Toast::create(App::getString("NETEXCEPTION"));
		YYXLayer::sendNotify("LoginScenehttpAccountRegisteOver");
	});
	newHttp(httpData);
}

void AppHttp::httpUserRedPackets(YYXCallBack callback1)
{
	string url = string(IP).append(NET_USERREDPACKET).append("?memberId=").append(App::getMemberID());
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setCallback([=](HttpData *data) {
		string json = data->getJson();
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			result = YYXLayer::getBool4Json(false, doc, "result");
			if (result)
			{
				App::GetInstance()->m_redPacket.clear();
				int  expiring_coupon_count = YYXLayer::getIntForJson(0, doc, "expiring_coupon_count");
				rapidjson::Value arraylist;
				if (YYXLayer::getJsonArray4Json(arraylist, doc, "data"))
				{
					YYXLayer::getDataForJsonArray(arraylist, [=](rapidjson::Value & item, int i) {
						int coupon_id = YYXLayer::getInt4Json(-999, item, "coupon_id");
						int coupon_amount100 = YYXLayer::getDouble4Json(-999.00, item, "coupon_amount") * 100;
						string coupon_expire_time = YYXLayer::getString4Json("", item, "coupon_expire_time");
						if (coupon_id != -999 || coupon_amount100 != -99900) {
							map<string, YYXStruct> mapresult;
							YYXStruct::initMapYYXStruct(mapresult, "coupon_id", coupon_id);
							YYXStruct::initMapYYXStruct(mapresult, "coupon_amount", coupon_amount100);
							YYXStruct::initMapYYXStruct(mapresult, "coupon_expire_time", 0, coupon_expire_time);
							App::GetInstance()->m_redPacket.push_back(mapresult);
						}
					});
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "expiring_coupon_count", expiring_coupon_count);
					if (callback1)
						callback1();
				}
			}
		}
	});
	newHttp(httpData);
}

void AppHttp::httpChargerInfo(int key)
{
	string url = string(IP) + NET_GETCHARGEACITIVITY + "?type=2";
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setPriority(HttpPriority::_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setUserValue(Value(key));
	httpData->setCallback([](HttpData *data) {
		string json = data->getJson();
		int key = data->getUserValue().asInt();
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			result = YYXLayer::getBoolForJson(false, doc, "result");
			if (result)
			{
				rapidjson::Value arrayData;
				YYXLayer::getJsonArray4Json(arrayData, doc, "Coupon");
				YYXLayer::getDataForJsonArray(arrayData, [](rapidjson::Value & item, int index) {
					int coupon_amount = YYXLayer::getInt4Json(-999, item, "coupon_amount");
					int coupon_id = YYXLayer::getInt4Json(-999, item, "coupon_id");
					//赠送红包列表
					ChargeData* dat = nullptr;
					dat = Charger::getInstance()->getData(index);
					if (dat == nullptr)
					{
						dat = ChargeData::create();
						dat->setIndex(index);
					}
					dat->setHongbao(coupon_amount*100);
					dat->setHongbaoId(coupon_id);
					Charger::getInstance()->addData(dat);
				});
				YYXLayer::getJsonArray4Json(arrayData, doc, "data");
				YYXLayer::getDataForJsonArray(arrayData, [](rapidjson::Value & item, int index) {
					int real_price = YYXLayer::getInt4Json(-999, item, "real_price");
					int id = YYXLayer::getInt4Json(-999, item, "id");
					//充值金额列表
					ChargeData* dat = nullptr;
					dat = Charger::getInstance()->getData(index);
					if (dat == nullptr)
					{
						dat = ChargeData::create();
						dat->setIndex(index);
					}
					dat->setPayId(id);
					dat->setPrice(real_price * 100);
					Charger::getInstance()->addData(dat);
				});
				YYXLayer::sendNotify("setNodePrice", "", -1);
				if(key >0)
					YYXLayer::sendNotify(TAG_REMOVEWAITING, "", key);
			}
		}
	});
	newHttp(httpData);
}

void AppHttp::httpAmendUserInfo(int sex, string city, string province)
{
	string url = std::string(IP).append(NET_MODIFY_CITY);
	map<string, string> parameter;
	parameter["memberId"] = App::getMemberID();
	parameter["city"] = city;
	parameter["sex"] = Value(sex).asString();
	parameter["province"] = province;
	parameter["resource"] = App::m_resource;
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setPriority(HttpPriority::_high);
	httpData->setParamter(parameter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([=](HttpData *data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			auto code = YYXLayer::getIntForJson(-1, doc, "code");
			if (code == 0)
			{
				result = true;
				//正确结果
				User::getInstance()->setSex(sex);
				User::getInstance()->setUserCity(city);
				User::getInstance()->setUserProvince(province);
				Toast::create(App::getString("XIUGAIXINXICHENGGONG"));
				YYXLayer::sendNotify("MODIFY_USER_INFO_SUCCESS");
				YYXLayer::sendNotify("REFRESH_USER_INFO_OVER");
				YYXLayer::sendNotify(TAG_REMOVEWAITING);
			}
			else
				result = false;
		}
		if (!result)
		{
			Toast::create(App::getString("MODIFY_INFO_FAILED"));
			YYXLayer::sendNotify("MODIFY_USER_INFO_FAILED");
			YYXLayer::sendNotify(TAG_REMOVEWAITING);
		}
	});
	httpData->setCallbackerror([](HttpData *data) {
		Toast::create(App::getString("NETEXCEPTION"));
		YYXLayer::sendNotify("MODIFY_USER_INFO_FAILED");
		YYXLayer::sendNotify(TAG_REMOVEWAITING);
	});
	newHttp(httpData);
}

//登录
void AppHttp::httpLogIn(string& account, string password)
{
	string url = string(IP).append(NET_LOGIN);
	map<string, string> parameter;
	parameter["username"] = account;
	parameter["password"] = password;
	parameter["resource"] = App::m_resource;
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setPriority(HttpPriority::_high);
	httpData->setParamter(parameter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setUserValue(Value(account));
	httpData->setCallback([](HttpData *data) {
		string json = data->getJson();
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
				YYXVisitor::getInstance()->logoutVisitor();
				//登陆成功
				User::getInstance()->setMemberId(memberId);
				string account =data->getUserValue().asString();
				User::getInstance()->setUserAccount(account);
				User::getInstance()->setSex(memberSex);
				User::getInstance()->setUserCity(memberCity);
				User::getInstance()->setUserProvince(memberProvince);
				//大量的用户信息请求
				App::loginCallback();
				YYXLayer::sendNotify("USER_LOGIN_SUCCESS");
			}
			else
			{
				errorMessage = YYXLayer::getStringForJson("", doc, "errorMessage");
			}
		}
		if (!result)
		{
			YYXLayer::sendNotify("USER_LOGIN_FAILED");
			if(!errorMessage.empty())
				Toast::create(errorMessage.c_str());
		}
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("NETEXCEPTION"));
		YYXLayer::sendNotify("USER_LOGIN_FAILED");
	});
	newHttp(httpData);
}

void AppHttp::httpBookInfo(int bookId, const function<void(string )>& callback1)
{
	auto url = std::string(IP).append(NET_BOOKINFO).append(Value(bookId).asString())
		.append("?memberId=").append(App::getMemberID()).append("&resource=").append(App::m_resource);
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			auto code = YYXLayer::getIntForJson(1, doc, "code");
			if (code == 0)
			{
				bool isvip = YYXLayer::getBoolForJson(false, doc, "isVip");
				auto bookId = YYXLayer::getIntForJson(-999, doc, "data", "bookId");
				auto bookAuthor = YYXLayer::getStringForJson("", doc, "data", "bookAuthor");
				auto bookPage = YYXLayer::getIntForJson(-999, doc, "data", "bookPage");
				auto bookSize = YYXLayer::getStringForJson("", doc, "data", "bookSize");
				auto bookPress = YYXLayer::getStringForJson("", doc, "data", "bookPress");
				auto bookIntroduction = YYXLayer::getStringForJson("", doc, "data", "bookIntroduction");
				int bookPrice100 = YYXLayer::getDoubleForJson(-999, doc, "data", "bookPrice") * 100;
				if (bookPrice100 == -99900)
					bookPrice100 = YYXLayer::getIntForJson(-999, doc, "data", "bookPrice") * 100;
				int bookmarketPrice100 = YYXLayer::getDoubleForJson(-999, doc, "data", "bookmarketPrice") * 100;
				auto bookName = YYXLayer::getStringForJson("", doc, "data", "bookName");
				auto bookPlayUrl = YYXLayer::getStringForJson("", doc, "data", "bookPlayUrl");
				auto bookCoverUrl = YYXLayer::getStringForJson("", doc, "data", "bookCoverUrl");
				auto bookViewUrl = YYXLayer::getStringForJson("", doc, "data", "bookViewUrl");
				auto remainTime = YYXLayer::getIntForJson(-999, doc, "data", "remainTime");
				float avgScore = YYXLayer::getDoubleForJson(4, doc, "data", "avgScore");
				//解析正常
				auto bookData = Book::create();
				bookData->setBookId(bookId)->setCoverURL(bookCoverUrl);
				bookData->setBookAuthor(bookAuthor);
				bookData->setBookmarketPrice(bookmarketPrice100);
				bookData->setBookPlayUrl(bookPlayUrl);
				bookData->setBookPress(bookPress);
				bookData->setBookPrice(bookPrice100);
				bookData->setName(bookName);
				bookData->setContent(bookIntroduction);
				bookData->setBookPage(bookPage);
				bookData->setScore(avgScore);
				BookCache::getInstance()->addBook(bookData);
				result = true;
				YYXBuyBook::GetInstance()->setBookPrice( bookPrice100);
				if (callback1)
					callback1(bookName);
			}
		}
		if(!result)
			Toast::create(App::getString("HUOQUSHUJIXIANGQINGYICHANG"), false);
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("HUOQUSHUJIXIANGQINGYICHANG"),false);
	});
	newHttp(httpData);
}

void AppHttp::httpBuyBookWithRedPacket(int couponId, int bookId, const function<void(int)>& callback1)
{
	string url = string(IP).append(NET_BUYBOOKWITHREDPACKET);
	map<string, string> paramter;
	paramter["memberId"] = App::getMemberID();
	paramter["couponId"] = StringUtils::format("%d", couponId);
	paramter["bookId"] = StringUtils::format("%d", bookId);
	paramter["resource"] = App::m_resource;
	paramter["chargePlatformType"] = "0";
	paramter["chargePlatformName"] = "android";
	HttpData* httpData = HttpData::create();
	httpData->setPriority(_high);
	httpData->setUrl(url);
	httpData->setParamter(paramter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setUserData((void*)bookId);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool resu = YYXLayer::getJsonObject4Json(doc, json);
		if (!resu)
			return;
		auto result = YYXLayer::getBoolForJson(false, doc, "result");
		auto code = YYXLayer::getIntForJson(-1, doc, "code");
		auto errorMessage = YYXLayer::getStringForJson("", doc, "errorMessage");
		if (result)
		{			//购书成功
			int _bookId = (int)data->getUserData();
			if (callback1)
				callback1(_bookId);
			Toast::create(App::getString("BUY_SCEESSE"));
		}
		else
		{
			if (errorMessage.empty())
				Toast::create(App::getString("BUY_FAIL"));
			else
				Toast::create(errorMessage.c_str());
		}
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("BUY_FAIL"));
	});
	newHttp(httpData);
}

void AppHttp::httpBuyBookWithOutRedPacket(int bookId, const function<void(int)>& callback1, YYXCallBack callback2)
{
	string url = string(IP).append(NET_RECHARG_BUYBOOK);
	map<string, string> paramter;
	paramter["bookId"] = StringUtils::format("%d", bookId);
	paramter["memberId"] = App::getMemberID();
	paramter["resource"] = App::m_resource;
	paramter["chargePlatformName"] = App::m_resource;
	paramter["chargePlatformType"] = "0";
	paramter["goodsType"] = "1";
	paramter["promotionsId"] = "";
	paramter["orderMessage"] = "";
	paramter["payType"] = "2";
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setPriority(_high);
	httpData->setParamter(paramter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setUserData((void*)bookId);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		if (!result)
			return;
		auto code = YYXLayer::getIntForJson(-1, doc, "code");
		if (code == 0)
		{
			//购书成功
			int _bookId =(int) data->getUserData();
			if (callback1)
				callback1(_bookId);
		}
		else if (code == -1)
		{//余额不足
			if (callback2)
				callback2();
		}
		else
		{
			Toast::create(App::getString("BUY_FAIL"));
		}
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("BUY_FAIL"));
	});
	newHttp(httpData);
}

void AppHttp::httpSendComment(int types, int bookId, int score, string orderId, string comment)
{
	string url = string(IP).append(NET_SEND_COMMENT);
	map<string, string> paramter;
	paramter["memberId"] = App::getMemberID();
	string name = "170****5246";
	if (YYXVisitor::getInstance()->getVisitorMode())
		name = YYXVisitor::getInstance()->getVisitorName();
	else
		name = User::getInstance()->getUserAccount();
	paramter["memberName"] = name;
	paramter["orderId"] = orderId;
	paramter["bookId"] = StringUtils::format("%d", bookId);
	paramter["type"] = StringUtils::format("%d", types);
	paramter["comment"] = comment;
	paramter["title"] = "*****";
	paramter["score"] = StringUtils::format("%d", score);
	paramter["resource"] = "iphone6";
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(paramter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			string desc = App::getString("COMMENT_SUCCESS");
			if (YYXLayer::getBoolForJson(false, doc, "result"))
				YYXLayer::sendNotify(TAG_SENDCOMMENTSUCCESS);
			else
				desc = YYXLayer::getStringForJson(App::getString("COMMENT_FAILED"), doc, "errorMessage");
			Toast::create(desc.c_str());
		}
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("COMMENT_FAILED"));
	});
	newHttp(httpData);
}

//获取当前书籍的订单情况
void AppHttp::httpBookOrderId(int bookId, const function<void(string, int)>& callback1, YYXCallBack callback2)
{
	string url = string(IP).append(NET_ISBUY);
	map<string, string> paramter;
	paramter["bookid"] = StringUtils::format("%d", bookId);
	paramter["memberId"] = App::getMemberID();
	paramter["resource"] = App::m_resource;
	paramter["orderid"] = "-1";
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(paramter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = false;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			result = YYXLayer::getBoolForJson(false, doc, "result");
			if (result)
			{
				auto type = YYXLayer::getIntForJson(-1, doc, "type");
				if (type == 0 || type == 1)
				{
					auto order_id = YYXLayer::getString4Json("", doc, "order_id");
					if (!order_id.empty())
					{
						if (callback1)
							callback1(order_id, type);
						result = true;
					}
				}
				else
					Toast::create(App::getString("COMMENT_TIP_BUY"), false);
			}
		}
		if(!result)
		{
			Toast::create(App::getString("FAXIANNINWEICHENGYUEDU"), false);
		}
	});
	httpData->setCallbackerror([=](HttpData* data) {
		Toast::create(App::getString("COMMENT_FAILED"), false);
		if (callback2)
			callback2();
	});
	newHttp(httpData);
}


void AppHttp::httpUploadReadBookRecord(ReadBookRecord* record, const function<void(ReadBookRecord*)>& callback1, const function<void(ReadBookRecord*)>& callback2)
{
	map<string, string> paramter;
	paramter["memberId"] = StringUtils::format("%d", record->getReadMemberId());
	paramter["childrenId"] = StringUtils::format("%d", record->getReadChildren());
	paramter["bookId"] = StringUtils::format("%d", record->getReadBookId());
	paramter["readStartTime"] = record->getReadStartTime();
	paramter["readEndTime"] = record->getReadEndTime();
	paramter["resource"] = App::m_resource;
	string url = string(IP).append(NET_SAVE_READHISTORY);
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(paramter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			auto code = YYXLayer::getIntForJson(-1, doc, "code");
			if (code == 0)
			{
				//上传成功
				if (callback1)
					callback1(record);
			}
		}
		if (!result) {
			//上传错误
			if (callback2)
				callback2(record);
		}
	});
	httpData->setCallbackerror([=](HttpData* data) {
		//上传失败
		if (callback2)
			callback2(record);
	});
	newHttp(httpData);
}

//删除评论
void AppHttp::httpDeleteComment(int id, YYXCallBack callback1)
{
	string url = string(IP).append(NET_DELETECOMMENT);
	map<string, string> parameter;
	parameter["gevalId"] = StringUtils::format("%d", id);
	parameter["memberId"] = App::getMemberID();
	parameter["resource"] = App::m_resource;

	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setPriority(_high);
	httpData->setParamter(parameter);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			auto result = YYXLayer::getIntForJson(0, doc, "result");
			auto success = YYXLayer::getStringForJson("", doc, "success");
			if (result == 1)
			{
				if (callback1)
					callback1();
			}
			else
				Toast::create(App::getString("SHANCHUPINGLUNSHIBAI"));
		}
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("SHANCHUPINGLUNSHIBAI"));
	});
	newHttp(httpData);
}

//获取包年服务信息
void AppHttp::httpCheckVIP(YYXCallBack callback1)
{
	string url = string(IP).append(NET_CHECHVIP);
	map<string, string> par;
	par["memberId"] = App::getMemberID();
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(par);
	httpData->setPriority(_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			if (YYXLayer::getBoolForJson(false, doc, "result"))
			{
				if (YYXLayer::getBoolForJson(false, doc, "isVip"))
				{
					auto startTime = YYXLayer::getInt64ForJson(0, doc, "startTime");
					auto expireTime = YYXLayer::getInt64ForJson(0, doc, "expireTime");
					auto times = YYXLayer::getInt64ForJson(0, doc, "times");
					User::getInstance()->setVip(true);
					User::getInstance()->setStartTime(YYXTime::getStringFormLongLong(startTime));
					User::getInstance()->setEndTime(YYXTime::getStringFormLongLong(expireTime));
					User::getInstance()->setVipTime(times);					
					if (callback1)
						callback1();
				}
				else
				{
					User::getInstance()->setVip(false);
					App::GetInstance()->VIPbook.clear();
				}
			}
		}
	});
	newHttp(httpData);
}

void AppHttp::httpThe_relationship_between_human_and_books(int bookId, const function<void(int)>& runable)
{
	string url = string(IP) + NET_BOOKBETWEENUSER + "?memberId=" + StringUtils::format("%d&bookId=%d&resource=", App::getMemberId(), bookId) + App::m_resource;
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setPriority(_high);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		if (!result)
			return;
		if (YYXLayer::getBoolForJson(false, doc, "result"))
		{
			auto code = YYXLayer::getIntForJson(0, doc, "code");
			if (runable)
				runable(code);
		}
		else
			Toast::create(App::getString("SHUJIZHUANGTAIHUOQUSHIBAI"));
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("SHUJIZHUANGTAIHUOQUSHIBAI"));
	});
	newHttp(httpData);
}

void AppHttp::httpBookCityInfo(YYXCallBack callback1)
{
	string url = string(IP).append(NET_BOOKCITY).append("?memberId=").append(App::getMemberID()).append("&page=1&pagesize=1000&resource=").append(App::m_resource);
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setPriority(_high);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			int code = YYXLayer::getIntForJson(-999, doc, "code");
			if (code == 0)
			{
				BookCityCache::getInstance()->allClear();
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

					auto info = CityButtonInfo::create()->setStoreId(castleId)->setType(storeBorder == 1 ? _double : _single)->setUrl(bgUrl)->setStoreName(castleName);
					BookCityCache::getInstance()->addStore(castleId, info);
				});
				YYXLayer::sendNotify(TAG_REMOVEWAITING);
				YYXLayer::sendNotify(TAG_CITYSCENEERFERVIEW);
				BookCityCache::getInstance()->writeFile();
			}
			else
				result = false;
		}
		if (!result)
		{
			if (callback1)
				callback1();
		}
	});
	httpData->setCallbackerror([=](HttpData* data) {
		if (callback1)
			callback1();
	});
	newHttp(httpData);
}

void AppHttp::httpNotification() {
	string url = std::string(IP).append(NET_NOTIFICATION);
	map<string, string> paramter;
	paramter["memberId"] = App::getMemberID();
	paramter["resourceType"] = "0";
	paramter["resource"] = App::m_resource;

	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(paramter);
	httpData->setPriority(_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			string desc = YYXLayer::getStringForJson("", doc, "desc");
			if (desc == "success")
			{
				auto doingFunction = [&](string key, string typeName) {
					if (typeName == "INT")
					{
						int number = YYXLayer::getIntForJson(-999, doc, "data", key);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, key, number);
						if (number != -999)
							YYXLayer::setFileValue(key, StringUtils::format("%d", number));
					}
					else if (typeName == "STRING")
					{
						auto str = YYXLayer::getStringForJson("", doc, "data", key);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, key, -999, str);
						if (str != "")
							YYXLayer::setFileValue(key, str);
					}
				};
				doingFunction("pushId", "INT");
				doingFunction("pushType", "INT");
				doingFunction("imageUrl", "STRING");
				doingFunction("pushUrl", "STRING");
				doingFunction("pushTitle", "STRING");
				doingFunction("pushString", "STRING");
				doingFunction("pushVersion", "STRING");
			}
		}
	});
	newHttp(httpData);
}

//获取修改密码的验证码
void AppHttp::httpCAPTCHA(std::string account)
{
	string url = string(IP).append(NET_GETCODE);
	map<string, string> p;
	p["mobileNum"] = account;
	p["type"] = "2";
	p["resource"] = App::m_resource;

	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(p);
	httpData->setPriority(_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			auto code = YYXLayer::getIntForJson(1, doc, "code");
			if (code == 0)
			{
				auto num = YYXLayer::getStringForJson("", doc, "num");
				YYXLayer::setFileValue("yanzhengCode", num);
				YYXLayer::setFileValue("codeTime", StringUtils::format("%d", (int)YYXLayer::getCurrentTime4Second()));
				Toast::create(App::getString("MESAAGEBOX_GETCODE_SUCCESS"));
				App::startTime();
				return;
			}
			else
			{
				auto desc = YYXLayer::getStringForJson("", doc, "desc");
				Toast::create(desc.c_str());
				return;
			}
		}
		Toast::create(App::getString("YANZHENGMAQINGQIUSHIBAIQINGCHONGSHI"));
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("YANZHENGMAQINGQIUSHIBAIQINGCHONGSHI"));
	});
	newHttp(httpData);	
}

//用户修改密码
void AppHttp::httpChangePassword(string acount, string newPsd) {
	string url = std::string(IP).append(NET_MODIFY_PASSWORD);
	map<string, string> p;
	p["memberName"] = acount;
	p["password"] = newPsd;
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(p);
	httpData->setPriority(HttpPriority::_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			auto code = YYXLayer::getIntForJson(1, doc, "code");
			if (code == 0)
			{
				Toast::create(App::getString("MESAAGEBOX_FORGETPASSWORD_SUCCESS"));
				return;
			}
		}
		Toast::create(App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR"));
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR"));

	});
	newHttp(httpData);
}

void AppHttp::httpRentBook(int bookid, YYXCallBack callback1)
{
	string url = string(IP).append(NET_RENT);
	map<string, string> par;
	par["memberId"] = StringUtils::format("%d", User::getInstance()->getMemberId());
	par["bookId"] = StringUtils::format("%d", bookid);
	par["resource"] = App::m_resource;
	par["payType"] = "2";
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(par);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool rent = false;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			auto code = YYXLayer::getIntForJson(-1, doc, "code");
			if (code == 0 || code == 4)
				rent = true;
		}
		if (rent)
		{
			if (callback1)
				callback1();
		}
		else
			Toast::create(App::getString("ZUSHUSHIBAI"), false);
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("ZUSHUSHIBAI"), false);
	});
	newHttp(httpData);
}

void AppHttp::httpVisitorMemberId(string deviceStr, const function<void(int)>& callback1, YYXCallBack callback2)
{
	string url = string(IP).append("/ellabook-server/member/TouristRegister");
	map<string, string> parameter;
	parameter["equipmentCode"] = deviceStr;
	parameter["resource"] = App::m_resource;
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setPriority(HttpPriority::_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([=](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			result = YYXLayer::getBoolForJson(false, doc, "result");
			if (result)
			{
				auto memberId = YYXLayer::getIntForJson(-999, doc, "memberId");
				if (callback1)
					callback1(memberId);
			}
			else
			{
				if (callback2)
					callback2();
			}
		}
		else
		{
			if (callback2)
				callback2();
		}
	});
	httpData->setCallbackerror([=](HttpData* data) {
		if (callback2)
			callback2();
	});
	newHttp(httpData);
}

void AppHttp::httpVipHint_VipType(int memberId, int waitkey)
{
	map<string, string> p;
	p["memberId"] = App::getMemberID();
	auto parameter = signPostData("ellabook.pay.selVipInfo", p, "1.0");

	string url = string(IP).append("/ellabook-server/rest/api/service");
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setPriority(HttpPriority::_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setUserValue(Value(waitkey));
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool resykt = YYXLayer::getJsonObject4Json(doc, json);
		if (!resykt)
			return;
		auto status = YYXLayer::getIntForJson(0, doc, "status");
		if (status == 1)
		{
			auto expireTime = YYXLayer::getIntForJson(-999, doc, "data","expirationReminder","expireTime");
			auto readBookNum = YYXLayer::getIntForJson(-999, doc, "data", "expirationReminder", "readBookNum");
			auto hours = YYXLayer::getIntForJson(-999, doc, "data", "expirationReminder", "hours");
			auto minute = YYXLayer::getIntForJson(-999, doc, "data", "expirationReminder", "minute");
			auto booksPrice = YYXLayer::getIntForJson(-999, doc, "data", "expirationReminder", "booksPrice");
			auto saveMoney = YYXLayer::getIntForJson(-999, doc, "data", "expirationReminder", "saveMoney");
			auto money = YYXLayer::getDoubleForJson(-999, doc, "data", "expirationReminder", "money")*100;
			if (money == -99900)
				money = YYXLayer::getIntForJson(-999, doc, "data", "expirationReminder", "money") * 100;
			if (YYXVisitor::getInstance()->getVisitorMode())
				YYXVisitor::getInstance()->setMoney(money);
			else
				User::getInstance()->setMoney(money);
			auto vn = VipNotify::getInstance();
			vn->setTimeOutDay(expireTime);
			vn->setBookCount(readBookNum);
			vn->setReadBookHour(hours);
			vn->setReadBookMinute(minute);
			vn->setAllMoney(booksPrice);
			vn->setSurplus(saveMoney);
			rapidjson::Value array;
			auto result = YYXLayer::getJsonArray4Json(array, doc, "data", "cardTypeInfo");
			if (result)
			{
				BuyVip::getInstance()->clear();
				YYXLayer::getDataForJsonArray(array, [](rapidjson::Value& item, int idx) {
					auto androidPrice = YYXLayer::getInt4Json(-999, item, "androidPrice");
					auto cardName = YYXLayer::getString4Json("", item, "cardName");
					auto cardTime = YYXLayer::getInt4Json(-999, item, "cardTime");
					auto cardType = YYXLayer::getInt4Json(-999, item, "cardType");
					auto abelType = YYXLayer::getInt4Json(-999, item, "abelType");
					auto cardTypeName = YYXLayer::getString4Json("", item, "cardTypeName");
					auto cardAbelType = YYXLayer::getInt4Json(-999, item, "cardAbelType");
					auto viptypeData = VipTypeData::create()->setPrice(androidPrice)->setBiaoqian((vipType)cardAbelType)->
						setBuycount(abelType)->setName(cardName)->setTimeName(cardTypeName)->setCardType(cardType);
					BuyVip::getInstance()->addData(viptypeData);
				});
			}
		}
		int key = data->getUserValue().asInt();
		if (key > 0)
			YYXLayer::sendNotify(TAG_REMOVEWAITING, "", key);
		YYXLayer::sendNotify(TAG_SHOWVIPHINT);
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("NETEXCEPTION"));
	});
	newHttp(httpData);
}

void AppHttp::httpMyMoneyPayVip(int memberId, int cardType, int waitkey)
{
	map<string, string> p;
	p["memberId"] = App::getMemberID();
	p["chargePlatformName"] = "android";
	p["chargePlatformTypeStr"] = "0";
	p["resource"] = App::m_resource;
	p["cardType"] = Value(cardType).asString();
	auto parameter = signPostData("ellabook.applepay.BalancePayVip", p, "1.0");

	string url = string(IP).append("/ellabook-server/rest/api/service");
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setPriority(HttpPriority::_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setUserValue(Value(waitkey));
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		if (!result)
		{
			Toast::create(App::getString("NETEXCEPTION"));
			YYXLayer::sendNotify(TAG_REMOVEWAITING);
			return;
		}
		auto status = YYXLayer::getIntForJson(0, doc, "status");
		if (status == 1)
		{
			auto expireTime = YYXLayer::getStringForJson("", doc, "data", "expireTime");
			auto _money = YYXLayer::getStringForJson("", doc, "data", "androidAmount");
			int money = Value(_money).asDouble() * 100;
			User::getInstance()->setMoney(money);
			User::getInstance()->setEndTime(expireTime);
			User::getInstance()->setVip(true);
		}
		int key = data->getUserValue().asInt();
		if (key > 0)
			YYXLayer::sendNotify(TAG_REMOVEWAITING, "", key);
		YYXLayer::sendNotify(TAG_SHOWVIPHINT);
	});
	httpData->setCallbackerror([](HttpData* data) {
		Toast::create(App::getString("NETEXCEPTION"));
		YYXLayer::sendNotify(TAG_REMOVEWAITING);
	});
	newHttp(httpData);
}

void AppHttp::httpUserIsOffLine()
{
	map<string, string> p;
	p["memberId"] = App::getMemberID();
	p["equipmentCode"] = CrossPlatform::getPhoneModel(3);
	auto parameter = signPostData("ellabook.member.signInStatus", p, "1.0");

	string url = string(IP).append("/ellabook-server/rest/api/service");
	HttpData* httpData = HttpData::create();
	httpData->setUrl(url);
	httpData->setParamter(parameter);
	httpData->setPriority(HttpPriority::_high);
	httpData->setHttpType(HttpRequest::Type::POST);
	httpData->setCallback([](HttpData* data) {
		string json = data->getJson();
		rapidjson::Document doc;
		bool result = YYXLayer::getJsonObject4Json(doc, json);
		if (!result)
		{
			return;
		}
		auto status = YYXLayer::getIntForJson(0, doc, "status");
		int youke = 2;
		if (status == 1)
		{
			string statusId = YYXLayer::getStringForJson("-1", doc, "data", "statusId");
			if (Value(statusId).asInt() == 0)
			{
				//在线
				youke = 0;
			}
			else if (Value(statusId).asInt() == 1)
			{
				//离线
				youke = 1;
				//弹出下线
				App::cancelData();
				YYXVisitor::getInstance()->loginVisitor();
				ControlScene::getInstance()->setDangqianScene(ControlScene::getInstance()->getCurrentScene()->getName());
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([youke]() {
					CrossPlatform::getInstance()->newOffLine();
				});
			}
		}
		else
		{
			//游客
			youke = 2;
		}		
	});
	httpData->setCallbackerror([](HttpData* data) {	});
	newHttp(httpData);
}