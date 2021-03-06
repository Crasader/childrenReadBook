﻿#include <iostream>   
#include <string>   
#include "XZLayer.h"
#include "App.h"
#include "YYXVisitor.h"
#include "User.h"
#include "YYXTime.h"
#include "AppHttp.h"

//展示分享红包
void XZLayer::safe_ShowShareRedPacket(string redCount)
{
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
		map<string, int64String> parameter;
		YYXLayer::insertMap4ParaType(parameter, "className", -999, "showShareRedPacket");
		YYXLayer::insertMap4ParaType(parameter, "csb", -999, HONGBAOMESSAGEBOX);
		auto layer = create(parameter);
		if (!layer)
			return;
		auto content = (ImageView*)layer->findControl(FIND_SPRITE_HONGBAOMESSAGEBOX_CONTEXT);
		auto gotohongbaolist = (ImageView*)layer->findControl(FIND_IMAGEBUTTON_HONGBAOMESSAGEBOX_CONTEXT);
		auto background = (ImageView*)layer->findControl(FIND_IMAGE_HONGBAOMESSAGEBOX_CONTEXT);
		auto text = (Text*)layer->findControl(FIND_TEXT_HONGBAOMESSAGEBOX_CONTEXT);
		content->setTouchEnabled(true);
		gotohongbaolist->setTouchEnabled(true);
		background->setTouchEnabled(true);
		if (text)
			text->setText(redCount);
		if (background)
		{
			background->addClickEventListener([=](Ref* sender) {
				layer->removeFromParentAndCleanup(true);
			});
		}
		layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
		layer->getParentNode()->setPosition(Director::getInstance()->getVisibleSize() / 2);
		Director::getInstance()->getRunningScene()->addChild(layer, 10);
	});
}

////到期提示
//YYXLayer* XZLayer::showVIPRenew(function<void()>runleave)
//{
//	map<string, int64String> paramter;
//	YYXLayer::insertMap4ParaType(paramter, "className", -999, "IndexSceneShowVIPRenew");
//	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "VIP/csb/daoqitixing.csb");
//	auto viphint = YYXLayer::create(paramter);
//	auto pNode = viphint->getParentNode();
//	pNode->setAnchorPoint(Vec2(0.5, 0.5));
//
//	auto bkground = (ImageView*)viphint->findControl("Image_1");
//	pNode->setPosition(Director::getInstance()->getWinSize() / 2);
//	bkground->setSwallowTouches(true);
//	if (bkground)
//	{
//		bkground->setTouchEnabled(true);
//		bkground->addClickEventListener([=](Ref* sender) {
//
//		});
//	}
//	auto bkground1 = (ImageView*)viphint->findControl("Image_2");
//	if (bkground1)
//	{
//		bkground1->addClickEventListener([=](Ref* sender) {
//
//		});
//	}
//	auto bclose = (Button*)viphint->findControl("Button_1");
//	if (bclose)
//	{
//		bclose->addClickEventListener([=](Ref* sender) {
//			viphint->removeFromParent();
//		});
//	}
//	auto time = (Text*)viphint->findControl("Text_2");
//	if (time)
//	{
//		time->setText(StringUtils::format("%d", User::getInstance()->getVipTime() / 86400));
//	}
//	auto gopay = (Button*)viphint->findControl("Button_3");
//	if (gopay)
//	{
//		gopay->addClickEventListener([=](Ref* sender) {
//			if (!YYXVisitor::getInstance()->getVisitorMode())
//			{
//				viphint->removeFromParent();
//				auto layer = XZLayer::payVip_xufei();
//				if (layer)
//					Director::getInstance()->getRunningScene()->addChild(layer);
//			}
//			else
//			{
//				if(runleave)
//					runleave();
//				//Index::GoToLoginScene();
//				auto control = ControlScene::getInstance();
//				control->replaceScene(control->getCurrentScene(), ControlScene::getInstance()->getSceneInfo(LoginScene));
//			}
//		});
//	}
//	return viphint;
//}
//
////续费包年服务
//YYXLayer * XZLayer::payVip_xufei()
//{
//	bool isPayWx = true;//是否是微信支付
//	map<string, int64String> paramter;
//	YYXLayer::insertMap4ParaType(paramter, "className", -999, "payVip");
//	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "VIP/csb/vipxufei.csb");
//	auto viphint = YYXLayer::create(paramter);
//	auto pNode = viphint->getParentNode();
//	pNode->setAnchorPoint(Vec2(0.5, 0.5));
//	pNode->setPosition(Director::getInstance()->getWinSize() / 2);
//	auto bkground = (ImageView*)viphint->findControl("Image_1");
//	bkground->setSwallowTouches(true);
//	if (bkground)
//	{
//		bkground->setTouchEnabled(true);
//		bkground->addClickEventListener([=](Ref* sender) {
//
//		});
//	}
//	auto bkground1 = (ImageView*)viphint->findControl("Image_2");
//	if (bkground1)
//	{
//		bkground1->addClickEventListener([=](Ref* sender) {
//
//		});
//	}
//	auto phonenum = (Text*)viphint->findControl("Text_12");
//	if (phonenum)
//	{
//		string userAccount = "***********";
//		if (YYXVisitor::getInstance()->getVisitorMode())
//		{
//			userAccount = YYXVisitor::getInstance()->getVisitorName();
//		}
//		else
//		{
//			userAccount = User::getInstance()->getUserAccount();
//		}
//		phonenum->setText(userAccount);
//	}
//	auto datenumuntil = (Text*)viphint->findControl("Text_13");
//	if (datenumuntil)
//	{
//		datenumuntil->setText(User::getInstance()->getEndTime());
//	}
//	auto datenumnow = (Text*)viphint->findControl("Text_15");
//	if (datenumnow)
//	{
//		datenumnow->setText(App::GetStringTime3());
//
//	}
//	auto isalipaycheck = false;
//	auto wechatPay = (CheckBox*)viphint->findControl("CheckBox_3");
//	auto aliPay = (CheckBox*)viphint->findControl("CheckBox_4");
//	auto lister_wechatPay = [=](Ref* sender) {
//		aliPay->setSelectedState(false);
//		wechatPay->setSelectedState(true);
//
//	};
//	wechatPay->addClickEventListener(lister_wechatPay);
//	auto lister_aliPay = [=](Ref* sender) {
//		aliPay->setSelectedState(true);
//		wechatPay->setSelectedState(false);
//	};
//	aliPay->addClickEventListener(lister_aliPay);
//
//	auto gopay = (Button*)viphint->findControl("Button_3");
//	if (gopay)
//	{
//		gopay->addClickEventListener([=](Ref* sender) {
//			int m_price = 24000;//充值价格，单位“分”
//			if (App::m_PayTest == 1)
//				m_price = 1;
//			long payCount = m_price;
//			string info = App::getString("RECHARGE") + StringUtils::format("%.02f ", m_price / 100.0) + App::getString("YUAN");
//			string paytype = "alipay";
//			if (wechatPay->getSelectedState())
//				paytype = "weixinpay";
//			App::log("支付启动");
//				NetIntface::httpVIPPay(App::GetInstance()->getMemberId(), payCount, m_price, paytype, info, "httpVIPPaySuccess", [](string str) {
//				//续费成功
//				Toast::create(App::getString("CHONGZHICHENGGONG"), false);
//				AppHttp::getInstance()->httpCheckVIP([]() {
//					YYXLayer::sendNotify("refershMemberIDVIP");//提示刷新父母设置界面
//				});
//				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
//				for(auto it : da)
//					YYXLayer::deleteFileValue(it);
//			}, "httpVIPPayError", [](string error) {
//				Toast::create(error.c_str());
//			});
//			viphint->removeFromParentAndCleanup(true);
//		});
//	}
//	auto bclose = (Button*)viphint->findControl("Button_1");
//	if (bclose)
//	{
//		bclose->addClickEventListener([=](Ref* sender) {
//			viphint->removeFromParent();
//		});
//	}
//	auto back = (Button*)viphint->findControl("Button_4");
//	if (back)
//	{
//		back->addClickEventListener([=](Ref* sender) {
//			viphint->removeFromParent();
//		});
//	}
//	return viphint;
//}
//
////开通年卡服务  (您进入的是年卡书店)   i=0在父母设置界面(去掉您进入的是年卡书店)  i=1 在书店
//YYXLayer* XZLayer::OpenVIPCardService(int i, function<void()>runleave, const function<void()>& callback)
//{
//	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "showYouAreVip", 0);
//	map<string, int64String> paramter;
//	YYXLayer::insertMap4ParaType(paramter, "className", -999, "bookstoreShowUserIsVip");
//	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "VIP/csb/youarenotvip.csb");
//	auto viphint = YYXLayer::create(paramter);
//	auto text = (ImageView*)viphint->findControl("Image_6");
//	auto content = (ImageView*)viphint->findControl("Image_5");
//	if (i == 0)
//	{
//		if (text)
//			text->setVisible(false);
//		if (content)
//			content->setPosition(content->getPosition() + Vec2(0, 40));
//	}
//	if (i == 2)
//	{
//		if (text)
//		{
//			text->setSize(Size(600, 106));
//			text->loadTexture("other/Backcover_kaitongniankafuwu_ipad@2x.png");
//		}
//	}
//	auto pNode = viphint->getParentNode();
//	pNode->setAnchorPoint(Vec2(0.5, 0.5));
//	pNode->setPosition(Director::getInstance()->getWinSize() / 2);
//	auto bkground = (ImageView*)viphint->findControl("Image_1");
//	bkground->setSwallowTouches(true);
//	if (bkground)
//	{
//		bkground->setTouchEnabled(true);
//		bkground->addClickEventListener([=](Ref* sender) {
//		});
//	}
//	auto bclose = (Button*)viphint->findControl("Button_1");
//	if (bclose)
//	{
//		bclose->addClickEventListener([=](Ref* sender) {
//			viphint->removeFromParent();
//		});
//	}
//	auto pay = (Button*)viphint->findControl("Button_3");
//	if (pay)
//	{
//		pay->addClickEventListener([=](Ref* sender) {
//			YYXVisitor::getInstance()->hintLogin([=]() {
//				if (runleave)
//					runleave();
//				//Index::GoToLoginScene();
//			}, [=]() {
//				if (runleave)
//					runleave();
//				//Index::GoToLoginScene();
//			}, [=]() {
//				viphint->removeFromParent();
//				Director::getInstance()->getRunningScene()->addChild(Index::SelectLayer([=]() {
//					auto paylayer = XZLayer::payBuyVip(callback);
//					if (paylayer)
//						Director::getInstance()->getRunningScene()->addChild(paylayer);
//				}));				
//			}, "  ");
//		});
//	}
//	return viphint;
//}
//
////购买包年服务
//YYXLayer * XZLayer::payBuyVip(const function<void()>& callback)
//{
//	map<string, int64String> paramter;
//	YYXLayer::insertMap4ParaType(paramter, "className", -999, "payVip");
//	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "VIP/csb/vippayy.csb");
//	auto viphint = YYXLayer::create(paramter);
//	auto pNode = viphint->getParentNode();
//	pNode->setAnchorPoint(Vec2(0.5, 0.5));
//	pNode->setPosition(Director::getInstance()->getWinSize() / 2);
//	auto bkground = (ImageView*)viphint->findControl("Image_1");
//	bkground->setSwallowTouches(true);
//	if (bkground)
//	{
//		bkground->setTouchEnabled(true);
//		bkground->addClickEventListener([=](Ref* sender) {});
//	}
//	auto bkground1 = (ImageView*)viphint->findControl("Image_2");
//	if (bkground1)
//	{
//		bkground1->addClickEventListener([=](Ref* sender) {});
//	}
//	auto phonenum = (Text*)viphint->findControl("Text_12");
//	if (phonenum)
//	{
//		string userAccount = "***********";
//		if (YYXVisitor::getInstance()->getVisitorMode())
//		{
//			userAccount = YYXVisitor::getInstance()->getVisitorName();
//		}		
//		else
//		{
//			userAccount = User::getInstance()->getUserAccount();
//		}
//		phonenum->setText(userAccount);
//	}
//	auto datenum = (Text*)viphint->findControl("Text_15");
//	if (datenum)
//	{
//		datenum->setText(YYXTime::getStringFormLongLong(YYXTime::getInstance()->getNowTime4S()));
//	}
//	auto isalipaycheck = false;
//	auto wechatPay = (CheckBox*)viphint->findControl("CheckBox_3");
//	auto aliPay = (CheckBox*)viphint->findControl("CheckBox_4");
//	auto lister_wechatPay = [=](Ref* sender) {
//		aliPay->setSelectedState(false);
//		wechatPay->setSelectedState(true);
//	};
//	wechatPay->addClickEventListener(lister_wechatPay);
//	auto lister_aliPay = [=](Ref* sender) {
//		aliPay->setSelectedState(true);
//		wechatPay->setSelectedState(false);
//	};
//	aliPay->addClickEventListener(lister_aliPay);
//
//	auto gopay = (Button*)viphint->findControl("Button_3");
//	if (gopay)
//	{
//		gopay->addClickEventListener([=](Ref* sender) {
//			int m_price = 24000;
//			if (App::m_PayTest == 1)
//				m_price = 1;
//			long payCount = m_price;
//			string info = App::getString("RECHARGE") + StringUtils::format("%.02f ", m_price / 100.0) + App::getString("YUAN");
//			string paytype = "alipay";
//			if (wechatPay->getSelectedState())
//				paytype = "weixinpay";
//			App::log("支付启动");
//				NetIntface::httpVIPPay(App::GetInstance()->getMemberId(), payCount, m_price, paytype, info, "httpVIPPaySuccess", [=](string str) {
//				Toast::create(App::getString("CHONGZHICHENGGONG"), false);
//				AppHttp::getInstance()->httpCheckVIP([]() {
//					YYXLayer::sendNotify("refershMemberIDVIP");//提示刷新父母设置界面
//				});
//				YYXLayer::sendNotify("initHttp","bookinfo");
//				if (callback)
//					callback();
//			}, "httpVIPPayError", [](string error) {
//				Toast::create(error.c_str());
//			});
//			viphint->removeFromParentAndCleanup(true);
//		});
//	}
//
//	auto bclose = (Button*)viphint->findControl("Button_1");
//	if (bclose)
//	{
//		bclose->addClickEventListener([=](Ref* sender) {
//			viphint->removeFromParent();
//		});
//	}
//	auto back = (Button*)viphint->findControl("Button_4");
//	if (back)
//	{
//		back->addClickEventListener([=](Ref* sender) {
//			viphint->removeFromParent();
//		});
//	}
//	return viphint;
//}