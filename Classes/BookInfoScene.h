﻿#ifndef __BOOKINFO_SCENE_H__
#define __BOOKINFO_SCENE_H__

#include "cocos2d.h"
#include "IndexScene.h"
#include "FileNames.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
//#include "SqliteManager.h"
#include "ThreadQueue.h"
#include "network/CCDownloader.h"
#include "App.h"
#include "Toast.h"
#include "YYXStruct.h"
#include "BookInfoControl.h"
#include "BookCache.h"

using namespace std;

#define BookInfoCountdown "BookInfo_time1"//定时器名称

class BookInfo : public Layer
{
private:
	int m_bookStatus = 0;//书籍和用户关系		
						 //1.判断包年用户
						 //	A.包年用户	 1 << 4
						 //	B.非包年	 0 << 4
						 //2.判断是否购买过书籍
						 //	A.购买	 1 << 3
						 //	B.未购买	 0 << 3
						 //3.判断是否满365本
						 //	A.满	 1 << 2
						 //	B.未	满	 0 << 2
						 //4.判断是否已经租过
						 //	A.租过	 1 << 1
						 //	B.未租	 0 << 1
						 //5.判断书籍是否属于包年类型书籍
						 //	A.属于	 1
						 //	B.不属于  0
						 //6.判断书籍是否免费
						 //	A.免费	 1<<5
						 //	B.不免费  0<<5
		
	Layer* node;//背景节点图层 csb导入
	int m_bookId;
	int m_orderId;//订单号
	std::string m_BookName = "";//书籍名称
	int m_Selling100;//售价*100
	int m_refre;//刷新次数
	map<string, map<std::string, YYXStruct>> m_comment;//评论的数据源
	vector<string> _comment;//评论顺序
	ListView* m_listview = nullptr;//评论

	Text* m_Text_RemainDay;//活动倒计时控件
	ListView* m_listview_Info;//简介控件
	Layer* m_View_Info; //简介图层 控件
	Layer* m_View_Comment;//评论 控件
	//Button* m_tryReadButton;//试读按钮
	//Button* m_ReadButton;//阅读按钮
	Text* m_price_z;//售价整数部分
	Text* m_price_x;//售价小数部分
	Text* m_originalPrice;//原价
	Text* m_name;//名字
	ImageView* m_control;

	//long long m_createTime;//场景创建时间
	 //时刻集合
	//map<string, long long> timeMap;

	map<string, YYXStruct> m_tag;//标记数据
	bool isComment = false;

	bool listview_click;//评论listview 是否允许点击
public:
	BookInfo();
	~BookInfo();

    static cocos2d::Scene* createScene(SceneInfo* sceneInfo = nullptr);

	static BookInfo * create(SceneInfo* sceneInfo = nullptr);

	virtual bool init(SceneInfo* sceneInfo = nullptr);

	void showPrice();	//展示价格

	void showTime(long long li);//展示倒计时

	Layer* initIntroView(); //初始化简介界面

	Layout * creatItem_Introduction();
	Layout * creatItem_hengxian();
	Layout * creatItem_Press();
	Layout * creatItem_bookPage();
	Layout * creatItem_bookAuthor();

	Layer* initCommonView();//初始化评论界面
							
	void initEvent();

	void initHttp();

	void onEnterTransitionDidFinish();

	virtual void cleanup();

	void refreshUI();//刷新界面
	
	void startSchedu();//启动活动倒计时

	void recharge();

	void back();//返回

	int getOptimalRedPacket();//选出最优红包
	void hintHongbao();
	string DownLoadBook(int bookId, string bookPlayUrl, string ZipName);
	
	void log(int stauts);
	void ButtonControl();//按钮的控制逻辑
private:
	Node* m_show;//记录当前显示的图层
	
	bool isDownloading;//完整资源是否正在下载
	bool isDownloaded;//完整资源是否已下载
	bool isUnzip;//完整资源是否已解压
	//bool isBuy;//完整资源是否已购买
	bool isDamage;//完整资源是否损坏

	bool isViewDownloading;//试读资源是否正在下载
	bool isViewDownloaded;//试读资源是否已下载
	bool isViewUnzip;//试读资源是否已解压
	bool isViewDamage;//试读资源是否损坏

private:
	int m_xingji=4;//书籍评分
	BookInfoControl* m_relation;//控制和解析人跟书的关系
	void showBuyButtons(int val);//展示按钮组合

	void visitorNoBuyNoRent();
	void userNoBuyNoRent();
	void visitorRent();
	void borrowBook();
	void userBuy();
	void onClickLister();
	void readBook();
	void sendDownloadBookAndReadBook();
	void buyBook();
	void callBackBuyBook();
	void hintIsBuyIsRent();
	void gotoLogin();
	void addDownloadListener(Button* button);
	void showXingji();
	void showCommentCount(int count);
	void readbook_download_pause_tryreadbook(Button* bt);
	void downloadCover();

private:
	Book* m_bookData = nullptr;//书籍信息
};

#endif //  __BOOKINFO_SCENE_H__
