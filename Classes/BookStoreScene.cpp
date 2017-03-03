#include "BookStoreScene.h"
#include "NetIntface.h"
#include "YYXVisitor.h"

using namespace cocostudio::timeline;

//std::map<int, map<std::string, ParaType>>* BookStore::m_viewPageData = nullptr;

int BookStore::isNetConnect = 0;//是否联网
//std::mutex BookStore::m_mutex_cover;

BookStore::~BookStore()
{
	App::log("BookStore::~BookStore()");
	
	App::log("BookStore::~BookStore()---END");
}

Scene* BookStore::createScene(int BookStoreId)
{	
	App::log("BookStore::createScene");
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = BookStore::create(BookStoreId);

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
	App::log("BookStore::createScene--END");
    return scene;
}

BookStore* BookStore::create(int BookStoreId) {
	App::log("BookStore::create");
	BookStore *pRet = new(std::nothrow) BookStore();
		if (pRet && pRet->init(BookStoreId))
		{
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = nullptr;
			return nullptr;
		} 
}

// on "init" you need to initialize your instance
bool BookStore::init(int BookStoreId)
{
	App::log("BookStore::init");
    if ( !Layer::init() )
    {
        return false;
    }	
	 
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
	//		Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(m_pageView, false);
	//	}
	//	return true;
	//};
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(lister_high, -200);
	//等待层
	YYXVisitor::getInstance()->bookStoreSceneInit();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();	
	m_click = true;
	m_selectPageIndex = new int[3];
	m_selectPageIndex[0] = -1;
	int index = 0;
	index = atoi(App::GetInstance()->m_showSceneData.stringData.c_str());
	if (index == -999  || index<0  || index >1000)
		index = 0;
	m_selectPageIndex[1] = index;
	m_selectPageIndex[2] = -1;
	App::log("*******", m_selectPageIndex[1]);
	//得到书店ID,从数据库中读取书店信息,修改书店名称
	m_bookStoreId = BookStoreId;
	//---------------------------背景层-------------------------------	
	Data data;
	Node* background;
	if (App::GetInstance()->getData(BOOKSTORE_BACKGROUND_CSB,data))
		background = CSLoader::createNode(data);
	else
		background = CSLoader::createNode(BOOKSTORE_BACKGROUND_CSB);
	background->setAnchorPoint(Vec2(0.5f, 0.5f));
	background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	//店名
	m_bookstorename = ( Text*)background->getChildByName(FIND_BUTTON_BY_NAME_BOOKSTORE_BOOKSTORENAME);
	
	if (App::isNight() == false)
	{
		auto chuang = (Sprite*)background->getChildByName(FIND_SPRITE_BY_NAME_BOOKSTORE_NIGHT);
		chuang->setVisible(false);
	}
	this->addChild(background);
	//展示几分之几页
	//页数圆圈节点
	pageCircle = Node::create();
	pageCircle->setPosition(Vec2(0, 0));
	addChild(pageCircle, 1);
	//翻页圆圈
	moveCircle = Label::create(App::getString("CIRCLE"), "wryh.TTF", 65);
	moveCircle->setScale(0.5f);
	moveCircle->setPosition(Vec2(-100, -100));
	addChild(moveCircle, 1);
	//-----------------------------背景层END---------------------------------------

	//----------------------------------返回按钮---------------------------------------------------------------------------
	m_createTime = App::getCurrentTime();
	//返回按钮 添加事件
	auto b_back = (Button*)background->getChildByName(FIND_BUTTON_BY_NAME_BOOKSTORE_BACK);
	b_back->removeFromParentAndCleanup(true);
	b_back->setAnchorPoint(Vec2(0, 1));
	b_back->setPosition(origin + Vec2(14, visibleSize.height - 14));
	addChild(b_back, 10);
	b_back->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "bookstoreSceneBackButtonTime", [=]() {
			addorStartQueue([=]() {
				back();
			}, "back();");
		});
	});
	auto bookroom = (Button*)background->getChildByName("Button_1");
	if (bookroom)
	{
		bookroom->removeFromParent();
		bookroom->setAnchorPoint(Vec2(1, 1));
		bookroom->setPosition(origin + visibleSize + Vec2(-14, -14));
		addChild(bookroom, 10);
		bookroom->setScale(0.4);
		bookroom->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "bookstoreSceneBookRoomButtonTime", [=]() {
				addorStartQueue([=]() {
					App::GetInstance()->pushScene(m_backScene, m_bookStoreId, StringUtils::format("%d", m_selectPageIndex[1]));
					Index::GoToBookRoomScene();
				}, "GoToBookRoomScene");
			});
		});
	}
	//----------------------------------返回按钮END---------------------------------------------------------------------------

	//-----------------------------初始化书店名称---------------------------------------
	//火车车厢的书店名称是固定的,通过判断实现
	std::string BookStoreName = string(App::getString("UNKNOEPRICE"));
	m_visitFrom = "0";
	switch (m_bookStoreId)
	{
		//绘本
	case P_TRAIN_2:
		BookStoreName = App::getString("STR_PICTUREBOOK");
		m_backScene = MySceneName::PictureBook;
		break;
	case P_TRAIN_3:
		BookStoreName = App::getString("STR_RECOMMEND");
		m_backScene = MySceneName::Recommend;
		break;
	case P_TRAIN_4:
		BookStoreName = App::getString("STR_LIMITFREE");
		m_backScene = MySceneName::Free;
		break;
	case P_TRAIN_5:
		BookStoreName = App::getString("STR_EXCELLENT_FIVE_STAR");
		m_backScene = MySceneName::GoodReputation;
		break;
	case P_TRAIN_6:
		BookStoreName = App::getString("STR_NEWBOOK");
		m_backScene = MySceneName::NewBook;
		break;
	case P_TRAIN_7:
		BookStoreName = App::getString("STR_KANGXUANBOOKSTORE");
		m_backScene = MySceneName::KangXuanStore;
		break;
	case BOOKSTOREID_TRAIN_8:
		BookStoreName = App::getString("VIPZHUANSHUSHUDIAN");
		m_backScene = MySceneName::VIPBOOK;
		break;
	default:
		//书城的书店名称是通过网络接口实现,当前只需要去数据库查询出来
		m_visitFrom = "1";
		m_backScene = MySceneName::BookCityCHILD;
	//	auto result = App::sqliteStoreInfo(App::sqliteOpen(), m_bookStoreId);
	//	if (result.size() > 0)
	//	{
	//		auto value = result[0]["castleName"];
	//		BookStoreName = value.stringPara;
	//	}
	//	else
	//	{
	//		App::log("function error: sqliteStoreInfo");
	//	}
	//	break;
	};
	m_bookstorename->setString(BookStoreName);
	//m_bookstorename->addClickEventListener([=](Ref* sender) {
	//	RenderTexture* renderTexture = RenderTexture::create(visibleSize.width, visibleSize.height, Texture2D::PixelFormat::RGBA8888);
	//	renderTexture->begin();
	//	Director::getInstance()->getRunningScene()->visit();
	//	renderTexture->end();
	//	string dir = FileUtils::getInstance()->getWritablePath() + "temp";
	//	if (!FileUtils::getInstance()->isDirectoryExist(dir))
	//		FileUtils::getInstance()->createDirectory(dir);
	//	renderTexture->saveToFile(StringUtils::format("temp/share_%d.png", (int)YYXLayer::getRandom()), false, [=](RenderTexture* t, const string& path) {
	//		auto headpath = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildHeadPortrait", "");
	//		App::log(path);
	//		NetIntface::share(path, BookStoreName, "www.ellabook.cn", headpath, "");
	//	});
	//});
	App::m_RunningScene = m_backScene;
	//查询书店信息
	if (m_visitFrom == "1")
	{
		/*auto result = App::sqliteStoreInfo(App::sqliteOpen(), m_bookStoreId);
		if (result.size() > 0)
		{
		auto value = result[0]["castleName"].stringPara;
		if (value.length() > 0)
		m_bookstorename->setString(value);
		}
		else
		App::log("书店信息查询为空");*/
		string castleNameKey = StringUtils::format("castleName+castleId=%d", m_bookStoreId);
		auto name = YYXStruct::getMapString(App::GetInstance()->myData, castleNameKey, "");
		m_bookstorename->setString(name);
	}
	//-----------------------------初始化书店名称END---------------------------------------
	//初始化木马
	auto moveViews = background->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOKPAGE);
	if(moveViews)
		refershThreePages(moveViews);
	//---------------------------启动子线程请求书店的当前页书籍列表信息-------------------------------
	getCurrentlyPageBookListInfo(BookStoreId, m_selectPageIndex[1]);
	//启动一个控制刷新的子线程
	//m_needRefre = 0;
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreNeedRefresh", 0);
	//启动定时器
	schedule([=](float f) {
		int needRefersh = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreNeedRefresh", 0);
		if (needRefersh > 0 && !m_move)
		{
			App::log("刷新前", needRefersh);
			//refershThreePages(moveViews);
			addorStartQueue([=]() {
				refershThreePages(moveViews);
			}, "refershThreePages(moveViews);");
			auto temp = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreNeedRefresh", 0) - needRefersh;
			if (temp >= 0)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreNeedRefresh", temp);
			else
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreNeedRefresh", 0);
		}
	}, 1, "bookStoreSceneReferUISchedule");
	//-----------------------------控制点击间隔时间 以及轮子转动和循环滚动----------------------------------------------------------
	auto touchmove = EventListenerTouchOneByOne::create();
	touchmove->onTouchBegan = [=](Touch* t, Event* e) {
		App::log("BookStore touchmove");
		m_click = true;
		return true;
	};
	touchmove->onTouchMoved = [=](Touch* t, Event* e) {
		App::log("BookStore touchmove onTouchMoved");
		auto moveLocation = t->getStartLocation() - t->getLocation();
		if (moveLocation.x > 30 || moveLocation.x < -30 || moveLocation.y > 30 || moveLocation.y < -30)
			m_click = false;		
	};
	touchmove->onTouchEnded = [=](Touch* t, Event* e) {
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchmove, -100);
	auto touchlistener = EventListenerTouchOneByOne::create();
	touchlistener->onTouchBegan = [=](Touch* t, Event* e) {
		App::log("BookStore touchlistener");
		return true;
	};
	touchlistener->onTouchMoved = [=](Touch* t, Event* e) {
		App::log("BookStore touchlistener onTouchMoved");
		auto moveLocation = t->getStartLocation() - t->getLocation();
		auto key = "bookstoreMoveTime";
		if (moveLocation.x > 100)
		{				//向右
			if(!m_move)
				YYXLayer::controlTouchTime(2, key, [=]() {
				//viewMoveAnimation(moveViews, false);
				addorStartQueue([=]() {
					viewMoveAnimation(moveViews, false);
				}, "viewMoveAnimation(moveViews, false);");
			});
		}
		else if (moveLocation.x < -100)
		{				//向左
			if(!m_move)
				YYXLayer::controlTouchTime(2, key, [=]() {
				addorStartQueue([=]() {
					viewMoveAnimation(moveViews, true);
				}, "viewMoveAnimation(moveViews, true);");
			});
		}
		else
			m_move = false;
	};
	touchlistener->onTouchEnded = [=](Touch* t, Event* e) {
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchlistener, 1);
	//通知点击书籍跳转详情
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("bookStoreSceneClickBook", [=](EventCustom* e) {
		auto bookid = (int)e->getUserData();
		//auto bookid = data->getInt64Data(*data, -999);
		if (bookid != -999)
		{
			addorStartQueue([=]() {
				App::log("点击书籍跳转bookid = ", bookid);
				App::GetInstance()->pushScene(m_backScene, m_bookStoreId, StringUtils::format("%d", m_selectPageIndex[1]));
				//leave();
				Index::GoToBookInfo(bookid);
			}, "点击书籍跳转bookid");
		}
		//if (data)
			//delete data;
	});
	//通知下载封面完成
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("bookstoreCoverDownloadSuccess", [=](EventCustom* e) {
		auto temp = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreNeedRefresh", -999);
		if (temp > 0)
			temp += 1;
		else
			temp = 1;
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreNeedRefresh", temp );
	});
	
	//添加一个图层 监听分配给图层 图层控制轮子转动和循环滚动
	//m_lister = EventListenerTouchOneByOne::create();
	//m_lister->onTouchBegan = [=](Touch* t, Event* e) {
	//	//CCLOG("onTouchBegan");
	//	m_click = true;
	//	m_touch = true;
	//	if (m_pageCount < 2)
	//		return false;
	//	//获取轮子,然后记录角度
	//	getAllWheel(m_pageView);
	//	for (Node* it : m_wheel)
	//	{
	//		m_Rotation.push_back(it->getRotation());
	//	}
	//	//CCLOG("startRotation = %f", m_wheel.at(2)->getRotation());
	//	return true;
	//};
	//m_lister->onTouchMoved = [=](Touch* t, Event* e) {
	//	if (/*YYXLayer::getBoolFromXML(SOUND_KEY) && */(App::getCurrentTime() - m_soundTime)>2)
	//	{
	//		//SimpleAudioEngine::getInstance()->playEffect(ELLA_SOUND_CASTLENEXTPAGE);
	//		m_soundTime = App::getCurrentTime();
	//	}		
	//	if (m_pageCount < 2)
	//		return;
	//	//设置角度,轮子滚动
	//	auto moveLocation = t->getStartLocation() - t->getLocation();
	//	if (moveLocation.x > 30 || moveLocation.x < -30 ||	moveLocation.y > 30 || moveLocation.y < -30)
	//		m_click = false;
	//	//根据木马移动的距离算轮子的滚动角度
	//	auto move_x = (0 - m_pageView->getPage(1)->getPosition().x) / 2.5;
	//	for (ssize_t i = 0; i < m_wheel.size(); i++)
	//	{
	//		m_wheel.at(i)->setRotation(m_Rotation.at(i) - move_x);
	//	}
	//};
	//m_lister->onTouchEnded = [=](Touch* t, Event* e) {
	//	//CCLOG("onTouchEnded");		
	//	if (m_pageCount < 2)
	//	{
	//		m_touch = false;
	//		return;
	//	}
	//	//动态计算页面,循环滚动pageview
	//	if (m_pageView->getPage(1)->getPosition().x >= m_ScrollThreshold)
	//	{
	//		//右滚一页 -->>滚动完,当前页面的下标是0, 计算第0项item 属于内容的第几页			
	//		schedule([=](float f) {
	//			//计算page自行滚动的移动距离,计算出轮子角度
	//			getAllWheel(m_pageView);
	//			auto move_x = 0 - m_pageView->getPage(1)->getPosition().x;
	//			for (ssize_t i = 0; i < m_wheel.size(); ++i)
	//			{
	//				m_wheel.at(i)->setRotation(m_Rotation.at(i) - move_x / 3);
	//			}
	//			//滚动结束后,终止计时器,然后重新建立pageview
	//			if (m_pageView->getPage(1)->getPosition().x >= 1093.98)
	//			//if (m_pageView->getCurPageIndex()==0)
	//			{
	//				m_selectPageIndex[1] = m_selectPageIndex[1] - 1;
	//				changPageNumber();					
	//				unschedule(TIMER_RIGHTROLLING);
	//				notify(REFRESHUI);
	//				notify(HTTPBOOKINFO);
	//				m_touch = false;
	//				m_refre = 0;
	//			}
	//		}, TIMER_RIGHTROLLING);
	//	}
	//	else if (m_pageView->getPage(1)->getPosition().x <= -1 * m_ScrollThreshold)
	//	{
	//		//左滚一页  <<---滚动完,当前页面的下标是2,计算第2项item属于内容的第几页			
	//		schedule([=](float f) {
	//			//计算page自行滚动的移动距离,计算出轮子角度
	//			getAllWheel(m_pageView);
	//			auto move_x = 0 - m_pageView->getPage(1)->getPosition().x;
	//			for (ssize_t i = 0; i < m_wheel.size(); ++i)
	//			{
	//				m_wheel.at(i)->setRotation(m_Rotation.at(i) - move_x / 5);
	//			}
	//			//滚动结束后,终止计时器,然后重新建立pageview
	//			if (m_pageView->getPage(1)->getPosition().x <= -1093.98)
	//			{
	//				m_selectPageIndex[1] = m_selectPageIndex[1] + 1;
	//				changPageNumber();
	//				unschedule(TIMER_LEFTROLLING);
	//				notify(REFRESHUI);
	//				notify(HTTPBOOKINFO);
	//				m_touch = false;
	//				m_refre = 0;
	//			}
	//		}, TIMER_LEFTROLLING);
	//	}
	//	else
	//	{
	//		m_touch = false;
	//	}
	//};
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_lister, -1);
	//m_lister->setEnabled(false);
	//-----------------------------控制轮子转动和循环滚动----------------------------------------------------------
/*
	//----------------------------------接受通知<刷新界面>---------------------------------------------------------------------------
	auto lister_redraw = EventListenerCustom::create(NOTIFY_REFRESHUI, [=](EventCustom* event) {
		if(moveViews)
			refershThreePages(moveViews);
	});
	_eventDispatcher->addEventListenerWithFixedPriority(lister_redraw, 1);
	//----------------------------------接受通知<刷新界面>---------------------------------------------------------------------------

	//----------------------------------接受通知<请求封面>---------------------------------------------------------------------------
	//auto lister_Cover = EventListenerCustom::create(NOTIFY_HTTPCOVER, [=](EventCustom* event) {
	//	++m_needRefre;
	//	CCLOG("[开始封面下载]");	
	//	//用页码找出bookid
	//	auto page = (int)(event->getUserData());
	//	auto sorts = getSortsOfThisPage(page);
	//	auto countover = new int();
	//	*countover = sorts.size();
	//	auto httpFunction = [=](int it) {
	//		std::thread httpImage(&BookStore::addHttpQueue_httpDownLoadImage, this, it, countover);
	//		httpImage.detach();
	//	};
	//	for (auto it:sorts)
	//	{
	//		auto path = (*m_viewPageData)[it]["path"].stringPara;
	//		auto downloadtime = (*m_viewPageData)[it]["downloadtime"].intPara;
	//		auto bookId = (*m_viewPageData)[it]["bookId"].intPara;
	//		auto time = App::getCurrentTime() - downloadtime;
	//		//判断是否有资源 有就用 
	//		if (App::IsHaveFile(path)  && time < NET_UPDATE_COVER_TIME_INTERVAL * 60)
	//		{//判断资源是否存在 并且有效
	//			//count4lock(m_mutex_cover, countover);
	//			continue;
	//		}
	//		else
	//		{
	//			//无资源,再去数据库查一下 确认是否有资源
	//			auto res = App::sqliteBookPicture(App::sqliteOpen(), bookId);
	//			if (res.size() > 0)
	//			{
	//				path = res[0]["path"].stringPara;
	//				downloadtime = res[0]["downloadtime"].intPara;
	//				auto time1 = App::getCurrentTime() - downloadtime;
	//				if (App::IsHaveFile(path) && time1 < NET_UPDATE_COVER_TIME_INTERVAL * 60)
	//				{//有效,更新数据源
	//					App::initParaType((*m_viewPageData)[it], "path", -999, path);
	//					App::initParaType((*m_viewPageData)[it], "downloadtime", downloadtime, "");
	//					//count4lock(m_mutex_cover, countover);
	//					++m_needRefre;
	//					continue;
	//				}
	//				else
	//					httpFunction(it);
	//			}
	//			else
	//				httpFunction(it);
	//		}		
	//	}
	//	////判断封面请求是否全部结束,超时10秒直接通知刷新
	//	//std::thread pthreadIsHttpCoverOver(&BookStore::IsHttpCoverOver, this, countover);
	//	//pthreadIsHttpCoverOver.detach();
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(lister_Cover, 1);
	//----------------------------------接受通知<请求封面>---------------------------------------------------------------------------

	//-----------------------------------接受通知<请求当前界面的书籍简介>------------------------------------------------------------
	auto lister_bookInfo = EventListenerCustom::create(NOTIFY_HTTPBOOKINFO, [=](EventCustom* event) {
		//std::thread bookinfo(&BookStore::httpBooksInfo, this, m_bookStoreId);
		//bookinfo.detach();
		getCurrentlyPageBookListInfo(BookStoreId, m_selectPageIndex[1]);
	});
	_eventDispatcher->addEventListenerWithFixedPriority(lister_bookInfo, 1);
	//-----------------------------------接受通知<请求当前界面的书籍简介>------------------------------------------------------------


	//-------------------------------------接受通知<去掉等待屏蔽层,开启滚动监听>-----------------------------------------------------------------------------------------------------------------------
	auto removeWait =  EventListenerCustom::create("REMOVEWAIT", [=](EventCustom* event) {
		//去掉等待层
		if (this == nullptr)
			return;
		auto wait = (Layer*)getChildByName("waitLayer");
		if (wait != nullptr && wait->getParent() != nullptr)
		{
			wait->removeFromParent();
		}
		m_lister->setEnabled(true);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(removeWait, 1);
	//--------------------------------------接受通知<去掉等待屏蔽层,开启滚动监听>---------------------------------------------------------------------------------------------

	//--------------------------------接受通知<返回上级按钮设置开启点击状态>----------------------------------------------
	auto lister_homeButton = EventListenerCustom::create("HOMEBUTTOMSETTRUE", [=](EventCustom* event) {
		b_back->setTouchEnabled(true);
	});
	_eventDispatcher->addEventListenerWithFixedPriority(lister_homeButton, 1);
	//------------------------------接受通知<返回上级按钮设置开启点击状态>------------------------------------------------
	App::sqliteClose();*/

	////安卓返回键
	//auto androidListener = EventListenerKeyboard::create();
	//androidListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
	//	if (!App::GetInstance()->isBack)
	//		return;
	//	switch (keyCode)
	//	{
	//	case EventKeyboard::KeyCode::KEY_ESCAPE:
	//		back();
	//		App::backThreading();
	//		break;
	//	default:
	//		break;
	//	}
	//};
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(androidListener, this);


	_eventDispatcher->addCustomEventListener("showPageNumber", [=](EventCustom* e) {
		showBookPageNumber();
	});
	App::log("BookStore::init--END");
    return true;
}

void BookStore::onEnterTransitionDidFinish()
{
	App::log("BookStore::onEnterTransitionDidFinish");
	//App::sqliteClose();	
	Layer::onEnterTransitionDidFinish();	
	App::log("当前场景:", App::GetInstance()->m_RunningScene);
	Toast::GetInstance()->SceneInitToast();
	if (m_bookStoreId == BOOKSTOREID_TRAIN_8)
	{
		baonianStore();
	}
	App::log("BookStore::onEnterTransitionDidFinish--END");
}

//初始化viewpage
//void BookStore::init4ViewPage()
//{
//	App::log("BookStore::init4ViewPage");
//	auto visibleSize = Director::getInstance()->getVisibleSize();
//	Data data;
//	if (App::GetInstance()->getData(BOOKSTORE_PAGE_CSB, data))
//	{
//		m_book1 = CSLoader::createNode(data);
//		m_book2 = CSLoader::createNode(data);
//		m_book3 = CSLoader::createNode(data);
//	}
//	else
//	{
//		m_book1 = CSLoader::createNode(BOOKSTORE_PAGE_CSB);
//		m_book2 = CSLoader::createNode(BOOKSTORE_PAGE_CSB);
//		m_book3 = CSLoader::createNode(BOOKSTORE_PAGE_CSB);
//	}
//	m_book1->setAnchorPoint(Vec2(0.5f, 0.5f));
//	m_book1->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
//	m_book1->retain();
//	m_book2->setAnchorPoint(Vec2(0.5f, 0.5f));
//	m_book2->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
//	m_book2->retain();
//	m_book3->setAnchorPoint(Vec2(0.5f, 0.5f));
//	m_book3->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
//	m_book3->retain();
//	std::vector<Vec2> vposition;
//	vposition.push_back(Vec2(385 - 66, 318));
//	vposition.push_back(Vec2(548 - 66, 318));
//	vposition.push_back(Vec2(710 - 66, 318));
//	vposition.push_back(Vec2(873 - 66, 318));
//	vposition.push_back(Vec2(385 - 66, 106));
//	vposition.push_back(Vec2(548 - 66, 106));
//	vposition.push_back(Vec2(710 - 66, 106));
//	vposition.push_back(Vec2(873 - 66, 106));
//	//初始化 书籍节点,viewpage虽然被删除,但是page反复被使用
//	//m_vbook1前一页
//	std::for_each(vposition.begin(), vposition.end(), [=](Vec2& item) {
//		Node* book;
//		Data data;
//		if (App::GetInstance()->getData(BOOKSTORE_BOOKVIEW_CSB, data))
//			book = CSLoader::createNode(data);
//		else
//			book = CSLoader::createNode(BOOKSTORE_BOOKVIEW_CSB);
//		book->retain();
//		m_vbook1.push_back(book);
//		m_book1->addChild(book);
//		book->setPosition(item);
//	});
//	//m_vbook2当前展示页
//	std::for_each(vposition.begin(), vposition.end(), [=](Vec2& item) {
//		Node* book;
//		Data data;
//		if (App::GetInstance()->getData(BOOKSTORE_BOOKVIEW_CSB, data))
//			book = CSLoader::createNode(data);
//		else
//			book = CSLoader::createNode(BOOKSTORE_BOOKVIEW_CSB);
//		book->retain();
//		m_vbook2.push_back(book);
//		m_book2->addChild(book);
//		book->setPosition(item);
//	});
//	//m_vbook3后一页
//	std::for_each(vposition.begin(), vposition.end(), [=](Vec2& item) {
//		Node* book;
//		Data data;
//		if (App::GetInstance()->getData(BOOKSTORE_BOOKVIEW_CSB, data))
//			book = CSLoader::createNode(data);
//		else
//			book = CSLoader::createNode(BOOKSTORE_BOOKVIEW_CSB);
//		book->retain();
//		m_vbook3.push_back(book);
//		m_book3->addChild(book);
//		book->setPosition(item);
//	});
//	//loadTheDataIntoMemory();
//	refreshUI();
//	App::log("BookStore::init4ViewPage---END");
//}

////判断封面请求是否全部结束
//void BookStore::IsHttpCoverOver(int* count)
//{
//	App::log("BookStore::IsHttpCoverOver");
//	auto start = App::getCurrentTime();
//	while (*count > 0)
//	{
//		CCLOG("[判断封面下载是否结束...%d]", *count);
//		App::ccsleep(300);
//		auto current = App::getCurrentTime();
//		auto move = current - start;
//		if (move > 10)
//			break;
//	}
//	if (count)
//	{
//		delete count;
//	}
//	count = nullptr;
//	CCLOG("[发送通知:请求封面结束]IsHttpCoverOver");
//	++m_needRefre;
//	App::log("BookStore::IsHttpCoverOver---END");
//}

////请求网络图片下载到本地
//void BookStore::httpDownLoadImage(int sort, int* count)
//{
//	App::log("BookStore::httpDownLoadImage");
//	if (!CocosAndroidJni::IsNetConnect())
//	{
//		//count4lock(m_mutex_cover, count);
//		return;
//	}
//	//到数据源中找出url
//	auto data = (*m_viewPageData)[sort];
//	int bookid = -999;
//	if (data.find("bookId") != data.end())
//	{
//		bookid = data["bookId"].intPara;
//	}
//	if (bookid == -999) {
//		//count4lock(m_mutex_cover, count);
//		CCLOG("error: httpDownLoadImage bookId == -999");
//		return;
//	}
//	std::string bookPicUrl="";
//	if (data.find("coverURL") != data.end())
//	{
//		bookPicUrl = data["coverURL"].stringPara;
//	}
//	if (bookPicUrl.length() < 20)
//	{
//		CCLOG("error: bookid = %d[没有下载地址,无法下载...]", bookid);
//		//count4lock(m_mutex_cover, count);
//		return;
//	}
//	HttpRequest* pRequest = new HttpRequest();
//	CCLOG("[图片下载]=%s", bookPicUrl.c_str());
//	pRequest->setUrl(bookPicUrl.c_str());
//	pRequest->setRequestType(HttpRequest::Type::GET);
//	pRequest->setResponseCallback([=](HttpClient* client, HttpResponse* response)
//	{
//			if (!response)
//			{
//				//count4lock(m_mutex_cover, count);
//				CCLOG("error HttpResponse is nullptr");
//				return;
//			}
//			if (!response->isSucceed()) {
//				CCLOG("connect failed!  error buffer: %s", response->getErrorBuffer());
//				//count4lock(m_mutex_cover, count);
//				return;
//			}
//			std::vector<char> *buffer = response->getResponseData();
//			CCLOG("[下载的数据大小] = %d", buffer->size());
//			auto buf = new char[buffer->size()];
//			std::copy(buffer->begin(), buffer->end(), buf);
//			std::thread callback(&BookStore::httpDownLoadImageCallback, this, buf, buffer->size(), bookid, sort,  count);
//			callback.detach();
//	});
//	HttpClient::getInstance()->send(pRequest);
//	pRequest->release();
//	App::log("BookStore::httpDownLoadImage--END");
//}
//
////请求网络图片下载到本地回调函数
//void BookStore::httpDownLoadImageCallback(char* buf,int length,int bookId,int sort, int* count)
//{
//	App::log("BookStore::httpDownLoadImageCallback");
//	auto dir = FileUtils::getInstance()->getWritablePath() + "bookCover/";
//	std::string fileName = StringUtils::format("%d", bookId) + ".png";
//	if (!FileUtils::getInstance()->createDirectory(dir)) {
//		CCLOG("error : createDirectory %s ", dir.c_str());
//		return;
//	}
//	FILE *fp = fopen((dir + "/" + fileName).c_str(), "wb+");
//	fwrite(buf, 1, length, fp);
//	fclose(fp);
//	CC_SAFE_DELETE_ARRAY(buf);
//	//更新数据源
//	auto time = App::getCurrentTime();
//	auto path = dir+ fileName;
//	++m_needRefre;
//	auto db = SqliteManager::OpenDataBase();
//	//判断是否有记录 有则删除
//	SqliteManager::DeleteData(db, DB_RES, StringUtils::format("where bookId = %d and status = 0", bookId).c_str());
//	//插入数据库
//	unordered_map<string, string> paraInfoSTR;
//	unordered_map<string, long long> paraInfoINT;
//	paraInfoINT["bookId"] = bookId;
//	paraInfoINT["status"] = 0;
//	paraInfoINT["downloadtime"] = time;
//	paraInfoSTR["path"] = path;
//	SqliteManager::InsertData(db, DB_RES, paraInfoSTR, paraInfoINT);
//	SqliteManager::CloseDataBase(db);
//	//判断内存容器中是否已经存在,
//	if (m_viewPageData != nullptr)
//	{
//		if ((*m_viewPageData).find(sort) == (*m_viewPageData).end())
//		{
//			//不存在则插入
//			std::map<std::string, ParaType> value;
//			App::initParaType(value, "path", -999, path);
//			App::initParaType(value, "downloadtime", time, "");
//			(*m_viewPageData)[sort] = value;
//		}
//		else
//		{
//			//存在则更新数据
//			App::initParaType((*m_viewPageData)[sort], "path", -999, path);
//			App::initParaType((*m_viewPageData)[sort], "downloadtime", time, "");
//		}
//	}
//	CCLOG("bookId = %d[网络图片下载结束]", bookId);
//	App::log("BookStore::httpDownLoadImageCallback--END");
//}

//获取全部轮子
//void BookStore::getAllWheel(ui::PageView* pageview)
//{
//	App::log("BookStore::getAllWheel");
//	m_wheel.clear();
//	for (size_t i = 0; i < pageview->getPages().size(); i++)
//	{
//		auto page = pageview->getPage(i);
//		auto layer = page->getChildByTag(999); 
//		if (layer == nullptr)
//			return;
//		auto lun1 = ((Layer*)layer)->getChildByName(FIND_SPRITE_BY_NAME_BOOKSTORE_FRONTWHEEL);
//		auto lun2 = ((Layer*)layer)->getChildByName(FIND_SPRITE_BY_NAME_BOOKSTORE_BACKWHEEL);
//		auto lun3 = ((Layer*)layer)->getChildByName(FIND_SPRITE_BY_NAME_BOOKSTORE_MINWHEEL);
//		m_wheel.pushBack(lun1);
//		m_wheel.pushBack(lun2);
//		m_wheel.pushBack(lun3);
//	}
//	App::log("BookStore::getAllWheel--END");
//}

//PageView* BookStore::createPageView()
//{
//	App::log("BookStore::createPageView");
//	//pageview
//	auto pageview = ui::PageView::create();
//	pageview->setContentSize(Size(1094, 614));
//	pageview->setAnchorPoint(Vec2(0, 0));
//	pageview->setPosition(Vec2(0, 0));
//	pageview->setTouchEnabled(true);
//	m_ScrollThreshold = 80;
//	pageview->setCustomScrollThreshold(m_ScrollThreshold);
//	pageview->setSwallowTouches(false);
//	App::log("BookStore::createPageView--END");
//	return pageview;
//}

//翻页时,计算内容页码
bool BookStore::changPageNumber()
{
	App::log("BookStore::changPageNumber");
	//总页数
	string totalCountKey = StringUtils::format("totalCount+bookStoreID=%d", m_bookStoreId);
	auto totalCount = YYXStruct::getMapInt64(App::GetInstance()->myData, totalCountKey, 0);
	int count = ceil(totalCount / 8.0);
	if (count < 2)
		return false;
	int left = m_selectPageIndex[0];
	int min = m_selectPageIndex[1];
	int right = m_selectPageIndex[2];
	if (min < 0)
		min = count - 1;
	if (min > count - 1)
		min = 0;
	left = min - 1;
	right = min + 1;
	if (right > count - 1)
		right = 0;
	if (left < 0)
		left = count - 1;
	m_selectPageIndex[0] = left;
	m_selectPageIndex[1] = min;
	m_selectPageIndex[2] = right;
	CCLOG("[count] = %d", count);
	CCLOG("[left] = %d", m_selectPageIndex[0]);
	CCLOG("[min] = %d", m_selectPageIndex[1]);
	CCLOG("[right] = %d", m_selectPageIndex[2]);
	return true;
	App::log("BookStore::changPageNumber--END");
}

//void BookStore::closeTime()
//{
//	App::log("BookStore::closeTime");
//	unschedule("EventTimeIsOneSecond");
//	unschedule(TIMER_RIGHTROLLING);
//	unschedule(TIMER_LEFTROLLING);
//	App::log("BookStore::closeTime--END");
//}

////网络请求本店的当前页的书籍信息
//void BookStore::httpBooksInfo(int BookStoreId)
//{	
//	App::log("BookStore::httpBooksInfo");
//	m_httpBookListTime = App::getCurrentTime();
//	if (!CocosAndroidJni::IsNetConnect())
//	{
//		notify(REMOVEWAITLayer);
//		Toast::create(App::getString("MESSAGEBOX_NETCONNECTERROR"));
//		return;
//	}
//	int pageIndex = m_selectPageIndex[1] + 1;
//	HttpRequest* request = new HttpRequest();
//	request->setRequestType(HttpRequest::Type::GET);
//	auto strurl = std::string(IP).append(NET_BOOKSID).append(StringUtils::format("%d", BookStoreId))
//		.append("?page=").append(StringUtils::format("%d", pageIndex))
//		.append("&pagesize=").append("8")
//		.append("&resource=").append(App::m_resource)
//		.append("&memberId=").append(App::getMemberID())
//		.append("&visitFrom=").append(m_visitFrom);
//	request->setUrl(strurl.c_str());
//	App::log ("书店当前页书籍列表"+strurl);
//	//请求的回调函数
//	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
//		std::string str4result("");
//		if (!response)
//		{
//			Toast::create( App::getString("MESSAGEBOX_NETCONNECTERROR"));
//			return;
//		}
//		if (!response->isSucceed())
//		{
//			App::log("response failed");
//			return;
//		}
//		std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
//		
//		//子线程中继续执行回调函数
//		std::thread callback(&BookStore::httpBooksIdResponseCallback,this, str, BookStoreId, pageIndex);
//		callback.detach();
//	});
//	cocos2d::network::HttpClient::getInstance()->send(request);
//	request->release();
//	App::log("BookStore::httpBooksInfo---END");
//}

////网络请求本店的所有书籍信息的回调函数,pageIndex是网络接口(1开始计数)  本地页码是从0开始计数
//void BookStore::httpBooksIdResponseCallback(std::string str, int BookStoreId, int pageIndex)
//{
//	App::log("BookStore::httpBooksIdResponseCallback");
//	if (App::GetInstance()->m_RunningScene < MySceneName::BookCityCHILD)
//	{
//		App::log("BookStore::httpBooksIdResponseCallback--1END  runScene =", App::GetInstance()->m_RunningScene);
//		return;
//	}
//	App::log(str);
//	//-----------------------解析json,获得书店的所有书籍信息---------------------------------------
//	rapidjson::Document doc;
//	doc.Parse<0>(str.c_str());
//	std::vector<int> sorts;
//	auto db = SqliteManager::OpenDataBase();
//	if (doc.HasParseError())
//		CCLOG("json parse error : %s", doc.GetParseError());
//	else
//	{
//		if (doc.IsObject() && doc.HasMember("code"))
//		{
//			rapidjson::Value &code = doc["code"];
//			if (code.IsInt() && code.GetInt() == 0)
//			{
//				//{"code":0, "data" : [{"goodsId":100293, "bookId" : 339, "bookName" : "你愿意和我一起撑伞吗？", "bookCoverUrl" : "", "bookListenUrl" : "", "bookReadUrl" : "", "bookPlayUrl" : "", "bookMakeUrl" : "", "bookPrice" : 3.00, "downloadNum" : 60, "downloadTime" : 0, "hashBuy" : 1, "orderId" : 0, "bookAuthor" : "曹珑", "goods_addtime" : 1454142919, "goods_state" : 1, "goods_verify" : 1, "ossbucket" : null, "osskey" : null, "bookDownloadSize" : 0, "coverKey" : "", "bookPress" : "爱心河精品绘本馆", "payMent" : 54.00, "bookScore" : 0, "bookmarketPrice" : 18.00, "remainTime" : 2550413},
//				if (doc.HasMember("totalCount"))
//				{
//					rapidjson::Value &totalCount = doc["totalCount"];
//					if (totalCount.IsInt())
//					{
//						CCLOG("[网络请求前的书籍总数] = %d", m_storeBookCount);
//						auto conut = totalCount.GetInt();
//						if (conut > 0)
//						{
//							m_storeBookCount = conut;
//							string key = StringUtils::format("%d", BookStoreId);
//							App::GetInstance()->myData[key] = YYXStruct(conut,"",nullptr);
//						}
//						else
//							return;
//						CCLOG("[现在书籍总数] = %d", m_storeBookCount);
//					}
//				}
//				if(pageIndex >= 1)
//					sorts = getSortsOfThisPage(pageIndex - 1);
//				if (sorts.size() == 0) return;
//				if (doc.HasMember("data"))
//				{
//					rapidjson::Value &data = doc["data"];
//					if (data.IsArray())
//					{
//						rapidjson::SizeType count = data.Size()>sorts.size() ? sorts.size() : data.Size();
//						for (rapidjson::SizeType i = 0; i < count; i++)
//						{
//							rapidjson::Value &item = data[i];
//							int sort = sorts[i];
//							int bookId = App::analysisJsonInt(item, "bookId");
//							auto coverURL = App::analysisJsonString(item, "bookCoverUrl");
//							auto price = App::analysisJsonDouble(item, "bookPrice") * 100;
//							long long upTime = App::getCurrentTime();
//							if (bookId == -999)
//								continue;							
//							if (m_viewPageData == nullptr)
//							{
//								CCLOG("数据请求返回时,界面已经释放return1");
//								return;
//							}
//							//判断内存容器中是否已法方经存在,
//							if ((*m_viewPageData).find(sort) == (*m_viewPageData).end())
//							{
//								//不存在则插入
//								std::map<std::string, ParaType> value;
//								App::initParaType(value, "sort", sort, "");
//								App::initParaType(value, "bookId", bookId, "");
//								App::initParaType(value, "coverURL", -999, coverURL);
//								App::initParaType(value, "price", price, "");
//								App::initParaType(value, "upTime", upTime, "");
//								(*m_viewPageData)[sort] = value;
//							}
//							else
//							{
//								//存在则更新数据
//								App::initParaType((*m_viewPageData)[sort], "sort", sort, "");
//								App::initParaType((*m_viewPageData)[sort], "bookId", bookId, "");
//								App::initParaType((*m_viewPageData)[sort], "coverURL", -999, coverURL);
//								App::initParaType((*m_viewPageData)[sort], "price", price, "");
//								App::initParaType((*m_viewPageData)[sort], "upTime", upTime, "");
//							}
//							++m_needRefre;
//							//插入数据库
//							unordered_map<string, string> paraInfoSTR;
//							unordered_map<string, long long> paraInfoINT;
//							paraInfoINT["castleId"] = m_bookStoreId;
//							paraInfoINT["bookId"] = bookId;
//							paraInfoINT["sort"] = sort;
//							paraInfoSTR["coverURL"] = coverURL;
//							paraInfoINT["price"] = price;
//							paraInfoINT["upTime"] = upTime;
//							SqliteManager::DeleteData(db, DB_BOOKLIST, StringUtils::format("where bookId = %d and castleId = %d", bookId, m_bookStoreId));
//							auto insert = SqliteManager::InsertData(db, DB_BOOKLIST, paraInfoSTR, paraInfoINT);
//							//if (!insert)
//							//{
//							//	CCLOG("[书籍简介插入数据库失败]where bookId = %d", bookId);
//							//	std::thread db_operation(&BookStore::addThread_db, this, DB_BOOKLIST, paraInfoSTR, paraInfoINT);
//							//	db_operation.detach();
//							//}
//						}
//						CCLOG("[发送通知:书籍简介信息完毕]");
//						notify(HTTPCOVER, pageIndex - 1);
//					}
//				}
//			}
//		}
//	}
//	SqliteManager::CloseDataBase(db);
//	App::log("BookStore::httpBooksIdResponseCallback--END");
//}

//计数器减一
//void BookStore::count4lock(std::mutex& mutexcover, int* count)
//{
//	if (count)
//	{
//		mutexcover.lock();
//		CCLOG("[计数器减一] = %d", *count);
//		*count = *count - 1;
//		CCLOG("[计数器减一] = %d", *count);
//		mutexcover.unlock();
//	}
//}

////装载数据进内存,初始化场景的时候使用,在主线程中
//void BookStore::loadTheDataIntoMemory()
//{	
//	App::log("BookStore::loadTheDataIntoMemory");
//	auto boosInfo = App::sqliteBookList(App::sqliteOpen(), m_bookStoreId);
//	if(m_storeBookCount == 0)
//		m_storeBookCount = boosInfo.size();
//	m_pageCount = ceil(m_storeBookCount / 8.0);
//	//内存中记录了总的书籍数量
//	string key = StringUtils::format("%d", m_bookStoreId);
//	if (App::GetInstance()->myData.find(key) != App::GetInstance()->myData.end())
//	{
//		int count = App::GetInstance()->myData[key].intData;
//		m_pageCount = ceil(count / 8.0);
//		m_storeBookCount = count;
//	}
//	changPageNumber();
//	for (int i = 0; i < boosInfo.size();i++)
//	{
//		auto sort = boosInfo[i]["sort"].intPara;
//		auto bookId = boosInfo[i]["bookId"].intPara;
//		auto coverURL = boosInfo[i]["coverURL"].stringPara;
//		auto price = boosInfo[i]["price"].intPara;
//		auto upTime = boosInfo[i]["upTime"].intPara;
//		auto res = App::sqliteBookPicture(App::sqliteOpen(), bookId);
//		std::string path = "";
//		int downloadtime = 0;
//		if (res.size() > 0)
//		{
//			path = res[0]["path"].stringPara;
//			downloadtime = res[0]["downloadtime"].intPara;
//		}
//		std::map<std::string, ParaType> value;
//		App::initParaType(value, "sort", sort, "");
//		App::initParaType(value, "bookId", bookId, "");
//		App::initParaType(value, "coverURL", -999, coverURL);
//		App::initParaType(value, "price", price, "");
//		App::initParaType(value, "upTime", upTime, "");
//		App::initParaType(value, "path", -999, path);
//		App::initParaType(value, "downloadtime", downloadtime, "");
//		(*m_viewPageData)[sort] = value;
//	}
//	App::sqliteClose();
//	App::log("BookStore::loadTheDataIntoMemory--END");
//}

//刷新界面
//void BookStore::refreshUI()
//{
//	App::log("BookStore::refreshUI");
//	auto temp = m_needRefre;
//	if (m_touch)
//	{
//		++m_needRefre;
//		return;
//	}
//	//1.记录删除前轮子的角度
//	m_Rotation.clear();
//	for (Node* it : m_wheel)
//	{
//		m_Rotation.push_back(it->getRotation());
//	}
//	//2.删除旧的pageview
//	if (m_pageView != nullptr)
//	{
//		m_pageView->removeAllPages();
//		m_pageView->removeFromParentAndCleanup(true);
//		m_pageView = nullptr;
//	}
//	m_book1->removeFromParent();
//	m_book2->removeFromParent();
//	m_book3->removeFromParent();
//	//3.创建新的pageview
//	m_pageView = createPageView();
//	//4.新的viewpage添加3个子页
//	getView4ViewPage();
//	//5.滚到中间页
//	m_pageView->scrollToPage(1);
//	//6.显示到场景上
//	this->addChild(m_pageView);
//	//7.还原轮子角度
//	getAllWheel(m_pageView);
//	auto wheelcount = m_wheel.size();
//	for (int i = 0; i < wheelcount; i++)
//	{
//		int ro = 0;
//		try{
//			ro = m_Rotation.at(i);
//		}catch (const std::exception& e){	}
//		m_wheel.at(i)->setRotation(ro);
//	}
//	//8.展示第1/5页
//	showBookPageNumber();
//	++m_refre;
//	m_needRefre = m_needRefre - temp;
//	App::log("BookStore::refreshUI--END");
//}

//新的viewpage添加3个子页,如果只有一页则不翻页
//void BookStore::getView4ViewPage()
//{
//	App::log("BookStore::getView4ViewPage");
//	//m_pageCount = ceil(m_storeBookCount / 8.0);
//	string totalCountKey = StringUtils::format("totalCount+bookStoreID=%d", m_bookStoreId);
//	auto totalCount = YYXStruct::getMapInt64(App::GetInstance()->myData, totalCountKey, 0);
//	m_pageCount = ceil(totalCount / 8.0);
//	//没有或者只有1页则不翻页
//	if (m_pageCount <= 1)
//	{
//		CCLOG("pageInit[m_book1]");
//		//pageInit(m_book1, 0, m_vbook1);
//		m_pageView->addPage(pageInit(m_book1, 0, m_vbook1,true));
//	}
//	else
//	{
//		//重新初始化3个子页面
//		auto begin_layout = pageInit(m_book1, m_selectPageIndex[0], m_vbook1,false);
//		auto min_layout = pageInit(m_book2, m_selectPageIndex[1], m_vbook2,true);
//		auto end_layout = pageInit(m_book3, m_selectPageIndex[2], m_vbook3,false);
//		//创建添加
//		m_pageView->addPage(begin_layout);
//		m_pageView->addPage(min_layout);
//		m_pageView->addPage(end_layout);
//	}
//	App::log("BookStore::getView4ViewPage--END");
//}

//展示第1/5页
void BookStore::showBookPageNumber()
{
	App::log("BookStore::showBookPageNumber");
	string totalCountKey = StringUtils::format("totalCount+bookStoreID=%d", m_bookStoreId);
	auto totalCount = YYXStruct::getMapInt64(App::GetInstance()->myData, totalCountKey, 0);
	int count = ceil(totalCount / 8.0);
	//创建页数圆圈
	if(pageCircle->getChildrenCount()>0)
		pageCircle->removeAllChildrenWithCleanup(true);
	if (count <= 0)
		return;
	//notify(REMOVEWAITLayer);
	auto size = Director::getInstance()->getVisibleSize();
	int startPosX = size.width/2 - count * 30 / 2;
	for (int i = 0; i < count; i++) {
		auto pLabel = Label::create(App::getString("CIRCLE"), "wryh.TTF", 65);
		pLabel->setPosition(Vec2(startPosX + i * 30, 20));
		pLabel->setScale(0.5f);
		pLabel->setColor(ccc3(120, 120, 120));
		pageCircle->addChild(pLabel, 0, i);
	}
	auto it = pageCircle->getChildByTag(m_selectPageIndex[1]);
	if(it)
		moveCircle->setPosition(it->getPosition());
	App::log("BookStore::showBookPageNumber--- END");
}

////将下载线程放入队列管理
//void BookStore::addHttpQueue_httpDownLoadImage(int sort,  int* count)
//{
//	App::log("BookStore::addHttpQueue_httpDownLoadImage");
//	if (m_queue)
//	{
//		m_queue->push_back(std::bind(&BookStore::httpDownLoadImage, this, sort, count));
//	}
//	App::log("BookStore::addHttpQueue_httpDownLoadImage--END");
//}
//
//void BookStore::addThread_db(string tableName, unordered_map<string, string> paraInfoSTR, unordered_map<string, long long> paraInfoINT)
//{
//	App::log("BookStore::addThread_db");
//	if (m_queue)
//	{
//		m_queue->push_back(std::bind(&BookStore::thread4insert, this, tableName, paraInfoSTR, paraInfoINT));
//	}
//	App::log("BookStore::addThread_db---END");
//}
//
////数据操作失败,开线程插入数据库
//void BookStore::thread4insert(string tableName, unordered_map<string, string> paraInfoSTR, unordered_map<string, long long> paraInfoINT)
//{
//	App::log("BookStore::thread4insert");
//	int num = 0;
//	bool result = false;
//	do 
//	{
//		App::ccsleep(App::getCurrentTime() % 300);
//		if (num >2)
//			break;
//		auto db = SqliteManager::OpenDataBase();
//		SqliteManager::DeleteData(db, tableName, StringUtils::format("where bookId = %d", paraInfoINT["bookId"]));
//		result = SqliteManager::InsertData(db, tableName, paraInfoSTR, paraInfoINT);
//		SqliteManager::CloseDataBase(db);
//		++num;
//	} while (result == false);
//	if (result)
//	{
//		CCLOG("[重复一次插入数据库操作成功]");
//	}
//	else
//	{
//		CCLOG("[重复一次插入数据库操作失败]");
//	}
//	App::log("BookStore::thread4insert--END");
//}

////初始化viewpage子页/刷新子页
//ui::Layout* BookStore::pageInit(
//	Node* view													//viewpage的子页面(m_book1,m_book2,m_book3三个是复用的)
//	, int index													//内容的页码
//	, std::vector<Node*> booksNode								//子页上8本书的书籍节点
//	,bool addEvent											//是否添加点击事件
//	)
//{	
//	App::log("BookStore::pageInit");
//	auto layout = ui::Layout::create();
//	if (view == nullptr)
//		return layout;
//	view->setTag(999);
//	layout->addChild(view);
//	//价格控件获取
//	auto price0 = (Text*)view->getChildByName(FIND_TEXT_BY_NAME_BOOKSTORE_PRICE0);
//	auto price1 = (Text*)view->getChildByName(FIND_TEXT_BY_NAME_BOOKSTORE_PRICE1);
//	auto price2 = (Text*)view->getChildByName(FIND_TEXT_BY_NAME_BOOKSTORE_PRICE2);
//	auto price3 = (Text*)view->getChildByName(FIND_TEXT_BY_NAME_BOOKSTORE_PRICE3);
//	auto price4 = (Text*)view->getChildByName(FIND_TEXT_BY_NAME_BOOKSTORE_PRICE4);
//	auto price5 = (Text*)view->getChildByName(FIND_TEXT_BY_NAME_BOOKSTORE_PRICE5);
//	auto price6 = (Text*)view->getChildByName(FIND_TEXT_BY_NAME_BOOKSTORE_PRICE6);
//	auto price7 = (Text*)view->getChildByName(FIND_TEXT_BY_NAME_BOOKSTORE_PRICE7);
//	vector<Text*> v_price;
//	v_price.push_back(price0);
//	v_price.push_back(price1);
//	v_price.push_back(price2);
//	v_price.push_back(price3);
//	v_price.push_back(price4);
//	v_price.push_back(price5);
//	v_price.push_back(price6);
//	v_price.push_back(price7);
//	//计算当前页上的书籍sort
//	auto sorts = getSortsOfThisPage(index);
//	//把不需要的书籍控件隐藏
//	for (int i = 0; i < booksNode.size(); i++)
//	{
//		if (i < sorts.size())
//		{
//			//需要显示的书籍设置价格,封面
//			booksNode[i]->setVisible(true);
//			v_price[i]->setVisible(true);
//			//获取当前sort对应的书籍信息
//			auto cover = (ImageView*)booksNode[i]->getChildByName(FIND_IMAGEVIEW_BY_NAME_BOOKCOVER);
//			//书店的排序号码
//			auto dataSort = sorts[i];
//			string sortKey = StringUtils::format("sort=%d+bookStoreID=%d", dataSort, m_bookStoreId);
//			int bookId = YYXStruct::getMapInt64(App::GetInstance()->myData, sortKey, -999);
//			if (bookId == -999)
//				continue;
//			string BookPriceKey = StringUtils::format("bookPrice+bookID=%d", bookId);
//			int price = YYXStruct::getMapInt64(App::GetInstance()->myData, BookPriceKey, -999);
//			if (price == -999)
//				continue;
//			string path = FileUtils::getInstance()->getWritablePath() + StringUtils::format("bookCover/%d.png", bookId);
//			auto isNewEllaBook =(int) YYXStruct::getMapRef(App::GetInstance()->myData, BookPriceKey, (Ref*)-999);
//			string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", bookId);
//			int hashBuy = YYXStruct::getMapInt64(App::GetInstance()->myData, BookPlayUrlKey, -999);
//			cover->setTag(bookId);
//			//设置价格
//			std::string strprice = App::getString("UNKNOEPRICE");
//			if (price > 0 && price < 100000)
//			{				
//				strprice = StringUtils::format("%s %.02f",App::getChars("STR_MONEY"),price / 100.0);
//			}
//			if (price == 0)
//			{
//				strprice = App::getString("STR_FREE");
//			}
//			v_price[i]->setText(strprice);
//			if (hashBuy == 0)
//				v_price[i]->setTextColor(Color4B(25, 25, 125, 255));
//			else
//				v_price[i]->setTextColor(Color4B(90, 53, 46, 255));
//			//设置封面
//			if (FileUtils::getInstance()->isFileExist(path))
//				cover->loadTexture(path);
//			else
//			{
//				App::log(path +" this picture is NULL");
//				cover->loadTexture(PICTURE_BOOKSTORE_BOOKCOVER, TextureResType::PLIST);
//			}
//			if (addEvent)
//			{
//				//给书籍添加点击事件
//				cover->setTouchEnabled(true);
//				cover->addClickEventListener([=](Ref* sender) {
//					//auto t = m_createTime + 1;
//					//if (App::getCurrentTime() <= t)
//					//{
//						//Toast::create("正在加载,稍后点击",false);
//						//return;
//					//}
//					App::log("m_click begin [点击书籍]bookId", cover->getTag());
//					if (m_click ) {
//						Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
//						this->addChild(Index::WaitLayer(), 10, "waitLayer");
//						closeTime();
//						App::log("m_click end [点击书籍]bookId", cover->getTag());
//						App::GetInstance()->pushScene(m_backScene, m_bookStoreId, StringUtils::format("%d", m_selectPageIndex[1]));
//						//跳转到书籍详情
//						auto id = cover->getTag();
//						if (id > 0)
//						{
//							Index::GoToBookInfo(id);
//							leave();
//						}
//					}
//				});	
//				cover->setSwallowTouches(false);
//			}
//		}
//		else
//		{
//			booksNode[i]->setVisible(false);
//			v_price[i]->setVisible(false);
//		}
//	}	
//	App::log("BookStore::pageInit--END");
//	return layout;
//}

//根据页码 返回该页码page上的所有书籍的sort 书籍小于等于8本
vector<int> BookStore::getSortsOfThisPage(int index)
{
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = index * 8;
	end = index * 8 + 7;
	string totalCountKey = StringUtils::format("totalCount+bookStoreID=%d", m_bookStoreId);
	auto count = YYXStruct::getMapInt64(App::GetInstance()->myData, totalCountKey, 0);
	if (count <= 0)
	{
		auto lay = YYXLayer::LoadingLayer(5, []() {});
		if (lay)
		{
			addChild(lay, 9);
		}
	}
	//auto count = m_storeBookCount;
	if (end >= count)
		end = count - 1;
	vector<int> v_boosSort;
	for (int i = begin; i <= end; i++)
	{
		v_boosSort.push_back(i);
	}
	return v_boosSort;
}

////通知 
//void BookStore::notify(NotifyType type, int index)
//{
//	//App::log("BookStore::notify  ", type);
//	if (Director::getInstance()->getEventDispatcher() && !Director::getInstance()->getEventDispatcher()->isEnabled())
//		Director::getInstance()->getEventDispatcher()->setEnabled(true);
//	switch (type)
//	{
//	case REFRESHUI:
//		Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
//			EventCustom event(NOTIFY_REFRESHUI);
//			Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
//		});
//		break;
//	case HTTPCOVER:
//		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
//			EventCustom event(NOTIFY_HTTPCOVER);
//			event.setUserData((void*)index);
//			Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
//		});
//		break;
//	case HTTPBOOKINFO:
//		Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
//			EventCustom event(NOTIFY_HTTPBOOKINFO);
//			Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
//		});
//		break;
//	case REMOVEWAITLayer:
//		Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
//			EventCustom event("REMOVEWAIT");
//			Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
//		});
//		break; 
//	case NOTIFY_HOME:
//		Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
//			EventCustom event("HOMEBUTTOMSETTRUE");
//			Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
//		});
//		break;
//	}
//	//App::log("BookStore::notify---END");
//}

//返回上级
void BookStore::back()
{
	App::log("BookStore::back");
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	YYXLayer::PLAYBUTTON();	
	App::GetInstance()->popBack();
	//leave();
	//清除全部的回调函数,只有在这个地方清除影响较小
	NetIntface::m_functionMap.clear();
	Index::BackPreviousScene();
	App::log("BookStore::back----END");
}

//离开场景时处理的业务
void BookStore::cleanup()
{
	App::log("BookStore::cleanup");
	while (!runQueue.empty())
	{
		runQueue.pop();
	}
	//关闭定时器
	unschedule("bookStoreSceneReferUISchedule");
	//Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	//m_queue->stop();
	//if (m_queue)
	//{
	//	delete m_queue;
	//	m_queue = nullptr;
	//}
	//closeTime();
	CC_SAFE_DELETE_ARRAY(m_selectPageIndex);
	//Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(NOTIFY_REFRESHUI);
	//Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(NOTIFY_HTTPCOVER);
	//Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(NOTIFY_HTTPBOOKINFO);
	/*if (m_book1->getReferenceCount() > 1)
		m_book1->release();
	if (m_book2->getReferenceCount() > 1)
		m_book2->release();
	if (m_book3->getReferenceCount() > 1)
		m_book3->release();*/
	//for (auto it : m_vbook1)
	//{
	//	it->release();
	//}
	//for (auto it : m_vbook2)
	//{
	//	it->release();
	//}
	//for (auto it : m_vbook3)
	//{
	//	it->release();
	//}
	//m_book1->release();
	//m_book2->release();
	//m_book3->release();
	App::log("BookStore::cleanup--END");
}

void BookStore::getCurrentlyPageBookListInfo(int bookStoreID, int pageIndex)//pageIndex从0开始
{
	int memberID = -999;
	if (App::GetInstance()->m_me)
		memberID = App::GetInstance()->m_me->id;
	//string runKey = StringUtils::format("httpBookStoreSceneCurrentlyPageBookListInfo%d", (int)YYXLayer::getRandom());
	//string errorKey = StringUtils::format("httpBookStoreSceneCurrentlyPageBookListInfo%d", (int)YYXLayer::getRandom());
	string runKey = "httpBookStoreSceneCurrentlyPageBookListInfoSuccess";
	string errorKey = "httpBookStoreSceneCurrentlyPageBookListInfoFail";
	NetIntface::httpBookStoreSceneCurrentlyPageBookListInfo(memberID, bookStoreID, pageIndex + 1, m_visitFrom, runKey, [=](string json) {
		//解析
		NetIntface::httpBookStoreSceneCurrentlyPageBookListInfoCallBack(json, [=](int totalCount, int index, int bookId, int isNewEllaBook, int hashBuy, int bookPrice,
			int bookmarketPrice, int remainTime, string bookName, string bookAuthor, string bookPress, string bookCoverUrl, string bookPlayUrl) {
			/*App::log("totalCount=", totalCount);
			App::log("index=", index);
			App::log("bookId=", bookId);
			App::log("isNewEllaBook=", isNewEllaBook);
			App::log("hashBuy=", hashBuy);
			App::log("bookPrice=", bookPrice);
			App::log("bookmarketPrice=", bookmarketPrice);
			App::log("remainTime=", remainTime);
			App::log("bookName="+ bookName);
			App::log("bookAuthor=" + bookAuthor);
			App::log("bookPress=" + bookPress);
			App::log("bookCoverUrl=" + bookCoverUrl);
			App::log("bookPlayUrl=" + bookPlayUrl);*/
			//处理json数组
			//书店书籍总数
			string totalCountKey = StringUtils::format("totalCount+bookStoreID=%d", bookStoreID);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, totalCountKey, totalCount);
			//书店的排序
			string sortKey = StringUtils::format("sort=%d+bookStoreID=%d", pageIndex * 8 + index, bookStoreID);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, sortKey, bookId);
			//书籍信息 书籍名称+原价
			string BookNameKey = StringUtils::format("bookName+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookNameKey, bookmarketPrice, bookName);
			//现价+封面url+新书标记
			string BookPriceKey = StringUtils::format("bookPrice+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPriceKey, bookPrice, bookCoverUrl, (Ref*)isNewEllaBook);
			//购书标记+下载URL
			string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", bookId);
			if (m_backScene == MySceneName::VIPBOOK)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, hashBuy, bookPlayUrl,(Ref*)1);
			else
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, hashBuy, bookPlayUrl);
			//if (BookPlayUrlKey == "bookPlayUrl+bookID=455")
			//{
			//	auto isvip = (int)YYXStruct::getMapRef(App::GetInstance()->myData, BookPlayUrlKey, nullptr);
			//	App::log(" usvip=", isvip);
			//}
			//书籍作者+活动倒计时
			string BookAuthorKey = StringUtils::format("bookAuthor+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookAuthorKey, remainTime, bookAuthor);
			//出版社
			string BookPressKey = StringUtils::format("bookPress+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPressKey, -999, bookPress);
			//下载封面
			string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
			string fileName = StringUtils::format("%d", bookId) + ".png";
			auto path = dir + "/" + fileName;
			if (!FileUtils::getInstance()->isFileExist(path))
			{
				//string DownLoadImageRunKey = StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom());
				//string DownLoadImageErrorKey = StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom());
				string DownLoadImageRunKey = "BookStoreSceneDownLoadImageSuccess";
				string DownLoadImageErrorKey = "BookStoreSceneDownLoadImageFail";
				NetIntface::DownLoadImage(bookCoverUrl, dir, fileName, DownLoadImageRunKey, [=](string downPath) {
					YYXLayer::sendNotify("bookstoreCoverDownloadSuccess");
				}, DownLoadImageErrorKey, [=](string str) {
					string sstr = string(bookName).append(App::getString("FENGMIANXIAZAISHIBAI"));
					Toast::create(sstr.c_str(), false);
				});
			}
		}, [=]() {
			//解析完毕
			YYXLayer::sendNotify("bookstoreCoverDownloadSuccess");
			YYXLayer::sendNotify("showPageNumber");
		}, []() {
			//解析错误
			Toast::create(App::getString("SHUJUYICHANG"));
			YYXLayer::deleteLoadingLayer(1);
		});
	}, errorKey, [](string str) {
		//网络连接错误
		Toast::create(App::getString("NETEXCEPTION"));
		YYXLayer::deleteLoadingLayer(1);
	});
}

//创建一个loading图层
//YYXLayer* BookStore::LoadingLayer()
//{
//	App::log("BookStore::LoadingLayer");
//	auto co = YYXStruct::getMapInt64(App::GetInstance()->myData, "LoadingLayerCount", 1);
//	if (co >0)
//	{
//		return nullptr;
//	}
//	map<string, int64String> paramter;
//	YYXLayer::insertMap4ParaType(paramter, "className", -999, "bookstoreLoading");
//	YYXLayer::insertMap4ParaType(paramter, "csb", -999, WAITMESSAGEBOX_CSB);
//	YYXLayer::insertMap4ParaType(paramter, "csbTimeLine", 28,"");
//	YYXLayer::insertMap4ParaType(paramter, "gotoFrameAndPlayIsLoop", 0, "");
//	auto loading = YYXLayer::create(paramter, map<string, function<void(YYXLayer*)>>()); 
//	auto listener = EventListenerCustom::create("loadingDelete", [=](EventCustom* e) {
//		loading->removeFromParentAndCleanup(true);
//		m_createTime = YYXLayer::getCurrentTime4Second() * 2;
//		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "LoadingLayerCount", 0);
//	});
//	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, loading);
//	loading->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
//	loading->setPosition(Director::getInstance()->getVisibleSize() / 2);
//	auto imageview = (ImageView*)loading->findControl(FIND_IMAGEVIEW_BY_NAME_WAITMESSAGEBOX);
//	imageview->setTouchEnabled(true);
//	imageview->setSwallowTouches(true);
//	imageview->addClickEventListener([=](Ref* sender) {
//		YYXLayer::controlTouchTime(1, "bookStoreSceneClickLoadingLayerTime", [=]() {
//			if (YYXLayer::getCurrentTime4Second() > m_createTime + 3)
//			{
//				loading->removeFromParentAndCleanup(true);
//				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "LoadingLayerCount", 0);
//			}
//		});
//	});
//	m_createTime = YYXLayer::getCurrentTime4Second();
//	string key = "bookStoreSceneDeleteLoadingLayerKeyName";
//	loading->addSchedule(key, 1, [=]() {
//		auto t = YYXLayer::getCurrentTime4Second();
//		if (t > m_createTime + 5)
//		{
//			loading->removeFromParentAndCleanup(true);
//			m_createTime = YYXLayer::getCurrentTime4Second() * 2;
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "LoadingLayerCount", 0);
//		}
//	});
//	App::log("BookStore::LoadingLayer--END");
//	return loading;
//}

void BookStore::refreshBook(Node* node, int bookId, string path, int price, int isNewEllaBook, int hashBuy)
{
	node->setVisible(true);
	//封面
	auto cover = (ImageView*)node->getChildByName(FIND_IMAGEVIEW_BY_NAME_BOOKSTORE_BOOK_BOOKCOVER);
	cover->setTouchEnabled(true);
	cover->setSwallowTouches(false);
	if (cover)
	{
		if (FileUtils::getInstance()->isFileExist(path))
			cover->loadTexture(path);
		else
			cover->loadTexture("other/Book_cover.png");
		cover->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "bookStoreSceneClickBookTime", [=]() {
				if(m_click)
					App::log("点击书籍", bookId);
				if (bookId != -999 && m_click)
					YYXLayer::sendNotify("bookStoreSceneClickBook", "",bookId);
			});
		});
	}
	//价格
	auto bookPrice = (Text*)node->getChildByName(FIND_TEXT_BY_NAME_BOOKSTORE_BOOK_BOOKPRICE);
	std::string strprice = App::getString("UNKNOEPRICE");
	if (price > 0 && price < 100000)
	{
		strprice = StringUtils::format("%s %.02f", App::getString("STR_MONEY"), price / 100.0);
	}
	if (price == 0)
	{
		strprice = App::getString("STR_FREE");
	}
	if (bookPrice)
	{
		bookPrice->setText(strprice);
		if (hashBuy == 0)
			bookPrice->setTextColor(Color4B::BLACK);
		else
			bookPrice->setTextColor(Color4B(90, 53, 46, 255));
	}
	//新书标记
	auto newbook = (Sprite*)node->getChildByName(FIND_SPRITE_BY_NAME_BOOKSTORE_BOOK_NEWBOOK);
	if (newbook)
	{
		if (isNewEllaBook == 1)
			newbook->setVisible(true);
		else
			newbook->setVisible(false);
	}
}

void BookStore::refershThreePages(Node* node)
{
	App::log("BookStore::refershThreePages");
	auto firstPage = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_FIRSTPAGE);
	auto secondPage = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_SECONDPAGE);
	auto threePage = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_THREEPAGE);
	changPageNumber();	
	if(firstPage)
		refershPage(firstPage, m_selectPageIndex[0]);
	if(secondPage)
		refershPage(secondPage, m_selectPageIndex[1]);
	if(threePage)
		refershPage(threePage, m_selectPageIndex[2]);	
	App::log("BookStore::refershThreePages-- END");
}

void BookStore::refershPage(Node* node, int index)
{
	auto book0 = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOK0);
	auto book1 = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOK1);
	auto book2 = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOK2);
	auto book3 = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOK3);
	auto book4 = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOK4);
	auto book5 = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOK5);
	auto book6 = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOK6);
	auto book7 = node->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOK7);
	auto lun1 = node->getChildByName(FIND_SPRITE_BY_NAME_BOOKSTORE_FRONTWHEEL); 
	auto lun3 = node->getChildByName(FIND_SPRITE_BY_NAME_BOOKSTORE_BACKWHEEL);
	auto lun2 = node->getChildByName(FIND_SPRITE_BY_NAME_BOOKSTORE_MINWHEEL);
	vector<Node*> booksvector;
	booksvector.push_back(book0);
	booksvector.push_back(book1);
	booksvector.push_back(book2);
	booksvector.push_back(book3);
	booksvector.push_back(book4);
	booksvector.push_back(book5);
	booksvector.push_back(book6);
	booksvector.push_back(book7);
	auto sorts = getSortsOfThisPage(index);
	for (int i = 0; i < 8; i++)
	{
		if (i < sorts.size()) {
			string sortKey = StringUtils::format("sort=%d+bookStoreID=%d", sorts[i] , m_bookStoreId);
			auto bookId = YYXStruct::getMapInt64(App::GetInstance()->myData, sortKey, -999);
			string BookPriceKey = StringUtils::format("bookPrice+bookID=%d", bookId);
			int bookPrice = YYXStruct::getMapInt64(App::GetInstance()->myData, BookPriceKey, -999);
			if (index == m_selectPageIndex[1] ) 
			{
				if (bookPrice >= 0) {
					YYXLayer::controlTouchTime(2, "bookStoreSceneSendLoadingDeleteTime", []() {
						YYXLayer::deleteLoadingLayer(1);
					});
				}
				else
				{
					YYXLayer::controlTouchTime(2, "bookStoreSceneSendLoadingAddTime", [=]() {
						auto lay = YYXLayer::LoadingLayer(5, []() {});
						if (lay)
						{
							addChild(lay, 9);
						}
					});
				}
			}
			string bookCoverUrl = YYXStruct::getMapString(App::GetInstance()->myData, BookPriceKey, "");
			int isNewEllaBook = (int)YYXStruct::getMapRef(App::GetInstance()->myData, BookPriceKey, (Ref*)-999);
			string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", bookId);
			int hashBuy = YYXStruct::getMapInt64(App::GetInstance()->myData, BookPlayUrlKey, -999);
			string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
			std::string fileName = StringUtils::format("%d", bookId) + ".png";
			auto path = dir + "/" + fileName;
			refreshBook(booksvector[i], bookId, path, bookPrice, isNewEllaBook, hashBuy);			
		}
		else
		{
			booksvector[i]->setVisible(false);
		}
	}
	string scrollKey2 = "lunzigundongLeft";
	string scrollKey1 = "lunzigundongRight";
	int rotate = 360*2;
	auto lun1listener = EventListenerCustom::create(scrollKey1, [=](EventCustom* e) {
		lun1->runAction(RotateBy::create(1, -1* rotate));
		lun2->runAction(RotateBy::create(1, -1* rotate));
		lun3->runAction(RotateBy::create(1, -1* rotate));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lun1listener->clone(), lun1);
	auto lun2listener = EventListenerCustom::create(scrollKey2, [=](EventCustom* e) {
		lun1->runAction(RotateBy::create(1, rotate));
		lun2->runAction(RotateBy::create(1, rotate));
		lun3->runAction(RotateBy::create(1, rotate));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lun2listener->clone(), lun1);
}

void BookStore::viewMoveAnimation(Node* node, bool isLeft)
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	//播放声音
	//分左右移动
	//一秒移动完成后刷新
	//移动中暂停监听
	if (!node)
		return;
	Vec2 posmove;
	if (isLeft) {
		posmove = Vec2(1094, 0);
		m_selectPageIndex[1] -= 1;
	}
	else {		
		posmove = Vec2(-1094, 0);
		m_selectPageIndex[1] += 1;
	}
	bool gundong = changPageNumber();
	if (!gundong)
		return;
	m_move = true;
	if (App::GetInstance()->isSoundEffect)
		AudioEngine::play2d(ELLA_SOUND_CASTLENEXTPAGE);
	Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(Director::getInstance()->getRunningScene());
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BackButtonTouch", 1);
	getCurrentlyPageBookListInfo(m_bookStoreId, m_selectPageIndex[1]);
	m_createTime = YYXLayer::getCurrentTime4Second();
	node->setPosition(Vec2::ZERO);
	if (isLeft)
		YYXLayer::sendNotify("lunzigundongLeft");
	else
		YYXLayer::sendNotify("lunzigundongRight");
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreSceneProtectAnimationTime", YYXLayer::getCurrentTime4Second());
	showBookPageNumber();
	node->runAction(Sequence::create(MoveTo::create(1, posmove), DelayTime::create(0.1) , CallFuncN::create([=](Ref* sender) {
		//addorStartQueue([=]() {
			App::log("移动完成调用函数");				
			node->setPosition(Vec2::ZERO);
			refershThreePages(node);
			Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(Director::getInstance()->getRunningScene());
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BackButtonTouch", 0);
			m_move = false;
		//});
	}),NULL));
	addQueue([=]() {
		protectAnimation();
	});
}

void BookStore::protectAnimation()
{	//保护动画时间
	int runable = YYXStruct::getMapInt64(App::GetInstance()->myData, "BackButtonTouch", -999);
	if (runable == 0) {
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreSceneKeepFunction", 0);
		return;
	}
	else
	{
		auto it = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreSceneProtectAnimationTime", YYXLayer::getCurrentTime4Second());
		auto now = YYXLayer::getCurrentTime4Second() - it;
		if (now >= 3) {
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreSceneKeepFunction", 0);
			return;
		}
		else
		{
			//函数在队列中不被删除
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreSceneKeepFunction", 1);
			stopQueue();
		}
	}
}

void BookStore::addorStartQueue(function<void()> runable, string str)
{
	App::log(" addorStartQueue ");
	addQueue(runable);
	startQueue();
	App::log(str + " / addorStartQueue--END runQueue.size() = ", runQueue.size());
}

void BookStore::addQueue(function<void()> runable)
{
	App::log(" BookStore::addQueue ");
	if(runable)
		runQueue.push(runable);
	App::log(" BookStore::addQueue-- END ");
}

void BookStore::startQueue()
{
	App::log(" BookStore::startQueue ");
	if (!queueIsRuning)
	{
		queueIsRuning = true;
		runingQueue();
	}
	App::log(" BookStore::startQueue---END ");
}

void BookStore::stopQueue()
{
	App::log(" BookStore::stopQueue ");
	queueIsRuning = false;
	App::log(" BookStore::stopQueue---END ");
}

void BookStore::runingQueue()
{
	App::log(" BookStore::runingQueue ");
	if (!queueIsRuning) {
		App::log(" BookStore::runingQueue1---END ");
		return;
	}
	if (runQueue.size() > 0)
	{
		auto func = runQueue.front();
		if (func)
		{
			func();
			int keep = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreSceneKeepFunction", 0);
			if (keep != 1)
			{
				App::log("pop");
				if(!runQueue.empty())
					runQueue.pop();
			}
		}
		runingQueue();
	}
	else
	{
		queueIsRuning = false;
		App::log(" BookStore::runingQueue2---END ");
		return;
	}
}

void BookStore::baonianStore()
{
	//提示非年卡用户 升级年卡
	if (App::GetInstance()->m_me && App::GetInstance()->m_me->vip)
		return ;
	int show = YYXStruct::getMapInt64(App::GetInstance()->myData, "showYouAreVip", 0);
	if (show == 1)
	{
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "showYouAreVip", 0);
		auto layer = XZLayer::OpenVIPCardService(1, [=]() {
			App::GetInstance()->pushScene(m_backScene, m_bookStoreId, StringUtils::format("%d", m_selectPageIndex[1]));
		});
		if (layer)
			addChild(layer, 9);
	}
}
