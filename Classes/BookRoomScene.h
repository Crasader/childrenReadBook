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

    virtual bool init();
	void initHttp();
	void onEnterTransitionDidFinish();
	//void showDownloadRecord();
	//void showReadRecord();
	//最近下载界面, 计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
	vector<int> getCurrentPageBookID4DownLoad();
	//最近阅读界面, 计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
	vector<int> getCurrentPageBookID4Read();
	vector<int> getCurrentPageBookID4Collect();
	vector<int> getCurrentPageBookID4Delect();
	vector<int> getCurrentPageBookID4Rent();
	//展示按钮
	void showImageView(int status, ImageView* view1, ImageView* view2, ImageView* view3, ImageView* view4);
	//内存中加载数据
	void loadTheDataInTheMemory();
	//刷新当前页书籍
	void refershPage(int status);
	//刷新book节点信息
	void refershBookNode(Node* book, int bookid);
	//长按
	void bookLongClick(Node* book, int bookid);
	void LongClick(Ref* sender, TouchEventType type);
	//处理点击
	void bookClick(Node* book, int bookid);
	//下载状态的展示
	void showStauts(int stauts, Sprite* download, Sprite* textbg, Text* text);
	//计算得到当前页的所有书籍bookid
	vector<int> getCurrentPageBookID();
	//滚动动画
	void RollingAnimation(Node* node1, Node* node2, function<void()> callback);
	void showBookPageNumber();
	//void roolingAn(Node* node);
private:
	Label *moveCircle;//第几/几页
	Node* pageCircle;
	int m_currentPageNumber;
	//6本书
	vector<Node*> books;
	//编辑状态
	bool m_compile = false;
	//离线模式
	bool m_offline = false;
	//允许触发点击
	bool m_click = true;
	//展示模式 全部图书=0 购书列表=1  最近下载=2 最近阅读=3 vip包年图书=4
	int bookMode = 0;
};

#endif // __BookRoom_SCENE_H__
