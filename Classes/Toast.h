#ifndef __Toast_H__
#define __Toast_H__

#include "cocos2d.h"
#include <queue>
#include "YYXLayer.h"
USING_NS_CC;
using namespace std;

class Toast
{
public:
	static Toast* instance;
	long long m_time = 0;//上次调用时刻
	bool m_runing = false;//是否正在运行
	int m_showTime = 3;//展示时间
	Node* m_showLayer = nullptr;//展示的图层
	queue<std::string> m_dataQueue;
	map<string, int> huifuMap;//转场恢复展示的次数
public:
	Toast();
	~Toast();
	static Toast* GetInstance();
	static void create(const char* msg, bool show=true, float time=3.0);
	void init(const char* msg, bool show);
	void showToast();
	void showNext();
	void unvisiable();
	void setShowTime(int second);
	int getShowTime();
	void setRuning(bool s);
	bool getRuning();
	void SceneInitToast();
};

#endif // __Toast_SCENE_H__
