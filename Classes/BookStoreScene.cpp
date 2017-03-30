#include "BookStoreScene.h"
#include "NetIntface.h"
#include "YYXVisitor.h"
#include "YYXDownloadImages.h"
#include "YYXSound.h"
#include "BookCache.h"
//#include "ControlScene.h"

using namespace cocostudio::timeline;

#define zz(d) App::log(string("BookStore::")+#d);
#define zzz(d) App::log(string("BookStore::")+#d+"----END");

int BookStore::isNetConnect = 0;//是否联网

BookStore::~BookStore()
{
	App::log("BookStore::~BookStore()");
	ControlScene::getInstance()->end();
	App::log("BookStore::~BookStore()---END");
}

Scene* BookStore::createScene(SceneInfo* sceneInfo)
{	
	App::log("BookStore::createScene");
    auto scene = Scene::create();
    
    auto layer = BookStore::create(sceneInfo);

    scene->addChild(layer);

	App::log("BookStore::createScene--END");
    return scene;
}

BookStore* BookStore::create(SceneInfo* sceneInfo) {
	zz(create)
	BookStore *pRet = new(std::nothrow) BookStore();
		if (pRet && pRet->init(sceneInfo))
		{
			pRet->autorelease();
			zzz(create)
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = nullptr;
			zzz(create == nullptr)
			return nullptr;
		} 
}

// on "init" you need to initialize your instance
bool BookStore::init(SceneInfo* sceneInfo)
{
	zz(init)
    if ( !Layer::init() )
    {
		zzz(init = false)
        return false;
    }	 
	sp2 = Sprite::create();
	addChild(sp2);
	//等待层
	YYXVisitor::getInstance()->bookStoreSceneInit();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();	
	m_click = true;
	m_selectPageIndex = new int[3];
	//--------------------------场景初始化-----------------------------------------
	m_bookStoreId = sceneInfo->getData("bookStoreId", Value(0)).asInt();
	int index = sceneInfo->getData("index", Value(0)).asInt();
	if (index == -999 || index < 0 || index >1000)
		index = 0;
	m_selectPageIndex[0] = -1;
	m_selectPageIndex[1] = index;
	m_selectPageIndex[2] = -1;
	/*int index = 0;
	index = atoi(App::GetInstance()->m_showSceneData.stringData.c_str());
	if (index == -999  || index<0  || index >1000)
		index = 0;
	m_selectPageIndex[1] = index;
	m_selectPageIndex[2] = -1;
	App::log("*******", m_selectPageIndex[1]);*/
	//得到书店ID,从数据库中读取书店信息,修改书店名称
	//m_bookStoreId = BookStoreId;
	
	//---------------------------背景层-------------------------------	
	Data data;
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
	if (pageCircle == nullptr)
	{
		pageCircle = Node::create();
		pageCircle->setPosition(Vec2(0, 0));
		addChild(pageCircle, 1);
	}
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
			back();
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
				auto control = ControlScene::getInstance();
				control->replaceScene(control->getCurrentScene()->setData("bookStoreId", Value(m_bookStoreId))->setData("index", Value(m_selectPageIndex[1])), ControlScene::getInstance()->getSceneInfo(BookRoomScene));
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
		//m_backScene = MySceneName::PictureBook;
		break;
	case P_TRAIN_3:
		BookStoreName = App::getString("STR_RECOMMEND");
		//m_backScene = MySceneName::Recommend;
		break;
	case P_TRAIN_4:
		BookStoreName = App::getString("STR_LIMITFREE");
		//m_backScene = MySceneName::Free;
		break;
	case P_TRAIN_5:
		BookStoreName = App::getString("STR_EXCELLENT_FIVE_STAR");
		//m_backScene = MySceneName::GoodReputation;
		break;
	case P_TRAIN_6:
		BookStoreName = App::getString("STR_NEWBOOK");
		//m_backScene = MySceneName::NewBook;
		break;
	case P_TRAIN_7:
		BookStoreName = App::getString("STR_KANGXUANBOOKSTORE");
		//m_backScene = MySceneName::KangXuanStore;
		break;
	case BOOKSTOREID_TRAIN_8:
		BookStoreName = App::getString("VIPZHUANSHUSHUDIAN");
		//m_backScene = MySceneName::VIPBOOK;
		break;
	default:
		//书城的书店名称是通过网络接口实现,当前只需要去数据库查询出来
		m_visitFrom = "1";
	};
	m_bookstorename->setString(BookStoreName);

	//查询书店信息
	if (m_visitFrom == "1")
	{
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
	getCurrentlyPageBookListInfo(m_bookStoreId, m_selectPageIndex[1]);
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
			//addorStartQueue([=]() {
				refershThreePages(moveViews);
			//}, "refershThreePages(moveViews);");
			auto temp = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreNeedRefresh", 0) - needRefersh;
			if (temp >= 0)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreNeedRefresh", temp);
			else
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreNeedRefresh", 0);
		}
	}, 1, "bookStoreSceneReferUISchedule");

	
	
	initEvent();
	zzz(init)
    return true;
}

void BookStore::initEvent()
{
	listerName.push_back("showPageNumber");
	auto listerner1 = EventListenerCustom::create("showPageNumber", [=](EventCustom* e) {
		zz(EventListenerCustom::showPageNumber)
			showBookPageNumber();
		zzz(EventListenerCustom::showPageNumber)
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listerner1, pageCircle);

	//-----------------------------控制点击间隔时间 以及轮子转动和循环滚动----------------------------------------------------------
	touchmove = EventListenerTouchOneByOne::create();
	//Began
	touchmove->onTouchBegan = [=](Touch* t, Event* e) {
		zz(EventListenerTouchOneByOne  onTouchBegan  touchmove)
			m_click = true;
		zzz(EventListenerTouchOneByOne  onTouchBegan  touchmove)
			return true;
	};
	//Moved
	touchmove->onTouchMoved = [=](Touch* t, Event* e) {
		zz(EventListenerTouchOneByOne  onTouchMoved  touchmove)
			auto moveLocation = t->getStartLocation() - t->getLocation();
		if (moveLocation.x > 30 || moveLocation.x < -30 || moveLocation.y > 30 || moveLocation.y < -30)
			m_click = false;
		zzz(EventListenerTouchOneByOne  onTouchMoved  touchmove)
	};
	//Ended
	touchmove->onTouchEnded = [=](Touch* t, Event* e) {
		zz(EventListenerTouchOneByOne  onTouchEnded  touchmove)
			zzz(EventListenerTouchOneByOne  onTouchEnded  touchmove)
	};
	//add
	auto sp1 = Sprite::create();
	addChild(sp1);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchmove, sp1);

	moveViews = background->getChildByName(FIND_NODE_BY_NAME_BOOKSTORE_BOOKPAGE);
	//Began
	touchlistener = EventListenerTouchOneByOne::create();
	touchlistener->onTouchBegan = [=](Touch* t, Event* e) {
		zz(EventListenerTouchOneByOne  onTouchBegan  touchlistener)
			zzz(EventListenerTouchOneByOne  onTouchBegan  touchlistener)
			return true;
	};
	//Moved
	touchlistener->onTouchMoved = [=](Touch* t, Event* e) {
		zz(EventListenerTouchOneByOne  onTouchMoved  touchlistener)
			auto moveLocation = t->getStartLocation() - t->getLocation();
		auto key = "bookstoreMoveTime";
		if (moveLocation.x > 100)
		{				//向右
			if (!m_move)
				YYXLayer::controlTouchTime(2, key, [=]() {
				//addorStartQueue([=]() {
				viewMoveAnimation(moveViews, false);
				//}, "viewMoveAnimation(moveViews, false);");
			});
		}
		else if (moveLocation.x < -100)
		{				//向左
			if (!m_move)
				YYXLayer::controlTouchTime(2, key, [=]() {
				//addorStartQueue([=]() {
				viewMoveAnimation(moveViews, true);
				//}, "viewMoveAnimation(moveViews, true);");
			});
		}
		else
			m_move = false;
		zzz(EventListenerTouchOneByOne  onTouchMoved  touchlistener)
	};
	//Ended
	touchlistener->onTouchEnded = [=](Touch* t, Event* e) {
		zz(EventListenerTouchOneByOne  onTouchMoved  touchlistener)
			zzz(EventListenerTouchOneByOne  onTouchMoved  touchlistener)
	};
	//add
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchlistener, sp2);

	auto listener2 = EventListenerCustom::create(TAG_RECOVERBOOKSTORE, [=](EventCustom *e) {
		recoverAnimation();
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, moveViews);
}

void BookStore::onEnterTransitionDidFinish()
{
	App::log("BookStore::onEnterTransitionDidFinish");
	//App::sqliteClose();	
	Layer::onEnterTransitionDidFinish();	
	//App::log("当前场景:", App::GetInstance()->m_RunningScene);
	Toast::GetInstance()->SceneInitToast();
	if (m_bookStoreId == BOOKSTOREID_TRAIN_8)
	{
		baonianStore();
	}
	App::log("BookStore::onEnterTransitionDidFinish--END");
}

//翻页时,计算内容页码
bool BookStore::changPageNumber()
{
	zz(changPageNumber)
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
	zzz(changPageNumber)
	return true;
}

//展示第1/5页
void BookStore::showBookPageNumber()
{
	zz(showBookPageNumber)
	string totalCountKey = StringUtils::format("totalCount+bookStoreID=%d", m_bookStoreId);
	auto totalCount = YYXStruct::getMapInt64(App::GetInstance()->myData, totalCountKey, 0);
	int count = ceil(totalCount / 8.0);
	//创建页数圆圈
	if (pageCircle)
	{
		if(pageCircle->getChildrenCount()>0)
			pageCircle->removeAllChildrenWithCleanup(true);
	}
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
	zzz(showBookPageNumber)
}

//根据页码 返回该页码page上的所有书籍的sort 书籍小于等于8本
vector<int> BookStore::getSortsOfThisPage(int index)
{
	zz(getSortsOfThisPage)
		App::log("index = ", index);
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
	zzz(getSortsOfThisPage)
	return v_boosSort;
}

//返回上级
void BookStore::back()
{
	zz(back)
	//Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	//YYXLayer::PLAYBUTTON();	
	//App::GetInstance()->popBack();
	//Index::BackPreviousScene();
	auto control = ControlScene::getInstance();
	control->backFromScene();
	zzz(back)
}

//离开场景时处理的业务
void BookStore::cleanup()
{
	zz(cleanup)
	//while (!runQueue.empty())
	//{
	//	runQueue.pop();
	//}
	//关闭定时器
	unschedule("bookStoreSceneReferUISchedule");
	CC_SAFE_DELETE_ARRAY(m_selectPageIndex);
	//for (auto it : listerName) {
	//	_eventDispatcher->removeCustomEventListeners(it);
	//}
	//_eventDispatcher->removeEventListener(touchmove);
	//_eventDispatcher->removeEventListener(touchlistener);
	zzz(cleanup)
}

void BookStore::getCurrentlyPageBookListInfo(int bookStoreID, int pageIndex)//pageIndex从0开始
{
	zz(getCurrentlyPageBookListInfo)
	auto memberID = App::GetInstance()->getMemberId();
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
			//if (m_backScene == MySceneName::VIPBOOK)
				//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, hashBuy, bookPlayUrl,(Ref*)1);
			//else
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, hashBuy, bookPlayUrl);
			//书籍作者+活动倒计时
			string BookAuthorKey = StringUtils::format("bookAuthor+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookAuthorKey, remainTime, bookAuthor);
			//出版社
			string BookPressKey = StringUtils::format("bookPress+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPressKey, -999, bookPress);
			//下载封面
			BookCache::getInstance()->addBook(Book::create()->setBookId(bookId)->setCoverURL(bookCoverUrl));
			string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
			string fileName = StringUtils::format("%d", bookId) + ".png";
			auto path = dir + "/" + fileName;
			if (!FileUtils::getInstance()->isFileExist(path))
			{
				YYXDownloadImages::GetInstance()->newDownloadImage(bookCoverUrl, dir, fileName, high, 0, [=](string downPath) {
					safe_downCoverOk();
				},  [=](string str) {
					string sstr = string(bookName).append(App::getString("FENGMIANXIAZAISHIBAI"));
					Toast::create(sstr.c_str(), false);
				});
			}
		}, [=]() {
			//解析完毕
			safe_downCoverOk();
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
	zzz(getCurrentlyPageBookListInfo)
}

void BookStore::refreshBook(Node* node, int bookId, string path, int price, int isNewEllaBook, int hashBuy)
{
	zz(refreshBook)		
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
				if (bookId > 0 && m_click)
				{
					gotoBookInfo(bookId);
				}
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
	zzz(refreshBook)
}

void BookStore::refershThreePages(Node* node)
{
	zz(refershThreePages)
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
	zzz(refershThreePages)
}

void BookStore::refershPage(Node* node, int index)
{
	zz(refershPage)
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
			long isNewEllaBook = (long)YYXStruct::getMapRef(App::GetInstance()->myData, BookPriceKey, (Ref*)-999);
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
	zzz(refershPage)
}

void BookStore::viewMoveAnimation(Node* node, bool isLeft)
{
	zz(viewMoveAnimation)
	auto visibleSize = Director::getInstance()->getVisibleSize();
	//播放声音
	//分左右移动
	//一秒移动完成后刷新
	//移动中暂停监听
	if (!node)
	{
		zzz(viewMoveAnimation node == nullptr)
		return;
	}
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
	YYXSound::getInstance()->playStore_ScrollSound();
	//Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(Director::getInstance()->getRunningScene());
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
			//App::log("移动完成调用函数");				
			//node->setPosition(Vec2::ZERO);
			//refershThreePages(node);
			////Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(Director::getInstance()->getRunningScene());
			//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BackButtonTouch", 0);
			//m_move = false;
		YYXLayer::sendNotify(TAG_RECOVERBOOKSTORE);
	}),NULL));
	//addQueue([=]() {
		//protectAnimation();
	//});
	zzz(viewMoveAnimation)
}

void BookStore::gotoBookInfo(int bookid)
{
	zz(gotoBookInfo)
	if (bookid > 0)
	{
		App::log("goto bookinfo ==>>bookid = ", bookid);
		auto control = ControlScene::getInstance();
		control->replaceScene(control->getCurrentScene()->setData("bookStoreId",
			Value(m_bookStoreId))->setData("index", Value(m_selectPageIndex[1])), ControlScene::getInstance()->getSceneInfo(BookInfoScene)->setData("bookId", Value(bookid)));
	}
	zzz(gotoBookInfo)
}

void BookStore::safe_downCoverOk()
{
	zz(downCoverOk)
		auto temp = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreNeedRefresh", -999);
	if (temp > 0)
		temp += 1;
	else
		temp = 1;
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreNeedRefresh", temp);
	zzz(downCoverOk)
}

//void BookStore::protectAnimation()
//{	//保护动画时间
//	zz(protectAnimation)
//	int runable = YYXStruct::getMapInt64(App::GetInstance()->myData, "BackButtonTouch", -999);
//	if (runable == 0) {
//		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreSceneKeepFunction", 0);
//		zzz(protectAnimation   runable == 0)
//		return;
//	}
//	else
//	{
//		auto it = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreSceneProtectAnimationTime", YYXLayer::getCurrentTime4Second());
//		auto now = YYXLayer::getCurrentTime4Second() - it;
//		if (now >= 3) {
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreSceneKeepFunction", 0);
//			zzz(protectAnimation   now >= 3)
//			return;
//		}
//		else
//		{
//			//函数在队列中不被删除
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BookStoreSceneKeepFunction", 1);
//			stopQueue();
//		}
//	}
//	zzz(protectAnimation)
//}
//
//void BookStore::addorStartQueue(function<void()> runable, string str)
//{
//	zz(addorStartQueue)
//	addQueue(runable);
//	startQueue();
//	zzz(addorStartQueue)
//	App::log("runQueue.size() = ", runQueue.size());
//}
//
//void BookStore::addQueue(function<void()> runable)
//{
//	App::log(" BookStore::addQueue ");
//	if(runable)
//		runQueue.push(runable);
//	App::log(" BookStore::addQueue-- END ");
//}
//
//void BookStore::startQueue()
//{
//	App::log(" BookStore::startQueue ");
//	if (!queueIsRuning)
//	{
//		queueIsRuning = true;
//		runingQueue();
//	}
//	App::log(" BookStore::startQueue---END ");
//}
//
//void BookStore::stopQueue()
//{
//	App::log(" BookStore::stopQueue ");
//	queueIsRuning = false;
//	App::log(" BookStore::stopQueue---END ");
//}
//
//void BookStore::runingQueue()
//{
//	zz(runingQueue)
//	if (!queueIsRuning) {
//		zzz(runingQueue null)
//		return;
//	}
//	if (runQueue.size() > 0)
//	{
//		auto func = runQueue.front();
//		if (func)
//		{
//			func();
//			int keep = YYXStruct::getMapInt64(App::GetInstance()->myData, "BookStoreSceneKeepFunction", 0);
//			if (keep != 1)
//			{
//				if(!runQueue.empty())
//					runQueue.pop();
//			}
//		}
//		runingQueue();
//	}
//	else
//	{
//		queueIsRuning = false;
//		zzz(runingQueue over)
//		return;
//	}
//}
//移动后恢复原样
void BookStore::recoverAnimation()
{
	moveViews->setPosition(Vec2::ZERO);
	refershThreePages(moveViews);
	//Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(Director::getInstance()->getRunningScene());
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BackButtonTouch", 0);
	m_move = false;
}

void BookStore::baonianStore()
{
	zz(baonianStore)
	//提示非年卡用户 升级年卡
	if (App::GetInstance()->m_me && App::GetInstance()->m_me->vip)
		return ;
	int show = YYXStruct::getMapInt64(App::GetInstance()->myData, "showYouAreVip", 0);
	if (show == 1)
	{
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "showYouAreVip", 0);
		auto layer = XZLayer::OpenVIPCardService(1, [=]() {
			//App::GetInstance()->pushScene(m_backScene, m_bookStoreId, StringUtils::format("%d", m_selectPageIndex[1]));
			auto control = ControlScene::getInstance();
			control->replaceScene(control->getCurrentScene()->
				setData("bookStoreId",Value(m_bookStoreId))->setData("index",Value(m_selectPageIndex[1])), 
				ControlScene::getInstance()->getSceneInfo(LoginScene));
		});
		if (layer)
			addChild(layer, 9);
	}
	zzz(baonianStore)
}
