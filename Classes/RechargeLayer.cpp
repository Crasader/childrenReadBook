#include "RechargeLayer.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "NetIntface.h"
#include "XZLayer.h"

USING_NS_CC;
using namespace cocostudio::timeline;

Recharge::Recharge() {
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Pay/csb/payong.plist");
	m_price = 0;
	isPayWx = true;
}

Recharge::~Recharge() {
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("Pay/csb/payong.plist");
}

Recharge* Recharge::create() {
	Recharge *pRect = new Recharge();
	if (pRect) {
		if (pRect->init()) {
			pRect->autorelease();
		}
		else {
			pRect->release();
			pRect = nullptr;
		}
	}
	return pRect;
}

bool Recharge::init() {
	if (!Layer::init())
	{
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	httpRedPacketActivity();
	Layer* pLayer;
	Data data;
	if (App::GetInstance()->getData(RECHARGE_CSB, data))
		pLayer = (Layer*)CSLoader::createNode(data);
	else
		pLayer = (Layer*)CSLoader::createNode(RECHARGE_CSB);
	pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(pLayer);
	//触摸屏蔽层
	auto bg_ImgView = (ImageView*)pLayer->getChildByName("bg_Imgview");
	bg_ImgView->setTouchEnabled(true);
	//背景--节点
	auto sprBg = (Sprite*)pLayer->getChildByName("sprBg");
	//6中充值币
	auto coinImg1 = (ImageView*)sprBg->getChildByName("coin1");
	coinImg1->setTouchEnabled(true);
	auto coinImg2 = (ImageView*)sprBg->getChildByName("coin2");
	coinImg2->setTouchEnabled(true);
	auto coinImg3 = (ImageView*)sprBg->getChildByName("coin3");
	coinImg3->setTouchEnabled(true);
	auto coinImg4 = (ImageView*)sprBg->getChildByName("coin4");
	coinImg4->setTouchEnabled(true);
	auto coinImg5 = (ImageView*)sprBg->getChildByName("coin5");
	coinImg5->setTouchEnabled(true);
	auto coinImg6 = (ImageView*)sprBg->getChildByName("coin6");
	coinImg6->setTouchEnabled(true);
	//活动飘带
	auto red0 = pLayer->getChildByName("FileNode_0");
	auto red1 = pLayer->getChildByName("FileNode_1");
	auto red2 = pLayer->getChildByName("FileNode_2");
	auto red3 = pLayer->getChildByName("FileNode_3");
	auto red4 = pLayer->getChildByName("FileNode_4");
	auto red5 = pLayer->getChildByName("FileNode_5");
	auto function = [=](Node* red0, int index , string key) {
		if (red0) {
			auto count = YYXStruct::getMapInt64(App::GetInstance()->myData, key, 0);
			setpiaodao(red0, index, count);
		}
	};
	function(red0, 0, "RechargeIndex=0");
	function(red1, 1, "RechargeIndex=1");
	function(red2, 2, "RechargeIndex=2");
	function(red3, 3, "RechargeIndex=3");
	function(red4, 4, "RechargeIndex=4");
	function(red5, 5, "RechargeIndex=5");
	//选中图片
	auto sprSelected = (Sprite*)sprBg->getChildByName("spr_select");
	m_price = 300;
	sprSelected->setPosition(Vec2(coinImg1->getPositionX(), coinImg1->getPositionY() + 6));
	//充值币点击事件
	coinImg1->addClickEventListener([=](Ref* sender) {
		m_price = 300;
		sprSelected->setPosition(Vec2(coinImg1->getPositionX(), coinImg1->getPositionY() + 6));
		YYXLayer::sendNotify("changePiaoDai", "", 0);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "rechargeSuccessIndex", 0);
	});
	coinImg2->addClickEventListener([=](Ref* sender) {
		m_price = 600;
		sprSelected->setPosition(Vec2(coinImg2->getPositionX(), coinImg2->getPositionY() + 6));
		YYXLayer::sendNotify("changePiaoDai", "", 1);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "rechargeSuccessIndex", 1);
	});
	coinImg3->addClickEventListener([=](Ref* sender) {
		m_price = 1200;
		sprSelected->setPosition(Vec2(coinImg3->getPositionX(), coinImg3->getPositionY() + 6));
		YYXLayer::sendNotify("changePiaoDai", "", 2);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "rechargeSuccessIndex", 2);
	});
	coinImg4->addClickEventListener([=](Ref* sender) {
		m_price = 3000;
		sprSelected->setPosition(Vec2(coinImg4->getPositionX(), coinImg4->getPositionY() + 6));
		YYXLayer::sendNotify("changePiaoDai", "", 3);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "rechargeSuccessIndex", 3);
	});
	coinImg5->addClickEventListener([=](Ref* sender) {
		m_price = 5000;
		sprSelected->setPosition(Vec2(coinImg5->getPositionX(), coinImg5->getPositionY() + 6));
		YYXLayer::sendNotify("changePiaoDai", "", 4);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "rechargeSuccessIndex", 4);
	});
	coinImg6->addClickEventListener([=](Ref* sender) {
		m_price = 9800;
		sprSelected->setPosition(Vec2(coinImg6->getPositionX(), coinImg6->getPositionY() + 6));
		YYXLayer::sendNotify("changePiaoDai", "", 5);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "rechargeSuccessIndex", 5);
	});
	//微信支付
	auto pay_weixin = (ImageView*)sprBg->getChildByName("Image_11");
	pay_weixin->setTouchEnabled(true);
	//支付宝支付
	auto pay_zhifubao = (ImageView*)sprBg->getChildByName("Image_12");
	pay_zhifubao->setTouchEnabled(true);

	pay_weixin->addClickEventListener([=](Ref* sender) {
		isPayWx = true;
		pay_weixin->loadTexture("Pay/res/ApplePay_weixin_sel_667h.png", TextureResType::PLIST);
		pay_zhifubao->loadTexture("Pay/res/ApplePay_zhifubao_667h.png", TextureResType::PLIST);
	});
	pay_zhifubao->addClickEventListener([=](Ref* sender) {
		isPayWx = false;
		pay_weixin->loadTexture("Pay/res/ApplePay_weixin_667h.png", TextureResType::PLIST);
		pay_zhifubao->loadTexture("Pay/res/ApplePay_zhifubao_sel_667h.png", TextureResType::PLIST);
	});
	//关闭按钮
	auto close_btn = (Button*)sprBg->getChildByName("close_btn");
	close_btn->setTouchEnabled(true);
	close_btn->addClickEventListener([=](Ref* sender) {
		this->removeFromParentAndCleanup(true);
	});
	//支付按钮
	auto pay_btn = (Button*)sprBg->getChildByName("pay_btn");
	pay_btn->setTouchEnabled(true);
	pay_btn->addClickEventListener([=](Ref* sender) {
		if (App::m_PayTest == 1)
			m_price = 1;
		long payCount = m_price;
		string info = App::getString("RECHARGE") + StringUtils::format("%.02f ", m_price / 100.0) + App::getString("YUAN");
		string paytype = "alipay";
		if (isPayWx)
			paytype = "weixinpay";
		App::log("支付启动");
		if(App::GetInstance()->m_me)
			NetIntface::httpPay(App::GetInstance()->m_me->id, payCount, m_price, paytype, info, "httpPaySuccess", [](string str) {
			YYXLayer::sendNotify("refershBalanceAndShowRedPacket");
			YYXLayer::sendNotify("CallBackPaySuccessGoToBuyBook");
			int index = YYXStruct::getMapInt64(App::GetInstance()->myData, "rechargeSuccessIndex", -999);
			if (index >= 0)
			{
				string key = StringUtils::format("RechargeIndex=%d", index);
				int count = YYXStruct::getMapInt64(App::GetInstance()->myData, key, -999);
				if (count > 0)
					XZLayer::showShareRedPacket(StringUtils::format("%d%s", count, App::getString("YUAN")));
			}
		}, "httpPayError", [](string error) {
			Toast::create(error.c_str());
		});
		pLayer->removeFromParentAndCleanup(true);
	});
	return true;
}

void Recharge::stringHttpRequest(HttpRequest::Type type, string url, map<string, string> paramter, int outTime, std::function<void(string json)> runable, std::function<void()> errorRunable)
{
	if (!&url || !&paramter)
	{
		CCLOGERROR("参数异常");
		return;
	}
	//if (!CocosAndroidJni::IsNetConnect())
	//{
	//	Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
	//		Toast::create(App::getString("NETEXCEPTION"));
	//	});
	//	return;
	//}
	if (!NetIntface::IsNetConnect(true))
		return;
	HttpRequest* request = new HttpRequest();
	request->setRequestType(type);
	request->setUrl(url.c_str());
	//CCLOG("%s", url.c_str());
	App::log(url);
	if (type == HttpRequest::Type::POST)
	{
		string postData;
		if (paramter.size() > 0)
		{
			for (auto it : paramter)
			{
				auto key = it.first;
				auto value = it.second;
				postData.append(StringUtils::format("&%s=%s", key.c_str(), value.c_str()));
			}
			postData.erase(0, 1);
			request->setRequestData(postData.c_str(), postData.length());
		}
		//CCLOG("%s", postData.c_str());
		App::log(postData);
	}
	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		if (!response)
		{
			if (errorRunable)
				errorRunable();
			return;
		}
		if (!response->isSucceed())
		{
			if (errorRunable)
				errorRunable();
			CCLOG("http failed \n code = %d \n error = %s", response->getResponseCode(), response->getErrorBuffer());
			return;
		}
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		if (runable)
			runable(str);
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	if(outTime>0)
		cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(outTime);
	request->release();
}

//获取到充值送红包的活动内容
void Recharge::httpRedPacketActivity()
{
	string url = string(IP) + NET_GETCHARGEACITIVITY;
	string runkey = "httpRedPacketActivitySuccess";
	NetIntface::httpGet(url, runkey, [](string json) {
		NetIntface::httpGetRechargeActivityCallBack(json, [](int index, int redPacketCount) {
			string redPacketActivityKey = StringUtils::format("RechargeIndex=%d", index);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, redPacketActivityKey, redPacketCount);
		}, []() {
			YYXLayer::sendNotify("refershPiaodai");
		}, []() {});
	}, "", [](string str) {});
}

//设置飘带的活动内容
void Recharge::setpiaodao(Node* node, int index ,int redPacketCount)
{
	node->setVisible(true);
	auto liang = (Sprite*)node->getChildByName("liang");
	auto an = (Sprite*)node->getChildByName("an");
	auto text = (Text*)node->getChildByName("Text");
	if (text)
	{
		if (redPacketCount > 0)
			text->setText(StringUtils::format("%s%d%s", App::getString("SONG"), redPacketCount, App::getString("YUAN")));
		else
			node->setVisible(false);
	}
	auto listener = EventListenerCustom::create("changePiaoDai", [=](EventCustom* e) {
		int i = (int)e->getUserData();
		if (index == i)
		{
			liang->setVisible(true);
			an->setVisible(false);
		}
		else
		{
			liang->setVisible(false);
			an->setVisible(true);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, node);
	auto refersh = EventListenerCustom::create("refershPiaodai", [=](EventCustom* e) {
		if (text)
		{
			string redKey = StringUtils::format("RechargeIndex=%d", index);
			int count = YYXStruct::getMapInt64(App::GetInstance()->myData, redKey, 0);
			if (count > 0)
			{
				text->setText(StringUtils::format("%s%d%s", App::getString("SONG"), count, App::getString("YUAN")));
				node->setVisible(true);
			}
			else
				node->setVisible(false);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(refersh, node);
}
