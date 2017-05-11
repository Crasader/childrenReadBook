#include "BookCityScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "extensions/cocos-ext.h"
#include "CityTableView.h"
#include "AppHttp.h"
#include "YYXVisitor.h"
using namespace cocostudio::timeline;

bool BookCityScene::m_isMoved = false;

BookCityScene::BookCityScene() {
}

BookCityScene::~BookCityScene(){
	ControlScene::getInstance()->end();
	BookStoreCache::getInstance()->allClear();
}

Scene* BookCityScene::createScene(SceneInfo* data)
{
    auto scene = Scene::create();
    auto layer = BookCityScene::create(data);
    scene->addChild(layer);
    return scene;
}

BookCityScene* BookCityScene::create(SceneInfo* data /*= nullptr*/)
{
	BookCityScene *pRet = new(std::nothrow) BookCityScene();
	if (pRet && pRet->init(data))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool BookCityScene::init(SceneInfo* sceneInfo)
{
	if (!Layer::init())
	{
		return false;
	}
	//游客
	YYXVisitor::getInstance()->bookCitySceneInit();
	App::log("setVisitorMode = ");
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "bookCityClickButton", 0);
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	addChild(LayerColor::create(Color4B::WHITE));
	//--------------------------------------------------------------------背景控件-------------------------------------------------------------------------------------------------------------------------
	addChild(Index::WaitLayer());

	Data data;
	if (App::GetInstance()->getData(BABYCENTER_BOOKCITY_CSB,data))
	{
		node = (Layer*)CSLoader::createNode(data);
	}
	else
		node = (Layer*)CSLoader::createNode(BABYCENTER_BOOKCITY_CSB);
	if (node == nullptr)
		return false; 
	node->setAnchorPoint(Vec2(0.5f, 0.5f));
	node->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
	//左边柱子
	auto leftColumn = (Sprite*)node->getChildByName(BABYCENTER_FIND_LEFTCOLUMN);
	leftColumn->setPositionX((1094 - visibleSize.width) / 2);
	leftColumn->setTag(100);
	//右边柱子
	auto rightColumn = (Sprite*)node->getChildByName(BABYCENTER_FIND_RIGHTCOLUMN);
	rightColumn->getTexture()->setAliasTexParameters();
	rightColumn->setPositionX((1094 + visibleSize.width) / 2);
	rightColumn->setTag(101);
	//背景墙 控件名称
	auto backWall = (Sprite*)node->getChildByName(BABYCENTER_FIND_BACKWALL);
	//地板
	auto floor = (Sprite*)node->getChildByName(BABYCENTER_FIND_FLOOR);
	floor->setTag(103);
	addChild(node);
	//返回按钮点击事件
	auto homeButton = ( Button*)node->getChildByName(BABYCENTER_FIND_BACK);
	homeButton->removeFromParentAndCleanup(true);
	homeButton->setAnchorPoint(Vec2(0, 1));
	homeButton->setPosition(origin + Vec2(14, visibleSize.height - 14));
	addChild(homeButton, 10);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "bookCityClickBookStoreTime", [=]() {
			auto control = ControlScene::getInstance();
			control->backFromScene();
		});
	});
	auto bookroom = (Button*)node->getChildByName("Button_1");
	if (bookroom)
	{
		bookroom->removeFromParent();
		bookroom->setAnchorPoint(Vec2(1, 1));
		bookroom->setPosition(origin + visibleSize + Vec2(-14, -14));
		addChild(bookroom, 10);
		bookroom->setScale(0.4);
		bookroom->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "bookstoreSceneBookRoomButtonTime", [=]() {
				auto control = ControlScene::getInstance();
				control->replaceScene(ControlScene::getInstance()->getSceneInfo(BookCity), ControlScene::getInstance()->getSceneInfo(BookRoomScene));
			});
		});
	}
	CityTableView* view = CityTableView::create();
	view->setPosition(Vec2(20, 0));
	view->loadData();
	addChild(view);	
	auto listener30 = EventListenerCustom::create(TAG_CITYSCENEERFERVIEW, [=](EventCustom* e) {
		view->loadData();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener30, view);

	auto lister = EventListenerTouchOneByOne::create();
	lister->onTouchBegan = [&](Touch* touch, Event *event2) {
		m_isMoved = false;
		m_touchBegan = touch->getLocation();
		return true;
	};
	lister->onTouchMoved = [&](Touch* touch, Event *event2) {
		auto touchMoved = touch->getLocation();
		if (touchMoved.x - m_touchBegan.x < -30 || touchMoved.x - m_touchBegan.x > 30 ||
			touchMoved.y - m_touchBegan.y < -30 || touchMoved.y - m_touchBegan.y > 30) {
			m_isMoved = true;
		}
	};
	auto sp = Sprite::create();
	addChild(sp);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(lister, sp);

	//注册转换场景动作通知
	auto listenTurnAction = EventListenerCustom::create("Listen_Turn_Action", [=](EventCustom* e) {
		if (m_isMoved)
			return;
		int storeId = (long)e->getUserData();
		CallFunc *callFunc = CallFunc::create([=]() {
			auto control = ControlScene::getInstance();
			control->replaceScene(ControlScene::getInstance()->getSceneInfo(BookCity),
				ControlScene::getInstance()->getSceneInfo(BookCityCHILD)->setData("bookStoreId", Value(storeId))->setData("index", Value(0)));
		});
		YYXLayer::sendNotify("CityButtonAction_" + Value(storeId).asString());
		view->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(0, 1430 * 0.4)), callFunc, NULL));
		backWall->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(0, 1430 * 0.4)), NULL));
		floor->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(0, -320 * 0.4)), NULL));
		leftColumn->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(-200 * 0.4, 0)), NULL));
		rightColumn->runAction(Sequence::create(DelayTime::create(0.6f), MoveBy::create(0.5f, Point(200 * 0.4, 0)), NULL));
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenTurnAction, view);
	return true;
}

void BookCityScene::onEnterTransitionDidFinish()
{
	App::log("BookCityScene::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	AppHttp::getInstance()->httpBookCityInfo([]() {
		YYXLayer::sendNotify(TAG_DELETEWAITLAYER);
	});
	App::log("BookCityScene::onEnterTransitionDidFinish---END");
}

void BookCityScene::cleanup()
{
	node->removeAllChildrenWithCleanup(true);
	node->removeFromParentAndCleanup(true);
}
