#ifndef __NEXT_SCENE_H__
#define __NEXT_SCENE_H__

#include "cocos2d.h"
#include "App.h"
#include "FileNames.h"

class Next : public cocos2d::Layer
{
public:
	Next();
	~Next();
    static cocos2d::Scene* createScene(SceneInfo* data=nullptr);

    virtual bool init(SceneInfo* data=nullptr);
	static Next* create(SceneInfo* data=nullptr);
};

#endif // __Next_SCENE_H__
