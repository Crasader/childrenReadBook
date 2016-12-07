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
//#include "SqliteManager.h"
#include "ThreadQueue.h"
#include "network/CCDownloader.h"
#include "App.h"
#include <fstream>
#include <iostream>
#include "YYXLayer.h"
USING_NS_CC;
using namespace daocode;
using namespace std;

#define TIMER_RIGHTROLLING "rightrolling"//右滚一页的定时器名字
#define TIMER_LEFTROLLING "leftrolling"//左滚一页的定时器名字

class BookStore : public cocos2d::Layer
{
private:
	//static std::mutex m_mutex_cover;
	//static std::map<int, map<std::string, ParaType>>* m_viewPageData;//数据源
	//bool m_touch;//true =触摸状态
	queue<function<void()>> runQueue;//执行主线程队列

	bool queueIsRuning;//队列是否正在运行

	static int isNetConnect;//是否联网 0=联网  1=断网

	bool m_click;//是否允许封面点击

	Data csb;//评分复用csb

	bool m_move = false;//移动状态,正在移动不可刷新

	int m_bookStoreId;//书店ID

	Text* m_bookstorename;//店名

	std::string m_visitFrom;//网络参数 0 = 首页进入  1 = 书城进入

	//size_t m_pageCount;//总页数
	//size_t m_storeBookCount;//书籍总数	
	//Vector<Node*> m_wheel;//所有轮子
	//std::vector<float> m_Rotation;//所有轮子角度
	//float m_ScrollThreshold;//pageview 翻页阀值
	//PageView* m_pageView;
	//Node* m_book1;//m_book1 前一页 csb文件创建viewpage子页面
	//Node* m_book2;//m_book2 当前展示页 csb文件创建viewpage子页面
	//Node* m_book3;//m_book3 后一页 csb文件创建viewpage子页面
	//std::vector<Node*> m_vbook1;//m_book1 的8个book
	//std::vector<Node*> m_vbook2;//m_book2 的8个book
	//std::vector<Node*> m_vbook3;//m_book3 的8个book

	Label *moveCircle;//第几/几页
	Node* pageCircle;

	//bool m_event;//标记通知是否发送
	//bool m_sendBookInfo;//是否完成上一次的书籍详情的请求,完成=true
	//threadQueue* m_queue;//网络请求队列管理
	//int m_getHttp;//请求次数
	//int m_refre;//刷新界面次数
	//int m_needRefre;//需要刷新的次数
	//Node* m_waitLayer; //等待图层 阻止触摸
	//long long m_httpBookListTime;//网络请求书籍列表的时刻
	//long long *m_eventTime;

	int* m_selectPageIndex;//记录当前展示的3个viewpage子页的内容页码

	MySceneName m_backScene;//记录当前场景

	EventListenerTouchOneByOne* m_lister;//控制轮子滚动和翻页的监听

	long long m_soundTime = 0;//声音播放时刻

	long long m_createTime;//场景创建时间
public:
	~BookStore();

    static cocos2d::Scene* createScene(int BookStoreId);

    virtual bool init(int BookStoreId);

	void onEnterTransitionDidFinish();

	static BookStore* create(int BookStoreId);
	
	//void httpDownLoadImage(int sort, int* count);//网络请求下载图片
	//void httpDownLoadImageCallback(char * buf, int length, int bookId, int sort, int* count);//网络请求下载图片的回调函数
	//void getAllWheel(ui::PageView* pageview);//获取全部轮子
	//PageView* createPageView();//创建一个pageview
	//void closeTime();//关闭定时器
	//void httpBooksInfo(int BookStoreId);	//网络请求本店的所有书籍信息
	//void httpBooksIdResponseCallback(std::string str , int BookStoreId, int pageIndex);//网络请求本店的所有书籍信息的回调函数
	//void addHttpQueue_httpDownLoadImage(int bookId, int* count);//httpDownLoadImage请求加入队列
	//void IsHttpCoverOver(int* count);//判断封面请求是否全部结束
	//void count4lock(std::mutex& mute, int* count);//计数器减一 加锁 线程同步
	//void loadTheDataIntoMemory();//装载数据进内存
	//void refreshUI();//刷新界面
	//void getView4ViewPage();//新的viewpage添加3个子页
	//ui::Layout * pageInit(Node * view, int index, std::vector<Node*> booksNode, bool addEvent);//初始化viewpage子页/刷新子页
	//void init4ViewPage();//初始化viewpage
	//void addThread_db(string tableName, unordered_map<string, string> paraInfoSTR, unordered_map<string, long long> paraInfoINT);//添加线程到队列
	//void thread4insert(string tableName, unordered_map<string, string> paraInfoSTR, unordered_map<string, long long> paraInfoINT);//数据操作失败,开线程插入数据库

	bool changPageNumber( );//翻页时,计算内容页码

	//static void notify(NotifyType type, int index=-999);//通知 
								
	void showBookPageNumber(); //展示第1/5页

	vector<int> getSortsOfThisPage(int index);//根据页码 返回该页码page上的所有书籍的sort 书籍小于等于8本

	 //返回上级
	void back();

	//离开场景时处理的业务
	void leave();

	//获取当前页书籍列表信息
	void getCurrentlyPageBookListInfo(int bookStoreID, int pageIndex);

	//创建一个loading图层
	//YYXLayer* LoadingLayer();

	//刷新一本书
	void refreshBook(Node* node, int bookId =-999, string path = "", int price = -999, int isNewEllaBook = -999, int hashBuy = -999);

	//刷新3页
	void refershThreePages(Node* node);

	void refershPage(Node* node, int index);

	//木马移动的动画
	void viewMoveAnimation(Node* node, bool isLeft);

	void protectAnimation();
	void addorStartQueue(function<void()> runable, string str = "");
	//添加并执行队列函数
	void addQueue(function<void()> runable);
	void startQueue();
	void stopQueue();
	void runingQueue();
	//展示vip用户提示
	YYXLayer* showUserIsVip();
};

#endif // __BookStore_SCENE_H__