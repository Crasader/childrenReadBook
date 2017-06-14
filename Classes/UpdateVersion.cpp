#include "UpdateVersion.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocostudio::timeline;

UpdateVersion::UpdateVersion() {
	//m_curVersion = "";
	//m_Version = "";
}

UpdateVersion::~UpdateVersion() {
	_eventDispatcher->removeCustomEventListeners("UpdataVersionDownloading");
	_eventDispatcher->removeCustomEventListeners("UpdataVersionDownload_Success");
}

UpdateVersion* UpdateVersion::create() {
	YYXLayer::logb("UpdateVersion::create() ");
	UpdateVersion *pRect = new UpdateVersion();
	if (pRect) {
		pRect->init();
		pRect->autorelease();
	}
	else {
		pRect->release();
		pRect = nullptr;
	}
	YYXLayer::loge("UpdateVersion::create() ");
	return pRect;
}

void UpdateVersion::onEnterTransitionDidFinish() {
	string url = "http://download.ellabook.cn/android/ellabook-latest.apk";
	App::log(url);
	string version = YYXStruct::getMapString(App::GetInstance()->myData, "netVersion", "");
	string dir = CrossPlatform::getDownloadDir();
	string fileName = "www_ellabook_cn.apk";
	string path = dir + "/" + fileName;
	auto dat = DownLoadFileData::create();
	dat->setUrl(url);
	dat->setDir(dir);
	dat->setFileName(fileName);
	dat->setBeginFunc([](DownLoadFileData* ddata) {
		string path = ddata->getPath();
		if (FileUtils::getInstance()->isFileExist(path))
			FileUtils::getInstance()->removeFile(path);
	});
	dat->setDownloadingFunc([](DownLoadFileData* ddata) {
		int progessing = ddata->getPausePint();
		YYXLayer::sendNotify("UpdataVersionDownloading", "UpdateVersion", progessing);
	});
	dat->setEndFunc([](DownLoadFileData* ddata) {
		if (ddata->getStatus() == _over)
		{
			string path = ddata->getPath();
			auto total = ddata->getSize();
			if (total > 0)
			{
				auto size = FileUtils::getInstance()->getFileSize(path);
				if (size == total)
				{
					CrossPlatform::installInstallationPackage(path);
				}
			}
			YYXLayer::sendNotify("UpdataVersionDownload_Success");
		}
	});
	YYXDownload::getInstance()->newDownloadFile(dat);	
}

bool UpdateVersion::init() {
	if (!Layer::init())
	{
		return false;
	}
	/*vector<string> paraStr;
	paraStr.push_back("appversionA");
	paraStr.push_back("appversionB");
	paraStr.push_back("appversionC");
	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_APPINFO, paraStr, "");
	App::sqliteClose;
	//当前APP的版本号
	int A = result[0]["appversionA"].intPara;
	int B = result[0]["appversionB"].intPara;
	int C = result[0]["appversionC"].intPara;
	m_curVersion = StringUtils::format("%d.%d.%d",A,B,C);*/
	//m_Version = UserDefault::getInstance()->getStringForKey("NetEllaVersion");
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto pLayer = (Layer*)CSLoader::createNode("Notification/csb/update.csb");
	pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(pLayer);
	//触摸屏蔽层
	auto bg_ImgView = ( ImageView*)pLayer->getChildByName("bg_ImgView");
	bg_ImgView->setTouchEnabled(true);

	//内容文本
	auto m_curVersion = App::GetInstance()->version;
	auto text_info1 =  Text::create();
	text_info1->setAnchorPoint(Vec2(0, 0.5));
	text_info1->setColor(ccc3(111, 166, 125));
	text_info1->setString(m_curVersion);
	text_info1->setPosition(Vec2(440, 430));
	text_info1->setFontSize(48);
	text_info1->setScale(0.5f);
	pLayer->addChild(text_info1);
	auto text_info2 =  Text::create();
	text_info2->setAnchorPoint(Vec2(0, 0.5));
	text_info2->setColor(ccc3(111, 166, 125));
	string version = YYXStruct::getMapString(App::GetInstance()->myData, "netVersion", "");
	text_info2->setString(version);
	text_info2->setPosition(Vec2(440, 398));
	text_info2->setFontSize(48);
	text_info2->setScale(0.5f);
	pLayer->addChild(text_info2);

	//进度条
	auto progressBar = (LoadingBar*)pLayer->getChildByName("progressBar");
	progressBar->setPercent(0);

	//后台运行按钮
	auto button = (Button*)pLayer->getChildByName("button");
	button->setTouchEnabled(true);
	button->addClickEventListener([=](Ref* sender) {
		pLayer->removeAllChildrenWithCleanup(true);
		pLayer->removeFromParentAndCleanup(true);
	});

	//下载进度通知
	auto listenDownloading = EventListenerCustom::create("UpdataVersionDownloading", [=](EventCustom* event) {
		//char* buf = static_cast<char*>(event->getUserData());
		//progressBar->setPercent(atoi(buf));
		int rateofprogress = (int)event->getUserData();
		if(progressBar)
			progressBar->setPercent(rateofprogress);
	});
	//下载成功事件注册
	auto listenDownSuccess = EventListenerCustom::create("UpdataVersionDownload_Success", [=](EventCustom* event) {
		removeAllChildrenWithCleanup(true);
		removeFromParentAndCleanup(true);
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenDownloading, progressBar);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenDownSuccess, this);
	return true;
}