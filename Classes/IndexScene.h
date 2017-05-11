#ifndef __Index_SCENE_H__
#define __Index_SCENE_H__

#include "cocos2d.h"
#include "ui/UIVideoPlayer.h"
#include "ui/CocosGUI.h"
#include "Bird.h"
#include "ContinuousMove.h"
#include "cocostudio/CocoStudio.h"
#include "LoadScene.h"
#include "BookRoomScene.h"
#include "ParentScene.h"
#include "LoginScene.h"
#include "BookStoreScene.h"

#include "BabyCenter.h"
#include "BookCityScene.h"
#include "BookInfoScene.h"
#include "RestRemind.h"
#include "FileNames.h"
#include "ShowNotification.h"


enum ControlMusic { CLOSE = 0, OPEN = 1 };

enum TrainItem { LOCOMOTIVE = 0 , BOOKCITY = 1 , PICTUREBOOK = 2 , 
	RECOMMEND = 3 , LIMITFREE = 4 , EXCELLENT_FIVE_STAR = 5 , 
	NEWBOOK = 6 , KANGXUANBOOKSTORE = 7 , VIPBOOKSTORE = 8};

class Index : public cocos2d::Layer
{
public:
	EventListenerTouchAllAtOnce* m_lister;//进入父母设置,双指右滑

public:
	Index();
	void replace(const function<void()>& func = nullptr);
	~Index();

    static cocos2d::Scene* createScene(SceneInfo* sceneInfo = nullptr);
	static Index* create(SceneInfo* data = nullptr);
    virtual bool init(SceneInfo* sceneInfo = nullptr);

	void InitVIPCard();//初始化包年

	void onEnterTransitionDidFinish();
    
    void menuCloseCallback(cocos2d::Ref* pSender);
    
	//创建每节火车车厢
	Layout* createItem(int tag,const std::string& csbfilename, int startIndex, int endIndex, bool loop, Size layoutsize);

	//创建花
	void creatFlows(Vec2 position, const std::string &imageFileName, float time, int nFrameNumber, const std::string& c_plist, const char* c_FileNameFormat, float delay);

	//火车回调
	void selectedItemEvent(Ref *pSender, ListViewEventType type);

	//添加等待messagebox
	//static Layer* WaitLayer();

	//3种颜色 随机变化,正确选择颜色
	static Layer * SelectLayer(std::function<void()>);	

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
	//控制第一次播放动画 之后不再播放动画
	void playAnimator(function<void()> animator, function<void()> animatorOver);
	//火车动画
	void huocheAnimation(ListView* listview, ImageView* parent, ImageView* portrait, ImageView* door_img, Sprite* door, Sprite* door2, Vec2 origin, const string & plist);
};

#endif // __Index_SCENE_H__
