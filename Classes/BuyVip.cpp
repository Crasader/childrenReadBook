#include "BuyVip.h"
#include <string>
#include "App.h"
#include "YYXTime.h"
#include "YYXVisitor.h"
#include "Charger.h"
#include "AppHttp.h"
#include "HttpWaiting.h"
using namespace std;

BuyVip* BuyVip::instance = nullptr;

BuyVip* BuyVip::getInstance()
{
	if(instance == nullptr)
		instance = new BuyVip();
	return instance;
}

void BuyVip::destoryInstance()
{
	if (instance)
	{
		for (auto it : instance->m_dat)
		{
			if (it)
			{
				VipTypeData::del(it);
			}
		}
		instance->m_dat.clear();
		delete instance;
	}
	instance = nullptr;
}

Layer* BuyVip::show()
{
	Layer* layer = nullptr;
	m_callback = nullptr;
	m_callbackerror = nullptr;
	if (BuyVip::getInstance()->getDataSize() > 0)
	{
		BuyVip::getInstance()->controlShow();
	}
	else
	{
		AppHttp::getInstance()->httpVipHint_VipType(App::getMemberId(), 1100);
		layer = HttpWaiting::getInstance()->newWaitingLayer(1100, []() {
			BuyVip::getInstance()->controlShow();
		});
	}
	return layer;
}

Layer* BuyVip::BuyVip7DayLayer()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Data data;
	string csb = "Notification/csb/kaitonghuiyuan7day.csb";
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
	auto bg = (ImageView*)pLayer->getChildByName("bg");
	bg->setTouchEnabled(true);
	//3个卡片
	auto node1 = pLayer->getChildByName("FileNode_1");
	auto node2 = pLayer->getChildByName("FileNode_1_0");
	auto node3 = pLayer->getChildByName("FileNode_1_1");
	vector<Node*> nodes;
	nodes.push_back(node1);
	nodes.push_back(node2);
	nodes.push_back(node3);
	for (int i = 0; i < 3; i++)
	{
		int count = m_dat.size();
		auto node = nodes[i];
		if (i < count)
		{
			auto dat = m_dat[i];
			initNode(node, dat);
		}
		else
			initNode(node, nullptr);
	}
	addAnimation(pLayer);
	return pLayer;
}

Layer* BuyVip::BuyVipLayer()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Data data;
	string csb = "Notification/csb/kaitonghuiyuan.csb";
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
	auto bg = (ImageView*)pLayer->getChildByName("bg");
	bg->setTouchEnabled(true);
	auto node1 = pLayer->getChildByName("FileNode_1_0");
	auto node2 = pLayer->getChildByName("FileNode_1_1");
	addAnimation(pLayer);
	initNode(node1,nullptr);
	initNode(node2, nullptr);
	vector<Node*> nodes;
	nodes.push_back(node1);
	nodes.push_back(node2);
	for (int i = 0; i < 2; i++)
	{
		int count = m_dat.size();
		auto node = nodes[i];
		if (i < count)
		{
			auto dat = m_dat[i];
			initNode(node, dat);
		}
		else
			initNode(node, nullptr);
	}
	return pLayer;
}

void BuyVip::controlShow()
{
	Layer* layer = nullptr;
	switch (m_dat.size())
	{
	case 3:
		layer = BuyVip7DayLayer();
		break;
	case 2:
		layer = BuyVipLayer();
		break;
	default:
		break;
	}
	if (layer)
		Director::getInstance()->getRunningScene()->addChild(layer);
}

Layer* BuyVip::payLayer()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Data data;
	string csb = "Notification/csb/pay.csb";
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
		BuyVip::destoryInstance();
	});
	auto bg = (ImageView*)pLayer->getChildByName("bg");
	bg->setTouchEnabled(true);
	auto money = (Text*)pLayer->getChildByName("Text_23_0");
	money->setText(StringUtils::format("%s%.02f", App::getString("RENMINGBI"), m_money / 100.0));
	auto pay = (Button*)pLayer->getChildByName("Button_3");
	pay->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "BuyVipButtonTime",[=]() {
			pLayer->removeFromParentAndCleanup(true);
			//支付
			if (m_payType == MyMoney)
			{
				int waitkey = YYXTime::getInstance()->getIntNumber();
				AppHttp::getInstance()->httpMyMoneyPayVip(App::getMemberId(), m_VipType, waitkey);
				auto waitlayer = HttpWaiting::getInstance()->newWaitingLayer(waitkey, []() {
					auto callback = BuyVip::getInstance()->getCallback();
					if (callback)
						callback("");
					BuyVip::destoryInstance();
				});
				if(waitlayer)
					Director::getInstance()->getRunningScene()->addChild(waitlayer);
			}
			else
			{
				string paytype = "alipay";
				if (m_payType == WeixinPay)
					paytype = "wxpay";
				CrossPlatform::getInstance()->pay(App::getMemberId(), m_VipType, "card", paytype, [paytype](string json) {
					if (paytype == "alipay")
					{
						map<string, string> data;
						YYXLayer::TraversingJson(json, data);
						data["paytype"] = paytype;
						YYXLayer::setFileValue("payinfo", YYXLayer::getStringFormMap(data));
					}
					//续费成功
					Toast::create(App::getString("GOUMAIHUIYUANCHENGGONG"));
					BuyVip::destoryInstance();
					AppHttp::getInstance()->httpCheckVIP([]() {
						YYXLayer::sendNotify("refershMemberIDVIP");//提示刷新父母设置界面
					});
					vector<string> da = { "VIP_WEEK1","VIP_WEEK2","VIP_WEEK3","VIP_DAY1" ,"VIP_DAY2" ,"VIP_DAY3" ,"VIP_DAY4" ,"VIP_DAY5" ,"VIP_DAY6" ,"VIP_DAY7" };
					for (auto it : da)
					{
						YYXLayer::deleteFileValue(it);
					}
					auto callback = BuyVip::getInstance()->getCallback();
					if (callback)
						callback(json);
				}, [](string json) {
					string error = "";
					rapidjson::Document doc;
					bool result = YYXLayer::getJsonObject4Json(doc, json);
					if (result)
						error = YYXLayer::getStringForJson("", doc, "error");
					if (error.empty())
						error = App::getString("GOUMAIHUIYUANSHIBAI");
					Toast::create(error.c_str());
					auto callback = BuyVip::getInstance()->getCallbackerror();
					if (callback)
						callback(json);
				});
			}
		});
	});
	if (User::getInstance()->getMoney() < m_money)
	{
		m_payType = WeixinPay;
	}

	auto node1 = pLayer->getChildByName("FileNode_1");
	initPayNode_MyMoney(node1);
	auto node2 = pLayer->getChildByName("FileNode_2");
	initPayNode_otherPay(node2, AliPay);
	auto node3 = pLayer->getChildByName("FileNode_3");
	initPayNode_otherPay(node3, WeixinPay);
	auto listener = EventListenerCustom::create(TAG_REMOVEBUYVIPPAY, [=](EventCustom* e) {
		pLayer->removeFromParentAndCleanup(true);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, pLayer);
	return pLayer;
}

void BuyVip::clear()
{
	for (auto it : m_dat)
	{
		if (it)
		{
			VipTypeData::del(it);
		}
		it = nullptr;
	}
	m_dat.clear();
}

void BuyVip::addData(VipTypeData* data)
{
	if (data)
		m_dat.push_back(data);
}

void BuyVip::initPayNode_MyMoney(Node* node)
{
	auto dagou = (ImageView*)node->getChildByName("Image_3");
	auto myMoney = (Text*)node->getChildByName("Text_2");
	auto yuebuzu = (Text*)node->getChildByName("Text_3");
	auto chongzhi = (Button*)node->getChildByName("Button_1");
	auto kuang = (ImageView*)node->getChildByName("Image_1");
	kuang->setTouchEnabled(true);
	kuang->addClickEventListener([=](Ref* sender) {
		if (User::getInstance()->getMoney() >= m_money)
		{
			m_payType = MyMoney;
			YYXLayer::sendNotify(TAG_BUYVIPSELECTPAYTYPE);
		}
		else
		{
			Toast::create(App::getString("YUEBUZU"));
		}
	});
	if (m_payType == MyMoney)
	{
		dagou->loadTexture("other/yilahuiyuan_zhifufangshi_xuanzhong_736h.png");
		kuang->loadTexture("other/yilahuiyuan_zhifufangshi_waikuang_pre_736h.png");
	}
	else
	{
		dagou->loadTexture("other/yilahuiyuan_zhifufangshi_weixuanzhong_736h.png");
		kuang->loadTexture("other/yilahuiyuan_zhifufangshi_waikuang_nor_736h.png");
	}
	auto listener = EventListenerCustom::create(TAG_BUYVIPSELECTPAYTYPE, [=](EventCustom* e) {
		if (m_payType == MyMoney)
		{
			dagou->loadTexture("other/yilahuiyuan_zhifufangshi_xuanzhong_736h.png");
			kuang->loadTexture("other/yilahuiyuan_zhifufangshi_waikuang_pre_736h.png");
		}
		else
		{
			dagou->loadTexture("other/yilahuiyuan_zhifufangshi_weixuanzhong_736h.png");
			kuang->loadTexture("other/yilahuiyuan_zhifufangshi_waikuang_nor_736h.png");
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, dagou);
	myMoney->setText(StringUtils::format("%.02f%s", User::getInstance()->getMoney() / 100.0, App::getString("YUAN")));
	if (User::getInstance()->getMoney() < m_money)
	{
		yuebuzu->setVisible(true);
	}
	else
		yuebuzu->setVisible(false);
	chongzhi->addClickEventListener([](Ref* sender) {
		YYXLayer::controlTouchTime(1, "chongzhiButtonTime", [=]() {
			YYXLayer::sendNotify(TAG_REMOVEBUYVIPPAY);
			Charger::getInstance()->show();
			Charger::getInstance()->setCallback([]() {
				auto con = BuyVip::getInstance();
				auto call1 = con->getCallback();
				auto call2 = con->getCallbackerror();
				auto layer = con->show();
				con->setCallback(call1);
				con->setCallbackerror(call2);
				if(layer)
					Director::getInstance()->getRunningScene()->addChild(layer);
			});
		});
	});
}

void BuyVip::initPayNode_otherPay(Node* node, payType types)
{
	auto dagou = (ImageView*)node->getChildByName("Image_3");
	auto typeName = (Text*)node->getChildByName("Text_1");
	auto tuBiao = (ImageView*)node->getChildByName("Image_2");
	auto kuang = (ImageView*)node->getChildByName("Image_1");
	kuang->setTouchEnabled(true);
	kuang->addClickEventListener([=](Ref* sender) {
		m_payType = types;
		YYXLayer::sendNotify(TAG_BUYVIPSELECTPAYTYPE);
	});
	if (m_payType == types)
	{
		dagou->loadTexture("other/yilahuiyuan_zhifufangshi_xuanzhong_736h.png");
		kuang->loadTexture("other/yilahuiyuan_zhifufangshi_waikuang_pre_736h.png");
	}
	else
	{
		dagou->loadTexture("other/yilahuiyuan_zhifufangshi_weixuanzhong_736h.png");
		kuang->loadTexture("other/yilahuiyuan_zhifufangshi_waikuang_nor_736h.png");
	}
	auto listener = EventListenerCustom::create(TAG_BUYVIPSELECTPAYTYPE, [=](EventCustom* e) {
		if (m_payType == types)
		{
			dagou->loadTexture("other/yilahuiyuan_zhifufangshi_xuanzhong_736h.png");
			kuang->loadTexture("other/yilahuiyuan_zhifufangshi_waikuang_pre_736h.png");
		}
		else
		{
			dagou->loadTexture("other/yilahuiyuan_zhifufangshi_weixuanzhong_736h.png");
			kuang->loadTexture("other/yilahuiyuan_zhifufangshi_waikuang_nor_736h.png");
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, dagou);
	if (types == AliPay)
	{
		tuBiao->loadTexture("other / yilahuiyuan_icon_zhifubao_736h.png");
		typeName->setText(App::getString("ZHIFUBAOZHIFU"));
	}
	else if (types == WeixinPay)
	{
		tuBiao->loadTexture("other/yilahuiyuan_icon_weixin_736h.png");
		typeName->setText(App::getString("WEIXINZHIFU"));
	}
}

BuyVip::BuyVip()
{
}

BuyVip::~BuyVip()
{
	for (auto it : m_dat)
	{
		VipTypeData::del(it);
	}
}

void BuyVip::initNode(Node* node, VipTypeData* data)
{
	if (node == nullptr)
		return;
	if (data == nullptr)
	{
		node->setVisible(false);
		return;
	}
	else
		node->setVisible(true);
	auto bg = (ImageView*)node->getChildByName("bg");
	auto kuang = (ImageView*)node->getChildByName("kuang");
	auto biaoqian = (ImageView*)node->getChildByName("Image_1");
	auto name = (Text*)node->getChildByName("Text_1");
	auto time = (Text*)node->getChildByName("Text_2");
	auto price = (Text*)node->getChildByName("Text_3");
	auto xiangou = (Text*)node->getChildByName("Text_4");
	if (bg)
		bg->setVisible(false);
	if (kuang) {
		kuang->setVisible(true);
		kuang->setTouchEnabled(true);
		kuang->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "selecetPayButtonTime", [=]() {
				bg->setVisible(true);
				m_VipType = data->getCardType();
				m_money = data->getPrice();
				YYXLayer::sendNotify(TAG_REMOVEBUYVIP);
				//去选择支付方式
				auto layer = payLayer();
				if (layer)
				{
					Director::getInstance()->getRunningScene()->addChild(layer);
					layer->setScale(0.5);
					layer->runAction(Sequence::create(DelayTime::create(0.3f), ScaleTo::create(0.3f, 1), NULL));
				}
			});
		});
	}
	auto biaoq = data->getBiaoqian();
	switch (biaoq)
	{
	case _7Day:
		biaoqian->setVisible(true);
		biaoqian->loadTexture("other/yilahuiyuan_chaozhitiyan_736h.png");
		break;
	case _30Day:
		biaoqian->setVisible(false);
		break;
	case _365Day:
		biaoqian->setVisible(true);
		biaoqian->loadTexture("other/yilahuiyuan_tehui_736h.png");
		break;
	}
	name->setText(data->getName());
	time->setText(data->getTimeName());	
	price->setText(StringUtils::format("%s%.02f", App::getString("RENMINGBI"), data->getPrice() / 100.0));
	if (xiangou)
	{
		if (data->getBuycount() == 1)
			xiangou->setVisible(true);
		else
			xiangou->setVisible(false);
	}
}

void BuyVip::addAnimation(Layer* layer)
{
	auto listener = EventListenerCustom::create(TAG_REMOVEBUYVIP, [=](EventCustom* e) {
		layer->runAction(Sequence::create(ScaleTo::create(0.5f, 0), CallFuncN::create([=](Ref* sender) {
			layer->removeFromParentAndCleanup(true);
		}), NULL));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, layer);
}

VipTypeData* VipTypeData::create()
{
	return new VipTypeData();
}

void VipTypeData::del(VipTypeData* data)
{
	if (data)
		delete data;
	data = nullptr;
}

VipTypeData::VipTypeData()
{

}

VipTypeData::~VipTypeData()
{

}
