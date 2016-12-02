#include "BookRoomScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "YYXFunctionQueue.h"

USING_NS_CC;
using namespace cocostudio::timeline;
using namespace std;


BookRoom::BookRoom() {
	YYXLayer::logb("BookRoom::BookRoom()");
	YYXLayer::loge("BookRoom::BookRoom()");
}

BookRoom::~BookRoom() {
	YYXLayer::logb("BookRoom::~BookRoom()");
	YYXLayer::loge("BookRoom::~BookRoom()");
}

Scene* BookRoom::createScene()
{
    auto scene = Scene::create();
    auto layer = BookRoom::create();
    scene->addChild(layer);
    return scene;
}

bool BookRoom::init()
{
	YYXLayer::logb("BookRoom::init()");
    if ( !Layer::init() )
    {
        return false;
    }
	//数据
	loadTheDataInTheMemory();
	m_offline = !NetIntface::IsNetConnect();
	YYXFunctionQueue::GetInstance()->clear();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	App::m_RunningScene = MySceneName::BookRoomScene;
	Layer* layer;
	Data data;
	if (App::GetInstance()->getData(BOOKROOM_CSB, data))
		layer = (Layer*)CSLoader::createNode(data);
	else
		layer = (Layer*)CSLoader::createNode(BOOKROOM_CSB);
	layer->setAnchorPoint(Vec2(0.5f, 0.5f));
	layer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(layer);
	m_currentPageNumber = 0;
	m_currentPageNumber = App::GetInstance()->m_showSceneData.intData;
	string bookmode = App::GetInstance()->m_showSceneData.stringData;
	bookMode = atoi(bookmode.c_str());
	if (bookMode == 0 || bookMode == 2 || bookMode == 3 || bookMode == 4)
		App::log("                             bookmode= " + bookmode);
	else
		bookMode = 0;
	if (m_currentPageNumber < 0 || m_currentPageNumber >1000)
		m_currentPageNumber = 0;
	//1.展示白天或者黑夜
	if (!App::isNight()) {
		auto sprNight = (Sprite*)layer->getChildByName(FIND_SPRITE_BY_NAME_NIGHT_PIC);
		sprNight->setVisible(false);
	}
	auto name = Label::create();
	name->setPosition(Vec2(550,550));
	name->setTTFConfig(App::m_ttf);
	addChild(name, 1);
	//Director::getInstance()->getEventDispatcher()->addCustomEventListener("showName", [=](EventCustom* e) {
	//	int st = (int)e->getUserData();
	//	switch (st)
	//	{
	//	case 1:
	//		name->setString(App::getString("STR_BUY"));
	//		break;
	//	case 2:
	//		name->setString(App::getString("STR_DOWNLOAD"));
	//		break;
	//	case 3:
	//		name->setString(App::getString("STR_READ"));
	//		break;
	//	case 4:
	//		name->setString(App::getString("VIPXIAZAI"));
	//		break;
	//	default:
	//		break;
	//	}		
	//});
	//2.无登录
	//if (!App::GetInstance()->m_me) 
		//return true;
	//3.音乐
	if (YYXLayer::getBoolFromXML(MUSIC_KEY))
	{
		if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
			SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
		else
			SimpleAudioEngine::getInstance()->playBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY, true);
	}
	else
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	//左边树枝
	auto lefttree = (Sprite*)layer->getChildByName(FIND_SPRITE_BY_NAME_LEFT_TREE);
	lefttree->setPositionX((1094 - visibleSize.width) / 2);
	//右边树枝
	auto righttree = (Sprite*)layer->getChildByName(FIND_SPRITE_BY_NAME_RIGHT_TREE);
	righttree->setPositionX((1094 + visibleSize.width) / 2);
	//树杈
	auto tree = (Sprite*)layer->getChildByName(FIND_SPRITE_BY_NAME_BRANCH);
	tree->setPositionX((1094 + visibleSize.width) / 2);
	//咿啦
	auto ellaBird = ( ImageView*)tree->getChildByTag(34);
	ellaBird->setTouchEnabled(true);
	ellaBird->addClickEventListener([=](Ref* sender) {
		if (!m_click)
			return;	
		if (App::m_debug == 0)
		{
			App::runTestFunction();
		}
	});
	//页数圆圈节点
	pageCircle = Node::create();
	pageCircle->setPosition(Vec2(0, 0));
	addChild(pageCircle, 1);
	//翻页圆圈
	moveCircle = Label::create(App::getString("CIRCLE"), "wryh.TTF", 65);
	moveCircle->setScale(0.5f);
	moveCircle->setPosition(Vec2(-100, -100));
	addChild(moveCircle, 1);
	//6本书
	auto shang3ben = layer->getChildByName("Node_1");
	auto xia3ben = layer->getChildByName("Node_2");
	if (shang3ben)
	{
		Node* book0 = shang3ben->getChildByName("book0");
		Node* book1 = shang3ben->getChildByName("book1");
		Node* book2 = shang3ben->getChildByName("book2");
		books.push_back(book0);
		books.push_back(book1);
		books.push_back(book2);
	}
	if (xia3ben)
	{
		Node* book3 = xia3ben->getChildByName("book3");
		Node* book4 = xia3ben->getChildByName("book4");
		Node* book5 = xia3ben->getChildByName("book5");
		books.push_back(book3);
		books.push_back(book4);
		books.push_back(book5);
	}	
	//书籍
	refershPage(bookMode);
	//滑动控制
	auto touchmove = EventListenerTouchOneByOne::create();
	touchmove->onTouchBegan = [=](Touch* t, Event* e) {
		//App::log("BookRoom touchmove");
		m_click = true;
		return true;
	};
	touchmove->onTouchMoved = [=](Touch* t, Event* e) {
		//App::log("BookRoom touchmove onTouchMoved");
		auto moveLocation = t->getStartLocation() - t->getLocation();
		if (moveLocation.x > 30 || moveLocation.x < -30 || moveLocation.y > 30 || moveLocation.y < -30)
		{//滑动
			m_click = false;
			unschedule("longAnEditMode");
			unschedule("UNEditModeTime");
		}
	};
	touchmove->onTouchEnded = [=](Touch* t, Event* e) {
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchmove, -100);

	auto touchlistener = EventListenerTouchOneByOne::create();
	touchlistener->onTouchBegan = [=](Touch* t, Event* e) {
		//App::log("BookRoom touchlistener");
		App::GetInstance()->addTime("longAnEditModeTime", NetIntface::getMillsTime());
		if (m_compile)
		{
			schedule([=](float f) {
				auto t1 = App::GetInstance()->getTime("longAnEditModeTime", NetIntface::getMillsTime());
				auto nowt = NetIntface::getMillsTime();
				if (nowt > (t1 + 500))
				{
					m_compile = false;
					YYXLayer::sendNotify("bookRoomSceneCompileChange", "", 0);
					unschedule("UNEditModeTime");
				}
			}, 0.5f, "UNEditModeTime");
		}
		return true;
	};
	touchlistener->onTouchMoved = [=](Touch* t, Event* e) {
		//App::log("BookStore touchlistener onTouchMoved");
		auto moveLocation = t->getStartLocation() - t->getLocation();		
		auto key = "bookRoomMoveTime";
		if (moveLocation.x > 160 || moveLocation.y < -80)
		{				//向右
			YYXLayer::controlTouchTime(2, key, [=]() {
				++m_currentPageNumber;
				RollingAnimation(shang3ben, xia3ben, [=]() {
					refershPage(bookMode);
				});
			});
		}
		else if (moveLocation.x < -160 || moveLocation.y > 80)
		{				//向左
			YYXLayer::controlTouchTime(2, key, [=]() {
				--m_currentPageNumber;
				RollingAnimation(shang3ben, xia3ben, [=]() {
					refershPage(bookMode);
				});
			});
		}
	};
	touchlistener->onTouchEnded = [=](Touch* t, Event* e) {
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchlistener, 1);
	//页码
	showBookPageNumber();
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("bookRoomSceneHttpGetUserBuyBooks_Success", [=](EventCustom *e) {
		showBookPageNumber();
	});
	//全部图书
	auto allBookButton = (ImageView*)layer->getChildByName("Image_14");
	//vip图书
	auto vipBookButton = (ImageView*)layer->getChildByName("Image_14_0");
	//最近下载
	auto downloadBookButton = (ImageView*)layer->getChildByName("Image_14_1");
	//最近阅读
	auto readBookButton = (ImageView*)layer->getChildByName("Image_14_2");
	if (allBookButton)
	{
		allBookButton->setTouchEnabled(true);
		allBookButton->addClickEventListener([=](Ref* sbeder) {
			bookMode = 0;
			showImageView(bookMode, allBookButton, vipBookButton, downloadBookButton, readBookButton);
			m_currentPageNumber = 0;
			refershPage(bookMode);
			showBookPageNumber();
		});
	}

	if (vipBookButton)
	{
		vipBookButton->setTouchEnabled(true);
		vipBookButton->addClickEventListener([=](Ref* sbeder) {
			bookMode = 4;
			showImageView(bookMode, allBookButton, vipBookButton, downloadBookButton, readBookButton);
			m_currentPageNumber = 0;
			refershPage(bookMode);
			showBookPageNumber();
		});
	}
	
	if (downloadBookButton)
	{
		downloadBookButton->setTouchEnabled(true);
		downloadBookButton->addClickEventListener([=](Ref* sbeder) {
			bookMode = 2;
			showImageView(bookMode, allBookButton, vipBookButton, downloadBookButton, readBookButton);
			m_currentPageNumber = 0;
			refershPage(bookMode);
			showBookPageNumber();
		});
	}
	
	if (readBookButton)
	{
		readBookButton->setTouchEnabled(true);
		readBookButton->addClickEventListener([=](Ref* sbeder) {
			bookMode = 3;
			showImageView(bookMode, allBookButton, vipBookButton, downloadBookButton, readBookButton);
			m_currentPageNumber = 0;
			refershPage(bookMode);
			showBookPageNumber();
		});
	}
	showImageView(bookMode, allBookButton, vipBookButton, downloadBookButton, readBookButton);
	//最上层返回
	auto homeButton = (Button*)layer->getChildByName(FIND_BUTTON_BY_NAME_BOOKROOM_BACK);
	homeButton->removeFromParent();
	addChild(homeButton);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		if (!m_click)
			return;
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		App::GetInstance()->popBack();
		Index::BackPreviousScene();
	});
	homeButton->setPosition(origin + Vec2(14, visibleSize.height - 14));
	initHttp();
	YYXLayer::loge("BookRoom::init()");
    return true;
}

void BookRoom::initHttp()
{
	if (App::GetInstance()->m_me != nullptr)
	{
		auto memberid = App::GetInstance()->m_me->id;
		if (memberid > 0)
			NetIntface::httpGetUserBuyBooks(memberid, "bookRoomSceneHttpGetUserBuyBooksSuccess", [=](string json) {
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
					//书籍信息 原价+书籍名称+ 书页数
					string BookNameKey = StringUtils::format("bookName+bookID=%d", bookId);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookNameKey, 18, bookName, (Ref*)8);
				}
				//封面下载
				string fileName = StringUtils::format("%d", bookId) + ".png";
				if (!FileUtils::getInstance()->isFileExist(App::getBookCoverPngPath(bookId)))
				{
					string DownLoadImageRunKey = "BookRoomSceneDownLoadImageSuccess";
					string DownLoadImageErrorKey = "BookRoomSceneDownLoadImageFail";
					NetIntface::DownLoadImage(bookCoverUrl, App::getCoverDir(), fileName, DownLoadImageRunKey, [=](string downPath) {
						YYXLayer::sendNotify("bookRoomCoverDownloadSuccess");
					}, DownLoadImageErrorKey, [=](string str) {
						string sstr = string("<<" + bookName + ">>").append(App::getString("FENGMIANXIAZAISHIBAI"));
						Toast::create(sstr.c_str(), false);
					});
				}
			}, [memberid]() {
				//解析成功
				YYXLayer::buyBooksJsonWriteFile(App::GetInstance()->myBuyBookVector, memberid, FileUtils::getInstance()->getWritablePath() + "books.json");
				YYXLayer::sendNotify("bookRoomSceneHttpGetUserBuyBooks_Success");
			}, []() {
				//解析错误
				Toast::create(App::getString("YIGOUSHUJILIEBIAOGENGXINSHIBAI"));
			});
		}, "LoginSceneHttpGetUserBuyBooksFail", [](string str) {
			//网络错误
			Toast::create(App::getString("YIGOUSHUJILIEBIAOGENGXINSHIBAI"));
		});
	}
}

//内存中加载数据
void BookRoom::loadTheDataInTheMemory()
{
	if (!App::GetInstance()->m_me)
	{
		App::GetInstance()->myBuyBookMap.clear();
		App::GetInstance()->myBuyBookVector.clear();
		return;
	}
	string bookjsonPath = FileUtils::getInstance()->getWritablePath() + "books.json";
	auto mapc = App::GetInstance()->myBuyBookMap.size();
	auto vectorc = App::GetInstance()->myBuyBookVector.size();
	if (mapc == vectorc && mapc > 0)
	{
		return;
	}
	//内存中数据可能不足
	if (!FileUtils::getInstance()->isFileExist(bookjsonPath))
		return;
	string jsonstr = FileUtils::getInstance()->getStringFromFile(bookjsonPath);
	rapidjson::Document doc;
	bool result = YYXLayer::getJsonObject4Json(doc, jsonstr);
	if (result)
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
}

//刷新当前页书籍
void BookRoom::refershPage(int status)
{	
	vector<int> bookidData;
	if (status == 0)//全部图书
	{
		bookidData = getCurrentPageBookID();
	}
	else if (status == 2)//最近下载
	{
		bookidData = getCurrentPageBookID4DownLoad();
	}
	else if (status == 3)//最近阅读
	{
		bookidData = getCurrentPageBookID4Read();
	}
	else if (status == 4)//租书
	{
		bookidData = getCurrentPageBookID4Rent();
	}
	//YYXLayer::sendNotify("showName", "", status);
	for (int i = 0; i < 6; i++)
	{
		if (i < bookidData.size())
		{
			refershBookNode(books[i], bookidData[i]);
		}
		else
		{
			refershBookNode(books[i], -999);
		}
	}
}

//刷新book节点信息
void BookRoom::refershBookNode(Node* book, int bookid)
{
	book->setVisible(true);
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(book);
	if (bookid < 0)
	{
		book->setVisible(false);
		return;
	}	
	//离线模式
	if (m_offline && !FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
	{
		book->setVisible(false);
		return;
	}
	//vip包年标签
	auto vip = (Sprite*)book->getChildByName("Sprite_1");
	if (vip)
	{
		vip->setVisible(false);
		string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", bookid);
		auto isvip = (int)YYXStruct::getMapRef(App::GetInstance()->myData, BookPlayUrlKey, nullptr);
		if (BookPlayUrlKey == "bookPlayUrl+bookID=455")
		{
			auto isvip = (int)YYXStruct::getMapRef(App::GetInstance()->myData, BookPlayUrlKey, nullptr);
			//App::log(" usvip=", isvip);
		}
		if (isvip == 1 && App::GetInstance()->m_me)
		{
			if (App::GetInstance()->m_me->vip)
			{
				if (App::GetInstance()->myBuyBookMap.find(bookid) == App::GetInstance()->myBuyBookMap.end())
					vip->setVisible(true);
			}
		}
	}
	//关闭
	auto close = (ImageView*)book->getChildByName("close");
	close->setSwallowTouches(true);
	//1.显示关闭按钮 必须是书籍存在 + 编辑状态
	//改变编辑状态 0=正常 1=编辑	
	auto listenerCompileChangeBook = EventListenerCustom::create("bookRoomSceneCompileChange", [=](EventCustom* e) {
		long s = (long)e->getUserData();
		if (s == 1 && FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
		{
			//编辑
			close->setVisible(true);
			//book->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.2f, -1), RotateTo::create(0.2f, 1), NULL)));
			book->setTag(1);
		}
		else
		{
			//App::log("*****************************222");
			close->setVisible(false);
			if (book->getTag() == 1) {
				//book->stopAllActions();
				//App::log("	book->stopAllActions();");
			}
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerCompileChangeBook, book);
	//初始化关闭
	if (m_compile && FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
	{
		close->setVisible(true);
		//book->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.2f, -1), RotateTo::create(0.2f, 1), NULL)));
		book->setTag(1);
	}
	else
	{
		if (book->getTag() == 1) {
			//book->stopAllActions();
			book->setTag(0);
			//App::log("	book->stopAllActions();");
		}
		close->setVisible(false);
	}
	close->setTouchEnabled(true);
	close->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "bookRoomSceneDeleteBookTime", [=]() {
			auto message = YYXLayer::MyMessageBox(App::getString("QUEDINGSHANCHU"), "", [=]() {
				YYXLayer::DeleteDirectory(App::getBookReadPath(bookid));
				refershBookNode(book, bookid);
				if (!FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
				{
					if (App::GetInstance()->myRentBookMap.find(bookid) != App::GetInstance()->myRentBookMap.end())
						App::deleteRentBook(bookid);
				}
			}, "", []() {});
			if (message)
				addChild(message, 1);
		});
	});
	//封面
	auto cover = (ImageView*)book->getChildByName("Image");
	cover->setTag(bookid);
	auto coverpath =App::getBookCoverPngPath(bookid);
	if (FileUtils::getInstance()->isFileExist(coverpath) && cover)
	{
		cover->loadTexture(coverpath);
	}
	else
	{
		cover->loadTexture("other/Book_cover.png");
	}
	auto listenerCoverBook = EventListenerCustom::create(StringUtils::format("bookRoomCoverDownloadSuccess"), [=](EventCustom* e) {
		if (FileUtils::getInstance()->isFileExist(coverpath) && cover)
		{
			cover->loadTexture(coverpath);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerCoverBook, book);

	//提示下载
	auto download = (Sprite*)book->getChildByName("Download");
	//文字背景
	auto textbg = (Sprite*)book->getChildByName("DownloadBackground");
	//文字
	auto text = (Text*)book->getChildByName("Text");
	string zippath = App::getBookReadZipPath(bookid);
	string taskTag = YYXDownload::getTag(zippath);
	if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
	{
		if (download)
			download->setVisible(false);
		if (textbg)
			textbg->setVisible(false);
		if (text)
			text->setVisible(false);
	}
	else {		
		if (taskTag != "")
		{
			//判断是否在下载队列 准备队列 暂停  下完删除了
			if (YYXDownload::GetInstance()->isExitDownloadQueue(taskTag) || YYXDownload::GetInstance()->isExitReadyQueue(taskTag))
			{
				showStauts(0, download, textbg, text);
				auto listenerProgressing = EventListenerCustom::create(taskTag + "_BookProgressing", [=](EventCustom* e) {
					long p = (long)e->getUserData();
					showStauts(p, download, textbg, text);
					if (p == 100)
					{
						refershBookNode(book, bookid);
					}
				});
				Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerProgressing, book);
			}
			else if (YYXDownload::GetInstance()->isPause(taskTag))
				showStauts(8000, download, textbg, text);
			else
			{
				showStauts(9000, download, textbg, text);
			}
		}
		else
			showStauts(9000, download, textbg, text);
	}
	bookClick(book, bookid);
	bookLongClick(book, bookid);
}

//长按
void BookRoom::bookLongClick(Node* book, int bookid)
{
	//封面
	auto cover = (ImageView*)book->getChildByName("Image");
	cover->setTouchEnabled(true);
	cover->setSwallowTouches(false);
	cover->addTouchEventListener(this, toucheventselector(BookRoom::LongClick));
}

void BookRoom::LongClick(Ref* sender, TouchEventType type)
{
	switch (type)
	{
	case TOUCH_EVENT_BEGAN:
		schedule([=](float f) {
			App::log("longAnEditMode");
			auto t1 = App::GetInstance()->getTime("longAnEditModeTime", NetIntface::getMillsTime());
			auto nowt = NetIntface::getMillsTime();
			if (nowt > (t1 + 1000))
			{
				m_compile = true;
				YYXLayer::sendNotify("bookRoomSceneCompileChange", "", 1);
				unschedule("longAnEditMode");
			}
		}, 0.2f, "longAnEditMode");
		break;
	case TOUCH_EVENT_MOVED:
		break;
	case TOUCH_EVENT_ENDED:
		unschedule("longAnEditMode");
		break;
	case TOUCH_EVENT_CANCELED:
		break;
	}
}

void BookRoom::bookClick(Node* book, int bookid)
{
	//提示下载
	auto download = (Sprite*)book->getChildByName("Download");
	//文字背景
	auto textbg = (Sprite*)book->getChildByName("DownloadBackground");
	//文字
	auto text = (Text*)book->getChildByName("Text");
	//封面
	auto cover = (ImageView*)book->getChildByName("Image");
	//封面点击
	cover->setTouchEnabled(true);
	cover->setSwallowTouches(false);
	cover->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "BookRoomSceneClickCoverTime", [=]() {		
			if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
			{
				if (!m_click || m_compile)
					return;
				//记录场景
				string bookmode = StringUtils::format("%d", bookMode);
				App::GetInstance()->pushScene(BookRoomScene, m_currentPageNumber, bookmode);
				//暂停背景音乐
				SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
				//打开试读书本
				Director::getInstance()->replaceScene(Waiting::createScene(StringUtils::format("%d", bookid), false));
				//试读不需要上传阅读记录
			}
			else
			{
				//不存在 
				//下载的时候, 点击暂停下载
				string zippath = App::getBookReadZipPath(bookid);
				string taskTag = YYXDownload::getTag(zippath);
				if (taskTag != "" && !YYXDownload::GetInstance()->isEnd(taskTag) && !YYXDownload::GetInstance()->isPause(taskTag))
				{
					Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(taskTag + "_BookProgressing");
					YYXDownload::GetInstance()->pause(taskTag);
					showStauts(8000, download, textbg, text);
				}
				else
				{			//未下载的情况, 点击下载
					auto pairData = App::GetInstance()->myBuyBookMap.find(bookid);
					//App::log("             App::GetInstance()->myBuyBookMap.find(bookid);", bookid);
					if (pairData != App::GetInstance()->myBuyBookMap.end())
					{
						string url = pairData->second;
						//App::log("             App::GetInstance()->myBuyBookMap.find(bookid);"+ url, bookid);
						if (url == "")
						{
							Toast::create(App::getString("WEIZHAODAOXIAZAIDIZHI"), false);
						}
						else if (url != ""  && App::getNetSetAndHintSet())
						{
							string beginKey = "bookRoomSceneDownLoadBookBegin";
							string progressKey = "bookRoomSceneDownLoadBookProgress";
							string endKey = "bookRoomSceneDownLoadBookEnd";
							string bookTag = YYXDownload::GetInstance()->download(url, App::getReadZipDir(), StringUtils::format("%d.zip", bookid), beginKey, [=](YYXStruct data)
							{
								//auto taskTag = YYXStruct::getStringData(data, "");
								//if (!YYXDownload::GetInstance()->isPause(taskTag))
								//{
								//	App::downloadFailDeleteFiles(App::getBookReadZipPath(bookid), App::getBookReadPath(bookid));
								//}
							}, progressKey, [=](YYXStruct data)
							{
								auto taskTag = YYXStruct::getStringData(data, "");
								/*//查看线程
								auto it = this_thread::get_id();
								if (ida.find(it) != ida.end())
								{
									YYXLayer::controlTouchTime(0.2, "fdsafasfsdafsdafasdfsaf", [=]() {
										string str = StringUtils::format("   taskName=%s     threadName = %d, jindu = %d, threadCount = %d/n", ida[it].stringData.c_str(), (int)ida[it].intData, (int)data.intData, (int)ida.size());
										App::log(str);
										YYXLayer::writeFilepp(str, NetIntface::getDownloadDir() + "/log.txt");
									});
								}
								else
								{
									ida[it] = YYXStruct(YYXLayer::getRandom(), taskTag, 0);
								}*/
								if (YYXDownload::GetInstance()->isPause(taskTag))
									return;
								if (data.intData <= 0)
									return;
								if (data.intData <= 99)
								{
									YYXLayer::controlTouchTime(0.3, "BookProgressingTime", [=]()
									{
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
									NetIntface::AddDownLoadRecord(App::GetInstance()->m_me->id, bookid, "", [](string str) {}, "", [](string str) {});
									//下载成功,解压
									std::thread(&YYXDownload::uncompress, App::getReadZipDir(), StringUtils::format("%d", bookid), App::getReadDir(), [=](string zipPath) {
										YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 100);
									}, [=](string zipPath) {
										//解压失败
										if (!YYXDownload::GetInstance()->isPause(taskTag2))
										{
											YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 999);
											string bookNameError = bookName + App::getString("JIEYACUOWUCHANGXINXIAZAI") + StringUtils::format("%d", bookid);
											Toast::create(bookNameError.c_str());
											App::downloadFailDeleteFiles(App::getBookReadZipPath(bookid), App::getBookReadPath(bookid));
										}
										else
										{
											YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 8000);
										}
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
										App::downloadFailDeleteFiles(App::getBookReadZipPath(bookid), App::getBookReadPath(bookid));
									}
								}
							});
							App::addRecordBookDownload(bookid);
							//任务字符串绑定书名
							string BookNameKey = StringUtils::format("bookName+bookID=%d", bookid);
							//书籍信息 原价+书籍名称+ 书页数
							string taskBookName = YYXStruct::getMapString(App::GetInstance()->myData, BookNameKey, "");
							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookTag, bookid, "<<" + taskBookName + ">>");
							//加监听
							showStauts(0, download, textbg, text);
							auto listenerProgressing = EventListenerCustom::create(bookTag + "_BookProgressing", [=](EventCustom* e) {
								long p = (long)e->getUserData();
								showStauts(p, download, textbg, text);
								if (p == 100)
								{
									refershBookNode(book, bookid);
								}
							});
							Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerProgressing, book);
						}
					}
					else
					{
						if (NetIntface::IsNetConnect())
						{
							//记录场景
							string bookmode = StringUtils::format("%d", bookMode);
							App::GetInstance()->pushScene(BookRoomScene, m_currentPageNumber, bookmode);
							Index::GoToBookInfo(bookid);
						}
						else
							Toast::create(App::getString("ZHESHISHIDUSHUJI"));
					}
				}
			}
		}, []() { });
	});	
}

//下载状态展示
void BookRoom::showStauts(int stauts, Sprite* download, Sprite* textbg, Text* text)
{
	if (textbg)
		textbg->setVisible(true);
	if (text)
		text->setVisible(true);
	if (stauts < 0)//准备下载
	{
		if (download)
			download->setVisible(false);
		if (text)
			text->setText("0%");
	}
	else if (stauts <= 99)//下载中
	{
		if (download)
			download->setVisible(false);
		text->setText(StringUtils::format("%d%%", stauts));
	}
	else if (stauts == 100)//下载完成
	{
		if (download)
			download->setVisible(false);
		if (textbg)
			textbg->setVisible(false);
		if (text)
			text->setVisible(false);
	}
	else if (stauts == 9000 || stauts == 999)//未下载
	{
		if (download)
			download->setVisible(true);
		if (textbg)
			textbg->setVisible(false);
		if (text)
			text->setVisible(false);
	}
	else if (stauts == 8000)//暂停
	{
		if (download)
			download->setVisible(false);
		if (text)
			text->setText(App::getString("ZANTING"));
	}
}

//全部图书
//计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
vector<int> BookRoom::getCurrentPageBookID()
{
	vector<int> v_boosSort;
	map <int, int> alldata;
	//vip书+已购书
	for (auto it : App::GetInstance()->myRentBookMap)
	{
		alldata[it.first] = it.second;
		string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", it.first);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, -999, "", (Ref*)1);
	}
	for (auto it : App::GetInstance()->myBuyBookVector)
	{
		alldata[it] = 0;
	}
	if (m_offline)
	{
		vector<int> removebook;
		//离线的时候把未下载的书全部剔除
		for (auto it : alldata)
		{
			if (!FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(it.first)))
			{
				removebook.push_back(it.first);
			}
		}
		for (auto it : removebook)
		{
			alldata.erase(it);
		}
		//App::GetInstance()->myBuyBookMap.clear();
		//App::GetInstance()->myBuyBookVector.clear();
		//for (auto it : data)
		//{
		//	App::GetInstance()->myBuyBookVector.push_back(it);
		//	App::GetInstance()->myBuyBookMap[it] = "";
		//}
	}
	//auto booksum = App::GetInstance()->myBuyBookMap.size();
	auto sortDat = App::sortMapToVector(alldata);
	auto booksum = sortDat.size();
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "AllBookSum", booksum);
	int pagesum = ceil(booksum / 6.0);
	if (m_currentPageNumber < 0)
	{
		m_currentPageNumber = pagesum - 1;
	}
	else if (m_currentPageNumber > pagesum - 1)
	{
		m_currentPageNumber = 0;
	}
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = m_currentPageNumber * 6;
	end = m_currentPageNumber * 6 + 5;
	if (end >= booksum)
		end = booksum - 1;
	for (int i = begin; i <= end; i++)
	{
		//auto bookid = App::GetInstance()->myBuyBookVector[i];
		auto bookid = sortDat[i].first;
		v_boosSort.push_back(bookid);
	}
	return v_boosSort;
}

//滚动动画
void BookRoom::RollingAnimation(Node* node1, Node* node2,  function<void()> callback)
{
	auto ac = Sequence::create(MoveBy::create(0.5, Vec2(0, -185)), CallFuncN::create([=](Ref* sender) {
		if (callback)
			callback();
		showBookPageNumber();
	}), MoveBy::create(0.5, Vec2(0, 185)), NULL);
	node1->runAction(ac->clone());
	node2->runAction(ac);
}

//展示第1/5页
void BookRoom::showBookPageNumber()
{
	App::log("BookRoom::showBookPageNumber", bookMode);
	int booksum = 0;
	if (bookMode == 0)
	{
		booksum = YYXStruct::getMapInt64(App::GetInstance()->myData, "AllBookSum", 0);
		//booksum = App::GetInstance()->myBuyBookMap.size();
	}
	else if (bookMode == 2)
	{
		booksum = App::GetInstance()->bookDownLoad.size();
	}
	else if (bookMode == 3)
	{
		booksum = App::GetInstance()->bookRead.size();
	}
	else if (bookMode == 4)
	{
		booksum = App::GetInstance()->myRentBookMap.size();
	}
	int pagesum = ceil(booksum / 6.0);
	//创建页数圆圈
	if (pageCircle->getChildrenCount() > 0)
		pageCircle->removeAllChildrenWithCleanup(true);
	if (pagesum <= 0)
	{
		moveCircle->setPosition(Vec2(-1000, -1000));
		return;
	}
	//notify(REMOVEWAITLayer);
	auto size = Director::getInstance()->getVisibleSize();
	int startPosX = size.width / 2 - pagesum * 30 / 2;
	for (int i = 0; i < pagesum; i++) {
		auto pLabel = Label::create(App::getString("CIRCLE"), "wryh.TTF", 65);
		pLabel->setPosition(Vec2(startPosX + i * 30, 20));
		pLabel->setScale(0.5f);
		pLabel->setColor(ccc3(120, 120, 120));
		pageCircle->addChild(pLabel, 0, i);
	}
	auto p = pageCircle->getChildByTag(m_currentPageNumber);
	if(p)
		moveCircle->setPosition(p->getPosition());
	App::log("BookRoom::showBookPageNumber--- END", booksum);
}

void BookRoom::onEnterTransitionDidFinish()
{
	App::log("BookRoom::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	App::log("BookRoom::onEnterTransitionDidFinish---END");
}

//展示最近下载
//void BookRoom::showDownloadRecord()
//{
//	m_currentPageNumber = 0;
//	refershPage(2);
//}
//
////展示最近阅读
//void BookRoom::showReadRecord()
//{
//	m_currentPageNumber = 0;
//	refershPage(3);
//}

//最近下载界面, 计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
vector<int> BookRoom::getCurrentPageBookID4DownLoad()
{
	vector<int> v_boosSort;
	auto booksum = App::GetInstance()->bookDownLoad.size();
	int pagesum = ceil(booksum / 6.0);
	if (m_currentPageNumber < 0)
	{
		m_currentPageNumber = pagesum - 1;
	}
	else if (m_currentPageNumber > pagesum - 1)
	{
		m_currentPageNumber = 0;
	}
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = m_currentPageNumber * 6;
	end = m_currentPageNumber * 6 + 5;
	if (end >= booksum)
		end = booksum - 1;
	auto vcdata = App::sortMapToVector(App::GetInstance()->bookDownLoad);
	for (int i = begin; i <= end; i++)
	{
		auto bookid = vcdata[i];
		v_boosSort.push_back(bookid.first);
	}
	return v_boosSort;
}

//最近阅读界面, 计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
vector<int> BookRoom::getCurrentPageBookID4Read()
{
	vector<int> v_boosSort;
	auto booksum = App::GetInstance()->bookRead.size();
	int pagesum = ceil(booksum / 6.0);
	if (m_currentPageNumber < 0)
	{
		m_currentPageNumber = pagesum - 1;
	}
	else if (m_currentPageNumber > pagesum - 1)
	{
		m_currentPageNumber = 0;
	}
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = m_currentPageNumber * 6;
	end = m_currentPageNumber * 6 + 5;
	if (end >= booksum)
		end = booksum - 1;
	auto vcdata = App::sortMapToVector(App::GetInstance()->bookRead);
	for (int i = begin; i <= end; i++)
	{
		auto bookid = vcdata[i];
		v_boosSort.push_back(bookid.first);
	}
	return v_boosSort;
}

//最近租书界面, 计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
vector<int> BookRoom::getCurrentPageBookID4Rent()
{
	vector<int> v_boosSort;
	auto booksum = App::GetInstance()->myRentBookMap.size();
	int pagesum = ceil(booksum / 6.0);
	if (m_currentPageNumber < 0)
	{
		m_currentPageNumber = pagesum - 1;
		if (m_currentPageNumber < 0)
			m_currentPageNumber = 0;
	}
	else if (m_currentPageNumber > pagesum - 1)
	{
		m_currentPageNumber = 0;
	}
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = m_currentPageNumber * 6;
	end = m_currentPageNumber * 6 + 5;
	if (end >= booksum)
		end = booksum - 1;
	auto vcdata = App::sortMapToVector(App::GetInstance()->myRentBookMap);
	for (int i = begin; i <= end; i++)
	{
		auto bookid = vcdata[i];
		v_boosSort.push_back(bookid.first);
		App::log("   -------------------------------------------       rent bookid= ", bookid.first);
	}
	return v_boosSort;
}

void BookRoom::showImageView(int status, ImageView* view1, ImageView* view2, ImageView* view3, ImageView* view4 )
{
	//全部图书 = 0 购书列表 = 1  最近下载 = 2 最近阅读 = 3 vip包年图书 = 4
	switch (status)
	{
	case 0:
		view1->loadTexture("BookRoom/res/BookInfo_ studyr_button_allbook_press_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/BookInfo_ studyr_button_vip book_736h.png", TextureResType::PLIST);
		view3->loadTexture("BookRoom/res/BookInfo_ studyr_button_downloaded_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/BookInfo_ studyr_button_reading_736h.png", TextureResType::PLIST);
		break;
	case 3:
		view1->loadTexture("BookRoom/res/BookInfo_ studyr_button_allbook_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/BookInfo_ studyr_button_vip book_736h.png", TextureResType::PLIST);
		view3->loadTexture("BookRoom/res/BookInfo_ studyr_button_downloaded_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/BookInfo_ studyr_button_reading_press_736h.png", TextureResType::PLIST);
		break;
	case 2:
		view1->loadTexture("BookRoom/res/BookInfo_ studyr_button_allbook_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/BookInfo_ studyr_button_vip book_736h.png", TextureResType::PLIST);
		view3->loadTexture("BookRoom/res/BookInfo_ studyr_button_downloaded_press_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/BookInfo_ studyr_button_reading_736h.png", TextureResType::PLIST);
		break;
	case 4:
		view1->loadTexture("BookRoom/res/BookInfo_ studyr_button_allbook_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/BookInfo_ studyr_button_vip book_press_736h.png", TextureResType::PLIST);
		view3->loadTexture("BookRoom/res/BookInfo_ studyr_button_downloaded_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/BookInfo_ studyr_button_reading_736h.png", TextureResType::PLIST);
		break;
	}
}