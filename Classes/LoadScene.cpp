#include "LoadScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "NetIntface.h"
#include "YYXLayer.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* Load::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Load::create();
	layer->setTag(9);
    // add layer as a child to scene
    scene->addChild(layer);
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Load::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
		return false;
    }
	App::GetInstance();	
	//记录当前场景
	App::m_RunningScene = LoadScene;
	totalCount = 23;
	currentCount = 0;
	Size m_Size = Director::getInstance()->getVisibleSize();
	auto img = ImageView::create();
	img->setAnchorPoint(Vec2(0.5, 0.5));
	img->loadTexture("kaichanglogo.png", TextureResType::LOCAL);
	img->setPosition(Vec2(m_Size.width/2, m_Size.height/2));
	addChild(img);
	//app打开时间
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "APPOpenTime", YYXLayer::getCurrentTime4Second());
	//获取机型
	NetIntface::getPhoneModel();
	//通知异步加载图片完成
	auto listenLoadPngOver = EventListenerCustom::create("NOTIFY_LOAD_PNG_OVER", [=](EventCustom* event) {
		auto cache = SpriteFrameCache::getInstance();
		cache->addSpriteFramesWithFile(PLIST_BACKGROUND);
		cache->addSpriteFramesWithFile(PLIST_INDEX);
		cache->addSpriteFramesWithFile(PLIST_TRAIN);
		m_cacheOver = true;
		YYXLayer::sendNotify("LoadSceneOverGoToIndex");
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listenLoadPngOver, 1);
	initEvent();
	App::GetInstance()->pushScene(MySceneName::LoadScene);
    return true;
}

void Load::initEvent()
{
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("LoadSceneOverGoToIndex", [=](EventCustom* e) {
		//加载完成 && (书城网络请求结束 && 网络超时5秒)
		auto time = YYXStruct::getMapInt64(App::GetInstance()->myData, "APPOpenTime", 0) + 5;
		if (m_cacheOver) {
			if (YYXLayer::getCurrentTime4Second() >= time || m_httpOver)
			{
				unschedule("LoadSceneOverGoToIndexSchedule");
				Director::getInstance()->replaceScene(Index::createScene());
			}
		}
	});
	schedule([](float f) {
		YYXLayer::sendNotify("LoadSceneOverGoToIndex");
	}, 1, "LoadSceneOverGoToIndexSchedule");
}

void Load::onEnter()
{
	Layer::onEnter();
}

void Load::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	initDir();
	initData();
	initMemberHttp();
	App::protectedTiming();
	thread([]() {
		YYXLayer::CopyDirectory(FileUtils::getInstance()->getWritablePath()+"unzip", FileUtils::getInstance()->getWritablePath()+"bookUNZip");
	}).detach();
	initHttp();
}

void Load::initData()
{
	//加载本地数据
	loadingLocalFileData();
	//初始化Toast
	Toast::GetInstance()->showToast();
	// 声音初始化数据
	if (!YYXLayer::getBoolFromXML("MCGMUSIC"))
	{
		initUserData();
		YYXLayer::setFileValue("MCGMUSIC", "true");
	}
	//预加载声音
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY);
	//预加载音效
	SimpleAudioEngine::getInstance()->preloadEffect(ELLA_SOUND_CASTLENEXTPAGE);//铁轨
	SimpleAudioEngine::getInstance()->preloadEffect(ELLA_SOUND_BIRDS);//鸟
	SimpleAudioEngine::getInstance()->preloadEffect(ELLA_SOUND_TRAINWHISTLE);//汽笛
	SimpleAudioEngine::getInstance()->preloadEffect(ELLA_SOUND_BUTTON);//按钮	
	//加载plist大图
	loadPlistPng();
	//缓存csb文件
	loadCsbFile();
   //下载初始化
	YYXDownload::GetInstance()->setDownloadMaxCount(3);
	YYXDownload::GetInstance()->startAll();
}

void Load::initHttp()
{
	//上传阅读记录
	App::searchReadRecordJson();
	//上传错误日志
	//App::upLoadingErrorLog();
	//书城
	httpBookCityInfoAndDownLoad();
	//红包活动
	httpRedPacketActivity();
	//请求最新版本号
	getEllaVersion();
	//请求消息推送
	getEllaNotification();
}

void Load::initMemberHttp()
{
	//根据加载的数据 网络请求
	if (App::GetInstance()->m_me)
	{
		//检查vip
		App::httpCheckVIP(App::GetInstance()->m_me->id);
		//获取孩子信息
		getChildList(App::GetInstance()->m_me->id, []() {
			auto ShowChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
			if (ShowChildID <= 0)
			{
				auto childrenId = YYXStruct::getMapInt64(App::GetInstance()->myData, "firstChildID", -999);
				string namekey = StringUtils::format("name+childID=%d", childrenId);
				string birthdaykey = StringUtils::format("birthday+childID=%d", childrenId);
				string childrenName = YYXStruct::getMapString(App::GetInstance()->myData, namekey, "");
				int childrenSex = YYXStruct::getMapInt64(App::GetInstance()->myData, birthdaykey, 0);
				string childrenBirth = YYXStruct::getMapString(App::GetInstance()->myData, birthdaykey, "");
				//赋值
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildName", -999, childrenName);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildSex", childrenSex);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildBirthday", -999, childrenBirth);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", childrenId);
			}
		});
		//获取红包列表
		httpGetUserRedPackets(App::GetInstance()->m_me->id);
		//获取当前用户已购列表
		getUserBuyBook(App::GetInstance()->m_me->id);
	}
}

void Load::initDir()
{
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "temp"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "temp");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "readBookRecord"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "readBookRecord");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "rentBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "rentBook");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "readBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "readBook");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "downloadBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "downloadBook");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "voiceComment"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "voiceComment");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "errorLog"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "errorLog");
}

//获取到充值送红包的活动内容
void Load::httpRedPacketActivity()
{
	YYXLayer::logb("Load::httpRedPacketActivity()");
	string url = string(IP) + NET_GETCHARGEACITIVITY;
	string runkey = "httpRedPacketActivitySuccess";
	NetIntface::httpGet(url, runkey, [](string json) {
		NetIntface::httpGetRechargeActivityCallBack(json, [](int index, int redPacketCount) {
			string redPacketActivityKey = StringUtils::format("RechargeIndex=%d", index);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, redPacketActivityKey, redPacketCount);
		}, []() {}, []() {});
	}, "", [](string str) {});
	YYXLayer::loge("Load::httpRedPacketActivity()");
}

//获取本地保存的设置并根据视力保护时间开始计时
//void Load::getLocalSetup() {
//
//	//本地读取最近下载 最近阅读
//	string downloadPath = FileUtils::getInstance()->getWritablePath() + "temp/download.json";
//	string readPath = FileUtils::getInstance()->getWritablePath() + "temp/read.json";
//	map<string, string> data;
//	App::getMapFromFile(downloadPath, data);
//	App::GetInstance()->bookDownLoad.clear();
//	for (auto it : data)
//	{
//		int d = atoi(it.first.c_str());
//		if (d != 0)
//		{
//			App::GetInstance()->bookDownLoad[d] = atoi(it.second.c_str());
//		}
//	}
//	map<string, string> dat;
//	App::getMapFromFile(readPath, dat);
//	App::GetInstance()->bookRead.clear();
//	for (auto its : dat)
//	{
//		int d = atoi(its.first.c_str());
//		if (d != 0)
//		{
//			App::GetInstance()->bookRead[d] = atoi(its.second.c_str());
//		}
//	}
//
//	//获取视力保护时间
//	string PROTECT_TIMEstr = YYXLayer::getFileValue("PROTECT_TIME", "0");
//	App::GetInstance()->protectTime = atoi(PROTECT_TIMEstr.c_str());
//
//	//视力保护时间计时
//	App::protectedTiming();
//
//
//	//获取是否仅限WiFi下载
//	string result = YYXLayer::getFileValue("IS_ONLY_WIFI", "");
//	if (result =="" || result == "0")
//		App::GetInstance()->isOnlyWifi = true;
//	else 
//		App::GetInstance()->isOnlyWifi = false;
//	string userIdstr = YYXLayer::getFileValue("userId", "-999");
//	string userBalancestr = YYXLayer::getFileValue("userBalance", "0");
//	if (App::GetInstance()->m_me == nullptr)
//		App::GetInstance()->m_me = new MyAccount();
//	App::GetInstance()->m_me->id = atoi(userIdstr.c_str());
//	App::GetInstance()->m_me->momey = atoi(userBalancestr.c_str());
//	string IS_ONLY_WIFIstr = YYXLayer::getFileValue("IS_ONLY_WIFI", "");
//	if (IS_ONLY_WIFIstr != "")
//	{
//		if (IS_ONLY_WIFIstr == "1")
//			App::GetInstance()->isOnlyWifi = false;
//		else
//			App::GetInstance()->isOnlyWifi = true;
//	}
//	string userAccount = YYXLayer::getFileValue("userAccount", "");
//	if (userAccount != "")	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userAccount", -999, userAccount);
//	auto userPassword = YYXLayer::getFileValue("userPassword", "");
//	if (userPassword != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userPassword", -999, userPassword);
//	auto userSexstr = YYXLayer::getFileValue("userSex", "");
//	if (userSexstr != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userSex", atoi(userSexstr.c_str()));
//	auto userCity = YYXLayer::getFileValue("userCity", "");
//	if (userCity != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userCity", -999, userCity);
//	auto userProvince = YYXLayer::getFileValue("userProvince", "");
//	if (userProvince != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userProvince", -999, userProvince);
//	string ShowChildIDstr = YYXLayer::getFileValue("ShowChildID", "");
//	if (ShowChildIDstr != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", atoi(ShowChildIDstr.c_str()));
//	string ShowChildHeadPortrait = YYXLayer::getFileValue("ShowChildHeadPortrait", "");
//	if (ShowChildHeadPortrait != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, ShowChildHeadPortrait);
//	string ShowChildName = YYXLayer::getFileValue("ShowChildName", "");
//	if (ShowChildName != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildName", -999, ShowChildName);
//	string ShowChildSexstr = YYXLayer::getFileValue("ShowChildSex", "");
//	if (ShowChildSexstr != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildSex", atoi(ShowChildSexstr.c_str()));
//	string ShowChildBirthday = YYXLayer::getFileValue("ShowChildBirthday", "");
//	if (ShowChildBirthday != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildBirthday", -999, ShowChildBirthday);
//	getShowIDandShowHeadPortrait();
//
//}

////自动登录
//void Load::defaultLogin(string userAccount ,string userPassword) {
//	/*本地信息在自动登录时全部一次性加载进内存,不再在场景中再选择加载
//	//从本地取出最近一次登陆的用户信息
//	userAccount 账号
//		userPassword 密码
//		userId 用户ID
//		userSex 用户性别
//		userCity 用户城市
//		userProvince 用户省份
//		ShowChildID 选中的孩子ID
//		ShowChildHeadPortrait 选中孩子的头像
//		ShowChildName 选中的孩子名字
//		ShowChildSex 选中的孩子性别
//		ShowChildBirthday 选中的孩子生日*/
//
//	//验证登陆
//	if (userAccount == "" || userPassword == "")
//		return;
//	NetIntface::httpLogIn(userAccount, userPassword, "httpLogInSuccess", [=](string json) {
//		NetIntface::httpLogInCallBack(json, [=](int memberID, int memberSex, int memberGrade, string memberCity, string memberProvince) {
//			//获取红包列表
//			httpGetUserRedPackets(memberID);
//			//获取当前用户已购列表
//			getUserBuyBook(memberID);
//			//登陆成功
//			if (App::GetInstance()->m_me == nullptr)
//				App::GetInstance()->m_me = new MyAccount();
//			App::GetInstance()->m_me->id = memberID;
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userAccount", -999, userAccount);
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userSex", memberSex);
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userCity", -999, memberCity);
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userProvince", -999, memberProvince);
//			//用户信息存储本地
//			YYXLayer::setFileValue("userAccount", userAccount);
//			YYXLayer::setFileValue("userPassword", userPassword);
//			YYXLayer::setFileValue("userId", StringUtils::format("%d", memberID));
//			YYXLayer::setFileValue("userSex", StringUtils::format("%d", memberSex));
//			YYXLayer::setFileValue("userCity", memberCity);
//			YYXLayer::setFileValue("userProvince", memberProvince);
//			//查询vip
//			App::httpCheckVIP(memberID);
//			//孩子ID和头像
//			getShowIDandShowHeadPortrait();
//		}, [](string str) {});
//	}, "", [](string st) {});
//}

//验证登陆
/*void Load::verification(std::string account, std::string password)
{
	////网络验证登陆
	//HttpRequest* request = new HttpRequest();
	//request->setRequestType(HttpRequest::Type::POST);
	//request->setUrl(std::string(IP).append(NET_LOGIN).c_str());
	//CCLOG("%s", std::string(IP).append(NET_LOGIN).c_str());
	//auto postData = std::string("username=");
	//postData = postData.append(account).append("&password=").append(password)
	//	.append("&resource=").append(App::m_resource);
	//CCLOG("%s", postData.c_str());
	//request->setRequestData(postData.c_str(), strlen(postData.c_str()));
	////请求的回调函数
	//request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
	//	if (!response){
	//		Load::notifyLoginFailed();
	//		CCLOG("22222222222");
	//		return;
	//	}
	//	if (!response->isSucceed()){
	//		Load::notifyLoginFailed();
	//		CCLOG("3333333333");
	//		log("response failed! error buffer: %s", response->getErrorBuffer());
	//		return;
	//	}
	//	//json解析
	//	std::vector<char> *buffer = response->getResponseData();
	//	std::string str(buffer->begin(), buffer->end());
	//	thread callback(verficationCallback, str, password);
	//	callback.detach();
	//});
	//cocos2d::network::HttpClient::getInstance()->send(request);
	//cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(5);
	//if (App::m_RunningScene == LoginScene)
	//	Toast::create(App::getString("MESAAGEBOX_IS_LOGINING"));
	//request->release();
	App::GetInstance()->m_acount->memberName = account;
	App::GetInstance()->m_acount->password = password;
	if (!CocosAndroidJni::IsNetConnect())
	{
		Toast::create(App::getString("NETEXCEPTION"));
		notifyLoginFailed();
		return;
	}
	App::log(StringUtils::format ("CocosAndroidJni::AccountLogin(account.c_str() = %s, password.c_str() = %s);", account.c_str(), password.c_str()));
	CocosAndroidJni::AccountLogin(account.c_str(), password.c_str());
}*/

//java验证登录回调 主线程
/*
void Load::verficationCallback(std::string str) {
	App::log("验证登录回调");
	rapidjson::Document doc;
	doc.Parse<0>(str.c_str());
	CocosAndroidJni::Log("show", str.c_str());
	if (doc.HasParseError()) {
		CocosAndroidJni::Log("show","rapidjson::doc.HasParseError");
		notifyLoginFailed();
		return;
	}
	if (!doc.IsObject() || !doc.HasMember("result") || !doc.HasMember("data")) {
		CocosAndroidJni::Log("show", "doc is not object");
		notifyLoginFailed();
		return;
	}
	rapidjson::Value &value = doc["result"];
	if (!value.IsBool() || !value.GetBool()) {
		if (doc.HasMember("errorMessage"))
		{
			rapidjson::Value &errorMessage = doc["errorMessage"];
			if (errorMessage.IsString())
			{
				auto str = errorMessage.GetString();
				App::log(str);
				Toast::create(str);
			}
		}
		CocosAndroidJni::Log("show", "errorMessage");
		notifyLoginFailed();
		return;
	}
	//登陆成功
	rapidjson::Value &data = doc["data"];
	if (App::GetInstance()->m_me == nullptr)
		App::GetInstance()->m_me = new MyAccount();

	App::GetInstance()->m_me->id = App::analysisJsonInt(data, "memberId");
	App::GetInstance()->m_me->sex = App::analysisJsonInt(data, "memberSex");
	App::GetInstance()->m_me->memberProvince = App::analysisJsonString(data, "memberProvince");
	App::GetInstance()->m_me->memberCity = App::analysisJsonString(data, "memberCity");

	//用户信息存储本地
	UserDefault::getInstance()->setIntegerForKey("userId", App::GetInstance()->m_me->id);
	UserDefault::getInstance()->setStringForKey("userName", App::GetInstance()->m_acount->memberName);
	UserDefault::getInstance()->setStringForKey("password", App::GetInstance()->m_acount->password);
	UserDefault::getInstance()->flush();
	//获取用户详情
	Load::getUserDetails(App::GetInstance()->m_acount->password);
	//获取孩子详情
	Load::getChildDetails();
	NetIntface::httpGetChildDetails(App::GetInstance()->m_me->id, "IndexScenehttpGetChildDetailsSuccess", "IndexScenehttpGetChildDetailsFail");
	//获取当前用户已购列表
	Load::getBuyBook();	
}
*/
//获取用户详情
/*void Load::getUserDetails(std::string pswd) {
	if (App::GetInstance()->m_me == nullptr)//用户未登录
		return;

	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::GET);

	auto strUrl = std::string(IP).append(NET_USERDETAILS).append(App::getMemberID());
	//CCLOG(strUrl.c_str());
	request->setUrl(strUrl.c_str());
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {//请求回调
		if (!response)
			return;
		log("response code:  %d", response->getResponseCode());
		if (!response->isSucceed()) {
			log("connect failed!  error buffer: %s", response->getErrorBuffer());
			return;
		}
		//json解析
		std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
		std::thread callback(getUserDetailsCallback, str, pswd);
		callback.detach();
	});

	HttpClient::getInstance()->send(request);
	request->release();
}*/

//获取用户详情回调
/*void Load::getUserDetailsCallback(std::string str, std::string pswd) {
	rapidjson::Document doc;
	doc.Parse<0>(str.c_str());
	if (doc.HasParseError())
		return;

	if (!doc.IsObject() || !doc.HasMember("errorMessage") || !doc.HasMember("data"))
		return;

	rapidjson::Value &data = doc["data"];

	//更新数据库
	sqlite3* myDB = SqliteManager::OpenDataBase();
	unordered_map<string, string> paraInfoSTR;
	unordered_map<string, long long> paraInfoINT;
	paraInfoINT["status"] = 0;
	//用户信息表里的每条记录状态置为0
	SqliteManager::UpdateData(myDB, DB_USERINFO, paraInfoSTR, paraInfoINT, "where status=1");

	//当前用户信息插入
	paraInfoINT["member_id"] = App::analysisJsonInt(data, "memberId");
	paraInfoINT["member_sex"] = App::analysisJsonInt(data, "memberSex");
	paraInfoINT["status"] = 1;
	paraInfoINT["upTime"] = App::getCurrentTime();
	paraInfoSTR["member_name"] = App::analysisJsonString(data, "memberName");
	App::GetInstance()->m_me->memberName = App::analysisJsonString(data, "memberName");
	paraInfoSTR["member_passwd"] = pswd;
	paraInfoSTR["member_avatar"] = App::analysisJsonString(data, "memberAvatar");
	paraInfoSTR["memberProvince"] = App::GetInstance()->m_me->memberProvince;
	paraInfoSTR["memberCity"] = App::GetInstance()->m_me->memberCity;
	paraInfoSTR["member_email"] = App::analysisJsonString(data, "memberEmail");
	paraInfoSTR["member_qq"] = App::analysisJsonString(data, "memberQq");
	paraInfoSTR["member_weixin"] = "";
	SqliteManager::DeleteData(myDB, DB_USERINFO, StringUtils::format("where member_id=%d", App::analysisJsonInt(data, "memberId")));
	SqliteManager::InsertData(myDB, DB_USERINFO, paraInfoSTR, paraInfoINT);
	SqliteManager::CloseDataBase(myDB);

	//线程安全-通知已更新用户详情表
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
		EventCustom eventRefreshUserInfo("REFRESH_USER_INFO_OVER");
		Director::getInstance()->getEventDispatcher()->setEnabled(true);
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventRefreshUserInfo);
	});
}*/

//网络获取已购书籍列表
//void Load::getBuyBook() {
//	if (App::GetInstance()->m_me == NULL) {
//		Toast::create(App::getString("STR_YOUDONTLOGIN"));
//		notifyLoginFailed();
//		return;
//	}
//	CocosAndroidJni::GetBuyBook(App::GetInstance()->m_me->id);
//	//HttpRequest* request = new HttpRequest();
//	//request->setRequestType(HttpRequest::Type::GET);
//	//auto strUrl = std::string(IP).append(NET_BOOKROOMBOOKLIST);
//	//strUrl = strUrl.append("memberId=").append(App::getMemberID()).append("&page=1&pagesize=1000&resource=").append(App::m_resource);
//	//request->setUrl(strUrl.c_str());
//	//request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {//请求回调
//	//	if (!response) {
//	//		Toast::create(App::getString("MESSAGEBOX_NETCONNECTERROR"));
//	//		return;
//	//	}
//	//	if (!response->isSucceed()) {
//	//		Toast::create(App::getString("MESSAGEBOX_NETCONNECTERROR"));
//	//		log("connect failed!  error buffer: %s", response->getErrorBuffer());
//	//		return;
//	//	}
//	//	//json解析
//	//	std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
//	//	std::thread callback(buyBookCallback, str);
//	//	callback.detach();
//	//});
//	//HttpClient::getInstance()->send(request);
//	//request->release();
//}

//网络获取已购书籍列表回调
//void Load::buyBookCallback(std::string str) {
//	rapidjson::Document doc;
//	doc.Parse<0>(str.c_str());
//	if (doc.HasParseError()) {
//		Toast::create(App::getString("MESSAGEBOX_NETCONNECTERROR"));
//		return;
//	}
//
//	if (!doc.IsObject() || !doc.HasMember("code") || !doc.HasMember("data")) {
//		Toast::create(App::getString("MESSAGEBOX_NETCONNECTERROR"));
//		return;
//	}
//
//	rapidjson::Value &value = doc["code"];
//	if (!value.IsInt() || value.GetInt() != 0) {
//		Toast::create(App::getString("MESSAGEBOX_NETCONNECTERROR"));
//		return;
//	}
//	
//	rapidjson::Value &data = doc["data"];
//	if (!data.IsArray()) {
//		Toast::create(App::getString("MESSAGEBOX_NETCONNECTERROR"));
//		return;
//	}
//
//	//获取已购列表成功
//	//清空当前用户已购书籍列表
//	//sqlite3* myDB = SqliteManager::OpenDataBase();
//	//SqliteManager::DeleteData(myDB, DB_USERBOOKLIST, "where member_id=" + App::getMemberID());
//	////网络请求的已购列表插入本地数据库
//	//for (rapidjson::SizeType i = 0; i < data.Size(); i++) {
//	//	rapidjson::Value &pData = data[i];
//	//	unordered_map<string, string> paraInfoSTR;
//	//	unordered_map<string, long long> paraInfoINT;
//	//	paraInfoINT["member_id"] = App::GetInstance()->m_me->id;
//	//	paraInfoINT["bookId"] = App::analysisJsonInt(pData, "bookId");
//	//	paraInfoINT["orderId"] = App::analysisJsonInt(pData, "orderId");
//	//	paraInfoINT["upTime"] = App::getCurrentTime();
//	//	paraInfoSTR["bookCoverUrl"] = App::analysisJsonString(pData, "bookCoverUrl");
//	//	paraInfoSTR["bookPlayUrl"] = App::analysisJsonString(pData, "bookPlayUrl");
//	//	SqliteManager::InsertData(myDB, DB_USERBOOKLIST, paraInfoSTR, paraInfoINT);
//	//}
//	//SqliteManager::CloseDataBase(myDB);
//
//	//通知登录成功
//	Load::notifyLoginSuccess();
//
//	//线程安全-通知书房刷新书架
//	Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
//		EventCustom eventRefreshBooks("REFRESH_BOOKS");
//		Director::getInstance()->getEventDispatcher()->setEnabled(true);
//		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventRefreshBooks);
//	});
//}

//网络获取孩子详情
/*void Load::getChildDetails() {
	if (App::GetInstance()->m_me == nullptr)//用户未登录
	{
		notifyLoginFailed();
		return;
	}
	CocosAndroidJni::Log("show", StringUtils::format("getChildDetails(%d)", App::GetInstance()->m_me->id).c_str());
	CocosAndroidJni::getChildDetails(App::GetInstance()->m_me->id);
	//HttpRequest* request = new HttpRequest();
	//request->setRequestType(HttpRequest::Type::POST);
	//request->setUrl(std::string(IP).append(NET_CHILD_DETAILS).c_str());
	//std::string postData = "memberId=" + App::getMemberID();

	//request->setRequestData(postData.c_str(), strlen(postData.c_str()));

	////请求的回调函数
	//request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
	//	if (!response){
	//		return;
	//	}
	//	if (!response->isSucceed()){
	//		log("response failed");
	//		log("error buffer: %s", response->getErrorBuffer());
	//		return;
	//	}

	//	//json解析
	//	std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
	//	std::thread callback(getChildDetailsCallback, str);
	//	callback.detach();
	//});
	//cocos2d::network::HttpClient::getInstance()->send(request);
	//request->release();
}*/

////通知登录成功
//void Load::notifyLoginSuccess() {
//	CCLOG("this is login success");
//	Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
//		Director::getInstance()->getEventDispatcher()->setEnabled(true);
//		EventCustom eventModifySuccess("USER_LOGIN_SUCCESS");
//		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventModifySuccess);
//	});
//}
//
////通知登录失败
//void Load::notifyLoginFailed() {
//	CCLOG("this is login failed");
//	Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
//		Director::getInstance()->getEventDispatcher()->setEnabled(true);
//		EventCustom eventModifyFailed("USER_LOGIN_FAILED");
//		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventModifyFailed);
//	});
//}

//异步加载PNG大图
void Load::loadPlistPng() {
	auto cache = Director::getInstance()->getTextureCache();
	cache->addImageAsync(LOADSCENE_FIND_PNG1, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG2, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG3, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG4, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG5, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG6, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG7, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG8, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG9, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG10, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG11, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG12, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG13, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG14, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG15, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG16, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG17, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG18, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG19, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG20, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG21, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_PNG22, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
	cache->addImageAsync(LOADSCENE_FIND_Toast, CC_CALLBACK_1(Load::loadPlistPngCallback, this));
}

void Load::loadPlistPngCallback(Texture2D* sender) {
	sender->retain();
	currentCount++;
	if (currentCount >= totalCount) {
		App::createDataBase();
		auto cache = SpriteFrameCache::getInstance();
		cache->addSpriteFramesWithFile(PLIST_BACKGROUND);
		cache->addSpriteFramesWithFile(PLIST_INDEX);
		cache->addSpriteFramesWithFile(PLIST_TRAIN);
		Director::getInstance()->replaceScene(Index::createScene());
		return;
	}
}

////网络请求书城的按钮列表
//void Load::httpButtonsInfo()
//{
//	//CocosAndroidJni::GetBookCityButtonsInfo();
//	//HttpRequest* request = new HttpRequest();
//	//request->setRequestType(HttpRequest::Type::GET);
//	//auto strurl = std::string(IP).append(NET_BOOKCITY)
//	//	.append("?page=").append("1")
//	//	.append("&pagesize=").append("1000")
//	//	.append("&resource=").append(App::m_resource)
//	//	.append("&memberId=-999");
//	//request->setUrl(strurl.c_str());
//
//	////请求的回调函数
//	//request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
//	//	if (!response){
//	//		return;
//	//	}
//	//	if (!response->isSucceed())
//	//	{
//	//		CCLOG("response failed! error buffer: %s", response->getErrorBuffer());
//	//		return;
//	//	}
//	//	rapidjson::Document doc;
//	//	std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
//	//	//子线程中继续执行回调函数
//	//	std::thread callback(httpButtonsInfoResponseCallback, str);
//	//	callback.detach();
//	//});
//	//cocos2d::network::HttpClient::getInstance()->send(request);
//	//request->release();
//}

void Load::httpBookCityInfoAndDownLoad()
{
	string url = string(IP).append(NET_BOOKCITY).append("?memberId=").append(App::getMemberID()).append("&page=1&pagesize=1000&resource=").append(App::m_resource);
	string runkey = "LoadScenehttpBookCityInfo";
	NetIntface::httpGet(url, runkey, [=](string json) {
		NetIntface::httpBookCityInfoCallBack(json, [=](int index, int castleId, int castleType, int sort, int storeBorder, int hasNewBook, string castleName, string borderUrl, string bgUrl, string bagUrl) {
			//书城的排序  (ID+名称+storeBorder)
			string sortKey = StringUtils::format("sort=%d+bookcity", index);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, sortKey, castleId, castleName, (Ref*)storeBorder);
			//castleName
			string castleNameKey = StringUtils::format("castleName+castleId=%d", castleId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, castleNameKey, -999, castleName);
			//borderUrl
			string borderUrlKey = StringUtils::format("borderUrl+castleId=%d", castleId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, borderUrlKey, -999, borderUrl);
			//bgUrl
			string bgUrlKey = StringUtils::format("bgUrl+castleId=%d", castleId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bgUrlKey, -999, bgUrl);
			//下载
			string dir = FileUtils::getInstance()->getWritablePath() + "bookCity";
			string borderfileName = StringUtils::format("border_%d", castleId) + ".png";
			string bgfileName = StringUtils::format("bg_%d", castleId) + ".png";
			auto borderpath = dir + "/" + borderfileName;
			auto bgpath = dir + "/" + bgfileName;
			if (!FileUtils::getInstance()->isDirectoryExist(dir))
			{
				FileUtils::getInstance()->createDirectory(dir);
			}
			if (!FileUtils::getInstance()->isFileExist(borderpath))
			{
				NetIntface::DownLoadImage(borderUrl, dir, borderfileName, "", [=](string downPath) {}, "", [=](string str) {});
			}
			else
			{
				m_httpOver = true;
			}
			if (!FileUtils::getInstance()->isFileExist(bgpath))
			{
				NetIntface::DownLoadImage(bgUrl, dir, bgfileName, "", [=](string downPath) {}, "", [=](string str) {});
			}
		}, [](int totalPage) {
			//书店数量
			string totalCountKey = StringUtils::format("BookCityTotalPage");
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, totalCountKey, totalPage);
		}, []() {
		});
	}, "", [](string str) {
	});
	m_httpOver = true;
}

////确定需要展示的头像和孩子ID
//void Load::getShowIDandShowHeadPortrait()
//{
//	//auto member_id = UserDefault::getInstance()->getIntegerForKey("userId", -999);
//	string member_idstr = YYXLayer::getFileValue("userId", "");
//	if (member_idstr != "")
//	{
//		int member_id = atoi(member_idstr.c_str());
//		if (App::GetInstance()->m_me == nullptr)
//			App::GetInstance()->m_me = new MyAccount();
//		App::GetInstance()->m_me->id = member_id;
//		string ShowChildIDstr = YYXLayer::getFileValue("ShowChildID", "");
//		if (ShowChildIDstr != "")
//		{//有孩子ID
//			int ShowChildID = atoi(ShowChildIDstr.c_str());
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", ShowChildID);
//			string ShowChildHeadPortrait = YYXLayer::getFileValue("ShowChildHeadPortrait", "");
//			if (ShowChildHeadPortrait != "" && FileUtils::getInstance()->isFileExist(ShowChildHeadPortrait))
//			{//有孩子ID有头像
//				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, ShowChildHeadPortrait);
//			}
//			else
//			{//有孩子ID无头像
//				string savePath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("childHead_%d.png", ShowChildID);
//				if (FileUtils::getInstance()->isFileExist(savePath))
//				{
//					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, savePath);
//				}
//				getChildList(member_id, []() {});
//			}
//		}
//		else//没孩子ID
//			getDefaultHeadPortrait(member_id);
//	}
//}

void  Load::initUserData()
{
	//setBoolToXML(SOUND_KEY, true);
	//setBoolToXML(MUSIC_KEY, true);
	YYXLayer::setFileValue(SOUND_KEY, "true");
	YYXLayer::setFileValue(MUSIC_KEY, "true");
}

//下载头像
//void Load::downloadChildAvatar(string strUrl) {
//	if (strUrl.length() == 0) {
//		return;
//	}
//
//	//网络请求图片
//	HttpRequest* pRequest = new HttpRequest();
//	pRequest->setUrl(strUrl.c_str());
//	pRequest->setRequestType(HttpRequest::Type::GET);
//	pRequest->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
//		if (!response) {
//			return;
//		}
//		if (!response->isSucceed()) {
//			log("connect failed!  error buffer: %s", response->getErrorBuffer());
//			return;
//		}
//
//		std::vector<char> *buffer = response->getResponseData();
//		char* buf = (char*)malloc(buffer->size());
//		std::copy(buffer->begin(), buffer->end(), buf);
//		string str;
//		CocosAndroidJni::GetPhotoPath(str);
//		auto filePath = str+"/" + PRICTURE_TAKEPHOTO_NAME + App::getMemberID() + ".png";
//		FILE *fp = fopen(filePath.c_str(), "wb+");
//		fwrite(buf, 1, buffer->size(), fp);
//		fclose(fp);
//		App::GetInstance()->m_photsName = PRICTURE_TAKEPHOTO_NAME + App::getMemberID() + ".png";
//		CocosAndroidJni::CompressPictureByTakePhoto(App::GetInstance()->m_photsName.c_str());
//		App::log("下载头像,处理头像");
//		unordered_map<string, string> paraInfoSTR;
//		unordered_map<string, long long> paraInfoINT;
//		paraInfoINT["bookId"] = App::GetInstance()->m_me->id;
//		paraInfoSTR["path"] = filePath;
//		paraInfoINT["status"] = 8;
//		paraInfoINT["downloadtime"] = App::getCurrentTime();
//		paraInfoINT["upTime"] = App::getCurrentTime();
//		SqliteManager::InsertData(App::sqliteOpen(), DB_RES, paraInfoSTR, paraInfoINT);
//		if (buf != nullptr)
//			free(buf);
//	});
//	HttpClient::getInstance()->send(pRequest);
//	pRequest->release();
//}

void Load::loadCsbFile()
{
	auto app = App::GetInstance();
	string strs[] = { LOCOMOTIVE_CSB,			BOOKCITY_CSB,		PICTUREBOOK_CSB,		RECOMMEND_CSB,		LIMITFREE_CSB,
		EXCELLENT_FIVE_STAR_CSB,		NEWBOOK_CSB,		KANGXUANBOOKSTORE_CSB,		PARENTMESSAGEBOX_CSB,
		PARENT_BACKGROUND_CSB,		ACCOUNT_CSB,		CONTACT_CSB ,		SET_CSB ,		LOGIN_CSB ,		MESSAGEBOX_LOGOFF_YESORNO_CSB ,
		MESSAGEBOX_CHANGEPASSWORD_CSB ,		MESSAGEBOX_WIPECACHE_CSB ,		MESSAGEBOX_FEEDBACK_CSB ,		LOGINMESSAGEBOX_CSB ,
		FORGETPASSWORDMESSAGEBOX_CSB ,		REGISTERMESSAGEBOX_CSB ,		ADDCHILD_CSB ,		BOOKROOM_CSB ,		BOOK_CSB ,BOOKSTORE_PAGE_CSB ,
		BOOKSTORE_BACKGROUND_CSB ,		BOOKSTORE_BOOKVIEW_CSB ,		BOOKINFO_BOOKINFO_CSB ,		BOOKINFO_JIANJIE_CSB ,		BOOKINFO_PINGLUN_CSB,
		BOOKINFO_SCORE_CSB ,		BABYCENTER_CENTERMAIN_CSB ,		BABYCENTER_TIPLOGIN_CSB ,			BABYCENTER_PHOTOSHOW_CSB ,
		BABYCENTER_BOOKCITY_CSB ,		BABYCENTER_DOUBLEBUTTON_CSB ,		BABYCENTER_BIGBUTTON_CSB ,		BABY_RESTREMIND_CSB ,
		WAITMESSAGEBOX_CSB};
	for (auto it : strs)
	{
		Data data;
		if(!app->addData(it, data))
			App::log(it + "----csb文件缓存失败");
		//auto data = FileUtils::getInstance()->getDataFromFile(it);
		//if (data.getSize()> 0)
		//{
		//	app->CsbCache[it] = data;
		//}
		//else
		//{
		//	App::log(it+"csb文件缓存失败");
		//}
	}	
}

//网络请求消息推送
void Load::getEllaNotification() {
	App::log("-------------getellanotification");
	string url = std::string(IP).append(NET_NOTIFICATION);
	map<string, string> paramter;
	paramter["memberId"] = App::getMemberID();
	paramter["resourceType"] = "0";
	paramter["resource"] = App::m_resource;
	NetIntface::httpPost(url, paramter, "getEllaNotificationSuccess", [](string json) {
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			string desc = YYXLayer::getStringForJson("", doc, "desc");
			if (desc == "success")
			{
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushId", YYXLayer::getIntForJson(-999, doc, "pushId"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushType", YYXLayer::getIntForJson(-999, doc, "pushType"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "imageUrl", -999, YYXLayer::getStringForJson("", doc, "imageUrl"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushUrl", -999, YYXLayer::getStringForJson("", doc, "pushUrl"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushTitle", -999, YYXLayer::getStringForJson("", doc, "pushTitle"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushString", -999, YYXLayer::getStringForJson("", doc, "pushString"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushVersion", -999, YYXLayer::getStringForJson("", doc, "pushVersion"));
			}
		}
	}, "", [](string str) {});
	////测试
	//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushId", 10);
	//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushType", 2);
	//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "imageUrl", -999, "http://b.hiphotos.baidu.com/image/h%3D200/sign=22ca0df79b13b07ea2bd57083cd69113/5fdf8db1cb134954d01b62b25e4e9258d0094adb.jpg");
	//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushUrl", -999, "www.baidu.com");
	//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushTitle", -999, "Title");
	//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushString", -999, App::getString("YOUARENOTVIP"));
	//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushVersion", -999, "1.9.0");
}

//网络请求版本号
void Load::getEllaVersion() {
	YYXLayer::logb("Load::getEllaVersion()");
	string url = string(IP).append(NET_ELLAVERSION);
	map<string, string> parameter;
	parameter["versionResource"] = "android";
	NetIntface::httpPost(url, parameter, "getEllaVersionSuccess", [](string json) {
		NetIntface::httpAppVersionCallBack(json, [](string versionNum) {
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
		}, [](string str) {
			YYXLayer::loge("Get version number exception");
		});
	}, "getEllaVersionFail", [](string str) {
		YYXLayer::loge("Get version number error");
	});
	YYXLayer::loge("Load::getEllaVersion()");
	/*{
	"code": 0,
	"data": {
		"id": 79,
		"versionResource": "android",
		"versionNum": "1.3.3",
		"versionAddtime": "2016-09-20 19:34:30.0",
		"versionFilepath": "ellabook-public",
		"versionFilename": "http://ellabook-public.oss-cn-hangzhou.aliyuncs.com/android/ellabook-latest.apk?Expires=1476674001&OSSAccessKeyId=wCSYOmYFgJmttmWs&Signature=r42BfQkNspfPGTamsXqXzqXY4LA%3D",
		"versionDescription": "增加新功能\n1、丰富了红包功能，包括扫码注册送红包、注册送红包、充值送红包、分享送红包。\n2、增加成功领取红包后的弹窗提醒。\n3、在图书详情页增加了红包抵扣说明。\n4、修复部分BUG。"
	},
	"desc": "success"
}*/
	/*HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_ELLAVERSION).c_str());
	auto postData = std::string("versionResource=android");
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));
	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		if (!response) {
			return;
		}
		if (!response->isSucceed()) {
			return;
		}
		//json解析
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		rapidjson::Document doc;
		doc.Parse<0>(str.c_str());

		if (doc.HasParseError()) {
			return;
		}
		if (!doc.IsObject() || !doc.HasMember("desc") || !doc.HasMember("data")) {
			return;
		}
		rapidjson::Value &value = doc["desc"];
		if (!value.IsString() || strcmp(value.GetString(), "success") != 0) {
			CCLOG("----------getEllaVersion error");
			return;
		}

		rapidjson::Value &data = doc["data"];
		//将最新版本号保存至xml文件
		auto numstr = App::analysisJsonString(data, "versionNum");
		if (numstr != "")
		{
			//App::log("获取服务器版本=" + numstr);
			UserDefault::getInstance()->setStringForKey("NetEllaVersion", numstr);
			auto index1 = numstr.find_first_of(".");
			auto index2 = numstr.find_last_of(".");
			int A = atoi(numstr.substr(0, 1).c_str()) *10000;
			int B = atoi(numstr.substr(index1+1, 1).c_str())*1000;
			int C = atoi(numstr.substr(index2+1).c_str());
			//App::log("运算服务器版本=" , A + B + C);
			UserDefault::getInstance()->setIntegerForKey("VersionNumber", A+B+C);
			UserDefault::getInstance()->flush();
		}
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(5);
	request->release();*/
}

//void Load::getDefaultHeadPortrait(int member_id)
//{
//	//无记录情况下,默认获取第一个孩子的头像作为头像
//	auto runkey = "loadSceneHttpGetFirstChildHeadPortraitSuccess";
//	NetIntface::httpGetFirstChildHeadPortrait(member_id, runkey, [](string json) {
//		NetIntface::httpGetFirstChildHeadPortraitCallBack(json, [](int id, string url) {
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", id);
//			YYXLayer::setFileValue("ShowChildID", StringUtils::format("%d", id));
//			NetIntface::DownLoadImage(url, FileUtils::getInstance()->getWritablePath() + "temp", StringUtils::format("2headportrait%d.png", id),
//				StringUtils::format("DownLoadImage%d.png", (int)YYXLayer::getRandom()), [=](string path) {
//				if (path != "")
//				{
//					string savePath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("childHead_%d.png", id);
//					App::makeRoundImage(path, savePath);
//					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, savePath);
//					YYXLayer::setFileValue("ShowChildHeadPortrait", savePath);
//				}
//			}, "", [](string s) {});
//		}, []() {});
//	}, "", [](string s) {});
//}

void Load::getChildList(int member_id, function<void()> downloadHeadPortrait)
{
	auto runkey = StringUtils::format("httpGetChildDetails%d", (int)YYXLayer::getRandom());
	NetIntface::httpGetChildDetails(member_id, runkey, [=](string json) {
		NetIntface::httpGetChildDetailsCallBack(json, [=](int index, int childrenId,int childrenSex, string childrenName, string childrenBirth, string url, long long uptime) {
			if (index == 0)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "firstChildID", childrenId);
			if (index == 1)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "secondChildID", childrenId);
			if (index == 2)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "threeChildID", childrenId);
			string namekey = StringUtils::format("name+childID=%d", childrenId);
			string pathkey = StringUtils::format("path+childID=%d", childrenId);
			string birthdaykey = StringUtils::format("birthday+childID=%d", childrenId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, namekey, childrenId, childrenName, nullptr);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathkey, uptime, "", nullptr);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, birthdaykey, childrenSex, childrenBirth, nullptr);
			string urlkey = StringUtils::format("url+childID=%d", childrenId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, urlkey, -999, url);
			string savePath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("childHead_%d.png", childrenId);
			if (!FileUtils::getInstance()->isFileExist(savePath))
			{
				NetIntface::DownLoadImage(url, FileUtils::getInstance()->getWritablePath() + "temp", StringUtils::format("9HeadPortrait_%d.png", childrenId),
					StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom()), [=](string path) {
					if (path != "" && FileUtils::getInstance()->isFileExist(path))
					{
						string savePath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("childHead_%d.png", childrenId);
						App::makeRoundImage(path, savePath);
						string pathkey = StringUtils::format("path+childID=%d", childrenId);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathkey, YYXLayer::getCurrentTime4Second(), savePath);
						YYXLayer::sendNotify("IndexSceneReferShowHeadPortrait");
					}
				}, "", [](string str) {});
			}
		}, [=](int b) {
			if (b == 1)
			{
				if(downloadHeadPortrait)
					downloadHeadPortrait();
			}
		});
	}, "", [](string stt) {});
}

void Load::getUserBuyBook(int member_id)
{
	NetIntface::httpGetUserBuyBooks(App::GetInstance()->m_me->id, "LoadSceneHttpGetUserBuyBooksSuccess", [=](string json) {
		NetIntface::httpGetUserBuyBooksCallBack(json, []() {
			//json成功, array前执行
			App::GetInstance()->myBuyBookMap.clear();
			App::GetInstance()->myBuyBookVector.clear();
		}, [](int bookId, int orderId, string bookCoverUrl, string bookPlayUrl, string bookName) {
			//解析过程
			if (App::GetInstance()->myBuyBookMap.find(bookId) == App::GetInstance()->myBuyBookMap.end())
			{
				App::GetInstance()->myBuyBookMap[bookId] = bookPlayUrl;
				App::GetInstance()->myBuyBookVector.push_back(bookId);
			}
		}, [=]() {
			//解析成功
			if (App::GetInstance()->m_me)
				YYXLayer::buyBooksJsonWriteFile(App::GetInstance()->myBuyBookVector, App::GetInstance()->m_me->id, FileUtils::getInstance()->getWritablePath() + "books.json");
		}, []() {App::log("httpGetUserBuyBooks 解析错误"); });
	}, "", [](string str) {App::log("httpGetUserBuyBooks net error"); });
}

//获取有效红包
void Load::httpGetUserRedPackets(int memberId)
{
	string url = string(IP).append(NET_USERREDPACKET).append("?memberId=").append(StringUtils::format("%d", memberId));
	string runKey = "LoginSceneHttpGetUserRedPacketSuccess";
	//string errorKey = "BookInfoSceneHttpGetUserRedPacketFail";
	NetIntface::httpGet(url, runKey, [](string json) {
		NetIntface::httpGetUserRedPacketsCallBack(json, []() {
			App::GetInstance()->m_redPacket.clear();
		}, [](int coupon_id, int coupon_amount100, string coupon_expire_time) {
			if (coupon_id != -999 || coupon_amount100 != -99900) {
				map<string, YYXStruct> mapresult;
				YYXStruct::initMapYYXStruct(mapresult, "coupon_id", coupon_id);
				YYXStruct::initMapYYXStruct(mapresult, "coupon_amount", coupon_amount100);
				YYXStruct::initMapYYXStruct(mapresult, "coupon_expire_time", 0, coupon_expire_time);
				App::GetInstance()->m_redPacket.push_back(mapresult);
			}
		}, [](int expiring_coupon_count) {
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "expiring_coupon_count", expiring_coupon_count);
			//YYXLayer::sendNotify("referRedPackets", "BookInfo");
		}, []() {
			//Toast::create(App::getString("HONGBAOHUOQUSHIBAI"));
		});
	}, "", [](string str) {
		//Toast::create(App::getString("NETEXCEPTION"));
	});
}

void Load::loadingLocalFileData()
{
	//载入用户
	string userAccount = YYXLayer::getFileValue("userAccount", "");
	if (userAccount != "")	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userAccount", -999, userAccount);
	auto userPassword = YYXLayer::getFileValue("userPassword", "");
	if (userPassword != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userPassword", -999, userPassword);
	auto userIdstr = YYXLayer::getFileValue("userId", "-999");
	if (userIdstr != "-999")
	{
		int userId = atoi(userIdstr.c_str());
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userId", userId);
		if (!App::GetInstance()->m_me)
			App::GetInstance()->m_me = new MyAccount();
		App::GetInstance()->m_me->id = userId;
		auto userBalancestr = YYXLayer::getFileValue("userBalance", "");
		int money = atoi(userBalancestr.c_str());
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userBalance", money);
		App::GetInstance()->m_me->momey = money;
		auto userSexstr = YYXLayer::getFileValue("userSex", "");
		if (userSexstr != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userSex", atoi(userSexstr.c_str()));
		auto userCity = YYXLayer::getFileValue("userCity", "");
		if (userCity != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userCity", -999, userCity);
		auto userProvince = YYXLayer::getFileValue("userProvince", "");
		if (userProvince != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userProvince", -999, userProvince);
		string ShowChildName = YYXLayer::getFileValue("ShowChildName", "");
		if (ShowChildName != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildName", -999, ShowChildName);
		string ShowChildSexstr = YYXLayer::getFileValue("ShowChildSex", "");
		if (ShowChildSexstr != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildSex", atoi(ShowChildSexstr.c_str()));
		string ShowChildBirthday = YYXLayer::getFileValue("ShowChildBirthday", "");
		if (ShowChildBirthday != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildBirthday", -999, ShowChildBirthday);
		//孩子信息
		string ShowChildIDstr = YYXLayer::getFileValue("ShowChildID", "");
		if (ShowChildIDstr != "")
		{//有孩子ID
			int ShowChildID = atoi(ShowChildIDstr.c_str());
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", ShowChildID);
			string ShowChildHeadPortrait = YYXLayer::getFileValue("ShowChildHeadPortrait", "");
			if (ShowChildHeadPortrait != "" && FileUtils::getInstance()->isFileExist(ShowChildHeadPortrait))
			{//有孩子ID有头像
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, ShowChildHeadPortrait);
			}
			else
			{//有孩子ID无头像
				string savePath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("childHead_%d.png", ShowChildID);
				if (FileUtils::getInstance()->isFileExist(savePath))
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, savePath);
			}
		}
	}	
	//载入租书信息
	App::getLocalRentJson();
	//载入购书信息
	if (!App::GetInstance()->m_me)
		return;
	string bookjsonPath = FileUtils::getInstance()->getWritablePath() + "books.json";
	if (!FileUtils::getInstance()->isFileExist(bookjsonPath))
		return;
	string jsonstr = FileUtils::getInstance()->getStringFromFile(bookjsonPath);
	rapidjson::Document doc;
	if (YYXLayer::getJsonObject4Json(doc, jsonstr))
	{
		auto memberId = YYXLayer::getIntForJson(0, doc, "memberId");
		if (memberId > 0 && memberId == App::GetInstance()->m_me->id)
		{
			App::GetInstance()->myBuyBookMap.clear();
			App::GetInstance()->myBuyBookVector.clear();
			rapidjson::Value arrayData;
			YYXLayer::getJsonArray4Json(arrayData, doc, "books");
			YYXLayer::getDataForJsonArray(arrayData, [](rapidjson::Value & item, int index) {
				auto bookid = item.GetInt();
				App::GetInstance()->myBuyBookMap[bookid] = "";
				App::GetInstance()->myBuyBookVector.push_back(bookid);
			});
		}
	}
	//本地读取最近下载 最近阅读
	string rfilename = "readBook/download_" + userIdstr + ".json";
	string dfilename = "downloadBook/download_" + userIdstr + ".json";
	string downloadPath = FileUtils::getInstance()->getWritablePath() + dfilename;
	string readPath = FileUtils::getInstance()->getWritablePath() + rfilename;
	map<string, string> data;
	App::getMapFromFile(downloadPath, data);
	App::GetInstance()->bookDownLoad.clear();
	for (auto it : data)
	{
		int d = atoi(it.first.c_str());
		if (d != 0)
			App::GetInstance()->bookDownLoad[d] = atoi(it.second.c_str());
	}
	map<string, string> dat;
	App::getMapFromFile(readPath, dat);
	App::GetInstance()->bookRead.clear();
	for (auto its : dat)
	{
		int d = atoi(its.first.c_str());
		if (d != 0)
			App::GetInstance()->bookRead[d] = atoi(its.second.c_str());
	}
	//是否仅WiFi下载
	string result = YYXLayer::getFileValue("IS_ONLY_WIFI", "");
	if (result == "" || result == "0")
		App::GetInstance()->isOnlyWifi = true;
	else
		App::GetInstance()->isOnlyWifi = false;
	//获取视力保护时间
	string PROTECT_TIMEstr = YYXLayer::getFileValue("PROTECT_TIME", "0");
	App::GetInstance()->protectTime = atoi(PROTECT_TIMEstr.c_str());
}