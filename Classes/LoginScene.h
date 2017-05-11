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
	~Login();
    static cocos2d::Scene* createScene(SceneInfo *sceneInfo = nullptr);
	static Login* create(SceneInfo* data = nullptr);
    virtual bool init(SceneInfo *sceneInfo = nullptr);

	void onEnterTransitionDidFinish();
	void cleanup();

	Layer* registerInit();//注册 对话框初始化

	Layer* forgetPasswordInit();//忘记密码 对话框初始化

	Layer* loginInit();//登陆 对话框初始化
	void getCode(std::string account, string type);//获取验证码
	void forgetPassword(std::string memberName, std::string password, Node* forgetPasswordScene);//修改密码

	//关闭定时器 以免控件丢失报错
	void closeTime();

	//手动登陆
	//void LogIn(string member_name, string member_passwd);

	//void getDefaultHeadPortrait(int member_id);

	//void httpGetUserRedPackets(int memberId);
};

#endif // __Login_SCENE_H__
