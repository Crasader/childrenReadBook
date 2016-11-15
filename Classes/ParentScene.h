#ifndef __Parent_SCENE_H__
#define __Parent_SCENE_H__

#include "cocos2d.h"
#include "IndexScene.h"
#include "FileNames.h"
#include "App.h"
//#include "SqliteManager.h"
#include "DropBox.h"
//#include "DownloadRes.h"
#include "RechargeLayer.h"

class Parent : public cocos2d::Layer
{
private:
	Node* m_show;//记录当前显示的图层
	Node* m_parentNode;//整个场景
	int m_times;//获取验证码计时
	map<string,void*> m_voids;//存指针
public:
	~Parent();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

	void onEnterTransitionDidFinish();
	// implement the "static create()" method manually
    CREATE_FUNC(Parent);	

	//设置界面的初始化
	Layer* initNode_Set();

	//联系我们界面的初始化
	Layer* initNode_Contact();

	//账号信息界面初始化
	Layer* initNode_Account();

	//登陆界面初始化
	Layer* initNode_Login();

	//预加载
	//static void prestrain();

	//获取本地缓存大小
	float getLocalCacheSize();

	//获取文件夹大小
	long getDirectorySize(string);

	//清除缓存
	void deleteCache();

	//删除本地非空目录
	static void DeleteDirectory(string );

	//意见反馈
	void FeedBack(string,string);

	//网络请求修改用户信息
	//void updateInfo(int, string, string);
	//void updateInfoCallback(string, int, string, string);

	//通知修改用户信息成功
	//void notifyModifySuccess();

	//通知修改用户信息失败
	//void notifyModifyFailed();

	//获取验证码
	void getCode(string);

	//网络请求修改密码
	void modifyPassword(string, string);

	//网络请求账户余额
	void getUserMoney();

	//展示红包
	void showRedPacket();

	//获取有效红包的回调
	//void httpGetRedpacketCallback(string json);

	void listViewShowRedPacket(ListView * listview);

	void loadRedPacketData(Node * node, int index);
	//获取有效红包
	void httpGetUserRedPacket();

	//void stringHttpRequest(HttpRequest::Type type, string url, map<string, string> paramter, int outTime, std::function<void(string json)> runable, std::function<void()> errorRunable);
};

#endif // __Parent_SCENE_H__
