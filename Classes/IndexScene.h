#ifndef __Index_SCENE_H__
#define __Index_SCENE_H__

#include "cocos2d.h"
#include "ui\UIVideoPlayer.h"
#include "ui\CocosGUI.h"
#include "Bird.h"
#include "ContinuousMove.h"
#include "cocostudio\CocoStudio.h"
#include "LoadScene.h"
#include "BookRoomScene.h"
#include "ParentScene.h"
#include "LoginScene.h"
#include "BookStoreScene.h"

#include "BabyCenter.h"
#include "BookCityScene.h"
#include "BookInfoScene.h"
#include "RestRemind.h"
#include "WaitingScene.h"
#include "FileNames.h"
#include "SimpleAudioEngine.h"
#include "ShowNotification.h"

using namespace CocosDenshion;

enum ControlMusic { CLOSE = 0, OPEN = 1 };

enum TrainItem { LOCOMOTIVE = 0 , BOOKCITY = 1 , PICTUREBOOK = 2 , 
	RECOMMEND = 3 , LIMITFREE = 4 , EXCELLENT_FIVE_STAR = 5 , 
	NEWBOOK = 6 , KANGXUANBOOKSTORE = 7 , VIPBOOKSTORE = 8};

class Index : public cocos2d::Layer
{
public:
	EventListenerTouchAllAtOnce* m_lister;//进入父母设置,双指右滑

public:
	~Index();

    static cocos2d::Scene* createScene();

    virtual bool init();

	void InitVIPCard();//初始化包年

	void onEnterTransitionDidFinish();
    
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    CREATE_FUNC(Index);

	//创建每节火车车厢
	Layout* createItem(int tag,const std::string& csbfilename, int startIndex, int endIndex, bool loop, Size layoutsize);

	//创建花
	void creatFlows(Vec2 position, const std::string &imageFileName, float time, int nFrameNumber, const std::string& c_plist, const char* c_FileNameFormat, float delay);

	//火车回调
	void selectedItemEvent(Ref *pSender, ListViewEventType type);

	//添加等待messagebox
	static Layer* WaitLayer();

	//3种颜色 随机变化,正确选择颜色
	static Layer * SelectLayer(std::function<void()>);

	//---------------------------转场函数-----------------------------------------------------
	//void replaceSceneAnimation(std::function<void()> runfunction);

	//跳转书房
	static void GoToBookRoomScene();

	//跳转父母设置
	static void GoToParentScene();

	//跳转首页
	static void GoToIndexScene();

	//跳转登录
	static void GoToLoginScene();

	//跳转宝贝中心
	static void GoToBabyCenterScene();

	//跳转书城
	static void GoToBookCity();

	//跳转书城里的书店
	static void GoToBookCityChildStore(int bookStoreId);

	//跳转书籍详情
	static void GoToBookInfo(int bookId);

	//跳转绘本
	static void GoToPictureBook();

	//跳转咿啦推荐
	static void GoToRecommend();

	//跳转限时免费
	static void GoToFree();

	//跳转五星好评
	static void GoToGoodReputation();

	//跳转咿啦新书
	static void GoToNewBook();

	//跳转vip书店
	static void GoToVIPBook();

	//跳转康轩书店
	static void GoToKangXuanStore();

	//返回前一场景
	static void BackPreviousScene();

	//过渡遮罩
	void maskAnimation(Layout* layout, Point point);

	//展示消息推送
	void showNotification();

	//判断是否需要版本更新
	void dealEllaVersion();

	//展示一个恭喜获得红包列表界面
	void showRedPacket();

	void listViewShowRedPacket(ListView* listview);

	//加载红包数据到节点上
	void loadRedPacketData(Node* node, int index);
};

#endif // __Index_SCENE_H__
