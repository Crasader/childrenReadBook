#ifndef __VIPNOTIFY_H__
#define __VIPNOTIFY_H__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
/*会员系列图层
会员到期提醒
会员袭来通知
*/
class VipNotify
{
public:
	static VipNotify* getInstance();
	static void destoryInstance();

	Layer* VipNotifyLayer();//会员重磅来袭
	bool controlAdvertisement();

	bool controlVipxufei();//控制会员续费提醒;
	Layer* VipTimeOutLayer();//会员到期提醒

	VipNotify();
	~VipNotify();

	int getTimeOutDay() const { return m_timeOutDay; }
	void setTimeOutDay(int val) { m_timeOutDay = val; }
	int getBookCount() const { return m_bookCount; }
	void setBookCount(int val) { m_bookCount = val; }
	int getReadBookHour() const { return m_readBookHour; }
	void setReadBookHour(int val) { m_readBookHour = val; }
	int getReadBookMinute() const { return m_readBookMinute; }
	void setReadBookMinute(int val) { m_readBookMinute = val; }
	int getAllMoney() const { return m_allMoney; }
	void setAllMoney(int val) { m_allMoney = val; }
	int getSurplus() const { return m_surplus; }
	void setSurplus(int val) { m_surplus = val; }
	bool getBreak_VipNotifyLayer() const { return m_break_VipNotifyLayer; }
	void setBreak_VipNotifyLayer(bool val) { m_break_VipNotifyLayer = val; }
private:
	static VipNotify* instance;
	int m_timeOutDay = -999;//到期时间
	int m_bookCount = -999;//书籍数量
	int m_readBookHour = -999;//阅读时间 小时
	int m_readBookMinute = -999;//阅读时间 分钟
	int m_allMoney = -999;//书的总价值
	int m_surplus = -999;//节省价值
	//是否中断过程
	bool m_break_VipNotifyLayer = false;
	Node* m_notify = nullptr;
};

#endif