#include "WaitingScene.h"
#include "Toast.h"
#include "NetIntface.h"
#include "XZLayer.h"
#include "SendCommentLayer.h"
#include "FKPageLayer.h"
#include "YYXBuyBook.h"
#include "YYXRentBook.h"
#include "YYXBookOver.h"

USING_NS_CC;
USING_NS_FK;


Scene* Waiting::createScene(std::string bookId, bool isView)
{
    auto scene = Scene::create();
    
    auto layer = Waiting::create(bookId, isView);
    scene->addChild(layer);

    return scene;
}

Waiting* Waiting::create(std::string bookId, bool isView)
{ 
	Waiting *pRet = new(std::nothrow) Waiting();
	if (pRet && pRet->init(bookId, isView))
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

bool Waiting::init(std::string bookId, bool isView)
{
	App::log("Waiting::init");
    if ( !Layer::init() )
    {
        return false;
    }
	int m_bookId = atoi(bookId.c_str());
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	auto layer = Index::WaitLayer();
	auto imagev = (ImageView*)layer->getChildByName("Image_1");
	imagev->setScale9Enabled(true);
	//imagev->loadTexture("kaichanglogo.png");
	imagev->setVisible(false);
	addChild(LayerColor::create(Color4B::WHITE), -1);
	auto text = LabelTTF::create();
	string str = "Version: " + BookParser::getInstance()->getVersion();
	text->setFontFillColor(Color3B::BLACK);
	text->setFontSize(80);
	text->setString(str);
	text->setAnchorPoint(Vec2(0.5, 0.5));
	text->setScale(0.35);
	text->setPosition(Vec2(visibleSize.width/2, origin.y + 100));
	addChild(text);
	layer->setAnchorPoint(Vec2(0.5, 0.5));
	layer->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
	addChild(layer);
	this->runAction(Sequence::create(DelayTime::create(0.1f), CallFuncN::create([=](Node* n) {
		YYXBookOver::getInstance()->init(m_bookId, App::GetInstance()->m_me->id, isView);
		string bookPath, drawPath;
		bookPath = FileUtils::getInstance()->getWritablePath() + "bookUNZip/" + bookId + "/Iphone1334";
		drawPath = "bookUNZip/" + bookId + "/";
		//if (App::getMemberID() == "5656")
		//{
		//	BookParser::getInstance()->setBookPlayModeState(AUTOPLAY);
		//}
		BookParser::getInstance()->setDrawFilePath(drawPath);
		//退出按钮
		BookParser::getInstance()->setPageQuitCallBack([=]() {
			App::GetInstance()->popBack();
			Index::BackPreviousScene();
			//保存阅读记录
			saveReadRecordEnd(bookId, isView);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "UserIsReadingBook", 0);
			App::GetInstance()->resumeBackGroundMusic();
		});
		//向前按钮
		BookParser::getInstance()->setPageUpCallBack([=]() {
			YYXLayer::controlTouchTime(1, "ReadTime", [=]() {
				BookParser::getInstance()->pageUp();
				//saveReadRecording(bookId, isView);
				saveReadRecording(bookId);
			});			
		});
		//向后按钮
		BookParser::getInstance()->setPageDownCallBack([=]() {
			YYXLayer::controlTouchTime(1, "ReadTime", [=]() {
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
					if (BookParser::getInstance()->getCurrentPage() == 5)
					{
						if (YYXBookOver::getInstance()->UserBuy() || YYXBookOver::getInstance()->UserVip())
						{
							BookParser::getInstance()->pageDown();
							saveReadRecording(bookId);
						}
						else
						{
							AudioPlayer::getInstance()->stopAllEffect();
							Layer* it = YYXBookOver::getInstance()->tryReadBookOverLayer();
							initResource(it);
							auto scene = Scene::create();
							scene->addChild(it);
							//Director::getInstance()->pushScene(TransitionPageTurn::create(1.0f, scene, false));1
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
							//Director::getInstance()->pushScene(TransitionPageTurn::create(1.0f, scene, false));1
							Director::getInstance()->replaceScene(TransitionPageTurn::create(1.0f, scene, false));
							saveReadRecording(bookId);
						}
					}
				});
			});
		});
		//分享
		BookParser::getInstance()->setBookShareCallBack([=](RenderTexture* img) {
			YYXLayer::controlTouchTime(3, "ClickShareTime", [=]()
			{
				ShareBook(img, m_bookId);
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
			App::GetInstance()->popBack();
			Index::BackPreviousScene();
			Toast::create(App::getString("SHUJIYICHANGJIANYICHANGXINXIAZAI"));
		}
		else
		{//阅读成功
			saveReadRecordStart(bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "UserIsReadingBook", 1);
			App::GetInstance()->stopOtherVoice();
			App::GetInstance()->pauseBackGroundMusic();
		}
	}), NULL));
	App::log("Waiting::init--end");
	return true;
 }

 //新分享
 void Waiting::ShareBook(RenderTexture* img, int bookId)
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
				 int memberID = -999;
				 if (App::GetInstance()->m_me)
					 memberID = App::GetInstance()->m_me->id;
				 if (memberID <= 0)
				 {
					 Toast::create(App::getString("DENGLUHOUFENXIANGKEHUODEYILAHONGBAO"));
				 }
				 NetIntface::httpShareWithCoupon(memberID, "", [](string json) {
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

 void Waiting::Share(string json, string path)
 {
	 //文件IO操作需要时间
	 //auto time = App::GetInstance()->getTime("shareTime", YYXLayer::getCurrentTime4Second());
	 //if (YYXLayer::getCurrentTime4Second() <= time)
		 //App::ccsleep(1000);
	 if (path == "" || !FileUtils::getInstance()->isFileExist(path))
	 {
		 App::log(path+" is error");
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
	 //TODO
	 //string testpath;
	 //CocosAndroidJni::GetPhotoPath(testpath);
	 //App::copyFile(path, testpath + "/1.png");
	 //分享
	 string runKey = "shareSuccess";
	 string errorKey = "shareFail";
	 auto headpath = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildHeadPortrait", "");
	 const char* headstr = "";
	 if (headpath != "" && FileUtils::getInstance()->isFileExist(headpath))
		 headstr = headpath.c_str();
		 //CocosAndroidJni::Share(path.c_str(), name.c_str(), "http://www.ellabook.cn", headpath.c_str(), "");
	 NetIntface::share(path.c_str(), name.c_str(), "http://www.ellabook.cn", headstr, "", runKey, [](string json) {
		 //分享成功
		 int memberID = -999;
		 if (App::GetInstance()->m_me)
			 memberID = App::GetInstance()->m_me->id;
		 if (memberID <= 0)
		 {
			 Toast::create(App::getString("DENGLUHOUFENXIANGKEHUODEYILAHONGBAO"));
		 }
		 string Couponrunkey = "waitingSceneHttpShareWithCouponSuccess";
		 string Couponerrorkey = "waitingSceneHttpShareWithCouponFail";
		 NetIntface::httpShareWithCoupon(memberID, Couponrunkey, [](string json) {
			//网络请求成功
			 NetIntface::httpShareWithCouponCallBack(json, [](int coupon_amount) {
				//解析成功 红包数值
				 XZLayer::showShareRedPacket(StringUtils::format("%d",coupon_amount /100) + App::getString("YUAN"));
			 }, []() {
				//解析成功后
			 }, []() {
				//解析失败
			 });
		 },
			 Couponerrorkey, [](string str) {
			//网络请求失败
		 });
	 }, errorKey, [](string str) {
		 //分享失败或者取消
	 });
	 return;
 }

 //void Waiting::saveReadRecord()
 //{
	// App::log(" Waiting::saveReadRecord()");
	// App::GetInstance()->m_read->childrenId = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
	// if (App::GetInstance()->m_me == nullptr || App::GetInstance()->m_read->childrenId == -999 ||App::GetInstance()->m_read->bookId == -999 || App::GetInstance()->m_read->startTime.length() == 0) 
	//	 return;
	//NetIntface::saveReadRecord(App::GetInstance()->m_me->id,
	//	App::GetInstance()->m_read->childrenId,
	//	App::GetInstance()->m_read->bookId,
	//	App::GetInstance()->m_read->startTime,
	//	App::GetStringTime2(), "", [](string json) {}, "", [](string str) {});
	//App::log(" Waiting::saveReadRecord()---END");
 //}
	//readBookId
	// readStartTime
	// readEndTime
	// readMemberId
	// readChildren 
	// readFileName
 void Waiting::saveReadRecordStart(string bookid, bool isView)
 {//开始阅读第一页
	 if (!App::GetInstance()->m_me || isView)
		 return;
	 int childrenid = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);	 
	 string time = App::GetInstance()->GetStringTime2();
	 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "readBookId", -999, bookid);
	 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "readStartTime", -999, time);
	 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "readEndTime", -999, time);
	 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "readMemberId", App::GetInstance()->m_me->id);
	 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "readChildren", childrenid);
	 string path = FileUtils::getInstance()->getWritablePath() + "readBookRecord/" + StringUtils::format("%d.json", (int)YYXLayer::getRandom());
	 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "readFileName", -999, path);

	 map<string, string> parater;
	 parater["readBookId"] = bookid;
	 parater["readMemberId"] = StringUtils::format("%d", App::GetInstance()->m_me->id);
	 parater["readChildren"] = StringUtils::format("%d", childrenid);
	 parater["readStartTime"] = time;
	 parater["readEndTime"] = time;
	 string josn = YYXLayer::getStringFormMap(parater);
	 YYXLayer::writeFile(josn, path);
 }

 void Waiting::saveReadRecording(string bookid, bool isView)
 {//中间翻页
	 if (!App::GetInstance()->m_me || isView)
		 return;
	 string readFileName = YYXStruct::getMapString(App::GetInstance()->myData, "readFileName", "");
	 string readBookId = YYXStruct::getMapString(App::GetInstance()->myData, "readBookId", "");
	 int readMemberId = YYXStruct::getMapInt64(App::GetInstance()->myData, "readMemberId", -999);
	 int readChildren = YYXStruct::getMapInt64(App::GetInstance()->myData, "readChildren", -999);
	 string readStartTime = YYXStruct::getMapString(App::GetInstance()->myData, "readStartTime", "");
	 map<string, string> parater;
	 parater["readBookId"] = readBookId;
	 parater["readMemberId"] = StringUtils::format("%d", readMemberId);
	 parater["readChildren"] = StringUtils::format("%d", readChildren);
	 parater["readStartTime"] = readStartTime;
	 parater["readEndTime"] = App::GetInstance()->GetStringTime2();
	 string json = YYXLayer::getStringFormMap(parater);
	 YYXLayer::writeFile(json, readFileName);
 }

 void Waiting::saveReadRecordEnd(string bookid, bool isView)
 {//结束阅读最后一页
	 if (!App::GetInstance()->m_me || isView)
		 return;
	 string readFileName = YYXStruct::getMapString(App::GetInstance()->myData, "readFileName", "");
	 string readBookId = YYXStruct::getMapString(App::GetInstance()->myData, "readBookId", "");
	 int readMemberId = YYXStruct::getMapInt64(App::GetInstance()->myData, "readMemberId", -999);
	 int readChildren = YYXStruct::getMapInt64(App::GetInstance()->myData, "readChildren", -999);
	 string readStartTime = YYXStruct::getMapString(App::GetInstance()->myData, "readStartTime", "");
	 map<string, string> parater;
	 parater["readBookId"] = readBookId;
	 parater["readMemberId"] = StringUtils::format("%d", readMemberId);
	 parater["readChildren"] = StringUtils::format("%d", readChildren);
	 parater["readStartTime"] = readStartTime;
	 parater["readEndTime"] = App::GetStringTime2();
	 string josn = YYXLayer::getStringFormMap(parater);
	 YYXLayer::writeFile(josn, readFileName);
	 YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "readBookId");
	 YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "readStartTime");
	 YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "readEndTime");
	 YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "readMemberId");
	 YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "readChildren");
 }

 //试读封底
 Layer* Waiting::tryReadBackCover(int bookid)
 {
	 App::httpComment(bookid, []() {
		 YYXLayer::sendNotify("showCommentListView", "", -1);
	 });
	 map<string, int64String> parameter;
	 YYXLayer::insertMap4ParaType(parameter, "className", -999, "tryReadBackCover");
	 YYXLayer::insertMap4ParaType(parameter, "csb", -999, "Book/csb/tryReadBookEndPage.csb");
	 auto layer = YYXLayer::create(parameter);
	 layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	 layer->getParentNode()->setPosition(Director::getInstance()->getWinSize() / 2);
	 auto goBuy = (Button*)layer->findControl("Button_2");
	 auto vipDownload = (Button*)layer->findControl("Button_2_0"); 
	 auto noComment = (Sprite*)layer->findControl("Sprite_4");
	 auto noCommenttext = (Text*)layer->findControl("Text_2");
	 auto listComment = (ListView*)layer->findControl("ListView_1");
	 auto yaoqing = (Button*)layer->findControl("Button_1");
	 listComment->setVisible(false);
	 listComment->setScrollBarEnabled(false);
	 //初始化界面
	 if (yaoqing) {
		 yaoqing->setAnchorPoint(Vec2(1, 0));
		 yaoqing->setPosition(Vec2((1094 + Director::getInstance()->getVisibleSize().width) / 2, 50));
		 yaoqing->addClickEventListener([=](Ref* sender) {
			 if (App::GetInstance()->m_me)
			 {
				 App::GetInstance()->stopOtherVoice();
				 YYXTableView::stopAllAnimation();
				 string url = string("http://ellabook.cn/ellaBook-invite-red/index.html?memberId=").append(App::getMemberID());
				 NetIntface::inviteRegister(App::GetInstance()->m_me->id, url, "", [](string json) {}, "", [](string str) {});
			 }
			 else
				 Toast::create(App::getString("SHANGWEIDENGLU"));
		 });
	 }	 
	 if (App::GetInstance()->versioncode <= 172)
	 {
		 auto listenershowCommentListViewend = EventListenerCustom::create("showCommentListViewEND", [=](EventCustom* e) {
			 App::log("showCommentListViewEND^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
			 int count = (int)e->getUserData();
			 if (count > 0)
			 {
				 noComment->setVisible(false);
				 noCommenttext->setVisible(false);
			 }
			 else
			 {
				 noComment->setVisible(true);
				 noCommenttext->setVisible(true);
				 noCommenttext->setText(App::getString("HAIMEIYOURENPINGLUNGUOCISHU"));
				 listComment->setVisible(false);
			 }
		 });
		 Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenershowCommentListViewend, layer);
	 }
	if(App::GetInstance()->versioncode <=172)
	{
		YYXLayer::showCommentListView(listComment, bookid, -1, 25 * 3, 600, 100 * 3, 500);
		if (listComment->getItems().size() > 0)
		{
			noComment->setVisible(false);
			noCommenttext->setVisible(false);
		}
		else
		{
			noComment->setVisible(true);
			noCommenttext->setVisible(true);
			noCommenttext->setText(App::getString("HAIMEIYOURENPINGLUNGUOCISHU"));
			listComment->setVisible(false);
		}
	}
	else
	{
		listComment->setVisible(false);
		auto tbview = YYXTableView::create();
		auto data = tbview->loadData(bookid);
		listComment->setVisible(false);
		if (data.size() > 0)
		{
			noComment->setVisible(false);
			noCommenttext->setVisible(false);
			tbview->setPosition(Vec2(0, 0));
			tbview->setPosition(listComment->getPosition());
			tbview->setTag(159);
			layer->addChild(tbview);
		}
		else
		{
			noComment->setVisible(true);
			noCommenttext->setVisible(true);
			noCommenttext->setText(App::getString("HAIMEIYOURENPINGLUNGUOCISHU"));
		}
	}
	 //按钮
	 if (vipDownload)
	 {
		 vipDownload->setVisible(false);
		 vipDownload->addClickEventListener([=](Ref* sender) {
			 YYXLayer::controlTouchTime(1, "backCoverGoDownloadTime", [=]() {
				 //指示书籍详情的动作
				 App::GetInstance()->stopOtherVoice();
				 App::GetInstance()->playBackGroundMusic();
				 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ComeFromBookDoSomething", -999, "download");
				 Index::GoToBookInfo(bookid);
			 });
		 });
	 }
	 if (goBuy)
	 {
		 goBuy->setVisible(false);
		 goBuy->addClickEventListener([=](Ref* sender) {
			 YYXLayer::controlTouchTime(1, "backCoverGoBuyTime", [=]() {
				 //指示书籍详情的动作
				 App::GetInstance()->stopOtherVoice();
				 App::GetInstance()->playBackGroundMusic();
				 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ComeFromBookDoSomething", -999, "buy");
				 if (App::GetInstance()->m_me)
					 Index::GoToBookInfo(bookid);
				 else
				 {
					 App::cancelData();
					 App::GetInstance()->pushScene(BookInfoScene, bookid);
					 Index::GoToLoginScene();
				 }
			 });
		 });
	 }
	 if (App::GetInstance()->m_me)
	 {		
		 if (App::GetInstance()->m_me->vip)
			 vipDownload->setVisible(true);
		 else
			 goBuy->setVisible(true);			 
	 }
	 else
		 goBuy->setVisible(true);
	 return layer;
 }

 //封底
 Layer* Waiting::BackCover(int bookid)
 {
	 httpGetReadTime(bookid);
	 App::httpComment(bookid, []() {
		 YYXLayer::sendNotify("showCommentListView", "", -1);
	 });
	 map<string, int64String> parameter;
	 YYXLayer::insertMap4ParaType(parameter, "className", -999, "BookEndPage");
	 YYXLayer::insertMap4ParaType(parameter, "csb", -999, "Book/csb/BookEndPage.csb");
	 auto layer = YYXLayer::create(parameter);
	 layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	 layer->getParentNode()->setPosition(Director::getInstance()->getWinSize() / 2);
	 auto goComment = (Button*)layer->findControl("Button_2_1");
	 auto noComment = (Sprite*)layer->findControl("Sprite_4"); 
	 auto noCommenttext = (Text*)layer->findControl("Text_2_1"); 
	 auto listComment = (ListView*)layer->findControl("ListView_1");
	 auto yaoqing = (Button*)layer->findControl("Button_1");
	 auto jingcaiComment = (ImageView*)layer->findControl("Image_1");
	 auto MyComment = (ImageView*)layer->findControl("Image_2");
	 auto jingcaiText = (Text*)layer->findControl("Text_4");
	 auto mydeText = (Text*)layer->findControl("Text_4_0");
	 //auto readcount = (Text*)layer->findControl("Text_2");
	 //auto readtime = (Text*)layer->findControl("Text_2_0");
	 if (yaoqing) {
		 yaoqing->setAnchorPoint(Vec2(1, 0));
		 yaoqing->setPosition(Vec2((1094 + Director::getInstance()->getVisibleSize().width) / 2, 50));
		 yaoqing->addClickEventListener([=](Ref* sender) {
			 if (App::GetInstance()->m_me)
			 {
				 App::GetInstance()->stopOtherVoice();
				 YYXTableView::stopAllAnimation();
				 string url = string("http://ellabook.cn/ellaBook-invite-red/index.html?memberId=").append(App::getMemberID());
				 NetIntface::inviteRegister(App::GetInstance()->m_me->id, url, "", [](string json) {}, "", [](string str) {});
			 }
			 else
				 Toast::create(App::getString("SHANGWEIDENGLU"));
		 });
	 }
	 if (App::GetInstance()->versioncode <= 172)
	 {
		 if (listComment) {
			 listComment->setScrollBarEnabled(false);
			 YYXLayer::showCommentListView(listComment, bookid, -1, 25 * 3, 600, 100 * 3, 500);
			 if (listComment->getItems().size() > 0)
			 {
				 noComment->setVisible(false);
				 noCommenttext->setVisible(false);
			 }
			 else
			 {
				 noComment->setVisible(true);
				 noCommenttext->setVisible(true);
				 noCommenttext->setText(App::getString("HAIMEIYOURENPINGLUNGUOCISHU"));
				 listComment->setVisible(false);
			 }
		 }
		 listComment->setVisible(false);
	 }
	 else {
		 listComment->setVisible(false);
		 auto tbview = YYXTableView::create();
		 auto data = tbview->loadData(bookid);
		 listComment->setVisible(false);
		 if (data.size() > 0)
		 {
			 noComment->setVisible(false);
			 noCommenttext->setVisible(false);
			 tbview->setPosition(listComment->getPosition());
			 tbview->setTag(159);
			 layer->addChild(tbview);
		 }
		 else
		 {
			 noComment->setVisible(true);
			 noCommenttext->setVisible(true);
			 noCommenttext->setText(App::getString("HAIMEIYOURENPINGLUNGUOCISHU"));
		 }
	 }
	 
	//初始化界面 
	//阅读记录
		 //int childID = (int)YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
		 //string readtimekey = StringUtils::format("bookid=%d+childid=%d+memberid=%s+readtime", bookid, childID, App::getMemberID().c_str());
		 //string frequencykey = StringUtils::format("bookid=%d+childid=%d+memberid=%s+frequency", bookid, childID, App::getMemberID().c_str());
		 //if (readcount)
			// readcount->setText(StringUtils::format("%d", (int)YYXStruct::getMapInt64(App::GetInstance()->myData, frequencykey, 1)));
		 //if (readtime)
			//readtime->setText(StringUtils::format("%d", (int)YYXStruct::getMapInt64(App::GetInstance()->myData, readtimekey, 1)));
	//精彩评论
		if (jingcaiComment) {
		jingcaiComment->setTag(1);
		jingcaiComment->loadTexture("Book/res/Backcover_comments_press_736h.png", TextureResType::PLIST);
		jingcaiComment->setTouchEnabled(true);
		jingcaiComment->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "jingcaiCommentTime", [=]() {
				jingcaiComment->loadTexture("Book/res/Backcover_comments_press_736h.png", TextureResType::PLIST);
				MyComment->loadTexture("Book/res/Backcover_comments_736h.png", TextureResType::PLIST);
				jingcaiComment->setTag(1);
				jingcaiText->setTextColor(Color4B(187, 93, 39, 255));
				mydeText->setTextColor(Color4B(255, 255, 255, 255));
				MyComment->setTag(0);
				noComment->setVisible(false);
				noCommenttext->setVisible(false);
				App::GetInstance()->stopOtherVoice();
				YYXTableView::stopAllAnimation();				
				YYXLayer::sendNotify("showCommentListView", "444444444444444444", -1);
			});
		});
	}
			 //评论成功
	 	 auto ListenerClickComment = EventListenerCustom::create("bookinfoSceneHttpSendCommentSuccess", [=](EventCustom* e) {
		 int memberid = -1;
		 if (MyComment->getTag() == 1)
			 memberid= atoi(App::GetInstance()->getMemberID().c_str());
		 App::httpComment(bookid, [=]() {
			 YYXLayer::sendNotify("showCommentListView", "", memberid);
		 });
	 });
	 Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(ListenerClickComment, layer);
	 //刷新列表
	 auto listenerRefersh = EventListenerCustom::create("showCommentListView", [=](EventCustom* e) {
		 int memberid = (int)e->getUserData();
		 App::log(" --------------     run showCommentListView()   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^memberid = ", memberid);
		 App::GetInstance()->stopOtherVoice();
		 YYXTableView::stopAllAnimation();
		 if (App::GetInstance()->versioncode <= 172)
		 {
			 YYXLayer::showCommentListView(listComment, bookid, memberid, 25 * 3, 600, 100 * 3, 500);
		 }
		 else
		 {
			 auto tbview = (YYXTableView*)layer->getChildByTag(159);
			 if (tbview == nullptr)
			 {
				 tbview = YYXTableView::create();
				 tbview->setTag(159);
				 tbview->setPosition(listComment->getPosition());
				 layer->addChild(tbview);
			 }
			 auto data = tbview->loadData(bookid, memberid);
			 if (data.size() > 0)
			 {
				 noComment->setVisible(false);
				 noCommenttext->setVisible(false);
			 }
			 else
			 {
				 noComment->setVisible(true);
				 noCommenttext->setVisible(true);
				 if (memberid == -1)
					 noCommenttext->setText(App::getString("HAIMEIYOURENPINGLUNGUOCISHU"));
				 else
					 noCommenttext->setText(App::getString("NIHAIMEIYOUPINGLUNGUOCISHU"));
			 }
		 }
	 });
	 Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerRefersh, layer);
	 if (App::GetInstance()->versioncode <= 172)
	 {
		 auto listenershowCommentListViewend = EventListenerCustom::create("showCommentListViewEND", [=](EventCustom* e) {
			 int count = (int)e->getUserData();

			 if (count > 0)
				 noComment->setVisible(false);
			 else
			 {
				 noComment->setVisible(true);
				 listComment->setVisible(false);
			 }
		 });
		 Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenershowCommentListViewend, layer);
	 }
	 //刷新阅读时间
	 //auto listenersumReadTime = EventListenerCustom::create("sumReadTime", [=](EventCustom* e) {
		// int time = (int)e->getUserData();
		// if (time <= 0)
		//	 time = 3;
		// readtime->setText(StringUtils::format("%d", time));
		// string key = StringUtils::format("bookid=%d+childid=%d+memberid=%s+readtime", bookid, 
		//	 YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999), App::getMemberID().c_str());
		// YYXStruct::initMapYYXStruct(App::GetInstance()->myData, key, time);
	 //});
	 //Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenersumReadTime, readtime);
	 //auto listenerfrequency = EventListenerCustom::create("frequency", [=](EventCustom* e) {
		// int time = (int)e->getUserData();
		// if (time <= 0)
		//	 time = 1;
		// readcount->setText(StringUtils::format("%d", time));
		// string key = StringUtils::format("bookid=%d+childid=%d+memberid=%s+frequency", bookid,
		//	 YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999), App::getMemberID().c_str());
		// YYXStruct::initMapYYXStruct(App::GetInstance()->myData, key, time);
	 //});
	 //Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerfrequency, readcount);
	 
	 //我的评论
	 if (MyComment) {
		 MyComment->setTag(0);
		 MyComment->loadTexture("Book/res/Backcover_comments_736h.png", TextureResType::PLIST);
		 MyComment->setTouchEnabled(true);
		 MyComment->addClickEventListener([=](Ref* sender) {
			 YYXLayer::controlTouchTime(1, "MyCommentTime", [=]() {
				 jingcaiComment->loadTexture("Book/res/Backcover_comments_736h.png", TextureResType::PLIST);
				 MyComment->loadTexture("Book/res/Backcover_comments_press_736h.png", TextureResType::PLIST);
				 mydeText->setTextColor(Color4B(187, 93, 39, 255));
				 jingcaiText->setTextColor(Color4B(255, 255, 255, 255));
				 jingcaiComment->setTag(0);
				 MyComment->setTag(1);
				 noComment->setVisible(false);
				 App::GetInstance()->stopOtherVoice();
				 YYXTableView::stopAllAnimation();				
				 YYXLayer::sendNotify("showCommentListView", "", atoi(App::GetInstance()->getMemberID().c_str()));
			 });
		 });
	 }
	 //按钮
	 if (goComment) {
		 goComment->setVisible(true);
		 goComment->addClickEventListener([=](Ref* sender) {
			 YYXLayer::controlTouchTime(1, "backCoverGoCommentTime", [=]() {
				 Director::getInstance()->getRunningScene()->addChild(SendComment::create(bookid));
				 App::GetInstance()->stopOtherVoice();
				 YYXTableView::stopAllAnimation();				 
			 });
		 });
		 if (App::GetInstance()->m_me)
			 goComment->setVisible(true);
	 }
	 return layer;
 }
 

 void Waiting::httpGetReadTime(int bookId)
 {
	 int childid = (int)YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
	 string url = string(IP).append("/ellabook-server/member/getReadTimeandfrequency?memberId=").append(App::getMemberID()).append("&childrenID=").
		 append(StringUtils::format("%d", childid)).
		 append("&bookId=").append(StringUtils::format("%d", bookId)).append("&resource=").append(App::m_resource);
	 NetIntface::httpGet(url, "httpGetReadTimeSuccess", [=](string json) {
		/*{"result":true,"data":[{"sumReadTime":"0","frequency":"0"}],"code":0}*/
		 rapidjson::Document doc;
		 if (YYXLayer::getJsonObject4Json(doc, json))
		 {
			 int code = YYXLayer::getIntForJson(1, doc, "code");
			 if (code == 0)
			 {
				 rapidjson::Value arrayData;
				 YYXLayer::getJsonArray4Json(arrayData, doc, "data");
				 YYXLayer::getDataForJsonArray(arrayData, [=](rapidjson::Value & item, int index){
					 auto sumReadTime = YYXLayer::getString4Json("", item,  "sumReadTime");
					 auto frequency = YYXLayer::getString4Json("", item,  "frequency");
					 YYXLayer::sendNotify("sumReadTime", "", atoi(sumReadTime.c_str()));
					 YYXLayer::sendNotify("frequency", "", atoi(frequency.c_str()));
					 string readtimekey = StringUtils::format("bookid=%d+childid=%d+memberid=%s+readtime", bookId, childid, App::getMemberID().c_str());
					 string frequencykey = StringUtils::format("bookid=%d+childid=%d+memberid=%s+frequency", bookId, childid, App::getMemberID().c_str());
					 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, readtimekey, atoi(sumReadTime.c_str()));
					 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, frequencykey, atoi(frequency.c_str()));
				 });
			 }
		 }
	 }, "", [](string err) {});
 }

 void Waiting::initResource(Layer* layer)
 {
	 Size winSize = Director::getInstance()->getWinSize();
	 float highScale = winSize.height / 1536;
	 auto pageUp = MenuItemImage::create("ReadBook_PageUp_ipad.png",
		 "ReadBook_PageUp_Selected_ipad.png",
		 [=](Ref* sender) {
		 YYXLayer::controlTouchTime(1, "ReadTime", [=]() {
			 Director::getInstance()->replaceScene(TransitionPageTurn::create(1.0f, PageLayer::createScene(), true));
			 //Director::getInstance()->popScene();
			 //BookParser::getInstance()->pageUp();
		 });
	 });
	 pageUp->setPosition(-winSize.width / 2, -winSize.height / 2);
	 pageUp->setScale(highScale*0.63);
	 pageUp->setAnchorPoint(Vec2(0, 0));
	 pageUp->setTag(BUTTON_TAG_PAGE_UP);


	 auto pageDown = MenuItemImage::create("ReadBook_PageDown_ipad.png",
		 "ReadBook_PageDown_Selected_ipad.png",
		 [](Ref* sender) {
		 //Director::getInstance()->popScene();
		 BookParser::getInstance()->pageDown();
	 });
	 pageDown->setPosition(winSize.width / 2,  - winSize.height / 2);
	 pageDown->setScale(highScale*0.63);
	 pageDown->setAnchorPoint(Vec2(1.0f, 0));
	 pageDown->setTag(BUTTON_TAG_PAGE_DOWN);

	 auto pageQuit = MenuItemImage::create("Common_Return_ipad@2x.png",
		 "Common_Return_Selected_ipad@2x.png",
		 [](Ref* sender) {
		 BookParser::getInstance()->bookQuit();
		 App::GetInstance()->popBack();
		 Index::BackPreviousScene();
		 YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "UserIsReadingBook", 0);
		 App::GetInstance()->playBackGroundMusic();
	 });
	 pageQuit->setPosition(-winSize.width / 2, winSize.height / 2);
	 pageQuit->setScale(highScale*0.8);
	 pageQuit->setAnchorPoint(Vec2(0, 1.0f));
	 pageQuit->setTag(BUTTON_TAG_PAGE_QUIT);

	 Menu *menuPage = Menu::create(pageUp,  pageQuit,  NULL);
	 layer->addChild(menuPage, 100, 10000);
	 BookParser::getInstance()->setPageMenu(menuPage);
 }