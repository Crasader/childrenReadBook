#include "YYXImageView.h"
#include "CrossPlatform.h"
#include "YYXDownloadImages.h"

YYXImageView* YYXImageView::create(string url, int memberId,string defaultPath)
{
	YYXImageView *widget = new (std::nothrow) YYXImageView;
	if (widget && widget->init(url, memberId, defaultPath))
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

bool YYXImageView::init(string url, int memberId, string defaultPath)
{
	bool bRet = true;
	do
	{
		if (!ImageView::init())
		{
			bRet = false;
			break;
		}
	} while (0);
	this->url = url;
	this->memberId = memberId;
	this->defaultPath = defaultPath;
	auto lister = EventListenerCustom::create(StringUtils::format("NotifyUserImage%d", memberId), [=](EventCustom* e) {
		loadImage();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister, this);
	string dir = FileUtils::getInstance()->getWritablePath() + "temp";
	string fileName = StringUtils::format("user_%d.png", memberId);
	string path = dir + "/" + fileName;
	if (FileUtils::getInstance()->isFileExist(path))
		this->loadTexture(path);
	else
	{
		downloadImage();
		if (FileUtils::getInstance()->isFileExist(defaultPath))
			this->loadTexture(defaultPath);
	}
	return bRet;
}
//重新加载新的网络图片
void YYXImageView::reloadUrl(string url, int memberId)
{
	auto oldmemberId = this->memberId;
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format("NotifyUserImage%d", oldmemberId));
	this->memberId = memberId;
	this->url = url;
	auto lister = EventListenerCustom::create(StringUtils::format("NotifyUserImage%d", memberId), [=](EventCustom* e) {
		loadImage();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister, this);
	string dir = FileUtils::getInstance()->getWritablePath() + "temp";
	string fileName = StringUtils::format("user_%d.png", memberId);
	string path = dir + "/" + fileName;
	if (FileUtils::getInstance()->isFileExist(path))
		this->loadTexture(path);
	else
	{
		downloadImage();
		if (FileUtils::getInstance()->isFileExist(defaultPath))
			this->loadTexture(defaultPath);
	}
}

void YYXImageView::loadImage()
{
	string dir = FileUtils::getInstance()->getWritablePath() + "temp";
	string fileName = StringUtils::format("user_%d.png", memberId);
	string path = dir + "/" + fileName;
	if (FileUtils::getInstance()->isFileExist(path))
		this->loadTexture(path);
	else
	{
		if (FileUtils::getInstance()->isFileExist(defaultPath))
			this->loadTexture(defaultPath);
	}
}

void YYXImageView::downloadImage()
{
	if (url == "" || memberId == -999)
		return;
	YYXDownloadImages::GetInstance()->newDownloadImage(url, FileUtils::getInstance()->getWritablePath() + "temp", StringUtils::format("user%d.png", memberId), 
		normal, 0, [=](string path) {
		string yuanPath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("user_%d.png", memberId);
		FileUtils::getInstance()->removeFile(yuanPath);
		CrossPlatform::cutTheRounded(path, yuanPath, 140, 140, "", [=](string yuandepath) {
			YYXLayer::sendNotify(StringUtils::format("NotifyUserImage%d", memberId));
		}, "",[=](string str) {
			YYXLayer::sendNotify(StringUtils::format("NotifyUserImage%d", memberId));
		});
	});
	/*NetIntface::DownLoadImage(url, FileUtils::getInstance()->getWritablePath() + "temp", StringUtils::format("user%d.png", memberId), "", [=](string path) {
		string yuanPath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("user_%d.png", memberId);
		FileUtils::getInstance()->removeFile(yuanPath);
		NetIntface::cutTheRounded(path, yuanPath, 140, 140, "", [=](string yuandepath) {
			YYXLayer::sendNotify(StringUtils::format("NotifyUserImage%d", memberId));
		}, "", [=](string str) {
			YYXLayer::sendNotify(StringUtils::format("NotifyUserImage%d", memberId));
		});
	}, "", [](string error) {});*/
}

void YYXImageView::setUrl(string url)
{
	this->url = url;
}

std::string YYXImageView::getUrl()
{
	return url;
}

void YYXImageView::setMemberId(int id)
{
	this->memberId = id;
}

int YYXImageView::getMemberId()
{
	return memberId;
}

