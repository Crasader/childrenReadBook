#pragma once
#include "cocos2d.h"
//#include "ui/CocosGUI.h"
USING_NS_CC;
//using namespace cocos2d::ui;


class FrameAnimation
{
public:
	FrameAnimation();
	~FrameAnimation();
	static Action* create(int nFrameNumber, const std::string& c_plist, const char* c_FileNameFormat, float delay);
	static Animate* createAnimate(int nFrameNumber, const std::string& c_plist, const char* c_FileNameFormat, float delay);
};

