#ifndef __Recharge_LAYER_H__
#define __Recharge_LAYER_H__

#include "cocos2d.h"
#include "App.h"
#include "YYXLayer.h"

class Recharge : public cocos2d::Layer
{
public:
	Recharge();
	~Recharge();
	bool init();
	void stringHttpRequest(HttpRequest::Type type, string url, map<string, string> paramter, int outTime, std::function<void(string json)> runable, std::function<void()> errorRunable);
	void httpRedPacketActivity();
	void setpiaodao(Node* node, int index, int redPacketCount);
	static Recharge* create();

private:
	int m_price = 300;//充值价格，单位“分”
	bool isPayWx = true;//是否是微信支付
};

#endif // __Recharge_LAYER_H__
