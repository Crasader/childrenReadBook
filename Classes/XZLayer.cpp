#include <iostream>   
#include <string>   
#include "XZLayer.h"
#include "App.h"


//展示分享红包
void XZLayer::showShareRedPacket(string redCount)
{
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
		map<string, int64String> parameter;
		YYXLayer::insertMap4ParaType(parameter, "className", -999, "showShareRedPacket");
		YYXLayer::insertMap4ParaType(parameter, "csb", -999, HONGBAOMESSAGEBOX);
		auto layer = create(parameter);
		if (!layer)
			return;
		auto content = (ImageView*)layer->findControl(FIND_SPRITE_HONGBAOMESSAGEBOX_CONTEXT);
		auto gotohongbaolist = (ImageView*)layer->findControl(FIND_IMAGEBUTTON_HONGBAOMESSAGEBOX_CONTEXT);
		auto background = (ImageView*)layer->findControl(FIND_IMAGE_HONGBAOMESSAGEBOX_CONTEXT);
		auto text = (Text*)layer->findControl(FIND_TEXT_HONGBAOMESSAGEBOX_CONTEXT);
		content->setTouchEnabled(true);
		gotohongbaolist->setTouchEnabled(true);
		background->setTouchEnabled(true);
		if (text)
			text->setText(redCount);
		if (background)
		{
			background->addClickEventListener([=](Ref* sender) {
				layer->removeFromParentAndCleanup(true);
			});
		}
		layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
		layer->getParentNode()->setPosition(Director::getInstance()->getVisibleSize() / 2);
		Director::getInstance()->getRunningScene()->addChild(layer, 10);
	});
}