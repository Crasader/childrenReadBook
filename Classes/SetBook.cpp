#include "SetBook.h"
#include "cocos2d.h"
#include "FKBookParser.h"
#include "YYXBookOver.h"
#include "App.h"
#include "ReadBook.h"
#include "CrossPlatform.h"
#include "cocos2d.h"
#include "FKAudioPlayer.h"
#include "FKPageLayer.h"
#include "YYXSound.h"
#include "YYXTime.h"
#include "AppHttp.h"
#include "HttpWaiting.h"
#include "BookInfoControl.h"
#include "YYXRentBook.h"
USING_NS_CC;
USING_NS_FK;

SetBook* SetBook::SetBook::instance=nullptr;

SetBook::SetBook()
{

}

SetBook::~SetBook()
{

}

SetBook * SetBook::getInstance()
{
	if (instance == nullptr)
	{
		instance = new SetBook();
	}
	return instance;
}

void SetBook::zushunet(int bookid)
{
	AppHttp::getInstance()->httpThe_relationship_between_human_and_books(bookid, [](int code) {
		BookInfoControl* control = new BookInfoControl();
		control->BookStauts(code);
		if (control->IsMemberVIP() && control->IsBookVIP() && !control->IsBookBuy() && control->IsBookRent() && !control->IsBorrow())	{
			YYXRentBook::getInstance()->backgroundThreadRentBook(control->BookId(),control->MemberId(), nullptr);
		}
	});
}

void SetBook::readBook(int bookId, bool isview)
{
	auto wait = HttpWaiting::getInstance()->newWaitingLayer();
	Director::getInstance()->getRunningScene()->addChild(wait, 1000);
	int childrenid = User::getInstance()->getChildId();
	setChildrenId(childrenid);
	setBookId(bookId);
	setIsView(isview);
	setMemberId(App::GetInstance()->getMemberId());
	thread mythread([=]() {
		App::ccsleep(1000);
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
			readBook();
		});
	});
	mythread.detach();
}

void SetBook::readBook()
{	
	string bookPath, drawPath;
	string bookIdstr = StringUtils::format("%d", getBookId());
	bookPath = FileUtils::getInstance()->getWritablePath() + "bookUNZip/" + bookIdstr + "/Iphone1334";
	//if (App::getMemberID() == "5656")
	//{
	//	BookParser::getInstance()->setBookPlayModeState(AUTOPLAY);
	//}
	BookParser::getInstance()->setBookPlayModeState(READ);
	//退出按钮
	BookParser::getInstance()->setPageQuitCallBack([]() {
		YYXLayer::controlTouchTime(3, "setPageQuitCallBackTime", []() {
			SetBook::getInstance()->setButtonOpacity();
			auto node = CSLoader::createNode("Book/csb/readbook.csb");
			if (node){
				Button* tuichu = (Button*)node->getChildByName("Button_1");
				Button* fenxiang = (Button*)node->getChildByName("Button_2");
				Button* close = (Button*)node->getChildByName("Button_3");
				ImageView* bg = (ImageView*)node->getChildByName("bg_0");
				if (bg){
					bg->addClickEventListener([](Ref* sender) {});
				}
				tuichu->addClickEventListener([=](Ref* sender) {
					SetBook::getInstance()->setButtonOpacity();
					node->removeFromParentAndCleanup(true);
					BookParser::getInstance()->bookQuit();
					auto control = ControlScene::getInstance();
					control->backFromScene();
					thread mythread([=]() {
						App::ccsleep(3000);
						control->end();
					});
					mythread.detach();
					//保存阅读记录
					SetBook::getInstance()->saveReadRecordEnd();
					YYXSound::getInstance()->resumeBackGroundMusic();
				});
				fenxiang->addClickEventListener([=](Ref* sender) {
					YYXLayer::controlTouchTime(3, "shareBookTime", [=]() {
						SetBook::getInstance()->setButtonOpacity();
						node->removeFromParentAndCleanup(true);
						SetBook::getInstance()->ShareBook(BookParser::getInstance()->pageScreenShot(), SetBook::getInstance()->getBookId());
					});
				});
				close->addClickEventListener([=](Ref* sender) {
					SetBook::getInstance()->setButtonOpacity();
					node->removeFromParentAndCleanup(true);
				});
				Director::getInstance()->getRunningScene()->addChild(node,10000);
			}			
		});
	});
	//向前按钮
	BookParser::getInstance()->setPageUpCallBack([=]() {
		//YYXLayer::controlTouchTime(3, "ReadTime", [=]() {
			SetBook::getInstance()->setButtonOpacity();
			BookParser::getInstance()->pageUp();
			saveReadRecording();
		//});
	});
	//向后按钮
	BookParser::getInstance()->setPageDownCallBack([=]() {
		//YYXLayer::controlTouchTime(3, "ReadTime", [=]() {
			SetBook::getInstance()->setButtonOpacity();
			//Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
				saveReadRecording();
				if (BookParser::getInstance()->getCurrentPage() == 5)
				{
					if (YYXBookOver::getInstance()->UserBuy() || YYXBookOver::getInstance()->UserVip())
					{
						BookParser::getInstance()->pageDown(true);
					}
					else
					{
						AudioPlayer::getInstance()->stopAllEffect();
						Layer* it = YYXBookOver::getInstance()->tryReadBookOverLayer();
						initResource(it);
						auto scene = Scene::create();
						scene->addChild(it);
						Director::getInstance()->replaceScene(TransitionPageTurn::create(1.0f, scene, false));
					}
				}
				else
				{
					if (BookParser::getInstance()->pageDown(true) == 0)
					{
						AudioPlayer::getInstance()->stopAllEffect();
						Layer* it = YYXBookOver::getInstance()->readBookOverLayer();
						initResource(it);
						auto scene = Scene::create();
						scene->addChild(it);
						Director::getInstance()->replaceScene(TransitionPageTurn::create(1.0f, scene, false));
					}
				}
			//});
		//});
	});
	//BookParser::getInstance()->setPageMenuChangeCallBack([](Menu* m) {
		//auto down =(MenuItemImage*) m->getChildByTag(BUTTON_TAG_PAGE_DOWN);
		//down->setVisible(true);
	//});
	//分享
	//BookParser::getInstance()->setBookShareCallBack([=](RenderTexture* img) {
	//	YYXLayer::controlTouchTime(3, "ClickShareTime", [=]()
	//	{
	//		ShareBook(img, getBookId());
	//	}, []() {
	//		Toast::create(App::getString("CAOZUOGUOYUPINGFAN"));
	//	});
	//});
	readButton();
	int result = -999;
	HttpWaiting::getInstance()->remove();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	result = BookParser::getInstance()->bookJsonParser(bookPath, 0.818, 0.96, "win32");
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	result = BookParser::getInstance()->bookJsonParser(bookPath, 0.818, 0.96, "android");
#endif
	drawPath = "bookUNZip/" + bookIdstr + "/";
	BookParser::getInstance()->setDrawFilePath(drawPath);
	if (result != 0)
	{//阅读失败
		auto control = ControlScene::getInstance();
		control->backFromScene();
		Toast::create(App::getString("SHUJIYICHANGJIANYICHANGXINXIAZAI"));
	}
	else
	{//阅读成功			
		YYXBookOver::getInstance()->init(getBookId(), App::GetInstance()->getMemberId(), isView);
		saveReadRecordStart();
		YYXSound::getInstance()->stopAll();
		YYXSound::getInstance()->pauseBackGroundMusic();
		ReadBook::getInstance()->addBook(getBookId(), (int)YYXLayer::getCurrentTime4Second());
		zushunet(bookId);
	}
}

void SetBook::readButton()
{
	BookParser::getInstance()->setPageMenuChangeCallBack([](Menu* menu) {
		auto myup = (MenuItemImage*)menu->getChildByTag(BUTTON_TAG_PAGE_UP);
		auto mydown = (MenuItemImage*)menu->getChildByTag(BUTTON_TAG_PAGE_DOWN);
		auto mymenu = (MenuItemImage*)menu->getChildByTag(BUTTON_TAG_PAGE_QUIT);
		auto myshare = (MenuItemImage*)menu->getChildByTag(BUTTON_TAG_PAGE_SHARE);
		if (myshare)
			myshare->setVisible(false);
		mydown->setVisible(true);
		myup->setNormalImage(Sprite::create("other/yueduyemianyouhua-button-shangyiye-nor_736h.png"));
		myup->setSelectedImage(Sprite::create("other/yueduyemianyouhua-button-shangyiye-pre_736h.png"));
		mydown->setNormalImage(Sprite::create("other/yueduyemianyouhua-button-xiayiye-nor_736h.png"));
		mydown->setSelectedImage(Sprite::create("other/yueduyemianyouhua-button-xiayiye-pre_736h.png"));
		mymenu->setNormalImage(Sprite::create("other/yueduyemianyouhua-button-liebiao-nor_736h.png"));
		mymenu->setSelectedImage(Sprite::create("other/yueduyemianyouhua-button-liebiao-pre_736h.png"));
		auto visibleSize = Director::getInstance()->getVisibleSize();
		auto sc = visibleSize.height / 1536;
		myup->setScale(sc);
		mydown->setScale(sc);
		mymenu->setScale(sc);
		auto time = SetBook::getInstance()->getOpacityTime();
		long long nowts = YYXTime::getInstance()->getNowTime4S() - time;
		int op = 150;
		if (nowts < 10)
			op = 255;
		myup->setOpacity(op);
		mydown->setOpacity(op);
		mymenu->setOpacity(op);
		
		auto listenerup = EventListenerCustom::create(TAG_SETOPACITY, [=](EventCustom* e) {
			int op2 = (int)e->getUserData();
			if (op2 == 255)
			{
				myup->setOpacity(op2);
				mydown->setOpacity(op2);
				mymenu->setOpacity(op2);
			}
			else
			{
				auto time = SetBook::getInstance()->getOpacityTime();
				long long nowts = YYXTime::getInstance()->getNowTime4S() - time;
				if (nowts >= 10)
				{
					myup->setOpacity(op2);
					mydown->setOpacity(op2);
					mymenu->setOpacity(op2);
				}
			}
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerup, myup);
	});
}

void SetBook::setButtonOpacity(int time)
{
	//YYXLayer::sendNotify(TAG_SETOPACITY, "", 255);
	//setOpacityTime(YYXTime::getInstance()->getNowTime4S());
	//thread([time]() {
	//	App::ccsleep(time * 1000);
	//	YYXLayer::sendNotify(TAG_SETOPACITY,"", 70);
	//}).detach();
}

void SetBook::saveReadRecordStart()
{
	if (childrenId <= 0)
		return;
	if (m_CurrentReadBook == nullptr)
		m_CurrentReadBook = ReadBookRecord::create();
	else
	{
		if (m_CurrentReadBook->getReadBookId() != bookId)
		{
			if(m_CurrentReadBook->getIsHttp())
				m_CurrentReadBook->httpUploadReadingRecord([](ReadBookRecord* record) {
				ReadBookRecord::del(record);
			});
			m_CurrentReadBook = ReadBookRecord::create();
		}
	}
	string time = App::GetInstance()->GetStringTime2();
	m_CurrentReadBook->setReadBookId(bookId);
	m_CurrentReadBook->setReadChildren(childrenId);
	m_CurrentReadBook->setReadMemberId(memberId);
	m_CurrentReadBook->setReadStartTime(time);
	m_CurrentReadBook->setReadEndTime(time);
}

void SetBook::saveReadRecording()
{
	if (childrenId <= 0)
		return;
	//中间翻页
	string time = App::GetInstance()->GetStringTime2();
	if (m_CurrentReadBook == nullptr)
	{
		m_CurrentReadBook = ReadBookRecord::create();
		m_CurrentReadBook->setReadBookId(bookId);
		m_CurrentReadBook->setReadChildren(childrenId);
		m_CurrentReadBook->setReadMemberId(memberId);
		m_CurrentReadBook->setReadStartTime(time);
	}
	m_CurrentReadBook->setReadEndTime(time);
}

void SetBook::saveReadRecordEnd()
{
	if (childrenId <= 0)
		return;
	//结束阅读最后一页
	//将数据上传，如果失败就存本地，下次app启动时候再读出来上传
	string time = App::GetInstance()->GetStringTime2();
	if (m_CurrentReadBook == nullptr)
	{
		m_CurrentReadBook = ReadBookRecord::create();
		m_CurrentReadBook->setReadBookId(bookId);
		m_CurrentReadBook->setReadChildren(childrenId);
		m_CurrentReadBook->setReadMemberId(memberId);
		m_CurrentReadBook->setReadStartTime(time);
	}
	m_CurrentReadBook->setReadEndTime(time);
	m_CurrentReadBook->httpUploadReadingRecord();
}

void SetBook::ShareBook(RenderTexture* img, int bookId)
{
	CrossPlatform::getInstance()->ShareWeb(App::getMemberId(), SetBook::getInstance()->getBookId());
/*	string fileName = StringUtils::format("temp/share_%d.png", YYXTime::getInstance()->getRandomL());
	string dir = FileUtils::getInstance()->getWritablePath() + "temp";
	if (!FileUtils::getInstance()->isFileExist(dir))
		FileUtils::getInstance()->createDirectory(dir);
	bool savebool = img->saveToFile(fileName, false, [=](RenderTexture* r, string savepath) {
		AppHttp::getInstance()->httpBookInfo(bookId, [=](string name) {
			//分享
			string path = savepath;
			if (path.empty() || !FileUtils::getInstance()->isFileExist(path))
			{
				App::log(path + " is error");
				Toast::create(App::getString("FENXIANGSHIBAI"));
				return;
			}
			const char* headstr = "";
			auto it = User::getInstance()->getChild(User::getInstance()->getChildId());
			if (it)
			{
				auto headpath = it->getPath();
				if (!headpath.empty() && FileUtils::getInstance()->isFileExist(headpath))
					headstr = headpath.c_str();
			}
			CrossPlatform::share(path.c_str(), name.c_str(), "http://www.ellabook.cn", headstr, "", "", [](string json) {
				//分享成功				
				if (User::getInstance()->getMemberId() <= 0)
					Toast::create(App::getString("DENGLUHOUFENXIANGKEHUODEYILAHONGBAO"));
				else
					AppHttp::getInstance()->httpShareHongBao();
			}, "", [](string str) {
				//分享失败或者取消
			});
			return;
		});
	});
	if (!savebool)
	{
		App::log("save is error");
		Toast::create(App::getString("JIETUSHIBAI"));
		return;
	}*/
}

void SetBook::initResource(Layer * layer)
{/*		myup->setNormalImage(Sprite::create("other/yueduyemianyouhua-button-shangyiye-nor_736h.png"));
		myup->setSelectedImage(Sprite::create("other/yueduyemianyouhua-button-shangyiye-pre_736h.png"));
		mydown->setNormalImage(Sprite::create("other/yueduyemianyouhua-button-xiayiye-nor_736h.png"));
		mydown->setSelectedImage(Sprite::create("other/yueduyemianyouhua-button-xiayiye-pre_736h.png"));
		mymenu->setNormalImage(Sprite::create("other/yueduyemianyouhua-button-liebiao-nor_736h.png"));
		mymenu->setSelectedImage(Sprite::create("other/yueduyemianyouhua-button-liebiao-pre_736h.png"));*/
	Size winSize = Director::getInstance()->getWinSize();
	float highScale = winSize.height/1536;
	auto pageUp = MenuItemImage::create("other/yueduyemianyouhua-button-shangyiye-nor_736h.png",
		"other/yueduyemianyouhua-button-shangyiye-pre_736h.png",
		[=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "ReadTime", [=]() {
			Director::getInstance()->replaceScene(TransitionPageTurn::create(1.0f, PageLayer::createScene(), true));
		});
	});
	pageUp->setPosition(-winSize.width / 2, -winSize.height / 2);
	pageUp->setScale(highScale);
	pageUp->setAnchorPoint(Vec2(0, 0));
	pageUp->setTag(BUTTON_TAG_PAGE_UP);


	auto pageDown = MenuItemImage::create("other/yueduyemianyouhua-button-xiayiye-nor_736h.png",
		"other/yueduyemianyouhua-button-xiayiye-pre_736h.png",
		[](Ref* sender) {
		BookParser::getInstance()->pageDown(true);
	});
	pageDown->setPosition(winSize.width / 2, -winSize.height / 2);
	pageDown->setScale(highScale);
	pageDown->setAnchorPoint(Vec2(1.0f, 0));
	pageDown->setTag(BUTTON_TAG_PAGE_DOWN);

	auto pageQuit = MenuItemImage::create("other/yueduyemianyouhua-button-liebiao-nor_736h.png",
		"other/yueduyemianyouhua-button-liebiao-pre_736h.png",
		[](Ref* sender) {
		YYXLayer::controlTouchTime(3, "setPageQuitCallBackTime", []() {
			SetBook::getInstance()->setButtonOpacity();
			auto node = CSLoader::createNode("Book/csb/readbook.csb");
			if (node) {
				Button* tuichu = (Button*)node->getChildByName("Button_1");
				Button* fenxiang = (Button*)node->getChildByName("Button_2");
				Button* close = (Button*)node->getChildByName("Button_3");
				ImageView* bg = (ImageView*)node->getChildByName("bg_0");
				if (bg) {
					bg->addClickEventListener([](Ref* sender) {});
				}
				tuichu->addClickEventListener([=](Ref* sender) {
					SetBook::getInstance()->setButtonOpacity();
					node->removeFromParentAndCleanup(true);
					BookParser::getInstance()->bookQuit();
					auto control = ControlScene::getInstance();
					control->backFromScene();
					thread mythread([=]() {
						App::ccsleep(3000);
						control->end();
					});
					mythread.detach();
					//保存阅读记录
					SetBook::getInstance()->saveReadRecordEnd();
					YYXSound::getInstance()->resumeBackGroundMusic();
					CrossPlatform::getInstance()->ShareWeb(App::getMemberId(), SetBook::getInstance()->getBookId());
				});
				fenxiang->addClickEventListener([=](Ref* sender) {
					YYXLayer::controlTouchTime(3, "shareBookTime", [=]() {
						SetBook::getInstance()->setButtonOpacity();
						node->removeFromParentAndCleanup(true);
						SetBook::getInstance()->ShareBook(BookParser::getInstance()->pageScreenShot(), SetBook::getInstance()->getBookId());
					});
				});
				close->addClickEventListener([=](Ref* sender) {
					SetBook::getInstance()->setButtonOpacity();
					node->removeFromParentAndCleanup(true);
				});
				Director::getInstance()->getRunningScene()->addChild(node, 10000);
			}
		});
	});
	pageQuit->setPosition(-winSize.width / 2, winSize.height / 2);
	pageQuit->setScale(highScale);
	pageQuit->setAnchorPoint(Vec2(0, 1.0f));
	pageQuit->setTag(BUTTON_TAG_PAGE_QUIT);

	Menu *menuPage = Menu::create(pageUp, pageQuit, NULL);
	layer->addChild(menuPage, 100, 10000);
	BookParser::getInstance()->setPageMenu(menuPage);
}

ReadBookRecord::ReadBookRecord()
{
	readBookId = -999;
	readMemberId = -999;
	readChildren = -999;
	readStartTime="";
	readEndTime="";
	setIsHttp(true);
}

ReadBookRecord::~ReadBookRecord()
{

}

ReadBookRecord * ReadBookRecord::create()
{
	ReadBookRecord *pRet = new(std::nothrow) ReadBookRecord();
	pRet->autorelease();
	pRet->retain();
	return pRet;
}

void ReadBookRecord::del(ReadBookRecord* pRet)
{
	if (pRet)
	{
		if (pRet->getReferenceCount() > 0)
			CC_SAFE_RELEASE_NULL(pRet);
	}
}

void ReadBookRecord::WriteFile()
{
	string path = FileUtils::getInstance()->getWritablePath() + "readBookRecord/" + StringUtils::format("%d.json", YYXTime::getInstance()->getRandomL());
	map<string, string> parater;
	parater["readBookId"] = StringUtils::format("%d", getReadBookId());
	parater["readMemberId"] = StringUtils::format("%d", getReadMemberId());
	parater["readChildren"] = StringUtils::format("%d", getReadChildren());
	parater["readStartTime"] = getReadStartTime();
	parater["readEndTime"] = getReadEndTime();
	string josn = YYXLayer::getStringFormMap(parater);
	YYXLayer::writeFile(josn, path);
}

void ReadBookRecord::httpUploadReadingRecord(function<void(ReadBookRecord* )> callback)
{
	if (!getIsHttp())
		return;
	AppHttp::getInstance()->httpUploadReadBookRecord(this, [=](ReadBookRecord* r) {
		setIsHttp(false);
		if (callback)
			callback(r);
	}, [=](ReadBookRecord* r) {
		setIsHttp(true);
		WriteFile();
		if (callback)
			callback(r);
	});
}