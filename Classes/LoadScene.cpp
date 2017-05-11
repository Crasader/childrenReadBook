#include "LoadScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "CrossPlatform.h"
#include "YYXLayer.h"
#include "YYXTableView.h"
#include "YYXVisitor.h"
#include "Charger.h"
#include "YYXDownloadImages.h"
#include "DownloadBook.h"
#include "ReadBook.h"
#include "MyBook.h"
#include "YYXSound.h"
#include "BookCache.h"
#include "User.h"
#include "AppHttp.h"
#include "BookStoreCache.h"
USING_NS_CC;

using namespace cocostudio::timeline;

Load::~Load()
{
	ControlScene::getInstance()->end();
}

Scene* Load::createScene(SceneInfo* data)
{
    auto scene = Scene::create();
    auto layer = Load::create(data);
	layer->setTag(9);
    scene->addChild(layer);
    return scene;
}

Load* Load::create(SceneInfo* data)
{
	Load *pRet = new(std::nothrow) Load();
	if (pRet && pRet->init(data))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

// on "init" you need to initialize your instance
bool Load::init(SceneInfo* sceneInfo)
{
    if ( !Layer::init() )
    {
		return false;
    }
	App::GetInstance();
	totalCount = 23;
	currentCount = 0;
	Size m_Size = Director::getInstance()->getVisibleSize();
	addChild(LayerColor::create(Color4B::WHITE));
	auto bg = Sprite::create("kaichanglogo.png");
	bg->setPosition(Vec2(m_Size.width/2, m_Size.height/2));
	addChild(bg);
	//app打开时间
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "APPOpenTime", YYXLayer::getCurrentTime4Second());
	App::GetInstance()->manageThread();
	//游客
	YYXVisitor::getInstance()->loadSceneInit();
	//本机信息
	getPhoneInfo();
	initEvent();
	scheduleOnce([](float t) {
		YYXLayer::sendNotify("LoadSceneOverGoToIndex");
	}, 2, "LoadSceneOverGoToIndexscheduleOnce");
    return true;
}

void Load::initEvent()
{
	auto listener1 = EventListenerCustom::create("LoadSceneOverGoToIndex", [=](EventCustom* e) {
		//加载完成 && (书城网络请求结束 && 网络超时5秒)
		ControlScene::getInstance()->replaceScene(
			ControlScene::getInstance()->getSceneInfo(MySceneName::LoadScene),
			ControlScene::getInstance()->getSceneInfo(MySceneName::IndexScene));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener1, this);
}

void Load::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	initDir();
	initData();
	initMemberHttp();
	App::protectedTiming();
	thread mythread([]() {
		YYXLayer::CopyDirectory(FileUtils::getInstance()->getWritablePath()+"unzip", FileUtils::getInstance()->getWritablePath()+"bookUNZip");
	});
	mythread.detach();
	initHttp();
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "indexAnimator", 0);
}

void Load::initData()
{
	//加载本地数据
	thread mythread([]() {	App::preLoad(); });
	mythread.detach();
	//初始化Toast
	Toast::GetInstance()->showToast();
	YYXSound::getInstance()->init();
	YYXSound::getInstance()->playBackGroundMusic();
	//加载plist大图
	//loadPlistPng();
	//缓存csb文件
	loadCsbFile();
   //下载初始化
	YYXDownload::getInstance()->setDownloadMaxCount(3);
}

void Load::initHttp()
{
	//上传阅读记录
	App::searchReadRecordJson();
	//上传错误日志
	//App::upLoadingErrorLog();
	//红包活动
	AppHttp::getInstance()->httpChargerInfo();
	//请求最新版本号
	AppHttp::getInstance()->httpAppVersion();
	//请求消息推送
	AppHttp::getInstance()->httpNotification();
	//if (App::m_debug == 0)
		//App::upLogFiles();
}

void Load::initMemberHttp()
{
	if (User::getInstance()->getMemberId() < 0)
	{
		YYXVisitor::getInstance()->loginVisitor();
	}
	else
	{		//请求大量的用户信息
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
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "bookCity"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "bookCity");
	DownloadBook::getInstance()->initDir();
	ReadBook::getInstance()->initDir();
	MyBook::getInstance()->initDir();
	BookCache::getInstance()->initDir();
	YYXVisitor::getInstance()->initDir();
	User::getInstance()->initDir();
	BookStoreCache::getInstance()->initDir();
}

void Load::cleanup()
{
	unschedule("LoadSceneOverGoToIndexscheduleOnce");
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

void Load::getPhoneInfo()
{
	//获取机型
	App::GetInstance()->phoneModel = CrossPlatform::getPhoneModel(0) ;
	//android版本
	App::GetInstance()->systemVersion = CrossPlatform::getPhoneModel(2);
}