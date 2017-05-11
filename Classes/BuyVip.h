#ifndef __BUYVIP_H__
#define __BUYVIP_H__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <string>
#include "FileNames.h"

USING_NS_CC;
using namespace std;

enum payType
{
	MyMoney,AliPay,WeixinPay
};

enum vipType
{
	_7Day = 2, _30Day = 0, _365Day = 1
};

/*开通会员+ 支付选择 */
class VipTypeData
{
private:
	int m_price = -999;//29
	string m_name = "";//咿啦体验会员
	string m_timeName = "";//月卡
	vipType m_biaoqian = _30Day;//0 无 1 体验 2 特惠
	int m_buycount = -999;//1限购 999不限购
	int m_cardType = -999;//上传用的类型
public:
	static VipTypeData* create();
	static void del(VipTypeData*);
	VipTypeData();
	~VipTypeData();

	int getPrice() const { return m_price; }
	VipTypeData* setPrice(int val) {
		m_price = val;
		return this;
	}

	std::string getName() const { return m_name; }
	VipTypeData* setName(std::string val) {
		m_name = val; 
		return this;
	}

	std::string getTimeName() const { return m_timeName; }
	VipTypeData* setTimeName(std::string val) {
		m_timeName = val;
		return this;
	}

	vipType getBiaoqian() const { return m_biaoqian; }
	VipTypeData* setBiaoqian(vipType val) {
		m_biaoqian = val;
		return this;
	}

	int getBuycount() const { return m_buycount; }
	VipTypeData* setBuycount(int val) {
		m_buycount = val;
		return this;
	}

	int getCardType() const { return m_cardType; }
	VipTypeData* setCardType(int val) {
		m_cardType = val;
		return this;
	}
};

class BuyVip
{
public:
	BuyVip();
	~BuyVip();
	static BuyVip* getInstance();
	static void destoryInstance();
	//会员选择
	Layer* show();
	void controlShow();
	//支付选择
	Layer* payLayer();
	void clear();
	void addData(VipTypeData* data);
	int getDataSize()
	{
		return m_dat.size();
	}

	CallBack_String getCallback() const { return m_callback; }
	void setCallback(const CallBack_String& val) { m_callback = val; }
	CallBack_String getCallbackerror() const { return m_callbackerror; }
	void setCallbackerror(const CallBack_String& val) { m_callbackerror = val; }
private:
	static BuyVip* instance;
	int m_money = -999;
	payType m_payType = WeixinPay;
	int m_VipType = -999;
	vector<VipTypeData*> m_dat;//会员类型的数据
	CallBack_String m_callback = nullptr;
	CallBack_String m_callbackerror = nullptr;

	void initNode(Node* node, VipTypeData* data);
	void initPayNode_MyMoney(Node* node);
	void initPayNode_otherPay(Node* node, payType types);
	void addAnimation(Layer* layer);
	Layer* BuyVip7DayLayer();
	Layer* BuyVipLayer();
};

#endif