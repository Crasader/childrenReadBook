#ifndef __LoginControl_SCENE_H__
#define __LoginControl_SCENE_H__

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "FileNames.h"
#include "ParentScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "Toast.h"
#include "MyEditBox.h"

USING_NS_CC;

class LoginControl
{
private:
	static LoginControl* instance;

public:
	LoginControl();
	~LoginControl();
	static LoginControl* getInstance();

	void Login(CallBack_String call1, CallBack_String call2);//游客切换到用户 调用原始界面去交互登陆
	void LoginCallback(string json);
	void Logout();//用户切换到游客
};

#endif
