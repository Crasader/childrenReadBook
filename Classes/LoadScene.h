#ifndef __Load_SCENE_H__
#define __Load_SCENE_H__

#include "cocos2d.h"
#include "2d\CCTransition.h"
#include "IndexScene.h"
#include "App.h"
#include "FileNames.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

class Load : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
	void onEnter();
	void initEvent();
	virtual void onEnterTransitionDidFinish();

	void initData();//初始化本地数据
	void initHttp();//初始化网络请求
	void initMemberHttp();//根据memberID进行网络初始化
	void initDir();//初始化创建文件夹
	void httpRedPacketActivity();
	// implement the "static create()" method manually
    CREATE_FUNC(Load);

	//获取本地保存的设置
	//void getLocalSetup();
	//自动登录
	//void defaultLogin();
	//void defaultLogin(string userAccount, string userPassword);
	//验证登陆过程
	//static void verification(std::string account, std::string password);
	//验证登录回调
	//static void verficationCallback(std::string json);
	//获取用户详情
	//static void getUserDetails(std::string);
	//获取用户详情回调
	//static void getUserDetailsCallback(std::string, std::string);
	//获取孩子详情
	//static void getChildDetails();
	////获取孩子详情回调
	//static void getChildDetailsCallback(std::string);
	//下载头像
	//static void downloadChildAvatar(std::string);
	//缓存csb文件
	void loadCsbFile();
	//网络请求已购书籍列表
	//static void getBuyBook();
	//网络请求已购书籍列表回调
	//static void buyBookCallback(std::string);

	//通知登录成功
	//static void notifyLoginSuccess();
	//通知登录失败
	//static void notifyLoginFailed();

	//异步加载plist大图
	void loadPlistPng();
	void loadPlistPngCallback(cocos2d::Texture2D* sender);

	//网络请求书城的按钮信息
	//static void httpButtonsInfo();

	//网络请求书城信息和书店图标下载
	void httpBookCityInfoAndDownLoad();

	//网络请求书城按钮信息的回调
	//static void httpButtonsInfoResponseCallback(std::string str);

	//网络请求消息推送
	void getEllaNotification();

	//网络请求版本号
	void getEllaVersion();

	//获取第一个孩子的头像和ID作为选中的孩子
	//void getDefaultHeadPortrait(int member_id);

	//获取孩子列表
	void getChildList(int member_id, function<void()> downloadHeadPortrait);
	//用户已购书籍
	void getUserBuyBook(int member_id);
	//获取用户红包
	void httpGetUserRedPackets(int memberId);
	//用户不使用网络 自动通过本地数据登录
	void defaultAutoLogin();

	
	//确定需要展示的头像和孩子ID
	//void getShowIDandShowHeadPortrait();
	//声音
	void initUserData();
	//将本地的数据全部加载内存
	void loadingLocalFileData();
	void getPhoneInfo();
private:
	int totalCount;
	int currentCount;
	bool m_cacheOver = false; //加载资源是否完成
	bool m_httpOver = false; // 网络请求是否完成 或者超时5秒
};

#endif // __Load_SCENE_H__
