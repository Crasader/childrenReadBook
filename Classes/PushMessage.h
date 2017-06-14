#pragma once
#ifndef __PushMessage_H__
#define __PushMessage_H__
#include "cocos2d.h"

using namespace std;

enum MessageType
{
	skipScene = 0,//跳转
	showMessage = 1,//弹窗
	logout = 2//登出
};

class PushMessage
{
public:
	static PushMessage* getInstance();
	PushMessage();
	~PushMessage();

	mutex m_Mdata;
	void pushTask(string json);
	string popTask();
	int getTaskCount();
	void clear();

	void newTask(string json);
private:
	static PushMessage* instance;
	queue<string> m_data;//队列
	std::atomic_bool m_run;
	mutex m_Mhander;
	void startHander();
	void handerTask(string json);
};
#endif