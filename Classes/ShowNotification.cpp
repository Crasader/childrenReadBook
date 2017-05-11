#include "ShowNotification.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "YYXDownloadImages.h"
//#include "ControlScene.h"

USING_NS_CC;

using namespace cocostudio::timeline;

ShowNotification::ShowNotification() {
	m_tag = false;
}

ShowNotification::~ShowNotification() {
}

ShowNotification* ShowNotification::create(bool tag) {
	ShowNotification *pRect = new ShowNotification();
	if (pRect) {
		pRect->m_tag = tag;
		pRect->init();
		pRect->autorelease();
	}
	else {
		pRect->release();
		pRect = nullptr;
	}
	return pRect;
}

bool ShowNotification::init() {
	if (!Layer::init())
	{
		return false;
	}

	if (m_tag) {
		initVersion();
		return true;
	}
	auto imageUrl = YYXStruct::getMapString(App::GetInstance()->myData, "imageUrl", "");
	//if (App::GetInstance()->m_notification->imageUrl.length() == 0) 
	if(imageUrl == "")
		initWord();
	else
		initActivity();
	return true;
}

//图片信息页面
void ShowNotification::initActivity() {
	App::log("------------initActivity");
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto pLayer = (Layer*)CSLoader::createNode("Notification/csb/activity.csb");
	pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(pLayer);
	
	//触摸屏蔽层
	auto bg_ImgView = (ImageView*)pLayer->getChildByName("bg_ImgView");
	bg_ImgView->setTouchEnabled(true);

	//不再提醒选中图片
	auto sprSlected = (Sprite*)pLayer->getChildByName("selected");
	sprSlected->setVisible(false);

	//不再提醒
	auto noTipAgain = (Text*)pLayer->getChildByName("Text_1");
	noTipAgain->setTouchEnabled(true);
	noTipAgain->addClickEventListener([=](Ref* sender) {
		sprSlected->setVisible(!sprSlected->isVisible());
	});

	//图片-内容
	auto content_ImgView = (ImageView*)pLayer->getChildByName("imgView");
	content_ImgView->setTouchEnabled(true);
	content_ImgView->addClickEventListener([=](Ref* sender) {//打开跳转链接
		if (sprSlected->isVisible()) {
			//记录本地，下次将不会推送此消息
			int pushId = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushId", -999);
			YYXLayer::setFileValue("notification_push_id", StringUtils::format("%d", pushId));
		}
		int pushType = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushType", -999);
		//if (App::GetInstance()->m_notification->pushType == 2) {//新书
		if(pushType == 2)
		{
			//Index::GoToNewBook();
			auto control = ControlScene::getInstance();
			control->replaceScene(control->getCurrentScene(), ControlScene::getInstance()->getSceneInfo(NewBook));
			return;
		}
		//弹出网页
		auto pushUrl = YYXStruct::getMapString(App::GetInstance()->myData, "pushUrl", "");
		CocosAndroidJni::GotoUrl(pushUrl.c_str());
		//CocosAndroidJni::GotoUrl(App::GetInstance()->m_notification->pushUrl.c_str());
		//App::log("pushUrl-----" + App::GetInstance()->m_notification->pushUrl);
		setVisible(false);	
	});

	//关闭按钮
	auto close_btn = (Button*)pLayer->getChildByName("close_btn");
	close_btn->setTouchEnabled(true);
	close_btn->addClickEventListener([=](Ref* sender) {
		if (sprSlected->isVisible()) {
			//记录本地，下次将不会推送此消息
			//YYXLayer::setFileValue("notification_push_id", StringUtils::format("%d", App::GetInstance()->m_notification->pushId));
			int pushId = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushId", -999);
			YYXLayer::setFileValue("notification_push_id", StringUtils::format("%d", pushId));
		}
		pLayer->removeFromParent();
	});

	//下载图片完成通知
	auto listenLoadPic = EventListenerCustom::create("Download_Success_Show_Notification", [=](EventCustom* event) {
		auto userData = (YYXStruct*)event->getUserData();
		string path = YYXStruct::getStringData(*userData, "");
		if (FileUtils::getInstance()->isFileExist(path))
			content_ImgView->loadTexture(path);
		if (userData)
			delete userData;
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenLoadPic, content_ImgView);
	//下载图片
	//thread pDownPic(&ShowNotification::downloadPic, this);
	//pDownPic.detach();
	ShowNotification::downloadPic();
}

//文字信息页面
void ShowNotification::initWord() {
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "ShowNotification::initWord");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "Notification/csb/version.csb");
	auto pLayer = YYXLayer::create(paramter);
	if (!pLayer)
		return;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//auto pLayer = (Layer*)CSLoader::createNode("Notification/csb/version.csb");
	pLayer->getParentNode()->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->getParentNode()->setPosition(visibleSize / 2);
	addChild(pLayer);
	//触摸屏蔽层
	auto bg_ImgView = (ImageView*)pLayer->findControl("bg_ImgView");
	bg_ImgView->setTouchEnabled(true);
	//不再提醒选中图片
	auto sprSlected = (Sprite*)pLayer->findControl("selected");
	sprSlected->setVisible(false);
	//不再提醒
	auto noTipAgain = (ImageView*)pLayer->findControl("isNextTip");
	noTipAgain->setTouchEnabled(true);
	noTipAgain->addClickEventListener([=](Ref* sender) {
		sprSlected->setVisible(!sprSlected->isVisible());
	});
	//标题文本
	auto title_text = (Text*)pLayer->findControl("title");
	string pushTitle = YYXStruct::getMapString(App::GetInstance()->myData, "pushTitle", "");
	//if (App::GetInstance()->m_notification->pushTitle.length() != 0) 
	if(pushTitle != "")
	{
		title_text->setString(pushTitle);
		//title_text->setString(App::GetInstance()->m_notification->pushTitle);
	}
	auto text_info = (Text*)pLayer->findControl("text_info");
	text_info->setVisible(false);
	//内容文本
	string pushString = YYXStruct::getMapString(App::GetInstance()->myData, "pushString", "");
	//if (App::GetInstance()->m_notification->pushString.length() != 0) 
	if(pushString != "")
	{
		auto content_listview = ListView::create();
		content_listview->setDirection(SCROLLVIEW_DIR_VERTICAL);
		content_listview->setBounceEnabled(true);
		content_listview->setScrollBarEnabled(false);
		//content_listview->setAnchorPoint(Vec2(0, 1));
		//content_listview->setPosition(Vec2(275, 460));
		int x = 275-(1094 - visibleSize.width) / 2;
		content_listview->setPosition(Vec2(x, 155));
		content_listview->setSize(Size(545, 310));
		pLayer->addChild(content_listview);

		auto widget = Layout::create();
		auto test = Label::create();
		test->setTTFConfig(TTFConfig("hkhb.TTF", 25));
		test->setMaxLineWidth(545);
		//test->setString(App::GetInstance()->m_notification->pushString);
		test->setString(pushString);
		test->setAnchorPoint(Vec2(0, 0));
		test->setColor(ccc3(92, 183, 92));
		widget->addChild(test);

		widget->setContentSize(Size(545, test->getContentSize().height));
		content_listview->pushBackCustomItem(widget);
	}

	//关闭按钮
	auto close_btn = (Button*)pLayer->findControl("close_btn");
	close_btn->setTouchEnabled(true);
	close_btn->addClickEventListener([=](Ref* sender) {
		if (sprSlected->isVisible()) {
			//记录本地，下次将不会推送此消息
			int pushId = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushId", -999);
			//YYXLayer::setFileValue("notification_push_id", StringUtils::format("%d", App::GetInstance()->m_notification->pushId));
			YYXLayer::setFileValue("notification_push_id", StringUtils::format("%d", pushId));
		}
		pLayer->removeFromParent();
	});
	//更新按钮
	auto update_btn = (Button*)pLayer->findControl("update_btn");
	//前往按钮
	auto button = (Button*)pLayer->findControl("button");
	int pushType = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushType", -999);
	//if (App::GetInstance()->m_notification->pushType != 3) {
	if(pushType != 3)
	{
		update_btn->setTouchEnabled(false);
		update_btn->setVisible(false);
		button->setTouchEnabled(true);
		button->setVisible(true);
	}
	else
	{//3是版本更新
		update_btn->setTouchEnabled(true);
		update_btn->setVisible(true);
		button->setTouchEnabled(false);
		button->setVisible(false);
	}
	update_btn->addClickEventListener([=](Ref* sender) {
		if (sprSlected->isVisible()) {
			//记录本地，下次将不会推送此消息
			int pushId = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushId", -999);
			YYXLayer::setFileValue("notification_push_id", StringUtils::format("%d", pushId));
		}
		//跳下载更新页面
		auto pScene = Director::getInstance()->getRunningScene();
		pScene->addChild(UpdateVersion::create());
		setVisible(false);
	});
	button->addClickEventListener([=](Ref* sender) {		
		if (sprSlected->isVisible()) {
			//记录本地，下次将不会推送此消息
			int pushId = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushId", -999);
			YYXLayer::setFileValue("notification_push_id", StringUtils::format("%d", pushId));
		}
		int pushType = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushType", -999);
		if (pushType == 2)
		{//新书
			auto control = ControlScene::getInstance();
			control->replaceScene(control->getCurrentScene(), ControlScene::getInstance()->getSceneInfo(NewBook));
			return;
		}
		auto pushUrl = YYXStruct::getMapString(App::GetInstance()->myData, "pushUrl", "http://www.ellabook.cn");
		//string httpstr = "http://www.ellabook.cn";
		if (&pushUrl && !pushUrl.empty())
		{
			//弹出网页
			CocosAndroidJni::GotoUrl(pushUrl.c_str());
			setVisible(false);
		}
	});
}

//版本更新页面
void ShowNotification::initVersion() {
	YYXLayer::logb("ShowNotification::initVersion()");
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "upversion");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "Notification/csb/version.csb");
	auto pLayer = YYXLayer::create(paramter);
	if (!pLayer)
		return;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	pLayer->getParentNode()->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->getParentNode()->setPosition(visibleSize / 2);
	//触摸屏蔽层
	auto bg_ImgView = (ImageView*)pLayer->findControl("bg_ImgView");
	bg_ImgView->setTouchEnabled(true);
	//不再提醒选中图片
	auto sprSlected = (Sprite*)pLayer->findControl("selected");
	sprSlected->setVisible(false);
	//不再提醒未选中图片
	auto sprNoSlected = (Sprite*)pLayer->findControl("noSelected");
	sprNoSlected->setVisible(false);
	//不再提醒文本
	auto text_tip = (Sprite*)pLayer->findControl("Text_1");
	text_tip->setVisible(false);
	//不再提醒
	auto noTipAgain = (ImageView*)pLayer->findControl("isNextTip");
	noTipAgain->setTouchEnabled(false);
	noTipAgain->setVisible(false);
	//内容文本
	auto text_info1 = Text::create();
	text_info1->setAnchorPoint(Vec2(0, 0.5));
	text_info1->setColor(ccc3(111, 166, 125));
	text_info1->setString(App::GetInstance()->version);
	text_info1->setPosition(Vec2(440, 430));
	text_info1->setFontSize(48);
	text_info1->setScale(0.5f);
	pLayer->addChild(text_info1);
	auto text_info2 = Text::create();
	text_info2->setAnchorPoint(Vec2(0, 0.5));
	text_info2->setColor(ccc3(111, 166, 125));
	auto version = YYXStruct::getMapString(App::GetInstance()->myData, "netVersion", "");
	text_info2->setString(version);
	text_info2->setPosition(Vec2(440, 398));
	text_info2->setFontSize(48);
	text_info2->setScale(0.5f);
	pLayer->addChild(text_info2);

	//关闭按钮
	auto close_btn = (Button*)pLayer->findControl("close_btn");
	close_btn->setTouchEnabled(true);
	close_btn->addClickEventListener([=](Ref* sender) {
		pLayer->removeFromParent();
	});
	//前往按钮
	auto button = (Button*)pLayer->findControl("button");
	button->setVisible(false);
	button->setEnabled(false);
	//更新按钮
	auto update_btn = (Button*)pLayer->findControl("update_btn");
	update_btn->setTouchEnabled(true);
	update_btn->setPositionX(547.0f);
	update_btn->addClickEventListener([=](Ref* sender) {
		//跳下载更新页面
		auto show = UpdateVersion::create();
		if (show)
		{
			Director::getInstance()->getRunningScene()->addChild(show);
		}
		else
			Toast::create("UpdateVersion::create() ");
		setVisible(false);
	});
	addChild(pLayer);
	YYXLayer::loge("ShowNotification::initVersion()");
}

//下载图片
void ShowNotification::downloadPic() {
	auto imageUrl = YYXStruct::getMapString(App::GetInstance()->myData, "imageUrl", "");
	if (imageUrl != "")
	{
		auto dir = FileUtils::getInstance()->getWritablePath() + "temp";
		int pushId = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushId", -999);
		auto fileName = StringUtils::format("notification_%d.png", pushId);
		YYXDownloadImages::GetInstance()->newDownloadImage(imageUrl, dir, fileName, high, 0, [](string path) {
			YYXLayer::sendNotify4YYXStruct("Download_Success_Show_Notification", -999, path);
		});
		/*NetIntface::DownLoadImage(imageUrl, dir, fileName, "Download_Success_Show_Notification", [](string path) {
			YYXLayer::sendNotify4YYXStruct("Download_Success_Show_Notification", -999, path);
		}, "", [](string st) {});*/
	}
	//if (App::GetInstance()->m_notification == nullptr || App::GetInstance()->m_notification->imageUrl.length() == 0) {
	//	return;
	//}
	////网络请求图片
	//HttpRequest* pRequest = new HttpRequest();
	//pRequest->setUrl(App::GetInstance()->m_notification->imageUrl.c_str());
	//pRequest->setRequestType(HttpRequest::Type::GET);
	//pRequest->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
	//	if (!response) {
	//		return;
	//	}
	//	if (!response->isSucceed()) {
	//		return;
	//	}

	//	std::vector<char> *buffer = response->getResponseData();
	//	char* buf = (char*)malloc(buffer->size());
	//	std::copy(buffer->begin(), buffer->end(), buf);

	//	auto dir = FileUtils::getInstance()->getWritablePath() + "bookCover/";
	//	auto fileName = StringUtils::format("notification_%d.png", App::GetInstance()->m_notification->pushId);
	//	if (!FileUtils::getInstance()->createDirectory(dir)) {
	//		return;
	//	}
	//	FILE *fp = fopen((dir + fileName).c_str(), "wb+");
	//	fwrite(buf, 1, buffer->size(), fp);
	//	fclose(fp);

	//	if (buf != nullptr)
	//		free(buf);

	//	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {//下载成功操作
	//		EventCustom eventDownSuccess("Download_Success_Show_Notification");
	//		Director::getInstance()->getEventDispatcher()->setEnabled(true);
	//		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventDownSuccess);
	//	});
	//});
	//HttpClient::getInstance()->send(pRequest);
	//pRequest->release();
}