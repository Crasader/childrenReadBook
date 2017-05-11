#include "SendCommentLayer.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "BookInfoScene.h"
#include "YYXVisitor.h"
#include "YYXSound.h"
#include "User.h"
#include "AppHttp.h"

USING_NS_CC;

using namespace cocostudio::timeline;

SendComment::SendComment() {
	star_score = 5;
	m_bookId = -999;
	m_orderId = -999;
}

SendComment::~SendComment() {

}

SendComment* SendComment::create(int bookId) {
	SendComment *pRect = new SendComment();
	if (pRect){
		if (pRect->init(bookId)){
			pRect->autorelease();
		}
		else{
			pRect->release();
			pRect = nullptr;
		}
	}
	return pRect;
}

bool SendComment::init(int bookId) {
	if (!Layer::init())
	{
		return false;
	}
	CCLOG("send_______________________1");
	m_bookId = bookId;
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto commentLayer = CSLoader::createNode(SENDCOMMENT_CSB);
	commentLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	commentLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(commentLayer);

	//黑色半透明背景屏蔽下层触摸
	auto myImgView = (ImageView*)commentLayer->getChildByName(SENDCOMMENT_BLACK_IMGVIEW);
	myImgView->setTouchEnabled(true);
	myImgView->addClickEventListener([](Ref* sender) {
		CCLOG("this is black imgView");
	});
	auto liste = EventListenerCustom::create("DeleteSendComment", [=](EventCustom* e) {
		commentLayer->removeFromParentAndCleanup(true);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(liste, commentLayer);
	//----------------------------父母确认--------------------------------
	auto parentSure = (Node*)commentLayer->getChildByName(SENDCOMMENT_KUANG_PARENT_SURE);
	//关闭按钮
	auto parent_close = (Button*)parentSure->getChildByName(SENDCOMMENT_KUANG_PARENT_SURE_CLOSE_BTN);
	parent_close->addClickEventListener([=](Ref* sender) {
		this->removeFromParentAndCleanup(true);
	});
	//提示文本
	auto parent_text = (Text*)parentSure->getChildByName(SENDCOMMENT_KUANG_PARENT_SURE_TEXT_TIP);
	//随机文本
	int goal_color = rand() % 3;
	if (goal_color == 0)
		parent_text->setString(App::getString("STR_RED"));
	else if (goal_color == 1)
		parent_text->setString(App::getString("STR_YELLOW"));
	else if (goal_color == 2)
		parent_text->setString(App::getString("STR_BLUE"));
	//红色按钮
	auto parent_red = (Button*)parentSure->getChildByName(SENDCOMMENT_KUANG_PARENT_SURE_RED);
	//黄色按钮
	auto parent_yellow = (Button*)parentSure->getChildByName(SENDCOMMENT_KUANG_PARENT_SURE_YELLOW);
	//蓝色按钮
	auto parent_blue = (Button*)parentSure->getChildByName(SENDCOMMENT_KUANG_PARENT_SURE_BLUE);


	//发表评论
	auto comment = (Node*)commentLayer->getChildByName(SENDCOMMENT_KUANG_COMMENT);
	//关闭按钮
	auto comment_close = (Button*)comment->getChildByName(SENDCOMMENT_KUANG_COMMENT_CLOSE_BTN);
	comment_close->addClickEventListener([=](Ref* sender) {
		this->removeFromParentAndCleanup(true);
	});
	//第一颗星星
	auto comment_star1 = (ImageView*)comment->getChildByName(SENDCOMMENT_KUANG_COMMENT_STAR1);
	//第二颗星星
	auto comment_star2 = (ImageView*)comment->getChildByName(SENDCOMMENT_KUANG_COMMENT_STAR2);
	//第三颗星星
	auto comment_star3 = (ImageView*)comment->getChildByName(SENDCOMMENT_KUANG_COMMENT_STAR3);
	//第四颗星星
	auto comment_star4 = (ImageView*)comment->getChildByName(SENDCOMMENT_KUANG_COMMENT_STAR4);
	//第五颗星星
	auto comment_star5 = (ImageView*)comment->getChildByName(SENDCOMMENT_KUANG_COMMENT_STAR5);
	comment_star1->setTouchEnabled(true);
	comment_star2->setTouchEnabled(true);
	comment_star3->setTouchEnabled(true);
	comment_star4->setTouchEnabled(true);
	comment_star5->setTouchEnabled(true);
	comment_star1->addClickEventListener([=](Ref* sender) {
		star_score = 1;
		showXingJi(star_score, comment_star2, comment_star3, comment_star4, comment_star5);
		//comment_star2->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
		//comment_star3->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
		//comment_star4->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
		//comment_star5->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
	});
	comment_star2->addClickEventListener([=](Ref* sender) {
		star_score = 2;
		showXingJi(star_score, comment_star2, comment_star3, comment_star4, comment_star5);
		//comment_star2->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
		//comment_star3->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
		//comment_star4->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
		//comment_star5->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
	});
	comment_star3->addClickEventListener([=](Ref* sender) {
		star_score = 3;
		showXingJi(star_score, comment_star2, comment_star3, comment_star4, comment_star5);
		//comment_star2->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
		//comment_star3->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
		//comment_star4->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
		//comment_star5->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
	});
	comment_star4->addClickEventListener([=](Ref* sender) {
		star_score = 4;
		showXingJi(star_score, comment_star2, comment_star3, comment_star4, comment_star5);
		//comment_star2->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
		//comment_star3->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
		//comment_star4->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
		//comment_star5->loadTexture(SENDCOMMENT_FINDPIC_STAR1, ui::Widget::TextureResType::PLIST);
	});
	comment_star5->addClickEventListener([=](Ref* sender) {
		star_score = 5;
		showXingJi(star_score, comment_star2, comment_star3, comment_star4, comment_star5);
		//comment_star2->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
		//comment_star3->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
		//comment_star4->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
		//comment_star5->loadTexture(SENDCOMMENT_FINDPIC_STAR2, ui::Widget::TextureResType::PLIST);
	});
	//内容背景
	auto img = (ImageView*)comment->getChildByName("Image_1");
	//输入内容
	auto textContent = MyEditBox::create(Size(350 - 10, 175 - 20), App::getString("EDITBOX_FEEDBOX"));
	textContent->setMyFontSize(18);
	textContent->setMyInputMode(cocos2d::ui::EditBox::InputMode::ANY);
	textContent->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	//textContent->setAnchorPoint(Vec2(0, 1));
	textContent->setPosition(img->getPosition());
	textContent->setBoxMaxLength(100);
	//textContent->setColor(Color3B::GREEN);
	comment->addChild(textContent);
	//发表语音评论
	auto yuying = (Button*)comment->getChildByName("Button_1");
	if (yuying)
	{
		yuying->setTouchEnabled(true);
		yuying->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "sendCommentyuyingTime", [=]() {
				YYXLayer::setFileValue("temp_bookid", Value(m_bookId).asString());
				YYXLayer::setFileValue("temp_star_score", Value(star_score).asString());
				AppHttp::getInstance()->httpBookOrderId(m_bookId, [=](string orderid, int types) {
					if (orderid.empty())
					{
						Toast::create(App::getString("FAXIANNINWEICHENGYUEDU"));
						return;
					}
					int temp_bookid = Value(YYXLayer::getFileValue("temp_bookid", "-1")).asInt();
					int temp_star_score = Value(YYXLayer::getFileValue("temp_star_score", "-1")).asInt();
					YYXLayer::deleteFileValue("temp_bookid");
					YYXLayer::deleteFileValue("temp_star_score");
					YYXLayer::deleteFileValue("temp_content");
					YYXSound::getInstance()->stopAll();
					YYXSound::getInstance()->pauseBackGroundMusic();
					string account = "";
					if (YYXVisitor::getInstance()->getVisitorMode())
						account = YYXVisitor::getInstance()->getVisitorName();
					else
						account = User::getInstance()->getUserAccount();
					//打开语音界面
					CrossPlatform::goToSendRecording(temp_bookid, App::getMemberId(), types, account, orderid, "goToSendRecordingSuccess", [](string json) {
						//语音成功的回调 - 
						Toast::create(App::getString("COMMENT_SUCCESS"));
						YYXLayer::sendNotify(TAG_SENDCOMMENTSUCCESS);
						YYXLayer::sendNotify("DeleteSendComment");
						YYXSound::getInstance()->playBackGroundMusic();
					}, "goToSendRecordingFail", [=](string str) {
						//语音失败的回调
						Toast::create(App::getString("COMMENT_FAILED"));
						YYXSound::getInstance()->playBackGroundMusic();
					}, "goToSendRecordingCloseButton", [=](string str) {
						//关闭按钮的回调
						YYXSound::getInstance()->playBackGroundMusic();
					});
					YYXLayer::sendNotify("DeleteSendComment");
				}, []() {
					YYXSound::getInstance()->playBackGroundMusic();
				});
			});
		});
	}
	//发表按钮
	auto comment_send = (Button*)comment->getChildByName(SENDCOMMENT_KUANG_COMMENT_SEND_BTN);
	comment_send->addClickEventListener([=](Ref* sender) {
		string strContent = textContent->getString();
		//内容少于10个字
		if (strContent.length() < 30) {
			Toast::create(App::getString("COMMENT_CONTENT_ERROR"));
			return;
		}
		sendComment("      ", strContent);
		comment_send->setTouchEnabled(false);
		this->removeFromParentAndCleanup(true);
	});

	//红色按钮点击事件
	parent_red->addClickEventListener([=](Ref* sender) {
		if (goal_color != 0) {
			this->removeFromParentAndCleanup(true);
			return;
		}
		parentSure->setPositionX(1094);
		comment->setPositionX(0);
	});
	//黄色按钮点击事件
	parent_yellow->addClickEventListener([=](Ref* sender) {
		if (goal_color != 1) {
			this->removeFromParentAndCleanup(true);
			return;
		}
		parentSure->setPositionX(1094);
		comment->setPositionX(0);
	});
	//蓝色按钮点击事件
	parent_blue->addClickEventListener([=](Ref* sender) {
		if (goal_color != 2) {
			this->removeFromParentAndCleanup(true);
			return;
		}
		parentSure->setPositionX(1094);
		comment->setPositionX(0);
	});
	auto listener_showComment = EventListenerCustom::create("SendCommentShowTextComment", [=](EventCustom* e) {
		parentSure->setPositionX(1094);
		comment->setPositionX(0);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener_showComment, parentSure);
	return true;
}

//文字评论
void SendComment::sendComment(string title, string content) {
	YYXLayer::setFileValue("temp_bookid", Value(m_bookId).asString());
	YYXLayer::setFileValue("temp_star_score", Value(star_score).asString());
	YYXLayer::setFileValue("temp_content", content);
	AppHttp::getInstance()->httpBookOrderId(m_bookId, [](string orderid, int types) {
		if (orderid.empty())
		{
			Toast::create(App::getString("FAXIANNINWEICHENGYUEDU"));
			return;
		}
		int temp_bookid = Value(YYXLayer::getFileValue("temp_bookid", "-1" )).asInt();
		int temp_star_score = Value(YYXLayer::getFileValue("temp_star_score", "-1")).asInt();
		string content = YYXLayer::getFileValue("temp_content", "good book");
		YYXLayer::deleteFileValue("temp_bookid");
		YYXLayer::deleteFileValue("temp_star_score");
		YYXLayer::deleteFileValue("temp_content");
		AppHttp::getInstance()->httpSendComment(types, temp_bookid, temp_star_score, orderid, content);
	});
}

void SendComment::sendOver(int tag) {
	this->removeFromParentAndCleanup(true);
	if (tag == 0) {
		Toast::create(App::getString("COMMENT_SUCCESS"), "");
	}
	else if(tag == 1){
		Toast::create(App::getString("COMMENT_ALREADY"), "");
	}
	else{
		Toast::create(App::getString("COMMENT_FAILED"), "");
	}
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([]() {
		Director::getInstance()->getEventDispatcher()->setEnabled(true);
		EventCustom event("httpBookComments");
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
	});
}

void SendComment::showXingJi(int data, ImageView* xing2, ImageView* xing3, ImageView* xing4, ImageView* xing5 )
{
	App::log("                             xing ji = ", data);
	switch (data)
	{
	case 1:
		xing2->loadTexture("BookInfo/res/Voicecomment_star.png",TextureResType::PLIST);
		xing3->loadTexture("BookInfo/res/Voicecomment_star.png", TextureResType::PLIST);
		xing4->loadTexture("BookInfo/res/Voicecomment_star.png", TextureResType::PLIST);
		xing5->loadTexture("BookInfo/res/Voicecomment_star.png", TextureResType::PLIST);
		break;
	case 2:
		xing2->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing3->loadTexture("BookInfo/res/Voicecomment_star.png", TextureResType::PLIST);
		xing4->loadTexture("BookInfo/res/Voicecomment_star.png", TextureResType::PLIST);
		xing5->loadTexture("BookInfo/res/Voicecomment_star.png", TextureResType::PLIST);
		break;
	case 3:
		xing2->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing3->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing4->loadTexture("BookInfo/res/Voicecomment_star.png", TextureResType::PLIST);
		xing5->loadTexture("BookInfo/res/Voicecomment_star.png", TextureResType::PLIST);
		break;
	case 4:
		xing2->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing3->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing4->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing5->loadTexture("BookInfo/res/Voicecomment_star.png", TextureResType::PLIST);
		break;
	case 5:
		xing2->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing3->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing4->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing5->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		break;
	default:
		xing2->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing3->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing4->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		xing5->loadTexture("BookInfo/res/Voicecomment_star_press.png", TextureResType::PLIST);
		break;
	}
}