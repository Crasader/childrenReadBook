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

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;
using namespace cocos2d::ui;

class BabyCenter : public cocos2d::Layer
{
private:
	static int m_readBooks;
	static long m_readTimes;

	//Node* m_parentNode;
	//Node* m_photoNode;
	int m_childrenId;
	//string selectPath="";//选中的原图
	//string selectCutRoundPath = "";//选中的圆形图片 用于展示界面
	
public:
	ImageView* m_BigPhoto;//大头像控件
	ImageView* m_img_photo;//宝贝中心 小头像
	static std::map<int, map<std::string, YYXStruct>> m_data;//数据源
	std::vector<int> m_childID;//孩子的ID列表
	/*
	int ChildId
	map 
	{
		int ChildId,
		string Path,
		int ChildSex,
		string ChildName,
		string ChildBirth,
		string ChildAvatarUrl
	}
	*/

public:
	BabyCenter();

	~BabyCenter();

	static cocos2d::Scene* createScene();

	virtual bool init();

	void initEvent();

	void initHttp();

	virtual void onEnterTransitionDidFinish();

	CREATE_FUNC(BabyCenter);

	//初始化宝贝中心
	void initBabyCenter();

	//初始化登录界面
	void initLoginLayer();

	//初始化头像界面
	void initPhotoLayer();

	//切换孩子界面
	void initChangeChild();

	//添加宝贝 对话框初始化
	Layer* initAddChild();

	void setNodeChild(Node* node, int index);

	//网络请求阅读记录
	void getReadRecord(int);
	
	//网络请求修改宝贝信息
	//void modifyBabyInfo(int, string, int, string);
	//void modifyBabyInfoCallback(string, int, string, int, string);

	//通知修改宝贝信息成功
	//void notifyModifySuccess();
	//通知修改宝贝信息失败
	//void notifyModifyFailed();

	//获取宝贝信息的回调函数
	//static void getChildDetailsCallback(std::string json);

	//上传头像
	void uploadAvatar();

	//下载孩子头像的回调
	void CallBackDownLoadChildPortrait(string str);

	//展示头像
	bool showPortrait(ImageView * imgview);

	//获取孩子列表的业务回调
	void getChildDetailsBusinessLogic(string json, function<void()> errorRunable);

	static string getStringFrommyData(int id, string key);
	static int getIntFrommyData(int id, string key);
	static void setBabyInfoToXml(string name, string birthday, int sex, int id=-999);
	//void makeRoundImage(string path);
	bool getBoolFromXML(string str);
};

#endif // __BABYCENTE_SCENE_H__
