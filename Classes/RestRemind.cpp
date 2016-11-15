#include "RestRemind.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

using namespace cocostudio::timeline;

RestRemind::RestRemind(){
	m_times = 300;
}

RestRemind::~RestRemind() {
	unschedule("Countdown");
	_eventDispatcher->removeEventListener(myTouchListener);
}

// on "init" you need to initialize your instance
bool RestRemind::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }    

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//主界面层
	auto restRemind = CSLoader::createNode(BABY_RESTREMIND_CSB);
	restRemind->setAnchorPoint(Vec2(0.5f, 0.5f));
	restRemind->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
	addChild(restRemind, 1);

	//半透明背景
	auto bgLayer = LayerColor::create(ccColor4B::BLACK, 1094, 614);
	bgLayer->setOpacity(100);
	addChild(bgLayer, 0);

	//倒计时
	auto m_timeDown = (Text*)restRemind->getChildByName(BABY_RESTREMIND_TIMEDOWN);
	schedule([=](float f) {
		m_times--;
		m_timeDown->setString(StringUtils::format("%d:%02d", m_times / 60, m_times % 60));
		if (m_times == 0) {
			App::protectedTiming();
			unschedule("Countdown");
			this->removeFromParentAndCleanup(true);
		}
	}, 1.0f, "Countdown");

	//解锁
	auto unlock = (Sprite*)restRemind->getChildByName(BABY_RESTREMIND_UNLOCK);

	//父母确认节点
	auto parent_sure = (Node*)restRemind->getChildByName(BABY_RESTREMIND_NODE);
	//提示文本
	auto text_tip = (Text*)parent_sure->getChildByTag(61);
	//颜色-红
	auto color_red = (Sprite*)parent_sure->getChildByTag(63);
	//颜色-黄
	auto color_yellow = (Sprite*)parent_sure->getChildByTag(64);
	//颜色-蓝
	auto color_blue = (Sprite*)parent_sure->getChildByTag(62);
	//关闭父母确认按钮
	auto btn_close = (Sprite*)parent_sure->getChildByTag(50);

	//随机目标颜色
	int goal_color = rand() % 3 + 62;
	if (goal_color == 62)
		text_tip->setString(App::getString("STR_BLUE"));
	else if(goal_color == 63)
		text_tip->setString(App::getString("STR_RED"));
	else if(goal_color == 64)
		text_tip->setString(App::getString("STR_YELLOW"));
	//随机三种颜色的位置

	//触摸事件
	myTouchListener = EventListenerTouchOneByOne::create();
	myTouchListener->onTouchBegan = [=](Touch* touch, Event* event) {
		auto touchPoint = touch->getLocation() + Vec2((1094 - visibleSize.width)/2, 0);

		if (parent_sure->getPositionX() == 0) {//父母确认节点显示
			if (color_red->boundingBox().containsPoint(touchPoint)) {
				color_red->setVisible(false);
				color_red->setOpacity(254);
				return true;
			}

			if (color_yellow->boundingBox().containsPoint(touchPoint)) {
				color_yellow->setVisible(false);
				color_yellow->setOpacity(254);
				return true;
			}

			if (color_blue->boundingBox().containsPoint(touchPoint)) {
				color_blue->setVisible(false);
				color_blue->setOpacity(254);
				return true;
			}

			if (btn_close->boundingBox().containsPoint(touchPoint)) {
				btn_close->setVisible(false);
				btn_close->setOpacity(254);
				return true;
			}
		}
		else{
			if (unlock->boundingBox().containsPoint(touchPoint)) {
				unlock->setVisible(false);
				unlock->setOpacity(254);
			}
		}
		return true;
	};
	myTouchListener->onTouchMoved = [=](Touch* touch, Event* event) {
		auto touchPoint = touch->getLocation() + Vec2((1094 - visibleSize.width) / 2, 0);

		if (parent_sure->getPositionX() == 0) {//父母确认节点显示
			if (color_red->getOpacity() == 254) {
				if (color_red->boundingBox().containsPoint(touchPoint))
					color_red->setVisible(false);
				else
					color_red->setVisible(true);
			}
				
			if (color_yellow->getOpacity() == 254) {
				if (color_yellow->boundingBox().containsPoint(touchPoint))
					color_yellow->setVisible(false);
				else
					color_yellow->setVisible(true);
			}

			if (color_blue->getOpacity() == 254) {
				if (color_blue->boundingBox().containsPoint(touchPoint))
					color_blue->setVisible(false);
				else
					color_blue->setVisible(true);
			}

			if (btn_close->getOpacity() == 254) {
				if (btn_close->boundingBox().containsPoint(touchPoint))
					btn_close->setVisible(false);
				else
					btn_close->setVisible(true);
			}
		}
		else{
			if (unlock->getOpacity() == 254 && unlock->boundingBox().containsPoint(touchPoint))
				unlock->setVisible(false);
			else
				unlock->setVisible(true);
		}
		
	};
	myTouchListener->onTouchEnded = [=](Touch* touch, Event* event) {
		auto touchPoint = touch->getLocation() + Vec2((1094 - visibleSize.width) / 2, 0);

		if (parent_sure->getPositionX() == 0) {
			color_red->setVisible(true);
			color_yellow->setVisible(true);
			color_blue->setVisible(true);
			btn_close->setVisible(true);

			if (color_red->boundingBox().containsPoint(touchPoint) && color_red->getOpacity() == 254) {
				if (goal_color == color_red->getTag()) {
					App::protectedTiming();
					this->removeFromParentAndCleanup(true);
					return;
				}
				else{
					Toast::create(App::getString("STR_YOURAREERROR"));
					parent_sure->setPosition(Vec2(2736, 0));
				}
			}

			if (color_yellow->boundingBox().containsPoint(touchPoint) && color_yellow->getOpacity() == 254) {
				if (goal_color == color_yellow->getTag()) {
					App::protectedTiming();
					this->removeFromParentAndCleanup(true);
					return;
				}
				else {
					Toast::create( App::getString("STR_YOURAREERROR"));
					parent_sure->setPosition(Vec2(2736, 0));
				}
			}

			if (color_blue->boundingBox().containsPoint(touchPoint) && color_blue->getOpacity() == 254) {
				if (goal_color == color_blue->getTag()) {
					App::protectedTiming();
					this->removeFromParentAndCleanup(true);
					return;
				}
				else {
					Toast::create( App::getString("STR_YOURAREERROR"));
					parent_sure->setPosition(Vec2(2736, 0));
				}
			}

			if (btn_close->boundingBox().containsPoint(touchPoint) && btn_close->getOpacity() == 254) {
				parent_sure->setPosition(Vec2(2736, 0));
			}

			color_red->setOpacity(255);
			color_yellow->setOpacity(255);
			color_blue->setOpacity(255);
			btn_close->setOpacity(255);
		}
		else{
			unlock->setVisible(true);
			if (unlock->boundingBox().containsPoint(touchPoint) && unlock->getOpacity() == 254) {
				parent_sure->setPosition(Vec2(0, 0));
			}
			unlock->setOpacity(255);
		}
		
	};
	_eventDispatcher->addEventListenerWithFixedPriority(myTouchListener, -200);
	myTouchListener->setSwallowTouches(true);
    return true;
}