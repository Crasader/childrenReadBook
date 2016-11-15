#include "FrameAnimation.h"
#include "App.h"


FrameAnimation::FrameAnimation()
{
}


FrameAnimation::~FrameAnimation()
{
}

Action* FrameAnimation::create(int nFrameNumber, const std::string& c_plist, const char* c_FileNameFormat, float delay )
{
	auto cache = SpriteFrameCache::getInstance();	
	Vector<SpriteFrame*> vector;
	char str[100] = { 0 };
	for (int i = 0; i < nFrameNumber; ++i)
	{
		sprintf(str, c_FileNameFormat, i);
		auto frame = cache->spriteFrameByName(str);
		if (frame==nullptr)
		{
			cache->addSpriteFramesWithFile(c_plist);
			--i;
		}
		vector.pushBack(frame);
	}
	auto animation = Animation::createWithSpriteFrames(vector, delay);
	auto animate = Animate::create(animation);
	CCASSERT(animate != nullptr, "动画is NULL");
	auto seq = Sequence::create(animate, animate->reverse(), NULL);
	return RepeatForever::create(seq);
}

Animate* FrameAnimation::createAnimate(int nFrameNumber, const std::string& c_plist, const char* c_FileNameFormat, float delay)
{
	auto cache = SpriteFrameCache::getInstance();
	Vector<SpriteFrame*> vector;
	char str[100] = { 0 };
	for (int i = 0; i < nFrameNumber; ++i)
	{
		sprintf(str, c_FileNameFormat, i);
		auto frame = cache->spriteFrameByName(str);
		if (frame == nullptr)
		{
			App::log("               ---------------------- createAnimate                     add  " + string(str), i);
			cache->addSpriteFramesWithFile(c_plist);
			--i;
		}
		else
			vector.pushBack(frame);
	}
	App::log(c_plist.c_str(), vector.size());
	auto animation = Animation::createWithSpriteFrames(vector, delay);
	auto animate = Animate::create(animation);
	CCASSERT(animate != nullptr, "动画is NULL");
	return animate;
}