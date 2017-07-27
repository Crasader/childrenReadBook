#include "YYXBuyBook.h"
#include "YYXVisitor.h"
#include "Charger.h"
#include "MyBook.h"
#include "User.h"
#include "AppHttp.h"
#include "YYXTime.h"
#include "HttpWaiting.h"

YYXBuyBook* YYXBuyBook::instance = nullptr;

void YYXBuyBook::buyBook(const function<void(int)>& buyBookCallBack, const function<void()>& goToLoginCallBack)
{
	m_buyBookSuccessCallBack = [=](int bookid) {
		MyBook::getInstance()->addBook(bookid, YYXTime::getInstance()->getNowTime4S());
		if (buyBookCallBack)
			buyBookCallBack(bookid);
	};
	setOutScene(goToLoginCallBack);
	httpMoney();
	httpRedPacket();
	httpBookInfo();//获取书籍信息成功后 会回调购书的第一个界面
}

void YYXBuyBook::setOutScene(const function<void()>& goToLoginCallBack /*= nullptr*/)
{
	m_outSceneCallBack = goToLoginCallBack;
}

void YYXBuyBook::httpMoney()
{
	AppHttp::getInstance()->httpUserBalance([=](int userBalance) {
		m_myMoney = userBalance;
		YYXLayer::sendNotify(TAG1);
	});
}

void YYXBuyBook::httpRedPacket()
{
	AppHttp::getInstance()->httpUserRedPackets([]() {
		YYXLayer::sendNotify(TAG2);
	});
}

void YYXBuyBook::httpBookInfo()
{
	AppHttp::getInstance()->httpBookInfo(m_bookId, [=](string name) {
		m_bookName = name;
		YYXLayer::sendNotify(TAG_REMOVEWAITING, "", 2001);
	});	
	auto wlayer = HttpWaiting::getInstance()->newWaitingLayer(2001, []() {
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([]() {
			YYXBuyBook::GetInstance()->buyBookFirst();
		});
	});
	if (wlayer)
	{
		Director::getInstance()->getRunningScene()->addChild(wlayer);
	}
}

void YYXBuyBook::httpBuyBook()
{
	if (m_hongbao <= 0)
	{//无红包购书
		AppHttp::getInstance()->httpBuyBookWithOutRedPacket(m_bookId, m_buyBookSuccessCallBack, [=]() {
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
				feeCharging();
			});
		});
	}
	else
	{//有红包购书
		AppHttp::getInstance()->httpBuyBookWithRedPacket(m_hongbaoId, m_bookId, m_buyBookSuccessCallBack);
	}
}

Layer* YYXBuyBook::selectRedPacke()
{
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "selectRedPacket");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_SELECTREDPACKET_CSB);
	auto redPacketLayer = YYXLayer::create(paramter);

	auto myredpacket = (ImageView*)redPacketLayer->findControl(FIND_IMAGEVIEW_BY_NAME_SELECTREDPACKET_MYREDPACKET);
	auto backbutton = (ImageView*)redPacketLayer->findControl(FIND_IMAGEVIEW_BY_NAME_SELECTREDPACKET_BACK);
	//有N个红包到期
	auto daoqi = (Text*)redPacketLayer->findControl(FIND_TEXT_BY_NAME_SELECTREDPACKET_DAOQI);
	if (daoqi)
	{
		int expiring_coupon_count = YYXStruct::getMapInt64(App::GetInstance()->myData, "expiring_coupon_count", 0);
		daoqi->setText(StringUtils::format("%d%s", expiring_coupon_count, App::getString("NGEHONGBAOJIJIANGDAOQI")));
	}
	//刷新红包按钮
	auto refer = (Text*)redPacketLayer->findControl(FIND_TEXT_BY_NAME_SELECTREDPACKET_REFER);
	auto listview = (ListView*)redPacketLayer->findControl(FIND_LISTVIEW_BY_NAME_SELECTREDPACKET_REDPACKET);
	listview->setScrollBarAutoHideEnabled(false);
	listview->setScrollBarEnabled(true);
	//不使用红包
	auto without = (ImageView*)redPacketLayer->findControl(FIND_IMAGEVIEW_BY_NAME_SELECTREDPACKET_DONOTWITHREDPACKET);
	//红包说明
	auto message = (ImageView*)redPacketLayer->findControl("Image_5");
	if (message)
	{
		message->setTouchEnabled(true);
		message->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneREDPACKETMESSAGETime", [=]() {
				redPacketLayer->removeFromParentAndCleanup(true);
				map<string, int64String> paramter;
				YYXLayer::insertMap4ParaType(paramter, "className", -999, "BookInfo");
				YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_REDPACKETMESSAGE_CSB);
				auto messageLayer = YYXLayer::create(paramter);
				auto mypacket = (ImageView*)messageLayer->findControl(FIND_IMAGEVIEW_BY_NAME_REDPACKETMESSAGE_MYPACKET);
				auto backbutton = (ImageView*)messageLayer->findControl(FIND_IMAGEVIEW_BY_NAME_REDPACKETMESSAGE_BACK);
				auto listeners = [=](Ref* sender) {
					messageLayer->removeFromParentAndCleanup(true);
					selectRedPacke();
				};
				mypacket->setTouchEnabled(true);
				mypacket->addClickEventListener(listeners);
				backbutton->setTouchEnabled(true);
				backbutton->addClickEventListener(listeners);
				messageLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
				Size visibleSize = Director::getInstance()->getVisibleSize();
				messageLayer->setPosition(visibleSize / 2);
				Director::getInstance()->getRunningScene()->addChild(messageLayer);
			});
		});
	}
	//返回上级
	auto listerfunction = [=](Ref* sneder) {
		redPacketLayer->removeFromParentAndCleanup(true);
		buyBookFirst();
	};
	if (myredpacket && backbutton)
	{
		backbutton->setTouchEnabled(true);
		myredpacket->setTouchEnabled(true);
		backbutton->addClickEventListener(listerfunction);
		myredpacket->addClickEventListener(listerfunction);
	}
	if (refer)
	{
		refer->setTouchEnabled(true);
		refer->addClickEventListener([=](Ref* send) {
			YYXLayer::controlTouchTime(1, "bookInfoSceneSelectRedPacketReferRedPacketListviewTime", [=]() {
				httpRedPacket();
			});
		});
	}
	redPacketLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	redPacketLayer->setPosition(Director::getInstance()->getVisibleSize() / 2);
	if (without)
	{
		if (m_hongbao <= 0)
		{
			without->setTag(0);//不使用红包状态
			without->loadTexture(IMAGE_DONOTWITHREDPACKET, TextureResType::PLIST);
		}
		else
		{
			without->setTag(1);
		}
		without->setTouchEnabled(true);
		without->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "bookInfoSceneSelectRedPacketBUSHIYONGHONGBAOTime", [=]() {
				if (without->getTag() == 1)
				{
					//0不使用红包
					without->setTag(0);
					without->loadTexture(IMAGE_DONOTWITHREDPACKET, TextureResType::PLIST);
					m_hongbao = 0;
					m_hongbaoId = 0;
					m_manualSelectHongbao = true;
					YYXLayer::sendNotify("selectOver");
				}
				else
				{
					//1使用红包
					without->setTag(1);
					getOptimalRedPacket();
					YYXLayer::sendNotify("selectOver");
					m_manualSelectHongbao = true;
					without->loadTexture(IMAGE_WITHREDPACKET, TextureResType::PLIST);
				}
			});
		});
	}
	auto eventListene = EventListenerCustom::create(TAG2, [=](EventCustom* e) {
		//展示红包
		listViewShowRedPacket(listview);
		Toast::create(App::getString("SHUAXINHONGBAO"));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListene, redPacketLayer);
	listViewShowRedPacket(listview);
	//选择完成红包,通知返回上级
	auto evenet = EventListenerCustom::create("selectOver", [=](EventCustom* e) {
		Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(redPacketLayer, true);
		redPacketLayer->runAction(Sequence::create(ScaleTo::create(0.3, 0), CallFuncN::create([=](Ref* d) {
			redPacketLayer->removeFromParentAndCleanup(true);
			auto layer = buyBookFirst();
			if (layer)
			{
				layer->setScale(0);
				layer->runAction(ScaleTo::create(0.3, 1));
			}
		}), NULL));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(evenet, redPacketLayer);
	Director::getInstance()->getRunningScene()->addChild(redPacketLayer);
	return redPacketLayer;
}

Layer* YYXBuyBook::buyBookFirst()
{
	if (!m_show)
		return nullptr;
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "showpay");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_WHETHERTOPAY_CSB);
	auto payLayer = YYXLayer::create(paramter);
	auto gotoPay = (Button*)payLayer->findControl("Button_1");
	auto balance = (Text*)payLayer->findControl("Text_1");
	auto bookname = (Text*)payLayer->findControl("Text_6");
	auto bookprice = (Text*)payLayer->findControl("Text_6_0");
	auto close = (Button*)payLayer->findControl("close");
	auto hongbaoshow = (Text*)payLayer->findControl("Text_6_1");
	auto paypricetext = (Text*)payLayer->findControl("Text_6_2");
	auto selcetRedPacket1 = (ImageView*)payLayer->findControl("Image_3");
	auto selcetRedPacket2 = (ImageView*)payLayer->findControl("Image_2");
	auto listenfunction = [=](Ref* sender) {
		payLayer->removeFromParentAndCleanup(true);
		//打开选择红包界面
		selectRedPacke();
		m_show = true;
	};
	if (selcetRedPacket1 && selcetRedPacket2)
	{
		selcetRedPacket1->setTouchEnabled(true);
		selcetRedPacket2->setTouchEnabled(true);
		selcetRedPacket1->addClickEventListener(listenfunction);
		selcetRedPacket2->addClickEventListener(listenfunction);
	}
	if (bookname)
	{
		if (m_bookName.length() >25 )
		{
			bookname->setFontSize(bookname->getFontSize() - 8);
		}
		bookname->setText(m_bookName);
	}
	if (bookprice)
		bookprice->setText(StringUtils::format("%.02f", m_bookPrice / 100.0) + App::getString("YUAN"));
	
	if (close)
	{
		close->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneclose", [=]() {
				payLayer->removeFromParentAndCleanup(true);
				m_show = true;
			});
		});
	}
	if (gotoPay)
	{
		gotoPay->setTag(-999);
		gotoPay->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneclose", [=]() {
				payLayer->removeFromParentAndCleanup(true);
				if (gotoPay->getTag() == 0)//红包购书
					buyBookSecond();
				if (gotoPay->getTag() == 1)//无红包购书
					buyBookSecond();
				if (gotoPay->getTag() == 2)//去充值
					feeCharging();
				m_show = true;
			});
		});
	}
	//1.红包展示
	//2.实付
	//3.充值或者购买按钮
	int payPrice100 = 0;//实付价格
	bool withRedPacket = false;
	if (App::GetInstance()->m_redPacket.size() == 0)
	{
		if (hongbaoshow)
			hongbaoshow->setText(App::getString("MEIYOUKEYONGHONGBAO"));
		payPrice100 = m_bookPrice;
		if (paypricetext)
			paypricetext->setText(StringUtils::format("%.02f", m_bookPrice / 100.0) + App::getString("YUAN"));
	}
	else
	{
		int redpacketPrice100 = 0;
		if (m_manualSelectHongbao)
		{
			//选择红包界面回来
			m_manualSelectHongbao = false;
			if (m_hongbao < 0)
				redpacketPrice100 = getOptimalRedPacket();
			else
				redpacketPrice100 = m_hongbao;
		}
		else
		{
			redpacketPrice100 = getOptimalRedPacket();
		}
		if (hongbaoshow)
		{
			if (redpacketPrice100 > 0)
				hongbaoshow->setText(App::getString("HONGBAO") + StringUtils::format("%.02f", redpacketPrice100 / 100.0) + App::getString("YUAN"));
			else if (redpacketPrice100 == 0)
				hongbaoshow->setText(App::getString("BUSHIYONGHONGBAO"));
			else if (redpacketPrice100 < 0)
				hongbaoshow->setText(App::getString("MEIYOUKEYONGHONGBAO"));
		}
		payPrice100 = m_bookPrice - redpacketPrice100;
		if (payPrice100 < 0)
			payPrice100 = 0;
		if (paypricetext)
			paypricetext->setText(StringUtils::format("%.02f", payPrice100 / 100.0) + App::getString("YUAN"));
		withRedPacket = true;
	}
	if (m_myMoney >= payPrice100)
	{
		if (gotoPay)
		{
			gotoPay->setTitleText(App::getString("GOUMAI"));
			if (withRedPacket)
				gotoPay->setTag(0);
			else
				gotoPay->setTag(1);
		}
	}
	else
	{
		if (gotoPay)
		{
			gotoPay->setTitleText(App::getString("CHONGZHI"));
			gotoPay->setTag(2);
		}
	}
	payLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	Size visibleSize = Director::getInstance()->getVisibleSize();
	payLayer->setPosition(visibleSize / 2);
	Director::getInstance()->getRunningScene()->addChild(payLayer);
	m_show = false;
	if (balance)
		balance->setText(StringUtils::format("%.02f", m_myMoney / 100.0) + App::getString("YUAN"));
	auto balanceChangeListener = EventListenerCustom::create(TAG1, [=](EventCustom* e) {
		balance->setText(StringUtils::format("%.02f", m_myMoney / 100.0) + App::getString("YUAN"));
		if (m_myMoney >= (m_bookPrice - getOptimalRedPacket()))
		{
			if (gotoPay)
			{
				gotoPay->setTitleText(App::getString("GOUMAI"));
				if (withRedPacket)
					gotoPay->setTag(0);
				else
					gotoPay->setTag(1);
			}
		}
		else
		{
			if (gotoPay)
			{
				gotoPay->setTitleText(App::getString("CHONGZHI"));
				gotoPay->setTag(2);
			}
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(balanceChangeListener, balance);
	return payLayer;
}

Layer* YYXBuyBook::buyBookSecond()
{
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "IsNotGoToPay");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_ISPAY_CSB);
	auto payLayer = YYXLayer::create(paramter);
	auto yes = (Button*)payLayer->findControl(FIND_BUTTON_ISPAY_BUY);
	auto no = (Button*)payLayer->findControl(FIND_BUTTON_ISPAY_NO);
	auto content = (Text*)payLayer->findControl(FIND_TEXT_ISPAY_CONTEXT);
	string text = "";
	if (m_hongbao > 0)
	{
		int payprice = m_bookPrice - m_hongbao;
		if (payprice < 0)
			payprice = 0;
		text = StringUtils::format(App::getString("HUAFEIMONEYBUYBOOK"), (payprice / 100.0), m_bookName.c_str());
		if (payprice == 0)
			text = StringUtils::format(App::getString("NINGJIANGMIANFEIGOUMAISHUJI"), m_bookName.c_str());
	}
	else
	{
		text = StringUtils::format(App::getString("HUAFEIMONEYBUYBOOK"), (m_bookPrice / 100.0), m_bookName.c_str());
	}
	auto ttf = Label::create();
	ttf->setAnchorPoint(Vec2(0.5, 0.5));
	ttf->setColor(Color3B(92, 92, 92));
	ttf->setTTFConfig(TTFConfig(content->getFontName().c_str(), 50));
	ttf->setScale(0.5);
	ttf->setMaxLineWidth(800);
	ttf->setString(text);
	ttf->setLineHeight(ttf->getLineHeight() + 15);
	ttf->setAdditionalKerning(ttf->getAdditionalKerning() + 5);
	content->setVisible(false);
	payLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	payLayer->setPosition(Director::getInstance()->getVisibleSize() / 2);
	payLayer->addChild(ttf);
	if (yes)
	{
		yes->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneyes", [=]() {
				payLayer->removeFromParentAndCleanup(true);
				httpBuyBook();
			});
		});
	}
	if (no)
	{
		no->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "BookInfoSceneno", [=]() {
				payLayer->removeFromParentAndCleanup(true);
			});
		});
	}
	Director::getInstance()->getRunningScene()->addChild(payLayer);
	return payLayer;
}

YYXBuyBook::YYXBuyBook()
{
	App::log("YYXBuyBook::YYXBuyBook()");
	App::log("YYXBuyBook::YYXBuyBook()---END");
}

YYXBuyBook::~YYXBuyBook()
{
	App::log("~YYXBuyBook()");
	App::log("~YYXBuyBook()---END");
}

YYXBuyBook* YYXBuyBook::GetInstance()
{
	if (instance == nullptr)
		instance = new YYXBuyBook();
	return instance;
}

void YYXBuyBook::newBuyBook(int bookid, int memberid, const function<void(int)>& buyBookCallBack /*= nullptr*/, const function<void()>& goToLoginCallBack /*= nullptr*/)
{
	init(bookid, memberid);
	buyBook(buyBookCallBack, goToLoginCallBack);
}

void YYXBuyBook::init(int bookid, int memberid)
{
	m_bookId = bookid;
	m_memberId = memberid;
}

void YYXBuyBook::newBuyFreeBook(int bookid, int memberid, const function<void(int)>& buyBookCallBack)
{
	init(bookid, memberid);
	m_buyBookSuccessCallBack = buyBookCallBack;
	httpBuyBook();
}

void YYXBuyBook::loadRedPacketData(Node* node, int index)
{
	bool show = true;
	auto data = App::GetInstance()->m_redPacket;
	if (index >= data.size())
		return;
	int coupon_id = YYXStruct::getMapInt64(data[index], "coupon_id", -999);
	int coupon_amount = YYXStruct::getMapInt64(data[index], "coupon_amount", -999);
	auto coupon_expire_time = YYXStruct::getMapString(data[index], "coupon_expire_time", "");

	if (coupon_id == -999 || coupon_amount == -999 || coupon_expire_time == "")
		show = false;
	if (node)
		node->setVisible(show);
	else
		return;
	if (show)
	{
		//有效期
		auto text_coupon_expire_time = (Text*)node->getChildByName(FIND_TEXT_BY_NAME_EVERYREDPACKET_TIME);
		if (text_coupon_expire_time)
		{
			coupon_expire_time = coupon_expire_time.substr(0, 10);
			text_coupon_expire_time->setText(App::getString("YOUXIAOQIZHI") + coupon_expire_time);
		}
		//红包价值
		auto text_coupon_amount = (Text*)node->getChildByName(FIND_TEXT_BY_NAME_EVERYREDPACKET_PRICE);
		if (text_coupon_amount)
			text_coupon_amount->setText(StringUtils::format("%.01f", coupon_amount / 100.0) + App::getString("YUAN"));
		//打钩
		auto img_couponSelect = (ImageView*)node->getChildByName(FIND_IMAGEVIE_BY_NAME_EVERYREDPACKET_SELECT);
		img_couponSelect->setTag(coupon_id);
		if (img_couponSelect && coupon_id == m_hongbaoId)
			img_couponSelect->setVisible(true);
		else
			img_couponSelect->setVisible(false);
		//红包封面
		auto redPacket = (ImageView*)node->getChildByName(FIND_IMAGEVIE_BY_NAME_EVERYREDPACKET_COVER);
		redPacket->setTag(coupon_id);
		if (redPacket)
		{
			bool click = true;
			redPacket->setTouchEnabled(true);
			redPacket->setSwallowTouches(false);
			redPacket->addTouchEventListener([=](Ref* sender, cocos2d::ui::Widget::TouchEventType types) {
				switch (types)
				{
				case cocos2d::ui::Widget::TouchEventType::BEGAN:
					m_click = true;
					break;
				case cocos2d::ui::Widget::TouchEventType::MOVED:
					m_click = false;
					break;
				case cocos2d::ui::Widget::TouchEventType::ENDED:
					if (m_click)
					{
						m_hongbao = coupon_amount;
						m_hongbaoId = coupon_id;
						YYXLayer::sendNotify("selectOver");
						m_manualSelectHongbao = true;
					}
					break;
				case cocos2d::ui::Widget::TouchEventType::CANCELED:
					break;
				}
			});
		}
		auto evenet = EventListenerCustom::create("selectOver", [=](EventCustom* e) {
			if (coupon_id == m_hongbaoId)
				img_couponSelect->setVisible(true);
			else
				img_couponSelect->setVisible(false);
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(evenet, node);
	}
}

void YYXBuyBook::feeCharging()
{
	YYXVisitor::getInstance()->hintLogin([=]() {
		auto recharge = Charger::getInstance();
		recharge->setCallback([=]() {
			//充值成功后继续购买
			buyBook(m_buyBookSuccessCallBack);
		});
		recharge->show();
	}, [=]() {
		gotoLogin();
	}, [=]() {
		auto recharge = Charger::getInstance();
		recharge->setCallback([=]() {
			//充值成功后继续购买
			buyBook(m_buyBookSuccessCallBack);
		});
		recharge->show();
	});
}

void YYXBuyBook::gotoLogin()
{
	if (m_outSceneCallBack)
		m_outSceneCallBack();	
}

//展示红包
void YYXBuyBook::listViewShowRedPacket(ListView* listview)
{
	if (!listview)
	{
		App::log("listview is null");
		return;
	}
	listview->removeAllItems();
	auto data = App::GetInstance()->m_redPacket;
	for (int i = 0; i < data.size(); i++)
	{
		Layout* layout = Layout::create();
		map<string, int64String> paramter;
		YYXLayer::insertMap4ParaType(paramter, "className", -999, "listviewItem");
		YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_SELECTREDPACKET_ITEM_CSB);
		auto item = YYXLayer::create(paramter);
		auto leftRedPacket = (Node*)item->findControl(PAY_SELECTREDPACKET_ITEM_LEFT);
		auto rightRedPacket = (Node*)item->findControl(PAY_SELECTREDPACKET_ITEM_RIGHT);
		loadRedPacketData(leftRedPacket, i);
		loadRedPacketData(rightRedPacket, i + 1);
		item->getParentNode()->setAnchorPoint(Vec2(0, 0));
		if (i + 1 >= data.size())
			rightRedPacket->setVisible(false);
		layout->addChild(item);
		layout->setContentSize(Size(500, 200));
		listview->pushBackCustomItem(layout);
		i++;
	}
	listview->jumpToTop();
}

//选出最优红包
int YYXBuyBook::getOptimalRedPacket()
{
	int count = App::GetInstance()->m_redPacket.size();
	int optimal = 0;
	int optimalID = -999;
	if (count <= 0)
	{
		m_hongbao = optimal;
		m_hongbaoId = optimalID;
		return -1;
	}//无可用红包
	optimal = App::GetInstance()->m_redPacket[0]["coupon_amount"].intData;
	optimalID = App::GetInstance()->m_redPacket[0]["coupon_id"].intData;
	int difference = optimal - m_bookPrice;
	if (difference > 0)
	{
		for (int i = 1; i < count; i++)
		{
			auto coupon_amount = App::GetInstance()->m_redPacket[i]["coupon_amount"].intData;
			auto coupon_id = App::GetInstance()->m_redPacket[i]["coupon_id"].intData;
			auto difference2 = coupon_amount - m_bookPrice;
			if (difference2 > 0 && difference > difference2)
			{
				optimal = coupon_amount;
				optimalID = coupon_id;
				difference = difference2;
			}
		}
	}
	m_hongbao = optimal;
	m_hongbaoId = optimalID;
	return optimal;
}