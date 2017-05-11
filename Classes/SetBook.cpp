#include "SetBook.h"
#include "cocos2d.h"
#include "FKBookParser.h"
#include "YYXBookOver.h"
#include "App.h"
#include "ReadBook.h"
#include "NetIntface.h"
#include "cocos2d.h"
#include "FKAudioPlayer.h"
#include "FKPageLayer.h"
#include "YYXSound.h"
#include "YYXTime.h"
#include "AppHttp.h"
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

void SetBook::readBook(int bookId, bool isview)
{
	auto wait = YYXLayer::WaitLayer();
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
	YYXBookOver::getInstance()->init(getBookId(), App::GetInstance()->getMemberId(), isView);
	string bookPath, drawPath;
	string bookIdstr = StringUtils::format("%d", getBookId());
	bookPath = FileUtils::getInstance()->getWritablePath() + "bookUNZip/" + bookIdstr + "/Iphone1334";
	drawPath = "bookUNZip/" + bookIdstr + "/";
	//if (App::getMemberID() == "5656")
	//{
	//	BookParser::getInstance()->setBookPlayModeState(AUTOPLAY);
	//}
	BookParser::getInstance()->setDrawFilePath(drawPath);
	//退出按钮
	BookParser::getInstance()->setPageQuitCallBack([=]() {
		YYXLayer::controlTouchTime(1, "setPageQuitCallBackTime", [=]() {
			BookParser::getInstance()->bookQuit();
			auto control = ControlScene::getInstance();
			control->backFromScene();
			thread mythread([=]() {
				App::ccsleep(3000);
				control->end();
			});
			mythread.detach();
			//保存阅读记录
			saveReadRecordEnd();
			YYXSound::getInstance()->resumeBackGroundMusic();
		});
	});
	//向前按钮
	BookParser::getInstance()->setPageUpCallBack([=]() {
		YYXLayer::controlTouchTime(1, "ReadTime", [=]() {
			BookParser::getInstance()->pageUp();
			saveReadRecording();
		});
	});
	//向后按钮
	BookParser::getInstance()->setPageDownCallBack([=]() {
		YYXLayer::controlTouchTime(1, "ReadTime", [=]() {
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
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
			});
		});
	});
	BookParser::getInstance()->setPageMenuChangeCallBack([](Menu* m) {
		auto down =(MenuItemImage*) m->getChildByTag(BUTTON_TAG_PAGE_DOWN);
		down->setVisible(true);
	});
	//分享
	BookParser::getInstance()->setBookShareCallBack([=](RenderTexture* img) {
		YYXLayer::controlTouchTime(3, "ClickShareTime", [=]()
		{
			ShareBook(img, getBookId());
		}, []() {
			Toast::create(App::getString("CAOZUOGUOYUPINGFAN"));
		});
	});
	int result = -999;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	result = BookParser::getInstance()->bookJsonParser(bookPath, 0.818, 0.96, "win32");
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	result = BookParser::getInstance()->bookJsonParser(bookPath, 0.818, 0.96, "android");
#endif
	if (result != 0)
	{//阅读失败
		auto control = ControlScene::getInstance();
		control->replaceScene(control->getCurrentScene(), control->getFromScene(), false);
		Toast::create(App::getString("SHUJIYICHANGJIANYICHANGXINXIAZAI"));
	}
	else
	{//阅读成功			
		saveReadRecordStart();
		YYXSound::getInstance()->stopAll();
		YYXSound::getInstance()->pauseBackGroundMusic();
		ReadBook::getInstance()->addBook(getBookId(), (int)YYXLayer::getCurrentTime4Second());
	}
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
	string fileName = StringUtils::format("temp/share_%d.png", YYXTime::getInstance()->getRandomL());
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
			NetIntface::share(path.c_str(), name.c_str(), "http://www.ellabook.cn", headstr, "", "", [](string json) {
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
	}
}

void SetBook::initResource(Layer * layer)
{
	Size winSize = Director::getInstance()->getWinSize();
	float highScale = 1;
	auto pageUp = MenuItemImage::create("ReadBook_PageUp_667h.png",
		"ReadBook_PageUp_Selected_667h.png",
		[=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "ReadTime", [=]() {
			Director::getInstance()->replaceScene(TransitionPageTurn::create(1.0f, PageLayer::createScene(), true));
		});
	});
	pageUp->setPosition(-winSize.width / 2, -winSize.height / 2);
	pageUp->setScale(highScale);
	pageUp->setAnchorPoint(Vec2(0, 0));
	pageUp->setTag(BUTTON_TAG_PAGE_UP);


	auto pageDown = MenuItemImage::create("ReadBook_PageDown_667h.png",
		"ReadBook_PageDown_Selected_667h.png",
		[](Ref* sender) {
		BookParser::getInstance()->pageDown(true);
	});
	pageDown->setPosition(winSize.width / 2, -winSize.height / 2);
	pageDown->setScale(highScale);
	pageDown->setAnchorPoint(Vec2(1.0f, 0));
	pageDown->setTag(BUTTON_TAG_PAGE_DOWN);

	auto pageQuit = MenuItemImage::create("Common_Return_667h.png",
		"Common_Return_Selected_667h.png",
		[](Ref* sender) {
		YYXLayer::controlTouchTime(1, "setPageQuitCallBackTime", [=]() {
			BookParser::getInstance()->bookQuit();
			auto control = ControlScene::getInstance();
			control->backFromScene();
			thread mythread([=]() {
				App::ccsleep(3000);
				control->end();
			});
			mythread.detach();
			YYXSound::getInstance()->resumeBackGroundMusic();
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