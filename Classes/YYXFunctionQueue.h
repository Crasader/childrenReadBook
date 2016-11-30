#ifndef __YYXFunctionQueue_H__
#define __YYXFunctionQueue_H__

#include "cocos2d.h"
#include <queue>

using namespace std;

class YYXFunctionQueue
{
private:
	static YYXFunctionQueue* instance;
	//先进先出队列
	queue<function<void()>> runQueue;
	//阻塞
	bool block = false;
	//同步锁
	std::mutex mtx;
public:
	YYXFunctionQueue();
	~YYXFunctionQueue();
	//获取一个单例
	static YYXFunctionQueue* GetInstance();
	//队列开始执行
	void start();
	//队列清空
	void clear();
	//队列添加
	void add(function<void()> runFun);
	//阻塞,不再执行下一个
	void interdict();
	//恢复执行, 不阻塞
	void resumption();
};

#endif // __YYXFunctionQueue_SCENE_H__
