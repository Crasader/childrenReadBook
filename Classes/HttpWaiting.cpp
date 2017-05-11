#include "HttpWaiting.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "App.h"
#include "YYXTime.h"
USING_NS_CC;
using namespace std;

HttpWaiting* HttpWaiting::instance = nullptr;

HttpWaiting::HttpWaiting()
{
}

HttpWaiting::~HttpWaiting()
{
}

HttpWaiting* HttpWaiting::getInstance()
{
	if (instance == nullptr)
	{
		instance = new HttpWaiting();
	}
	return instance;
}

cocos2d::Layer* HttpWaiting::newWaitingLayer(int key,const CallBackFunction& func)
{
	if (m_layer)
	{
		remove();
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Data data;
	string csb = WAITMESSAGEBOX_CSB;
	Layer* pLayer = nullptr;
	if (App::GetInstance()->getData(csb, data))
		pLayer = (Layer*)CSLoader::createNode(data);
	else
		pLayer = (Layer*)CSLoader::createNode(csb);
	pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

	m_callback = func;

	ActionTimeline* nodeAnimation = nullptr;
	nodeAnimation = CSLoader::createTimeline(csb);
	if (nodeAnimation)
	{
		nodeAnimation->gotoFrameAndPlay(0, 28, true);
		pLayer->runAction(nodeAnimation);
	}
	auto imageview = (ImageView*)pLayer->getChildByName(FIND_IMAGEVIEW_BY_NAME_WAITMESSAGEBOX);
	imageview->setTouchEnabled(true);
	imageview->setSwallowTouches(true);
	imageview->addClickEventListener([=](Ref* sender) {
		int _key = pLayer->getTag();
		HttpWaiting::getInstance()->remove();
	});
	auto listener11 = EventListenerCustom::create(TAG_REMOVEWAITING, [](EventCustom* e) {
		long key = (long)e->getUserData();
		auto _func = HttpWaiting::getInstance()->getCallback();
		HttpWaiting::getInstance()->remove();
		if (_func)
			_func();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener11, pLayer);
	m_layer = pLayer;
	return pLayer;
}

void HttpWaiting::remove()
{
	if (m_layer)
	{
		App::log("------------------------------------------------------------------>>> HttpWaiting::remove() = ",m_layer->getReferenceCount());
		if (m_layer->getParent())
		{
			App::log("------------------------------------------------------------------>>> HttpWaiting::remove() = getParent", m_layer->getReferenceCount());
			m_layer->removeFromParentAndCleanup(true);
		}
		else
		{
			App::log("------------------------------------------------------------------>>> HttpWaiting::remove() = m_layer->release();", m_layer->getReferenceCount());
			m_layer->release();
		}
	}
	m_layer = nullptr;
}