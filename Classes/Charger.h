#ifndef __CHARGER_H__
#define __CHARGER_H__

#include "cocos2d.h"
#include "YYXStruct.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "App.h"
#include "CrossPlatform.h"

USING_NS_CC;

#define WEIXIN 0
#define ZHIFUBAO 1

class ChargeData
{
public:
	ChargeData();
	~ChargeData();
	static ChargeData* create();
	static void del(ChargeData* );

	int getHongbao() const { return m_hongbao; }
	void setHongbao(int val) { m_hongbao = val; }
	int getHongbaoId() const { return m_hongbaoId; }
	void setHongbaoId(int val) { m_hongbaoId = val; }
	int getPrice() const { return m_price; }
	void setPrice(int val) { m_price = val; }
	int getPayId() const { return m_payId; }
	void setPayId(int val) { m_payId = val; }
	int getIndex() const { return m_index; }
	void setIndex(int val) { m_index = val; }
private:
	int m_hongbao = -999;
	int m_hongbaoId = -999;
	int m_price = -999;
	int m_payId = -999;
	int m_index = -999;
};

class Charger : public Layer 
{
private:
	static Charger* instance;
	int m_payPlatform = 0;//微信0 支付宝1
	int m_index = -999;//点击的位置
	CallBackFunction m_callback=nullptr;//充值成功的回调
	CallBackFunction m_callbackerror = nullptr;//充值失败的回调
	std::unordered_map<int, ChargeData*> m_data;//充值金额容器

	Layer* pLayer = nullptr;
	void setNode(Node* node, ChargeData* data);

public:
	Charger();
	~Charger();
	static Charger* getInstance();

	void show();//展示充值界面

	void addData(ChargeData*);
	ChargeData* getData(int idx);
	bool haveData();//是否有数据
	Layer* newChargeLayer();
	void httpCallChargeLayer();//网络请求完成吊起界面

	CallBackFunction getCallback() const { return m_callback; }
	void setCallback(const CallBackFunction& val) { m_callback = val; }

	CallBackFunction getCallbackerror() const { return m_callbackerror; }
	void setCallbackerror(const CallBackFunction& val) { m_callbackerror = val; }
	int getPayPlatform() const { return m_payPlatform; }
	void setPayPlatform(int val) { m_payPlatform = val; }
	int getIndex() const { return m_index; }
	void setIndex(int val) { m_index = val; }
};

#endif