#include "Charger.h"

#define qq(d) App::log(#d);

void Charger::httpChargerInfo()
{
	string url = string(IP) + NET_GETCHARGEACITIVITY+"?type=2";
	NetIntface::httpPost(url, map<string, string>(),"", [](string json) {
		NetIntface::httpGetRechargeActivityCallBack(json, [&](int index, int hongbaoid, int hongbao) {
			//赠送红包列表
			string key = StringUtils::format("charger+index=%d", index);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, key, hongbao, StringUtils::format("%d", hongbaoid));
		}, [](int index, int charger) {
			//充值金额列表
			string key = StringUtils::format("charger+index=%d", index);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, key, -999, "", (Ref*)charger);
		}, []() {
			//解析结束的操作
			YYXLayer::sendNotify("setNodePrice", "", -1);
		}, []() {
			//解析异常的操作
		});
	}, "", [](string str) {
		//网络异常的操作
	});
}

void Charger::cleanup()
{
	qq(Charger::cleanup())
}

Charger* Charger::create()
{
	Charger *pRect = new Charger();
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

bool Charger::init()
{
	if (!Layer::init())
	{
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	httpChargerInfo();	
	Data data;
	string csb = RECHARGE_CSB;
	if (App::GetInstance()->getData(csb, data))
		pLayer = (Layer*)CSLoader::createNode(data);
	else
		pLayer = (Layer*)CSLoader::createNode(csb);
	pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	pLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(pLayer);
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

	setNodePrice(0);
	setNodePrice(1);
	setNodePrice(2);
	setNodePrice(3);
	setNodePrice(4);
	setNodePrice(5);

	//微信支付
	auto pay_weixin = (ImageView*)pLayer->getChildByName("Image_11");
	pay_weixin->setTouchEnabled(true);
	//支付宝支付
	auto pay_zhifubao = (ImageView*)pLayer->getChildByName("Image_12");
	pay_zhifubao->setTouchEnabled(true);

	pay_weixin->addClickEventListener([=](Ref* sender) {
		PayType(0);
		pay_weixin->loadTexture("Pay/res/ApplePay_weixin_sel_667h.png", TextureResType::PLIST);
		pay_zhifubao->loadTexture("Pay/res/ApplePay_zhifubao_667h.png", TextureResType::PLIST);
	});
	pay_zhifubao->addClickEventListener([=](Ref* sender) {
		PayType(1);
		pay_weixin->loadTexture("Pay/res/ApplePay_weixin_667h.png", TextureResType::PLIST);
		pay_zhifubao->loadTexture("Pay/res/ApplePay_zhifubao_sel_667h.png", TextureResType::PLIST);
	});
	//关闭按钮
	auto close_btn = (Button*)pLayer->getChildByName("close_btn");
	close_btn->setTouchEnabled(true);
	close_btn->addClickEventListener([=](Ref* sender) {
		this->removeFromParentAndCleanup(true);
	});

	//支付按钮
	auto pay_btn = (Button*)pLayer->getChildByName("pay_btn");
	pay_btn->setTouchEnabled(true);
	pay_btn->addClickEventListener([=](Ref* sender) {
		if (PayPrice() <=0)
		{
			Toast::create(App::getString("XUANZHECHONGZHIJINE"));
			return;
		}
		if (App::m_PayTest == 1)
			PayPrice(0.01);
		long payCount = PayPrice()*100;
		string info = App::getString("RECHARGE") + StringUtils::format("%.02f ", PayPrice() ) + App::getString("YUAN");
		string paytype = "";
		if (PayType() == 0)
			paytype = "weixinpay";
		if (PayType() == 1)
			paytype = "alipay";
		App::log(paytype+StringUtils::format(" paymoney = %f",PayPrice()*100));
		if (App::GetInstance()->m_me)
			NetIntface::httpPay(App::GetInstance()->m_me->id, payCount, PayPrice() * 100, paytype, info, "", [=](string str) {
			YYXLayer::sendNotify("refershBalanceAndShowRedPacket");
			YYXLayer::sendNotify("CallBackPaySuccessGoToBuyBook");
			auto value = YYXLayer::getFileValue("chargerSelectIndex", "0");
			auto money = getDatamoney(atoi(value.c_str()));
			auto count = getDatahongbao(atoi(value.c_str()));
			if (count > 0)
				XZLayer::showShareRedPacket(StringUtils::format("%d%s", count, App::getString("YUAN")));	
		}, "", [](string error) {
			Toast::create(error.c_str());
		});
		pLayer->removeFromParentAndCleanup(true);
	});	
	qq(Charger::init end)
	return true;
}

int Charger::getDatahongbao(int idx)
{
	App::log("Charger::getData", idx);
	string key = StringUtils::format("charger+index=%d", idx);
	int hongbao = YYXStruct::getMapInt64(App::GetInstance()->myData, key, 0);
	return hongbao;
}

int Charger::getDatamoney(int idx)
{
	App::log("Charger::getData", idx);
	string key = StringUtils::format("charger+index=%d", idx);
	int money = (int)YYXStruct::getMapRef(App::GetInstance()->myData, key, 0);
	return money;
}

Charger::Charger()
{
	qq(Charger::Charger())
	m_payPrice = 0;
	m_payType = 0;
	m_payIndex = 0;
}

Charger::~Charger()
{
	qq(Charger::~Charger())
}

void Charger::setNodePrice(int tag, bool sel)
{
	auto bt0 = pLayer->getChildByTag(tag);
	if (bt0 == nullptr)
		return;
	int idx = bt0->getTag();
	App::log("setNodePrice = ", idx);
	if (idx > 6 || idx < 0)
	{
		qq(idx > 6 || idx < 0)
			return;
	}
	qq(7777777777777)
	auto chongzhijine = (TextAtlas*)bt0->getChildByName("AtlasLabel_1");//充值金额
	auto selItem = (ImageView*)bt0->getChildByName("Image_1");//亮色选中活动飘带
	auto unSelItem = (ImageView*)bt0->getChildByName("Image_2");//暗色未选活动飘带
	auto texthongbao = (Text*)bt0->getChildByName("Text_1");//送多少钱红包
	auto text = (Text*)bt0->getChildByName("Text_2");//充值金额
	auto selSP = (Sprite*)bt0->getChildByName("sel");//选中打钩
	if(selSP)
		selSP->setVisible(sel);
	int money = getDatamoney(idx);
	auto hongbao = getDatahongbao(idx);
	if(chongzhijine)
		chongzhijine->setStringValue(StringUtils::format("%d", money));
	if (selItem == nullptr || unSelItem == nullptr || texthongbao== nullptr )
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
		texthongbao->setText(StringUtils::format("%s%d%s", App::getString("SONG"), hongbao, App::getString("YUAN")));
	}
	
	if(text)
		text->setText(StringUtils::format("%s%d%s", App::getString("RENMINBI"), money, App::getString("YUAN")));

	auto node = (ImageView*)bt0->getChildByName("Image_58");
	if (node == nullptr)
		return;
	node->setTouchEnabled(true);
	node->addClickEventListener([=](Ref* sneder) {
		auto idx = bt0->getTag();
		App:log("node->addClickEventListener", idx);
		if (idx > 6 || idx < 0)
			return;
		PayIndex(idx);
		PayPrice(getDatamoney(idx));
		setNodePrice(idx, true);
		YYXLayer::sendNotify("setNodePrice", "222222", idx);
	});
	_eventDispatcher->removeEventListenersForTarget(bt0);
	auto listener = EventListenerCustom::create("setNodePrice", [=](EventCustom* e) {
		int getIndex = (int)e->getUserData();
		if(getIndex != bt0->getTag())
			setNodePrice(bt0->getTag());
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, bt0);
	App::log("Charger:: setNodePrice END ");
}
