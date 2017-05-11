#ifndef __CHARGER_H__
#define __CHARGER_H__

#include "cocos2d.h"
#include "YYXStruct.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "App.h"
#include "NetIntface.h"

USING_NS_CC;

class Charger : public Layer 
{
private:
	float m_payPrice;
	int m_payType;//微信0 支付宝1
	int m_payIndex;//点击充值金额的位置
	Layer* pLayer;
	function<void()> m_callback=nullptr;//充值成功的回调
public:
	static Charger* create();
	bool init();
	static int getDatahongbao(int idx);
	static int getDatamoney(int idx);
	Charger();
	~Charger();	
	int PayType() const { return m_payType; }
	void PayType(int val) { m_payType = val; }
	int PayIndex()  {
		if (m_payIndex < 0 || m_payIndex>6)
			m_payIndex = 0;
		return m_payIndex;
	}
	void PayIndex(int val) {
		m_payIndex = val;
		YYXLayer::setFileValue("chargerSelectIndex", StringUtils::format("%d", val));
	}
	void setNodePrice(int tag, bool sel=false);
	//static void httpChargerInfo();//网络请求充值的信息
	virtual void cleanup();
	float PayPrice() const { return m_payPrice; }
	void PayPrice(float val) { m_payPrice = val; }
	void setCallback(const function<void()>& val) { m_callback = val; }
};

#endif