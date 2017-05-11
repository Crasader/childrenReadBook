#ifndef __BookRoom_SCENE_H__
#define __BookRoom_SCENE_H__

#include "cocos2d.h"
#include "IndexScene.h"
#include "FileNames.h"
#include "CrossPlatform.h"
#include "App.h"
#include "YYXDownload.h"
#include "YYXLayer.h"
#include "YYXStruct.h"
#include "BookRoomSelectPage.h"

USING_NS_CC;
using namespace cocos2d::network;
using namespace std;

class BookRoom : public cocos2d::Layer
{
public:
	map< thread::id, YYXStruct> ida;
	BookRoom();
	~BookRoom();
	
    static cocos2d::Scene* createScene(SceneInfo* sceneInfo=nullptr);
	static BookRoom* create(SceneInfo* data =nullptr);
	vector<int> vip_boosSort;
    virtual bool init(SceneInfo* sceneInfo = nullptr);
	void initEvent();
	void initHttp();
	void onEnterTransitionDidFinish();
	void cleanup();

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
	void downloadBook(string url, string dir, string filename, int bookid);
	int getCurrentPageNumber();
	void setCurrentPageNumber(int val) { 
		m_currentPageNumber = val; 
	}
private:
	Layer* layer = nullptr;
	Label *moveCircle = nullptr;//第几/几页
	Node* pageCircle = nullptr;
	mutex _mutex;
	int m_currentPageNumber = 0;
	//6本书
	vector<Node*> books;
	//编辑状态
	bool m_compile = false;
	//离线模式
	bool m_offline = false;
	//允许触发点击
	bool m_click = true;
	
	map<int , bool> canLongClinck;
	int longClickId = -999;
	Node* longClickNode = nullptr;
	int bookSum = 0;
	Point reorder =Point(-1000,0);

private:
	int bookMode = 0;
	BookRoomSelectPage *bookRoomSelectPage;
	vector<int> getPageData(vector<int> vecdata);
	EventListenerTouchOneByOne *touchmove, *touchlistener;
};

#endif // __BookRoom_SCENE_H__
