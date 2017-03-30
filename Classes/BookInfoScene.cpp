#include "BookInfoScene.h"
#include "YYXLayer.h"
#include "SendCommentLayer.h"
#include "IndexScene.h"
#include "FileNames.h"
#include "NetIntface.h"
#include <math.h>
#include "YYXDownload.h"
#include <stdio.h>
#include "AudioEngine.h"
#include "YYXVisitor.h"
#include "Charger.h"
#include "YYXRentBook.h"
#include "YYXBuyBook.h"
#include "YYXDownloadImages.h"
#include "DownloadBook.h"
#include "BookCommentList.h"
#include "SetBook.h"
#include "YYXSound.h"

using namespace experimental;
USING_NS_CC;
using namespace cocostudio::timeline;
USING_NS_CC_EXT;
using namespace std;

#define zz(d) App::log(string("BookInfo::")+#d);
#define zzz(d) App::log(string("BookInfo::")+#d+"----END");

//返回 免费=0;
#define FREEPRICE  0
//购买=1;
#define BUYBOOK  1
//下载=2;
#define DOWNLOAD  2
//vip下载=3;
#define VIPDOWNLOAD  3
//阅读=4;
#define READBOOK  4
//进度=5;
#define PROGRESSING 5
//暂停
#define PAUSE 6

BookInfo::BookInfo() {
	App::log("BookInfo::BookInfo");
	isDownloading = false;
	isDownloaded = false;
	isUnzip = false;
	//isBuy = false;
	isDamage = false;
	isViewDownloading = false;
	isViewDownloaded = false;
	isViewUnzip = false;
	isViewDamage = false;
	m_orderId = -999;
	m_Selling100 = 2.99;
	m_relation = new BookInfoControl();
	App::log("BookInfo::BookInfo---END");
}

BookInfo::~BookInfo()
{
	App::log("BookInfo::~BookInfo");
	if (m_relation)
	{
		delete m_relation;
		m_relation = nullptr;
	}
	ControlScene::getInstance()->end();
	App::log("BookInfo::~BookInfo---END");
}

Scene* BookInfo::createScene(SceneInfo* sceneInfo)
{
	App::log("BookInfo::createScene");
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = BookInfo::create(sceneInfo);

    // add layer as a child to scene
    scene->addChild(layer);
	layer->setTag(9);
    // return the scene
	App::log("BookInfo::createScene--END");
    return scene;
}

BookInfo* BookInfo::create(SceneInfo* sceneInfo)
{ 
	App::log("BookInfo::create");
	BookInfo *pRet = new(std::nothrow) BookInfo();
	if (pRet && pRet->init(sceneInfo))
	{
		pRet->autorelease();
		App::log("BookInfo::create---END");
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		App::log("BookInfo::create--END");
		return nullptr;
	}
}

bool BookInfo::init(SceneInfo* sceneInfo)
{
	App::log(" BookInfo::init");
	if (!Layer::init())
	{
		return false;
	}	

	//App::m_RunningScene = MySceneName::BookInfoScene;
	//Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	Director::getInstance()->getEventDispatcher()->setEnabled(true);
	YYXVisitor::getInstance()->bookInfoSceneInit();
	//--------------场景初始化--------------------------------------------
	m_bookId = sceneInfo->getData("bookId", Value(0)).asInt();
	//控制快速多次点击
	//m_eventTime = new long long(0);
	//控制层
	auto sp = Sprite::create();
	addChild(sp,11);
	auto lister_high = EventListenerTouchOneByOne::create();
	lister_high->onTouchBegan = [=](Touch* t, Event* e) {
		YYXStruct::initMapYYXStruct(m_tag, "touchismove", 0);
		return true;
	};
	lister_high->onTouchMoved = [&](Touch* t, Event* e) {
		auto move = t->getStartLocation() - t->getLocation();
		if (move.x < -30 || move.x >30 || move.y < -30 || move.y >30)
			YYXStruct::initMapYYXStruct(m_tag, "touchismove", 1);
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister_high, sp);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	//主场景  
	Data data;
	if (App::GetInstance()->getData(BOOKINFO_BOOKINFO_CSB,data))
		node = (Layer*)CSLoader::createNode(data);
	else
		node = (Layer*)CSLoader::createNode(BOOKINFO_BOOKINFO_CSB);
	node->setAnchorPoint(Vec2(0.5f, 0.5f));
	node->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(node);
	//红包抵扣
	hintHongbao();
	//评论数量
	auto pinglunshuliang = (Text*)node->getChildByName("pinglunshuliang");
	//封面
	auto cover = ( ImageView*)node->getChildByName("Image_2");
	//auto cover = (ImageView*)node->getChildByName(BOOKINFO_FIND_IMAGEVIEW_COVER);
	//售价符号 ￥ 控件名称
	auto price_fuhao = (Text*)node->getChildByName(BOOKINFO_FIND_TEXT_SELLINGPRICE_F);
	//剩余天数
	m_Text_RemainDay = (Text*)node->getChildByName(BOOKINFO_FIND_TEXT_REMAINDAY);

	//售价整数部分 控件名称
	m_price_z = (Text*)node->getChildByName(BOOKINFO_FIND_TEXT_SELLINGPRICE_Z);
	//售价小数部分 控件名称
	m_price_x = (Text*)node->getChildByName(BOOKINFO_FIND_TEXT_SELLINGPRICE_X);
	//原价格
	m_originalPrice = (Text*)node->getChildByName(BOOKINFO_FIND_TEXT_ORIGINALPRICE);
	//书名
	m_name = (Text*)node->getChildByName(BOOKINFO_FIND_BOOKNAME);
	//简介黄色
	auto introYellow = ( ImageView*) node->getChildByName(BOOKINFO_FIND_INTROYELLOW);
	auto introYellow2 = ( ImageView*) node->getChildByName(BOOKINFO_FIND_INTROYELLOW2);
	introYellow2->setTag(1);
	introYellow2->setTouchEnabled(true);
	introYellow->setTag(1);
	introYellow->setTouchEnabled(true);
	introYellow->setSwallowTouches(true);
	introYellow2->setSwallowTouches(true);
	//评论灰色
	auto commonGray = ( ImageView*)node->getChildByName(BOOKINFO_FIND_COMMONGRAY);
	auto commonGray2 = ( ImageView*)node->getChildByName(BOOKINFO_FIND_COMMONGRAY2);
	commonGray2->setTag(2);
	commonGray2->setTouchEnabled(true);
	commonGray->setTag(2);
	commonGray->setTouchEnabled(true);
	commonGray->setSwallowTouches(true);
	commonGray2->setSwallowTouches(true);

	//---------------------------界面--------------------------------------------------------------

	//简介是一个listview  创建控件,然后从内存中将数据载入控件
	m_View_Info = initIntroView();
	node->addChild(m_View_Info, 3);
	//评论
	m_View_Comment = initCommonView();
	m_View_Comment->setVisible(false);
	node->addChild(m_View_Comment, 2);
	
	//展示信息
	refreshUI();

	//封面
	string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
	std::string fileName = StringUtils::format("%d", m_bookId) + ".png";
	string bookCoverPath = dir + "/" + fileName;
	if (FileUtils::getInstance()->isFileExist(bookCoverPath))
	{
		App::log("存在");
		cover->loadTexture(bookCoverPath);
	}
	else {
		App::log("不存在");
		cover->loadTexture(PICTURE_BOOKINFO_COVER, TextureResType::PLIST);
	}
	initHttp();

	//返回按钮
	//m_createTime = App::getCurrentTime();
	auto homeButton = ( Button*)node->getChildByName(BOOKINFO_FIND_BACK);
	homeButton->removeFromParentAndCleanup(true);
	homeButton->setAnchorPoint(Vec2(0, 1));
	homeButton->setPosition(origin + Vec2(14, visibleSize.height - 14));
	addChild(homeButton, 10);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "BookInfoScenehomeButton", [=]() {
			back();
		});
	});
	initEvent();
	//页面的切换
	auto listenEvent = [=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "BookInfoScenelistenEvent", [=]() {
			YYXSound::getInstance()->playButtonSound();
			introYellow->loadTexture(BOOKINFO_ABSTRACT_PNG, TextureResType::PLIST);
			commonGray->loadTexture(BOOKINFO_COMMENT_PNG, TextureResType::PLIST);

			introYellow2->setTouchEnabled(true);
			commonGray2->setTouchEnabled(true);

			switch ((( ImageView*)sender)->getTag())
			{
			case 1:
				introYellow->loadTexture(BOOKINFO_ABSTRACT_Y_PNG, TextureResType::PLIST);
				isComment = false;
				pinglunshuliang->setTextColor(Color4B(12, 168, 137, 255));
				//简介界面
				m_View_Info->setVisible(true);
				m_View_Comment->setVisible(false);
				break;
			case 2:
				commonGray->loadTexture(BOOKINFO_COMMENT_Y_PNG, TextureResType::PLIST);
				isComment = true;
				//评论界面			
				m_View_Info->setVisible(false);
				m_View_Comment->setVisible(true);
				pinglunshuliang->setTextColor(Color4B(255, 246, 6, 255));
				auto ve = m_listview->getItems();
				if (ve.size() > 0)
					break;
				YYXLayer::showCommentListView(m_listview, m_bookId);
				string commentCountKey = StringUtils::format("comment_bookid=%d", m_bookId);//book评论的数量
				auto count = YYXStruct::getMapInt64(App::GetInstance()->myData, commentCountKey, 0);
				showCommentCount(count);
				//刷新列表
				Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("showCommentListView");
				auto listener2 =EventListenerCustom::create("showCommentListView", [=](EventCustom* e) {
					if (!isComment)
						return;
					int memberid = (int)e->getUserData();
					App::log(" --------------     run showCommentListView()   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^memberid = ", memberid);
					YYXSound::getInstance()->stopAll();
					YYXSound::getInstance()->resumeBackGroundMusic();
					YYXLayer::showCommentListView(m_listview, m_bookId);
					string commentCountKey = StringUtils::format("comment_bookid=%d", m_bookId);//book评论的数量
					auto count = YYXStruct::getMapInt64(App::GetInstance()->myData, commentCountKey, 0);
					showCommentCount(count);
				});
				Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener2, m_listview);
				m_listview->jumpToTop();
				break;
			};
			(( ImageView*)sender)->setTouchEnabled(false);
		});
	};
	introYellow2->addClickEventListener(listenEvent);
	commonGray2->addClickEventListener(listenEvent);
	m_name->setTouchEnabled(true);

	//1.8.0
	onClickLister();
	if (App::GetInstance()->m_me)
		m_relation->setControl(m_bookId, App::GetInstance()->getMemberId(), m_bookStatus);
	else
		m_relation->setControl(m_bookId, -999, m_bookStatus);
	//按钮界面单独初始化
	ButtonControl();
	App::log(" BookInfo::init--END", this->getReferenceCount());
	return true;
}

void BookInfo::initEvent()
{
	auto listener1 = EventListenerCustom::create(TAG_PINGLUNCOUNT, [=](EventCustom* e) {
		string commentCountKey = StringUtils::format("comment_bookid=%d", m_bookId);//book评论的数量
		auto count = YYXStruct::getMapInt64(App::GetInstance()->myData, commentCountKey, 0);
		showCommentCount(count);
	});
	auto pinglunshuliang = (Text*)node->getChildByName("pinglunshuliang");
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, pinglunshuliang);
	m_listenerName.push_back(TAG_PINGLUNCOUNT);

	auto listener2 = EventListenerCustom::create(TAG_READBOOK, [=](EventCustom* e) {
		int book = (int)e->getUserData();
		if (book == m_bookId)
			readBook();
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, node);
	m_listenerName.push_back(TAG_READBOOK);

	auto listener3 = EventListenerCustom::create(TAG_BUTTONCONTROL, [=](EventCustom* e) {
		ButtonControl();
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener3, node);
	m_listenerName.push_back(TAG_BUTTONCONTROL);

	//没有WiFi连接的通知
	auto listenNoWifi = EventListenerCustom::create("Down_No_Wifi", [=](EventCustom* event) {
		Toast::create(App::getString("NO_WIFI"));
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenNoWifi, node);
	m_listenerName.push_back("Down_No_Wifi");

	//------------------------------接受通知:刷新支付状态--------------------------------------------------------------------------
	auto listener4 = EventListenerCustom::create(TAG_BUYSUCCESSCALLBACK, [=](EventCustom* event) {
		callBackBuyBook();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener4, node);
	//------------------------------接受通知:刷新支付状态--------------------------------------------------------------------------
	m_listenerName.push_back(TAG_BUYSUCCESSCALLBACK);

	// ---------------------------- - 接受通知:充值成功后,自动继续购书--------------------------------------------------------------------------
	auto listener5 = EventListenerCustom::create(TAG_PAYSUCCESSGOTOBUG, [=](EventCustom* event) {
		buyBook();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener5, node);
	//-----------------------------接受通知:充值成功后,自动继续购书--END --------------------------------------------------------------------------
	m_listenerName.push_back(TAG_PAYSUCCESSGOTOBUG);

	//刷新红包抵扣提示
	auto listener6 = EventListenerCustom::create(TAG_DIKOUHONGBAO, [=](EventCustom* e) {
		hintHongbao();
	}); 
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener6, node);
	m_listenerName.push_back(TAG_DIKOUHONGBAO);

	//评论成功
	auto listener7 = EventListenerCustom::create(TAG_SENDCOMMENTSUCCESS, [=](EventCustom* e) {
		App::httpComment(m_bookId, []() {
			YYXLayer::sendNotify("showCommentListView", "", -1);
		});
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener7, node);
	m_listenerName.push_back(TAG_SENDCOMMENTSUCCESS);

	//------------------------------接受通知:刷新界面--------------------------------------------------------------------------
	auto listener8 = EventListenerCustom::create(TAG_BOOKINFOREFERUI, [=](EventCustom* event) {
		refreshUI();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener8, node);
	m_listenerName.push_back(TAG_BOOKINFOREFERUI);
	//------------------------------接受通知:刷新界面--------------------------------------------------------------------------

	////Began
	//auto touchlistener = EventListenerTouchOneByOne::create();
	//touchlistener->onTouchBegan = [=](Touch* t, Event* e) {
	//	zz(EventListenerTouchOneByOne  onTouchBegan  touchlistener)
	//		zzz(EventListenerTouchOneByOne  onTouchBegan  touchlistener)
	//		return true;
	//};
	////Moved
	//touchlistener->onTouchMoved = [=](Touch* t, Event* e) {
	//	zz(EventListenerTouchOneByOne  onTouchMoved  touchlistener)

	//	zzz(EventListenerTouchOneByOne  onTouchMoved  touchlistener)
	//};
	////Ended
	//touchlistener->onTouchEnded = [=](Touch* t, Event* e) {
	//	zz(EventListenerTouchOneByOne  onTouchMoved  touchlistener)
	//		zzz(EventListenerTouchOneByOne  onTouchMoved  touchlistener)
	//};
	////add
	//auto sp2 = Sprite::create();
	//addChild(sp2);
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchlistener, sp2);
}

void BookInfo::initHttp()
{
	//用户和书籍关系
	if (App::GetInstance()->m_me)
		App::whetherForVipDownloadJudgeInCharge(App::GetInstance()->getMemberId(), m_bookId, [=](int status) {
		m_bookStatus = status;
		App::log("whetherForVipDownloadJudgeInCharge_________________________=>", m_bookStatus);
		if(App::GetInstance()->m_me && m_relation)
			m_relation->setControl(m_bookId, App::GetInstance()->getMemberId(), status);
		else
			m_relation->setControl(m_bookId, -999, status);
		YYXLayer::sendNotify(TAG_BUTTONCONTROL);	
		YYXLayer::sendNotify(TAG_DIKOUHONGBAO);
	}, [](string error) {});
	//书籍详情
	string runKey = "bookInfoSceneHttpGetBookInfoSuccess";
	string errorKey = "bookInfoSceneHttpGetBookInfoFail";
	NetIntface::httpGetBookInfo(m_bookId, runKey, [](string json) {
		NetIntface::httpGetBookInfoCallBack(json, [](float avgScore, bool isvip, int bookId, int bookPage, int bookPrice100, int bookmarketPrice100, int remainTime, string bookAuthor,
			string bookSize, string bookPress, string bookIntroduction, string bookName, string bookPlayUrl, string bookCoverUrl, string bookViewUrl) {
			//解析正常
			//书籍信息 原价+书籍名称+ 书页数
			string BookNameKey = StringUtils::format("bookName+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookNameKey, bookmarketPrice100, bookName, (Ref*)bookPage);
			//现价+封面url+新书标记
			string BookPriceKey = StringUtils::format("bookPrice+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPriceKey, bookPrice100, bookCoverUrl);
			//下载封面
			string dir = App::getCoverDir();
			string filename = StringUtils::format("%d", bookId) + ".png";
			if (!FileUtils::getInstance()->isFileExist(dir + "/" + filename))
				YYXDownloadImages::GetInstance()->newDownloadImage(bookCoverUrl, dir, filename, high);
			//购书标记+下载URL + 	是否是vip书籍
			string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", bookId);
			if (isvip)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, -999, bookPlayUrl, (Ref*)1);
			else
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, -999, bookPlayUrl, (Ref*)0);
			//活动倒计时 + 书籍作者
			string BookAuthorKey = StringUtils::format("bookAuthor+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookAuthorKey, remainTime, bookAuthor);
			//出版社 + 书籍星级
			string BookPressKey = StringUtils::format("bookPress+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPressKey, avgScore, bookPress);
			//书籍资源大小
			string BookSizeKey = StringUtils::format("bookSize+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookSizeKey, -999, bookSize);
			//书籍介绍
			string bookIntroductionKey = StringUtils::format("bookIntroduction+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookIntroductionKey, -999, bookIntroduction);
			//试读url
			string bookViewUrlKey = StringUtils::format("bookViewUrl+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookViewUrlKey, -999, bookViewUrl);
			YYXLayer::sendNotify(TAG_BOOKINFOREFERUI);
			YYXLayer::sendNotify(TAG_BUTTONCONTROL);
		}, []() {
			//解析错误
			Toast::create(App::getString("HUOQUSHUJIXIANGQINGYICHANG"));
		});
	}, errorKey, [](string str) {
		Toast::create(App::getString("NETEXCEPTION"));
	});
	//获取评论
	App::httpComment(m_bookId, []() {
		YYXLayer::sendNotify("showCommentListView", "", -1);
		YYXLayer::sendNotify(TAG_PINGLUNCOUNT);
	});
}

void BookInfo::onEnterTransitionDidFinish()
{
	App::log("BookInfo::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	App::log("BookInfo::onEnterTransitionDidFinish---END");
}

void BookInfo::cleanup()
{
	App::log("BookInfo::cleanup");
	YYXSound::getInstance()->stopAll();
	YYXSound::getInstance()->resumeBackGroundMusic();
	App::log("BookInfo::cleanup-end");
}

//刷新界面
void BookInfo::refreshUI()
{
	App::log("BookInfo::refreshUI");
	//星级
	string BookPressKey = StringUtils::format("bookPress+bookID=%d", m_bookId);
	m_xingji = YYXStruct::getMapInt64(App::GetInstance()->myData, BookPressKey, 4);
	showXingji();
	//现价
	string BookPriceKey = StringUtils::format("bookPrice+bookID=%d", m_bookId);
	m_Selling100 = YYXStruct::getMapInt64(App::GetInstance()->myData, BookPriceKey, -999);
	//倒计时
	string BookAuthorKey = StringUtils::format("bookAuthor+bookID=%d", m_bookId);
	//价格
	showPrice();
	//书名
	string BookNameKey = StringUtils::format("bookName+bookID=%d", m_bookId);
	m_BookName = YYXStruct::getMapString(App::GetInstance()->myData, BookNameKey, "");
	if (m_BookName.length() > 45)
		m_name->setFontSize(42);
	else if(m_BookName.length() > 35)
		m_name->setFontSize(52);
	else if (m_BookName.length() > 25)
		m_name->setFontSize(62);
	m_name->setString(m_BookName);
	m_listview_Info->removeAllItems();
	////设置简介控件	
	m_listview_Info->pushBackCustomItem(creatItem_Introduction());

	//横线	
	m_listview_Info->pushBackCustomItem(creatItem_hengxian());

	//出版社
	m_listview_Info->pushBackCustomItem(creatItem_Press());

	//页码
	m_listview_Info->pushBackCustomItem(creatItem_bookPage());

	//作者
	m_listview_Info->pushBackCustomItem(creatItem_bookAuthor());

	m_listview_Info->jumpToTop();

	App::log("BookInfo::refreshUI----END");
}

void BookInfo::showTime(long long MillSecond)
{
	App::log("BookInfo::showTime");
	if (MillSecond <=0 && m_Text_RemainDay)
	{
		m_Text_RemainDay->setText("     ");
		return;
	}
	int li = (MillSecond % 1000) / 100;
	auto time = MillSecond / 1000;
	int day = 0, hour = 0, minute = 0, second = 0;
	day = time / 86400;
	hour = (time % 86400) / 3600;
	minute = (time % 3600) / 60;
	second = time % 60;  
	//m_Text_RemainDay->setText(StringUtils::format("%s%d%s%02d:%02d:%02d.%d", App::getString("STR_BOOINFOTIME"), day,App::getString("STR_BOOINFODAY"), hour, minute, second, li));
	if (day > 0)
		m_Text_RemainDay->setText(StringUtils::format("%s%d%s%", App::getString("STR_BOOINFOTIME"), day, App::getString("STR_BOOINFODAY")));
	else
		m_Text_RemainDay->setText(StringUtils::format("%s%d小时", App::getString("STR_BOOINFOTIME"), hour));
	App::log("BookInfo::showTime", MillSecond/1000);
	App::log("BookInfo::showTime---END");
}

//启动活动倒计时
void BookInfo::startSchedu()
{	
	App::log("BookInfo::startSchedu");
	//schedule([=](float f) {		
	//	showTime(m_startTime);
	//	m_startTime = m_startTime - 1000;
	//	if (m_startTime<=0)
	//		unschedule(BookInfoCountdown);
	//}, 1.0f, BookInfoCountdown);
	App::log("BookInfo::startSchedu---END");
}

////网络请求书籍详情
//void BookInfo::httpBookInfo(int BookId)
//{
//	App::log("BookInfo::httpBookInfo");
//	//请求获取当前bookId的书籍详情
//	HttpRequest* request = new HttpRequest();
//	request->setRequestType(HttpRequest::Type::GET);
//	auto strUrl = std::string(IP).append(NET_BOOKINFO).append(StringUtils::format("%d", BookId))
//		.append("?memberId=").append(App::getMemberID()).append("&resource=").append(App::GetInstance()->m_resource);
//	request->setUrl(strUrl.c_str());
//	//App::log(strUrl.c_str());
//	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {//请求回调
//		if (!response)
//		{
//			notify(MESSAGEBOX, App::getString("STR_NETERROR"));
//			App::log("BookInfo::httpBookInfo---END");
//			return;
//		}
//		if (!response->isSucceed()) 
//		{
//			App::log("connect failed!  error buffer");
//			notify(MESSAGEBOX, App::getString("STR_NETERROR"));
//			App::log("BookInfo::httpBookInfo---END");
//			return;
//		}
//		response->getHttpRequest();
//		//json解析
//		std::vector<char> *buffer = response->getResponseData();
//		std::string str(buffer->begin(), buffer->end());
//		App::log(str);
//		std::thread callback(&BookInfo::httpBookInfoCallback,this,str, BookId);
//		callback.detach();
//	});
//	HttpClient::getInstance()->send(request);
//	request->release();
//	App::log("BookInfo::httpBookInfo---END");
//}

//网络请求书籍详情回调函数
//void BookInfo::httpBookInfoCallback(std::string str, int BookId)
//{
//	App::log("BookInfo::httpBookInfoCallback");
//	if (App::GetInstance()->m_RunningScene != MySceneName::BookInfoScene)
//	{
//		App::log("BookInfo::httpBookInfoCallback---END");
//		return;
//	}
//	rapidjson::Document doc;
//	doc.Parse<0>(str.c_str());
//	if (doc.HasParseError())//json parse error
//	{
//		notify(MESSAGEBOX, App::getString("STR_NETERROR"));
//		App::log("BookInfo::httpBookInfoCallback--END");
//		return;
//	}
//	if (!doc.IsObject() || !doc.HasMember("code") || !doc.HasMember("data"))
//	{
//		notify(MESSAGEBOX, App::getString("STR_NETERROR"));
//		App::log("BookInfo::httpBookInfoCallback--END");
//		return;
//	}
//	rapidjson::Value &value = doc["code"];
//	if (value.IsInt() && value.GetInt() == 0) 
//	{//请求正常		
//		App::log("bookinfo connect successful!");
//		rapidjson::Value &data = doc["data"];
//		if (data.IsObject() ) {
//			//1.解析
//			auto bookId = App::analysisJsonInt(data, "bookId");
//			auto bookDownloadSize = App::analysisJsonInt(data, "bookDownloadSize");
//			auto avgScore100 = App::analysisJsonDouble(data, "avgScore") * 100;
//			auto bookPage = App::analysisJsonInt(data, "bookPage");
//			auto bookAge = App::analysisJsonString(data, "bookAge");
//			auto remainTime = App::analysisJsonInt(data, "remainTime");
//			auto bookPrice = App::analysisJsonDouble(data, "bookPrice") * 100;
//			auto bookmarketPrice = App::analysisJsonDouble(data, "bookmarketPrice") * 100;
//			auto bookName = App::analysisJsonString(data, "bookName");
//			auto bookPress = App::analysisJsonString(data, "bookPress");
//			auto bookCoverUrl = App::analysisJsonString(data, "bookCoverUrl");
//			auto bookPlayUrl = App::analysisJsonString(data, "bookPlayUrl");
//			auto bookViewUrl = App::analysisJsonString(data, "bookViewUrl");
//			auto upTime = App::getCurrentTime();
//			auto bookIntroduction = App::analysisJsonString(data, "bookIntroduction");
//			auto bookAuthor = App::analysisJsonString(data, "bookAuthor");			
//			//2.更新内存
//			m_Selling100 = bookPrice;
//			m_OriginalPrice100 = bookmarketPrice;
//			m_time = remainTime;
//			m_upTime = upTime;
//			m_BookName = bookName;
//			m_bookIntroduction = bookIntroduction;
//			m_bookPress = bookPress;
//			m_bookPage = bookPage;
//			m_bookAuthor = bookAuthor;
//			m_coverUrl = bookCoverUrl; 
//			m_playUrl = bookPlayUrl;
//			/*//查数据库更新一下 封面地址
//			auto resultres = App::sqliteBookPicture(App::sqliteOpen(), m_bookId);
//			if (resultres.size() > 0)
//			{
//				if(&m_cover)
//					m_cover = resultres[0]["path"].stringPara;
//				if(&m_DownloadTime)
//					m_DownloadTime = resultres[0]["downloadtime"].intPara;
//			}*/
//
//			//3.通知界面刷新
//			auto moveTime = App::getCurrentTime() - m_upTime;
//			m_startTime = (m_time - moveTime) * 1000;
//			m_startTime = m_time * 1000;
//			notify(REFRESHUI);			
//			//startSchedu();
//			//4.写入数据库			
//			unordered_map<string, string> paraInfoSTR;
//			unordered_map<string, long long> paraInfoINT;
//			paraInfoINT["bookId"] = bookId;
//			paraInfoINT["bookDownloadSize"] = bookDownloadSize;
//			paraInfoINT["evaluation_good_star"] = avgScore100;
//			paraInfoINT["bookPage"] = bookPage;
//			paraInfoSTR["bookAge"] = bookAge;
//			paraInfoINT["remainTime"] = remainTime;
//			paraInfoINT["bookPrice"] = bookPrice;
//			paraInfoINT["bookmarketPrice"] = bookmarketPrice;
//			paraInfoSTR["bookName"] = bookName;
//			paraInfoSTR["bookPress"] = bookPress;
//			paraInfoSTR["bookPic"] = bookCoverUrl;
//			paraInfoSTR["bookPlayUrl"] = bookPlayUrl;
//			paraInfoSTR["bookViewUrl"] = bookViewUrl;
//			paraInfoINT["upTime"] = upTime;
//			paraInfoSTR["bookIntroduction"] = bookIntroduction;
//			paraInfoSTR["bookAuthor"] = bookAuthor;
//			SqliteManager::DeleteData(App::sqliteOpen(), DB_BOOKINFO, std::string("where bookId=").append(StringUtils::format("%d", BookId)));
//			bool result = SqliteManager::InsertData(App::sqliteOpen(), DB_BOOKINFO, paraInfoSTR, paraInfoINT);
//			App::sqliteClose();
//		}
//	}
//	App::log("BookInfo::httpBookInfoCallback--END");
//}

/*void BookInfo::httpGetBookComment(int bookId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	getBookComments(bookId);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	getBookComments(bookId);
#endif
}

void BookInfo::getBookComments(int BookId)
{
	App::log("BookInfo::httpBookComments");
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	auto strUrl = std::string(IP).append(NET_BOOKCOMMENT).append(StringUtils::format("?bookId=%d", BookId))
		.append("&memberId=").append(App::getMemberID()).append("&resource=").append(App::GetInstance()->m_resource);
	request->setUrl(strUrl.c_str());
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {//请求回调
		if (!response)
		{
			App::log("BookInfo::httpBookComments--END");
			return;
		}
		if (!response->isSucceed())
		{
			App::log("connect failed!  error buffer");
			App::log("BookInfo::httpBookComments--END");
			return;
		}
		response->getHttpRequest();
		//json解析
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		App::log(str);
		std::thread callback(&BookInfo::httpBookCommentsCallback, this, str);
		callback.detach();
	});
	HttpClient::getInstance()->send(request);
	request->release();
	App::log("BookInfo::httpBookComments--END");
}

void BookInfo::httpBookCommentsCallback(std::string str)
{
	App::log("BookInfo::httpBookCommentsCallback");
	if (App::GetInstance()->m_RunningScene != MySceneName::BookInfoScene)
	{
		App::log("BookInfo::httpBookCommentsCallback---END");
		return;
	}
	rapidjson::Document doc;
	doc.Parse<0>(str.c_str());
	if (doc.HasParseError())//json parse error
	{
		App::log("BookInfo::httpBookCommentsCallback--END");
		return;
	}
	if (!doc.IsObject() || !doc.HasMember("code") || !doc.HasMember("data"))
	{
		App::log("BookInfo::httpBookCommentsCallback--END");
		return;
	}
	rapidjson::Value &value = doc["code"];
	if (value.IsInt() && value.GetInt() == 0)
	{//请求正常		
		App::log("connect successful!");
		rapidjson::Value &data = doc["data"];
		if (data.IsArray())
		{
			m_comment.clear();
			for (rapidjson::SizeType i = 0; i < data.Size(); i++)
			{
				rapidjson::Value &item = data[i];
				int gevalId = App::analysisJsonInt(item, "gevalId");
				auto memberId = App::analysisJsonInt(item, "memberId");
				auto memberName = App::analysisJsonString(item, "memberName");
				auto title = App::analysisJsonString(item, "title");
				auto score = App::analysisJsonInt(item, "score");
				auto content = App::analysisJsonString(item, "content");
				auto commentTime = App::analysisJsonInt(item, "commentTime");
				auto gevalState = App::analysisJsonInt(item, "gevalState");
				auto gevalOrderid = App::analysisJsonInt(item, "gevalOrderid");
				//更新数据
				std::map<std::string, ParaType> map;
				App::initParaType(map, "gevalId", gevalId, "");
				App::initParaType(map, "memberId", memberId, "");
				App::initParaType(map, "memberName", -999, memberName);
				App::initParaType(map, "title", -999, title);
				App::initParaType(map, "score", score, "");
				App::initParaType(map, "content", -999, content);
				App::initParaType(map, "commentTime", commentTime, "");
				App::initParaType(map, "gevalState", gevalState, "");
				App::initParaType(map, "gevalOrderid", gevalOrderid, "");
				m_comment.push_back(map);
			}
		}
	}	
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([]() {
		EventCustom event("showComment");
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
	});
	App::log("BookInfo::httpBookCommentsCallback--END");
}*/

//重新写入数据库
//void BookInfo::retryInsertData(string tableName, unordered_map<string, string> paraInfoSTR, unordered_map<string, long long> paraInfoINT)
//{
//	App::log("BookInfo::retryInsertData");
//	int num = 0;
//	bool result = false;
//	do 
//	{
//		App::ccsleep(300);
//		if (num >3)
//			break;
//		SqliteManager::DeleteData(App::sqliteOpen(), DB_BOOKINFO, std::string("where bookId=").append(StringUtils::format("%d", paraInfoINT["bookId"])));
//		result = SqliteManager::InsertData(App::sqliteOpen(), DB_BOOKINFO, paraInfoSTR, paraInfoINT);
//		App::sqliteClose();
//		++num;
//	} while (!result);
//	if (result)
//		App::log("[重新写入数据库成功]");
//	else
//		App::log("[重新写入数据库失败]");
//	App::log("BookInfo::retryInsertData---END");
//}

//初始化封面,价格,时间
/*void BookInfo::loadBackGroundData()
{
	App::log("BookInfo::loadBackGroundData");
	//从数据库中读出数据
	long long bookPrice100 = -999, originalPrice100 = -999, bookDownloadSize = -999
		, evaluation_good_star100 = -999, bookPage = -999, remainTime = -999
		, upTime = 0, downloadtime = 0;
	std::string bookName = "", bookPress = "", bookPicURL = "", bookAge = ""
		, bookPlayUrl = "", bookViewUrl = "", path = "", bookIntroduction = "", bookAuthor = "";
	auto result = App::sqliteBookInfo(App::sqliteOpen(), m_bookId);
	if (result.size() > 0)
	{
		bookName = result[0]["bookName"].stringPara;
		bookPress = result[0]["bookPress"].stringPara;
		bookPicURL = result[0]["bookPic"].stringPara;
		bookPrice100 = result[0]["bookPrice"].intPara;
		originalPrice100 = result[0]["bookmarketPrice"].intPara;
		bookDownloadSize = result[0]["bookDownloadSize"].intPara;
		evaluation_good_star100 = result[0]["evaluation_good_star"].intPara;
		bookPage = result[0]["bookPage"].intPara;
		bookAge = result[0]["bookAge"].stringPara;
		bookPlayUrl = result[0]["bookPlayUrl"].stringPara;
		bookViewUrl = result[0]["bookViewUrl"].stringPara;
		remainTime = result[0]["remainTime"].intPara;
		upTime = result[0]["upTime"].intPara;
		bookIntroduction = result[0]["bookIntroduction"].stringPara;
		bookAuthor = result[0]["bookAuthor"].stringPara;
	}
	auto resultres = App::sqliteBookPicture(App::sqliteOpen(), m_bookId);
	if (resultres.size() > 0)
	{
		path = resultres[0]["path"].stringPara;
		downloadtime = resultres[0]["downloadtime"].intPara;
	}
	//获取信息保存在内存中
	m_Selling100 = bookPrice100;
	m_OriginalPrice100 = originalPrice100;
	m_time = remainTime;
	m_upTime = upTime;
	//m_cover = path;
	m_coverUrl = bookPicURL;
	m_playUrl = bookPlayUrl;
	m_DownloadTime = downloadtime;
	m_BookName = bookName;
	m_bookPress = bookPress;
	m_bookPage = bookPage;
	m_bookAuthor = bookAuthor;
	m_bookIntroduction = bookIntroduction;
	App::log("BookInfo::loadBackGroundData---END");
}*/

//展示价格
void BookInfo::showPrice()
{	
	App::log("BookInfo::showPrice");
	//---------------内容---------------
	std::string str_price_z = "--", str_price_x = ".--", str_or = std::string(App::getString("STR_MONEY")).append("8.00");
	if (m_Selling100 >= 0 && m_Selling100 < 100000)
	{
		str_price_z = StringUtils::format("%d", m_Selling100 / 100);
		str_price_x = StringUtils::format(".%02d", m_Selling100 % 100);
	}
	string BookNameKey = StringUtils::format("bookName+bookID=%d", m_bookId);
	int bookmarketPrice100 = YYXStruct::getMapInt64(App::GetInstance()->myData, BookNameKey, -999);
	if (bookmarketPrice100 >= 1000 && bookmarketPrice100 < 100000)
	{
		str_or = StringUtils::format("%s%.02f", App::getString("STR_MONEY"), bookmarketPrice100 / 100.0);
	}
	if (bookmarketPrice100 >=0 && bookmarketPrice100 <1000)
	{
		str_or = StringUtils::format("%s %.02f", App::getString("STR_MONEY"), bookmarketPrice100 / 100.0);
	}
	
	//--------------设置----------------
	m_price_z->setAnchorPoint(Vec2(0, 0));
	m_price_x->setAnchorPoint(Vec2(0, 0));
	m_price_z->setText(str_price_z);
	auto width = m_price_z->getSize().width;
	m_price_x->setText(str_price_x);
	m_price_x->setPosition(Vec2(m_price_z->getPosition() .x+width , m_price_x->getPosition().y));
	m_originalPrice->setText(str_or);
	App::log("BookInfo::showPrice---END");
}

//初始化简介界面
Layer* BookInfo::initIntroView()
{
	App::log("BookInfo::initIntroView");
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto layer = LayerColor::create();
	layer->setColor(Color3B::RED);
	layer->setContentSize(Size(1094, 614));
	layer->setAnchorPoint(Vec2(0.5, 0.5));

	layer->setPosition(visibleSize / 2);
	m_listview_Info = ListView::create();
	m_listview_Info->setGravity(ListView::Gravity::LEFT);
	m_listview_Info->setDirection(SCROLLVIEW_DIR_VERTICAL);
	m_listview_Info->setBounceEnabled(true);
	m_listview_Info->setSize(Size(320, 300));
	m_listview_Info->setItemsMargin(20);
	m_listview_Info->setScrollBarEnabled(false);
	layer->addChild(m_listview_Info);
	m_listview_Info->setAnchorPoint(Vec2(0, 1));
	m_listview_Info->setPosition(Vec2(1550*0.4 - visibleSize.width / 2, 1230*0.4 - visibleSize.height / 2));
	////设置简介控件	
	//m_listview_Info->pushBackCustomItem(creatItem_Introduction());

	////横线	
	//m_listview_Info->pushBackCustomItem(creatItem_hengxian());

	////出版社
	//m_listview_Info->pushBackCustomItem(creatItem_Press());

	////页码
	//m_listview_Info->pushBackCustomItem(creatItem_bookPage());

	////作者
	//m_listview_Info->pushBackCustomItem(creatItem_bookAuthor());
	App::log("BookInfo::initIntroView---END");
	return layer;
}

//创建文字简介item
Layout* BookInfo::creatItem_Introduction()
{
	App::log("BookInfo::creatItem_Introduction");
	auto layout = Layout::create();
	string bookIntroductionKey = StringUtils::format("bookIntroduction+bookID=%d", m_bookId);
	auto str = YYXStruct::getMapString(App::GetInstance()->myData, bookIntroductionKey, "");
	auto m_text_bookIntroduction = Label::create("    " + str, "FZHLJW.TTF", 36, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	m_text_bookIntroduction->setMaxLineWidth(640);
	m_text_bookIntroduction->setTextColor(Color4B::BLACK);
	m_text_bookIntroduction->setAnchorPoint(Vec2(0, 0));
	App::log("                                                                            " + str);
	App::log("  length =  ",  str.length() / 3);
	if (str.length() <= 40 * 3)
	{
		m_text_bookIntroduction->setAdditionalKerning(25);//字体间距
		m_text_bookIntroduction->setLineHeight(76);//行间距
	}
	else if (str.length() <= 55 * 3)
	{
		m_text_bookIntroduction->setAdditionalKerning(20);//字体间距
		m_text_bookIntroduction->setLineHeight(66);//行间距
	}
	else if (str.length() <= 70*3)
	{
		m_text_bookIntroduction->setAdditionalKerning(15);//字体间距
		m_text_bookIntroduction->setLineHeight(54);//行间距
	}
	else if (str.length() <= 85 * 3)
	{
		m_text_bookIntroduction->setAdditionalKerning(13);//字体间距
		m_text_bookIntroduction->setLineHeight(52);//行间距
	}
	else if (str.length() <= 102 * 3)
	{
		m_text_bookIntroduction->setAdditionalKerning(10);//字体间距
		m_text_bookIntroduction->setLineHeight(48);//行间距
	}
	else
	{
		m_text_bookIntroduction->setAdditionalKerning(8);//字体间距
		m_text_bookIntroduction->setLineHeight(44);//行间距
	}
	m_text_bookIntroduction->setScale(0.5);
	layout->setContentSize(Size(320, m_text_bookIntroduction->getContentSize().height/2));
	layout->addChild(m_text_bookIntroduction);
	App::log("BookInfo::creatItem_Introduction---END");
	return layout;
}

//创建横线item
Layout* BookInfo::creatItem_hengxian()
{
	App::log("BookInfo::creatItem_hengxian");
	auto layout1 = Layout::create();
	auto henvs = Label::create("__________________________________", "FZHLJW.TTF", 48, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	henvs->setTextColor(Color4B::GRAY);
	henvs->setScale(0.5);
	henvs->setAnchorPoint(Vec2(0, 0));
	layout1->setContentSize(Size(320, henvs->getContentSize().height / 3));
	layout1->addChild(henvs);
	App::log("BookInfo::creatItem_hengxian---END");
	return layout1;
}

//创建出版社item
Layout* BookInfo::creatItem_Press()
{
	App::log("BookInfo::creatItem_Press");
	auto layout2 = Layout::create();
	string BookPressKey = StringUtils::format("bookPress+bookID=%d", m_bookId);
	auto press = YYXStruct::getMapString(App::GetInstance()->myData, BookPressKey, "");
	auto m_text_bookPress = Label::create(press, "FZHLJW.TTF", 34, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	m_text_bookPress->setTextColor(Color4B(70, 110, 157, 255));
	m_text_bookPress->setAnchorPoint(Vec2(0, 0));
	m_text_bookPress->setAdditionalKerning(8);//字体间距
	m_text_bookPress->setScale(0.5);
	layout2->setContentSize(m_text_bookPress->getContentSize()/2);
	layout2->addChild(m_text_bookPress);
	App::log("BookInfo::creatItem_Press--END");
	return layout2;
}

//创建页数item
Layout* BookInfo::creatItem_bookPage()
{
	App::log("BookInfo::creatItem_bookPage");
	auto layout3 = Layout::create();
	string str = "网络延迟请等待....";
	string BookNameKey = StringUtils::format("bookName+bookID=%d", m_bookId);
	auto page = (long)YYXStruct::getMapRef(App::GetInstance()->myData, BookNameKey, (Ref*)-999);
	if (page != -999)
		str = App::getString("STR_PAGENUMBER") + StringUtils::format("%d", page);
	auto m_text_bookPage = Label::create(str, "FZHLJW.TTF", 34, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	m_text_bookPage->setAnchorPoint(Vec2(0, 0));
	m_text_bookPage->setAdditionalKerning(8);			//字体间距
	m_text_bookPage->setTextColor(Color4B::GRAY);
	m_text_bookPage->setScale(0.5);
	layout3->setContentSize(m_text_bookPage->getContentSize()/2);
	layout3->addChild(m_text_bookPage);
	App::log("BookInfo::creatItem_bookPage--END");
	return layout3;
}

//创建作者item
Layout* BookInfo::creatItem_bookAuthor()
{
	App::log("BookInfo::creatItem_bookAuthor");
	auto layout4 = Layout::create();
	string str = "";
	string BookAuthorKey = StringUtils::format("bookAuthor+bookID=%d", m_bookId);
	string author = YYXStruct::getMapString(App::GetInstance()->myData, BookAuthorKey, "");
	if (author.length() !=0 )
		str = App::getString("STR_BOOKAUTHOE") + author;
	auto m_text_bookAuthor = Label::create(str, "FZHLJW.TTF",34, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	m_text_bookAuthor->setTextColor(Color4B::GRAY);
	m_text_bookAuthor->setAnchorPoint(Vec2(0, 0));
	m_text_bookAuthor->setAdditionalKerning(8);			//字体间距
	m_text_bookAuthor->setScale(0.5);
	m_text_bookAuthor->setMaxLineWidth(320*2);
	layout4->setContentSize(Size(320, m_text_bookAuthor->getContentSize().height/2));
	layout4->addChild(m_text_bookAuthor);
	App::log("BookInfo::creatItem_bookAuthor--END "+ str);
	return layout4;
}


//初始化评论界面
Layer* BookInfo::initCommonView()
{
	App::log("BookInfo::initCommonView");
	//图层
	Data data;
	Layer* pinglun;
	if (App::GetInstance()->getData(BOOKINFO_PINGLUN_CSB,data))
		pinglun = (Layer*)CSLoader::createNode(data);
	else
		pinglun = (Layer*)CSLoader::createNode(BOOKINFO_PINGLUN_CSB);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//评论按钮
	auto commentMe = ( Button*)pinglun->getChildByName(BOOKINFO_BUTTON_PINGLUN);
	commentMe->setTouchEnabled(true);
	commentMe->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "BookInfoScenecommentMe", [=]() {
			YYXSound::getInstance()->playButtonSound();
			//用户未登录
			if (App::GetInstance()->m_me == nullptr) {
				Toast::create(App::getString("COMMENT_TIP_LOGIN"));
				return;
			}
			//未购买
			if (m_relation->IsBookBuy() == false && m_relation->IsMemberVIP() == false) {
				Toast::create(App::getString("COMMENT_TIP_BUY"));
				return;
			}
			auto pScene = Director::getInstance()->getRunningScene();
			pScene->addChild(SendComment::create(m_bookId));
		});
	});
	m_listview = (ListView*)pinglun->getChildByName(BOOKINFO_FIND_LISTVIEW_BYNAME_COMMENT);
	m_listview->setScrollBarEnabled(false);
	m_listview->setSize(Size(320, 270));
	App::log("BookInfo::initCommonView---END");
	return pinglun;
}

//选出最优红包
int BookInfo::getOptimalRedPacket()
{
	int count = App::GetInstance()->m_redPacket.size();
	int optimal = 0;
	int optimalID = -999;
	if (count <= 0)
		return -1;//无可用红包
	optimal = App::GetInstance()->m_redPacket[0]["coupon_amount"].intData;
	optimalID = App::GetInstance()->m_redPacket[0]["coupon_id"].intData;
	int difference = optimal - m_Selling100;
	if (difference > 0)
	{
		for (int i = 1; i < count; i++)
		{
			auto coupon_amount = App::GetInstance()->m_redPacket[i]["coupon_amount"].intData;
			auto coupon_id = App::GetInstance()->m_redPacket[i]["coupon_id"].intData;
			auto difference2 = coupon_amount - m_Selling100;
			if (difference2 >0 && difference > difference2)
			{
				optimal = coupon_amount;
				optimalID = coupon_id;
				difference = difference2;
			}
		}
	}
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, StringUtils::format("bookId=%d+redpacket", m_bookId), optimal, "", (Ref*)optimalID);
	return optimal;
}

void BookInfo::back()
{
	App::log("BookInfo::back");
	//Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	//YYXLayer::PLAYBUTTON();
	//App::GetInstance()->popBack(MySceneName::BookInfoScene);
	//Index::BackPreviousScene();
	auto control = ControlScene::getInstance();
	control->backFromScene();
	App::log("BookInfo::back---END");
}

//去充值
void BookInfo::recharge()
{
	YYXVisitor::getInstance()->hintLogin([=]() {
		auto recharge = Charger::create();
		addChild(recharge);
	}, [=]() {
		gotoLogin();
	}, [=]() {
		auto recharge = Charger::create();
		addChild(recharge);
	});
}

//获取有效红包
void BookInfo::httpGetUserRedPackets()
{
	string url = string(IP).append(NET_USERREDPACKET).append("?memberId=").append(App::GetInstance()->getMemberID());
	string runKey = "BookInfoSceneHttpGetUserRedPacketSuccess";
	string errorKey = "BookInfoSceneHttpGetUserRedPacketFail";
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
			YYXLayer::sendNotify("referRedPackets", "BookInfo");
		}, []() {
			Toast::create(App::getString("HONGBAOHUOQUSHIBAI"));
		});
	}, errorKey, [](string str) {
		Toast::create(App::getString("NETEXCEPTION"));
	});
}

//红包抵扣提示
void BookInfo::hintHongbao()
{
	auto hongbaodikou = (ImageView*)node->getChildByName("Image_21");
	auto Tex = (Text*)node->getChildByName("dikou");
	if (hongbaodikou == nullptr || Tex == nullptr)
		return;
	hongbaodikou->setVisible(false);
	Tex->setVisible(false);
	if (m_relation->BookId() < 0)
		return;
	if (!m_relation->IsBookBuy())//未购买
	{
		float showred = getOptimalRedPacket() / 100.0;
		if (showred > 0) {//并且有红包
			hongbaodikou->setVisible(true);
			Tex->setVisible(true);
			if (Tex)
				Tex->setText(StringUtils::format("%s%.00f%s", App::getString("HONGBAODIKOU"), showred, App::getString("YUAN")));
		}
	}
}

//网络下载完整书籍
string BookInfo::DownLoadBook(int bookId, string bookPlayUrl, string ZipName)
{
	string beginKey = "bookInfoSceneDownLoadBookBegin";
	string progressKey = "bookInfoSceneDownLoadBookProgress";
	string endKey = "bookInfoSceneDownLoadBookEnd";
	string bookTag = YYXDownload::GetInstance()->download(bookPlayUrl, App::getReadZipDir(), ZipName, beginKey, [=](YYXStruct data) {
		App::downloadFailDeleteFiles(App::getReadZipDir() + "/" + ZipName, App::getBookReadPath(bookId));
	}, progressKey, [](YYXStruct data) {
		auto taskTag = YYXStruct::getStringData(data, "");
		if (data.intData <= 0)
			return;
		if (data.intData <= 99)
		{
				YYXLayer::sendNotify(taskTag + "_BookProgressing", "", data.intData);
		}
		else
			YYXLayer::sendNotify(taskTag + "_BookProgressing", "", 99);
	}, endKey, [=](YYXStruct data) {
		auto taskTag2 = YYXStruct::getStringData(data, "");
		int code = YYXStruct::getInt64Data(data, -2);
		string bookName = YYXStruct::getMapString(App::GetInstance()->myData, taskTag2, "");
		int bookid = YYXStruct::getMapInt64(App::GetInstance()->myData, taskTag2, -999);
		if (code == 0)
		{
			NetIntface::AddDownLoadRecord(App::GetInstance()->getMemberId(), bookId, "", [](string str) {}, "", [](string str) {});
			//下载成功,解压									
			std::thread(&YYXDownload::uncompress, App::getReadZipDir(), StringUtils::format("%d", bookid), App::getReadDir(), [=](string zipPath) {
				YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 100);
				//App::addDownloadBookRecord(bookid);
				DownloadBook::getInstance()->addBook(bookid, (int)YYXLayer::getCurrentTime4Second());
				YYXLayer::sendNotify(TAG_READBOOK, "", bookid);
			}, [=](string zipPath) {
				//解压失败
				string bookNameError = bookName + App::getString("JIEYACUOWUCHANGXINXIAZAI") + StringUtils::format("%d", bookid);
				Toast::create(bookNameError.c_str());
				YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 999);
				App::downloadFailDeleteFiles(App::getReadZipDir() + "/" + ZipName, App::getBookReadPath(bookId));
			}).detach();
		}
		else
		{
			//下载错误
			if (!YYXDownload::GetInstance()->isPause(taskTag2))
			{
				YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 999);
				string bookNameError = bookName + App::getString("XIAZAICUOWUCHANGXINXIAZAI");
				Toast::create(bookNameError.c_str());
				App::downloadFailDeleteFiles(App::getReadZipDir() + "/" + ZipName, App::getBookReadPath(m_bookId));
			}
		}
	});
	return bookTag;
}

void BookInfo::log(int stauts)
{
	string str = "";
	switch (stauts)
	{
	case 0:
		str = "FREEPRICE";
		break;
	case 1:
		str = "BUYBOOK";
		break;
	case 2:
		str = "DOWNLOAD";
		break;
	case 3:
		str = "VIPDOWNLOAD";
		break;
	case 4:
		str = "READBOOK";
		break;
	case 5:
		str = "PROGRESSING";
		break;
	case 6:
		str = "PAUSE";
		break;
	}
	App::log("*******************  BOOK = >"+str);
}

void BookInfo::ButtonControl()
{
	m_relation->BookStauts(m_bookStatus);
	//提示是购买过的 还是租过的
	hintIsBuyIsRent();
	if (m_relation->IsBookBuy())
	{
		//已购
		showBuyButtons(2);
		return;
	}
	if (m_relation->IsBookRent())
	{
		//租过
		if (YYXVisitor::getInstance()->getVisitorMode())
		{//游客
			
		}
		else
		{
			if (m_relation->IsMemberVIP())
			{//VIP
				showBuyButtons(3);
				return;
			}
			else
			{
				//普通用户
			}
		}
	}
	//未购买或者未租过也没有下载过
	if (m_relation->IsBookFree())//免费
	{
		showBuyButtons(2);		
	}
	else//收费
	{
		if (YYXVisitor::getInstance()->getVisitorMode())
		{
			showBuyButtons(1);
		}
		else
		{
			//正常用户
			if (m_relation->IsMemberVIP())
			{//VIP
				showBuyButtons(3);
			}
			else
			{//普通用户
				showBuyButtons(1);
			}
		}
	}
}

void BookInfo::showBuyButtons(int val)
{
	switch (val)
	{
	case 1://普通用户 没买 没租
		userNoBuyNoRent();
		break;
	case 2:// 任意用户 买了
		userBuy();
		break;
	case 3:// vip用户 
		visitorRent();
		break;
	}
}

void BookInfo::visitorNoBuyNoRent()
{
	//免费试读 购买 购买年卡
	auto mianfeishidu = (Button*)node->getChildByName("mianfeishidu");
	auto goumaihong = (Button*)node->getChildByName("goumaihong");
	auto goumainianka = (Button*)node->getChildByName("goumainianka");

	//购买 免费阅读 阅读
	auto goumai = (Button*)node->getChildByName("goumai");
	auto mianfeiyuedu = (Button*)node->getChildByName("mianfeiyuedu");
	auto yuedu = (Button*)node->getChildByName("yuedu");

	//阅读
	auto duliyuedu = (Button*)node->getChildByName("duliyuedu");

	mianfeishidu->setTitleText(App::getString("MIANFEISHIDU"));
	yuedu->setTitleText(App::getString("STR_READ"));
	mianfeiyuedu->setTitleText(App::getString("MIANFEIYUEDU"));
	duliyuedu->setTitleText(App::getString("STR_READ"));

	mianfeishidu->setVisible(true);
	goumaihong->setVisible(true);
	if (m_relation->IsMemberVIP())
		goumainianka->setVisible(false);
	else
		goumainianka->setVisible(true);

	goumai->setVisible(true);
	mianfeiyuedu->setVisible(true);
	yuedu->setVisible(true);

	duliyuedu->setVisible(false);
}

void BookInfo::userNoBuyNoRent()
{
	//免费试读 购买 购买年卡
	auto mianfeishidu = (Button*)node->getChildByName("mianfeishidu");
	auto goumaihong = (Button*)node->getChildByName("goumaihong");
	auto goumainianka = (Button*)node->getChildByName("goumainianka");

	//购买 免费阅读 阅读
	auto goumai = (Button*)node->getChildByName("goumai");
	auto mianfeiyuedu = (Button*)node->getChildByName("mianfeiyuedu");
	auto yuedu = (Button*)node->getChildByName("yuedu");

	//阅读
	auto duliyuedu = (Button*)node->getChildByName("duliyuedu");

	mianfeishidu->setTitleText(App::getString("MIANFEISHIDU"));
	yuedu->setTitleText(App::getString("STR_READ"));
	mianfeiyuedu->setTitleText(App::getString("MIANFEIYUEDU"));
	duliyuedu->setTitleText(App::getString("STR_READ"));

	mianfeishidu->setVisible(true);
	goumaihong->setVisible(true);
	if (m_relation->IsMemberVIP())
		goumainianka->setVisible(false);
	else
		goumainianka->setVisible(true);
	goumai->setVisible(false);
	mianfeiyuedu->setVisible(false);
	yuedu->setVisible(false);
	duliyuedu->setVisible(false);
}

void BookInfo::visitorRent()
{	
	//免费试读 购买 购买年卡
	auto mianfeishidu = (Button*)node->getChildByName("mianfeishidu");
	auto goumaihong = (Button*)node->getChildByName("goumaihong");
	auto goumainianka = (Button*)node->getChildByName("goumainianka");

	//购买 免费阅读 阅读
	auto goumai = (Button*)node->getChildByName("goumai");
	auto mianfeiyuedu = (Button*)node->getChildByName("mianfeiyuedu");
	auto yuedu = (Button*)node->getChildByName("yuedu");

	//阅读
	auto duliyuedu = (Button*)node->getChildByName("duliyuedu");

	mianfeishidu->setTitleText(App::getString("MIANFEISHIDU"));
	yuedu->setTitleText(App::getString("STR_READ"));
	mianfeiyuedu->setTitleText(App::getString("MIANFEIYUEDU"));
	duliyuedu->setTitleText(App::getString("STR_READ"));

	mianfeishidu->setVisible(false);
	goumaihong->setVisible(false);
	if (m_relation->IsMemberVIP())
		goumainianka->setVisible(false);
	else
		goumainianka->setVisible(true);
	goumai->setVisible(true);

	if (m_relation->IsBookRent())
	{
		yuedu->setVisible(true);
		mianfeiyuedu->setVisible(false);
	}
	else
	{
		mianfeiyuedu->setVisible(true);
		yuedu->setVisible(false);
	}

	duliyuedu->setVisible(false);
}

void BookInfo::userBuy()
{	
	//免费试读 购买 购买年卡
	auto mianfeishidu = (Button*)node->getChildByName("mianfeishidu");
	auto goumaihong = (Button*)node->getChildByName("goumaihong");
	auto goumainianka = (Button*)node->getChildByName("goumainianka");

	//购买 免费阅读 阅读
	auto goumai = (Button*)node->getChildByName("goumai");
	auto mianfeiyuedu = (Button*)node->getChildByName("mianfeiyuedu");
	auto yuedu = (Button*)node->getChildByName("yuedu");

	//阅读
	auto duliyuedu = (Button*)node->getChildByName("duliyuedu");

	mianfeishidu->setTitleText(App::getString("MIANFEISHIDU"));
	yuedu->setTitleText(App::getString("STR_READ"));
	mianfeiyuedu->setTitleText(App::getString("MIANFEIYUEDU"));
	duliyuedu->setTitleText(App::getString("STR_READ"));

	mianfeishidu->setVisible(false);
	goumaihong->setVisible(false);
	goumainianka->setVisible(false);
	goumai->setVisible(false);
	mianfeiyuedu->setVisible(false);
	yuedu->setVisible(false);
	duliyuedu->setVisible(true);
}

void BookInfo::onClickLister()
{
	//如果书不存在 将书的状态至于未下载
	string ZipName = StringUtils::format("%d.zip", m_bookId);
	auto bookTag = YYXDownload::GetInstance()->getTaskTag(App::getReadZipDir(), ZipName);
	auto status = YYXDownload::GetInstance()->getTaskStatus(bookTag);
	if (!FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(m_bookId)))
	{
		if (status == YYXDownloadStatus::_over)
		{
			YYXDownload::GetInstance()->setTaskStatus(bookTag, _null);
		}
	}

	//免费试读 购买 购买年卡
	auto mianfeishidu = (Button*)node->getChildByName("mianfeishidu");
	auto goumaihong = (Button*)node->getChildByName("goumaihong");
	auto goumainianka = (Button*)node->getChildByName("goumainianka");

	//购买 免费阅读 阅读
	auto goumai = (Button*)node->getChildByName("goumai");
	auto mianfeiyuedu = (Button*)node->getChildByName("mianfeiyuedu");
	auto yuedu = (Button*)node->getChildByName("yuedu");

	//阅读
	auto duliyuedu = (Button*)node->getChildByName("duliyuedu");

	mianfeishidu->setTitleText(App::getString("MIANFEISHIDU"));
	yuedu->setTitleText(App::getString("STR_READ"));
	mianfeiyuedu->setTitleText(App::getString("MIANFEIYUEDU"));
	duliyuedu->setTitleText(App::getString("STR_READ"));

	addDownloadListener(mianfeishidu);
	addDownloadListener(duliyuedu);
	addDownloadListener(mianfeiyuedu);
	addDownloadListener(yuedu);

	mianfeishidu->addClickEventListener([=](Ref*) {
		YYXLayer::controlTouchTime(1, "mianfeishiduTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			string ZipName = StringUtils::format("%d.zip", m_bookId);
			auto bookTag = YYXDownload::GetInstance()->getTaskTag(App::getReadZipDir(), ZipName);
			//下载状态 0 = 下载 1 = 准备下载   2 = 暂停  3 = 下载结束 - 1 = 提前创建任务标示
			auto status = YYXDownload::GetInstance()->getTaskStatus(bookTag);
			if (status == YYXDownloadStatus::_pause)
			{
				sendDownloadBookAndReadBook();
				mianfeishidu->setTitleText("0%");
				return;
			}
			YYXVisitor::getInstance()->hintLogin([=]() {
				readbook_download_pause_tryreadbook(mianfeishidu);
			}, [=]() {
				gotoLogin();
			}, [=]() {
				readbook_download_pause_tryreadbook(mianfeishidu);
			}, string(App::getString("ZHIJIESHIDU")) + ">>>");
		});
	});

	goumaihong->addClickEventListener([=](Ref*) {
		YYXLayer::controlTouchTime(1, "goumaihongTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			YYXVisitor::getInstance()->hintLogin([=]() {
				buyBook();
			}, [=]() {
				gotoLogin();
			}, [=]() {
				buyBook();
			});
		});
	});
	goumainianka->addClickEventListener([=](Ref*) {
		YYXLayer::controlTouchTime(1, "goumainiankaTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			YYXVisitor::getInstance()->hintLogin([=]() {
				gotoLogin();
			}, [=]() {
				gotoLogin();
			}, [=]() {
				auto layer = XZLayer::OpenVIPCardService(2, [=]() {
					auto control = ControlScene::getInstance();
					control->replaceScene(ControlScene::getInstance()->getSceneInfo(BookInfoScene)->setData("bookId", Value(m_bookId)), ControlScene::getInstance()->getSceneInfo(LoginScene));
				});
				if (layer)
					addChild(layer, 9);
			}, " ", false);
		});
	});
	goumai->addClickEventListener([=](Ref*) {
		YYXLayer::controlTouchTime(1, "goumaiTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			YYXVisitor::getInstance()->hintLogin([=]() {
				buyBook();
			}, [=]() {
				gotoLogin();
			}, [=]() {
				buyBook();
			});
		});
	});
	mianfeiyuedu->addClickEventListener([=](Ref*) {
		YYXLayer::controlTouchTime(1, "mianfeiyueduTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			string ZipName = StringUtils::format("%d.zip", m_bookId);
			auto bookTag = YYXDownload::GetInstance()->getTaskTag(App::getReadZipDir(), ZipName);
			//下载状态 0 = 下载 1 = 准备下载   2 = 暂停  3 = 下载结束 - 1 = 提前创建任务标示
			auto status = YYXDownload::GetInstance()->getTaskStatus(bookTag);
			if (status == YYXDownloadStatus::_download || status == YYXDownloadStatus::_ready)
			{//暂停
				//Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(bookTag + "_BookProgressing");
				YYXDownload::GetInstance()->pause(bookTag);
				mianfeiyuedu->setTitleText(App::getString("ZANTING"));
				return;
			}
			if (status == YYXDownloadStatus::_pause)
			{//下载
				sendDownloadBookAndReadBook();
				mianfeiyuedu->setTitleText("0%");
				//addDownloadListener(mianfeiyuedu);
				return;
			}
			YYXRentBook::getInstance()->newRentBook(m_bookId, App::GetInstance()->getMemberId(), [=]() {
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
					readbook_download_pause_tryreadbook(mianfeiyuedu);
				});
			});
		});
	});
	yuedu->addClickEventListener([=](Ref*) {
		YYXLayer::controlTouchTime(1, "yueduTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			if (m_relation->IsMemberVIP())
			{
				YYXRentBook::getInstance()->backgroundThreadRentBook(m_bookId, App::GetInstance()->getMemberId(), [=]() {
					Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
						readbook_download_pause_tryreadbook(yuedu);
					});
				});
			}
			else
				readbook_download_pause_tryreadbook(yuedu);
		});
	});

	duliyuedu->addClickEventListener([=](Ref*) {
		YYXLayer::controlTouchTime(1, "duliyueduTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			if (m_relation->IsBookBuy())
			{
				readbook_download_pause_tryreadbook(duliyuedu);
				return;
			}
			if (m_relation->IsBookFree())
			{
				YYXVisitor::getInstance()->hintLogin([=]() {
					//游客
					YYXBuyBook::GetInstance()->newBuyFreeBook(m_bookId, App::GetInstance()->getMemberId(), [](int bookid) {
						YYXLayer::sendNotify(TAG_BUYSUCCESSCALLBACK);
					});
				}, [=]() {
					//去登陆
					gotoLogin();
				}, [=]() {
					//正常流程
					YYXBuyBook::GetInstance()->newBuyFreeBook(m_bookId, App::GetInstance()->getMemberId(), [](int bookid) {
						YYXLayer::sendNotify(TAG_BUYSUCCESSCALLBACK);
					});
				}, App::getString("ZHIJIEYUEDU"));
			}
			else
			{
				readbook_download_pause_tryreadbook(duliyuedu);
			}
		});
	});
}

void BookInfo::readBook()
{
	ControlScene::getInstance()->pushCurrentScene(ControlScene::getInstance()->getSceneInfo(BookInfoScene)->setData("bookId", Value(m_bookId)));
	if (m_relation->IsBookBuy() || m_relation->IsMemberVIP())
	{
		//完整
		//打开书本
		SetBook::getInstance()->readBook(m_bookId, false);
	}
	else
	{
		//试读
		//打开书本
		SetBook::getInstance()->readBook(m_bookId, true);
	}
}

void BookInfo::sendDownloadBookAndReadBook()
{
	string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", m_bookId);
	string bookPlayUrl = YYXStruct::getMapString(App::GetInstance()->myData, BookPlayUrlKey, "");
	string zippath = App::getBookReadZipPath(m_bookId);
	string taskTag = YYXDownload::getTag(zippath);
	string ZipName = StringUtils::format("%d.zip", m_bookId);
	DownLoadBook(m_bookId, bookPlayUrl, ZipName);
}

void BookInfo::buyBook()
{
	YYXBuyBook::GetInstance()->newBuyBook(m_bookId, App::GetInstance()->getMemberId(), [=](int  bookid) {callBackBuyBook(); }, [=]() {
		gotoLogin();
	});
}

//购书成功的回调
void BookInfo::callBackBuyBook()
{
	App::log("BookInfo::callBackBuyBook");
	string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", m_bookId);
	string bookPlayUrl = YYXStruct::getMapString(App::GetInstance()->myData, BookPlayUrlKey, "");
	App::GetInstance()->myBookURLMap[m_bookId] = bookPlayUrl;
	App::GetInstance()->myBuyBook[m_bookId] = (int)YYXLayer::getCurrentTime4Second();
	m_relation->IsBookBuy(true);
	ButtonControl();
	if (m_relation->IsBookFree())
	{
		if (m_relation->IsBookExit())
			readBook();
		else
			sendDownloadBookAndReadBook();
	}
	//阅读
	auto duliyuedu = (Button*)node->getChildByName("duliyuedu");
	string ZipName = StringUtils::format("%d.zip", m_bookId);
	auto bookTag = YYXDownload::GetInstance()->getTaskTag(App::getReadZipDir(), ZipName);
	//下载状态 0 = 下载 1 = 准备下载   2 = 暂停  3 = 下载结束 - 1 = 提前创建任务标示
	auto status = YYXDownload::GetInstance()->getTaskStatus(bookTag);
	switch (status)
	{
	case YYXDownloadStatus::_download:
		duliyuedu->setTitleText("0%");
		break;
	case YYXDownloadStatus::_ready:
		duliyuedu->setTitleText("0%");
		break;
	case YYXDownloadStatus::_pause:
		duliyuedu->setTitleText(App::getString("ZANTING"));
		break;
	case YYXDownloadStatus::_over:
		break;
	case YYXDownloadStatus::_null:
		break;
	}

	App::log("BookInfo::callBackBuyBook --END");
}

//显示购买过 还是租过
void BookInfo::hintIsBuyIsRent()
{
	auto org = Director::getInstance()->getVisibleOrigin();
	auto win = Director::getInstance()->getWinSize();

	auto ygou = (ImageView*)node->getChildByName("Image_4");
	ygou->setAnchorPoint(Vec2(1, 1));
	if (ygou->getPosition().x > org.x + win.width)
	{
		ygou->setPosition(Vec2(Director::getInstance()->getWinSize().width / 2 + 1094 / 2, ygou->getPosition().y));
	}
	ygou->setVisible(true);
	if (m_relation->IsBookBuy())
		ygou->loadTexture("BookInfo/res/pager_yigou_736h.png", TextureResType::PLIST);
	else
	{
		if (m_relation->IsMemberVIP() && m_relation->IsBookRent())			//显示VIP已购
			ygou->loadTexture("BookInfo/res/yjgou.png", TextureResType::PLIST);
		else
			ygou->setVisible(false);
	}
}

void BookInfo::gotoLogin()
{
	auto control = ControlScene::getInstance();
	control->replaceScene(ControlScene::getInstance()->getSceneInfo(BookInfoScene)->setData("bookId", Value(m_bookId)), ControlScene::getInstance()->getSceneInfo(LoginScene));
}

void BookInfo::addDownloadListener(Button* button)
{
	string BookNameKey = StringUtils::format("bookName+bookID=%d", m_bookId);
	string taskBookName = YYXStruct::getMapString(App::GetInstance()->myData, BookNameKey, "");
	string ZipName = StringUtils::format("%d.zip", m_bookId);
	auto bookTag = YYXDownload::GetInstance()->getTaskTag(App::getReadZipDir() , ZipName);
	App::log("------------------------------------------>>>>>>>bookTag = " + bookTag);
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookTag, m_bookId, "<<" + taskBookName + ">>");
	//_eventDispatcher->removeCustomEventListeners(bookTag + "_BookProgressing");
	//进度
	auto listenerButton = EventListenerCustom::create(bookTag + "_BookProgressing", [=](EventCustom* e) {
		auto status = YYXDownload::GetInstance()->getTaskStatus(bookTag);
		if (status == _pause)
			return;
		auto progressing = (long)e->getUserData();
		if (progressing < 100 && progressing > 0)
			button->setTitleText(StringUtils::format("%d%%", progressing));
		else
		{
			if (progressing == 999 || progressing == 100)
				ButtonControl();
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerButton, button);
	m_listenerName.push_back(bookTag + "_BookProgressing");
}

void BookInfo::showXingji()
{
	auto wuxing = (ImageView*)node->getChildByName("wuxing");
	string path = "";
	switch (m_xingji)
	{
	case 0:
		path = "other/BookHome_wuxinghaoping_736h.png";
		break;
	case 1:
		path = "other/Backcover_star1_736h.png";
		break;
	case 2:
		path = "other/Backcover_star2_736h.png";
		break;
	case 3:
		path = "other/Backcover_star3_736h.png";
		break;
	case 4:
		path = "other/Backcover_star4_736h.png";
		break;
	default:
		path = "other/Backcover_star5_736h.png";
		break;
	}
	if (FileUtils::getInstance()->isFileExist(path))
		wuxing->loadTexture(path);
}

void BookInfo::showCommentCount(int count)
{
	if (count < 0 || count > 999)
		count = 0;
	Text* pinglunshuliang = nullptr;
	pinglunshuliang = (Text*)node->getChildByName("pinglunshuliang");
	if (pinglunshuliang == nullptr)
		return;
	if (count==0)
	{
		pinglunshuliang->setVisible(false);
	}
	else
	{
		pinglunshuliang->setText(StringUtils::format("(%d)", count));
		pinglunshuliang->setVisible(true);
	}	
}

void BookInfo::readbook_download_pause_tryreadbook(Button* bt)
{
	if (m_relation->IsBookExit())
		readBook();
	else
	{
		string ZipName = StringUtils::format("%d.zip", m_bookId);
		auto bookTag = YYXDownload::GetInstance()->getTaskTag (App::getReadZipDir() , ZipName);
		//下载状态 0 = 下载 1 = 准备下载   2 = 暂停  3 = 下载结束 - 1 = 提前创建任务标示
		auto status = YYXDownload::GetInstance()->getTaskStatus(bookTag);
		if (status == YYXDownloadStatus::_download ||status == YYXDownloadStatus::_ready)
		{
			//暂停
			//Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(bookTag + "_BookProgressing");
			YYXDownload::GetInstance()->pause(bookTag);
			bt->setTitleText(App::getString("ZANTING"));
		}
		if(status == YYXDownloadStatus::_pause || status == YYXDownloadStatus::_null)
		{//下载
			sendDownloadBookAndReadBook();
			bt->setTitleText("0%");
			//addDownloadListener(bt);
		}
	}
}