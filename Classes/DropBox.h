#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "App.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;
using namespace std;

#define DROPBOX_WIDTH_SITE 400 //地点listview的宽度
#define DROPBOX_WIDTH_DATE 266 //日期listview的宽度
#define DROPBOX_HEIGHT 48 //listview单个元素的高度
#define DROPBOX_PARAMCOUNT 9 //listview能显示的元素个数
#define DROPBOX_STARTYEAR 2000 //日期开始的年份
#define DROPBOX_WIDTHYEAR 30 //年份宽度

class DropBox:public Node
{
public:
	DropBox();
	~DropBox();
	static DropBox* create(Vec2, Size, string, TextHAlignment);//创建日期选择
	static DropBox* create(Vec2, Size, string, string, TextHAlignment);//创建地点选择
	void initDateBox(Vec2, Size, string, TextHAlignment);//初始化日期选择
	void initSiteBox(Vec2, Size, string, TextHAlignment);//初始化地点选择

	void setDateText(string);
	void setSiteText(string, string);
	void setSiteText(string);
	string getDateText();//获取时间选择框文本
	vector<string> getSiteText();//获取地点选择框文本

	void menuCallBack1(Ref* sender);
	void menuCallBack2(Ref* sender);
	void selectedItemEvent1(Ref *pSender, ListViewEventType type);
	void selectedItemEvent2(Ref *pSender, ListViewEventType type);

	void selectedItemEventDate1(Ref *pSender, ListViewEventType type);
	void selectedItemEventDate2(Ref *pSender, ListViewEventType type);
	void selectedItemEventDate3(Ref *pSender, ListViewEventType type);

	void changeItemSite();//修改地点列表信息
	void changeItemDate1();//修改日期列表信息
	void changeItemDate2();

	void setDropEnable(bool);
private:
	string plitFileName;

	Label *myLabel;//显示文本
	vector<string> myParam1;//下拉列表参数
	vector<string> myParam2;
	vector<string> myParam3;
	ListView* myListView1;//列表
	ListView* myListView2;
	ListView* myListView3;
	string selectedStr1;//选中的元素
	string selectedStr2;
	string selectedStr3;
	int lastIndex1;//上次选中元素的下标
	int lastIndex2;
	int lastIndex3;
	bool tag;//true表示日期列表，false表示地点列表
	bool isEnable;//是否可触发

	string strProvince;//省份
	string strCity;//城市

	ImageView* imgView;//用来屏蔽触摸
	LayerColor* bgLayer;//选择列表背景
};