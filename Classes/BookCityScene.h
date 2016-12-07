#ifndef __BOOKCITYSCENE_H__
#define __BOOKCITYSCENE_H__

#include "App.h"
#include "cocos2d.h"
#include "IndexScene.h"
#include "BookStoreScene.h"
#include "ThreadQueue.h"

USING_NS_CC;

class BookCityScene : public cocos2d::Layer
{
public:
	Data m_bookStoreCSBRes_double;//csb的加载 2颗小按钮
	Data m_bookStoreCSBRes_onlyone;//csb的加载 1颗大按钮
	ListView* m_listview;
	Vec2 m_touchBegan;
	//long long *m_eventTime;
	//static daocode::threadQueue* m_queue;//网络请求队列管理
	long long m_createTime;//场景创建时间
public:
	BookCityScene();
	~BookCityScene();
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BookCityScene);

	void initListView();
	void onEnterTransitionDidFinish();
	//YYXLayer * LoadingLayer();
	//初始化listview
	void httpBookCityInfo();
	//static void AsyncDownload(int, std::string, int);//按钮封面或边框的异步下载
	//static void threadDownd(int, std::string, int);
	//static void notifyLoadFailed(std::string);//通知下载封面或边框失败

	static bool m_isMoved;//判断是否移动
};

class BigBtn : public Node
{
public:
	BigBtn();
	~BigBtn();
	static BigBtn* create(Data, int);
	void initNode(Data);
private:
	int m_castleId;
};

class SmallBtn : public Node
{
public:
	SmallBtn();
	~SmallBtn();
	static SmallBtn* create(Data, int, int);
	void initNode(Data);
private:
	int m_castleId_top;
	int m_castleId_bottom;
};

#endif // __BOOKCITYSCENE_H__