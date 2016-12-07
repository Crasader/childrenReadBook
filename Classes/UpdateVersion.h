#ifndef __UpdateVersion_LAYER_H__
#define __UpdateVersion_LAYER_H__

#include "cocos2d.h"
#include "FileNames.h"
#include "App.h"
#include "MyEditBox.h"
#include "IndexScene.h"
class UpdateVersion : public cocos2d::Layer
{
public:
	UpdateVersion();
	~UpdateVersion();
	bool init();
	void onEnterTransitionDidFinish();
	static UpdateVersion* create();
private:
	//std::string m_curVersion;//当前版本号
	//std::string m_Version;//最新版本的版本号
};

#endif // __UpdateVersion_LAYER_H__
