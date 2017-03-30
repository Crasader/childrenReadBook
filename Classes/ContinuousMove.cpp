#include "ContinuousMove.h"


ContinuousMove::ContinuousMove(float f_Scale, Vec2 vec2_AnchorPoint, Vec2 vec2_Offset, float f_MoveTime, Vec2 Show4AnchorPoint, Vec2 Shadow4AnchorPoint, Vec2 MoveUpStart, float MoveUpTime, float ShakeTime, float JumpHeight,int Jumps, bool completeAnimation)
{
	m_Scale = f_Scale;
	m_AnchorPoint = vec2_AnchorPoint;
	m_Offset = vec2_Offset;
	m_MoveTime = f_MoveTime;
	m_Show4AnchorPoint = Show4AnchorPoint;
	m_Shadow4AnchorPoint = Shadow4AnchorPoint;
	m_MoveUpStart = MoveUpStart;
	m_MoveUpTime = MoveUpTime;
	m_ShakeTime = ShakeTime;
	m_JumpHeight = JumpHeight;
	m_Jumps = Jumps;
	m_completeAnimation = completeAnimation;
}


ContinuousMove::~ContinuousMove()
{
}

ContinuousMove* ContinuousMove::create(const std::string& c_FileName
	, float f_Scale
	, Vec2 vec2_AnchorPoint
	, Vec2 vec2_Offset
	, float f_MoveTime
	, Vec2 Show4AnchorPoint
	, Vec2 Shadow4AnchorPoint
	, Vec2 MoveUpStart
	, float MoveUpTime
	, float ShakeTime
	, float JumpHeight
	, int Jumps
	, bool completeAnimation)
{	
	ContinuousMove *node = new (std::nothrow) ContinuousMove(f_Scale, vec2_AnchorPoint, vec2_Offset
		, f_MoveTime, Show4AnchorPoint, Shadow4AnchorPoint, MoveUpStart, MoveUpTime, ShakeTime
		,  JumpHeight,  Jumps, completeAnimation);
	auto cache = SpriteFrameCache::getInstance();
	if (auto spf = cache->getSpriteFrameByName(c_FileName))
	{
		node->m_Cloud = Sprite::create();
		node->m_Cloud->initWithSpriteFrame(spf);
		node->m_Cloud->getTexture()->setAliasTexParameters();	
		node->m_Cloud1 = Sprite::create();
		node->m_Cloud1->initWithSpriteFrame(spf);
		node->m_Cloud1->getTexture()->setAliasTexParameters();
	}
	if (node && node->init())
	{                                    
		node->autorelease();		
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}

bool ContinuousMove::init()
{
	//云
	if (m_Cloud==nullptr || m_Cloud1==nullptr)
	{
		return false;
	}
	m_Cloud->setScale(m_Scale);
	m_Cloud->setAnchorPoint(m_Show4AnchorPoint);
	addChild(m_Cloud);
	m_Cloud->setPosition(m_MoveUpStart);
	//云1
	m_Cloud1->setScale(m_Scale);
	m_Cloud1->setAnchorPoint(m_Shadow4AnchorPoint);
	addChild(m_Cloud1);
	m_Cloud1->setPosition(m_AnchorPoint);
	if (m_completeAnimation)
	{
		m_Cloud->runAction(Sequence::create(MoveTo::create(m_MoveUpTime, m_AnchorPoint), EaseOut::create(JumpBy::create(m_ShakeTime, Vec2(0, 0), m_JumpHeight, m_Jumps), 1), CallFuncN::create([&](Ref *pSender)
		{
			m_Cloud->runAction(RepeatForever::create(Sequence::create(MoveBy::create(m_MoveTime, m_Offset), CallFuncN::create([=](Ref *pSender)
			{
				//CCLOG("m_Cloud = %f / %f", ((Sprite*)pSender)->getPosition().x, ((Sprite*)pSender)->getPosition().y);
				((Sprite*)pSender)->setPosition(m_AnchorPoint);
			}), NULL)));
			m_Cloud1->runAction(RepeatForever::create(Sequence::create(MoveBy::create(m_MoveTime, m_Offset), CallFuncN::create([=](Ref *pSender)
			{
				//CCLOG("m_Cloud1 = %f / %f", ((Sprite*)pSender)->getPosition().x, ((Sprite*)pSender)->getPosition().y);
				((Sprite*)pSender)->setPosition(m_AnchorPoint);
			}), NULL)));
		}), NULL));
	}
	else
	{
		CloudBack();
		m_Cloud->runAction(RepeatForever::create(Sequence::create(MoveBy::create(m_MoveTime, m_Offset), CallFuncN::create([=](Ref *pSender)
		{
			//CCLOG("m_Cloud = %f / %f", ((Sprite*)pSender)->getPosition().x, ((Sprite*)pSender)->getPosition().y);
			((Sprite*)pSender)->setPosition(m_AnchorPoint);
		}), NULL)));
		m_Cloud1->runAction(RepeatForever::create(Sequence::create(MoveBy::create(m_MoveTime, m_Offset), CallFuncN::create([=](Ref *pSender)
		{
			//CCLOG("m_Cloud1 = %f / %f", ((Sprite*)pSender)->getPosition().x, ((Sprite*)pSender)->getPosition().y);
			((Sprite*)pSender)->setPosition(m_AnchorPoint);
		}), NULL)));
	}
	return true;
}

void ContinuousMove::CloudBack()
{
	m_Cloud->setPosition(m_AnchorPoint);
	m_Cloud1->setPosition(m_AnchorPoint);
}

