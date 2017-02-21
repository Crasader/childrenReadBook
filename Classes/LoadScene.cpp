#include "LoadScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "NetIntface.h"
#include "YYXLayer.h"
#include "YYXTableView.h"
#include "YYXVisitor.h"
#include "Charger.h"
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
	addChild(LayerColor::create(Color4B::WHITE));
	auto bg = Sprite::create("kaichanglogo.png");
	bg->setPosition(Vec2(m_Size.width/2, m_Size.height/2));
	addChild(bg);
	//app打开时间
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "APPOpenTime", YYXLayer::getCurrentTime4Second());
	//游客
	YYXVisitor::getInstance()->loadSceneInit();
	//本机信息
	getPhoneInfo();
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
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "indexAnimator", 0);
}

void Load::initData()
{
	//加载本地数据
	App::preLoad();
	//初始化Toast
	Toast::GetInstance()->showToast();
	// 声音初始化数据
	if (YYXLayer::getBoolFromXML(MUSIC_KEY))
		App::GetInstance()->isMusicPlay = true;
	else
		App::GetInstance()->isMusicPlay = false;
	if (YYXLayer::getBoolFromXML(SOUND_KEY))
		App::GetInstance()->isSoundEffect = true;
	else
		App::GetInstance()->isSoundEffect = false;
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
	App::upLoadingErrorLog();
	//书城
	httpBookCityInfoAndDownLoad();
	//红包活动
	Charger::httpChargerInfo();
	//请求最新版本号
	getEllaVersion();
	//请求消息推送
	getEllaNotification();
	if (App::m_debug == 0)
		App::upLogFiles();
}

void Load::initMemberHttp()
{
	//根据加载的数据 网络请求
	if (App::GetInstance()->m_me)
	{
		//请求大量的用户信息
		App::loginCallback(false);
	}
}

void Load::initDir()
{
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "temp"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "temp");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "readBookRecord"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "readBookRecord");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "vipBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "vipBook");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "buyBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "buyBook");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "deleteBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "deleteBook");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "collectBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "collectBook");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "voiceComment"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "voiceComment");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "errorLog"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "errorLog"); 
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "temp/Log"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "temp/Log");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "collectBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "collectBook");
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "downloadBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "downloadBook");
}

void Load::cleanup()
{
	unscheduleAllSelectors();
}

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
		//App::createDataBase();
		//auto cache = SpriteFrameCache::getInstance();
		//cache->addSpriteFramesWithFile(PLIST_BACKGROUND);
		//cache->addSpriteFramesWithFile(PLIST_INDEX);
		//cache->addSpriteFramesWithFile(PLIST_TRAIN);
		//Director::getInstance()->replaceScene(Index::createScene());
		//return;
		YYXLayer::sendNotify("NOTIFY_LOAD_PNG_OVER");
	}
}


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
			if (!FileUtils::getInstance()->isFileExist(bgpath))
			{
				NetIntface::DownLoadImage(bgUrl, dir, bgfileName, "", [=](string downPath) {}, "", [=](string str) {});
			}
		}, [=](int totalPage) {
			//书店数量
			string totalCountKey = StringUtils::format("BookCityTotalPage");
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, totalCountKey, totalPage);
			m_httpOver = true;
		}, [=]() {
			m_httpOver = true;	
		});
	}, "", [=](string str) {
		m_httpOver = true;
	});	
}

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
				auto doingFunction = [&](string key, string typeName) {
					if (typeName =="INT")
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
						if(str != "")
							YYXLayer::setFileValue(key, str);
					}
				};
				doingFunction("pushId", "INT");
				doingFunction("pushType", "INT");
				doingFunction("imageUrl", "STRING");
				doingFunction("pushUrl", "STRING");
				doingFunction("pushTitle", "STRING");
				doingFunction("pushString", "STRING");
				doingFunction("pushVersion", "STRING");/*
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushId", YYXLayer::getIntForJson(-999, doc, "pushId"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushType", YYXLayer::getIntForJson(-999, doc, "pushType"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "imageUrl", -999, YYXLayer::getStringForJson("", doc, "imageUrl"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushUrl", -999, YYXLayer::getStringForJson("", doc, "pushUrl"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushTitle", -999, YYXLayer::getStringForJson("", doc, "pushTitle"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushString", -999, YYXLayer::getStringForJson("", doc, "pushString"));
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "pushVersion", -999, YYXLayer::getStringForJson("", doc, "pushVersion"));*/
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
}

void Load::getPhoneInfo()
{
	//获取机型
	App::GetInstance()->phoneModel = NetIntface::getPhoneModel(0) ;
	//android版本
	App::GetInstance()->systemVersion = NetIntface::getPhoneModel(2);
}