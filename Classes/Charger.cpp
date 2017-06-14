#include "Charger.h"
#include "YYXSound.h"
#include "AppHttp.h"
#include "HttpWaiting.h"
#include "XZLayer.h"

#define qq(d) App::log(#d);

Charger* Charger::instance = nullptr;

Charger* Charger::getInstance()
{
	if (instance == nullptr)
	{
		instance = new Charger();
	}
	return instance;
}


void Charger::show()
{
	if (haveData())
	{
		auto lay = newChargeLayer();
		if(lay)
			Director::getInstance()->getRunningScene()->addChild(lay);
	}
	else
	{
		httpCallChargeLayer();
	}
}

void Charger::addData(ChargeData* data)
{
	if (data)
	{
		int idx = data->getIndex();
		auto it = m_data.find(idx);
		if (it != m_data.end())
		{
			auto olddata = it->second;
			if (olddata && olddata != data)
			{
				ChargeData::del(olddata);
			}
		}
		m_data[idx] = data;
	}
}

ChargeData* Charger::getData(int idx)
{
	ChargeData* data = nullptr;
	auto it = m_data.find(idx);
	if (it != m_data.end())
	{
		data = it->second;
	}
	return data;
}

bool Charger::haveData()
{
	auto count = m_data.size();
	bool show = false;
	if (count == 6)
	{
		show = true;
	}
	return show;
}
cocos2d::Layer* Charger::newChargeLayer()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Data data;
	string csb = RECHARGE_CSB;
	if (App::GetInstance()->getData(csb, data))
		pLayer = (Layer*)CSLoader::createNode(data);
	else
		pLayer = (Layer*)CSLoader::createNode(csb);
	pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	//触摸屏蔽层
	auto bg_ImgView = (ImageView*)pLayer->getChildByName("bg_Imgview");
	bg_ImgView->setTouchEnabled(true);

	//选中图片
	auto sprSelected = (Sprite*)pLayer->getChildByName("spr_select");
	//6个按钮
	auto bt0 = pLayer->getChildByName("FileNode_0");
	auto bt1 = pLayer->getChildByName("FileNode_1");
	auto bt2 = pLayer->getChildByName("FileNode_2");
	auto bt3 = pLayer->getChildByName("FileNode_3");
	auto bt4 = pLayer->getChildByName("FileNode_4");
	auto bt5 = pLayer->getChildByName("FileNode_5");
	vector<Node*> nods;
	nods.push_back(bt0);
	nods.push_back(bt1);
	nods.push_back(bt2);
	nods.push_back(bt3);
	nods.push_back(bt4);
	nods.push_back(bt5);
	for (int i = 0; i < 6; i++)
	{
		auto node = nods[i];
		auto data = m_data[i];
		setNode(node, data);
	}


	//微信支付
	auto pay_weixin = (ImageView*)pLayer->getChildByName("Image_11");
	pay_weixin->setTouchEnabled(true);
	//支付宝支付
	auto pay_zhifubao = (ImageView*)pLayer->getChildByName("Image_12");
	pay_zhifubao->setTouchEnabled(true);

	if (getPayPlatform() == WEIXIN)
	{
		pay_weixin->loadTexture("Pay/res/ApplePay_weixin_sel_667h.png", TextureResType::PLIST);
		pay_zhifubao->loadTexture("Pay/res/ApplePay_zhifubao_667h.png", TextureResType::PLIST);
	}
	else if (getPayPlatform() == ZHIFUBAO)
	{
		pay_weixin->loadTexture("Pay/res/ApplePay_weixin_667h.png", TextureResType::PLIST);
		pay_zhifubao->loadTexture("Pay/res/ApplePay_zhifubao_sel_667h.png", TextureResType::PLIST);
	}

	pay_weixin->addClickEventListener([=](Ref* sender) {
		YYXSound::getInstance()->playButtonSound();
		pay_weixin->loadTexture("Pay/res/ApplePay_weixin_sel_667h.png", TextureResType::PLIST);
		pay_zhifubao->loadTexture("Pay/res/ApplePay_zhifubao_667h.png", TextureResType::PLIST);
		setPayPlatform(WEIXIN);
	});
	pay_zhifubao->addClickEventListener([=](Ref* sender) {
		YYXSound::getInstance()->playButtonSound();
		pay_weixin->loadTexture("Pay/res/ApplePay_weixin_667h.png", TextureResType::PLIST);
		pay_zhifubao->loadTexture("Pay/res/ApplePay_zhifubao_sel_667h.png", TextureResType::PLIST);
		setPayPlatform(ZHIFUBAO);
	});
	//关闭按钮
	auto close_btn = (Button*)pLayer->getChildByName("close_btn");
	close_btn->setTouchEnabled(true);
	close_btn->addClickEventListener([=](Ref* sender) {
		YYXSound::getInstance()->playButtonSound();
		pLayer->removeFromParentAndCleanup(true);
		pLayer = nullptr;
	});

	//支付按钮
	auto pay_btn = (Button*)pLayer->getChildByName("pay_btn");
	pay_btn->setTouchEnabled(true);
	pay_btn->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "zhifuTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			pLayer->removeFromParentAndCleanup(true);
			pLayer = nullptr;
			auto con = Charger::getInstance();
			int idx = con->getIndex();//点击位置
			auto data = con->getData(idx);
			if (data == nullptr)
			{
				Toast::create(App::getString("XUANZHECHONGZHIJINE"));
				return;
			}
			int _payObjectId = data->getPayId();
			int payFlatform = con->getPayPlatform();
			string payFlatform_Str = "";
			if (payFlatform == WEIXIN)
				payFlatform_Str = "wxpay";
			if (payFlatform == ZHIFUBAO)
				payFlatform_Str = "alipay";

			CrossPlatform::getInstance()->pay(App::getMemberId(), _payObjectId, "recharge", payFlatform_Str, [](string json) {
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([]() {
					YYXLayer::sendNotify("refershBalanceAndShowRedPacket");
					YYXLayer::sendNotify("CallBackPaySuccessGoToBuyBook");
					CallBackFunction callback = Charger::getInstance()->getCallback();
					if (callback)
						callback();
					auto con = Charger::getInstance();
					int idx = con->getIndex();//点击位置
					auto data = con->getData(idx);
					if (data)
					{
						int count = data->getHongbao();
						if (count > 0)
							XZLayer::safe_ShowShareRedPacket(StringUtils::format("%d%s", count/100, App::getString("YUAN")));
					}
				});
			}, [](string json) {
				string error = "";
				rapidjson::Document doc;
				bool result = YYXLayer::getJsonObject4Json(doc, json);
				if (result)
					error = YYXLayer::getStringForJson("", doc, "error");
				if (error.empty())
					error = App::getString("QUXIAOCAOZUO");
				Toast::create(error.c_str());
			});
		});
	});	
	return pLayer;
}


void Charger::httpCallChargeLayer()
{
	AppHttp::getInstance()->httpChargerInfo(1009);
	auto wlayer = HttpWaiting::getInstance()->newWaitingLayer(1009,[]() {
		auto layer = Charger::getInstance()->newChargeLayer();
		if (layer)
		{
			Director::getInstance()->getRunningScene()->addChild(layer);
		}
	});
	if (wlayer)
	{
		Director::getInstance()->getRunningScene()->addChild(wlayer);
	}
}

Charger::Charger()
{
}

Charger::~Charger()
{
}

void Charger::setNode(Node* bt0, ChargeData* data)
{
	if (bt0 == nullptr || data == nullptr)
		return;
	bool sel = false;
	if (data->getIndex() == getIndex())
		sel = true;
	auto chongzhijine = (TextAtlas*)bt0->getChildByName("AtlasLabel_1");//充值金额
	auto selItem = (ImageView*)bt0->getChildByName("Image_1");//亮色选中活动飘带
	auto unSelItem = (ImageView*)bt0->getChildByName("Image_2");//暗色未选活动飘带
	auto texthongbao = (Text*)bt0->getChildByName("Text_1");//送多少钱红包
	auto text = (Text*)bt0->getChildByName("Text_2");//充值金额
	auto selSP = (Sprite*)bt0->getChildByName("sel");//选中打钩
	if (selSP)
		selSP->setVisible(sel);
	int money = data->getPrice();
	auto hongbao = data->getHongbao();
	if (chongzhijine)
		chongzhijine->setStringValue(StringUtils::format("%d", money / 100));
	if (selItem == nullptr || unSelItem == nullptr || texthongbao == nullptr)
		return;
	if (hongbao <= 0)
	{
		selItem->setVisible(false);
		unSelItem->setVisible(false);
		texthongbao->setVisible(false);
	}
	else
	{
		selItem->setVisible(true);
		unSelItem->setVisible(!sel);
		texthongbao->setVisible(true);
		texthongbao->setText(StringUtils::format("%s%d%s", App::getString("SONG"), hongbao / 100, App::getString("YUAN")));
	}

	if (text)
		text->setText(StringUtils::format("%s%d%s", App::getString("RENMINBI"), money / 100, App::getString("YUAN")));

	auto node = (ImageView*)bt0->getChildByName("Image_58");
	if (node == nullptr)
		return;
	node->setTouchEnabled(true);
	node->addClickEventListener([=](Ref* sneder) {
		YYXSound::getInstance()->playButtonSound();
		YYXLayer::sendNotify(TAG_ChargerSelectNode);
		setIndex(data->getIndex());
	});
	_eventDispatcher->removeEventListenersForTarget(bt0);
	auto listener = EventListenerCustom::create(TAG_ChargerSelectNode, [=](EventCustom* e) {
		setNode(bt0, data);
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, bt0);
}

ChargeData::ChargeData()
{

}

ChargeData::~ChargeData()
{

}

ChargeData* ChargeData::create()
{
	return new ChargeData();
}

void ChargeData::del(ChargeData* data)
{
	if (data)
	{
		delete data;
	}
	data = nullptr;
}
