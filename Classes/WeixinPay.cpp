#include "WeixinPay.h"
#include <string>
#include "App.h"
using namespace std;

WeixinPay* WeixinPay::instance = nullptr;

WeixinPay::WeixinPay()
{

}

WeixinPay::~WeixinPay()
{

}

WeixinPay* WeixinPay::getInstance()
{
	if(instance == nullptr)
		instance = new WeixinPay();
	return instance;
}

void WeixinPay::destoryInstance()
{
	if (instance)
	{
		delete instance;
	}
	instance = nullptr;
}

void WeixinPay::newPay(int memberId, int payType, string card_or_recharge, const CallBack_String & call1, const CallBack_String & call2)
{
	m_callback = call1;
	m_callbackerror = call2;
	map<string, string> p;
	p["memberid"] = Value(memberId).asString();
	p["paytype"] = Value(payType).asString();
	p["contenttpye"] = card_or_recharge;
	p["resource"] = "android";
	p["version"] = App::GetInstance()->version;
	auto strJson = YYXLayer::getStringFormMap(p);
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "newWeixinPay", strJson, result);
}

void WeixinPay::writeOrderId(string json)
{
	map<string, string> data;
	YYXLayer::TraversingJson(json, data);
	data["paytype"] = "wxpay";
	YYXLayer::setFileValue("payinfo", YYXLayer::getStringFormMap(data));
}

