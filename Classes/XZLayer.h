#ifndef __XZLayer_H__
#define __XZLayer_H__

#include "cocos2d.h"
#include "YYXLayer.h"
#include "IndexScene.h"
USING_NS_CC;
using namespace std;

class XZLayer : public YYXLayer
{
public:
	static void safe_ShowShareRedPacket(string redCount);
	//提示续费包年服务
	//static YYXLayer* showVIPRenew(function<void()>runleave);
	//续费包年服务,选择支付界面
	//static YYXLayer* payVip_xufei();
	//提示开通年卡服务  (您进入的是年卡书店)   i=0在父母设置界面(去掉您进入的是年卡书店)  i=1 在书店
	//static YYXLayer* OpenVIPCardService(int cun, function<void()>runleave = nullptr, const function<void()>& callback = nullptr);
	//购买包年服务, 选择支付界面
	//static YYXLayer* payBuyVip(const function<void()>& callback = nullptr);
};

#endif 
