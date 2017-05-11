#ifndef __BOOKCITYSCENE_H__
#define __BOOKCITYSCENE_H__

#include "App.h"
#include "cocos2d.h"
#include "IndexScene.h"
#include "BookStoreScene.h"

USING_NS_CC;

class BookCityScene : public cocos2d::Layer
{
public:
	Layer* node = nullptr;
	Vec2 m_touchBegan;
public:
	BookCityScene();
	~BookCityScene();
    static cocos2d::Scene* createScene(SceneInfo* sceneInfo = nullptr);
	static BookCityScene* create(SceneInfo* data = nullptr);
    virtual bool init(SceneInfo* sceneInfo = nullptr);
	void onEnterTransitionDidFinish();
	void cleanup();
	static bool m_isMoved;//判断是否移动
};
#endif // __BOOKCITYSCENE_H__