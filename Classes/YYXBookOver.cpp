#include "YYXBookOver.h"
#include "YYXBuyBook.h"
#include "YYXRentBook.h"
#include "YYXTableView.h"
#include "FKPageLayer.h"
#include "SendCommentLayer.h"
#include "YYXVisitor.h"
#include "YYXSound.h"
#include "AppHttp.h"
#include "BuyVip.h"
#include "IndexScene.h"
#include "LoginControl.h"
USING_NS_FK;

YYXBookOver* YYXBookOver::instance = nullptr;

YYXBookOver::YYXBookOver()
{

}

YYXBookOver::~YYXBookOver()
{

}


YYXBookOver* YYXBookOver::getInstance()
{
	if (instance == nullptr)
	{
		instance = new YYXBookOver();
	}
	return instance;
}

void YYXBookOver::init(int bookId, int memberId, bool isTry)
{
	m_bookId = bookId;
	m_memberId = memberId;
	m_isBookCoverRuning = false;
	if (isTry)
	{
		m_isUserBuy = false;
		m_isUserVip = false;
	}
	else
	{
		m_isUserBuy = true;
		m_isUserVip = true;
	}
}

Layer* YYXBookOver::tryReadBookOverLayer()
{
	AppHttp::getInstance()->httpComments(m_bookId, []() {
		YYXLayer::sendNotify("showCommentListView", "", -1);
	});
	map<string, int64String> parameter;
	YYXLayer::insertMap4ParaType(parameter, "className", -999, "tryReadBackCover");
	YYXLayer::insertMap4ParaType(parameter, "csb", -999, "Book/csb/tryReadBookEndPage.csb");
	map<string, function<void(YYXLayer*)>> runmap;
	runmap["cleanup"] = [=](YYXLayer* sender) {
		m_isBookCoverRuning = false;
	};
	auto layer = YYXLayer::create(parameter, runmap);
	layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	layer->getParentNode()->setPosition(Director::getInstance()->getWinSize() / 2);
	auto goBuy = (Button*)layer->findControl("Button_2");
	auto vipDownload = (Button*)layer->findControl("Button_2_0");
	auto noComment = (Sprite*)layer->findControl("Sprite_4");
	auto noCommenttext = (Text*)layer->findControl("Text_2");
	auto listComment = (ListView*)layer->findControl("ListView_1");
	auto yaoqing = (Button*)layer->findControl("Button_1");
	auto goumaiwanzhengban = (Button*)layer->findControl("Button_4");
	auto jixuyuedu = (Button*)layer->findControl("Button_7");
	auto goumainianka = (Button*)layer->findControl("Button_5");

	//购买成功
	auto goumaichenggong = (ImageView*)layer->findControl("Image_2"); 
	goumaichenggong->setSize(Director::getInstance()->getWinSize());	
	goumaichenggong->setVisible(false);
	auto closed = (Button*)goumaichenggong->getChildByName("Button_2");
	auto jixuyueduBox = (Button*)goumaichenggong->getChildByName("Button_3");
	if (goumaichenggong)
	{
		App::log("buySuccessMessageBox3333");
		goumaichenggong->setTouchEnabled(true);
		goumaichenggong->setSwallowTouches(true);
		goumaichenggong->addClickEventListener([=](Ref* sender) {
			goumaichenggong->setVisible(false);
		});
	}
	if (jixuyueduBox)
	{
		App::log("buySuccessMessageBox44444");
		jixuyueduBox->setTitleColor(Color3B::WHITE);
		jixuyueduBox->addClickEventListener([=](Ref* sneder) {
			//Director::getInstance()->popScene();
			BookParser::getInstance()->pageDown(true);
		});
	}
	if (closed)
	{
		App::log("buySuccessMessageBox55555");
		closed->addClickEventListener([=](Ref* sneder) {
			goumaichenggong->setVisible(false);
		});
	}
	//购买完整版
	if (goumaiwanzhengban)
	{
		goumaiwanzhengban->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "goumaiwanzhengbanTime", [=]() {
				YYXVisitor::getInstance()->hintLogin([=]() {
					Director::getInstance()->getRunningScene()->addChild(Index::SelectLayer([=]() {
						YYXBuyBook::GetInstance()->newBuyBook(m_bookId, App::GetInstance()->getMemberId(), [=](int bookid) {
							//购买成功后
							m_isUserBuy = true;
							buySuccessMessageBox(goumaichenggong);
						}, []() {
							YYXBookOver::getInstance()->GotoLogin();
						});
					}));
				}, []() {
					YYXBookOver::getInstance()->GotoLogin();
				}, [=]() {
					Director::getInstance()->getRunningScene()->addChild(Index::SelectLayer([=]() {
						YYXBuyBook::GetInstance()->newBuyBook(m_bookId, App::GetInstance()->getMemberId(), [=](int bookid) {
							//购买成功后
							m_isUserBuy = true;
							buySuccessMessageBox(goumaichenggong);
						}, []() {
							YYXBookOver::getInstance()->GotoLogin();
						});
					}));
				});
			});
		});
	}

	//邀请注册
	if (yaoqing) {
		yaoqing->setAnchorPoint(Vec2(1, 0));
		yaoqing->setPosition(Vec2((1094 + Director::getInstance()->getVisibleSize().width) / 2, 50));
		yaoqing->addClickEventListener([=](Ref* sender) {
			if (!YYXVisitor::getInstance()->getVisitorMode())
			{
				YYXSound::getInstance()->stopAll();
				YYXTableView::stopAllAnimation();
				yaoqingzhuce();
			}
			else
				Toast::create(App::getString("SHANGWEIDENGLU"));
		});
	}

	//评论
	listComment->setVisible(false);
	listComment->setScrollBarEnabled(false);
	auto tbview = YYXTableView::create();
	auto data = tbview->loadData(m_bookId);
	if (data.size() > 0)
	{
		noComment->setVisible(false);
		noCommenttext->setVisible(false);
		tbview->setPosition(Vec2(0, 0));
		tbview->setPosition(listComment->getPosition());
		tbview->setTag(159);
		layer->addChild(tbview);
	}
	else
	{
		noComment->setVisible(true);
		noCommenttext->setVisible(true);
		noCommenttext->setText(App::getString("HAIMEIYOURENPINGLUNGUOCISHU"));
	}

	//购买年卡
	if (goumainianka)
	{
		goumainianka->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "goumainiankaTime", [=]() {
				YYXVisitor::getInstance()->hintLogin([]() {
					YYXBookOver::getInstance()->GotoLogin();
				}, []() {
					YYXBookOver::getInstance()->GotoLogin();
				}, [=]() {
					Director::getInstance()->getRunningScene()->addChild(Index::SelectLayer(
						[=]() {
						auto blayer = BuyVip::getInstance()->show();
						BuyVip::getInstance()->setCallback([=](string json) {
							AppHttp::getInstance()->httpCheckVIP();
							YYXRentBook::getInstance()->backgroundThreadRentBook(m_bookId, App::GetInstance()->getMemberId(), [=]() {
								m_isUserVip = true;
								buySuccessMessageBox(goumaichenggong);
							});
						});
						if (blayer)
							Director::getInstance()->getRunningScene()->addChild(blayer);
					}));
				}, "  ",  false);
			});
		});
	}

	if (jixuyuedu)
	{
		jixuyuedu->addClickEventListener([=](Ref* sender) {
			//Director::getInstance()->popScene();
			BookParser::getInstance()->pageDown(true);
		});
	}

	if (m_isUserBuy || m_isUserVip)
	{
		jixuyuedu->setVisible(true);
		goumaiwanzhengban->setVisible(false);
		goumainianka->setVisible(false);
	}
	else
	{
		jixuyuedu->setVisible(false);
		goumaiwanzhengban->setVisible(true);
		goumainianka->setVisible(true);
	}

	auto changeListener = EventListenerCustom::create("referBookOver", [=](EventCustom* e) {
		if (!YYXVisitor::getInstance()->getVisitorMode() && User::getInstance()->getVip()) {
			m_isUserVip = true;
			log("send m_isUserVip = true; %d",1);
		}
		if (m_isUserBuy || m_isUserVip)
		{
			jixuyuedu->setVisible(true);
			goumaiwanzhengban->setVisible(false);
			goumainianka->setVisible(false);
		}
		else
		{
			jixuyuedu->setVisible(false);
			goumaiwanzhengban->setVisible(true);
			goumainianka->setVisible(true);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(changeListener, layer);
	goumaichenggong->removeFromParent();
	layer->addChild(goumaichenggong);
	m_isBookCoverRuning = true;
	return layer;
}

Layer* YYXBookOver::readBookOverLayer()
{
	AppHttp::getInstance()->httpComments(m_bookId, []() {
		YYXLayer::sendNotify("showCommentListView", "", -1);
	});
	map<string, int64String> parameter;
	YYXLayer::insertMap4ParaType(parameter, "className", -999, "BookEndPage");
	YYXLayer::insertMap4ParaType(parameter, "csb", -999, "Book/csb/BookEndPage.csb");
	auto layer = YYXLayer::create(parameter);
	layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	layer->getParentNode()->setPosition(Director::getInstance()->getWinSize() / 2);
	auto goComment = (Button*)layer->findControl("Button_2_1");
	auto noComment = (Sprite*)layer->findControl("Sprite_4");
	auto noCommenttext = (Text*)layer->findControl("Text_2_1");
	auto listComment = (ListView*)layer->findControl("ListView_1");
	auto yaoqing = (Button*)layer->findControl("Button_1");
	auto jingcaiComment = (ImageView*)layer->findControl("Image_1");
	auto MyComment = (ImageView*)layer->findControl("Image_2");
	auto jingcaiText = (Text*)layer->findControl("Text_4");
	auto mydeText = (Text*)layer->findControl("Text_4_0");
	if (yaoqing) {
		yaoqing->setAnchorPoint(Vec2(1, 0));
		yaoqing->setPosition(Vec2((1094 + Director::getInstance()->getVisibleSize().width) / 2, 50));
		yaoqing->addClickEventListener([=](Ref* sender) {
			if (!YYXVisitor::getInstance()->getVisitorMode())
			{
				YYXSound::getInstance()->stopAll();
				YYXTableView::stopAllAnimation();
				yaoqingzhuce();
			}
			else
				Toast::create(App::getString("SHANGWEIDENGLU"));
		});
	}
	
	auto tbview = YYXTableView::create();
	auto data = tbview->loadData(m_bookId);
	listComment->setVisible(false);
	if (data.size() > 0)
	{
		noComment->setVisible(false);
		noCommenttext->setVisible(false);
		tbview->setPosition(listComment->getPosition());
		tbview->setTag(159);
		layer->addChild(tbview);
	}
	else
	{
		noComment->setVisible(true);
		noCommenttext->setVisible(true);
		noCommenttext->setText(App::getString("HAIMEIYOURENPINGLUNGUOCISHU"));
	}

	//初始化界面 
	//精彩评论
	if (jingcaiComment) {
		jingcaiComment->setTag(1);
		jingcaiComment->loadTexture("Book/res/Backcover_comments_press_736h.png", TextureResType::PLIST);
		jingcaiComment->setTouchEnabled(true);
		jingcaiComment->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "jingcaiCommentTime", [=]() {
				jingcaiComment->loadTexture("Book/res/Backcover_comments_press_736h.png", TextureResType::PLIST);
				MyComment->loadTexture("Book/res/Backcover_comments_736h.png", TextureResType::PLIST);
				jingcaiComment->setTag(1);
				jingcaiText->setTextColor(Color4B(187, 93, 39, 255));
				mydeText->setTextColor(Color4B(255, 255, 255, 255));
				MyComment->setTag(0);
				noComment->setVisible(false);
				noCommenttext->setVisible(false);
				YYXSound::getInstance()->stopAll();
				YYXTableView::stopAllAnimation();
				YYXLayer::sendNotify("showCommentListView", "444444444444444444", -1);
			});
		});
	}
	//评论成功
	auto ListenerClickComment = EventListenerCustom::create(TAG_SENDCOMMENTSUCCESS, [=](EventCustom* e) {
		int memberid = -1;
		if (MyComment->getTag() == 1)
			memberid = m_memberId;
		AppHttp::getInstance()->httpComments(m_bookId, [=]() {
			YYXLayer::sendNotify("showCommentListView", "", memberid);
		});
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(ListenerClickComment, layer);
	//刷新列表
	auto listenerRefersh = EventListenerCustom::create("showCommentListView", [=](EventCustom* e) {
		long memberid = (long)e->getUserData();
		App::log(" --------------     run showCommentListView()   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^memberid = ", memberid);
		YYXSound::getInstance()->stopAll();
		YYXTableView::stopAllAnimation();

		auto tbview = (YYXTableView*)layer->getChildByTag(159);
		if (tbview == nullptr)
		{
			tbview = YYXTableView::create();
			tbview->setTag(159);
			tbview->setPosition(listComment->getPosition());
			layer->addChild(tbview);
		}
		auto data = tbview->loadData(m_bookId, memberid);
		if (data.size() > 0)
		{
			noComment->setVisible(false);
			noCommenttext->setVisible(false);
		}
		else
		{
			noComment->setVisible(true);
			noCommenttext->setVisible(true);
			if (memberid == -1)
				noCommenttext->setText(App::getString("HAIMEIYOURENPINGLUNGUOCISHU"));
			else
				noCommenttext->setText(App::getString("NIHAIMEIYOUPINGLUNGUOCISHU"));
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerRefersh, layer);

	//我的评论
	if (MyComment) {
		MyComment->setTag(0);
		MyComment->loadTexture("Book/res/Backcover_comments_736h.png", TextureResType::PLIST);
		MyComment->setTouchEnabled(true);
		MyComment->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "MyCommentTime", [=]() {
				jingcaiComment->loadTexture("Book/res/Backcover_comments_736h.png", TextureResType::PLIST);
				MyComment->loadTexture("Book/res/Backcover_comments_press_736h.png", TextureResType::PLIST);
				mydeText->setTextColor(Color4B(187, 93, 39, 255));
				jingcaiText->setTextColor(Color4B(255, 255, 255, 255));
				jingcaiComment->setTag(0);
				MyComment->setTag(1);
				noComment->setVisible(false);
				YYXSound::getInstance()->stopAll();
				YYXTableView::stopAllAnimation();
				YYXLayer::sendNotify("showCommentListView", "", atoi(App::GetInstance()->getMemberID().c_str()));
			});
		});
	}
	//按钮
	if (goComment) {
		goComment->setVisible(true);
		goComment->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "backCoverGoCommentTime", [=]() {
				Director::getInstance()->getRunningScene()->addChild(SendComment::create(m_bookId));
				YYXSound::getInstance()->stopAll();
				YYXTableView::stopAllAnimation();
			});
		});
		goComment->setVisible(true);
	}
	return layer;
}

void YYXBookOver::buySuccessMessageBox(ImageView* goumaichenggongLayer)
{
	YYXLayer::sendNotify("referBookOver");	
	goumaichenggongLayer->setAnchorPoint(Vec2(0.5, 0.5));
	Size visibleSize = Director::getInstance()->getVisibleSize();
	goumaichenggongLayer->setPosition(Vec2(1094+ visibleSize.width, 614+ visibleSize.height) / 4);
	goumaichenggongLayer->setVisible(true);	
}

void YYXBookOver::yaoqingzhuce()
{
	string url = string("http://ellabook.cn/ellaBook-invite-red/index.html?memberId=").append(App::getMemberID());
	CrossPlatform::inviteRegister(App::GetInstance()->getMemberId(), url, "", [](string json) {}, "", [](string str) {});
}

void YYXBookOver::GotoLogin()
{
	/*auto control = ControlScene::getInstance();
	control->replaceScene(control->getCurrentScene(), ControlScene::getInstance()->getSceneInfo(LoginScene));
	thread ([=]() {
		App::ccsleep(3000);
		control->end();
	}).detach();*/
	ControlScene::getInstance()->setDangqianScene(BOOK);
	LoginControl::getInstance()->Login([](string json) {
		LoginControl::getInstance()->LoginCallback(json);
	}, nullptr);
}