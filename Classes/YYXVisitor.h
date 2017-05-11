//-------------------------------------------------游客模式-----------------------------------------------------------------
/*
根据本地设备号获取一个账号, 永久跟随设备,
1.在load场景中如果没有默认账号就获取一个本地游客账号, 游客账号需要存入本地visitor文件夹中
2.在首页, 游客显示请登录头像
3.在书房, 游客正常显示书籍
4.在宝贝中心和父母设置, 游客显示请登录
5.在书城和书店里, 提示选择游客购买还是登陆后购买
6.游客有账号ID，有账号余额，有购书列表，其他都没有
*/
#ifndef _YYXVISITOR_H__
#define _YYXVISITOR_H__

#include "App.h"

class YYXVisitor
{
public:
	YYXVisitor();
	~YYXVisitor();
	static YYXVisitor* instance;
	static YYXVisitor* getInstance();
	int VisitorMode;
	string m_dirpath;//本地存储的文件夹路径
	void setVisitorMode(bool sets);//设置游客模式 true=游客 false=非游客
	bool getVisitorMode();
	void loadLocationVisitorData();//本地加载游客信息
	void initDir();

	void hintLogin(function<void()> youke, function<void()> toLogin = nullptr, function<void()> user = nullptr, string visitorStr="", bool Notime=true);//提示选择游客购买还是登陆后购买
	void indexSceneInit(ImageView* portrait);//首页初始化游客
	void bookRoomSceneInit();//书房初始化游客
	void bookCitySceneInit();//书城初始化游客
	void bookStoreSceneInit();//书店初始化游客
	void baoNianBookStoreSceneInit(function<void()> visitorRun, function<void()> user);//包年书店处理游客
	void bookInfoSceneInit();//书籍详情初始化游客
	void babyCenterSceneInit(function<void()> youke, function<void()> user);//宝贝中心初始化游客
	void parentSceneInit(function<void()> youke, function<void()> user);//父母设置初始化游客
	void parentSceneLogout();//退出游客模式
	void loadSceneInit();//加载页初始化游客
	void loginVisitor();//登入游客账号
	void logoutVisitor();//登出游客账号
	void meIsNull();//判断本地账号是否是空的, 如果是空的就登入游客
	string getVisitorName();//获取游客的账号

	int getMemberId();
	void setMemberId(int val);
	int getMoney();
	void setMoney(int val);
private:
	int memberId = -999;//游客的账号ID
	int money = 0;//账户余额（分）
};
#endif