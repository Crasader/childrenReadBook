﻿#include "BookInfoScene.h"
#include "YYXLayer.h"
#include "SendCommentLayer.h"
#include "IndexScene.h"
#include "FileNames.h"
#include "CrossPlatform.h"
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
#include "AppHttp.h"
#include "BookCache.h"
#include "HttpWaiting.h"
#include "BuyVip.h"
#include "YYXTime.h"
#include "LoginControl.h"
#include "YYXTableViewBookInfoComment.h"

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

	Director::getInstance()->getEventDispatcher()->setEnabled(true);
	YYXVisitor::getInstance()->bookInfoSceneInit();
	//--------------场景初始化--------------------------------------------
	m_bookId = sceneInfo->getData("bookId", Value(0)).asInt();
	m_bookData = BookCache::getInstance()->getBook(m_bookId);
	//控制层
	if (App::m_debug == 0)
		Toast::create(Value(m_bookId).asString().c_str());
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
	//m_originalPrice = (Text*)node->getChildByName(BOOKINFO_FIND_TEXT_ORIGINALPRICE);
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
	bool _downloadCover = true;
	string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
	std::string fileName = StringUtils::format("%d", m_bookId) + ".png";
	string bookCoverPath = dir + "/" + fileName;
	if (FileUtils::getInstance()->isFileExist(bookCoverPath))
	{
		if (App::initImage(bookCoverPath))
		{
			cover->loadTexture(bookCoverPath);
			_downloadCover = false;
		}
		else
		{
			FileUtils::getInstance()->removeFile(bookCoverPath);
		}
	}
	if (_downloadCover)
	{
		cover->loadTexture("other/Book_cover.png");
		downloadCover();
	}
	auto listener = EventListenerCustom::create("haveCover_"+Value(m_bookId).asString(), [=](EventCustom* e) {
		string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
		std::string fileName = StringUtils::format("%d", m_bookId) + ".png";
		string bookCoverPath = dir + "/" + fileName;
		if (FileUtils::getInstance()->isFileExist(bookCoverPath))
		{
			cover->loadTexture(bookCoverPath);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, cover);
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
				m_listview->setVisible(false);
				m_listview->setScrollBarEnabled(false);
				//if (tbview == nullptr)
				//{
				//	tbview = YYXTableViewBookInfoComment::create();
				//	tbview->setTag(159);
				//	tbview->setAnchorPoint(Vec2(0, 1));
				//	tbview->setPosition(m_listview->getPosition());
				//	cocos2d::log("Atbview = %f,%f",tbview->getPosition().x, tbview->getPosition().y);
				//	pinglun->addChild(tbview);
				//}
				auto tbview = (YYXTableViewBookInfoComment*)m_View_Comment->getChildByTag(159);

				if (tbview) {
					tbview->setVisible(true);
					tbview->loadData(m_bookId);
				}
				//auto ve = m_listview->getItems();
				//if (ve.size() > 0)
				//	break;
				//YYXLayer::showCommentListView(m_listview, m_bookId);
				//string commentCountKey = StringUtils::format("comment_bookid=%d", m_bookId);//book评论的数量
				//auto count = YYXStruct::getMapInt64(App::GetInstance()->myData, commentCountKey, 0);
				//showCommentCount(count);
				////刷新列表
				//Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("showCommentListView");
				//auto listener2 =EventListenerCustom::create("showCommentListView", [=](EventCustom* e) {
				//	if (!isComment)
				//		return;
				//	int memberid = (int)e->getUserData();
				//	App::log(" --------------     run showCommentListView()   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^memberid = ", memberid);
				//	YYXSound::getInstance()->stopAll();
				//	YYXSound::getInstance()->resumeBackGroundMusic();
				//	YYXLayer::showCommentListView(m_listview, m_bookId);
				//	string commentCountKey = StringUtils::format("comment_bookid=%d", m_bookId);//book评论的数量
				//	auto count = YYXStruct::getMapInt64(App::GetInstance()->myData, commentCountKey, 0);
				//	showCommentCount(count);
				//});
				//Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener2, m_listview);
				//m_listview->jumpToTop();
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
	m_relation->setControl(m_bookId, App::GetInstance()->getMemberId(), m_bookStatus);
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

	auto listener2 = EventListenerCustom::create(TAG_READBOOK, [=](EventCustom* e) {
		int book = (int)e->getUserData();
		if (book == m_bookId)
			readBook();
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, node);

	auto listener3 = EventListenerCustom::create(TAG_BUTTONCONTROL, [=](EventCustom* e) {
		ButtonControl();
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener3, node);

	//没有WiFi连接的通知
	auto listenNoWifi = EventListenerCustom::create("Down_No_Wifi", [=](EventCustom* event) {
		Toast::create(App::getString("NO_WIFI"));
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenNoWifi, node);

	//------------------------------接受通知:刷新支付状态--------------------------------------------------------------------------
	auto listener4 = EventListenerCustom::create(TAG_BUYSUCCESSCALLBACK, [=](EventCustom* event) {
		callBackBuyBook();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener4, node);
	//------------------------------接受通知:刷新支付状态--------------------------------------------------------------------------

	// ---------------------------- - 接受通知:充值成功后,自动继续购书--------------------------------------------------------------------------
	auto listener5 = EventListenerCustom::create(TAG_PAYSUCCESSGOTOBUG, [=](EventCustom* event) {
		buyBook();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener5, node);
	//-----------------------------接受通知:充值成功后,自动继续购书--END --------------------------------------------------------------------------

	//刷新红包抵扣提示
	auto listener6 = EventListenerCustom::create(TAG_DIKOUHONGBAO, [=](EventCustom* e) {
		hintHongbao();
	}); 
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener6, node);

	//评论成功
	auto listener7 = EventListenerCustom::create(TAG_SENDCOMMENTSUCCESS, [=](EventCustom* e) {
		AppHttp::getInstance()->httpComments(m_bookId, []() {
			YYXLayer::sendNotify("showCommentListView", "", -1);
		});
		YYXLayer::controlTouchTime(3, "shareBookTime", [=]() {
			CrossPlatform::getInstance()->ShareWeb(App::getMemberId(), m_bookId);
		});
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener7, node);

	//------------------------------接受通知:刷新界面--------------------------------------------------------------------------
	auto listener8 = EventListenerCustom::create(TAG_BOOKINFOREFERUI, [=](EventCustom* e) {
		m_bookData = BookCache::getInstance()->getBook(m_bookId);
		if (m_bookData) {
			auto content = m_bookData->getContent();
			if (content.empty())
				m_bookData = BookCache::getInstance()->getBookFromLocation(m_bookId);
		}
		refreshUI();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener8, node);
	//------------------------------接受通知:刷新界面--------------------------------------------------------------------------
	auto listener9 = EventListenerCustom::create(TAG_BOOKINFO_CONTROL, [=](EventCustom* e) {
		long code = (long)e->getUserData();
		m_bookStatus = code;
		if (m_relation)
			m_relation->setControl(m_bookId, App::GetInstance()->getMemberId(), code);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener9, node);
}

void BookInfo::initHttp()
{
	//用户和书籍关系
	AppHttp::getInstance()->httpThe_relationship_between_human_and_books(m_bookId, [=](int code) {		
		YYXLayer::sendNotify(TAG_BOOKINFO_CONTROL, "", code);
		YYXLayer::sendNotify(TAG_BUTTONCONTROL);
		YYXLayer::sendNotify(TAG_DIKOUHONGBAO);
		YYXLayer::sendNotify(TAG_REMOVEWAITING);
	});
	auto layer = HttpWaiting::getInstance()->newWaitingLayer();
	if (layer)
		addChild(layer);
	//书籍详情
	AppHttp::getInstance()->httpBookInfo(m_bookId, [](string name) {
		YYXLayer::sendNotify(TAG_BOOKINFOREFERUI);
		YYXLayer::sendNotify(TAG_BUTTONCONTROL);
	});
	//获取评论
	AppHttp::getInstance()->httpComments(m_bookId, []() {
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
	if (m_View_Comment)
	{
		m_View_Comment->removeAllChildrenWithCleanup(true);
		m_View_Comment->removeFromParentAndCleanup(true);
	}
	node->removeAllChildrenWithCleanup(true);
	node->removeFromParentAndCleanup(true);
	HttpWaiting::getInstance()->remove();
	App::log("BookInfo::cleanup-end");
}

//刷新界面
void BookInfo::refreshUI()
{
	App::log("BookInfo::refreshUI");
	//星级
	if (m_bookData)
		m_xingji = m_bookData->getScore();
	else
		m_xingji = 4;
	showXingji();
	//现价
	if (m_bookData)
		m_Selling100 = m_bookData->getBookPrice();
	else
		m_Selling100 = -999;
	//价格
	showPrice();
	//书名
	if (m_bookData)
		m_BookName = m_bookData->getName();
	else
		m_BookName = "";
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
	App::log("BookInfo::startSchedu---END");
}

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
	//m_originalPrice->setText(str_or);
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
	string content = "";	
	if (m_bookData)
		content = m_bookData->getContent();
	auto m_text_bookIntroduction = Label::create(content, "FZHLJW.TTF", 48, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	m_text_bookIntroduction->setMaxLineWidth(640);
	m_text_bookIntroduction->setTextColor(Color4B::BLACK);
	m_text_bookIntroduction->setAnchorPoint(Vec2(0, 0));
	if (content.length() <= 40 * 3)
	{
		m_text_bookIntroduction->setAdditionalKerning(25);//字体间距
		m_text_bookIntroduction->setLineHeight(86);//行间距
	}
	else if (content.length() <= 55 * 3)
	{
		m_text_bookIntroduction->setAdditionalKerning(20);//字体间距
		m_text_bookIntroduction->setLineHeight(80);//行间距
	}
	else if (content.length() <= 70*3)
	{
		m_text_bookIntroduction->setAdditionalKerning(15);//字体间距
		m_text_bookIntroduction->setLineHeight(72);//行间距
	}
	else if (content.length() <= 85 * 3)
	{
		m_text_bookIntroduction->setAdditionalKerning(12);//字体间距
		m_text_bookIntroduction->setLineHeight(66);//行间距
	}
	else if (content.length() <= 102 * 3)
	{
		m_text_bookIntroduction->setAdditionalKerning(8);//字体间距
		m_text_bookIntroduction->setLineHeight(60);//行间距
	}
	else
	{
		m_text_bookIntroduction->setAdditionalKerning(4);//字体间距
		m_text_bookIntroduction->setLineHeight(56);//行间距
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
	//string BookPressKey = StringUtils::format("bookPress+bookID=%d", m_bookId);
	//auto press = YYXStruct::getMapString(App::GetInstance()->myData, BookPressKey, "");
	string press = "";
	if(m_bookData)
		press = m_bookData->getBookPress();
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
	if (m_bookData)
		str = App::getString("STR_PAGENUMBER") + Value(m_bookData->getBookPage()).asString();
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
	string author = "";
	if (m_bookData)
		author = m_bookData->getBookAuthor();
	if (!author.empty())
		author = App::getString("STR_BOOKAUTHOE") + author;
	auto m_text_bookAuthor = Label::create(author, "FZHLJW.TTF",34, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	m_text_bookAuthor->setTextColor(Color4B::GRAY);
	m_text_bookAuthor->setAnchorPoint(Vec2(0, 0));
	m_text_bookAuthor->setAdditionalKerning(8);			//字体间距
	m_text_bookAuthor->setScale(0.5);
	m_text_bookAuthor->setMaxLineWidth(320*2);
	layout4->setContentSize(Size(320, m_text_bookAuthor->getContentSize().height/2));
	layout4->addChild(m_text_bookAuthor);
	App::log("BookInfo::creatItem_bookAuthor--END "+ author);
	return layout4;
}


//初始化评论界面
Layer* BookInfo::initCommonView()
{
	App::log("BookInfo::initCommonView");
	//图层
	Data data;
	Layer* pinglun = nullptr;
	if (App::GetInstance()->getData(BOOKINFO_PINGLUN_CSB,data))
		pinglun = (Layer*)CSLoader::createNode(data);
	else
		pinglun = (Layer*)CSLoader::createNode(BOOKINFO_PINGLUN_CSB);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	if (pinglun == nullptr)
		return nullptr;
	//评论按钮
	auto commentMe = ( Button*)pinglun->getChildByName(BOOKINFO_BUTTON_PINGLUN);
	commentMe->setTouchEnabled(true);
	commentMe->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "BookInfoScenecommentMe", [=]() {
			YYXSound::getInstance()->playButtonSound();
			if (m_relation->IsBookBuy() || m_relation->IsBorrow() || (m_relation->IsBookVIP() && m_relation->IsBookRent()))
			{
				auto pScene = Director::getInstance()->getRunningScene();
				pScene->addChild(SendComment::create(m_bookId));
			}
			else
			{
				Toast::create(App::getString("COMMENT_TIP_BUY"));
			}
		});
	});
	m_listview = (ListView*)pinglun->getChildByName(BOOKINFO_FIND_LISTVIEW_BYNAME_COMMENT);
	//m_listview->setScrollBarEnabled(false);
	//m_listview->setSize(Size(320, 270));
	m_listview->setVisible(false);
	m_listview->setScrollBarEnabled(false);
	auto tbview = YYXTableViewBookInfoComment::create();
	tbview->setAnchorPoint(Vec2(0, 0));
	tbview->setPosition(m_listview->getPosition().x , m_listview->getPosition().y-270);
	tbview->setTag(159);
	pinglun->addChild(tbview);

	cocos2d::log("Btbview = %f,%f", tbview->getPosition().x, tbview->getPosition().y);
	//刷新列表
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("showCommentListView");
	auto listener2 = EventListenerCustom::create("showCommentListView", [=](EventCustom* e) {
		long memberid = (long)e->getUserData();
		App::log(" --------------     run showCommentListView()   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^memberid = ", memberid);
		YYXSound::getInstance()->stopAll();
		YYXTableViewBookInfoComment::stopAllAnimation();
		auto tbview = (YYXTableViewBookInfoComment*)m_View_Comment->getChildByTag(159);
		//if (tbview == nullptr)
		//{
		//	tbview = YYXTableViewBookInfoComment::create();
		//	tbview->setTag(159);
		//	tbview->setAnchorPoint(Vec2(0, 1));
		//	tbview->setPosition(m_listview->getPosition());
		//	cocos2d::log("Btbview = %f,%f", tbview->getPosition().x, tbview->getPosition().y);
		//	node->addChild(tbview);
		//}
		if (tbview && tbview->isVisible())
			tbview->loadData(m_bookId);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener2, tbview);
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

//红包抵扣提示
void BookInfo::hintHongbao()
{
	auto hongbaodikou = (ImageView*)node->getChildByName("Image_21");
	auto Tex = (Text*)node->getChildByName("dikou");
	if (hongbaodikou == nullptr || Tex == nullptr)
		return;
	hongbaodikou->setVisible(false);
	Tex->setVisible(false);
	if (m_relation->BookId() < 0 || m_relation->IsBorrow())
		return;
	if (!m_relation->IsBookBuy())//未购买
	{
		float showred = getOptimalRedPacket() / 100.0;
		if (showred > 0) {//并且有红包
			hongbaodikou->setVisible(true);
			Tex->setVisible(true);
			if (Tex)
				Tex->setText(StringUtils::format("%s%.01f%s", App::getString("HONGBAODIKOU"), showred, App::getString("YUAN")));
		}
	}
}

//网络下载完整书籍
string BookInfo::DownLoadBook(int bookId, string bookPlayUrl, string ZipName)
{
	auto data = DownLoadFileData::create();
	string bookTag = data->getTag();
	data->setUrl(bookPlayUrl);
	data->setDir(App::getReadZipDir());
	data->setFileName(ZipName);
	data->setUseData(Value(bookId));
	data->setBeginFunc([](DownLoadFileData* DData) {
		int bookid = DData->getUseData().asInt();
		App::downloadFailDeleteFiles(App::getReadZipDir() + "/" + DData->getPath(), App::getBookReadPath(bookid));
	});
	data->setPausePint(0);
	data->setDownloadingFunc([](DownLoadFileData* DData) {
		int progressing = DData->getPausePint();
		if (progressing <= 99)
			YYXLayer::sendNotify(DData->getTag() + "_BookProgressing", "", progressing);
	});
	data->setEndFunc([](DownLoadFileData* DData) {
		int bookid = DData->getUseData().asInt();
		if (DData->getStatus() == _over)
		{
			AppHttp::getInstance()->httpUploadDownloadRecord(bookid);
			//下载成功,解压									
			YYXDownload::uncompress( App::getReadZipDir(), StringUtils::format("%d", bookid), App::getReadDir(), [](string zipPath) {
				//路径就是tag
				auto ddata = YYXDownload::getInstance()->getTask(zipPath);
				YYXLayer::sendNotify(zipPath + "_BookProgressing", "", 100);
				DownloadBook::getInstance()->addBook(ddata->getUseData().asInt(), YYXTime::getInstance()->getNowTime4S());
				YYXLayer::sendNotify(TAG_READBOOK, "", ddata->getUseData().asInt());
			}, [](string zipPath) {
				//解压失败
				string bookNameError = App::getString("JIEYACUOWUCHANGXINXIAZAI");
				Toast::create(bookNameError.c_str());
				YYXLayer::sendNotify(zipPath + "_BookProgressing", "", 999);
				auto ddata = YYXDownload::getInstance()->getTask(zipPath);
				App::downloadFailDeleteFiles(zipPath, App::getBookReadPath(ddata->getUseData().asInt()));
			});
		}
		else
		{
			//下载错误
			if (DData->getStatus() != _pause)
			{
				YYXLayer::sendNotify(DData->getTag() + "_BookProgressing", "", 999);
				string bookNameError = App::getString("XIAZAICUOWUCHANGXINXIAZAI");
				Toast::create(bookNameError.c_str());
				App::downloadFailDeleteFiles(App::getReadZipDir() + "/" + DData->getFileName(), App::getBookReadPath(DData->getUseData().asInt()));
			}
		}
	});
	YYXDownload::getInstance()->newDownloadFile(data);	
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
	if (m_relation->IsBookBuy()) {
		//已购
		showBuyButtons(2);
		return;
	}
	if (m_relation->IsBorrow()) {//已免费借到了
		showBuyButtons(4);
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
	case 4://借阅
		borrowBook();
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

void BookInfo::borrowBook()
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
	yuedu->setVisible(true);
	mianfeiyuedu->setVisible(false);
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
				auto layer = BuyVip::getInstance()->show();
				if (layer)
					addChild(layer);
				BuyVip::getInstance()->setCallback([=](string json) {
					AppHttp::getInstance()->httpCheckVIP();
					AppHttp::getInstance()->httpThe_relationship_between_human_and_books(m_bookId, [=](int code) {
						YYXLayer::sendNotify(TAG_BOOKINFO_CONTROL, "", code);
						YYXLayer::sendNotify(TAG_BUTTONCONTROL);
						YYXLayer::sendNotify(TAG_DIKOUHONGBAO);
						YYXLayer::sendNotify(TAG_REMOVEWAITING);
					});
					auto layer = HttpWaiting::getInstance()->newWaitingLayer();
					if (layer)
						Director::getInstance()->getRunningScene()->addChild(layer);
				});
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
			auto bookData = YYXDownload::getInstance()->getTask(App::getReadZipDir()+"/"+ ZipName);
			if (bookData)
			{
				readbook_download_pause_tryreadbook(mianfeiyuedu);
			}
			else
			{
				YYXRentBook::getInstance()->newRentBook(m_bookId, App::GetInstance()->getMemberId(), [=]() {
					Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
						readbook_download_pause_tryreadbook(mianfeiyuedu);
					});
				});
			}			
		});
	});
	yuedu->addClickEventListener([=](Ref*) {
		YYXLayer::controlTouchTime(1, "yueduTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			readbook_download_pause_tryreadbook(yuedu);
		});
	});

	duliyuedu->addClickEventListener([=](Ref*) {
		YYXLayer::controlTouchTime(1, "duliyueduTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			if (m_relation->IsBookBuy() || m_relation->IsBorrow())
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
	if (m_relation->IsBookBuy() || m_relation->IsMemberVIP() || m_relation->IsBorrow())
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
	string bookPlayUrl = m_bookData->getBookPlayUrl();
	string zippath = App::getBookReadZipPath(m_bookId);
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
	auto bookDat = YYXDownload::getInstance()->getTask(App::getReadZipDir()+"/"+ ZipName);
	if (bookDat)
	{
		auto status = bookDat->getStatus();
		switch (status)
		{
		case _download:
			duliyuedu->setTitleText("0%");
			break;
		case _ready:
			duliyuedu->setTitleText("0%");
			break;
		case _pause:
			duliyuedu->setTitleText(App::getString("ZANTING"));
			break;
		}
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
	if (ygou->getPosition().x > org.x + win.width){
		ygou->setPosition(Vec2(Director::getInstance()->getWinSize().width / 2 + 1094 / 2, ygou->getPosition().y));
	}
	ygou->setVisible(true);
	if (m_relation->IsBookBuy())
		ygou->loadTexture("other/pager_yigou_736h.png");
	else{
		if (m_relation->IsMemberVIP() && m_relation->IsBookRent())			//显示VIP已购
			ygou->loadTexture("BookInfo/res/yjgou.png");
		else
			ygou->setVisible(false);
	}
}

void BookInfo::gotoLogin()
{
	//auto control = ControlScene::getInstance();
	//control->replaceScene(ControlScene::getInstance()->getSceneInfo(BookInfoScene)->setData("bookId", Value(m_bookId)), ControlScene::getInstance()->getSceneInfo(LoginScene));
	ControlScene::getInstance()->setDangqianScene(BookInfoScene);
	LoginControl::getInstance()->Login([](string json) {
		LoginControl::getInstance()->LoginCallback(json);
	}, nullptr);
}

void BookInfo::addDownloadListener(Button* button)
{
	string ZipName = StringUtils::format("%d.zip", m_bookId);
	string zippath = App::getReadZipDir() + "/" + ZipName;
	string bookTag = zippath;
	//进度
	auto listenerButton = EventListenerCustom::create(bookTag + "_BookProgressing", [=](EventCustom* e) {
		auto data = YYXDownload::getInstance()->getTask(bookTag);
		if (data){
			if (data->getStatus() == _pause)
				return;
		}
		auto progressing = (long)e->getUserData();
		if (progressing < 100 && progressing > 0)
			button->setTitleText(StringUtils::format("%d%%", progressing));
		else{
			if (progressing == 999 || progressing == 100)
				ButtonControl();
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerButton, button);
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
		auto bookTag = App::getReadZipDir() + "/" + ZipName;
		auto data = YYXDownload::getInstance()->getTask(bookTag);
		if (data)//建立了下载任务
		{
			auto status = data->getStatus();
			switch (status)
			{
			case _download:
				bt->setTitleText(App::getString("ZANTING"));
				YYXDownload::getInstance()->pause(bookTag);
				break;
			case _ready:
				bt->setTitleText(App::getString("ZANTING"));
				YYXDownload::getInstance()->pause(bookTag);
				break;
			case _pause:
				sendDownloadBookAndReadBook();
				bt->setTitleText("0%");
				break;
			case _over:
				sendDownloadBookAndReadBook();
				bt->setTitleText("0%");
				break;
			case _null:
				sendDownloadBookAndReadBook();
				bt->setTitleText("0%");
				break;
			}
		}
		else
		{
			sendDownloadBookAndReadBook();
			bt->setTitleText("0%");
		}
	}
}

void BookInfo::downloadCover()
{
	if (m_bookData)
	{
		auto url = m_bookData->getCoverURL();
		string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
		std::string fileName = StringUtils::format("%d", m_bookId) + ".png";
		YYXDownloadImages::GetInstance()->newDownloadImage(url, dir, fileName, high, 0, [=](string downpath) {
			YYXLayer::sendNotify("haveCover_" + Value(m_bookId).asString());
		});
	}
}