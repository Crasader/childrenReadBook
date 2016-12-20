#include "BookCityScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "extensions/cocos-ext.h"
#include "cocostudio/CocoStudio.h"
#include "YYXStruct.h"
#include "NetIntface.h"

using namespace cocostudio::timeline;

bool BookCityScene::m_isMoved = false;

//threadQueue* BookCityScene::m_queue=nullptr;//网络请求队列管理

BookCityScene::BookCityScene() {
	//m_queue = new threadQueue();
	//m_queue->SetThread_cnt(10);
	//m_queue->start();
}

BookCityScene::~BookCityScene(){
	//if (m_queue)
		//m_queue->stop();
}

Scene* BookCityScene::createScene()
{
    auto scene = Scene::create();
    auto layer = BookCityScene::create();
    scene->addChild(layer);
    return scene;
}

bool BookCityScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	 
	App::m_RunningScene = MySceneName::BookCity;	
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "bookCityClickButton", 0);
	httpBookCityInfo();
	//控制快速多次点击
	//m_eventTime = new long long(0);
	//控制层
	//auto lister_high = EventListenerTouchOneByOne::create();
	//lister_high->onTouchBegan = [=](Touch* t, Event* e) {
	//	if (App::getCurrentTime() > (*m_eventTime))
	//	{
	//		CCLOG("time = %d", (int)*m_eventTime);
	//		Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(Director::getInstance()->getRunningScene(), true);
	//		*m_eventTime = App::getCurrentTime();
	//	}
	//	else
	//	{
	//		Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(Director::getInstance()->getRunningScene(), true);
	//	}
	//	return true;
	//};
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(lister_high, -200);
	////安卓返回键
	//auto androidListener = EventListenerKeyboard::create();
	//androidListener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event) {
	//	if (!App::GetInstance()->isBack)
	//		return;
	//	switch (keyCode)
	//	{
	//	case EventKeyboard::KeyCode::KEY_ESCAPE:
	//		if (getBoolFromXML(SOUND_KEY))
	//			PLAYBUTTON;
	//		Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	//		Index::GoToIndexScene();
	//		App::backThreading();
	//		break;
	//	default:
	//		break;
	//	}
	//};
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(androidListener, this);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	addChild(LayerColor::create(Color4B::WHITE));

	//--------------------------------------------------------------------背景控件-------------------------------------------------------------------------------------------------------------------------
	Layer* node;
	Data data;
	if (App::GetInstance()->getData(BABYCENTER_BOOKCITY_CSB,data))
	{
		node = (Layer*)CSLoader::createNode(data);
	}
	else
		node = (Layer*)CSLoader::createNode(BABYCENTER_BOOKCITY_CSB);
	node->setAnchorPoint(Vec2(0.5f, 0.5f));
	node->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
	//左边柱子
	auto leftColumn = (Sprite*)node->getChildByName(BABYCENTER_FIND_LEFTCOLUMN);
	leftColumn->setPositionX((1094 - visibleSize.width) / 2);
	leftColumn->setTag(100);
	//右边柱子
	auto rightColumn = (Sprite*)node->getChildByName(BABYCENTER_FIND_RIGHTCOLUMN);
	rightColumn->getTexture()->setAliasTexParameters();
	rightColumn->setPositionX((1094 + visibleSize.width) / 2);
	rightColumn->setTag(101);
	//背景墙 控件名称
	auto backWall = (Sprite*)node->getChildByName(BABYCENTER_FIND_BACKWALL);
	//地板
	auto floor = (Sprite*)node->getChildByName(BABYCENTER_FIND_FLOOR);
	floor->setTag(103);
	addChild(node);
	m_createTime = App::getCurrentTime();
	//返回按钮点击事件
	auto homeButton = ( Button*)node->getChildByName(BABYCENTER_FIND_BACK);
	homeButton->removeFromParentAndCleanup(true);
	homeButton->setAnchorPoint(Vec2(0, 1));
	homeButton->setPosition(origin + Vec2(14, visibleSize.height - 14));
	addChild(homeButton, 10);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		auto t = m_createTime + 1;
		if (App::getCurrentTime() < t)
			return;
		YYXLayer::controlTouchTime(2, "bookCityClickBookStoreTime", [=]() {
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			Index::GoToIndexScene();
		});
	});
	auto bookroom = (Button*)node->getChildByName("Button_1");
	if (bookroom)
	{
		bookroom->removeFromParent();
		bookroom->setAnchorPoint(Vec2(1, 1));
		bookroom->setPosition(origin + visibleSize + Vec2(-14, -14));
		addChild(bookroom, 10);
		bookroom->setScale(0.4);
		bookroom->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "bookstoreSceneBookRoomButtonTime", [=]() {
				App::GetInstance()->pushScene(MySceneName::BookCity);
				Index::GoToBookRoomScene();
			});
		});
	}
	//每个书店的按钮 资源需要复用 先读入内存
	//auto fileUtils = FileUtils::getInstance();
	//m_bookStoreCSBRes_double = fileUtils->getDataFromFile(BABYCENTER_DOUBLEBUTTON_CSB); //2颗小按钮
	//m_bookStoreCSBRes_onlyone = fileUtils->getDataFromFile(BABYCENTER_BIGBUTTON_CSB);//1颗大按钮
	//初始化listview
	m_listview = (ListView*)node->getChildByName(BABYCENTER_FIND_LISTVIEW_BYNAME);
	m_listview->setTouchEnabled(true);
	m_listview->setBounceEnabled(true);
	m_listview->setScrollBarEnabled(false);
	m_listview->setPositionX((1094 - visibleSize.width) / 2);
	m_listview->setSize(visibleSize);
	auto layout = Layout::create();
	layout->setContentSize(Size(80, 400));
	m_listview->pushBackCustomItem(layout);

	initListView();
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("bookCitySceneRefershListView", [=](EventCustom* e) {
		initListView();
	});
	auto lister = EventListenerTouchOneByOne::create();
	lister->onTouchBegan = [&](Touch* touch, Event *event) {
		m_isMoved = false;
		m_touchBegan = touch->getLocation();
		return true;
	};
	lister->onTouchMoved = [&](Touch* touch, Event *event) {
		auto touchMoved = touch->getLocation();
		if (touchMoved.x - m_touchBegan.x < -30 || touchMoved.x - m_touchBegan.x > 30 ||
			touchMoved.y - m_touchBegan.y < -30 || touchMoved.y - m_touchBegan.y > 30) {
			m_isMoved = true;
		}
	};
	_eventDispatcher->addEventListenerWithFixedPriority(lister, -140);
	//点击跳转书店
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("BookCitySceneClickButton", [=](EventCustom* e) {
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookCityScrollPosition", m_listview->getScrollBarPositionFromCornerForVertical().y);
	});
	//注册转换场景动作通知
	auto listenTurnAction = EventListenerCustom::create("Listen_Turn_Action", [=](EventCustom* event) {
		m_listview->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(0, 1430 * 0.4)), NULL));
		backWall->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(0, 1430 * 0.4)), NULL));
		floor->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(0, -320 * 0.4)), NULL));
		leftColumn->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(-200 * 0.4, 0)), NULL));
		rightColumn->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(200 * 0.4, 0)), NULL));
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listenTurnAction, 1);
	return true;
}

//初始化listview中的item
//void BookCityScene::initListView()
//{
//	std::vector<std::string> vec;
//	vec.push_back("castleId");
//	vec.push_back("storeBorder");
//	auto result_big = SqliteManager::SelectData(App::sqliteOpen(), DB_STOREINFO, vec, "where storeBorder=0");
//	auto result_small = SqliteManager::SelectData(App::sqliteOpen(), DB_STOREINFO, vec, "where storeBorder=1");
//	App::sqliteClose();
//
//	if (result_small.size() > 2) {
//		auto layout = Layout::create();
//		layout->setContentSize(Size(240, 500));
//		layout->addChild(SmallBtn::create(m_bookStoreCSBRes_double, result_small[0]["castleId"].intPara, result_small[1]["castleId"].intPara));
//		m_listview->pushBackCustomItem(layout);
//
//		if (result_big.size() > 0) {
//			auto layout = Layout::create();
//			layout->setContentSize(Size(360, 450));
//			layout->addChild(BigBtn::create(m_bookStoreCSBRes_onlyone, result_big[0]["castleId"].intPara));
//			m_listview->pushBackCustomItem(layout);
//		}
//		
//		for (int i = 2; i < result_small.size(); i++)
//		{
//			auto layout = Layout::create();
//			layout->setContentSize(Size(240, 500));
//			if (i + 1 < result_small.size()) {
//				layout->addChild(SmallBtn::create(m_bookStoreCSBRes_double, result_small[i]["castleId"].intPara, result_small[i+1]["castleId"].intPara));
//				i++;
//			}
//			else {
//				layout->addChild(SmallBtn::create(m_bookStoreCSBRes_double, result_small[i]["castleId"].intPara, -999));
//			}
//			m_listview->pushBackCustomItem(layout);
//		}
//	}
//	else {//数据库没有缓存信息，则发出网络请求获取信息
//		if (!CocosAndroidJni::IsNetConnect())
//		{
//			Toast::create(App::getString("NETEXCEPTION"));
//			return;
//		}
//		Load::httpButtonsInfo();
//	}
//	auto y = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookCityScrollPosition", -999);
//	if ( y != -999)
//		m_listview->setScrollBarPositionFromCornerForVertical(Vec2(0, y));
//}

void BookCityScene::httpBookCityInfo()
{
	string url = string(IP).append(NET_BOOKCITY).append("?memberId=").append(App::getMemberID()).append("&page=1&pagesize=1000&resource=").append(App::m_resource);
	string runkey = "BookCityScenehttpBookCityInfo";
	string errorkey = runkey + "error";
	NetIntface::httpGet(url, runkey, [](string json) {
		NetIntface::httpBookCityInfoCallBack(json, [](int index, int castleId, int castleType, int sort, int storeBorder, int hasNewBook, string castleName, string borderUrl, string bgUrl, string bagUrl) {
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
				YYXLayer::deleteLoadingLayer(1);
			}
			if (!FileUtils::getInstance()->isFileExist(borderpath))
			{
				string DownLoadImageRunKey = StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom());
				string DownLoadImageErrorKey = StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom());
				NetIntface::DownLoadImage(borderUrl, dir, borderfileName, DownLoadImageRunKey, [=](string downPath) {
					//下载成功
					YYXLayer::deleteLoadingLayer(1);
					string listenerKey = StringUtils::format("bookcity+castleId=%d", castleId);
					YYXLayer::sendNotify(listenerKey);
				}, DownLoadImageErrorKey, [=](string str) {
					string sstr = string(castleName).append(App::getString("SHUDIANTUBIAOXIAZAISHIBAI"));
					Toast::create(sstr.c_str(), false);
					YYXLayer::deleteLoadingLayer(1);
				});
			}
			if (!FileUtils::getInstance()->isFileExist(bgpath))
			{
				string DownLoadImageRunKey = StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom());
				string DownLoadImageErrorKey = StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom());
				NetIntface::DownLoadImage(bgUrl, dir, bgfileName, DownLoadImageRunKey, [=](string downPath) {
					//下载成功
					YYXLayer::deleteLoadingLayer(1);
					string listenerKey = StringUtils::format("bookcity+castleId=%d", castleId);
					YYXLayer::sendNotify(listenerKey);
				}, DownLoadImageErrorKey, [=](string str) {
					string sstr = string(castleName).append(App::getString("SHUDIANTUBIAOXIAZAISHIBAI"));
					Toast::create(sstr.c_str(), false);
					YYXLayer::deleteLoadingLayer(1);
				});
			}
		}, [](int totalPage) {
			//书店数量
			string totalCountKey = StringUtils::format("BookCityTotalPage");
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, totalCountKey, totalPage);
			YYXLayer::sendNotify("bookCitySceneRefershListView");
			YYXLayer::deleteLoadingLayer(1);
		}, []() {
			Toast::create(App::getString("SHUCHENGXINXIYICHANG"));
			YYXLayer::deleteLoadingLayer(1);
		});
	}, errorkey, [](string str) {
		Toast::create(App::getString("NETEXCEPTION"));
		YYXLayer::deleteLoadingLayer(1);
	});
}

void BookCityScene::initListView()
{
	auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "bookCityClickButton", 0);
	if (click == 1)
		return;
	m_listview->removeAllItems();
	auto layout = Layout::create();
	layout->setContentSize(Size(100, 450));
	m_listview->pushBackCustomItem(layout);
	//书店数量
	string totalCountKey = StringUtils::format("BookCityTotalPage");
	auto BookCityTotalPage = YYXStruct::getMapInt64(App::GetInstance()->myData, totalCountKey, 0);
	if (BookCityTotalPage > 0)
	{
		YYXLayer::deleteLoadingLayer(1);
	}
	else
	{
		auto lay = YYXLayer::LoadingLayer(5, []() {});
		if (lay)
		{
			addChild(lay, 9);
		}
	}
	for (int i = 0; i < BookCityTotalPage; i++)
	{
		auto layout = Layout::create();		
		string sortKey = StringUtils::format("sort=%d+bookcity", i);
		int castleId = YYXStruct::getMapInt64(App::GetInstance()->myData, sortKey, -999);
		auto storeBorder =(int) YYXStruct::getMapRef(App::GetInstance()->myData, sortKey, (Ref*)1);
		if (castleId > 0)
		{
			if (storeBorder == 0)
			{
				layout->setContentSize(Size(360, 450));
				Data data;
				if (App::GetInstance()->getData(BABYCENTER_BIGBUTTON_CSB, data))
					layout->addChild(BigBtn::create(data, castleId));
			}
			else
			{
				layout->setContentSize(Size(240, 500));
				Data data;
				if (App::GetInstance()->getData(BABYCENTER_DOUBLEBUTTON_CSB, data))
				{
					string sortKey2 = StringUtils::format("sort=%d+bookcity", i+1);
					int castleId2 = YYXStruct::getMapInt64(App::GetInstance()->myData, sortKey2, -999);
					auto storeBorder2 = (int)YYXStruct::getMapRef(App::GetInstance()->myData, sortKey2, (Ref*)1);
					if (storeBorder2 != 0)
					{
						layout->addChild(SmallBtn::create(data, castleId, castleId2));
						++i;
					}
					else
						layout->addChild(SmallBtn::create(data, castleId, -999));
				}
			}
		}
		m_listview->pushBackCustomItem(layout);
	}
}

//YYXLayer* BookCityScene::LoadingLayer()
//{
//	App::log("BookCityScene::LoadingLayer");
//	map<string, int64String> paramter;
//	YYXLayer::insertMap4ParaType(paramter, "className", -999, "BookCitySceneLoading");
//	YYXLayer::insertMap4ParaType(paramter, "csb", -999, WAITMESSAGEBOX_CSB);
//	YYXLayer::insertMap4ParaType(paramter, "csbTimeLine", 28, "");
//	YYXLayer::insertMap4ParaType(paramter, "gotoFrameAndPlayIsLoop", 0, "");
//	auto loading = YYXLayer::create(paramter, map<string, function<void(YYXLayer*)>>());
//	auto listener = EventListenerCustom::create("loadingDelete", [=](EventCustom* e) {
//		App::log("+++++++++++++loadingDelete");
//		loading->removeFromParentAndCleanup(true);
//		App::log("+++++++++++++loadingDelete---END");
//	});
//	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, loading);
//	loading->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
//	loading->setPosition(Director::getInstance()->getVisibleSize() / 2);
//	auto imageview = (ImageView*)loading->findControl(FIND_IMAGEVIEW_BY_NAME_WAITMESSAGEBOX);
//	imageview->setTouchEnabled(true);
//	imageview->setSwallowTouches(true);
//	imageview->addClickEventListener([=](Ref* sender) {
//		YYXLayer::controlTouchTime(1, "bookCitySceneClickLoadingLayerTime", [=]() {
//			if (YYXLayer::getCurrentTime4Second() > m_createTime + 3)
//			{
//				loading->removeFromParentAndCleanup(true);
//			}
//		});
//	});
//	string key = "bookCitySceneDeleteLoadingLayerKeyName";
//	loading->addSchedule(key, 1, [=]() {
//		auto t = YYXLayer::getCurrentTime4Second();
//		if (t > m_createTime + 5)
//		{
//			YYXLayer::sendNotify("loadingDelete");
//		}
//	});
//	App::log("BookCityScene::LoadingLayer--END");
//	return loading;
//}
////异步下载封面或边框
//void BookCityScene::AsyncDownload(int id, std::string url, int tag) {
//	if (!CocosAndroidJni::IsNetConnect())
//	{
//		Toast::create(App::getString("NETEXCEPTION"));
//		return;
//	}
//	if (url.length() == 0)
//		return;
//	if(m_queue)
//		m_queue->push_back(std::bind(&BookCityScene::threadDownd, id, url, tag));
//}
//void BookCityScene::threadDownd(int id, std::string url, int tag) {
//	CCLOG("tag = %d",tag);
//	//网络请求图片
//	HttpRequest* pRequest = new HttpRequest();
//	pRequest->setUrl(url.c_str());
//	pRequest->setRequestType(HttpRequest::Type::GET);
//	pRequest->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
//		if (!response) {
//			notifyLoadFailed(StringUtils::format("Down_Failed_%d_%d", id, tag));
//			return;
//		}
//		if (!response->isSucceed()) {
//			log("connect failed!  error buffer: %s", response->getErrorBuffer());
//			notifyLoadFailed(StringUtils::format("Down_Failed_%d_%d", id, tag));
//			return;
//		}
//
//		std::vector<char> *buffer = response->getResponseData();
//		char* buf = (char*)malloc(buffer->size());
//		std::copy(buffer->begin(), buffer->end(), buf);
//
//		auto dir = FileUtils::getInstance()->getWritablePath() ;
//		std::string fileName = StringUtils::format("%d_%d.png", id, tag);
//		if (!FileUtils::getInstance()->createDirectory(dir)) {
//			CCLOG("error : createDirectory %s ", dir.c_str());
//			notifyLoadFailed(StringUtils::format("Down_Failed_%d_%d", id, tag));
//			return;
//		}
//		FILE *fp = fopen((dir + fileName).c_str(), "wb+");
//		fwrite(buf, 1, buffer->size(), fp);
//		fclose(fp);
//
//		//判断是否有记录 有则删除
//		sqlite3* myDB2 = SqliteManager::OpenDataBase();
//		SqliteManager::DeleteData(myDB2, DB_RES, StringUtils::format("where bookId=%d and status=%d", id, tag));
//		//插入数据库
//		unordered_map<string, string> paraInfoSTR;
//		unordered_map<string, long long> paraInfoINT;
//		paraInfoINT["bookId"] = id;
//		paraInfoINT["status"] = tag;
//		auto time = App::getCurrentTime();
//		paraInfoINT["downloadtime"] = time;
//		paraInfoSTR["path"] = dir+ fileName;
//		SqliteManager::InsertData(myDB2, DB_RES, paraInfoSTR, paraInfoINT);
//		SqliteManager::CloseDataBase(myDB2);
//
//		if (buf != nullptr)
//			free(buf);
//		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {//下载成功操作
//			EventCustom eventDownSuccess(StringUtils::format("Down_Success_%d_%d", id, tag));
//			Director::getInstance()->getEventDispatcher()->setEnabled(true);
//			Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventDownSuccess);
//		});
//	});
//	HttpClient::getInstance()->send(pRequest);
//	pRequest->release();
//}
//通知下载封面或边框失败
//void BookCityScene::notifyLoadFailed(string msg) {
//	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
//		EventCustom eventDownFailed(msg);
//		Director::getInstance()->getEventDispatcher()->setEnabled(true);
//		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventDownFailed);
//	});
//}

void BookCityScene::onEnterTransitionDidFinish()
{
	App::log("BookCityScene::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	App::log("BookCityScene::onEnterTransitionDidFinish---END");
}

//---------------------------------------BigBtn----------------------------------------------------
BigBtn::BigBtn() {
	m_castleId = -999;
}
BigBtn::~BigBtn() {

}

BigBtn* BigBtn::create(Data data, int id) {
	BigBtn* pRect = new BigBtn;
	if (pRect) {
		pRect->autorelease();
		pRect->m_castleId = id;
		pRect->initNode(data);
	}
	else {
		pRect->release();
		pRect = NULL;
	}
	return pRect;
}

void BigBtn::initNode(Data data) {
	auto button = CSLoader::createNode(data);
	addChild(button);

	if (m_castleId == -999)
		return;

	auto b_bg = (ImageView*)button->getChildByName(BABYCENTER_FIND_IMAGEVIEW_BYNAME_BIGBUTTON_BACKGROUND);//背景
	auto b_border = (ImageView*)Helper::seekWidgetByName(b_bg, BABYCENTER_FIND_IMAGEVIEW_BYNAME_BIGBUTTON_BORDER);//边框
	b_bg->setTouchEnabled(true);
	b_bg->setSwallowTouches(false);
	b_bg->addClickEventListener([=](Ref* sender) {
		if (BookCityScene::m_isMoved)
			return;
		YYXLayer::controlTouchTime(2, "bookCityClickBookStoreTime", [=]() {
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "bookCityClickButton", 1);
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			YYXLayer::sendNotify("Listen_Turn_Action");
			//EventCustom eventCtiyAction("Listen_Turn_Action");
			//Director::getInstance()->getEventDispatcher()->setEnabled(true);
			//Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventCtiyAction);
			auto sc1 = ScaleTo::create(0.3f, 1.25);
			RotateTo* ro1 = RotateTo::create(0.3f, 10);
			Spawn* sp1 = Spawn::create(sc1, ro1, NULL);

			auto sc2 = ScaleTo::create(0.1f, 1.2);
			RotateTo* ro2 = RotateTo::create(0.1f, -8);
			Spawn* sp2 = Spawn::create(sc2, ro2, NULL);

			auto sc3 = ScaleTo::create(0.1f, 1.15);
			RotateTo* ro3 = RotateTo::create(0.1f, 6);
			Spawn* sp3 = Spawn::create(sc3, ro3, NULL);

			auto sc4 = ScaleTo::create(0.15f, 1.1);
			RotateTo* ro4 = RotateTo::create(0.15f, -4);
			Spawn* sp4 = Spawn::create(sc4, ro4, NULL);

			auto sc5 = ScaleTo::create(0.1f, 1.1);
			RotateTo* ro5 = RotateTo::create(0.1f, 2);
			Spawn* sp5 = Spawn::create(sc5, ro5, NULL);

			auto sc6 = ScaleTo::create(0.25f, 1);
			RotateTo* ro6 = RotateTo::create(0.25f, 0);
			Spawn* sp6 = Spawn::create(sc6, ro6, NULL);

			CallFunc *callFunc = CallFunc::create([=]() {
				App::GetInstance()->pushScene(MySceneName::BookCity);
				if (m_castleId < 0)
					m_castleId = 21;
				Index::GoToBookCityChildStore(m_castleId);
				YYXLayer::sendNotify("BookCitySceneClickButton");
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "bookCityClickButton", 0);
			});

			auto seq = Sequence::create(sp1, sp2, sp3, sp4, sp5, sp6, DelayTime::create(1.3f), callFunc, NULL);
			b_border->runAction(seq);
			b_bg->runAction(seq);
		});
	});
	string dir = FileUtils::getInstance()->getWritablePath() + "bookCity";
	string borderfileName = StringUtils::format("border_%d", m_castleId) + ".png";
	string bgfileName = StringUtils::format("bg_%d", m_castleId) + ".png";
	auto borderpath = dir + "/" + borderfileName;
	auto bgpath = dir + "/" + bgfileName;
	if (FileUtils::getInstance()->isFileExist(borderpath))
	{
		b_border->loadTexture(borderpath);
	}
	if (FileUtils::getInstance()->isFileExist(bgpath))
	{
		b_bg->loadTexture(bgpath);
	}
	string listenerKey = StringUtils::format("bookcity+castleId=%d", m_castleId);
	auto listener = EventListenerCustom::create(listenerKey, [=](EventCustom* e) {
		string dir = FileUtils::getInstance()->getWritablePath() + "bookCity";
		string borderfileName = StringUtils::format("border_%d", m_castleId) + ".png";
		string bgfileName = StringUtils::format("bg_%d", m_castleId) + ".png";
		auto borderpath = dir + "/" + borderfileName;
		auto bgpath = dir + "/" + bgfileName;
		if (FileUtils::getInstance()->isFileExist(borderpath))
		{
			auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "bookCityClickButton", 0);
			if (click == 1)
				return;
			b_border->loadTexture(borderpath);
		}
		if (FileUtils::getInstance()->isFileExist(bgpath))
		{
			auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "bookCityClickButton", 0);
			if (click == 1)
				return;
			b_bg->loadTexture(bgpath);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, button);
	////查询获取URL
	//std::vector<std::string> vec;
	//vec.push_back("bgUrl");
	//vec.push_back("borderUrl");
	//auto result = SqliteManager::SelectData(App::sqliteOpen(), DB_STOREINFO, vec, StringUtils::format("where castleId=%d", m_castleId));
	//App::sqliteClose();
	//auto coverUrl = result[0]["bgUrl"].stringPara;
	//auto boderUrl = result[0]["borderUrl"].stringPara;
	////判断封面是否已下载
	//if (SqliteManager::IsRecExist(App::sqliteOpen(), DB_RES, StringUtils::format("where bookId=%d and status=6", m_castleId))) {
	//	vector<string> paraStr;
	//	paraStr.push_back("path");
	//	paraStr.push_back("downloadtime");
	//	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=6", m_castleId));
	//	auto fullpath = result[0]["path"].stringPara;
	//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
	//		b_bg->loadTexture(fullpath);
	//	}
	//	else
	//	{
	//		BookCityScene::AsyncDownload(m_castleId, coverUrl, 6);
	//	}
	//	//判断封面是否需要更新
	//	if (App::getCurrentTime() - result[0]["downloadtime"].intPara > NET_UPDATE_COVER_TIME_INTERVAL) {
	//		BookCityScene::AsyncDownload(m_castleId, coverUrl, 6);
	//	}
	//}
	//else{
	//	BookCityScene::AsyncDownload(m_castleId, coverUrl, 6);
	//}
	////判断边框是否已下载
	//if (SqliteManager::IsRecExist(App::sqliteOpen(), DB_RES, StringUtils::format("where bookId=%d and status=7", m_castleId))) {
	//	vector<string> paraStr;
	//	paraStr.push_back("path");
	//	paraStr.push_back("downloadtime");
	//	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=7", m_castleId));
	//	auto fullpath = result[0]["path"].stringPara;
	//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
	//		b_border->loadTexture(fullpath);
	//	}
	//	else
	//		BookCityScene::AsyncDownload(m_castleId, boderUrl, 7);
	//	//判断封面是否需要更新
	//	if (App::getCurrentTime() - result[0]["downloadtime"].intPara > NET_UPDATE_COVER_TIME_INTERVAL) {
	//		BookCityScene::AsyncDownload(m_castleId, boderUrl, 7);
	//	}
	//}
	//else {
	//	BookCityScene::AsyncDownload(m_castleId, boderUrl, 7);
	//}
	////注册封面下载成功的通知
	//auto listenDownCoverSuccess = EventListenerCustom::create(StringUtils::format("Down_Success_%d_6", m_castleId), [=](EventCustom* event) {
	//	vector<string> paraStr;
	//	paraStr.push_back("path");
	//	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=6", m_castleId));
	//	//设置封面
	//	auto fullpath =  result[0]["path"].stringPara;
	//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
	//		b_bg->loadTexture(fullpath);
	//	}
	//	else {
	//		CCLOG("%s this picture is NULL", fullpath.c_str());
	//	}
	//	App::sqliteClose();
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(listenDownCoverSuccess, 1);
	////注册封面下载失败的通知
	//auto listenDownCoverFailed = EventListenerCustom::create(StringUtils::format("Down_Failed_%d_6", m_castleId), [=](EventCustom* event) {
	//	BookCityScene::AsyncDownload(m_castleId, coverUrl, 6);
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(listenDownCoverFailed, 1);
	////注册边框下载成功的通知
	//auto listenDownBoderSuccess = EventListenerCustom::create(StringUtils::format("Down_Success_%d_7", m_castleId), [=](EventCustom* event) {
	//	vector<string> paraStr;
	//	paraStr.push_back("path");
	//	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=7", m_castleId));
	//	//设置封面
	//	auto fullpath = result[0]["path"].stringPara;
	//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
	//		b_border->loadTexture(fullpath);
	//	}
	//	else {
	//		CCLOG("%s this picture is NULL", fullpath.c_str());
	//	}
	//	App::sqliteClose();
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(listenDownBoderSuccess, 1);
	////注册边框下载失败的通知
	//auto listenDownBoderFailed = EventListenerCustom::create(StringUtils::format("Down_Failed_%d_7", m_castleId), [=](EventCustom* event) {
	//	BookCityScene::AsyncDownload(m_castleId, boderUrl, 7);
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(listenDownBoderFailed, 1);
}


//---------------------------------------SmallBtn--------------------------------------------------
SmallBtn::SmallBtn() {
	m_castleId_top = -999;
	m_castleId_bottom = -999;
}
SmallBtn::~SmallBtn() {

}

SmallBtn* SmallBtn::create(Data data, int id_top, int id_bottom) {
	SmallBtn* pRect = new SmallBtn;
	if (pRect) {
		pRect->autorelease();
		pRect->m_castleId_top = id_top;
		pRect->m_castleId_bottom = id_bottom;
		pRect->initNode(data);
	}
	else {
		pRect->release();
		pRect = NULL;
	}
	return pRect;
}

void SmallBtn::initNode(Data data) {
	auto button = CSLoader::createNode(data);
	addChild(button);

	if (m_castleId_top == -999)
		return;

	//------------------------------------------------------上面一个按钮----------------------------------------------------------------------
	auto b_bgUp = (ImageView*)button->getChildByName(BABYCENTER_FIND_IMAGEVIEW_BYNAME_DOUBLEBUTTON_UP_BACKGROUND);//上面一颗按钮的背景
	auto b_borderUp = (ImageView*)Helper::seekWidgetByName(b_bgUp, BABYCENTER_FIND_IMAGEVIEW_BYNAME_DOUBLEBUTTON_UP_BORDER);//上面一颗按钮的边框
	b_bgUp->setTouchEnabled(true);
	b_bgUp->setSwallowTouches(false);
	b_bgUp->addClickEventListener([=](Ref* sender) {
		if (BookCityScene::m_isMoved)
			return;
		//EventCustom eventCtiyAction("Listen_Turn_Action");
		//Director::getInstance()->getEventDispatcher()->setEnabled(true);
		//Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventCtiyAction);
		YYXLayer::controlTouchTime(2, "bookCityClickBookStoreTime", [=]() {
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "bookCityClickButton", 1);
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			YYXLayer::sendNotify("Listen_Turn_Action");
			auto sc1 = ScaleTo::create(0.3f, 1.25);
			RotateTo* ro1 = RotateTo::create(0.3f, 10);
			Spawn* sp1 = Spawn::create(sc1, ro1, NULL);
			auto sc2 = ScaleTo::create(0.1f, 1.2);
			RotateTo* ro2 = RotateTo::create(0.1f, -8);
			Spawn* sp2 = Spawn::create(sc2, ro2, NULL);
			auto sc3 = ScaleTo::create(0.1f, 1.15);
			RotateTo* ro3 = RotateTo::create(0.1f, 6);
			Spawn* sp3 = Spawn::create(sc3, ro3, NULL);
			auto sc4 = ScaleTo::create(0.15f, 1.1);
			RotateTo* ro4 = RotateTo::create(0.15f, -4);
			Spawn* sp4 = Spawn::create(sc4, ro4, NULL);
			auto sc5 = ScaleTo::create(0.1f, 1.1);
			RotateTo* ro5 = RotateTo::create(0.1f, 2);
			Spawn* sp5 = Spawn::create(sc5, ro5, NULL);
			auto sc6 = ScaleTo::create(0.25f, 1);
			RotateTo* ro6 = RotateTo::create(0.25f, 0);
			Spawn* sp6 = Spawn::create(sc6, ro6, NULL);
			CallFunc *callFunc = CallFunc::create([=]() {
				App::GetInstance()->pushScene(MySceneName::BookCity);
				if (m_castleId_top < 0)
					m_castleId_top = 21;
				Index::GoToBookCityChildStore(m_castleId_top);
				YYXLayer::sendNotify("BookCitySceneClickButton");
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "bookCityClickButton", 0);
			});
			auto seq = Sequence::create(sp1, sp2, sp3, sp4, sp5, sp6, DelayTime::create(1.3f), callFunc, NULL);
			b_borderUp->runAction(seq);
			b_bgUp->runAction(seq);
		});
	});
	string dir = FileUtils::getInstance()->getWritablePath() + "bookCity";
	string borderfileName = StringUtils::format("border_%d", m_castleId_top) + ".png";
	string bgfileName = StringUtils::format("bg_%d", m_castleId_top) + ".png";
	auto borderpath = dir + "/" + borderfileName;
	auto bgpath = dir + "/" + bgfileName;
	if (FileUtils::getInstance()->isFileExist(borderpath))
	{
		b_borderUp->loadTexture(borderpath);
	}
	if (FileUtils::getInstance()->isFileExist(bgpath))
	{
		b_bgUp->loadTexture(bgpath);
	}
	string listenerKey = StringUtils::format("bookcity+castleId=%d", m_castleId_top);
	auto listener = EventListenerCustom::create(listenerKey, [=](EventCustom* e) {
		string dir = FileUtils::getInstance()->getWritablePath() + "bookCity";
		string borderfileName = StringUtils::format("border_%d", m_castleId_top) + ".png";
		string bgfileName = StringUtils::format("bg_%d", m_castleId_top) + ".png";
		auto borderpath = dir + "/" + borderfileName;
		auto bgpath = dir + "/" + bgfileName;
		if (FileUtils::getInstance()->isFileExist(borderpath))
		{
			auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "bookCityClickButton", 0);
			if (click == 1)
				return;
			b_borderUp->loadTexture(borderpath);
		}
		if (FileUtils::getInstance()->isFileExist(bgpath))
		{
			auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "bookCityClickButton", 0);
			if (click == 1)
				return;
			b_bgUp->loadTexture(bgpath);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, button);
	////查询获取URL
	//std::vector<std::string> vec;
	//vec.push_back("bgUrl");
	//vec.push_back("borderUrl");
	//auto result = SqliteManager::SelectData(App::sqliteOpen(), DB_STOREINFO, vec, StringUtils::format("where castleId=%d", m_castleId_top));
	//App::sqliteClose();
	//auto coverUrl = result[0]["bgUrl"].stringPara;
	//auto boderUrl = result[0]["borderUrl"].stringPara;
	////判断封面是否已下载
	//if (SqliteManager::IsRecExist(App::sqliteOpen(), DB_RES, StringUtils::format("where bookId=%d and status=6", m_castleId_top))) {
	//	vector<string> paraStr;
	//	paraStr.push_back("path");
	//	paraStr.push_back("downloadtime");
	//	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=6", m_castleId_top));
	//	auto fullpath = result[0]["path"].stringPara;
	//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
	//		b_bgUp->loadTexture(fullpath);
	//	}
	//	else
	//		BookCityScene::AsyncDownload(m_castleId_top, coverUrl, 6);
	//	//判断封面是否需要更新
	//	if (App::getCurrentTime() - result[0]["downloadtime"].intPara > NET_UPDATE_COVER_TIME_INTERVAL) {
	//		BookCityScene::AsyncDownload(m_castleId_top, coverUrl, 6);
	//	}
	//}
	//else {
	//	BookCityScene::AsyncDownload(m_castleId_top, coverUrl, 6);
	//}
	////判断边框是否已下载
	//if (SqliteManager::IsRecExist(App::sqliteOpen(), DB_RES, StringUtils::format("where bookId=%d and status=7", m_castleId_top))) {
	//	vector<string> paraStr;
	//	paraStr.push_back("path");
	//	paraStr.push_back("downloadtime");
	//	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=7", m_castleId_top));
	//	auto fullpath =  result[0]["path"].stringPara;
	//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
	//		b_borderUp->loadTexture(fullpath);
	//	}
	//	else
	//		BookCityScene::AsyncDownload(m_castleId_top, boderUrl, 7);
	//	//判断封面是否需要更新
	//	if (App::getCurrentTime() - result[0]["downloadtime"].intPara > NET_UPDATE_COVER_TIME_INTERVAL) {
	//		BookCityScene::AsyncDownload(m_castleId_top, boderUrl, 7);
	//	}
	//}
	//else {
	//	BookCityScene::AsyncDownload(m_castleId_top, boderUrl, 7);
	//}
	////注册封面下载成功的通知
	//auto listenDownCoverSuccess = EventListenerCustom::create(StringUtils::format("Down_Success_%d_6", m_castleId_top), [=](EventCustom* event) {
	//	vector<string> paraStr;
	//	paraStr.push_back("path");
	//	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=6", m_castleId_top));
	//	//设置封面
	//	auto fullpath =  result[0]["path"].stringPara;
	//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
	//		b_bgUp->loadTexture(fullpath);
	//	}
	//	else {
	//		CCLOG("%s this picture is NULL", fullpath.c_str());
	//	}
	//	App::sqliteClose();
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(listenDownCoverSuccess, 1);
	////注册封面下载失败的通知
	//auto listenDownCoverFailed = EventListenerCustom::create(StringUtils::format("Down_Failed_%d_6", m_castleId_top), [=](EventCustom* event) {
	//	BookCityScene::AsyncDownload(m_castleId_top, coverUrl, 6);
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(listenDownCoverFailed, 1);
	////注册边框下载成功的通知
	//auto listenDownBoderSuccess = EventListenerCustom::create(StringUtils::format("Down_Success_%d_7", m_castleId_top), [=](EventCustom* event) {
	//	vector<string> paraStr;
	//	paraStr.push_back("path");
	//	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=7", m_castleId_top));
	//	//设置封面
	//	auto fullpath =  result[0]["path"].stringPara;
	//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
	//		b_borderUp->loadTexture(fullpath);
	//	}
	//	else {
	//		CCLOG("%s this picture is NULL", fullpath.c_str());
	//	}
	//	App::sqliteClose();
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(listenDownBoderSuccess, 1);
	////注册边框下载失败的通知
	//auto listenDownBoderFailed = EventListenerCustom::create(StringUtils::format("Down_Failed_%d_7", m_castleId_top), [=](EventCustom* event) {
	//	BookCityScene::AsyncDownload(m_castleId_top, boderUrl, 7);
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(listenDownBoderFailed, 1);
	//========================================================================================================================================

	//-------------------------------------------------------下面一个按钮----------------------------------------------------------------------
	auto b_bgDown = (ImageView*)button->getChildByName(BABYCENTER_FIND_IMAGEVIEW_BYNAME_DOUBLEBUTTON_DOWN_BACKGROUND);//下面一颗按钮的背景
	auto b_borderDown = (ImageView*)Helper::seekWidgetByName(b_bgDown, BABYCENTER_FIND_IMAGEVIEW_BYNAME_DOUBLEBUTTON_DOWN_BORDER);//下面一颗按钮的边框
	b_bgDown->setTouchEnabled(true);
	b_bgDown->setSwallowTouches(false);
	b_bgDown->addClickEventListener([=](Ref* sender) {
		if (BookCityScene::m_isMoved)
			return;
		YYXLayer::controlTouchTime(2, "bookCityClickBookStoreTime", [=]() {
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "bookCityClickButton", 1);
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			YYXLayer::sendNotify("Listen_Turn_Action");
			//EventCustom eventCtiyAction("Listen_Turn_Action");
			//Director::getInstance()->getEventDispatcher()->setEnabled(true);
			//Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventCtiyAction);
			b_bgDown->setZOrder(1);

			auto sc1 = ScaleTo::create(0.3f, 1.25);
			RotateTo* ro1 = RotateTo::create(0.3f, 10);
			Spawn* sp1 = Spawn::create(sc1, ro1, NULL);

			auto sc2 = ScaleTo::create(0.1f, 1.2);
			RotateTo* ro2 = RotateTo::create(0.1f, -8);
			Spawn* sp2 = Spawn::create(sc2, ro2, NULL);

			auto sc3 = ScaleTo::create(0.1f, 1.15);
			RotateTo* ro3 = RotateTo::create(0.1f, 6);
			Spawn* sp3 = Spawn::create(sc3, ro3, NULL);

			auto sc4 = ScaleTo::create(0.15f, 1.1);
			RotateTo* ro4 = RotateTo::create(0.15f, -4);
			Spawn* sp4 = Spawn::create(sc4, ro4, NULL);

			auto sc5 = ScaleTo::create(0.1f, 1.1);
			RotateTo* ro5 = RotateTo::create(0.1f, 2);
			Spawn* sp5 = Spawn::create(sc5, ro5, NULL);

			auto sc6 = ScaleTo::create(0.25f, 1);
			RotateTo* ro6 = RotateTo::create(0.25f, 0);
			Spawn* sp6 = Spawn::create(sc6, ro6, NULL);

			CallFunc *callFunc = CallFunc::create([=]() {
				App::GetInstance()->pushScene(MySceneName::BookCity);
				if (m_castleId_bottom < 0)
					m_castleId_bottom = 21;
				Index::GoToBookCityChildStore(m_castleId_bottom);
				YYXLayer::sendNotify("BookCitySceneClickButton");
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "bookCityClickButton", 0);
			});

			auto seq = Sequence::create(sp1, sp2, sp3, sp4, sp5, sp6, DelayTime::create(1.3f), callFunc, NULL);
			b_borderDown->runAction(seq);
			b_bgDown->runAction(seq);
		});
	});

	if (m_castleId_bottom == -999) {
		b_bgDown->setTouchEnabled(false);
		b_bgDown->setVisible(false);
		b_borderDown->setVisible(false);
	}
	else{
		////查询获取URL
		//std::vector<std::string> vec2;
		//vec2.push_back("bgUrl");
		//vec2.push_back("borderUrl");
		//auto result2 = SqliteManager::SelectData(App::sqliteOpen(), DB_STOREINFO, vec2, StringUtils::format("where castleId=%d", m_castleId_bottom));
		//App::sqliteClose();
		//auto coverUrl2 = result2[0]["bgUrl"].stringPara;
		//auto boderUrl2 = result2[0]["borderUrl"].stringPara;
		//
		////判断封面是否已下载
		//if (SqliteManager::IsRecExist(App::sqliteOpen(), DB_RES, StringUtils::format("where bookId=%d and status=6", m_castleId_bottom))) {
		//	vector<string> paraStr;
		//	paraStr.push_back("path");
		//	paraStr.push_back("downloadtime");
		//	vector<unordered_map<string, ParaType>> myresult = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=6", m_castleId_bottom));
		//	auto fullpath =  myresult[0]["path"].stringPara;
		//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
		//		b_bgDown->loadTexture(fullpath);
		//	}
		//	else
		//		BookCityScene::AsyncDownload(m_castleId_bottom, coverUrl2, 6);
		//	//判断封面是否需要更新
		//	if (App::getCurrentTime() - myresult[0]["downloadtime"].intPara > NET_UPDATE_COVER_TIME_INTERVAL) {
		//		BookCityScene::AsyncDownload(m_castleId_bottom, coverUrl2, 6);
		//	}
		//}
		//else {
		//	BookCityScene::AsyncDownload(m_castleId_bottom, coverUrl2, 6);
		//}
		//
		////判断边框是否已下载
		//if (SqliteManager::IsRecExist(App::sqliteOpen(), DB_RES, StringUtils::format("where bookId=%d and status=7", m_castleId_bottom))) {
		//	vector<string> paraStr;
		//	paraStr.push_back("path");
		//	paraStr.push_back("downloadtime");
		//	vector<unordered_map<string, ParaType>> myresult = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=7", m_castleId_bottom));
		//	auto fullpath =  myresult[0]["path"].stringPara;
		//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
		//		b_borderDown->loadTexture(fullpath);
		//	}
		//	else
		//		BookCityScene::AsyncDownload(m_castleId_bottom, boderUrl2, 7);
		//	//判断封面是否需要更新
		//	if (App::getCurrentTime() - myresult[0]["downloadtime"].intPara > NET_UPDATE_COVER_TIME_INTERVAL) {
		//		BookCityScene::AsyncDownload(m_castleId_bottom, boderUrl2, 7);
		//	}
		//}
		//else {
		//	BookCityScene::AsyncDownload(m_castleId_bottom, boderUrl2, 7);
		//}
		//
		////注册封面下载成功的通知
		//auto listenDownCoverSuccess2 = EventListenerCustom::create(StringUtils::format("Down_Success_%d_6", m_castleId_bottom), [=](EventCustom* event) {
		//	vector<string> paraStr;
		//	paraStr.push_back("path");
		//	vector<unordered_map<string, ParaType>> result_1 = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=6", m_castleId_bottom));
		//	//设置封面
		//	auto fullpath =  result_1[0]["path"].stringPara;
		//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
		//		b_bgDown->loadTexture(fullpath);
		//	}
		//	else {
		//		CCLOG("%s this picture is NULL", fullpath.c_str());
		//	}
		//	App::sqliteClose();
		//});
		//_eventDispatcher->addEventListenerWithFixedPriority(listenDownCoverSuccess2, 1);
		//
		////注册封面下载失败的通知
		//auto listenDownCoverFailed2 = EventListenerCustom::create(StringUtils::format("Down_Failed_%d_6", m_castleId_bottom), [=](EventCustom* event) {
		//	BookCityScene::AsyncDownload(m_castleId_bottom, coverUrl2, 6);
		//});
		//_eventDispatcher->addEventListenerWithFixedPriority(listenDownCoverFailed2, 1);
		//
		////注册边框下载成功的通知
		//auto listenDownBoderSuccess2 = EventListenerCustom::create(StringUtils::format("Down_Success_%d_7", m_castleId_bottom), [=](EventCustom* event) {
		//	vector<string> paraStr;
		//	paraStr.push_back("path");
		//	vector<unordered_map<string, ParaType>> result_2 = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, StringUtils::format("where bookId=%d and status=7", m_castleId_bottom));
		//	//设置封面
		//	auto fullpath =  result_2[0]["path"].stringPara;
		//	if (FileUtils::getInstance()->isFileExist(fullpath)) {
		//		b_borderDown->loadTexture(fullpath);
		//	}
		//	else {
		//		CCLOG("%s this picture is NULL", fullpath.c_str());
		//	}
		//	App::sqliteClose();
		//});
		//_eventDispatcher->addEventListenerWithFixedPriority(listenDownBoderSuccess2, 1);
		//
		////注册边框下载失败的通知
		//auto listenDownBoderFailed2 = EventListenerCustom::create(StringUtils::format("Down_Failed_%d_7", m_castleId_bottom), [=](EventCustom* event) {
		//	BookCityScene::AsyncDownload(m_castleId_bottom, boderUrl2, 7);
		//});
		//_eventDispatcher->addEventListenerWithFixedPriority(listenDownBoderFailed2, 1);
		string dir = FileUtils::getInstance()->getWritablePath() + "bookCity";
		string borderfileName = StringUtils::format("border_%d", m_castleId_bottom) + ".png";
		string bgfileName = StringUtils::format("bg_%d", m_castleId_bottom) + ".png";
		auto borderpath = dir + "/" + borderfileName;
		auto bgpath = dir + "/" + bgfileName;
		if (FileUtils::getInstance()->isFileExist(borderpath))
		{
			b_borderDown->loadTexture(borderpath);
		}
		if (FileUtils::getInstance()->isFileExist(bgpath))
		{
			b_bgDown->loadTexture(bgpath);
		}
		string listenerKey = StringUtils::format("bookcity+castleId=%d", m_castleId_bottom);
		auto listener = EventListenerCustom::create(listenerKey, [=](EventCustom* e) {
			string dir = FileUtils::getInstance()->getWritablePath() + "bookCity";
			string borderfileName = StringUtils::format("border_%d", m_castleId_bottom) + ".png";
			string bgfileName = StringUtils::format("bg_%d", m_castleId_bottom) + ".png";
			auto borderpath = dir + "/" + borderfileName;
			auto bgpath = dir + "/" + bgfileName;
			if (FileUtils::getInstance()->isFileExist(borderpath))
			{
				auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "bookCityClickButton", 0);
				if (click == 1)
					return;
				b_borderDown->loadTexture(borderpath);
			}
			if (FileUtils::getInstance()->isFileExist(bgpath))
			{
				auto click = YYXStruct::getMapInt64(App::GetInstance()->myData, "bookCityClickButton", 0);
				if (click == 1)
					return;
				b_bgDown->loadTexture(bgpath);
			}
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, button);
	}
	//=========================================================================================================================================

}