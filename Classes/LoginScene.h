#ifndef __Login_SCENE_H__
#define __Login_SCENE_H__

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "FileNames.h"
#include "ParentScene.h"
#include "network/HttpClient.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "Toast.h"
#include "MyEditBox.h"

USING_NS_CC;
using namespace cocos2d::network;
using namespace cocostudio::timeline;

class Login : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

	void onEnterTransitionDidFinish();
	// implement the "static create()" method manually
    CREATE_FUNC(Login);

	//预加载
	//static void prestrain();

	Layer* registerInit();//注册 对话框初始化

	Layer* forgetPasswordInit();//忘记密码 对话框初始化

	Layer* loginInit();//登陆 对话框初始化

	Layer* addChildInit();//添加宝贝 对话框初始化
							 
	//void verification(std::string account, std::string password);//验证登陆过程

	void registerAccount(std::string memberName, std::string memberPasswd);//注册过程

	void CallBackRegisterAccount(string str, Node* node);//注册账号的回调

	void getCode(std::string account, string type);//获取验证码

	void addChildInfo(std::string childrenName, int childrenSex, std::string childrenBirth);//添加宝贝

	void changeChildInfo(int childrenId, std::string childrenName, int childrenSex, std::string childrenBirth);//修改宝贝信息

	void forgetPassword(std::string memberName, std::string password, Node* forgetPasswordScene);//修改密码

	//关闭定时器 以免控件丢失报错
	void closeTime();

	//手动登陆
	void LogIn(string member_name, string member_passwd);
	//确定需要展示的头像和孩子ID
	void getShowIDandShowHeadPortrait(int member_id);

	void getDefaultHeadPortrait(int member_id);

	void getChildList(int member_id, function<void()> downloadHeadPortrait);

	void httpGetUserRedPackets(int memberId);

	void httpRedPacketActivity();
};

#endif // __Login_SCENE_H__
