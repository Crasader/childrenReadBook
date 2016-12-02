/****************************************************************************
YYX框架
针对cocos2dx封装图层类
俞越星
2016/06/12
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//#endif
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//#endif
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//#endif
****************************************************************************/

#include "YYXLayer.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#endif
#include <iostream>   
#include <string>   
#include "NetIntface.h"

using namespace std;
using namespace cocostudio::timeline;
using namespace rapidjson;

YYXLayer::YYXLayer()
: csbFilePath("")
, parentNode(nullptr)
, className("className")
{

}

YYXLayer::~YYXLayer()
{
	App::log(":~YYXLayer");
	stopSchedule();
	App::log(":~YYXLayer---END");
}

Scene* YYXLayer::createScene(map<string, int64String> paramter, int  tag, string sceneName)
{
	if (!&paramter || !&sceneName)
	{
		App::log("paramter is error");
		return nullptr;
	}
	auto scene = Scene::create();
	auto layer = YYXLayer::create(paramter);
	scene->addChild(layer);
	layer->setTag(tag);
	layer->runScene = sceneName;
	return scene;
}

//创建场景,设置图层的tag
Scene * YYXLayer::createScene(map<string, int64String> paramter, map<string, function<void(YYXLayer*)>> runable, int  tag, string sceneName)
{
	if (!&paramter || !&sceneName || !&runable)
	{
		App::log("paramter is error");
		return nullptr;
	}
	auto scene = Scene::create();
	auto layer = YYXLayer::create(paramter, runable);
	scene->addChild(layer);
	layer->setTag(tag);
	layer->runScene = sceneName;
	return scene;
}

YYXLayer *YYXLayer::create(map<string, int64String> paramter)
{	
    YYXLayer *ret = new (std::nothrow) YYXLayer();	
    if (ret && ret->init(paramter))
    {
		App::log("YYXLayer::create");
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
		App::log("图层创建失败");
        return nullptr;
    }
}


//创建图层
YYXLayer *YYXLayer::create(map<string, int64String> paramter, map<string, function<void(YYXLayer*)>> runable)
{
	YYXLayer *ret = new (std::nothrow) YYXLayer();
	if (ret && ret->init(paramter, runable))
	{
		App::log("YYXLayer::create");
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		App::log("图层创建失败");
		return nullptr;
	}
}

bool YYXLayer::init(map<string, int64String> paramter)
{	
	App::log(className + "::init()" );
	App::log("YYXLayer::init");
	if (!&paramter)
	{
		App::log("paramter is error");
		return false;
	}
	if (paramter.find("className") != paramter.end())
	{
		className = paramter["className"].stringPara;
	}
	if (paramter.find("csb") != paramter.end())
	{
		csbFilePath = paramter["csb"].stringPara;
		Data data;
		if (App::GetInstance()->getData(csbFilePath, data))
			parentNode = CSLoader::createNode(data);
		else
			parentNode = CSLoader::createNode(csbFilePath);
		addChild(parentNode);
	}
	App::log(className + "::init---END");
	return true;
}

bool YYXLayer::init(map<string, int64String> paramter, map<string, function<void(YYXLayer*)>> runable)
{
	App::log(className + "::init()");
	if (!&paramter)
	{
		App::log("paramter is error");
		return false;
	}
	m_runable = runable;
	m_paramter = paramter;
	if (runable.find("initCacheLoad") != runable.end())
	{
		runable["initCacheLoad"](this);
	}
	if (paramter.find("className") != paramter.end())
	{
		className = paramter["className"].stringPara;
	}
	if (paramter.find("csb") != paramter.end())
	{
		csbFilePath = paramter["csb"].stringPara;
		Data data;
		if (paramter.find("csbTimeLine") != paramter.end())
		{
			int endIndex = paramter["csbTimeLine"].intPara;
			ActionTimeline* nodeAnimation =nullptr;
			parentNode = CSLoader::createNode(csbFilePath);
			nodeAnimation = CSLoader::createTimeline(csbFilePath);
			if (nodeAnimation)
			{
				int loop = -999;
				if (paramter.find("gotoFrameAndPlayIsLoop") != paramter.end())
					loop = paramter["gotoFrameAndPlayIsLoop"].intPara;
				if (loop == 0)
					nodeAnimation->gotoFrameAndPlay(0, endIndex, true);
				else
					nodeAnimation->gotoFrameAndPlay(0, endIndex, false);
				parentNode->runAction(nodeAnimation);
			}
		}
		else
		{
			if (App::GetInstance()->getData(csbFilePath, data))
				parentNode = CSLoader::createNode(data);
			else
				parentNode = CSLoader::createNode(csbFilePath);
		}	
		addChild(parentNode);
	}
	if (runable.find("initLayer") != runable.end())
	{
		runable["initLayer"](this);
	}
	if (runable.find("initEvent") != runable.end())
	{
		runable["initEvent"](this);
	}
	if (runable.find("initHttp") != runable.end())
	{
		runable["initHttp"](this);
	}	
	App::log(className+"::init---END" );
	return true;
}

void YYXLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	App::log(className + " / onEnterTransitionDidFinish");
	if(m_runable.find("onEnterTransitionDidFinish") != m_runable.end())
		m_runable["onEnterTransitionDidFinish"](this);
}

Node* YYXLayer::getParentNode()
{
	return parentNode;
}

YYXLayer * YYXLayer::MyMessageBox(string titile, string yesString, function<void()> yesRunable, string noString, function<void()> noRunable)
{
	std::map<string, int64String > parameter;
	YYXLayer::insertMap4ParaType(parameter, "className", -999, "");
	YYXLayer::insertMap4ParaType(parameter, "csb", -999, SELCETMESSAGEBOX);
	auto messagebox = YYXLayer::create(parameter);
	auto yes = (Button*)messagebox->findControl(FIND_BUTTON_SELCETMESSAGEBOX_YES);
	auto no = (Button*)messagebox->findControl(FIND_BUTTON_SELCETMESSAGEBOX_NO);
	auto yestext = (Text*)messagebox->findControl(FIND_TEXT_SELCETMESSAGEBOX_YES);
	auto notext = (Text*)messagebox->findControl(FIND_TEXT_SELCETMESSAGEBOX_NO);
	auto content = (Text*)messagebox->findControl(FIND_TEXT_SELCETMESSAGEBOX_CONTEXT);
	if (yes)
	{
		yes->addClickEventListener([=](Ref* sender) {
			if (yesRunable)
				yesRunable();
			messagebox->removeFromParentAndCleanup(true);
		});
	}
	if (no)
	{
		no->addClickEventListener([=](Ref* sender) {
			if (noRunable)
				noRunable();
			messagebox->removeFromParentAndCleanup(true);
		});
	}
	if (yestext && yesString != "")
		yestext->setText(yesString);
	if (notext && noString != "")
		notext->setText(noString);
	if (content)
		content->setText(titile);
	messagebox->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	messagebox->getParentNode()->setPosition(Director::getInstance()->getVisibleSize() / 2);
	return messagebox;
}

void YYXLayer::stringHttpRequest(HttpRequest::Type type,string url,map<string,string> paramter,int outTime,std::function<void(string json)> runable,std::function<void()> errorRunable)
{
	//App::log("%s::stringHttpRequest()", className);
	if (!&url)
	{
		App::log("stringHttpRequest URL is error");
		return;
	}
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
			request->setRequestData(postData.c_str(), postData.length());
		}
	}
	//App::log("\n" + url + "       -----------------------     \n" + postData);
	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		if (!response)
		{
			if(errorRunable)
				errorRunable();
			//App::log(url + +" ( "+postData + " )" + " stringHttpRequest http failed code = 404" + "\n\n\n");
			return;
		}
		if (!response->isSucceed())
		{
			if(errorRunable)
				errorRunable();
			//App::log("\n\n\n" + url + "                                      \n" + postData + "                                                \n" + "stringHttpRequest http failed code =" + "\n\n\n", response->getResponseCode());
			return;
		}
		std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
		//App::log("\n\n\n"+url+"                                      \n" + postData +"                                                \n"+str+ "\n\n\n");
		if (runable)
			runable(str);
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(outTime);
	request->release();
	//App::log("%s::stringHttpRequest---END", className);
}

Ref* YYXLayer::findControl(string controlName)
{
	//App::log(className+"::findControl()" );
	if (parentNode == nullptr)
	{
		//App::log("parentNode is null");
		return nullptr;
	}
	if (!&controlName && controlName.empty())
	{
		//App::log("controlName is empty");
		return nullptr;
	}
	if (controls.find(controlName) == controls.end())
	{
		auto control = parentNode->getChildByName(controlName);
		if (control == nullptr)
		{
			//App::log("controlName is null");
			return nullptr;
		}
		else
		{
			controls[controlName] = control;
			//App::log(className + "::findControl---END");
			return control;
		}
	}
	else
	{
		auto control = controls[controlName];
		if (control)
		{
			//App::log(className+ "::findControl---END");
			return control;
		}
		else
		{
			//App::log("controlName is null");
			return nullptr;
		}
	}
}

void YYXLayer::leave()
{
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
}

void YYXLayer::controlTouchTime(float outTime, string keyOfSaveTime, std::function<void()> runable, std::function<void()> unTouchRun)
{
	if (!&keyOfSaveTime && outTime>0)
	{
		App::log("paramter is error");
		return;
	}
	int t1 = outTime * 1000;
	auto t = App::GetInstance()->getTime(keyOfSaveTime, 0);
	int time = t % 100000000;
	int nowtime = NetIntface::getMillsTime()% 100000000;
	auto reslut = time - nowtime;
	App::log(StringUtils::format(" ( %d ) controlTouchTime >= %f", abs(reslut), outTime));
	if (abs(reslut) >= t1)
	{
		App::GetInstance()->addTime(keyOfSaveTime, nowtime);
		if (runable)
		{
			App::log(StringUtils::format("( %d ) controlTouchTime RUN: TimeKey = %s, nowtime = %d, lastTime = %d  ", abs(reslut), keyOfSaveTime.c_str(), nowtime, time));
			runable();
		}
	}
	else
	{
		App::log(StringUtils::format("( %d ) controlTouchTime Error: TimeKey = %s, nowtime = %d, lastTime = %d  ", abs(reslut), keyOfSaveTime.c_str(), nowtime, time));
		if (unTouchRun) {
			unTouchRun();
		}
	}
}

void YYXLayer::MainThreadCallBack(string str, const std::function<void(string)>& runFunction)
{
	App::log(className + "::MainThreadCallBack()" );
	if (!&str)
	{
		App::log("paramter is error");
		return;
	}
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([runFunction, str] {
		if (runFunction)
			runFunction(str);
	});
	App::log(className + "::MainThreadCallBack---END" );
}

void YYXLayer::stopSchedule()
{
	App::log(className + "::stopSchedule()");
	for (auto it : timeNames)
	{
		unschedule(it);
	}
	App::log(className+ "::stopSchedule---END");
}

void YYXLayer::addSchedule(const string scheduleTimeName,float interval,const std::function<void()>& runable)
{
	App::log(className+"::addSchedule()");
	if (!&scheduleTimeName)
	{
		App::log("paramter is error");
		return;
	}
	schedule([runable](float f) {
		if(runable)
			runable();
	}, interval,scheduleTimeName);
	timeNames.push_back(scheduleTimeName);
	App::log(className+"::addSchedule---END");
}

void YYXLayer::sendNotify(string eventName,string className, int data)
{	
	//App::log(className + "::sendNotify = "+eventName ,data);
	if (!&eventName || !&className)
	{
		App::log("paramter is error");
		return;
	}
	Director::getInstance()->getEventDispatcher()->setEnabled(true);
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([eventName, data] {
		EventCustom event(eventName);
		if (data != -999)
			event.setUserData((void*)data);
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
	});
	//App::log(className + "::sendNotify = " + eventName +"---END");
}

void YYXLayer::sendNotify4YYXStruct(string eventName, long long luserdata, string userdata, string className)
{
	App::log(className + "::sendNotify = " + eventName);
	if (!&eventName || !&className || !&userdata)
	{
		App::log("sendNotify paramter is error");
		return;
	}
	Director::getInstance()->getEventDispatcher()->setEnabled(true);
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([eventName, luserdata, userdata] {
		EventCustom event(eventName);
		event.setUserData(new YYXStruct(luserdata, userdata, nullptr));
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
	});
	App::log(className + "::sendNotify = " + eventName + "---END");
}

////打开数据库
//sqlite3* YYXLayer::OpenDataBase() {
//	string path = FileUtils::getInstance()->getWritablePath() + "data.db";
//	sqlite3* MyDB = nullptr;
//	int result = sqlite3_open(path.c_str(), &MyDB);
//	if (result != SQLITE_OK)
//		App::log("Open database fialed! ErrCode = ", result);
//	else
//		App::log("Open database successful");
//	if (!MyDB)
//	{
//		App::log("sqlite is error");
//		return nullptr;
//	}
//	return MyDB;
//}

//关闭数据库
//void YYXLayer::CloseDataBase(sqlite3* MyDB) {
//	if (!MyDB)
//	{
//		App::log("sqlite is error");
//		return ;
//	}
//	int result = sqlite3_close(MyDB);
//	if (result != SQLITE_OK)
//		App::log("close database fialed!");
//	else
//		App::log("the database was closed");
//}

//向表中插入记录
//bool YYXLayer::InsertData(sqlite3* MyDB, string tableName, unordered_map<string, string> paraInfoSTR, unordered_map<string, long long> paraInfoINT)
//{
//	if (!MyDB)
//	{
//		App::log("sqlite is error");
//		return false;
//	}
//	string strPara1, strPara2;
//	unordered_map<string, string>::iterator itor1;//字符串类型参数的拼接
//	for (itor1 = paraInfoSTR.begin(); itor1 != paraInfoSTR.end(); itor1++)
//	{
//		strPara1 += itor1->first + ",";
//		strPara2 += "'" + itor1->second + "',";
//	}
//	//整型参数的拼接
//	unordered_map<string, long long>::iterator itor2;
//	for (itor2 = paraInfoINT.begin(); itor2 != paraInfoINT.end(); itor2++)
//	{
//		strPara1 += itor2->first + ",";
//		strPara2 += StringUtils::format("%d", itor2->second) + ",";
//	}
//	//去掉最后一个逗号
//	strPara1.erase(strPara1.size() - 1, 1);
//	strPara2.erase(strPara2.size() - 1, 1);
//	string sqlstr = "insert into " + tableName + " (" + strPara1 + ") values(" + strPara2 + ");";//sql语句
//	sqlite3_busy_timeout(MyDB, 1000); //注册超时等待时间为1000ms
//	//执行
//	char* errMsg;
//	int result = sqlite3_exec(MyDB, sqlstr.c_str(), NULL, NULL, &errMsg);
//	if (result != SQLITE_OK) {
//		return false;
//	}
//	return true;
//}

//更新记录
//bool YYXLayer::UpdateData(sqlite3* MyDB, string tableName, unordered_map<string, string> paraInfoSTR, unordered_map<string, long long> paraInfoINT, string condition) {
//	if (!MyDB)
//	{
//		App::log("sqlite is error");
//		return false;
//	}
//	string strPara;
//	unordered_map<string, string>::iterator itor1;//字符串类型参数的拼接
//	for (itor1 = paraInfoSTR.begin(); itor1 != paraInfoSTR.end(); itor1++)
//	{
//		strPara += itor1->first + "='" + itor1->second + "',";
//	}
//
//	//整型参数的拼接
//	unordered_map<string, long long>::iterator itor2;
//	for (itor2 = paraInfoINT.begin(); itor2 != paraInfoINT.end(); itor2++)
//	{
//		strPara += itor2->first + "=" + StringUtils::format("%d", itor2->second) + ",";
//	}
//
//	//去掉最后一个逗号
//	strPara.erase(strPara.size() - 1, 1);
//	string sqlstr = "update " + tableName + " set " + strPara + " " + condition;//sql语句
//
//																				//注册超时等待时间为1000ms
//	sqlite3_busy_timeout(MyDB, 1000);
//	//执行
//	char* errMsg;
//	int result = sqlite3_exec(MyDB, sqlstr.c_str(), NULL, NULL, &errMsg);
//	if (result != SQLITE_OK)
//		return false;
//
//	return true;
//}

//删除记录
//bool YYXLayer::DeleteData(sqlite3* MyDB, string tableName, string condition) {
//	if (!MyDB)
//	{
//		App::log("sqlite is error");
//		return false;
//	}
//	string sqlstr = "delete from " + tableName + " " + condition;//sql语句
//	sqlite3_busy_timeout(MyDB, 1000);//注册超时等待时间为1000ms
//	//执行
//	char* errMsg;
//	int result = sqlite3_exec(MyDB, sqlstr.c_str(), NULL, NULL, &errMsg);
//	if (result != SQLITE_OK)
//		return false;
//	return true;
//}

//查找记录或记录中的某些字段
//vector<unordered_map<string, int64String>> YYXLayer::SelectData(sqlite3* MyDB, string tableName, vector<string> paraStr, string condition) 
//{	
//	vector<unordered_map<string, int64String>> Result;//select结果集
//	if (!MyDB)
//	{
//		App::log("sqlite is error");
//		return Result;
//	}
//	string strPara;//拼接查找字段
//	vector<string>::iterator iter;
//	for (iter = paraStr.begin(); iter != paraStr.end(); iter++)
//	{
//		strPara += *iter + ",";
//	}
//	strPara.erase(strPara.size() - 1, 1);
//	string sqlstr = "select " + strPara + " from " + tableName + " " + condition;//sql语句
//
//	if (strcmp(paraStr[0].c_str(), "*") == 0) {//如果是查询全部字段，则找到对应表的全部字段
//		paraStr.clear();
//
//		char **pRes = NULL;
//		int nRow = 0, nCol = 0;
//		char *pErr = NULL;
//		sqlite3_get_table(MyDB, sqlstr.c_str(), &pRes, &nRow, &nCol, &pErr);
//		for (int i = 0; i < nRow; i++)
//		{
//			for (int j = 0; j < nCol; j++)
//			{
//				char *pv = *(pRes + nCol*i + j);
//				paraStr.push_back(pv);
//			}
//			break;
//		}
//
//		if (pErr != NULL)
//		{
//			sqlite3_free(pErr);
//		}
//		sqlite3_free_table(pRes);
//	}
//
//
//	sqlite3_stmt* stmt = NULL;
//	if (sqlite3_prepare_v2(MyDB, sqlstr.c_str(), strlen(sqlstr.c_str()), &stmt, NULL) != SQLITE_OK) {//准备
//		if (stmt)
//			sqlite3_finalize(stmt);
//		return Result;
//	}
//
//	//按步读取数据（每条记录一步）
//	int rc = sqlite3_step(stmt);
//	int  ncols = sqlite3_column_count(stmt);
//	while (rc == SQLITE_ROW)
//	{
//		unordered_map<string, int64String> Record;//一条记录
//		for (int i = 0; i < ncols; i++) {
//			if (i >= paraStr.size())
//				break;
//			int resType = sqlite3_column_type(stmt, i);
//
//			int64String myType = int64String();
//
//			if (resType == SQLITE_INTEGER) {
//				myType.intPara = sqlite3_column_int(stmt, i);
//				myType.stringPara = "";
//			}
//			else if (resType == SQLITE_TEXT) {
//				myType.intPara = -999;
//				myType.stringPara = string((const char*)sqlite3_column_text(stmt, i));
//			}
//
//			Record[paraStr[i]] = myType;
//			//delete myType;
//		}
//		Result.push_back(Record);
//		Record.clear();
//		rc = sqlite3_step(stmt);//执行下一步（下一条记录）
//
//	}
//	sqlite3_finalize(stmt);
//	return Result;
//}

//判断记录是否存在
//bool YYXLayer::IsRecExist(sqlite3* MyDB, string tableName, string condition) {
//	if (!MyDB)
//	{
//		App::log("sqlite is error");
//		return false;
//	}
//	string sqlstr = "select * from " + tableName + " " + condition;//sql语句
//
//	sqlite3_stmt* stmt = NULL;
//	if (sqlite3_prepare_v2(MyDB, sqlstr.c_str(), strlen(sqlstr.c_str()), &stmt, NULL) != SQLITE_OK) {//准备
//		if (stmt)
//			sqlite3_finalize(stmt);
//		return false;
//	}
//
//	int rc = sqlite3_step(stmt);
//	sqlite3_finalize(stmt);
//
//	if (rc == SQLITE_ROW)
//		return true;
//	else
//		return false;
//}

//bool YYXLayer::IsNetConnect()
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//	App::log("WIN32--This function is not implemented");
//	return true;
//#endif
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//	bool result = CocosAndroidJni::IsNetConnect();
//	if (!result)
//		App::log(App::getString("WEIJIANCEDAOWANGLUO"));
//	return result;
//#endif
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//	App::log("IOS--This function is not implemented");
//	return true;
//#endif
//}

long long YYXLayer::getCurrentTime4Second()
{
	long long times = -999;
	times = time(0);
	return times;
}

//每次获取不同的随机数
long long YYXLayer::getRandom()
{
	long long times = -999;
	times = time(0);
	auto num = YYXStruct::getMapInt64(App::GetInstance()->myData, "getRandom", -999);
	if (times <= num)
		++num;
	else
		num = times;
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "getRandom", num);
	return num;
}

bool YYXLayer::getJsonObject4Json(rapidjson::Document& doc, string json)
{
	doc.Parse<0>(json.c_str());
	if (&doc && doc.HasParseError())
	{
		App::log("doc HasParseError");
		return false;
	}
	else if (doc.IsObject())
	{
		return true;
	}
	else
	{
		App::log("doc Is not Object");
		return false;
	}
}

string YYXLayer::getString4Json(string defaultResult, rapidjson::Value & doc, string key1, string key2, string key3)
{
	string result = defaultResult;
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsString())
				result = key1value.GetString();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsString())
					result = key2value.GetString();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsString())
						result = key3value.GetString();
				}
			}
		}
	}
	return result;
}

int YYXLayer::getInt4Json(int defaultResult, rapidjson::Value & doc, string key1, string key2, string key3)
{
	int result = defaultResult;
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsInt())
				result = key1value.GetInt();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsInt())
					result = key2value.GetInt();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsInt())
						result = key3value.GetInt();
				}
			}
		}		
	}
	return result;
}

long long YYXLayer::getInt644Json(long long defaultResult, rapidjson::Value & doc, string key1, string key2, string key3)
{
	long long result = defaultResult;	
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsInt64())
				result = key1value.GetInt64();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsInt64())
					result = key2value.GetInt64();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsInt64())
						result = key3value.GetInt64();
				}
			}
		}
	}
	return result;	
}

bool YYXLayer::getBool4Json(bool defaultResult, rapidjson::Value & doc, string key1, string key2, string key3)
{
	bool result = defaultResult;
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsBool())
				result = key1value.GetBool();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsBool())
					result = key2value.GetBool();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsBool())
						result = key3value.GetBool();
				}
			}
		}
	}
	return result;
}

double YYXLayer::getDouble4Json(double defaultResult, rapidjson::Value & doc, string key1, string key2, string key3)
{
	double result = defaultResult;
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsDouble())
				result = key1value.GetDouble();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsDouble())
					result = key2value.GetDouble();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsDouble())
						result = key3value.GetDouble();
				}
			}
		}
	}
	return result;
}

bool YYXLayer::getJsonArray4Json(rapidjson::Value & result, rapidjson::Value & doc, string key1, string key2, string key3)
{
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsArray())
			{
				result = key1value;
				return true;
			}
			else
				return false;
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsArray())
				{
					result = key2value;
					return true;
				}
				else
					return false;
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsArray())
					{
						result = key3value;
						return true;
					}
					else
						return false;
				}
				return false;
			}
		}
	}
}

void YYXLayer::getData4JsonArray(rapidjson::Value & jsonarray, map<string,YYXStruct>& result, std::function<void(rapidjson::Value & item,map<string, YYXStruct>&)> runable)
{
	if (&jsonarray && jsonarray.IsArray() && &result )
	{
		rapidjson::SizeType count = jsonarray.Size();
		for (rapidjson::SizeType i = 0; i < count; i++)
		{
			rapidjson::Value &item = jsonarray[i];
			if(runable)
				runable(item,result);
		}
	}
}

void YYXLayer::getDataForJsonArray(rapidjson::Value & jsonarray, std::function<void(rapidjson::Value & item, int i)> runable)
{
	if (&jsonarray && jsonarray.IsArray())
	{
		rapidjson::SizeType count = jsonarray.Size();
		for (rapidjson::SizeType i = 0; i < count; i++)
		{
			rapidjson::Value &item = jsonarray[i];
			if (runable)
				runable(item, i);
		}
	}
}

bool YYXLayer::getBoolForJson(bool defaultResult, rapidjson::Document& doc, string key1, string key2, string key3)
{
	bool result = defaultResult;
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsBool())
				result = key1value.GetBool();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsBool())
					result = key2value.GetBool();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsBool())
						result = key3value.GetBool();
				}
			}
		}
	}
	return result;
}

string YYXLayer::getStringForJson(string defaultResult, rapidjson::Document & doc, string key1, string key2, string key3)
{
	string result = defaultResult;
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsString())
				result = key1value.GetString();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsString())
					result = key2value.GetString();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsString())
						result = key3value.GetString();
				}
			}
		}
	}
	return result;
}

int YYXLayer::getIntForJson(int defaultResult, rapidjson::Document & doc, string key1, string key2, string key3)
{
	int result = defaultResult;
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsInt())
				result = key1value.GetInt();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsInt())
					result = key2value.GetInt();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsInt())
						result = key3value.GetInt();
				}
			}
		}
	}
	return result;
}

double YYXLayer::getDoubleForJson(double defaultResult, rapidjson::Document & doc, string key1, string key2 /*= ""*/, string key3 /*= ""*/)
{
	double result = defaultResult;
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsDouble())
				result = key1value.GetDouble();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsDouble())
					result = key2value.GetDouble();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsDouble())
						result = key3value.GetDouble();
				}
			}
		}
	}
	App::log("json =>Double * 100 = ", result * 100);
	return result;
}

void YYXLayer::insertMap4ParaType(std::map<std::string, int64String>& map, std::string key, long long number, std::string str)
{
	if (&map)
	{
		int64String ty = int64String();
		ty.intPara = number;
		ty.stringPara = str;
		map[key] = ty;
	}
}

//使用转场场景进行转场
void YYXLayer::ChangeScene(std::function<void()> runable)
{
	map<string, int64String> paramter;
	insertMap4ParaType(paramter, "className", -999, "YYXLayer");
	function<void(YYXLayer*)> functionRun = [](YYXLayer* sender) {
		auto layer = LayerColor::create(Color4B::WHITE);
		layer->addChild(WaitLayer());
		sender->addChild(layer);
	};
	map<string, function<void(YYXLayer*)>> paramterRun;
	paramterRun["initLayer"] = functionRun;
	paramterRun["onEnterTransitionDidFinish"] = [runable](YYXLayer*) {
		runable();
	};
	Director::getInstance()->replaceScene( YYXLayer::createScene(paramter, paramterRun,9,"ChangeScene"));	
}


Layer* YYXLayer::WaitLayer()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(PLIST_WAITMESSAGEBOX);
	Data data;
	Layer* wait;
	ActionTimeline* timeline;
	if (App::GetInstance()->getData(WAITMESSAGEBOX_CSB, data))
	{
		wait = (Layer*)CSLoader::createNode(data);
		timeline = CSLoader::createTimeline(data, WAITMESSAGEBOX_CSB);
	}
	else
	{
		wait = (Layer*)CSLoader::createNode(WAITMESSAGEBOX_CSB);
		timeline = CSLoader::createTimeline(WAITMESSAGEBOX_CSB);
	}
	wait->setAnchorPoint(Vec2(0.5f, 0.5f));
	wait->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	wait->runAction(timeline);
	timeline->gotoFrameAndPlay(0, 28, true);
	auto imageview = (ImageView*)wait->getChildByName(FIND_IMAGEVIEW_BY_NAME_WAITMESSAGEBOX);
	imageview->setTouchEnabled(true);
	imageview->setSwallowTouches(true);
	imageview->addClickEventListener([](Ref* sender) {
		App::log("dianji");
	});
	return wait;
}

//创建一个loading图层
YYXLayer* YYXLayer::LoadingLayer(int dtimme, function<void()> runable)
{
	App::log("YYXLayer::LoadingLayer");
	auto co = YYXStruct::getMapInt64(App::GetInstance()->myData, "LoadingLayerCount", -1);
	if (co > 0)
	{
		return nullptr;
	}
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "createMyLoadingLayerTime", getCurrentTime4Second());
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "bookstoreLoading");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, WAITMESSAGEBOX_CSB);
	YYXLayer::insertMap4ParaType(paramter, "csbTimeLine", 28, "");
	YYXLayer::insertMap4ParaType(paramter, "gotoFrameAndPlayIsLoop", 0, "");
	auto loading = YYXLayer::create(paramter, map<string, function<void(YYXLayer*)>>());
	auto listener = EventListenerCustom::create("deleteMyLoadingLayer", [=](EventCustom* e) {
		App::log("              respond deleteMyLoadingLayer^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
		loading->removeFromParentAndCleanup(true);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "LoadingLayerCount", 0);
		if (runable)
			runable();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, loading);
	loading->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	loading->setPosition(Director::getInstance()->getVisibleSize() / 2);
	auto imageview = (ImageView*)loading->findControl(FIND_IMAGEVIEW_BY_NAME_WAITMESSAGEBOX);
	imageview->setTouchEnabled(true);
	imageview->setSwallowTouches(true);
	imageview->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "bookStoreSceneClickLoadingLayerTime", [=]() {
			auto nowtime = YYXLayer::getCurrentTime4Second();
			auto createTime = YYXStruct::getMapInt64(App::GetInstance()->myData, "createMyLoadingLayerTime", nowtime);
			if (nowtime > (createTime + dtimme))
			{
				loading->removeFromParentAndCleanup(true);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "LoadingLayerCount", 0);
				if (runable)
					runable();
			}
		});
	});
	string key = "DeleteLoadingLayerKeyName";
	loading->addSchedule(key, 1, [=]() {
		auto nowtime = YYXLayer::getCurrentTime4Second();
		auto createTime = YYXStruct::getMapInt64(App::GetInstance()->myData, "createMyLoadingLayerTime", nowtime);
		if (nowtime > (createTime + dtimme))
		{
			loading->removeFromParentAndCleanup(true);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "LoadingLayerCount", 0);
			if (runable)
				runable();
		}
	});
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "LoadingLayerCount", 1);
	App::log("YYXLayer::LoadingLayer--END");
	return loading;
}

//删除loading图层
void YYXLayer::deleteLoadingLayer(int t)
{
	thread([=]() {
		App::ccsleep(t*1000);
		YYXLayer::sendNotify("deleteMyLoadingLayer");
	}).detach();
}

void YYXLayer::DownLoadNotify(string url, string dir, string fileName, string runKey, string errorKey)
{
	stringHttpRequest(HttpRequest::Type::GET, url, map<string, string>(), 10, [runKey, dir, fileName](string json) {
		if (!FileUtils::getInstance()->isDirectoryExist(dir))
		{
			FileUtils::getInstance()->createDirectory(dir);
		}
		auto path = dir + "/" + fileName;
		auto buf = json.c_str();
		FILE *fp = fopen(path.c_str(), "wb+");
		fwrite(buf, 1, json.length(), fp);
		fclose(fp);
		sendNotify4YYXStruct(runKey, -999, path);
	}, [errorKey]() {
		sendNotify4YYXStruct(errorKey);
	});
}

void YYXLayer::DownLoad(string url, string dir, string fileName, std::function<void(string json)> runable, std::function<void()> errorRunable)
{
	App::log("DownLoad = >" + url);
	if (!&url)
	{
		App::log("DownLoad = >paramter is error");
		return;
	}
	HttpRequest* pRequest = new HttpRequest();
	pRequest->setUrl(url.c_str());
	pRequest->setRequestType(HttpRequest::Type::GET);
	pRequest->setResponseCallback([=](HttpClient* client, HttpResponse* response)
	{
		if (!response)
		{
			if (errorRunable)
				errorRunable();
			App::log("DownLoad = > 404");
			return;
		}
		if (!response->isSucceed())
		{
			if (errorRunable)
				errorRunable();
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

//删除文件夹
void YYXLayer::DeleteDirectory(string filePath)
{
	App::log("delete Dir = " + filePath);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_finddata_t FileInfo;
	string strfind = filePath + "/*";

	long Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L) {
		//CCLOG("can not match the folder path");
		return;
	}
	do {
		//判断是否有子目录
		if (FileInfo.attrib & _A_SUBDIR)
		{
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				//文件夹-递归
				string newPath = filePath + "/" + FileInfo.name;
				DeleteDirectory(newPath);
			}
		}
		else
		{
			//文件-删除
			string filename = (filePath + "/" + FileInfo.name);
			remove(filename.c_str());
		}
	} while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);

	//最后删除空文件夹
	rmdir(filePath.c_str());
#else
	struct dirent* ent = NULL;
	DIR *pDir;
	pDir = opendir(filePath.c_str());
	char FileName[512];
	char FolderName[512];
	strcpy(FolderName, filePath.c_str());
	strcat(FolderName, "/%s");
	if (pDir == NULL) {
		return;
	}
	while (NULL != (ent = readdir(pDir))) {
		if (ent->d_type == 8) {
			//文件-删除
			sprintf(FileName, FolderName, ent->d_name);
			remove(FileName);
		}
		else {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
				continue;
			}
			//文件夹-递归
			sprintf(FileName, FolderName, ent->d_name);
			DeleteDirectory(FileName);
		}
	}
	//删除空文件夹
	rmdir(filePath.c_str());
#endif // (CC_PLATFORM_ANDROID == CC_TARGET_PLATFORM)
}

void YYXLayer::WStrToUTF8(std::string& dest, const wstring& src) {
	dest.clear();
	for (size_t i = 0; i < src.size(); i++) {
		wchar_t w = src.at(i);
		if (w <= 0x7f) {
			dest.push_back((char)w);
		}
		else if (w <= 0x7ff) {
			dest.push_back(0xc0 | ((w >> 6) & 0x1f));
			dest.push_back(0x80 | (w & 0x3f));
		}
		else if (w <= 0xffff) {
			dest.push_back(0xe0 | ((w >> 12) & 0x0f));
			dest.push_back(0x80 | ((w >> 6) & 0x3f));
			dest.push_back(0x80 | (w & 0x3f));
		}
		else if (sizeof(wchar_t) > 2 && w <= 0x10ffff) {
			dest.push_back(0xf0 | ((w >> 18) & 0x07)); // wchar_t 4-bytes situation
			dest.push_back(0x80 | ((w >> 12) & 0x3f));
			dest.push_back(0x80 | ((w >> 6) & 0x3f));
			dest.push_back(0x80 | (w & 0x3f));
		}
		else {
			dest.push_back('?');
		}
	}
}

std::string YYXLayer::ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";  
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

std::wstring YYXLayer::s2ws(const std::string& s)
{
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

long long YYXLayer::getInt64ForJson(long long defaultResult, rapidjson::Value & doc, string key1, string key2 /*= ""*/, string key3 /*= ""*/)
{
	long long result = defaultResult;
	if (&doc && doc.HasMember(key1.c_str()) && &key1)
	{
		rapidjson::Value &key1value = doc[key1.c_str()];
		if (key2 == "" && key3 == "")
		{
			if (key1value.IsInt64())
				result = key1value.GetInt64();
		}
		else if (key3 == "")
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsInt64())
					result = key2value.GetInt64();
			}
		}
		else
		{
			if (key1value.IsObject() && key1value.HasMember(key2.c_str()))
			{
				rapidjson::Value &key2value = key1value[key2.c_str()];
				if (key2value.IsObject() && key2value.HasMember(key3.c_str()))
				{
					rapidjson::Value &key3value = key2value[key3.c_str()];
					if (key3value.IsInt64())
						result = key3value.GetInt64();
				}
			}
		}
	}
	return result;
}

//将已购列表的书籍ID用json方式存入path路径下
void YYXLayer::buyBooksJsonWriteFile(vector<int> mapValue, int memberID, string path)
{
	rapidjson::Document writedoc;
	writedoc.SetObject();
	rapidjson::Document::AllocatorType& allocator = writedoc.GetAllocator();
	rapidjson::Value array(rapidjson::kArrayType);
	rapidjson::Value object(rapidjson::kObjectType);

	// json 加入数组
	for (auto it : mapValue)
	{
		array.PushBack(it, allocator);
	}

	// json object 格式添加 “名称/值” 对
	writedoc.AddMember("memberId", memberID, allocator);
	writedoc.AddMember("books", array, allocator);

	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	writedoc.Accept(writer);
	FILE* file = fopen(path.c_str(), "wb");
	if (file)
	{
		fputs(buffer.GetString(), file);
		fclose(file);
	}
	App::log(buffer.GetString());
}

void YYXLayer::logb(string str)
{
	App::log(str);
}

void YYXLayer::loge(string str)
{
	App::log(str+"---END");
}

std::string YYXLayer::getStringFormMap(map<string, string> p)
{
	rapidjson::Document writedoc;
	writedoc.SetObject();
	rapidjson::Document::AllocatorType& allocator = writedoc.GetAllocator();
	rapidjson::Value root(kObjectType);
	for (auto it : p)
	{
		auto key = it.first;
		auto value = it.second;
		rapidjson::Value strkey(kStringType);
		strkey.SetString(key.c_str(), key.size(), allocator);
		rapidjson::Value strV(kStringType);
		strV.SetString(value.c_str(), value.size(), allocator);
		// json object 格式添加 “名称/值” 对
		root.AddMember(strkey, strV, allocator);
	}
	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	root.Accept(writer);
	return buffer.GetString();
}

void YYXLayer::writeFile(string text, string path)
{
	FileUtils::getInstance()->writeStringToFile(text, path);
}

std::string YYXLayer::readFile(string path)
{
	ssize_t size = 0;
	std::string load_str;
	unsigned char* titlech = FileUtils::getInstance()->getFileData(path, "r", &size);
	load_str = std::string((const char*)titlech, size);
	return load_str;
}

void YYXLayer::writeFilepp (string str, string path)//追加写文件
{
	size_t size = 0;
	const char* mode = "ab+";
	do
	{
		// Read the file from hardware
		FILE *fp = fopen(path.c_str(), mode);
		CC_BREAK_IF(!fp);
		Data retData;
		retData.copy((unsigned char*)str.c_str(), str.size());
		size = retData.getSize();
		fwrite(retData.getBytes(), size, 1, fp);
		fclose(fp);
		return ;
	} while (0);
	return ;
}

void YYXLayer::setFileValue(string key, string value)
{
	string dirPath = FileUtils::getInstance()->getWritablePath() + "Data";
	if (!FileUtils::getInstance()->isDirectoryExist(dirPath))
	{
		FileUtils::getInstance()->createDirectory(dirPath);
	}
	string path = dirPath + "/" + key;
	if (value.empty())
		return;
	writeFile(value, path);
}

std::string YYXLayer::getFileValue(string key, string defaultstr)
{
	string dirPath = FileUtils::getInstance()->getWritablePath() + "Data";
	string path = dirPath + "/" + key;
	if (FileUtils::getInstance()->isFileExist(path))
	{
		string str = readFile(path);
		return str;
	}
	else
		return defaultstr;
}

void YYXLayer::deleteFileValue(string key)
{
	string dirPath = FileUtils::getInstance()->getWritablePath() + "Data";
	string path = dirPath + "/" + key;
	if (FileUtils::getInstance()->isFileExist(path))
	{
		remove(path.c_str());
	}
}

bool YYXLayer::getBoolFromXML(string str)
{
	string data = YYXLayer::getFileValue(str, "true");
	if (data == "false")
	{
		return false;
	}
	else
		return true;
}

std::string YYXLayer::getStringTimeFromInt64Time(long long t)
{
	struct tm *lt;
	time_t tData = t;
	lt = localtime(&tData);
	char nowtime[24];
	memset(nowtime, 0, sizeof(nowtime));
	//strftime(nowtime, 24, "%Y-%m-%d %H:%M:%S", lt);
	strftime(nowtime, 24, "%Y-%m-%d", lt);
	return string(nowtime);
}

//void YYXLayer::showCommentListView(ListView * listview, map<string, map<string, YYXStruct>>& data, vector<string>& sortData, 
//	int titleMaxLength, int listviewMaxWidth, int contentTextMaxLength,int voiceMaxLength)
//{
//	App::log("    ---------------------          YYXLayer::showCommentListView", data.size());
//	if (!listview) {
//		loge("listview = null");
//		return;
//	}
//
//	listview->setVisible(true);
//	listview->removeAllItems();
//	for (auto it : sortData)
//	{
//		auto result = data.find(it);
//		if (result == data.end())
//			continue;
//		auto mapData = result->second;
//		int score = atoi(mapData["score"].stringData.c_str());//星级
//		auto memberName = mapData["memberName"].stringData;//用户名
//		time_t commentTime = atoi(mapData["commentTime"].stringData.c_str());//评论时间
//		auto state = mapData["gevalState"].stringData;//只打星 过滤不展示
//		auto titileText = mapData["title"].stringData;//文字标题
//		auto contentText = mapData["content"].stringData;//文字内容
//		auto commentType = mapData["gevalType"].stringData;//文字=""0 语音="1"
//		string voiceLength = mapData["gevalTime"].stringData;//语音时间
//		int gevalId = atoi(mapData["gevalId"].stringData.c_str());//评论id
//		App::log("memberName" + memberName);
//		auto memberId = mapData["memberId"].stringData;
//		double beishu = 3;
//		//评分+账号+日期 800*80
//		auto item2 = Layout::create();
//		memberName.replace(3, 4, "****");
//		Data dataCSB;
//		Layer* scorelayer;
//		if (App::GetInstance()->getData(BOOKINFO_SCORE_CSB, dataCSB))
//			scorelayer = (Layer*)CSLoader::createNode(dataCSB);
//		else
//			scorelayer = (Layer*)CSLoader::createNode(BOOKINFO_SCORE_CSB);//整行图层
//		if (scorelayer == nullptr) {
//			App::log("scorelayer == nullptr");
//			continue;
//		}
//		scorelayer->setAnchorPoint(Vec2(0, 0));
//		auto first = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_FIRST);//第1个星
//		auto second = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_SECOND);//第2个星
//		auto three = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_THREE);//第3个星
//		auto fourth = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_FOURTH);//第4个星
//		auto fifth = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_FIFTH);//第5个星
//		auto menbername = (Text*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_MEMBERNAME);//手机号码
//		auto time = (Text*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_TIME);//日期
//		menbername->setString(memberName);
//		time->setString(App::getFormatTime(commentTime));
//		item2->addChild(scorelayer);
//		item2->setContentSize(Size(listviewMaxWidth, scorelayer->getContentSize().height));
//		//间隔横线
//		auto item4 = Layout::create();
//		auto view = Text::create("________________________________________________________________________________________________", "FZHLJW.TTF", 20 * beishu);
//		view->setScale(1 / beishu);
//		view->setAnchorPoint(Vec2(0, 0));
//		view->setTextColor(Color4B::GRAY);
//		item4->addChild(view);
//		item4->setContentSize(Size(listviewMaxWidth, view->getContentSize().height / beishu - 10));
//		if (commentType == "0")//文字
//		{
//			if (state == "1")
//			{
//				App::log("state == 1");
//				continue;
//			}
//			//标题
//			auto item1 = Layout::create();
//			if (titileText.length() > titleMaxLength)
//				titileText = titileText.substr(0, titleMaxLength).append("...");
//			auto titil = Label::create(titileText, "wdtyj.TTF", 16 * beishu);
//			titil->setAnchorPoint(Vec2(0, 0));
//			titil->setTextColor(Color4B(251, 133,54, 255));
//			titil->setAdditionalKerning(4 * beishu);//字体间距
//			titil->setLineHeight(16 * beishu);//行间距
//			titil->setMaxLineWidth(listviewMaxWidth * beishu);
//			titil->setScale(1 / beishu);
//			item1->addChild(titil);
//			item1->setContentSize(Size(listviewMaxWidth, titil->getContentSize().height / beishu + 4));
//			//内容
//			auto item3 = Layout::create();
//			if (contentText.length() > contentTextMaxLength)
//				contentText = contentText.substr(0, contentTextMaxLength).append("...");
//			auto content = Label::create(contentText, "wdtyj.TTF", 14 * beishu);
//			content->setAnchorPoint(Vec2(0, 0));
//			content->setTextColor(Color4B(108, 72, 49, 255));
//			content->setAdditionalKerning(2 * beishu);//字体间距
//			content->setLineHeight(26 * beishu);//行间距
//			content->setTextColor(Color4B::BLACK);
//			content->setMaxLineWidth(listviewMaxWidth * beishu);
//			content->setScale(1 / beishu);
//			if (memberId == App::getMemberID())
//			{
//				auto imview = ImageView::create();
//				imview->setAnchorPoint(Vec2(0, 0));
//				imview->setPosition(Vec2(0, 0));
//				imview->setSize(Size(listviewMaxWidth, content->getContentSize().height / beishu + 4));
//				imview->setScale9Enabled(true);
//				imview->loadTexture("other/block.png");
//				imview->setOpacity(1);
//				imview->setTag(gevalId);
//				imview->setSwallowTouches(false);
//				imview->setTouchEnabled(true);
//				imview->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
//					auto view = (ImageView*)sender;
//					if (type == Widget::TouchEventType::BEGAN)
//					{
//						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
//					}
//					else if (type == Widget::TouchEventType::MOVED)
//					{
//						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
//					}
//					else	if (type == Widget::TouchEventType::ENDED)
//					{
//						auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
//						if (click == 1)
//							return;
//						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
//						//删除
//						auto message = YYXLayer::MyMessageBox(App::getString("SHANCHUPINGLUN"), App::getString("SHANCHU"), [=]() {
//							auto id = view->getTag();
//							NetIntface::httpDeleteComment(id, [=](int commentID) {
//								YYXLayer::sendNotify("DeleteCommentRefershListView", "", commentID);
//							}, [](string str) {
//								Toast::create(str.c_str());
//							});
//						}, "", []() {});
//						Director::getInstance()->getRunningScene()->addChild(message);
//					}
//					else if (type == Widget::TouchEventType::CANCELED)
//					{
//					}
//				});
//				item3->addChild(imview);
//			}
//			item3->addChild(content);
//			item3->setContentSize(Size(listviewMaxWidth, content->getContentSize().height / beishu + 4));
//
//			//插入
//			listview->pushBackCustomItem(item1);
//			listview->pushBackCustomItem(item2);
//			listview->pushBackCustomItem(item3);
//			listview->pushBackCustomItem(item4);
//		}
//		else if (commentType == "1")//语音
//		{
//			//语音
//			auto item3 = Layout::create();
//			int lengthv = atoi(voiceLength.c_str());
//			if (lengthv <= 0)
//				lengthv = 1;
//			if (lengthv >= 29)
//				lengthv = 30;
//			string voiceCSB = "BookInfo/csb/voicecomment.csb";
//			Data dataCSB;
//			Node* voiceLayer;
//			if (App::GetInstance()->getData(voiceCSB, dataCSB))
//				voiceLayer = CSLoader::createNode(dataCSB);
//			else
//				voiceLayer = CSLoader::createNode(voiceCSB);
//			if (voiceLayer == nullptr)
//			{
//				App::log("voiceLayer == nullptr");
//				continue;
//			}
//			voiceLayer->setTag(0);
//			auto bgView = (ImageView*)voiceLayer->getChildByName("Image_2");
//			bgView->setSize(Size(listviewMaxWidth,40));
//			auto voiceCommentView = (ImageView*)voiceLayer->getChildByName("Image_1");//100-220伸缩
//			auto voiceTimeView = (Text*)voiceLayer->getChildByName("Text_4");
//			if (voiceCommentView)
//			{
//				voiceCommentView->setTag(gevalId);
//				voiceCommentView->setSwallowTouches(false);
//				int width = (voiceMaxLength - 100) / 30 * lengthv + 100;
//				voiceCommentView->setSize(Size(width, voiceCommentView->getSize().height));
//				voiceCommentView->setTouchEnabled(true);
//				voiceCommentView->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
//					auto view = (ImageView*)sender;
//					if (type == Widget::TouchEventType::BEGAN)
//					{
//						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
//					}
//					else if (type == Widget::TouchEventType::MOVED)
//					{
//						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
//					}
//					else	if (type == Widget::TouchEventType::ENDED)
//					{
//						auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
//						if (click == 1)
//							return;
//						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
//						if (voiceLayer->getTag() == 1)
//						{//停止播放
//							AudioEngine::stopAll();
//							YYXLayer::sendNotify("StopAnimation");
//							if (YYXLayer::getBoolFromXML(MUSIC_KEY))
//								SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
//							else
//								SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
//						}
//						else
//						{//播放
//							YYXLayer::sendNotify("StopAnimation", "", (int)voiceLayer);
//							voiceLayer->setTag(1);
//							int id = view->getTag();
//							string paht = FileUtils::getInstance()->getWritablePath() + "voiceComment/" + StringUtils::format("%d.mp3", id);
//							if (!paht.empty() && FileUtils::getInstance()->isFileExist(paht))
//							{
//								if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
//								{
//									SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
//									SimpleAudioEngine::getInstance()->pauseAllEffects();
//								}
//								AudioEngine::stopAll();
//								auto AEplay = new AudioEngine();
//								auto animate = FrameAnimation::createAnimate(3, "other/vioceplist.plist", "other/Backcover_vioce%d_736h.png", 0.2f);
//								auto seqDoor = Sequence::create(animate, animate->reverse(), NULL);
//								auto act = RepeatForever::create(seqDoor);
//								auto voiceSp = (Sprite*)voiceLayer->getChildByName("Sprite_11");
//								if (voiceSp)
//									voiceSp->runAction(act);
//								auto playId = AEplay->play2d(paht);
//								AEplay->setFinishCallback(playId, [=](int id, string path) {
//									App::log(" music finish " + paht);
//									// 音乐结束回调
//									auto voiceSp = (Sprite*)voiceLayer->getChildByName("Sprite_11");
//									if (voiceSp) {
//										voiceSp->stopAllActions();
//										voiceSp->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("other/Backcover_vioce2_736h.png"));
//									}
//									voiceLayer->setTag(0);
//									if (YYXLayer::getBoolFromXML(MUSIC_KEY))
//										SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
//									else
//										SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
//									if (AEplay)
//										delete AEplay;
//								});
//							}
//							else
//								Toast::create(App::getString("YUYINGBOFANGSHIBAI"));
//						}
//					}
//					else if (type == Widget::TouchEventType::CANCELED)
//					{
//					}
//				});
//			}
//			if (voiceTimeView)
//				voiceTimeView->setText(StringUtils::format("%d\"", lengthv));
//			if (bgView)
//			{
//				bgView->setTag(gevalId);
//				bgView->setSwallowTouches(false);
//				if (memberId == App::getMemberID())
//				{
//					bgView->setTouchEnabled(true);
//					bgView->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
//						auto view = (ImageView*)sender;
//						if (type == Widget::TouchEventType::BEGAN)
//						{
//							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
//						}
//						else if (type == Widget::TouchEventType::MOVED)
//						{
//							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
//						}
//						else	if (type == Widget::TouchEventType::ENDED)
//						{
//							auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
//							if (click == 1)
//								return;
//							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
//							//删除
//							auto message = YYXLayer::MyMessageBox(App::getString("SHANCHUPINGLUN"), App::getString("SHANCHU"), [=]() {
//								auto id = view->getTag();
//								NetIntface::httpDeleteComment(id, [=](int commentID) {
//									YYXLayer::sendNotify("DeleteCommentRefershListView", "", commentID);
//								}, [](string str) {
//									Toast::create(str.c_str());
//								});
//							}, "", []() {});
//							Director::getInstance()->getRunningScene()->addChild(message);
//						}
//						else if (type == Widget::TouchEventType::CANCELED)
//						{
//						}
//					});
//				}
//			}
//			item3->addChild(voiceLayer);
//			item3->setContentSize(Size(listviewMaxWidth, voiceLayer->getContentSize().height + 5));
//			//删除评论后刷新列表
//			auto listenerDeleteComment = EventListenerCustom::create("DeleteCommentRefershListView", [&](EventCustom* e) {
//				auto commentID = (int)e->getUserData();
//				string commentid = StringUtils::format("%d", commentID);
//				if (data.find(commentid) != data.end())
//				{
//					data.erase(commentid);
//					AudioEngine::stopAll();
//					if (YYXLayer::getBoolFromXML(MUSIC_KEY))
//						SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
//					else
//						SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
//					showCommentListView(listview, data, sortData);
//				}
//			});
//			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerDeleteComment, listview);
//			//停止动画
//			auto listenerStopVoicePlay = EventListenerCustom::create("StopAnimation", [=](EventCustom* e) {
//				if ((int)e->getUserData() != (int)voiceLayer)
//				{
//					auto voiceSp = (Sprite*)voiceLayer->getChildByName("Sprite_11");
//					if (voiceSp) {
//						voiceSp->stopAllActions();
//						voiceSp->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("other/Backcover_vioce2_736h.png"));
//					}
//					voiceLayer->setTag(0);
//				}
//			});
//			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerStopVoicePlay, voiceLayer);
//			//插入
//			listview->pushBackCustomItem(item2);
//			listview->pushBackCustomItem(item3);
//			listview->pushBackCustomItem(item4);
//		}
//	}
//	listview->jumpToTop();
//	loge("YYXLayer::showCommentListView");
//}

//重新展示评论列表
void YYXLayer::showCommentListView(ListView * listview, int bookid,	int memberid, int titleMaxLength, int listviewMaxWidth, int contentTextMaxLength, int voiceMaxLength)
{
	App::log("YYXLayer::showCommentListView");
	if (!listview) {
		loge("listview = null");
		return;
	}
	listview->setVisible(true);
	listview->removeAllItems();
	int count = YYXStruct::getMapInt64(App::GetInstance()->myData, StringUtils::format("comment_bookid=%d", bookid),0);
	if (count <= 0)
		return;
	for (int i = 0; i < count; i++)
	{
		string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, i);//bookid + 下标顺序
		int commentID = YYXStruct::getMapInt64(App::GetInstance()->myData, idKey, -999);//评论id + 类型
		string type = YYXStruct::getMapString(App::GetInstance()->myData, idKey, "");
		if (commentID == -999)
			continue;
		string memberName = "";
		int score = 0;
		string contentText = "";
		string titileText = "";
		int commentTime = 0;
		int memberId = 0;
		int voiceLength = 0;
		string url = "";
		int commentType = 0;
		if (type == "TEXT")
		{
			commentType = 1;
			string contentKey = StringUtils::format("comment_gevalId=%d+score+content", commentID);
			string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", commentID);
			string titleKey = StringUtils::format("comment_gevalId=%d+title", commentID);

			contentText = YYXStruct::getMapString(App::GetInstance()->myData, contentKey, "");//文字内容
			titileText = YYXStruct::getMapString(App::GetInstance()->myData, titleKey, "");//文字标题
			memberName = YYXStruct::getMapString(App::GetInstance()->myData, timeKey, "132****1434");//用户名
			score = YYXStruct::getMapInt64(App::GetInstance()->myData, contentKey, 4);//星级
			commentTime = YYXStruct::getMapInt64(App::GetInstance()->myData, timeKey, 0);//评论时间
			memberId = (int)YYXStruct::getMapRef(App::GetInstance()->myData, timeKey, 0);//用户id
		}
		else if (type == "VOICE")
		{
			commentType = 2;
			string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", commentID);
			string voiceKey = StringUtils::format("comment_gevalId=%d+score+url+gevalTime", commentID);

			memberName = YYXStruct::getMapString(App::GetInstance()->myData, timeKey, "132****1434");//用户名
			score = YYXStruct::getMapInt64(App::GetInstance()->myData, voiceKey, 3);//星级
			commentTime = YYXStruct::getMapInt64(App::GetInstance()->myData, timeKey, 0);//评论时间
			memberId = (int)YYXStruct::getMapRef(App::GetInstance()->myData, timeKey, 0);//用户id
			voiceLength = (int)YYXStruct::getMapRef(App::GetInstance()->myData, voiceKey, 0);//语音时间
			url = YYXStruct::getMapString(App::GetInstance()->myData, voiceKey, "");//语音url
		}
		else {
			continue;
		}
		if (memberid != -1 && memberId != memberid)
		{//我的评论
			continue;
		}
		double beishu = 3;
		//评分+账号+日期 800*80
		auto item2 = Layout::create();
		memberName.replace(3, 4, "****");
		Data dataCSB;
		Layer* scorelayer;
		if (App::GetInstance()->getData(BOOKINFO_SCORE_CSB, dataCSB))
			scorelayer = (Layer*)CSLoader::createNode(dataCSB);
		else
			scorelayer = (Layer*)CSLoader::createNode(BOOKINFO_SCORE_CSB);//整行图层
		if (scorelayer == nullptr) {
			App::log("scorelayer == nullptr");
			continue;
		}
		scorelayer->setAnchorPoint(Vec2(0, 0));
		auto first = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_FIRST);//第1个星
		auto second = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_SECOND);//第2个星
		auto three = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_THREE);//第3个星
		auto fourth = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_FOURTH);//第4个星
		auto fifth = (ImageView*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_FIFTH);//第5个星
		auto menbername = (Text*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_MEMBERNAME);//手机号码
		auto time = (Text*)scorelayer->getChildByName(BOOKINFO_FIND_LISTVIEWITEM_BYNAME_TIME);//日期
		menbername->setString(memberName);
		time->setString(App::getFormatTime(commentTime));
		item2->addChild(scorelayer);
		item2->setContentSize(Size(listviewMaxWidth, scorelayer->getContentSize().height));
		//间隔横线
		auto item4 = Layout::create();
		auto view = Text::create("________________________________________________________________________________________________", "FZHLJW.TTF", 20 * beishu);
		view->setScale(1 / beishu);
		view->setAnchorPoint(Vec2(0, 0));
		view->setTextColor(Color4B::GRAY);
		item4->addChild(view);
		item4->setContentSize(Size(listviewMaxWidth, view->getContentSize().height / beishu - 10));
		if (commentType ==1)
		{
			//标题
			auto item1 = Layout::create();
			if (titileText.length() > titleMaxLength)
				titileText = titileText.substr(0, titleMaxLength).append("...");
			auto titil = Label::create(titileText, "wdtyj.TTF", 16 * beishu);
			titil->setAnchorPoint(Vec2(0, 0));
			titil->setTextColor(Color4B(251, 133, 54, 255));
			titil->setAdditionalKerning(4 * beishu);//字体间距
			titil->setLineHeight(16 * beishu);//行间距
			titil->setMaxLineWidth(listviewMaxWidth * beishu);
			titil->setScale(1 / beishu);
			item1->addChild(titil);
			item1->setContentSize(Size(listviewMaxWidth, titil->getContentSize().height / beishu + 4));
			//内容
			auto item3 = Layout::create();
			if (contentText.length() > contentTextMaxLength)
				contentText = contentText.substr(0, contentTextMaxLength).append("...");
			auto content = Label::create(contentText, "wdtyj.TTF", 14 * beishu, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
			content->setAnchorPoint(Vec2(0, 0));
			content->setTextColor(Color4B(108, 72, 49, 255));
			content->setAdditionalKerning(2 * beishu);//字体间距
			content->setLineHeight(26 * beishu);//行间距
			content->setMaxLineWidth(listviewMaxWidth * beishu);
			content->setScale(1 / beishu);

			if (StringUtils::format("%d",memberId) == App::getMemberID())
			{
				auto imview = ImageView::create();
				imview->setAnchorPoint(Vec2(0, 0));
				imview->setPosition(Vec2(0, 0));
				imview->setSize(Size(listviewMaxWidth, content->getContentSize().height / beishu + 4));
				imview->setScale9Enabled(true);
				imview->loadTexture("other/block.png");
				imview->setOpacity(1);
				imview->setTag(i);
				imview->setSwallowTouches(false);
				imview->setTouchEnabled(true);
				imview->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
					auto view = (ImageView*)sender;
					if (type == Widget::TouchEventType::BEGAN)
					{
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
					}
					else if (type == Widget::TouchEventType::MOVED)
					{
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
					}
					else	if (type == Widget::TouchEventType::ENDED)
					{
						auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
						if (click == 1)
							return;
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
						//删除
						auto message = YYXLayer::MyMessageBox(App::getString("SHANCHUPINGLUN"), App::getString("SHANCHU"), [=]() {
							auto index = view->getTag();
							string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);//bookid + 下标顺序
							int thisCommentID = YYXStruct::getMapInt64(App::GetInstance()->myData, idKey, 0);//评论id + 类型
							NetIntface::httpDeleteComment(thisCommentID, [=](int commentID) {
								string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);
								YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, idKey);
								YYXLayer::sendNotify("showCommentListView", "111111111111111111111", memberid);
							}, [](string str) {
								Toast::create(str.c_str());
							});
						}, "", []() {});
						Director::getInstance()->getRunningScene()->addChild(message);
					}
					else if (type == Widget::TouchEventType::CANCELED)
					{
					}
				});
				item3->addChild(imview);
			}
			item3->addChild(content);
			item3->setContentSize(Size(listviewMaxWidth, content->getContentSize().height / beishu + 4));

			//插入
			listview->pushBackCustomItem(item1);
			listview->pushBackCustomItem(item2);
			listview->pushBackCustomItem(item3);
			listview->pushBackCustomItem(item4);
		}
		else if (commentType == 2)
		{
			//语音
			auto item3 = Layout::create();
			if (voiceLength <= 0)
				voiceLength = 1;
			if (voiceLength >= 29)
				voiceLength = 30;
			string voiceCSB = "BookInfo/csb/voicecomment.csb";
			Data dataCSB;
			Node* voiceLayer;
			if (App::GetInstance()->getData(voiceCSB, dataCSB))
				voiceLayer = CSLoader::createNode(dataCSB);
			else
				voiceLayer = CSLoader::createNode(voiceCSB);
			if (voiceLayer == nullptr)
			{
				App::log("voiceLayer == nullptr");
				continue;
			}
			voiceLayer->setTag(0);
			auto bgView = (ImageView*)voiceLayer->getChildByName("Image_2");
			bgView->setSize(Size(listviewMaxWidth, 40));
			auto voiceCommentView = (ImageView*)voiceLayer->getChildByName("Image_1");//100-220伸缩
			auto voiceTimeView = (Text*)voiceLayer->getChildByName("Text_4");
			if (voiceCommentView)
			{
				voiceCommentView->setTag(commentID);
				voiceCommentView->setSwallowTouches(false);
				int width = (voiceMaxLength - 100) / 30 * voiceLength + 100;
				voiceCommentView->setSize(Size(width, voiceCommentView->getSize().height));
				voiceCommentView->setTouchEnabled(true);
				voiceCommentView->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
					auto view = (ImageView*)sender;
					if (type == Widget::TouchEventType::BEGAN)
					{
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
					}
					else if (type == Widget::TouchEventType::MOVED)
					{
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
					}
					else	if (type == Widget::TouchEventType::ENDED)
					{
						App::log("Widget::TouchEventType::ENDED");
						auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
						if (click == 1)
							return;
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
						if (voiceLayer->getTag() == 1)
						{//停止播放
							AudioEngine::stopAll();
							YYXLayer::sendNotify("StopAnimation");
							if (YYXLayer::getBoolFromXML(MUSIC_KEY))
								SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
							else
								SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
						}
						else
						{//播放
							YYXLayer::sendNotify("StopAnimation", "", (int)voiceLayer);
							voiceLayer->setTag(1);
							int id = view->getTag();
							string paht = FileUtils::getInstance()->getWritablePath() + "voiceComment/" + StringUtils::format("%d.mp3", id);
							if (!paht.empty() && FileUtils::getInstance()->isFileExist(paht))
							{
								if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
								{
									SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
									SimpleAudioEngine::getInstance()->pauseAllEffects();
								}
								AudioEngine::stopAll();
								auto AEplay = new AudioEngine();
								auto animate = FrameAnimation::createAnimate(3, "other/vioceplist.plist", "other/Backcover_vioce%d_736h.png", 0.2f);
								auto seqDoor = Sequence::create(animate, animate->reverse(), NULL);
								auto act = RepeatForever::create(seqDoor);
								auto voiceSp = (Sprite*)voiceLayer->getChildByName("Sprite_11");
								if (voiceSp)
									voiceSp->runAction(act);
								auto playId = AEplay->play2d(paht);
								AEplay->setFinishCallback(playId, [=](int id, string path) {
									App::log(" music finish " + paht);
									// 音乐结束回调
									auto voiceSp = (Sprite*)voiceLayer->getChildByName("Sprite_11");
									if (voiceSp) {
										voiceSp->stopAllActions();
										voiceSp->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("other/Backcover_vioce2_736h.png"));
									}
									voiceLayer->setTag(0);
									if (YYXLayer::getBoolFromXML(MUSIC_KEY))
										SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
									else
										SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
									if (AEplay)
										delete AEplay;
								});
							}
							else
								Toast::create(App::getString("YUYINGBOFANGSHIBAI"));
						}
					}
					else if (type == Widget::TouchEventType::CANCELED)
					{
					}
				});
			}
			if (voiceTimeView)
				voiceTimeView->setText(StringUtils::format("%d\"", voiceLength));
			if (bgView)
			{
				bgView->setTag(i);
				bgView->setSwallowTouches(false);
				if (StringUtils::format("%d",memberId) == App::getMemberID())
				{
					bgView->setTouchEnabled(true);
					bgView->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
						auto view = (ImageView*)sender;
						if (type == Widget::TouchEventType::BEGAN)
						{
							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
						}
						else if (type == Widget::TouchEventType::MOVED)
						{
							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
						}
						else	if (type == Widget::TouchEventType::ENDED)
						{
							auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "listviewClickAble", 0);//0=点击有效 1=点击无效
							if (click == 1)
								return;
							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "listviewClickAble", 1);//0=点击有效 1=点击无效
							//删除
							auto message = YYXLayer::MyMessageBox(App::getString("SHANCHUPINGLUN"), App::getString("SHANCHU"), [=]() {
								auto index = view->getTag();
								string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);//bookid + 下标顺序
								int thisCommentID = YYXStruct::getMapInt64(App::GetInstance()->myData, idKey, 0);//评论id + 类型
								NetIntface::httpDeleteComment(thisCommentID, [=](int commentID) {
									string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);
									YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, idKey);
									YYXLayer::sendNotify("showCommentListView", "222222222222222", memberid);
								}, [](string str) {
									Toast::create(str.c_str());
								});
							}, "", []() {});
							Director::getInstance()->getRunningScene()->addChild(message);
						}
						else if (type == Widget::TouchEventType::CANCELED)
						{
						}
					});
				}
			}
			item3->addChild(voiceLayer);
			item3->setContentSize(Size(listviewMaxWidth, voiceLayer->getContentSize().height + 5));
			//停止动画
			auto listenerStopVoicePlay = EventListenerCustom::create("StopAnimation", [=](EventCustom* e) {
				if ((int)e->getUserData() != (int)voiceLayer)
				{
					auto voiceSp = (Sprite*)voiceLayer->getChildByName("Sprite_11");
					if (voiceSp) {
						voiceSp->stopAllActions();
						voiceSp->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("other/Backcover_vioce2_736h.png"));
					}
					voiceLayer->setTag(0);
				}
			});
			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerStopVoicePlay, voiceLayer);
			//插入
			listview->pushBackCustomItem(item2);
			listview->pushBackCustomItem(item3);
			listview->pushBackCustomItem(item4);
		}
	}
	//刷新列表
	//auto listenerRefersh = EventListenerCustom::create("showCommentListView", [=](EventCustom* e) {
	//	int memberid = (int)e->getUserData();
	//	App::log(" --------------     run showCommentListView()   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^memberid = ", memberid);
	//	AudioEngine::stopAll();
	//	if (YYXLayer::getBoolFromXML(MUSIC_KEY))
	//		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	//	else
	//		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	//	showCommentListView(listview, bookid, memberid, titleMaxLength, listviewMaxWidth, contentTextMaxLength, voiceMaxLength);
	//});
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerRefersh, listview);
	listview->jumpToTop();
	YYXLayer::sendNotify("showCommentListViewEND","666666666666", listview->getItems().size());
	loge("YYXLayer::showCommentListView");
}

void YYXLayer::TraversingJson(string json, map<string, string>& data)
{
	rapidjson::Document doc;
	doc.Parse<0>(json.c_str());
	if (!doc.HasParseError())
	{
		for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); it++)
		{
			string key = (it->name).GetString();
			string value = (it->value).GetString();
			data[key] = value;
		}
	}
	else
		App::log("json is Error");
}

//返回按钮
void YYXLayer::PLAYBUTTON()
{
	if (getBoolFromXML(SOUND_KEY))
    SimpleAudioEngine::getInstance()->playEffect(ELLA_SOUND_BUTTON);
}

//复制文件夹 不替换
void YYXLayer::CopyDirectory(string sourceDir, string destDir)
{
	App::log("YYXLayer::CopyDirectory " + sourceDir + " == >>>" + destDir);
	if (!FileUtils::getInstance()->isDirectoryExist(destDir))
		FileUtils::getInstance()->createDirectory(destDir);
	NetIntface::TraversingFiles(sourceDir, [destDir](string filepath, string filename) {
		string destpath = destDir + "/" + filename;
		if (!FileUtils::getInstance()->isFileExist(destpath))
		{
			App::copyFile(filepath, destpath);
			App::log("copyFile=  " + filepath + "   ======>>" + destpath);
		}
	}, [destDir](string dirpath, string dirname) {
		CopyDirectory(dirpath, destDir + "/" + dirname);
	});
}