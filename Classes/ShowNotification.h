#ifndef __ShowNotification_LAYER_H__
#define __ShowNotification_LAYER_H__

#include "cocos2d.h"
#include "FileNames.h"
#include "App.h"
#include "MyEditBox.h"
#include "IndexScene.h"
#include "UpdateVersion.h"
class ShowNotification : public cocos2d::Layer
{
public:
	ShowNotification();
	~ShowNotification();
	bool init();
	static ShowNotification* create(bool);
	void initActivity();//图片信息页面页面
	void initWord();//文字信息页面
	
	void initVersion();//版本更新页面

	void downloadPic();//下载图片
private:
	bool m_tag;//判断是消息推送层或版本更新
};

#endif // __ShowNotification_LAYER_H__
