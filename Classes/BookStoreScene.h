#ifndef __BookStore_SCENE_H__
#define __BookStore_SCENE_H__
///*逻辑简介
//本类是书店场景,展示书籍简介(封面,价格)
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//UI --> 加载DB --> 装载数据进内存 --> 刷新界面
//|
//﹂--> 网络请求BookInfo --> 更新DB
//				|
//				﹂--> 装载数据进内存 --> 通知刷新界面
//																			*
//*******************************************
//*
//UI --> 刷新界面 --> 通知请求封面
//												*
//***************************
//*
//UI --> 网络请求封面 --> 下载 --> 更新DB
//							|
//							﹂--> 装载数据进内存 --> 通知刷新界面
//																						*		
//**************************************************
//*
//UI --> 刷新界面
//
//std::map<int, map<std::string, ParaType>> m_viewPageData是viewpage的数据源,int是sort(排序位置) map容器是书籍信息
//举例:map容器是书籍信息有如下数据内容
//std::map<std::string, ParaType> value;
//App::initParaType(value, "sort", sort, "");
//App::initParaType(value, "bookId", bookId, "");
//App::initParaType(value, "coverURL", -999, coverURL);
//App::initParaType(value, "price", price, "");
//App::initParaType(value, "upTime", upTime, "");
//App::initParaType(value, "path", -999, path);
//App::initParaType(value, "downloadtime", downloadtime, "");
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//2016.04.09 俞越星*/
#include "cocos2d.h"
#include "FileNames.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "IndexScene.h"
#include "network/CCDownloader.h"
#include "App.h"
#include <fstream>
#include <iostream>
#include "XZLayer.h"
#include "YYXLayer.h"
USING_NS_CC;
//using namespace daocode;
using namespace std;

#define TIMER_RIGHTROLLING "rightrolling"//右滚一页的定时器名字
#define TIMER_LEFTROLLING "leftrolling"//左滚一页的定时器名字

class BookStore : public cocos2d::Layer
{
private:
	static int isNetConnect;//是否联网 0=联网  1=断网

	bool m_click;//是否允许封面点击

	Data csb;//评分复用csb

	bool m_move = false;//移动状态,正在移动不可刷新

	int m_bookStoreId;//书店ID

	Text* m_bookstorename;//店名

	std::string m_visitFrom;//网络参数 0 = 首页进入  1 = 书城进入

	Label *moveCircle;//第几/几页
	Node* pageCircle = nullptr;

	int* m_selectPageIndex;//记录当前展示的3个viewpage子页的内容页码

	//MySceneName m_backScene;//记录当前场景

	EventListenerTouchOneByOne* m_lister;//控制轮子滚动和翻页的监听

	long long m_soundTime = 0;//声音播放时刻

	long long m_createTime;//场景创建时间
public:
	BookStore();
	~BookStore();

    static cocos2d::Scene* createScene(SceneInfo* sceneInfo = nullptr);

	static BookStore* create(SceneInfo* sceneInfo = nullptr);

    virtual bool init(SceneInfo* sceneInfo = nullptr);

	void initEvent();

	void onEnterTransitionDidFinish();
	
	//年卡书店初始化
	void baonianStore();

	bool haveData();//判断是否有数据展示;

	bool changPageNumber();//翻页时,计算内容页码
								
	void showBookPageNumber(); //展示第1/5页

	//vector<int> getSortsOfThisPage(int index);//根据页码 返回该页码page上的所有书籍的sort 书籍小于等于8本

	 //返回上级
	void back();

	//离开场景时处理的业务
	void cleanup();

	//获取当前页书籍列表信息
	//void getCurrentlyPageBookListInfo(int bookStoreID, int pageIndex);

	//刷新一本书
	void refreshBook(Node* node, int bookId =-999, string path = "", int price = -999, int isNewEllaBook = -999, int hashBuy = -999);

	//刷新3页
	void refershThreePages(Node* node);

	void refershPage(Node* node, int index);

	//木马移动的动画
	void viewMoveAnimation(Node* node, bool isLeft);

	//void protectAnimation();
	//void addorStartQueue(function<void()> runable, string str = "");
	////添加并执行队列函数
	//void addQueue(function<void()> runable);
	//void startQueue();
	//void stopQueue();
	//void runingQueue();

private:
	Sprite* sp2;
	Node* background;
	Node* moveViews;
	EventListenerTouchOneByOne *touchmove , *touchlistener;
	vector<string> listerName;
	void gotoBookInfo(int bookid);
	//void safe_downCoverOk();
	void recoverAnimation();
};

#endif // __BookStore_SCENE_H__