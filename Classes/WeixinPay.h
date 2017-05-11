#ifndef __WeixinPay_H__
#define __WeixinPay_H__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <string>
#include "FileNames.h"

USING_NS_CC;
using namespace std;

class WeixinPay
{
public:
	WeixinPay();
	~WeixinPay();
	static WeixinPay* getInstance();
	static void destoryInstance();

	void newPay(int memberId, int payType, string card_or_recharge, const CallBack_String & call1, const CallBack_String & call2);
	void writeOrderId(string json);

	CallBack_String getCallback() const { return m_callback; }
	void setCallback(const CallBack_String& val) { m_callback = val; }
	CallBack_String getCallbackerror() const { return m_callbackerror; }
	void setCallbackerror(const CallBack_String& val) { m_callbackerror = val; }
private:
	static WeixinPay* instance;
	CallBack_String m_callback = nullptr;
	CallBack_String m_callbackerror = nullptr;
};

#endif