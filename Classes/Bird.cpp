#include "Bird.h"
#include "YYXLayer.h"

Bird::Bird(const std::string& filename, const std::string& c_plist, const char* c_FileNameFormat, Vec2 P0, Vec2 P1, Vec2 P2, Vec2 P3, float DelayTime, float FlyTime)
{
	auto cache = SpriteFrameCache::getInstance();
	auto frame = cache->spriteFrameByName(filename);
	if (frame == nullptr)
	{
		CCLOG("空=%s", filename.c_str());
		cache->addSpriteFramesWithFile(c_plist);
	}
	this->initWithSpriteFrame(frame);
	m_P0 = P0;
	m_P1 = P1;
	m_P2 = P2;
	m_P3 = P3;
	m_DelayTime = DelayTime;
	m_FlyTime = FlyTime;
	m_FlyAnimation = FrameAnimation::create(4, c_plist, c_FileNameFormat, 0.06f);
}


Bird::~Bird()
{
}

Bird* Bird::create(const std::string& filename, const std::string& c_plist, const char* c_FileNameFormat, Vec2 P0, Vec2 P1, Vec2 P2, Vec2 P3, float DelayTime, float FlyTime)
{
	Bird *sprite = new (std::nothrow) Bird(filename, c_plist, c_FileNameFormat, P0, P1, P2, P3, DelayTime, FlyTime);
	if (sprite && sprite->init())
	{
		sprite->autorelease();		
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

bool Bird::init()
{		
	//扇翅膀动画
	runAction(m_FlyAnimation);
	//贝塞尔曲线运动	
	ccBezierConfig bezier;
	bezier.controlPoint_1 = m_P1;
	bezier.controlPoint_2 = m_P2;
	bezier.endPosition = m_P3;
	GoToBack();
	auto sequence = Sequence::create(DelayTime::create(m_DelayTime), CallFuncN::create([=](Ref* pSender) {
		//音效
		if (App::GetInstance()->isSoundEffect)
			AudioEngine::play2d(ELLA_SOUND_BIRDS, false, 0.5);
	}), BezierTo::create(m_FlyTime, bezier), CallFuncN::create(CC_CALLBACK_0(Bird::GoToBack, this)), NULL);
	runAction(RepeatForever::create(sequence));
	//schedule([this](float f){
	//	CCLOG("%f / %f",this->getPosition().x,this->getPosition().y);
	//}, "Test");
	return true;
}
//返回原点
void Bird::GoToBack()
{
	this->setPosition(m_P0);
}