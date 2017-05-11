#ifndef __HTTPWAITIND_H__
#define __HTTPWAITIND_H__

#include "cocos2d.h"
#include <atomic>
#include "FileNames.h"
using namespace std;
USING_NS_CC;

class HttpWaiting
{
public:
	HttpWaiting();
	~HttpWaiting();

	static HttpWaiting* getInstance();
	Layer* newWaitingLayer(int key=-999, const CallBackFunction& func = nullptr);
	void remove();

	CallBackFunction getCallback() const { return m_callback; }
	void setCallback(const CallBackFunction& val) { m_callback = val; }
private:
	static HttpWaiting* instance;
	Layer* m_layer = nullptr;
	CallBackFunction m_callback = nullptr;
};

#endif // __HttpWaiting_SCENE_H__
