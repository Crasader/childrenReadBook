#ifndef __BABYCENTER_SCENE_H__
#define __BABYCENTER_SCENE_H__

#include "cocos2d.h"
#include "DropBox.h"
#include "FileNames.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "CocosAndroidJni.h"
#include "App.h"
#include "YYXStruct.h"
#include "YYXLayer.h"
#include "ControlScene.h"
#include "User.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;
using namespace cocos2d::ui;

class BabyCenter : public cocos2d::Layer
{
private:
	static int m_readBooks;
	static long m_readTimes;
	string tempName = "";
	
public:
	ImageView* m_BigPhoto;//大头像控件
	ImageView* m_img_photo;//宝贝中心 小头像

public:
	BabyCenter();

	~BabyCenter();

	static Scene* createScene(SceneInfo* sceneInfo = nullptr);
	static BabyCenter* create(SceneInfo* data = nullptr);
	virtual bool init(SceneInfo* sceneInfo = nullptr);
	//vector<Node*> m_dels;//需要删除的csb文件控件
	virtual void onEnterTransitionDidFinish();
	void cleanup();
	//初始化宝贝中心
	Node* babyinfo = nullptr;
	void initBabyCenter();

	//初始化登录界面
	void initLoginLayer();

	//切换孩子界面
	void initChangeChild();

	//添加宝贝 对话框初始化
	Layer* initAddChild();

	void setNodeChild(Node* node, int index , ChildInfo* child);

	//网络请求阅读记录
	void getReadRecord(int);

	//展示头像
	bool showPortrait(ImageView* imgview, string path);
};

#endif
