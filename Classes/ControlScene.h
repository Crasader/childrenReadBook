/*写一个类 作用是控制当前只有一个场景， 并且在切换场景时保持正确，避免监听和场景错位导致闪退，
如何实现控制
1.转换场景不可以并发，拒绝执行第二任务
2.转换场景顺序  先第二场景 >> 构造函数  >> init() >> onEnterTransitionDidFinish() >> clernp() >> 第一个场景析构
*/
#pragma once
#ifndef __CONTROLSCENE_H__
#define __CONTROLSCENE_H__

#include "cocos2d.h"
USING_NS_CC;
using namespace std;

#define _huiben 22//绘本参数
#define _tuijian 21//咿啦推荐参数
#define _mianfei 50//限时免费参数
#define _haoping 51//五星好评参数
#define _xinshu 20//咿啦新书参数
#define _kangxuan 23//康轩书店参数
#define _baonian 100//年卡书店id

enum MySceneName {
	LoadScene //加载场景
	, BookRoomScene//书房
	, ParentScene//父母设置
	, IndexScene//首页
	, LoginScene//登陆
	, BabyCenterScene//宝贝中心
	, BookCity//书城
	, BookInfoScene//书籍详细
	, BookCityCHILD//书城中的书店	
	, PictureBook//绘本
	, Recommend//咿啦推荐
	, Free//限时免费
	, GoodReputation//五星好评
	, NewBook//咿啦新书
	, KangXuanStore//康轩书店
	, VIPBOOK//vip专属书店
	, BOOK//阅读器
	,NULLSCENE//空场景
};

class SceneInfo : public Ref
{
public:
	SceneInfo();
	~SceneInfo();

	static SceneInfo* create();

	static void del(SceneInfo* sc);

	MySceneName getName();
	SceneInfo* setName(MySceneName val) {
		name = val; 
		return this;
	}

	cocos2d::Value getData(string key, Value def);
	SceneInfo* setData(string key, Value val);
	string logName();
private:
	MySceneName name = LoadScene;
	map<string, Value> data;

};

class ControlScene
{
public:
	ControlScene();
	~ControlScene();
	static ControlScene* getInstance();
	static void showMemory();

	SceneInfo* getSceneInfo(MySceneName);
	void show();//展示栈内信息

	void backFromScene();
	void replaceScene(SceneInfo* CurrentScene, SceneInfo* NextScene, bool push = true);
	void end();
	void pushCurrentScene(SceneInfo* sc);
	SceneInfo* getFromScene(bool pop = true);
	bool getReplaceAble() const { return m_replace; }
	SceneInfo* getCurrentScene();
	void clear();

	int getLoadPlistCount() const { CCLOG("getLoadPlistCount = %d", m_loadPlistCount); return m_loadPlistCount; }
	void setLoadPlistCount(int val) { 
		lock.lock();
		m_loadPlistCount = val; 
		CCLOG("setLoadPlistCount = %d", m_loadPlistCount);
		lock.unlock();
	}
private:
	static ControlScene* instance;
	bool m_replace = true;
	SceneInfo* m_currentScene = nullptr;
	SceneInfo* m_beforeScene = nullptr;
	SceneInfo* m_nextScene = nullptr;
	vector<SceneInfo*> sceneStack;
	int m_loadPlistCount;//异步加载图片的数量
	mutex lock;
	std::unordered_map<int, SceneInfo*> sceneInfoMap;//元素由map提供 避免释放的问题

	bool replace(SceneInfo* CurrentScene, SceneInfo* NextScene, bool push =true);//核心函数;
	void preLoadResources(vector<string> plist, const function<void()> & callback);

};
#endif