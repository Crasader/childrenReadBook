#include "SendCommentLayer.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "BookInfoScene.h"

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

	//输入标题
	auto textTitle = MyEditBox::create(Size(200, 36), App::getString("COMMENT_TITLE"));
	textTitle->setMyFontSize(18);
	//textTitle->setBoxMaxLength(11);
	textTitle->setMyInputMode(cocos2d::ui::EditBox::InputMode::ANY);
	textTitle->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	textTitle->setAnchorPoint(Vec2(0, 1));
	textTitle->setPosition(Vec2(525, 325));
	comment->addChild(textTitle);
	//输入内容
	auto textContent = MyEditBox::create(Size(290, 130), App::getString("EDITBOX_FEEDBOX"));
	textContent->setMyFontSize(18);
	textContent->setMyInputMode(cocos2d::ui::EditBox::InputMode::ANY);
	textContent->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	textContent->setAnchorPoint(Vec2(0, 1));
	textContent->setPosition(Vec2(547, 215));
	comment->addChild(textContent);
	//发表语音评论
	auto yuying = (Button*)comment->getChildByName("Button_1");
	if (yuying)
	{
		yuying->setTouchEnabled(true);
		yuying->addClickEventListener([=](Ref* sender) {
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "sendCommentData", m_bookId, "", (Ref*)star_score);
			NetIntface::httpGetBookIsBuy(m_bookId, App::GetInstance()->m_me->id, -1, "bookinfoSceneGetBookIsBuySuccess", [=](string json) {
				NetIntface::httpGetBookIsBuyCallBack(json, [=](string orderid, int types) {
					auto userAccount = YYXLayer::getFileValue("userAccount", "");
					int bookid = YYXStruct::getMapInt64(App::GetInstance()->myData, "sendCommentData", -999);
					if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
					{
						SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
						SimpleAudioEngine::getInstance()->pauseAllEffects();
					}
					//打开语音界面
					NetIntface::goToSendRecording(m_bookId, App::GetInstance()->m_me->id, types, userAccount,  orderid, "goToSendRecordingSuccess", [](string json) {
						//语音成功
						Toast::create(App::getString("COMMENT_SUCCESS"));
						YYXLayer::sendNotify("bookinfoSceneHttpSendCommentSuccess");
						YYXLayer::sendNotify("DeleteSendComment");
					}, "goToSendRecordingFail", [=](string str) {					
						//返回的回调
						if (YYXLayer::getBoolFromXML(MUSIC_KEY))
							SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
						else
							SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
						//Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
						//	Director::getInstance()->getRunningScene()->addChild(SendComment::create(bookid), 10); 
						//	YYXLayer::sendNotify("SendCommentShowTextComment");
						//});
					});
					YYXLayer::sendNotify("DeleteSendComment");
				});
			}, "goToSendRecordingFail", [](string str) {
				//orderid 获取的失败回调
				Toast::create(App::getString("WUFAQIDONGYUYINGPINGLUN"));
			});
		});
	}
	//发表按钮
	auto comment_send = (Button*)comment->getChildByName(SENDCOMMENT_KUANG_COMMENT_SEND_BTN);
	comment_send->addClickEventListener([=](Ref* sender) {
		string strTitle = textTitle->getString();
		string strContent = textContent->getString();
		//标题为空
		if (strTitle.length() == 0) {
			Toast::create(App::getString("COMMENT_TITLE_ERROR"));
			return;
		}
		//内容少于10个字
		if (strContent.length() < 20) {
			Toast::create(App::getString("COMMENT_CONTENT_ERROR"));
			return;
		}

		sendComment(strTitle, strContent);
		comment_send->setTouchEnabled(false);
		this->removeFromParentAndCleanup(true);
	});

	//红色按钮点击事件
	parent_red->addClickEventListener([=](Ref* sender) {
		if (goal_color != 0) {
			CCLOG("------------error");
			this->removeFromParentAndCleanup(true);
			return;
		}
		parentSure->setPositionX(1094);
		comment->setPositionX(0);
	});
	//黄色按钮点击事件
	parent_yellow->addClickEventListener([=](Ref* sender) {
		if (goal_color != 1) {
			CCLOG("------------error");
			this->removeFromParentAndCleanup(true);
			return;
		}
		parentSure->setPositionX(1094);
		comment->setPositionX(0);
	});
	//蓝色按钮点击事件
	parent_blue->addClickEventListener([=](Ref* sender) {
		if (goal_color != 2) {
			CCLOG("------------error");
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
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "sendCommentData", m_bookId, "", (Ref*)star_score);
	NetIntface::httpGetBookIsBuy(m_bookId, App::GetInstance()->m_me->id, -1, "bookinfoSceneGetBookIsBuySuccess", [=](string json) {
		NetIntface::httpGetBookIsBuyCallBack(json, [=](string orderid, int types) {
			App::log("orderid=" + orderid);
			auto userAccount = YYXLayer::getFileValue("userAccount", "");
			int bookid = YYXStruct::getMapInt64(App::GetInstance()->myData, "sendCommentData", -999);
			auto starScore = (long)YYXStruct::getMapRef(App::GetInstance()->myData, "sendCommentData", (Ref*)5);
			NetIntface::httpSendComment(types, bookid, App::GetInstance()->m_me->id, starScore, orderid, userAccount, title, content, "bookinfoSceneSendCommentSuccess", [](string json) {
				rapidjson::Document doc;
				if (YYXLayer::getJsonObject4Json(doc, json))
				{
					string desc = App::getString("COMMENT_SUCCESS");
					if (YYXLayer::getBoolForJson(false, doc, "result"))
					{
						YYXLayer::sendNotify("bookinfoSceneHttpSendCommentSuccess");
					}
					else
					{
						desc = YYXLayer::getStringForJson(App::getString("COMMENT_FAILED"), doc, "errorMessage");
					}
					Toast::create(desc.c_str());
				}
			}, "bookinfoSceneSendCommentFail", [](string sttr) {
				Toast::create(App::getString("COMMENT_FAILED"));
			});
		});
	}, "bookinfoSceneSendCommentFail", [](string str) {
		Toast::create(App::getString("COMMENT_FAILED"));
	});
	//auto userAccount = YYXStruct::getMapString(App::GetInstance()->myData, "userAccount", "");
	//CocosAndroidJni::Comment(App::GetInstance()->m_me->id,m_bookId, userAccount.c_str(),
		//content.c_str(), title.c_str(), star_score);
	/*HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_SEND_COMMENT).c_str());

	auto postData = std::string("memberId=").append(App::getMemberID()).append("&memberName=").append(App::GetInstance()->m_acount->memberName).append
	(StringUtils::format("&orderId=%d", m_orderId)).append(StringUtils::format("&bookId=%d", m_bookId)).append("&comment=").append(content).append
	("&title=").append(title).append("&score=").append(StringUtils::format("%d", star_score));
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));

	App::log(std::string(IP).append(NET_SEND_COMMENT).c_str());
	App::log(postData.c_str());

	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		if (!response)
		{
			sendOver(-1);
			return;
		}
		if (!response->isSucceed())
		{
			sendOver(-1);
			log("response failed! error buffer: %s", response->getErrorBuffer());
			return;
		}
		
		//json解析
		std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
		App::log(str);
		rapidjson::Document doc;
		doc.Parse<0>(str.c_str());
		if (doc.HasParseError()) {
			sendOver(-1);
			return;
		}

		if (!doc.IsObject() || !doc.HasMember("code")) {
			sendOver(-1);
			return;
		}

		rapidjson::Value &value = doc["code"];
		if (!value.IsInt()) {
			sendOver(-1);
			return;
		}

		if (value.GetInt() == 0) {//评价成功
			sendOver(0);
			return;
		}
		else if(value.GetInt() == 1){//已评价过，不能再次评价
			sendOver(1);
			return;
		}
		else{//评价失败
			sendOver(-1);
			return;
		}
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();*/
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