﻿#include "AppDelegate.h"
#include "LoadScene.h"
#include "FKAudioPlayer.h"
#include "YYXBookOver.h"
#include "FKBookParser.h"
#include "YYXSound.h"
#include "ControlScene.h"
#include "YYXVisitor.h"
#include "AppHttp.h"

USING_NS_FK;
USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1094, 614);
//static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
//static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
//static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate() {
}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("ellabook_W2736H1536", Rect(0, 0, 1094, 614));
#else
        glview = GLViewImpl::create("ellabook_W2736H1536");
#endif
        director->setOpenGLView(glview);
	}
	else
	{
		Size frameSize = glview->getFrameSize();
		float h = frameSize.height;
		float w = frameSize.width;
		log("setFrameSize w =%f ", w);
		log("setFrameSize h = %f ", h);
		if (w > h)
			glview->setFrameSize(w, h);
		else
			glview->setFrameSize(h, w);
		log("getFrameSize w =%f  ", glview->getFrameSize().width);
		log("getFrameSize h = %f ", glview->getFrameSize().height);
	}
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//	director->setProjection(Director::Projection::_2D);
//#endif
	CCDirector::sharedDirector()->setDepthTest(false);
	CCDirector::sharedDirector()->setProjection(kCCDirectorProjection2D);
    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
	FileUtils::getInstance()->addSearchPath("res");
    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);
    //Size frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    //if (frameSize.height > mediumResolutionSize.height)
    //{        
    //    director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    //}
    //// if the frame's height is larger than the height of small size.
    //else if (frameSize.height > smallResolutionSize.height)
    //{        
    //    director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    //}
    //// if the frame's height is smaller than the height of medium size.
    //else
    //{        
    //    director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    //}	
	director->setContentScaleFactor(1);
    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = Load::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
	FK::AudioPlayer::getInstance()->pauseAllEffect();
	YYXSound::getInstance()->stopBackGroundMusic();
	YYXSound::getInstance()->stopAll();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	if (YYXBookOver::getInstance()->getBookCoverRuning()){
		FK::AudioPlayer::getInstance()->stopAllEffect();
	}
    Director::getInstance()->startAnimation();	

    // if you use SimpleAudioEngine, it must resume here
	YYXSound::getInstance()->init();
	if (FK::BookParser::getInstance()->getIsReading()) {
		FK::AudioPlayer::getInstance()->resumeAllEffect();
	}
	else
	{
		YYXSound::getInstance()->playBackGroundMusic();
	}
	//执行原生代码的回调
	ActivityOnCallback(CocosAndroidJni::getInstance()->getRuningKey());
	//处理下线通知
	thread([]() {
		App::ccsleep(1000);
		if (!YYXVisitor::getInstance()->getVisitorMode() && User::getInstance()->getMemberId() > 0) 
			AppHttp::getInstance()->httpUserIsOffLine();
	}).detach();	
}

void AppDelegate::ActivityOnCallback(int param1)
{
	string json = CocosAndroidJni::getInstance()->getfunc(param1);
	CocosAndroidJni::getInstance()->clearRuningKey();
	App::log("-------------------------------AppDelegate::ActivityOnCallback " + json, param1);
	ControlScene* control = ControlScene::getInstance();
	switch (param1)
	{
	case 1://跳转场景
		control->replaceScene(control->getCurrentScene(), control->getSceneInfo((MySceneName)jiexicanshu(json)));
		break;
	case 2://跳转场景并且处理展示红包列表
		control->replaceScene(control->getCurrentScene(), control->getSceneInfo((MySceneName)jiexicanshu(json))->setData("show",Value(1)));
		break;
	case 3://跳转当前场景
		if(control->getDangqianScene() != MySceneName::BOOK)
			control->replaceScene(control->getCurrentScene(), control->getSceneInfo(control->getDangqianScene()));
		break;
	default:
		break;
	}
}

int AppDelegate::jiexicanshu(string json)
{
	if (!json.empty())
	{
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			string scene = YYXLayer::getStringForJson("", doc, "scene");
			return Value(scene).asInt();
		}
	}
	return -999;
}
