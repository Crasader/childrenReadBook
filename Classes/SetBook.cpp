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
	int childrenid = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
	setChildrenId(childrenid);
	setBookId(bookId);
	setIsView(isview);
	setMemberId(App::GetInstance()->getMemberId());
	thread([=]() {
		App::ccsleep(1000);
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
			readBook();
		});
	}).detach();
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
			auto control = ControlScene::getInstance();
			control->backFromScene();
			thread([=]() {
				App::ccsleep(3000);
				control->end();
			}).detach();
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
						BookParser::getInstance()->pageDown();						
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
					if (BookParser::getInstance()->pageDown() == 0)
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
	string fileName = StringUtils::format("temp/share_%d.png", (int)YYXLayer::getRandom());
	string dir = FileUtils::getInstance()->getWritablePath() + "temp";
	if (!FileUtils::getInstance()->isFileExist(dir))
		FileUtils::getInstance()->createDirectory(dir);
	bool savebool = img->saveToFile(fileName, false, [=](RenderTexture* r, string savepath) {
		App::log("saveToFile path = " + savepath);
		NetIntface::httpGetBookInfo(bookId, "", [=](string json) {
			//获取书名然后分享
			string path = savepath;
			if (path == "" || !FileUtils::getInstance()->isFileExist(path))
			{
				App::log(path + " is error");
				Toast::create(App::getString("FENXIANGSHIBAI"));
				return;
			}
			rapidjson::Document doc;
			YYXLayer::getJsonObject4Json(doc, json);
			int code = YYXLayer::getIntForJson(-999, doc, "code");
			string name = "";
			if (code == 0)
				name = YYXLayer::getStringForJson("", doc, "data", "bookName");
			if (name == "")
				name = App::getString("TEBIEHAOKANDESHU");
			//分享
			auto headpath = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildHeadPortrait", "");
			const char* headstr = "";
			if (headpath != "" && FileUtils::getInstance()->isFileExist(headpath))
				headstr = headpath.c_str();
			NetIntface::share(path.c_str(), name.c_str(), "http://www.ellabook.cn", headstr, "", "", [](string json) {
				//分享成功				
				if (App::GetInstance()->getMemberId() <= 0)
				{
					Toast::create(App::getString("DENGLUHOUFENXIANGKEHUODEYILAHONGBAO"));
				}
				NetIntface::httpShareWithCoupon(App::GetInstance()->getMemberId(), "", [](string json) {
					//分享得红包 网络请求成功
					NetIntface::httpShareWithCouponCallBack(json, [](int coupon_amount) {
						//解析成功 红包数值
						XZLayer::showShareRedPacket(StringUtils::format("%d", coupon_amount / 100) + App::getString("YUAN"));
					}, []() {
						//解析成功后
					}, []() {
						//解析失败
					});
				}, "", [](string str) {
					//网络请求失败
				});
			}, "", [](string str) {
				//分享失败或者取消
			});
			return;
		}, "", [](string str) {
			Toast::create(App::getString("NETEXCEPTION"));
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
	float highScale = winSize.height / 1536;
	auto pageUp = MenuItemImage::create("ReadBook_PageUp_ipad.png",
		"ReadBook_PageUp_Selected_ipad.png",
		[=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "ReadTime", [=]() {
			Director::getInstance()->replaceScene(TransitionPageTurn::create(1.0f, PageLayer::createScene(), true));
		});
	});
	pageUp->setPosition(-winSize.width / 2, -winSize.height / 2);
	pageUp->setScale(highScale*0.63);
	pageUp->setAnchorPoint(Vec2(0, 0));
	pageUp->setTag(BUTTON_TAG_PAGE_UP);


	auto pageDown = MenuItemImage::create("ReadBook_PageDown_ipad.png",
		"ReadBook_PageDown_Selected_ipad.png",
		[](Ref* sender) {
		BookParser::getInstance()->pageDown();
	});
	pageDown->setPosition(winSize.width / 2, -winSize.height / 2);
	pageDown->setScale(highScale*0.63);
	pageDown->setAnchorPoint(Vec2(1.0f, 0));
	pageDown->setTag(BUTTON_TAG_PAGE_DOWN);

	auto pageQuit = MenuItemImage::create("Common_Return_ipad@2x.png",
		"Common_Return_Selected_ipad@2x.png",
		[](Ref* sender) {

		YYXLayer::controlTouchTime(1, "setPageQuitCallBackTime", [=]() {
			BookParser::getInstance()->bookQuit();
			auto control = ControlScene::getInstance();
			control->backFromScene();
			thread([=]() {
				App::ccsleep(3000);
				control->end();
			}).detach();
			YYXSound::getInstance()->resumeBackGroundMusic();
		});
	});
	pageQuit->setPosition(-winSize.width / 2, winSize.height / 2);
	pageQuit->setScale(highScale*0.8);
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
	string path = FileUtils::getInstance()->getWritablePath() + "readBookRecord/" + StringUtils::format("%d.json", (int)YYXLayer::getRandom());
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
	map<string, string> paramter;
	paramter["memberId"] = StringUtils::format("%d", readMemberId);
	paramter["childrenId"] = StringUtils::format("%d", readChildren);
	paramter["bookId"] = StringUtils::format("%d", readBookId); 
	paramter["readStartTime"] = readStartTime;
	paramter["readEndTime"] = readEndTime;
	paramter["resource"] = App::m_resource;
	string url = string(IP).append(NET_SAVE_READHISTORY);
	NetIntface::httpPost(url, paramter, "", [=](string json) {
		NetIntface::saveReadRecordCallBack(json, [=]() {
			//上传成功
			setIsHttp(false);
			if (callback)
				callback(this);
		}, [=]() {
			//上传错误
			setIsHttp(true);
			WriteFile();
			if (callback)
				callback(this);
		});
	}, "", [=](string str) {
		//上传失败
		setIsHttp(true);
		WriteFile();
		if (callback)
			callback(this);
	});

}