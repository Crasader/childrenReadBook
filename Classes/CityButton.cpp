#include "CityButton.h"
#include "CrossPlatform.h"
#include "YYXDownloadImages.h"

#define TAGNOTIFY "NotifyCityButtonImage%d"
#define FORMAT_IMAGENAME "bg_%d.png"

CityButton* CityButton::create(string url, int memberId, Size size,CityButtonType type2)
{
	CityButton *widget = new (std::nothrow) CityButton;
	if (widget && widget->init(url, memberId, size, type2))
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

bool CityButton::init(string url, int storeId, Size size, CityButtonType type2)
{
	bool bRet = true;
	if (!ImageView::init())
	{
		bRet = false;
	}
	m_size = size;
	m_type = type2;
	ignoreContentAdaptWithSize(false);
	setSize(m_size);
	downloadDir = FileUtils::getInstance()->getWritablePath() + "bookCity";
	this->url = url;
	this->storeId = storeId;
	this->defaultPath = defaultPath;
	auto lister = EventListenerCustom::create(StringUtils::format(TAGNOTIFY, storeId), [=](EventCustom* e) {
		loadImage();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister, this);
	loadImage();
	addBorder();
	addClickListener(storeId);
	return bRet;
}

void CityButton::addBorder()
{
	Sprite* im = new Sprite();
	if (m_type == _single)//大按钮
	{
		im->initWithSpriteFrameName("BookCity/res/img_937.png");
		im->setPosition(m_size / 2 + Size(-1, 4));
		im->setContentSize(Size(312, 392));
	}
	else//小按钮
	{
		//1-4之间 4种图片
		int cot = storeId % 4+1;
		im->initWithSpriteFrameName(StringUtils::format("BookCity/res/border%d.png",cot));
		im->setPosition(m_size / 2 - Size(0, 22));
		im->setScale(0.97);
	}
	addChild(im);
}

//重新加载新的网络图片
void CityButton::reloadUrl(string url, int storeId)
{
	if (storeId < 0)
		return;
	auto oldmemberId = this->storeId;
	addClickListener(storeId);
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(StringUtils::format(TAGNOTIFY, oldmemberId));
	auto lister = EventListenerCustom::create(StringUtils::format(TAGNOTIFY, storeId), [=](EventCustom* e) {
		loadImage();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister, this);
	this->storeId = storeId;
	this->url = url;
	loadImage();
}

void CityButton::addClickListener(int storeId)
{
	this->setTouchEnabled(true);
	this->setSwallowTouches(false);
	this->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "CityButtonTime", [=]() {
			YYXLayer::sendNotify("Listen_Turn_Action", "", storeId);
		});
	});
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("CityButtonAction_" + Value(this->getStoreId()).asString());
	auto listener = EventListenerCustom::create("CityButtonAction_"+Value(storeId).asString(), [=](EventCustom* e) {
		auto sc1 = ScaleTo::create(0.3f, 1.25);
		RotateTo* ro1 = RotateTo::create(0.3f, 10);
		Spawn* sp1 = Spawn::create(sc1, ro1, NULL);

		auto sc2 = ScaleTo::create(0.1f, 1.2);
		RotateTo* ro2 = RotateTo::create(0.1f, -8);
		Spawn* sp2 = Spawn::create(sc2, ro2, NULL);

		auto sc3 = ScaleTo::create(0.1f, 1.15);
		RotateTo* ro3 = RotateTo::create(0.1f, 6);
		Spawn* sp3 = Spawn::create(sc3, ro3, NULL);

		auto sc4 = ScaleTo::create(0.15f, 1.1);
		RotateTo* ro4 = RotateTo::create(0.15f, -4);
		Spawn* sp4 = Spawn::create(sc4, ro4, NULL);

		auto sc5 = ScaleTo::create(0.1f, 1.1);
		RotateTo* ro5 = RotateTo::create(0.1f, 2);
		Spawn* sp5 = Spawn::create(sc5, ro5, NULL);

		auto sc6 = ScaleTo::create(0.25f, 1);
		RotateTo* ro6 = RotateTo::create(0.25f, 0);
		Spawn* sp6 = Spawn::create(sc6, ro6, NULL);
		auto seq = Sequence::create(sp1, sp2, sp3, sp4, sp5, sp6, NULL);
		this->runAction(seq);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void CityButton::loadImage()
{
	string plistFileName = StringUtils::format("BookCity/res/bg_%d.png", storeId);
	SpriteFrame* sf = SpriteFrameCache::getInstance()->getSpriteFrameByName(plistFileName);
	if (sf)
	{
		this->loadTexture(sf);
	}
	else
	{
		string fileName = StringUtils::format(FORMAT_IMAGENAME, storeId);
		string path = downloadDir + "/" + fileName;
		if (FileUtils::getInstance()->isFileExist(path))
			this->loadTexture(path);
		else
		{
			string _default = "BookCity/res/jiazai.png";
			SpriteFrame* spf = SpriteFrameCache::getInstance()->getSpriteFrameByName(_default);
			if (spf)
				this->loadTexture(spf);
			downloadImage();
		}
	}
}

void CityButton::downloadImage()
{
	if (url.empty() || storeId < 0)
		return;
	YYXDownloadImages::GetInstance()->newDownloadImage(url, downloadDir, StringUtils::format(FORMAT_IMAGENAME, storeId),
		normal, 0, [=](string path) {
		YYXLayer::sendNotify(StringUtils::format(TAGNOTIFY, storeId));
	});
}