#include <iostream>   
#include <string>   
#include "XZLayer.h"
#include "App.h"


//展示分享红包
void XZLayer::showShareRedPacket(string redCount)
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

//到期提示
YYXLayer* XZLayer::showVIPRenew(function<void()>runleave)
{
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "IndexSceneShowVIPRenew");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "VIP/csb/daoqitixing.csb");
	auto viphint = YYXLayer::create(paramter);
	auto pNode = viphint->getParentNode();
	pNode->setAnchorPoint(Vec2(0.5, 0.5));

	auto bkground = (ImageView*)viphint->findControl("Image_1");
	pNode->setPosition(Director::getInstance()->getWinSize() / 2);
	bkground->setSwallowTouches(true);
	if (bkground)
	{
		bkground->setTouchEnabled(true);
		bkground->addClickEventListener([=](Ref* sender) {

		});
	}
	auto bkground1 = (ImageView*)viphint->findControl("Image_2");
	if (bkground1)
	{
		bkground1->addClickEventListener([=](Ref* sender) {

		});
	}
	auto bclose = (Button*)viphint->findControl("Button_1");
	if (bclose)
	{
		bclose->addClickEventListener([=](Ref* sender) {
			viphint->removeFromParent();
		});
	}
	auto time = (Text*)viphint->findControl("Text_2");
	if (time)
	{
		time->setText(StringUtils::format("%d", App::GetInstance()->m_me->vipTime / 86400));
	}
	auto gopay = (Button*)viphint->findControl("Button_3");
	if (gopay)
	{
		gopay->addClickEventListener([=](Ref* sender) {
			if (App::GetInstance()->m_me)
			{
				viphint->removeFromParent();
				auto layer = XZLayer::payVip_xufei();
				if (layer)
					Director::getInstance()->getRunningScene()->addChild(layer);
			}
			else
			{
				if(runleave)
					runleave();
				Index::GoToLoginScene();
			}
		});
	}
	return viphint;
}

//续费包年服务
YYXLayer * XZLayer::payVip_xufei()
{
	bool isPayWx = true;//是否是微信支付
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "payVip");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "VIP/csb/vipxufei.csb");
	auto viphint = YYXLayer::create(paramter);
	auto pNode = viphint->getParentNode();
	pNode->setAnchorPoint(Vec2(0.5, 0.5));
	pNode->setPosition(Director::getInstance()->getWinSize() / 2);
	auto bkground = (ImageView*)viphint->findControl("Image_1");
	bkground->setSwallowTouches(true);
	if (bkground)
	{
		bkground->setTouchEnabled(true);
		bkground->addClickEventListener([=](Ref* sender) {

		});
	}
	auto bkground1 = (ImageView*)viphint->findControl("Image_2");
	if (bkground1)
	{
		bkground1->addClickEventListener([=](Ref* sender) {

		});
	}
	auto phonenum = (Text*)viphint->findControl("Text_12");
	if (phonenum)
	{
		string userAccount = YYXLayer::getFileValue("userAccount", "");
		phonenum->setText(userAccount);
	}
	auto datenumuntil = (Text*)viphint->findControl("Text_13");
	if (datenumuntil)
	{
		datenumuntil->setText(App::GetInstance()->m_me->endvip);
	}
	auto datenumnow = (Text*)viphint->findControl("Text_15");
	if (datenumnow)
	{
		datenumnow->setText(App::GetStringTime3());

	}
	auto isalipaycheck = false;
	auto wechatPay = (CheckBox*)viphint->findControl("CheckBox_3");
	auto aliPay = (CheckBox*)viphint->findControl("CheckBox_4");
	auto lister_wechatPay = [=](Ref* sender) {
		aliPay->setSelectedState(false);
		wechatPay->setSelectedState(true);

	};
	wechatPay->addClickEventListener(lister_wechatPay);
	auto lister_aliPay = [=](Ref* sender) {
		aliPay->setSelectedState(true);
		wechatPay->setSelectedState(false);
	};
	aliPay->addClickEventListener(lister_aliPay);

	auto gopay = (Button*)viphint->findControl("Button_3");
	if (gopay)
	{
		gopay->addClickEventListener([=](Ref* sender) {
			int m_price = 24000;//充值价格，单位“分”
			if (App::m_debug == 0)
				m_price = 1;
			long payCount = m_price;
			string info = App::getString("RECHARGE") + StringUtils::format("%.02f ", m_price / 100.0) + App::getString("YUAN");
			string paytype = "alipay";
			if (wechatPay->getSelectedState())
				paytype = "weixinpay";
			App::log("支付启动");
			if (App::GetInstance()->m_me)
				NetIntface::httpVIPPay(App::GetInstance()->m_me->id, payCount, m_price, paytype, info, "httpVIPPaySuccess", [](string str) {
				//续费成功
				Toast::create(App::getString("CHONGZHICHENGGONG"), false);
				App::httpCheckVIP(App::GetInstance()->m_me->id);
				vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
				for(auto it : da)
					YYXLayer::deleteFileValue(it);
			}, "httpVIPPayError", [](string error) {
				Toast::create(error.c_str());
			});
			viphint->removeFromParentAndCleanup(true);
		});
	}
	auto bclose = (Button*)viphint->findControl("Button_1");
	if (bclose)
	{
		bclose->addClickEventListener([=](Ref* sender) {
			viphint->removeFromParent();
		});
	}
	auto back = (Button*)viphint->findControl("Button_4");
	if (back)
	{
		back->addClickEventListener([=](Ref* sender) {
			viphint->removeFromParent();
		});
	}
	return viphint;
}

//开通年卡服务  (您进入的是年卡书店)   i=0在父母设置界面(去掉您进入的是年卡书店)  i=1 在书店
YYXLayer* XZLayer::OpenVIPCardService(int i, function<void()>runleave)
{
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "showYouAreVip", 0);
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "bookstoreShowUserIsVip");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "VIP/csb/youarenotvip.csb");
	auto viphint = YYXLayer::create(paramter);
	if (i == 0)
	{
		auto text = (ImageView*)viphint->findControl("Image_6");
		if (text)
			text->setVisible(false);
		auto content = (ImageView*)viphint->findControl("Image_5");
		if (content)
			content->setPosition(content->getPosition() + Vec2(0, 40));
	}
	auto pNode = viphint->getParentNode();
	pNode->setAnchorPoint(Vec2(0.5, 0.5));
	pNode->setPosition(Director::getInstance()->getWinSize() / 2);
	auto bkground = (ImageView*)viphint->findControl("Image_1");
	bkground->setSwallowTouches(true);
	if (bkground)
	{
		bkground->setTouchEnabled(true);
		bkground->addClickEventListener([=](Ref* sender) {
		});
	}
	auto bkground1 = (ImageView*)viphint->findControl("Image_2");
	if (bkground1)
	{
		bkground1->addClickEventListener([=](Ref* sender) {

		});
	}
	auto bclose = (Button*)viphint->findControl("Button_1");
	if (bclose)
	{
		bclose->addClickEventListener([=](Ref* sender) {
			viphint->removeFromParent();
		});
	}
	auto pay = (Button*)viphint->findControl("Button_3");
	if (pay)
	{
		pay->addClickEventListener([=](Ref* sender) {
			if (App::GetInstance()->m_me)
			{
				viphint->removeFromParent();
				auto paylayer = XZLayer::payBuyVip();
				if (paylayer)
					Director::getInstance()->getRunningScene()->addChild(paylayer);
			}
			else
			{				//未登录跳转登录
				if (runleave)
					runleave();
				Index::GoToLoginScene();
			}
		});
	}
	return viphint;
}

//购买包年服务
YYXLayer * XZLayer::payBuyVip()
{
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "payVip");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "VIP/csb/vippayy.csb");
	auto viphint = YYXLayer::create(paramter);
	auto pNode = viphint->getParentNode();
	pNode->setAnchorPoint(Vec2(0.5, 0.5));
	pNode->setPosition(Director::getInstance()->getWinSize() / 2);
	auto bkground = (ImageView*)viphint->findControl("Image_1");
	bkground->setSwallowTouches(true);
	if (bkground)
	{
		bkground->setTouchEnabled(true);
		bkground->addClickEventListener([=](Ref* sender) {});
	}
	auto bkground1 = (ImageView*)viphint->findControl("Image_2");
	if (bkground1)
	{
		bkground1->addClickEventListener([=](Ref* sender) {});
	}
	auto phonenum = (Text*)viphint->findControl("Text_12");
	if (phonenum)
	{
		string userAccount = YYXLayer::getFileValue("userAccount", "");
		phonenum->setText(userAccount);
	}
	auto datenum = (Text*)viphint->findControl("Text_15");
	if (datenum)
	{
		//datenum->setText(App::GetInstance()->m_me->endvip);
		datenum->setText(App::GetStringTime3());
	}
	auto isalipaycheck = false;
	auto wechatPay = (CheckBox*)viphint->findControl("CheckBox_3");
	auto aliPay = (CheckBox*)viphint->findControl("CheckBox_4");
	auto lister_wechatPay = [=](Ref* sender) {
		aliPay->setSelectedState(false);
		wechatPay->setSelectedState(true);
	};
	wechatPay->addClickEventListener(lister_wechatPay);
	auto lister_aliPay = [=](Ref* sender) {
		aliPay->setSelectedState(true);
		wechatPay->setSelectedState(false);
	};
	aliPay->addClickEventListener(lister_aliPay);

	auto gopay = (Button*)viphint->findControl("Button_3");
	if (gopay)
	{
		gopay->addClickEventListener([=](Ref* sender) {
			int m_price = 24000;
			if (App::m_debug == 0)
				m_price = 1;
			long payCount = m_price;
			string info = App::getString("RECHARGE") + StringUtils::format("%.02f ", m_price / 100.0) + App::getString("YUAN");
			string paytype = "alipay";
			if (wechatPay->getSelectedState())
				paytype = "weixinpay";
			App::log("支付启动");
			if (App::GetInstance()->m_me)
				NetIntface::httpVIPPay(App::GetInstance()->m_me->id, payCount, m_price, paytype, info, "httpVIPPaySuccess", [](string str) {
				Toast::create(App::getString("CHONGZHICHENGGONG"), false);
				App::httpCheckVIP(App::GetInstance()->m_me->id);
			}, "httpVIPPayError", [](string error) {
				Toast::create(error.c_str());
			});
			viphint->removeFromParentAndCleanup(true);
		});
	}

	auto bclose = (Button*)viphint->findControl("Button_1");
	if (bclose)
	{
		bclose->addClickEventListener([=](Ref* sender) {
			viphint->removeFromParent();
		});
	}
	auto back = (Button*)viphint->findControl("Button_4");
	if (back)
	{
		back->addClickEventListener([=](Ref* sender) {
			viphint->removeFromParent();
		});
	}
	return viphint;
}