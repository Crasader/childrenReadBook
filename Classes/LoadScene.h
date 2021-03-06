﻿#ifndef __Load_SCENE_H__
#define __Load_SCENE_H__

#include "cocos2d.h"
#include "2d\CCTransition.h"
#include "IndexScene.h"
#include "App.h"
#include "FileNames.h"

class Load : public cocos2d::Layer
{
public:
	~Load();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene(SceneInfo* data=nullptr);

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init(SceneInfo* data=nullptr);
	static Load* create(SceneInfo* data=nullptr);
	void initEvent();
	virtual void onEnterTransitionDidFinish();

	void initData();//初始化本地数据
	void initHttp();//初始化网络请求
	void initMemberHttp();//根据memberID进行网络初始化
	static void initDir();//初始化创建文件夹
	// implement the "static create()" method manually
	virtual void cleanup();
	//缓存csb文件
	void loadCsbFile();
	//异步加载plist大图
	void loadPlistPng();
	void loadPlistPngCallback(cocos2d::Texture2D* sender);
	//网络请求消息推送
	//void getEllaNotification();	
	//用户不使用网络 自动通过本地数据登录
	void getPhoneInfo();
private:
	int totalCount;
	int currentCount;
};

#endif // __Load_SCENE_H__
