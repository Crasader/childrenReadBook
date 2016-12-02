#include "BookInfoScene.h"
#include "YYXLayer.h"
#include "SendCommentLayer.h"
#include "IndexScene.h"
#include "RechargeLayer.h"
#include "FileNames.h"
#include "NetIntface.h"
#include <math.h>
#include "YYXDownload.h"
#include <stdio.h>
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"


using namespace experimental;
USING_NS_CC;
using namespace cocostudio::timeline;
using namespace CocosDenshion;
USING_NS_CC_EXT;
using namespace std;

BookInfo::BookInfo() {
	App::log("BookInfo::BookInfo");
	isDownloading = false;
	isDownloaded = false;
	isUnzip = false;
	isBuy = false;
	isDamage = false;
	isViewDownloading = false;
	isViewDownloaded = false;
	isViewUnzip = false;
	isViewDamage = false;
	m_orderId = -999;
	m_Selling100 = 2.99;
	App::log("BookInfo::BookInfo---END");
}

BookInfo::~BookInfo()
{
	App::log("BookInfo::~BookInfo");
	App::log("BookInfo::~BookInfo---END");
}

Scene* BookInfo::createScene(int bookId)
{
	App::log("BookInfo::createScene");
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = BookInfo::create(bookId);

    // add layer as a child to scene
    scene->addChild(layer);
	layer->setTag(9);
    // return the scene
	App::log("BookInfo::createScene--END");
    return scene;
}

BookInfo* BookInfo::create(int bookId)
{ 
	App::log("BookInfo::create");
	BookInfo *pRet = new(std::nothrow) BookInfo();
	if (pRet && pRet->init(bookId))
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

// on "init" you need to initialize your instance
bool BookInfo::init(int bookId)
{
	App::log(" BookInfo::init");
	//////////////////////////////
// 1. super init first
	if (!Layer::init())
	{
		return false;
	}	
	App::m_RunningScene = MySceneName::BookInfoScene;
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	Director::getInstance()->getEventDispatcher()->setEnabled(true);
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
	// 音乐
	if (YYXLayer::getBoolFromXML(MUSIC_KEY))
	{
		if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
		{
			/*SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
			SimpleAudioEngine::getInstance()->playBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY, true);*/
			SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
			SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.3);
		}
		else
			SimpleAudioEngine::getInstance()->playBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY, true);
	}
	else
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	m_bookId = bookId;
	//主场景
	Layer* node;
	Data data;
	if (App::GetInstance()->getData(BOOKINFO_BOOKINFO_CSB,data))
		node = (Layer*)CSLoader::createNode(data);
	else
		node = (Layer*)CSLoader::createNode(BOOKINFO_BOOKINFO_CSB);
	node->setAnchorPoint(Vec2(0.5f, 0.5f));
	node->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(node);
	//封面
	auto cover = ( ImageView*)node->getChildByName(BOOKINFO_FIND_IMAGEVIEW_COVER);
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
	App::log(" BookInfo::init  22");
	//----------------------------------------------------------试读--------------------------------------------------------------
	//试读 下载中 按钮
	m_tryReadButton = ( Button*)node->getChildByName(BOOKINFO_FIND_BUTTON_YELLOW);
	m_tryReadButton->setTouchEnabled(true);
	m_tryReadButton->setTitleFontSize(36);
	m_tryReadButton->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "BookInfoScenem_ReadButton", [=]() {
			//试读url
			string bookViewUrlKey = StringUtils::format("bookViewUrl+bookID=%d", bookId);
			string bookViewUrl = YYXStruct::getMapString(App::GetInstance()->myData, bookViewUrlKey, "");
			string beginKey = "bookInfoSceneDownLoadTryReadBookBegin";
			string progressKey = "bookInfoSceneDownLoadTryReadBookProgress";
			string endKey = "bookInfoSceneDownLoadTryReadBookEnd";
			string tryReadZipName = StringUtils::format("%d.zip", m_bookId);
			if (bookViewUrl == "")
			{
				Toast::create(App::getString("WEIZHAODAOXIAZAIDIZHI"));
				return;
			}
			if (!FileUtils::getInstance()->isFileExist(App::getBookTryRead4Json_txtPath(m_bookId)) && App::getNetSetAndHintSet())
			{
				string taskTag = YYXDownload::GetInstance()->download(bookViewUrl, App::getTryReadZipDir(), tryReadZipName,
					beginKey, [](YYXStruct data) {
					//auto taskTag1 = YYXStruct::getStringData(data, "");				
				}, progressKey, [](YYXStruct data) {
					auto taskTag1 = YYXStruct::getStringData(data, "");
					if (data.intData <= 0)
						return;
					if (data.intData <= 99)
					{
						YYXLayer::controlTouchTime(0.3, "tryReadBookProgressingTime", [=]() {
							YYXLayer::sendNotify(taskTag1 + "_tryReadBookProgressing", "", data.intData);
						});
					}
					else
						YYXLayer::sendNotify(taskTag1 + "_tryReadBookProgressing", "", 99);
				}, endKey, [=](YYXStruct data) {
					auto taskTag2 = YYXStruct::getStringData(data, "");
					string bookName = YYXStruct::getMapString(App::GetInstance()->myData, taskTag2, "");
					int bookid = YYXStruct::getMapInt64(App::GetInstance()->myData, taskTag2, -999);
					int code = YYXStruct::getInt64Data(data, -2);
					if (code == 0)
					{
						//下载成功,解压
						std::thread(&YYXDownload::uncompress, App::getTryReadZipDir(), StringUtils::format("%d", bookid), App::getTryReadDir(), [=](string zipPath) {
							YYXLayer::sendNotify(taskTag2 + "_tryReadBookProgressing", "", 100);
						}, [=](string zipPath) {
							//解压失败
							string bookNameerror = bookName + App::getString("JIEYACUOWUCHANGXINXIAZAI") + StringUtils::format("%d", bookid);
							Toast::create(bookNameerror.c_str());
							YYXLayer::sendNotify(taskTag2 + "_tryReadBookProgressing", "", 100);
							App::downloadFailDeleteFiles(App::getBookTryReadZipPath(m_bookId), App::getBookTryReadPath(m_bookId));
						}).detach();
					}
					else
					{
						//下载错误
						YYXLayer::sendNotify(taskTag2 + "_tryReadBookProgressing", "", 100);
						string bookNameError = bookName + App::getString("XIAZAICUOWUCHANGXINXIAZAI");
						Toast::create(bookNameError.c_str());
						App::downloadFailDeleteFiles(App::getBookTryReadZipPath(m_bookId), App::getBookTryReadPath(m_bookId));
					}
				});
				m_tryReadButton->setTitleText("0%");
				App::addRecordBookDownload(m_bookId);
				//任务字符串绑定书名				
				//书籍信息 原价+书籍名称+ 书页数
				string BookNameKey = StringUtils::format("bookName+bookID=%d", bookId);
				string taskBookName = YYXStruct::getMapString(App::GetInstance()->myData, BookNameKey, "");
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, taskTag, m_bookId, "<<" + taskBookName + ">>" + App::getString("STR_SHIDU"));
				Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(taskTag + "_tryReadBookProgressing");
				auto listenerm_tryReadButton = EventListenerCustom::create(taskTag + "_tryReadBookProgressing", [=](EventCustom* e) {
					int progressing = (int)e->getUserData();
					m_tryReadButton->setTitleText(StringUtils::format("%d%%", progressing));
					if (progressing == 100)
					{
						m_tryReadButton->setTitleText(App::getString("STR_SHIDU"));
					}
				});
				Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerm_tryReadButton, m_tryReadButton);
			}
			else
			{
				//记录场景
				App::GetInstance()->pushScene(BookInfoScene, m_bookId);
				//暂停背景音乐
				SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
				//打开试读书本
				Director::getInstance()->replaceScene(Waiting::createScene(StringUtils::format("%d", m_bookId), true));
				leave();
				//试读不需要上传阅读记录
			}
		});
	});
	//--------------------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------完整--------------------------------------------------------------
	//免费 购买 下载 阅读 按钮
	isBuy = isBuyThisBook();
	m_ReadButton = ( Button*)node->getChildByName(BOOKINFO_FIND_BUTTON_RED);
	m_ReadButton->setTouchEnabled(true);
	m_ReadButton->setTitleFontSize(36);
	//string ZipDir = FileUtils::getInstance()->getWritablePath() + "bookZip";
	string ZipName = StringUtils::format("%d.zip", m_bookId);
	//string UNZipDir = FileUtils::getInstance()->getWritablePath() + "bookUNZip";
	//string bookPath = UNZipDir + "/" + StringUtils::format("%d", m_bookId);
	//string bookjsonPath = UNZipDir + "/" + StringUtils::format("%d", m_bookId) + "/Iphone1334/json.txt";
	m_ReadButton->addClickEventListener([=](Ref* sender)
	{		
		/*
		1.判断有没有登录
		2.登录之后, 有没有购买
		3.购买之后, 有没有下载
		4.下载之后, 进入阅读器
		*/
		YYXLayer::controlTouchTime(1, "BookInfoScenem_ReadButton", [=]() {
			auto childid = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
			if (App::GetInstance()->m_me == nullptr || childid == -999) {
				App::cancelData();
				App::GetInstance()->pushScene(BookInfoScene, m_bookId);
				Index::GoToLoginScene();
				return;
			}
			if (isBuy)
			{
				if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(m_bookId)))
				{
					//书籍存在
					//记录场景
					App::GetInstance()->pushScene(BookInfoScene, m_bookId);
					//暂停背景音乐
					SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
					//打开试读书本
					Director::getInstance()->replaceScene(Waiting::createScene(StringUtils::format("%d", m_bookId), false));
					leave();
					//试读不需要上传阅读记录
				}
				else
				{
					//书籍不存在
					//下载的时候, 点击暂停下载
					string zippath = App::getBookReadZipPath(m_bookId);
					string taskTag = YYXDownload::getTag(zippath);
					if (taskTag != "" && !YYXDownload::GetInstance()->isEnd(taskTag) && !YYXDownload::GetInstance()->isPause(taskTag))
					{
						Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(taskTag + "_BookProgressing");
						YYXDownload::GetInstance()->pause(taskTag);
						m_ReadButton->setTitleFontSize(36);
						m_ReadButton->setTitleText(App::getString("ZANTING"));
					}
					else
					{
						//判断未下载的书,不是购书列表中的, 属于可租书范围, 
						//未下载的情况, 点击下载
						//string beginKey = "bookInfoSceneDownLoadBookBegin";
						//string progressKey = "bookInfoSceneDownLoadBookProgress";
						//string endKey = "bookInfoSceneDownLoadBookEnd";
						string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", m_bookId);
						string bookPlayUrl = YYXStruct::getMapString(App::GetInstance()->myData, BookPlayUrlKey, "");
						if (bookPlayUrl != "" && App::getNetSetAndHintSet())
						{
							App::log("*************kaishi xiazai--------------------------");
							//vip并且是租书范围
							if (App::GetInstance()->m_me->vip && IsRentBook())
							{
								httpRentBook([=]() {
									m_ReadButton->setTitleFontSize(36);
									m_ReadButton->setTitleText("0%");
									if (App::GetInstance()->myBuyBookMap.find(m_bookId) == App::GetInstance()->myBuyBookMap.end())
										App::addRentBook(m_bookId);
									string bookTag = DownLoadBook(m_bookId, bookPlayUrl, ZipName);
									downloadingListener(m_bookId, bookTag, m_ReadButton);
								});
							}
							else
							{
								string bookTag = DownLoadBook(m_bookId, bookPlayUrl, ZipName);
								downloadingListener(m_bookId, bookTag, m_ReadButton);
								m_ReadButton->setTitleFontSize(36);
								m_ReadButton->setTitleText("0%");
							}
						}
					}
				}
			}
			else
			{
				//请求红包列表
				httpGetUserRedPackets();
				addChild(Index::SelectLayer([=]() {
					m_ReadButton->setTouchEnabled(false);
					if (m_Selling100 == 0)
						httpBuyBook();
					else
						httpGetUserBalance(App::getMemberID().c_str());		//网络请求刷新一次余额				
				}));
				return;
			}
		});
	});
	//完整书籍的下载按钮初始化
	string bookTag = YYXDownload::getTag(App::getBookReadZipPath(m_bookId));
	if (isBuy)
	{
		if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(m_bookId)))
		{
			m_ReadButton->setTitleText(App::getString("STR_READ"));
			m_tryReadButton->setVisible(false);
			if (App::GetInstance()->m_me)
			{
				if (App::GetInstance()->m_me->vip)
					App::addRentBook(m_bookId);
			}
		}
		else
		{
			//不存在
			if (bookTag != "")
			{
				m_ReadButton->setTitleFontSize(36);
				if (YYXDownload::GetInstance()->isPause(bookTag))
					m_ReadButton->setTitleText(App::getString("ZANTING"));
				else
					m_ReadButton->setTitleText("0%");
			}
			else
			{
				//未下载
				if (App::GetInstance()->m_me && App::GetInstance()->m_me->vip)
				{
					m_ReadButton->setTitleText(App::getString("VIPXIAZAI"));			
					m_ReadButton->setTitleFontSize(26);
				}
				else
				{
					m_ReadButton->setTitleFontSize(36);
					m_ReadButton->setTitleText(App::getString("STR_DOWNLOAD"));
				}
			}			
		}
	}
	else
	{
		m_ReadButton->setTitleFontSize(36);
		m_ReadButton->setTitleText(App::getString("STR_BUY"));
	}
	//进度监听
	if (bookTag != "")
	{
		auto listenerButton = EventListenerCustom::create(bookTag + "_BookProgressing", [=](EventCustom* e) {
			auto progressing = (long)e->getUserData();
			if (progressing < 0)
				return;
			m_ReadButton->setTitleFontSize(36);
			if (progressing <= 99)
				m_ReadButton->setTitleText(StringUtils::format("%d%%", progressing));
			else if (progressing == 100)
				m_ReadButton->setTitleText(App::getString("STR_READ"));
			else if (progressing == 999)
				m_ReadButton->setTitleText(App::getString("STR_DOWNLOAD"));
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerButton, m_ReadButton);
	}
	//购买刷新
	auto listenerm_ReadButton = EventListenerCustom::create("isBuyRefersh", [=](EventCustom* e) {
		isBuy = isBuyThisBook();
		if (isBuy)
		{
			if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(m_bookId)))
			{
				m_tryReadButton->setVisible(false);
				m_ReadButton->setTitleFontSize(36);
				m_ReadButton->setTitleText(App::getString("STR_READ"));
			}
			else
			{
				if (YYXDownload::GetInstance()->isPause(bookTag)) {
					m_ReadButton->setTitleText(App::getString("ZANTING"));
					m_ReadButton->setTitleFontSize(36);
				}
				else
				{
					if (App::GetInstance()->m_me && App::GetInstance()->m_me->vip && App::GetInstance()->myBuyBookMap.find(m_bookId) == App::GetInstance()->myBuyBookMap.end())
					{
						m_ReadButton->setTitleText(App::getString("VIPXIAZAI"));
						m_ReadButton->setTitleFontSize(26);
					}
					else
					{
						m_ReadButton->setTitleFontSize(36);
						m_ReadButton->setTitleText(App::getString("STR_DOWNLOAD"));
					}
				}
			}
		}
		else
		{
			m_ReadButton->setTitleText(App::getString("STR_BUY"));
			m_ReadButton->setTitleFontSize(36);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerm_ReadButton, m_ReadButton);

	//没有WiFi连接的通知
	auto listenNoWifi = EventListenerCustom::create("Down_No_Wifi", [=](EventCustom* event) {
		Toast::create(App::getString("NO_WIFI"));
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listenNoWifi, 1);
	App::log(" BookInfo::init 44");
	//---------------------------界面--------------------------------------------------------------
	//初始化封面,价格,时间等数据
	//loadBackGroundData();
	//简介是一个listview  创建控件,然后从内存中将数据载入控件
	m_View_Info = initIntroView();
	node->addChild(m_View_Info, 3);
	//评论
	m_View_Comment = initCommonView();
	m_View_Comment->setVisible(false);
	node->addChild(m_View_Comment, 2);
	
	//展示信息
	refreshUI();

	//vip提示文字
	initVipText(node);

	//App::log(m_cover);
	//封面
	string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
	std::string fileName = StringUtils::format("%d", bookId) + ".png";
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
	//---------------------------网络请求--------------------------------------------------------------------
	//std::thread httpBookinfo(&BookInfo::httpBookInfo,this,m_bookId);
	//httpBookinfo.detach();
	//std::thread httpbookcomment(&BookInfo::httpGetBookComment,this,m_bookId);
	//httpbookcomment.detach();
	//---------------------------网络请求--------------------------------------------------------------------

	//返回按钮
	m_createTime = App::getCurrentTime();
	auto homeButton = ( Button*)node->getChildByName(BOOKINFO_FIND_BACK);
	homeButton->removeFromParentAndCleanup(true);
	homeButton->setAnchorPoint(Vec2(0, 1));
	homeButton->setPosition(origin + Vec2(14, visibleSize.height - 14));
	addChild(homeButton, 10);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "BookInfoScenehomeButton", [=]() {
			auto t = m_createTime ;
			if (App::getCurrentTime() > t)
			{
				back();
				homeButton->setTouchEnabled(false);				
			}
		});
	});
	initEvent();
	//------------------------------接受通知:刷新支付状态--------------------------------------------------------------------------
	auto lister_pay = EventListenerCustom::create("pay", [=](EventCustom* event) {
		updateBuyBookList();//更新后台和本地书籍列表
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(lister_pay, 1);
	//------------------------------接受通知:刷新支付状态--------------------------------------------------------------------------

	//------------------------------接受通知:刷新界面--------------------------------------------------------------------------
	auto lister_redraw = EventListenerCustom::create("referui", [=](EventCustom* event) {
			m_refre = 1;
			if (this->m_refre > 0)
			{
				App::log("[刷新次数refreUI] = %d", m_refre);
			}
			refreshUI();
			++m_refre;
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(lister_redraw, 1);
	//------------------------------接受通知:刷新界面--------------------------------------------------------------------------

	//------------------------------接受通知:messagebox-------------------------------------------------------------------------
	auto messagebox = EventListenerCustom::create("messagebox", [=](EventCustom* event) {
		//MessageBox((const char*)event->getUserData(),"网络提示");
		Toast::create((const char*)event->getUserData());
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(messagebox, 1);
	//------------------------------接受通知:messagebox--------------------------------------------------------------------------

	//-----------------------------接受通知:余额扣款返回 --------------------------------------------------------------------------
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("buybookcallback", [=](EventCustom* event) {
		m_ReadButton->setTouchEnabled(true);
	});
	//----------------------接受通知:余额扣款返回 END--------------------------------------------------------------------------

	//-----------------------------接受通知:去充值 --------------------------------------------------------------------------
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("gotorecharge", [=](EventCustom* event) {
		recharge();
	});
	//----------------------接受通知:去充值 END--------------------------------------------------------------------------

	//-----------------------------接受通知:购书前查询余额的回调 --------------------------------------------------------------------------
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("showyue", [=](EventCustom* event) {
		showPay();
	});
	//-----------------------------接受通知:购书前查询余额的回调END --------------------------------------------------------------------------	

	//-----------------------------接受通知:网络获取评论 --------------------------------------------------------------------------
	//Director::getInstance()->getEventDispatcher()->addCustomEventListener("httpBookComments", [=](EventCustom* event) {
		//std::thread httpbookcomment(&BookInfo::httpGetBookComment, this, m_bookId);
		//httpbookcomment.detach();
	//});
	//-----------------------------接受通知:网络获取评论END --------------------------------------------------------------------------
	
	// ---------------------------- - 接受通知:判断是否是已购书籍并展示--------------------------------------------------------------------------
	/*Director::getInstance()->getEventDispatcher()->addCustomEventListener("panduanIsBuy", [=](EventCustom* event) {
		//查看这本书是否已购买
		if (SqliteManager::IsRecExist(App::sqliteOpen(), DB_USERBOOKLIST, StringUtils::format("where bookId=%d", m_bookId) + " and member_id=" + App::getMemberID())) {
			isBuy = true;
			YYXLayer::sendNotify("bookInfoRemoveHint");
			m_ReadButton->setTitleText(App::getString("STR_DOWNLOAD"));
		}
	});*/
	//-----------------------------接受通知:判断是否是已购书籍并展示END --------------------------------------------------------------------------

	// ---------------------------- - 接受通知:充值成功后,自动继续购书--------------------------------------------------------------------------
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("CallBackPaySuccessGoToBuyBook", [=](EventCustom* event) {
		//网络请求刷新一次余额,余额查询后会调用购买界面开始购买
		httpGetUserBalance(App::getMemberID().c_str());
	});
	//-----------------------------接受通知:充值成功后,自动继续购书--END --------------------------------------------------------------------------

	// ---------------------------- - 接受通知:删除等待层--------------------------------------------------------------------------

	//-----------------------------接受通知:删除等待层--END --------------------------------------------------------------------------

	// ---------------------------- - 接受通知:红包购书--------------------------------------------------------------------------
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("buybookwithredpacket", [=](EventCustom* event) {
		//提示是否确认继续支付
		IsNotGoToPayMessageBox(true);
	}); 
	//-----------------------------接受通知:红包购书END --------------------------------------------------------------------------

	// ---------------------------- - 接受通知:无红包购书 直接购书--------------------------------------------------------------------------
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("buybook", [=](EventCustom* event) {
		//提示是否确认继续支付
		IsNotGoToPayMessageBox();
	});
	//-----------------------------接受通知:无红包购书 直接购书END --------------------------------------------------------------------------

	//页面的切换
	auto listenEvent = [=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "BookInfoScenelistenEvent", [=]() {
			YYXLayer::sendNotify("StopAnimation", "", 0);
			AudioEngine::stopAll();
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			introYellow->loadTexture(BOOKINFO_ABSTRACT_PNG, TextureResType::PLIST);
			commonGray->loadTexture(BOOKINFO_COMMENT_PNG, TextureResType::PLIST);

			introYellow2->setTouchEnabled(true);
			commonGray2->setTouchEnabled(true);

			switch ((( ImageView*)sender)->getTag())
			{
			case 1:
				introYellow->loadTexture(BOOKINFO_ABSTRACT_Y_PNG, TextureResType::PLIST);
				isComment = false;
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
				auto ve = m_listview->getItems();
				if (ve.size() > 0)
					break;
				YYXLayer::showCommentListView(m_listview, m_bookId);
				//刷新列表
				Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("showCommentListView");
				Director::getInstance()->getEventDispatcher()->addCustomEventListener ("showCommentListView", [=](EventCustom* e) {
					if (!isComment)
						return;
					int memberid = (int)e->getUserData();
					App::log(" --------------     run showCommentListView()   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^memberid = ", memberid);
					AudioEngine::stopAll();
					if (YYXLayer::getBoolFromXML(MUSIC_KEY))
						SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
					else
						SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
					YYXLayer::showCommentListView(m_listview, m_bookId);
				});
				m_listview->jumpToTop();
				break;
			};
			(( ImageView*)sender)->setTouchEnabled(false);
		});
	};
	introYellow2->addClickEventListener(listenEvent);
	commonGray2->addClickEventListener(listenEvent);
	App::log(" BookInfo::init 88");
	App::GetInstance()->addTime("bookInfoSceneCreateTime", App::getCurrentTime());
	////安卓返回键
	/*auto androidListener = EventListenerKeyboard::create();
	androidListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		YYXLayer::controlTouchTime(3, "androidKeyTime", [keyCode,this]() {
			switch (keyCode)
			{
			case EventKeyboard::KeyCode::KEY_ESCAPE:
				back();
				break;
			default:
				break;
			}
		});
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(androidListener, this);*/
	App::log("当前场景:", App::GetInstance()->m_RunningScene);
	m_name->setTouchEnabled(true);
	App::log(" BookInfo::init--END", this->getReferenceCount());
	return true;
}

void BookInfo::initEvent()
{
	//通知处理:获取用户有效红包列表
	/*Director::getInstance()->getEventDispatcher()->addCustomEventListener("httpGetUserRedPackets_Success", [=](EventCustom* event) {
		auto data = (YYXStruct*)event->getUserData();
		auto stringdata = YYXStruct::getStringData(*data, "");
		if(&stringdata && stringdata != "")
			httpGetRedpacketCallback(stringdata);
		else
			Toast::create(App::getString("HONGBAOHUOQUSHIBAI"));
		if (data)
			delete data;
	});
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("httpGetUserRedPackets_Fail", [=](EventCustom* event) {
		Toast::create(App::getString("HONGBAOHUOQUSHIBAI"));
		auto data = (YYXStruct*)event->getUserData();
		if (data)
			delete data;
	});*/
	//通知处理:使用红包购书
	//Director::getInstance()->getEventDispatcher()->addCustomEventListener("httpBuyBookWithRedPacket_Success", [=](EventCustom* event) {
	//	YYXLayer::sendNotify("buybookcallback");
	//	auto data = (YYXStruct*)event->getUserData();
	//	auto stringdata = YYXStruct::getStringData(*data, "");
	//	if (&stringdata && stringdata != "")
	//		httpBuyBookCallback(stringdata);
	//	else
	//		Toast::create(App::getString("BUY_EXCEPTION"));
	//	if (data)
	//		delete data;
	//});
	//Director::getInstance()->getEventDispatcher()->addCustomEventListener("httpBuyBookWithRedPacket_Fail", [=](EventCustom* event) {
	//	YYXLayer::sendNotify("buybookcallback");
	//	Toast::create(App::getString("NETEXCEPTION"));
	//	auto data = (YYXStruct*)event->getUserData();
	//	if (data)
	//		delete data;
	//});
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("addHint", [=](EventCustom* e) {
		hint();
	}); 
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("yuebuzuAddMessagebox", [=](EventCustom* e) {
		YYXLayer::MyMessageBox(App::getString("YUEBUZU"), App::getString("QUCHONGZHI"), [=]() {
			recharge();
		}, App::getString("QUXIAO"), [=]() {});
	});
	//等待已购列表的刷新判断
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("loginSceneHttpGetUserBuyBooksSuccess", [=](EventCustom* e) {
		isBuy = isBuyThisBook();
		if (isBuy)
		{
			YYXLayer::sendNotify("bookInfoRemoveHint");
			YYXLayer::sendNotify("isBuyRefersh");
		}
	});
	//评论成功
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("bookinfoSceneHttpSendCommentSuccess", [=](EventCustom* e) {
		//httpGetComments();
		App::httpComment(m_bookId, []() {
			YYXLayer::sendNotify("showCommentListView", "", -1);
			App::log("httpComment  ===>>> showCommentListView");
		});
	});
}

void BookInfo::initHttp()
{
	//书籍详情
	string runKey = "bookInfoSceneHttpGetBookInfoSuccess";
	string errorKey = "bookInfoSceneHttpGetBookInfoFail";
	NetIntface::httpGetBookInfo(m_bookId, runKey, [](string json) {
		NetIntface::httpGetBookInfoCallBack(json, [](bool isvip, int bookId, int bookPage, int bookPrice100, int bookmarketPrice100, int remainTime, string bookAuthor,
			string bookSize, string bookPress, string bookIntroduction, string bookName, string bookPlayUrl, string bookCoverUrl, string bookViewUrl) {
			//解析正常
			//书籍信息 原价+书籍名称+ 书页数
			string BookNameKey = StringUtils::format("bookName+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookNameKey, bookmarketPrice100, bookName, (Ref*)bookPage);
			//现价+封面url+新书标记
			string BookPriceKey = StringUtils::format("bookPrice+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPriceKey, bookPrice100, bookCoverUrl);
			//购书标记+下载URL + 	是否是vip书籍
			string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", bookId);
			if (isvip)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, -999, bookPlayUrl, (Ref*)1);
			else
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, -999, bookPlayUrl, (Ref*)0);
			if (isvip)
			{
				App::log("               is VIP");
			}
			else
				App::log("    not is vip");
			//活动倒计时 + 书籍作者
			string BookAuthorKey = StringUtils::format("bookAuthor+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookAuthorKey, remainTime, bookAuthor);
			//出版社
			string BookPressKey = StringUtils::format("bookPress+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPressKey, -999, bookPress);
			//书籍资源大小
			string BookSizeKey = StringUtils::format("bookSize+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookSizeKey, -999, bookSize);
			//书籍介绍
			string bookIntroductionKey = StringUtils::format("bookIntroduction+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookIntroductionKey, -999, bookIntroduction);
			//试读url
			string bookViewUrlKey = StringUtils::format("bookViewUrl+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookViewUrlKey, -999, bookViewUrl);
			YYXLayer::sendNotify("referui");
			YYXLayer::sendNotify("addHint");
			YYXLayer::sendNotify("isBuyRefersh");
			YYXLayer::sendNotify("refershVIPText");
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
	});
}

void BookInfo::onEnterTransitionDidFinish()
{
	App::log("BookInfo::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	auto something = YYXStruct::getMapString(App::GetInstance()->myData, "ComeFromBookDoSomething", "");
	if (something =="buy")
	{
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ComeFromBookDoSomething", -999, "NULL");
		if (isBuyThisBook())
			return;
		else {
			if(App::GetInstance()->m_me)
				showPay();
		}
	}
	else if (something == "download")
	{
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ComeFromBookDoSomething", -999, "NULL");
		string strpath = App::getBookRead4Json_txtPath(m_bookId);
		App::log("                ===============>>>>>>>>>>>>>.    strpath= " + strpath);
		if (FileUtils::getInstance()->isFileExist(strpath)) {
			return;
		}
		if (isBuyThisBook())
		{
			string ZipName = StringUtils::format("%d.zip", m_bookId);
			string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", m_bookId);
			string bookPlayUrl = YYXStruct::getMapString(App::GetInstance()->myData, BookPlayUrlKey, "");
			if (bookPlayUrl != "" && App::getNetSetAndHintSet()) {
				if (App::GetInstance()->m_me->vip)
				{
					httpRentBook([=]() {
						m_ReadButton->setTitleFontSize(36);
						m_ReadButton->setTitleText("0%");
						string bookTag = DownLoadBook(m_bookId, bookPlayUrl, ZipName);
						downloadingListener(m_bookId, bookTag, m_ReadButton); 
					});
				}
				else {
					m_ReadButton->setTitleFontSize(36);
					m_ReadButton->setTitleText("0%");
					string bookTag = DownLoadBook(m_bookId, bookPlayUrl, ZipName);
					downloadingListener(m_bookId, bookTag, m_ReadButton);
				}
			}
		}
	}
	App::log("BookInfo::onEnterTransitionDidFinish---END");
}

void BookInfo::cleanup()
{
	leave();
	unscheduleAllSelectors();
	AudioEngine::stopAll();
}

//刷新界面
void BookInfo::refreshUI()
{
	App::log("BookInfo::refreshUI");
	//现价
	string BookPriceKey = StringUtils::format("bookPrice+bookID=%d", m_bookId);
	m_Selling100 = YYXStruct::getMapInt64(App::GetInstance()->myData, BookPriceKey, -999);
	//倒计时
	string BookAuthorKey = StringUtils::format("bookAuthor+bookID=%d", m_bookId);
	auto remainTime = YYXStruct::getMapInt64(App::GetInstance()->myData, BookAuthorKey, -999);
	showTime(remainTime * 1000);
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
	//判断是否为免费书籍
	if (isBuy)
		return;
	if (m_Selling100 == 0)
	{
		m_ReadButton->setTitleFontSize(36);
		m_ReadButton->setTitleText(App::getString("STR_FREE"));
	}
	else
	{
		m_ReadButton->setTitleFontSize(36);
		m_ReadButton->setTitleText(App::getString("STR_BUY"));
	}
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
		str_or = StringUtils::format("%s%.02f", App::getChars("STR_MONEY"), bookmarketPrice100 / 100.0);
	}
	if (bookmarketPrice100 >=0 && bookmarketPrice100 <1000)
	{
		str_or = StringUtils::format("%s %.02f", App::getChars("STR_MONEY"), bookmarketPrice100 / 100.0);
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
	m_listview_Info->setSize(Size(320, 320));
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
	auto page = (int)YYXStruct::getMapRef(App::GetInstance()->myData, BookNameKey, (Ref*)-999);
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
	string author = YYXStruct::getMapString(App::GetInstance()->myData, BookAuthorKey,"");
	if (author.length() !=0 )
		str = App::getString("STR_BOOKAUTHOE") + author;
	auto m_text_bookAuthor = Label::create(str, "FZHLJW.TTF",34, Size::ZERO, TextHAlignment::LEFT, TextVAlignment::TOP);
	m_text_bookAuthor->setTextColor(Color4B::GRAY);
	m_text_bookAuthor->setAnchorPoint(Vec2(0, 0));
	m_text_bookAuthor->setAdditionalKerning(8);			//字体间距
	m_text_bookAuthor->setScale(0.5);
	layout4->setContentSize(m_text_bookAuthor->getContentSize()/2);
	layout4->addChild(m_text_bookAuthor);
	App::log("BookInfo::creatItem_bookAuthor--END");
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
			//用户未登录
			if (App::GetInstance()->m_me == nullptr) {
				Toast::create(App::getString("COMMENT_TIP_LOGIN"));
				return;
			}
			//未购买
			if (!isBuyThisBook()) {
				Toast::create(App::getString("COMMENT_TIP_BUY"));
				return;
			}
			auto pScene = Director::getInstance()->getRunningScene();
			pScene->addChild(SendComment::create(m_bookId));
		});
	});
	m_listview = (ListView*)pinglun->getChildByName(BOOKINFO_FIND_LISTVIEW_BYNAME_COMMENT);
	m_listview->setScrollBarEnabled(false);
	m_listview->setSize(Size(320, 300));
	App::log("BookInfo::initCommonView---END");
	return pinglun;
}

//停止定时器
void BookInfo::closeTime()
{
	App::log("BookInfo::closeTime");
	unschedule("EventTimeIsOneSecond");
	unschedule(BookInfoCountdown);
	App::log("BookInfo::closeTime---END");
}

//展示选择红包,支付界面
Node* BookInfo::showPay(bool withR)
{
	App::log("BookInfo::showPay");
	if (m_Selling100 == -999)
	{
		Toast::create(App::getString("ZHENGZAIWANGLUOQINGQIUSHAOHOUZAISHI"));
		return nullptr;
	}
	int myMomey = 0;
	if (App::GetInstance()->m_me)
	{
		myMomey = App::GetInstance()->m_me->momey;
	}
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "showpay");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_WHETHERTOPAY_CSB);
	auto payLayer = YYXLayer::create(paramter);
	if (!payLayer)
		return nullptr;
	auto gotoPay = (Button*)payLayer->findControl(FIND_BUTTON_BY_NAME_PAY_GOTOPAY);
	auto balance = (Text*)payLayer->findControl(FIND_TEXT_BY_NAME_PAY_BALANCE);
	auto bookname = (Text*)payLayer->findControl(FIND_TEXT_BY_NAME_PAY_BOOKNAME);
	auto bookprice = (Text*)payLayer->findControl(FIND_TEXT_BY_NAME_PAY_BOOKPRICE);
	auto close = (Button*)payLayer->findControl(FIND_BUTTON_BY_NAME_PAY_CLOSE);
	auto hongbaoshow = (Text*)payLayer->findControl(FIND_TEXT_BY_NAME_SHOWREDPACKET);
	auto paypricetext = (Text*)payLayer->findControl(FIND_TEXT_BY_NAME_PAYPRICE);
	auto selcetRedPacket1 = (ImageView*)payLayer->findControl(FIND_IMAGEVIEW_BY_NAME_REDPACKETBACKGROUND);
	auto selcetRedPacket2 = (ImageView*)payLayer->findControl(FIND_IMAGEVIEW_BY_NAME_REDPACKETJIANTOU);
	auto timeOut = (Text*)payLayer->findControl(FIND_TEXT_BY_NAME_REDPACKET_TIMEOUT);
	if (timeOut)
	{
		int count = YYXStruct::getMapInt64(App::GetInstance()->myData, "expiring_coupon_count", 0);
		if (count >= 0 )
			timeOut->setText(StringUtils::format(App::getString("HONGBAOJIJIANGDAOQI"), count));
	}
	auto listenfunction = [=](Ref* sender) {
		payLayer->removeFromParentAndCleanup(true);
		//打开选择红包界面
		selectOrShowRedPackets();
	};
	if (selcetRedPacket1 && selcetRedPacket2)
	{
		selcetRedPacket1->setTouchEnabled(true);
		selcetRedPacket2->setTouchEnabled(true);
		selcetRedPacket1->addClickEventListener(listenfunction);
		selcetRedPacket2->addClickEventListener(listenfunction);
	}
	if (bookname && &m_BookName  && !m_BookName.empty())
		bookname->setText(m_BookName);
	if(bookprice)
		bookprice->setText(StringUtils::format("%.02f", m_Selling100 / 100.0) + App::getString("YUAN"));
	if(balance)
		balance->setText(StringUtils::format("%.02f", myMomey / 100.0) + App::getString("YUAN"));
	if (close)
	{
		close->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneclose", [=]() {
				payLayer->removeFromParentAndCleanup(true);
				m_ReadButton->setTouchEnabled(true);
			});
		});
	}
	if (gotoPay)
	{
		gotoPay->setTag(-999);
		gotoPay->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneclose", [=]() {
				payLayer->removeFromParentAndCleanup(true);
				m_ReadButton->setTouchEnabled(true);
				if(gotoPay->getTag() == 0)//红包购书
					YYXLayer::sendNotify("buybookwithredpacket", "BookInfo");
				if (gotoPay->getTag() == 1)//无红包购书
					YYXLayer::sendNotify("buybook", "BookInfo");
				if (gotoPay->getTag() == 2)//去充值
					YYXLayer::sendNotify("gotorecharge", "BookInfo");
			});
		});
	}
	//1.红包展示
	//2.实付
	//3.充值或者购买按钮
	int payPrice100 = 0;//实付价格
	bool withRedPacket = false;
	if (App::GetInstance()->m_redPacket.size() == 0)
	{
		if (hongbaoshow)
			hongbaoshow->setText(App::getString("MEIYOUKEYONGHONGBAO"));
		payPrice100 = m_Selling100;
		if (paypricetext)
			paypricetext->setText(StringUtils::format("%.02f", m_Selling100 / 100.0) + App::getString("YUAN"));
	}
	else
	{
		int redpacketPrice100 = 0;
		if (withR)
		{
			//选择红包界面回来
			redpacketPrice100 = YYXStruct::getMapInt64(App::GetInstance()->myData, StringUtils::format("bookId=%d+redpacket", m_bookId), -999);
			App::log("book之前选择的红包", redpacketPrice100);
			if (redpacketPrice100 == -999) {
				redpacketPrice100 = getOptimalRedPacket();
				App::log("book最优选择的红包", redpacketPrice100);
			}
		}
		else
		{
			redpacketPrice100 = getOptimalRedPacket();
		}
		if (hongbaoshow)
		{
			if(redpacketPrice100 > 0)
				hongbaoshow->setText(App::getString("HONGBAO") + StringUtils::format("%.02f", redpacketPrice100 / 100.0) + App::getString("YUAN"));
			else if (redpacketPrice100 == 0)
				hongbaoshow->setText(App::getString("BUSHIYONGHONGBAO"));
			else if (redpacketPrice100 <0)
				hongbaoshow->setText(App::getString("MEIYOUKEYONGHONGBAO"));
		}
		payPrice100 = m_Selling100 - redpacketPrice100;
		if (payPrice100 < 0)
			payPrice100 = 0;
		if (paypricetext)
			paypricetext->setText(StringUtils::format("%.02f", payPrice100 / 100.0) + App::getString("YUAN"));
		withRedPacket = true;
	}
	if (myMomey >= payPrice100)
	{
		if (gotoPay)
		{
			gotoPay->setTitleText(App::getString("GOUMAI"));
			if (withRedPacket)
				gotoPay->setTag(0);
			else
				gotoPay->setTag(1);
		}
	}
	else
	{
		if (gotoPay)
		{
			gotoPay->setTitleText(App::getString("CHONGZHI"));
			gotoPay->setTag(2);
		}
	}
	payLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	Size visibleSize = Director::getInstance()->getVisibleSize();
	payLayer->setPosition(visibleSize / 2);
	addChild(payLayer);	
	return payLayer;
	App::log("BookInfo::showPay--END");
}

//选出最大红包
int BookInfo::getMaxRedPacket()
{
	int count = App::GetInstance()->m_redPacket.size();
	int optimal = 0;
	int optimalID = -999;
	if (count <= 0)
		return -1;//无可用红包
	optimal = App::GetInstance()->m_redPacket[0]["coupon_amount"].intData;
	optimalID = App::GetInstance()->m_redPacket[0]["coupon_id"].intData;
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, StringUtils::format("bookId=%d+redpacket", m_bookId), optimal, "", (Ref*)optimalID);
	return optimal;
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

//网络请求 购书余额结算
bool BookInfo::httpBuyBook() {
	if (m_bookId == -999)
		return false;
	auto redid = (long)YYXStruct::getMapRef(App::GetInstance()->myData, StringUtils::format("bookId=%d+redpacket", m_bookId), (Ref*)-1);
	if (redid == -1)
	{//无红包购书
//		//Toast::create("buy book");
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//		CocosAndroidJni::RechargeBuyBook(App::getMemberID().c_str(), m_bookId, m_Selling100);
//#endif
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//#endif
		NetIntface::httpBuyBookWithOutRedPacket(App::GetInstance()->m_me->id, m_bookId, "httpBuyBookWithOutRedPacketSuccess", [](string json) {
			NetIntface::httpBuyBookWithOutRedPacketCallback(json, []() {
				//购书成功
				YYXLayer::sendNotify("pay");
				Toast::create(App::getString("BUY_SCEESSE"));
			}, []() {
				//余额不足
				YYXLayer::sendNotify("yuebuzuAddMessagebox");
			}, []() {
				//购书失败
				Toast::create(App::getString("BUY_FAIL"));
			});
		}, "httpBuyBookWithOutRedPacketFail", [](string str) {
			//网络异常
			Toast::create(App::getString("NETEXCEPTION"));
		});
	}
	else
	{//有红包购书
		int couponId = (int)redid;
		NetIntface::httpBuyBookWithRedPacket(App::GetInstance()->m_me->id, couponId, m_bookId, "httpBuyBookWithRedPacket_Success", [=](string json) {
			NetIntface::httpBuyBookWithRedPacketCallback(json, [=](string str) {
				YYXLayer::sendNotify("pay");
				httpGetUserRedPackets();
				Toast::create(App::getString("BUY_SCEESSE"));
			}, [](string error) {
				Toast::create(error.c_str());
			});
		}, "httpBuyBookWithRedPacket_Fail", [](string str) {
			Toast::create(App::getString("NETEXCEPTION"));
		});
	}
	return true;
}

//获取订单号
//void BookInfo::getOrderId() {
	/*App::log("BookInfo::getOrderId");
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_GET_ORDER).c_str());

	auto postData = std::string("memberId=").append(App::getMemberID()).append(StringUtils::format("&bookId=%d&resource=", m_bookId)).append(App::m_resource);
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));

	App::log(postData);

	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		if (!response) {
			Toast::create(App::getString("ORDER_FAILED"));
			m_ReadButton->setTouchEnabled(true);
			App::log("BookInfo::getOrderIdCALLBACK 1----END");
			return;
		}
		if (!response->isSucceed()){
			Toast::create( App::getString("ORDER_FAILED"));
			m_ReadButton->setTouchEnabled(true);
			log("error buffer: %s", response->getErrorBuffer());
			App::log("BookInfo::getOrderIdCALLBACK 2----END");
			return;
		}
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		rapidjson::Document doc;
		doc.Parse<0>(str.c_str());
		App::log(str);
		if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("code") || !doc.HasMember("data")){
			Toast::create( App::getString("ORDER_FAILED"));
			m_ReadButton->setTouchEnabled(true);
			App::log("BookInfo::getOrderIdCALLBACK 3----END");
			return;
		}
		
		rapidjson::Value &value = doc["code"];
		if (value.IsInt() && value.GetInt() == 0){//请求正常		
			rapidjson::Value &data = doc["data"];
			if (data.IsObject()) {
				m_orderId = App::analysisJsonInt(data, "orderId");
				//判断是否为免费书籍
				if (m_Selling100 == 0)
				{
					App::log("BookInfo::getOrderIdCALLBACK 4----END");
					Toast::create(App::getString("BUY_SCEESSE"));
					updateBuyBookList(true);//更新后台和本地书籍列表
				}
				else
				{
					bookPay();
					m_ReadButton->setTouchEnabled(true);					
				}
			}
		}
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
	App::log("BookInfo::getOrderId--END");*/
//}

//更新后台和本地书籍列表
void BookInfo::updateBuyBookList( ) {
	App::log("BookInfo::updateBuyBookList");
	m_ReadButton->setTouchEnabled(true);
	string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", m_bookId);
	string bookPlayUrl = YYXStruct::getMapString(App::GetInstance()->myData, BookPlayUrlKey, "");
	App::GetInstance()->myBuyBookMap[m_bookId] = bookPlayUrl;
	App::GetInstance()->myBuyBookVector.push_back(m_bookId);
	m_ReadButton->setTitleFontSize(36);
	m_ReadButton->setTitleText(App::getString("STR_DOWNLOAD"));
	isBuy = true;
	YYXLayer::sendNotify("bookInfoRemoveHint");
	YYXLayer::sendNotify("refershVIPText");
	App::log("BookInfo::updateBuyBookList price---END");
}

void BookInfo::back()
{
	App::log("BookInfo::back");
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	if (YYXLayer::getBoolFromXML(SOUND_KEY))
		YYXLayer::PLAYBUTTON;
	leave();
	App::GetInstance()->popBack(MySceneName::BookInfoScene);
	Index::BackPreviousScene();
	App::log("BookInfo::back---END");
}

//去充值
void BookInfo::recharge()
{
	auto recharge = Recharge::create();
	addChild(recharge);
}

//提示是否确认继续支付
void BookInfo::IsNotGoToPayMessageBox(bool withRedPacket)
{
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "IsNotGoToPay");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_ISPAY_CSB);
	auto payLayer = YYXLayer::create(paramter);
	if (!payLayer)
		return;
	auto yes = (Button*)payLayer->findControl(FIND_BUTTON_ISPAY_BUY);
	auto no = (Button*)payLayer->findControl(FIND_BUTTON_ISPAY_NO);
	auto content = (Text*)payLayer->findControl(FIND_TEXT_ISPAY_CONTEXT);
	string text = "";
	if (withRedPacket)
	{
		int redPacketPrice = YYXStruct::getMapInt64(App::GetInstance()->myData, StringUtils::format("bookId=%d+redpacket", m_bookId), 0);
		int payprice = m_Selling100 - redPacketPrice;
		if (payprice < 0)
			payprice = 0;
		text = StringUtils::format(App::getString("HUAFEIMONEYBUYBOOK"), (payprice / 100.0), m_BookName.c_str());		
		if (payprice == 0)
			text = StringUtils::format(App::getString("NINGJIANGMIANFEIGOUMAISHUJI"), m_BookName.c_str());
	}
	else
	{
		text = StringUtils::format(App::getString("HUAFEIMONEYBUYBOOK"), (m_Selling100 / 100.0), m_BookName.c_str());
	}
	auto ttf = Label::create();
	ttf->setAnchorPoint(Vec2(0.5, 0.5));
	ttf->setColor(Color3B(92, 92, 92));
	ttf->setTTFConfig(TTFConfig(content->getFontName().c_str(), 50));
	ttf->setScale(0.5);
	ttf->setMaxLineWidth(800);
	ttf->setString(text);
	ttf->setLineHeight(ttf->getLineHeight()+15);
	ttf->setAdditionalKerning(ttf->getAdditionalKerning()+5);
	content->setVisible(false);
	payLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	payLayer->setPosition(Director::getInstance()->getVisibleSize() / 2);
	payLayer->addChild(ttf);
	if (yes)
	{
		yes->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneyes", [=]() {
				payLayer->removeFromParentAndCleanup(true);
				httpBuyBook();
			});
		});
	}
	if (no)
	{
		no->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneno", [=]() {
				payLayer->removeFromParentAndCleanup(true);
				m_ReadButton->setTouchEnabled(true);
			});
		});
	}
	addChild(payLayer);
}

//查询余额
void BookInfo::httpGetUserBalance(const char* memberID)
{
	string runKey = "bookInfoSceneHttpGetUserBalanceSuccess";
	string errorKey = "bookInfoSceneHttpGetUserBalanceFail";
	NetIntface::httpGetUserBalance(App::GetInstance()->m_me->id, runKey, [](string json) {
		//解析json 
		//写入内存和xml
		NetIntface::httpGetUserBalanceCallBack(json, [](int id, int userBalance, long long uptime) {
			//正确解析
			if (App::GetInstance()->m_me)
				App::GetInstance()->m_me->momey = userBalance;
			//UserDefault::getInstance()->setIntegerForKey("userBalance", userBalance);
			YYXLayer::setFileValue("userBalance", StringUtils::format("%d", userBalance));
			YYXLayer::sendNotify("buybookcallback");
			YYXLayer::sendNotify("showyue");
		}, []() {
			//返回错误,或者解析错误
			Toast::create(App::getString("NETEXCEPTION"));
			YYXLayer::sendNotify("buybookcallback");
		});
	}, errorKey, [](string str) {
		//网络错误
		Toast::create(App::getString("NETEXCEPTION"));
		YYXLayer::sendNotify("buybookcallback");
	});
}

void BookInfo::leave()
{
	//App::GetInstance()->RefVector.push(YYXStruct(App::getCurrentTime(),"bookinfo",this));
	AudioEngine::stopAll();
	closeTime();
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format("%dDownloading", m_bookId));
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format("%dDownload_Success", m_bookId));
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format("%dDownload_Failed", m_bookId));
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format("%dUnpress_Success", m_bookId));
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format("%dviewDownload_Success", m_bookId));
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format("%dviewDownload_Failed", m_bookId));
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format("%dviewUnpress_Success", m_bookId));
}

//选择红包界面
void BookInfo::selectOrShowRedPackets()
{
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "selectRedPacket");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_SELECTREDPACKET_CSB);
	auto redPacketLayer = YYXLayer::create(paramter);

	if (!redPacketLayer)
		return;
	auto myredpacket = (ImageView*)redPacketLayer->findControl(FIND_IMAGEVIEW_BY_NAME_SELECTREDPACKET_MYREDPACKET);
	auto backbutton = (ImageView*)redPacketLayer->findControl(FIND_IMAGEVIEW_BY_NAME_SELECTREDPACKET_BACK);
	//有N个红包到期
	auto daoqi = (Text*)redPacketLayer->findControl(FIND_TEXT_BY_NAME_SELECTREDPACKET_DAOQI);
	if (daoqi)
	{
		int expiring_coupon_count = YYXStruct::getMapInt64(App::GetInstance()->myData, "expiring_coupon_count", 0);
		daoqi->setText(StringUtils::format("%d%s", expiring_coupon_count, App::getString("NGEHONGBAOJIJIANGDAOQI")));
	}
	//刷新红包按钮
	auto refer = (Text*)redPacketLayer->findControl(FIND_TEXT_BY_NAME_SELECTREDPACKET_REFER);
	auto listview = (ListView*)redPacketLayer->findControl(FIND_LISTVIEW_BY_NAME_SELECTREDPACKET_REDPACKET);
	listview->setScrollBarAutoHideEnabled(false);
	listview->setScrollBarEnabled(true);
	//不使用红包
	auto without = (ImageView*)redPacketLayer->findControl(FIND_IMAGEVIEW_BY_NAME_SELECTREDPACKET_DONOTWITHREDPACKET);
	//红包说明
	auto message = (ImageView*)redPacketLayer->findControl(FIND_IMAGEVIEW_BY_NAME_REDPACKETMESSAGE);
	if (message)
	{
		message->setTouchEnabled(true);
		message->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneREDPACKETMESSAGETime", [=]() {
				redPacketLayer->removeFromParentAndCleanup(true);
				map<string, int64String> paramter;
				YYXLayer::insertMap4ParaType(paramter, "className", -999, "BookInfo");
				YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_REDPACKETMESSAGE_CSB);
				auto messageLayer = YYXLayer::create(paramter);
				auto mypacket = (ImageView*)messageLayer->findControl(FIND_IMAGEVIEW_BY_NAME_REDPACKETMESSAGE_MYPACKET);
				auto backbutton = (ImageView*)messageLayer->findControl(FIND_IMAGEVIEW_BY_NAME_REDPACKETMESSAGE_BACK);
				auto listeners = [=](Ref* sender) {
					messageLayer->removeFromParentAndCleanup(true);
					selectOrShowRedPackets();
				};
				mypacket->setTouchEnabled(true);
				mypacket->addClickEventListener(listeners);
				backbutton->setTouchEnabled(true);
				backbutton->addClickEventListener(listeners);
				messageLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
				Size visibleSize = Director::getInstance()->getVisibleSize();
				messageLayer->setPosition(visibleSize / 2);
				addChild(messageLayer);
			});
		});
	}
	//返回上级
	auto listerfunction = [=](Ref* sneder) {
		redPacketLayer->removeFromParentAndCleanup(true);
		showPay();
	};
	if (myredpacket && backbutton)
	{
		backbutton->setTouchEnabled(true);
		myredpacket->setTouchEnabled(true);
		backbutton->addClickEventListener(listerfunction);
		myredpacket->addClickEventListener(listerfunction);
	}
	if (refer)
	{
		refer->setTouchEnabled(true);
		refer->addClickEventListener([=](Ref* send) {
			YYXLayer::controlTouchTime(1, "bookInfoSceneSelectRedPacketReferRedPacketListviewTime",[=]() {
				//NetIntface::httpGetUserRedPackets(App::GetInstance()->m_me->id, "httpGetUserRedPackets_Success", "httpGetUserRedPackets_Fail");//请求红包列表
				httpGetUserRedPackets();
			});
		});
	}
	redPacketLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	redPacketLayer->setPosition(Director::getInstance()->getVisibleSize() / 2);
	if (without)
	{
		int redPacketPrice = YYXStruct::getMapInt64(App::GetInstance()->myData, StringUtils::format("bookId=%d+redpacket", m_bookId), -999);
		if (redPacketPrice == 0)
		{
			without->setTag(0);//不使用红包状态
			without->loadTexture(IMAGE_DONOTWITHREDPACKET, TextureResType::PLIST);
		}
		else
		{
			without->setTag(1);
		}
		without->setTouchEnabled(true);
		without->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "bookInfoSceneSelectRedPacketBUSHIYONGHONGBAOTime", [=]() {
				if (without->getTag() == 1)
				{
					//0不使用红包
					without->setTag(0);
					without->loadTexture(IMAGE_DONOTWITHREDPACKET, TextureResType::PLIST);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, StringUtils::format("bookId=%d+redpacket", m_bookId), 0, "", (Ref*)-1);
					Director::getInstance()->getEventDispatcher()->setEnabled(true);
					EventCustom event("BookInfoSceneSelectRedPacket_CHANGE");
					event.setUserData((void*)-999);
					Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
					YYXLayer::sendNotify("removeBookInfoSceneSelectRedPacket", "bookinfo");
				}
				else
				{
					//1使用红包
					without->setTag(1);
					without->loadTexture(IMAGE_WITHREDPACKET, TextureResType::PLIST);
				}
			});
		});
	}
	auto eventListene = EventListenerCustom::create("referRedPackets", [=](EventCustom* e) {
		//展示红包
		listViewShowRedPacket(listview);
		Toast::create(App::getString("SHUAXINHONGBAO"));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListene, redPacketLayer);
	listViewShowRedPacket(listview);
	//选择完成红包,通知返回上级
	auto evenet = EventListenerCustom::create("removeBookInfoSceneSelectRedPacket", [=](EventCustom* e) {
		Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(redPacketLayer, true);
		redPacketLayer->runAction(Sequence:: create(ScaleTo::create(0.3, 0), CallFuncN::create([=](Ref* d) {
			redPacketLayer->removeFromParentAndCleanup(true);
			auto layer = showPay(true);
			if (layer) 
			{
				layer->setScale(0);
				layer->runAction(ScaleTo::create(0.3, 1));
			}
		}),NULL));		
	});	
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(evenet, redPacketLayer);	
	addChild(redPacketLayer);
}

//展示红包
void BookInfo::listViewShowRedPacket(ListView* listview)
{
	if (!listview)
	{
		App::log("listview is null");
		return;
	}
	listview->removeAllItems();
	auto data = App::GetInstance()->m_redPacket;
	for (int i = 0; i < data.size(); i++)
	{
		Layout* layout = Layout::create();
		map<string, int64String> paramter;
		YYXLayer::insertMap4ParaType(paramter, "className", -999, "listviewItem");
		YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_SELECTREDPACKET_ITEM_CSB);
		auto item = YYXLayer::create(paramter);
		auto leftRedPacket = (Node*)item->findControl(PAY_SELECTREDPACKET_ITEM_LEFT);
		auto rightRedPacket = (Node*)item->findControl(PAY_SELECTREDPACKET_ITEM_RIGHT);
		loadRedPacketData(leftRedPacket,i);
		loadRedPacketData(rightRedPacket,i+1);
		item->getParentNode()->setAnchorPoint(Vec2(0, 0));
		if (i+1 >= data.size())
			rightRedPacket->setVisible(false);
		layout->addChild(item);
		layout->setContentSize(Size(500,200));
		listview->pushBackCustomItem(layout);
		i++;
	}
	listview->jumpToTop();	
}

//加载红包数据到节点上
void BookInfo::loadRedPacketData(Node* node, int index)
{
	bool show = true;
	auto data = App::GetInstance()->m_redPacket;
	if (index >= data.size())
		return;
	int coupon_id = YYXStruct::getMapInt64(data[index], "coupon_id", -999);
	int coupon_amount = YYXStruct::getMapInt64(data[index], "coupon_amount", -999);
	auto coupon_expire_time = YYXStruct::getMapString(data[index], "coupon_expire_time", "");
	
	if (coupon_id == -999 || coupon_amount == -999 || coupon_expire_time == "")
		show = false;
	if (node)
		node->setVisible(show);
	else
		return;
	if (show)
	{
		//有效期
		auto text_coupon_expire_time = (Text*)node->getChildByName(FIND_TEXT_BY_NAME_EVERYREDPACKET_TIME);
		if (text_coupon_expire_time)
		{
			coupon_expire_time = coupon_expire_time.substr(0,10);
			text_coupon_expire_time->setText(App::getString("YOUXIAOQIZHI") + coupon_expire_time);
		}
		//红包价值
		auto text_coupon_amount = (Text*)node->getChildByName(FIND_TEXT_BY_NAME_EVERYREDPACKET_PRICE);
		if (text_coupon_amount)
			text_coupon_amount->setText(StringUtils::format("%d", (int)(coupon_amount / 100)) + App::getString("YUAN"));
		//打钩
		auto img_couponSelect = (ImageView*)node->getChildByName(FIND_IMAGEVIE_BY_NAME_EVERYREDPACKET_SELECT);
		img_couponSelect->setTag(coupon_id);
		auto optimalID =(int) YYXStruct::getMapRef(App::GetInstance()->myData, StringUtils::format("bookId=%d+redpacket",m_bookId), (Ref*)0);
		if (img_couponSelect && coupon_id == optimalID)
			img_couponSelect->setVisible(true);
		else
			img_couponSelect->setVisible(false);
		//红包封面
		auto redPacket = (ImageView*)node->getChildByName(FIND_IMAGEVIE_BY_NAME_EVERYREDPACKET_COVER);
		redPacket->setTag(coupon_id);
		if (redPacket)
		{
			bool click = true;
			redPacket->setTouchEnabled(true);
			redPacket->setSwallowTouches(false);
			/*auto redListener = EventListenerTouchOneByOne::create();
			redListener->onTouchBegan = [&](Touch* t , Event* e) {
				click = true;
				return true;
			};
			redListener->onTouchMoved = [&](Touch* t, Event* e) {		
				auto move = t->getStartLocation() - t->getLocation();
				if (move.x > 30 || move.y > 30 || move.x < -30 || move.y < -30)
				{
					click = false;
					App::log("click = false");
				}
			};
			redListener->onTouchEnded = [&](Touch* t, Event* e) {
			};
			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(redListener->clone(), redPacket);*/
			redPacket->addClickEventListener([=](Ref* sender) {
				App::log("redPacket->addClickEventListener");
				auto click = YYXStruct::getMapInt64(m_tag, "touchismove", -999);
				if (click == 1)
					return;
				YYXLayer::controlTouchTime(1, "BookInfoSceneClickRedPacketTime", [=]() {
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, StringUtils::format("bookId=%d+redpacket", m_bookId), coupon_amount, "", (Ref*)coupon_id);
					Director::getInstance()->getEventDispatcher()->setEnabled(true);
					EventCustom event("BookInfoSceneSelectRedPacket_CHANGE");
					event.setUserData((void*)coupon_id);
					Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
					YYXLayer::sendNotify("removeBookInfoSceneSelectRedPacket", "bookinfo");
				});
			});
		}
		auto evenet = EventListenerCustom::create("BookInfoSceneSelectRedPacket_CHANGE", [=](EventCustom* e) {
			int coupon_id = (int)e->getUserData();
			int tag = img_couponSelect->getTag();
			if(coupon_id == tag)
				img_couponSelect->setVisible(true);
			else
				img_couponSelect->setVisible(false);
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(evenet, node);
	}
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
void BookInfo::hint()
{
	App::log("BookInfo::hint");
	//判断是否需要显示
	//账号登录 + 有红包  +  未购买的书 + 不免费
	auto memberId = -999;
	if (App::GetInstance()->m_me)
	{
		memberId = App::GetInstance()->m_me->id;
		if (App::GetInstance()->m_me->vip)
			return;
	}
	if (memberId == -999)
	{
		App::log("红包抵扣提示memberId=", memberId);
		return;
	}
	if (App::GetInstance()->m_redPacket.size() <= 0)
	{
		App::log("红包抵扣提示App::GetInstance()->m_redPacket.size()=", App::GetInstance()->m_redPacket.size());
		return;
	}
	if (isBuy)
	{
		App::log("红包抵扣提示isBuy= true");
		return;
	}
	if (m_Selling100 == 0)
	{
		App::log("红包抵扣提示m_Selling100=", m_Selling100);
		return;
	}
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "BookInfoSceneHint");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, BOOKINFO_HINT_CSB);
	YYXLayer::insertMap4ParaType(paramter, "csbTimeLine", 160, "");
	YYXLayer::insertMap4ParaType(paramter, "gotoFrameAndPlayIsLoop", 0, "");
	auto hintNode = YYXLayer::create(paramter, map<string, function<void(YYXLayer*)>>());
	hintNode->getParentNode()->setAnchorPoint(Vec2(1, 0));
	auto winsize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	hintNode->setPosition(origin + Vec2(winsize.width, 97));
	auto imageview = (ImageView*)hintNode->findControl(BOOKINFO_FIND_IMAGEVIEW_BYNAME_HINTTEXT);
	auto text = (Text*)Helper::seekWidgetByName(imageview, BOOKINFO_FIND_TEXT_BYNAME_HINTTEXT);
	float showred = getOptimalRedPacket() /100.0;
	text->setText(StringUtils::format("%s%.00f%s", App::getString("HONGBAODIKOU"),  showred, App::getString("YUAN")));
	text->setFontSize(66);
	hintNode->setScale(0.5);
	auto listener1 = EventListenerCustom::create("refershRedPacketCount", [=](Ref* sender) {
		float showred = getOptimalRedPacket() / 100.0;
		text->setText(StringUtils::format("%s%.00f%s", App::getString("HONGBAODIKOU"), showred, App::getString("YUAN")));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener1, hintNode);
	auto listener2 = EventListenerCustom::create("bookInfoRemoveHint", [=](Ref* sender) {
		hintNode->removeFromParentAndCleanup(true);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener2, hintNode);
	addChild(hintNode, 1);
	App::log("BookInfo::hint--END");
}

bool BookInfo::isBuyThisBook()
{
	App::log("BookInfo::isBuyThisBook");
	if (App::GetInstance()->m_me)
	{
		if (App::GetInstance()->m_me->vip && IsRentBook())
		{
			//你是vip 同时必须这书属于租书范围
			return true;
		}
		else 
		{
			if (App::GetInstance()->myBuyBookMap.find(m_bookId) != App::GetInstance()->myBuyBookMap.end())
				return true;
		}
	}
	return false;
}

void BookInfo::initVipText(Node* node)
{	
	auto viphintText = (Text*)node->getChildByName("Text_1");
	auto bg = (Sprite*)node->getChildByName("Sprite_1");
	auto tian = (Text*)node->getChildByName("Text_2");
	auto baiwei = (Text*)node->getChildByName("Text_3");
	auto shiwei = (Text*)node->getChildByName("Text_3_0");
	auto gewei = (Text*)node->getChildByName("Text_3_1");
	auto listener = [=]() {
		viphintText->setVisible(false);
		bg->setVisible(false);
		tian->setVisible(false);
		baiwei->setVisible(false);
		shiwei->setVisible(false);
		gewei->setVisible(false);
		if (!IsRentBook())
		{
			return;
		}		
		if (App::GetInstance()->m_me && App::GetInstance()->m_me->vip)
		{//vip
			string path = App::getBookRead4Json_txtPath(m_bookId);
			if (FileUtils::getInstance()->isFileExist(path))
			{//下载
				if (App::GetInstance()->myBuyBookMap.find(m_bookId) == App::GetInstance()->myBuyBookMap.end())
				{
					viphintText->setVisible(true);
					bg->setVisible(true);
					tian->setVisible(true);
					baiwei->setVisible(true);
					shiwei->setVisible(true);
					gewei->setVisible(true);
					viphintText->setText(App::getString("NINDEVIPZHANGHAOKEYIYUEDUNTIAN"));
					auto vt = App::GetInstance()->m_me->vipTime / 86400;
					int bw = vt / 100;
					int shiw = vt % 100 / 10;
					int gew = vt % 10;
					baiwei->setText(StringUtils::format("%d", bw));
					shiwei->setText(StringUtils::format("%d", shiw));
					gewei->setText(StringUtils::format("%d", gew));
					App::log("           -----------------------------        App::GetInstance()->m_me->vipTime = ", App::GetInstance()->m_me->vipTime);
				}
			}
			else
			{//未下载
				//viphintText->setText(App::getString("NINSHIVIPYONGHUZHIJIEKEYIXIAZAI"));
			}
		}
		else
		{//非vip
			//if (!isBuy)
				//viphintText->setText(App::getString("SHENGJIVIPLIJISHENGJI"));
			//else
				//viphintText->setVisible(false);
		}
	};
	listener();
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("refershVIPText", [=](EventCustom* e) {
		listener();
	});
}

//判断是否属于租书范围
bool BookInfo::IsRentBook()
{
	//购书标记+下载URL + 	是否是vip书籍
	string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", m_bookId);
	int zushu = (int)YYXStruct::getMapRef(App::GetInstance()->myData, BookPlayUrlKey, (Ref*)-1);
	if (zushu == 1)
		return true;
	else
		return false;
}

//网络请求租书, 租书成功方可下载
void BookInfo::httpRentBook(function<void()> runable)
{
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "rentbookhttp");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "BookInfo/csb/rentbook.csb");
	auto layer = YYXLayer::create(paramter);
	auto yes = (Button*)layer->findControl("Button_2");
	auto close = (Button*)layer->findControl("Button_1");
	auto bookname = (Text*)layer->findControl("Text_6");
	auto time = (Text*)layer->findControl("Text_6_0");
	if (bookname)
	{
		bookname->setText(m_BookName);
	}
	if (time)
	{
		int tday = App::GetInstance()->m_me->vipTime / 86400;
		time->setText(StringUtils::format("%d%s", tday,App::getString("STR_BOOINFODAY")));
	}
	if (yes)
	{
		yes->addClickEventListener([=](Ref* send) {
			layer->removeFromParent();
			string url = string(IP).append(NET_RENT);
			map<string, string> par;
			par["memberId"] = App::getMemberID();
			par["bookId"] = StringUtils::format("%d", m_bookId);
			par["resource"] = App::m_resource;
			par["payType"] = "2";
			NetIntface::httpPost(url, par, "rentSuccess", [=](string json) {
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
					if (runable)
						runable();
				}
				else
					Toast::create(App::getString("ZUSHUSHIBAI"));
			}, "rentFail", [](string str) {
				Toast::create(App::getString("NETEXCEPTION"));
			});
		});
	}
	if (close)
	{
		close->addClickEventListener([=](Ref* sender) {
			layer->removeFromParent();
		});
	}
	layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	layer->getParentNode()->setPosition(Director::getInstance()->getWinSize() / 2);
	addChild(layer);	
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
			YYXLayer::controlTouchTime(0.3, "BookProgressingTime", [=]() {
				YYXLayer::sendNotify(taskTag + "_BookProgressing", "", data.intData);
			});
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
			NetIntface::AddDownLoadRecord(App::GetInstance()->m_me->id, bookId, "", [](string str) {}, "", [](string str) {});
			//下载成功,解压									
			std::thread(&YYXDownload::uncompress, App::getReadZipDir(), StringUtils::format("%d", bookid), App::getReadDir(), [=](string zipPath) {
				YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 100);
				YYXLayer::sendNotify("refershVIPText");				
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
	App::addRecordBookDownload(bookId);
	return bookTag;
}

//完整书籍下载进度的监听添加, 绑定书籍和任务
void BookInfo::downloadingListener(int bookId, string bookTag, Button* m_ReadButton)
{
	//书籍信息 原价+书籍名称+ 书页数
	string BookNameKey = StringUtils::format("bookName+bookID=%d", bookId);
	string taskBookName = YYXStruct::getMapString(App::GetInstance()->myData, BookNameKey, "");
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookTag, bookId, "<<" + taskBookName + ">>");
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(bookTag + "_BookProgressing");
	auto listenerButton = EventListenerCustom::create(bookTag + "_BookProgressing", [=](EventCustom* e) {
		auto progressing = (long)e->getUserData();
		if (progressing < 0)
			return;
		m_ReadButton->setTitleFontSize(36);
		if (progressing <= 99)
			m_ReadButton->setTitleText(StringUtils::format("%d%%", progressing));
		else if (progressing == 100)
			m_ReadButton->setTitleText(App::getString("STR_READ"));
		else if (progressing == 999)
			m_ReadButton->setTitleText(App::getString("STR_DOWNLOAD"));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerButton, m_ReadButton);
}