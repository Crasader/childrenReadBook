#ifndef __BookRoom_SCENE_H__
#define __BookRoom_SCENE_H__

#include "cocos2d.h"
#include "IndexScene.h"
#include "FileNames.h"
#include "NetIntface.h"
#include "App.h"
#include "YYXDownload.h"
#include "YYXLayer.h"
#include "YYXStruct.h"

USING_NS_CC;
using namespace cocos2d::network;
using namespace std;

class BookRoom : public cocos2d::Layer
{
public:
	map< thread::id, YYXStruct> ida;
	BookRoom();
	~BookRoom();
	CREATE_FUNC(BookRoom);
    static cocos2d::Scene* createScene();
	vector<int> vip_boosSort;
    virtual bool init();
	void initHttp();
	void onEnterTransitionDidFinish();
	//void showDownloadRecord();
	//void showReadRecord();
	//最近下载界面, 计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
	//vector<int> getCurrentPageBookID4DownLoad();
	//最近阅读界面, 计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
	//vector<int> getCurrentPageBookID4Read();
	vector<int> getCurrentPageBookID4Collect();//收藏
	vector<int> getCurrentPageBookID4VIP();//包年图书
	vector<int> getCurrentPageBookID4BuyBookDownload();//购书中下载部分
	vector<int> getCurrentPageBookID4BuyBook();//购书列表
	//展示按钮
	void showImageView(int status, ImageView* view1, ImageView* view2, ImageView* view3, ImageView* view4);
	//长按的效果是 一个背景图片盖住 上面一个书籍节点移动不同的位置 覆盖下面的书籍
	void changLongClickBookPosition(ImageView* backgro, Node* booknode, int position);
	//判断这书是否是我的包年图书
	bool bookIsMyVipBooks(int bookid);
	//判断用户是否是vip
	bool userIsVip();
	bool bookIsMyBuyBooks(int bookid);
	bool bookIsSuo(int bookid);
	//刷新当前页书籍
	void refershPage(int status);
	//刷新book节点信息
	void refershBookNode(Node* book, int bookid);
	//点击事件
	void bookClick(Node* book, int bookid);
	//长按
	void bookLongClick(Node* book, int bookid);
	//下载状态的展示
	void showStauts(int stauts, Sprite* download, Sprite* textbg, Text* text);
	//滚动动画
	void RollingAnimation(Node* node1, Node* node2, function<void()> callback);
	void showBookPageNumber();
	void down(Node* book, int bookid);
	int getCurrentPageNumber() const { return m_currentPageNumber; }
	void setCurrentPageNumber(int val) { 
		_mutex.lock();
		m_currentPageNumber = val; 
		_mutex.unlock();
	}
	//void roolingAn(Node* node);
private:
	Layer* layer;
	Label *moveCircle;//第几/几页
	Node* pageCircle;
	mutex _mutex;
	int m_currentPageNumber;
	//6本书
	vector<Node*> books;
	//编辑状态
	bool m_compile = false;
	//离线模式
	bool m_offline = false;
	//允许触发点击
	bool m_click = true;
	//展示模式 //购书列表 = 3 已收藏 = 2 已下载  = 0 包年图书 = 4	
	int bookMode = 0;
	//bool shangsuo = false;//过期上锁 点击失效
	map<int , bool> canLongClinck;
	int longClickId = -999;
	Node* longClickNode ;
	int bookSum = 0;
	Point reorder ;
};

#endif // __BookRoom_SCENE_H__
