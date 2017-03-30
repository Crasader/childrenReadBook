#include "CityButton.h"
#include "NetIntface.h"
#include "YYXDownloadImages.h"

#define TAGNOTIFY "NotifyCityButtonImage%d"
#define FORMAT_IMAGENAME "bg_%d.png"

CityButton* CityButton::create(string url, int memberId,string defaultPath)
{
	CityButton *widget = new (std::nothrow) CityButton;
	if (widget && widget->init(url, memberId, defaultPath))
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

bool CityButton::init(string url, int storeId, string defaultPath)
{
	bool bRet = true;
	if (!ImageView::init())
	{
		bRet = false;
	}
	downloadDir = FileUtils::getInstance()->getWritablePath() + "bookCity";
	this->url = url;
	this->storeId = storeId;
	this->defaultPath = defaultPath;
	auto lister = EventListenerCustom::create(StringUtils::format(TAGNOTIFY, storeId), [=](EventCustom* e) {
		loadImage();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister, this);
	string fileName = StringUtils::format(FORMAT_IMAGENAME, storeId);
	string path = downloadDir + "/" + fileName;
	if (FileUtils::getInstance()->isFileExist(path))
		this->loadTexture(path);
	else
	{
		downloadImage();
		if (FileUtils::getInstance()->isFileExist(defaultPath))
			this->loadTexture(defaultPath);
	}
	if (mycallback)
	{
		this->setTouchEnabled(true);
		this->addClickEventListener(mycallback);
	}
	return bRet;
}
//重新加载新的网络图片
void CityButton::reloadUrl(string url, int storeId)
{
	auto oldmemberId = this->storeId;
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format(TAGNOTIFY, oldmemberId));
	this->storeId = storeId;
	this->url = url;
	auto lister = EventListenerCustom::create(StringUtils::format(TAGNOTIFY, storeId), [=](EventCustom* e) {
		loadImage();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister, this);	
	string fileName = StringUtils::format(FORMAT_IMAGENAME, storeId);
	string path = downloadDir + "/" + fileName;
	if (FileUtils::getInstance()->isFileExist(path))
		this->loadTexture(path);
	else
	{
		downloadImage();
		if (FileUtils::getInstance()->isFileExist(defaultPath))
			this->loadTexture(defaultPath);
	}
	if (mycallback)
	{
		this->setTouchEnabled(true);
		this->addClickEventListener(mycallback);
	}
}

void CityButton::addClickListener(const function<void(Ref*)>& callback)
{
}

void CityButton::loadImage()
{
	string fileName = StringUtils::format(FORMAT_IMAGENAME, storeId);
	string path = downloadDir + "/" + fileName;
	if (FileUtils::getInstance()->isFileExist(path))
		this->loadTexture(path);
	else
	{
		if (FileUtils::getInstance()->isFileExist(defaultPath))
			this->loadTexture(defaultPath);
	}
}

void CityButton::downloadImage()
{
	if (url.empty() || storeId <0)
		return;
	YYXDownloadImages::GetInstance()->newDownloadImage(url, downloadDir, StringUtils::format(FORMAT_IMAGENAME, storeId),
		normal, 0, [=](string path) {
		YYXLayer::sendNotify(StringUtils::format(TAGNOTIFY, storeId));
	});
}