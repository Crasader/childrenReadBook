#include "NextScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

Next::Next()
{
}

Next::~Next()
{
	ControlScene::getInstance()->end();
}

Scene* Next::createScene(SceneInfo* data)
{
    auto scene = Scene::create();
    auto layer = Next::create(data);
	layer->setTag(9);
    scene->addChild(layer);
    return scene;
}

Next* Next::create(SceneInfo* data)
{
	Next *pRet = new(std::nothrow) Next();
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

bool Next::init(SceneInfo* sceneInfo)
{
	if (!Layer::init())
	{
		return false;
	}
	auto but = Button::create("other/Common_Popup_Close_ipad@2x.png");
	but->setPosition(Director::getInstance()->getWinSize() / 2);
	addChild(but);
	but->addClickEventListener([](Ref* sen) {
		auto con = ControlScene::getInstance();
		con->backFromScene();
	});
    return true;
}