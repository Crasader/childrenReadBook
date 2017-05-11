#include "vipNotifyLayer.h"
#include <string>
#include "App.h"
#include "YYXTime.h"
#include "YYXVisitor.h"
#include "BuyVip.h"
#include "WeixinPay.h"
#include "AppHttp.h"
using namespace std;

VipNotify* VipNotify::instance = nullptr;

VipNotify* VipNotify::getInstance()
{
	if(instance == nullptr)
		instance = new VipNotify();
	return instance;
}

void VipNotify::destoryInstance()
{
	if (instance)
	{
		delete instance;
	}
	instance = nullptr;
}

Layer* VipNotify::VipNotifyLayer()
{
	if (m_break_VipNotifyLayer)
	{
		auto leyer = BuyVip::getInstance()->show();
		BuyVip::getInstance()->setCallback([](string josn) {
			AppHttp::getInstance()->httpCheckVIP();
		});
		leyer->removeFromParentAndCleanup(true);
		m_break_VipNotifyLayer = false;
		return leyer;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Data data;
	string csb = "Notification/csb/zhongbanglaixi.csb";
	Layer* pLayer = nullptr;
	if (App::GetInstance()->getData(csb, data))
		pLayer = (Layer*)CSLoader::createNode(data);
	else
		pLayer = (Layer*)CSLoader::createNode(csb);
	pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	auto close = (Button*)pLayer->getChildByName("Button_1");
	close->addClickEventListener([=](Ref* sender) {
		YYXLayer::setFileValue("vipnotify", Value(YYXTime::getInstance()->getNowDay()).asString());
		pLayer->removeFromParentAndCleanup(true);
	});
	auto tiyan = (Button*)pLayer->getChildByName("Button_2");
	//用户体验按钮
	tiyan->addClickEventListener([=](Ref* sender) {
		pLayer->runAction(Sequence::create(ScaleTo::create(0.3f, 0), CallFuncN::create([=](Ref* sender) {
			YYXLayer::setFileValue("vipnotify", Value(YYXTime::getInstance()->getNowDay()).asString());
			pLayer->removeFromParentAndCleanup(true);
			if (YYXVisitor::getInstance()->getVisitorMode())
			{
				auto con = ControlScene::getInstance();
				con->replaceScene(con->getCurrentScene(), con->getSceneInfo(LoginScene));
				m_break_VipNotifyLayer = true;
			}
			else
			{
				auto leyer = BuyVip::getInstance()->show();
				if (leyer)
					Director::getInstance()->getRunningScene()->addChild(leyer);
				BuyVip::getInstance()->setCallback([](string josn) {
					AppHttp::getInstance()->httpCheckVIP();
				});
			}
		}), NULL));
	});
	auto bg = (ImageView*)pLayer->getChildByName("Image_1");
	bg->setTouchEnabled(true);
	return pLayer;
}

bool VipNotify::controlVipxufei()//控制会员续费提醒
{
	bool _show = false;
	auto times = VipNotify::getInstance()->getTimeOutDay();
	if (User::getInstance()->getVip())
	{
		if (times >= 21 && times <=30) {
			if (YYXLayer::getFileValue("VIP_WEEK1", "0") == "0") {
				_show = true;
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)					
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_WEEK1", "1");
			}
		}
		else if (times >= 14 && times <  21) {
			if (YYXLayer::getFileValue("VIP_WEEK2", "0") == "0") {
				_show = true;
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_WEEK2", "1");
			}
		}
		else if (times >=  7 && times <   14) {
			if (YYXLayer::getFileValue("VIP_WEEK3", "0") == "0") {
				_show = true;
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_WEEK3", "1");
			}
		}
		else if (times >=  6 && times <   7) {
			if (YYXLayer::getFileValue("VIP_DAY1", "0") == "0") {
				_show = true; 
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_DAY1", "1");
			}
		}
		else if (times >=   5 && times <   6) {
			if (YYXLayer::getFileValue("VIP_DAY2", "0") == "0") {
				_show = true; 
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_DAY2", "1");
			}
		}
		else if (times >=   4 && times <   5) {
			if (YYXLayer::getFileValue("VIP_DAY3", "0") == "0") {
				_show = true;
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_DAY3", "1");
			}
		}
		else if (times >=   3 && times <   4) {
			if (YYXLayer::getFileValue("VIP_DAY4", "0") == "0") {
				_show = true; 
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_DAY4", "1");
			}
		}
		else if (times >=   2 && times <   3) {
			if (YYXLayer::getFileValue("VIP_DAY5", "0") == "0") {
				_show = true; 
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_DAY5", "1");
			}
		}
		else if (times >=   1 && times <   2) {
			if (YYXLayer::getFileValue("VIP_DAY6", "0") == "0") {
				_show = true;
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_DAY6", "1");
			}
		}
		else if (times >=   0 && times <   1) {
			if (YYXLayer::getFileValue("VIP_DAY7", "0") == "0") {
				_show = true;
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for (auto it : da)
					YYXLayer::deleteFileValue(it);
				YYXLayer::setFileValue("VIP_DAY7", "1");
			}
		}
	}
	else
	{
		vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
		for (auto it : da)
			YYXLayer::deleteFileValue(it);
	}
	return _show;
}

Layer* VipNotify::VipTimeOutLayer()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Data data;
	string csb = "Notification/csb/viptimeout.csb";
	Layer* pLayer = nullptr;
	if (App::GetInstance()->getData(csb, data))
		pLayer = (Layer*)CSLoader::createNode(data);
	else
		pLayer = (Layer*)CSLoader::createNode(csb);
	pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	auto close = (Button*)pLayer->getChildByName("Button_1");
	close->addClickEventListener([=](Ref* sender) {
		pLayer->removeFromParentAndCleanup(true);
	});
	auto xufei = (Button*)pLayer->getChildByName("Button_7");
	xufei->addClickEventListener([=](Ref* sender) {
		pLayer->removeFromParentAndCleanup(true);
		auto layer = BuyVip::getInstance()->show();
		if (layer)
			Director::getInstance()->getRunningScene()->addChild(layer);
		BuyVip::getInstance()->setCallback([](string josn) {
			//如果是余额支付 vip升级成功后已经获得了余额，只要刷新余额
			YYXLayer::sendNotify("ParentScenehttpGetUserBalanceReferBalance");
			AppHttp::getInstance()->httpCheckVIP([]() {
				YYXLayer::sendNotify("refershMemberIDVIP");//提示刷新父母设置界面
			});
		});
	});
	auto bg = (ImageView*)pLayer->getChildByName("bg");
	bg->setTouchEnabled(true);
	//到期天数
	auto day = (Text*)pLayer->getChildByName("Text_48_1");
	day->setText(Value(m_timeOutDay).asString());
	//书籍数量
	auto count = (Text*)pLayer->getChildByName("Text_78_1_2_0_0");
	count->setText(Value(m_bookCount).asString());
	//阅读时间 小时
	auto hour = (Text*)pLayer->getChildByName("Text_78_1_2_0_1");
	hour->setText(Value(m_readBookHour).asString());
	//阅读时间 分钟
	auto minute = (Text*)pLayer->getChildByName("Text_78_1_2_0_2");
	minute->setText(Value(m_readBookMinute).asString());
	auto allMoney = (Text*)pLayer->getChildByName("Text_78_1_2_0");
	auto surplus = (Text*)pLayer->getChildByName("Text_78_1_2_0_3");
	if (m_surplus > 0)
	{
		//书的总价值
		allMoney->setText(Value(m_allMoney).asString());
		//节省价值
		surplus->setText(Value(m_surplus).asString());
	}
	else
	{
		auto text = (Text*)pLayer->getChildByName("Text_78_1_2");
		text->setVisible(false);
		allMoney->setVisible(false);
		surplus->setVisible(false);
	}
	return pLayer;
}

bool VipNotify::controlAdvertisement()//控制广告展示
{
	//一天一次
	bool show = false;
	string vipnotifyDay = YYXLayer::getFileValue("vipnotify", "0");
	int nowDay = YYXTime::getInstance()->getNowDay();
	if (nowDay != Value(vipnotifyDay).asInt())
	{
		if (YYXVisitor::getInstance()->getVisitorMode() || !User::getInstance()->getVip())
			show = true;
	}
	if (m_break_VipNotifyLayer && !YYXVisitor::getInstance()->getVisitorMode())
	{
		show = true;
	}
	return show;
}

VipNotify::VipNotify()
{
}

VipNotify::~VipNotify()
{
}