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
	Data m_bookStoreCSBRes_double;//csb�ļ��� 2��С��ť
	Data m_bookStoreCSBRes_onlyone;//csb�ļ��� 1�Ŵ�ť
	ListView* m_listview;
	Vec2 m_touchBegan;
	//long long *m_eventTime;
	//static daocode::threadQueue* m_queue;//����������й���
	long long m_createTime;//��������ʱ��
public:
	BookCityScene();
	~BookCityScene();
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BookCityScene);

	void initListView();
	void onEnterTransitionDidFinish();
	//YYXLayer * LoadingLayer();
	//��ʼ��listview
	void httpBookCityInfo();
	//static void AsyncDownload(int, std::string, int);//��ť�����߿���첽����
	//static void threadDownd(int, std::string, int);
	//static void notifyLoadFailed(std::string);//֪ͨ���ط����߿�ʧ��

	static bool m_isMoved;//�ж��Ƿ��ƶ�
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