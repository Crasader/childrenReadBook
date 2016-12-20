#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "FrameAnimation.h"
#include "FileNames.h"
USING_NS_CC;
using namespace cocos2d::ui;


class Bird:public Sprite
{
protected:
	Vec2 m_P0;	//原点
	Vec2 m_P1;	//控制点1
	Vec2 m_P2;	//控制点2
	Vec2 m_P3;	//终点
	float m_DelayTime;	//间隔时间
	float m_FlyTime;	//飞行时间
	Action* m_FlyAnimation;//飞行帧动画

public:
	Bird(const std::string& filename, const std::string& c_plist, const char* c_FileNameFormat, Vec2 P0, Vec2 P1, Vec2 P2, Vec2 P3, float DelayTime, float FlyTime);
	static Bird* create(const std::string& filename, const std::string& c_plist, const char* c_FileNameFormat, Vec2 P0, Vec2 P1, Vec2 P2, Vec2 P3, float DelayTime, float FlyTime);
	virtual ~Bird();
	virtual bool init();


	void GoToBack();
};

