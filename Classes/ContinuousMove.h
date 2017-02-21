#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace cocos2d::ui;


class ContinuousMove:public Node
{
protected:
	Sprite *m_Cloud;//展示的图片
	Sprite *m_Cloud1;//补全的图片
	float m_Scale;//缩放比例
	Vec2 m_AnchorPoint;//2张图片的锚点固定在这个位置上,滚动起点
	Vec2 m_Offset;//滚动偏移量
	Vec2 m_MoveUpStart;//上移的起点
	Vec2 m_Show4AnchorPoint;//展示图片的锚点
	Vec2 m_Shadow4AnchorPoint;//补全图片的锚点
	float m_MoveTime;//滚动时间
	float m_MoveUpTime;//上移时间
	float m_ShakeTime;//抖动时间
	float m_JumpHeight;//跳的高度
	int m_Jumps;//跳的次数
	bool m_completeAnimation;//true = 完整动画 false = 直接滚动

public:
	ContinuousMove(float f_Scale, Vec2 vec2_AnchorPoint, Vec2 vec2_Offset, float f_MoveTime, Vec2 Show4AnchorPoint
		, Vec2 Shadow4AnchorPoint, Vec2 MoveUpStart, float MoveUpTime, float Shake
		, float JumpHeight, int Jumps,bool completeAnimation=true);
	virtual ~ContinuousMove();
	static ContinuousMove* create(const std::string& c_FileName, float f_Scale, Vec2 vec2_AnchorPoint, Vec2 vec2_Offset
		, float f_MoveTime, Vec2 Show4AnchorPoint, Vec2 Shadow4AnchorPoint, Vec2 MoveUpStart, float MoveUpTime
		, float Shake, float JumpHeight, int Jumps, bool completeAnimation = true);
	virtual bool init();


	void CloudBack();
};

