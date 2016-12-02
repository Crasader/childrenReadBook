#pragma execution_character_set("utf-8")
#include "LoginScene.h"
#include "NetIntface.h"
#include "XZLayer.h"

Scene* Login::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Login::create();

    // add layer as a child to scene
    scene->addChild(layer);
	layer->setTag(9);
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Login::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
	 
	//记录当前场景
	App::m_RunningScene = LoginScene;

	////安卓返回键
	//auto androidListener = EventListenerKeyboard::create();
	//androidListener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event) {
	//	if (!App::GetInstance()->isBack)
	//		return;
	//	switch (keyCode)
	//	{
	//	case EventKeyboard::KeyCode::KEY_ESCAPE:
	//		App::GetInstance()->popBack();
	//		Index::BackPreviousScene();
	//		App::backThreading();
	//		break;
	//	default:
	//		break;
	//	}
	//};
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(androidListener, this);

	this->addChild(loginInit());
	//this->addChild(addChildInit());
    return true;
}

void Login::onEnterTransitionDidFinish()
{
	App::log("Login::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	App::log("Login::onEnterTransitionDidFinish---END");
}

//忘记密码 对话框初始化
Layer* Login::forgetPasswordInit()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto forgetPasswordScene = CSLoader::createNode(FORGETPASSWORDMESSAGEBOX_CSB);
	forgetPasswordScene->setAnchorPoint(Vec2(0.5f, 0.5f));
	forgetPasswordScene->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
	auto b_forgetpassword = ( Button*)forgetPasswordScene->getChildByName(FIND_BUTTON_BY_NAME_FORGETPASSWORD_FORGET);
	auto b_back = ( Button*)forgetPasswordScene->getChildByName(FIND_BUTTON_BY_NAME_FORGETPASSWORD_BACK);
	auto b_getCode = ( Button*)forgetPasswordScene->getChildByName(FIND_BUTTON_BY_NAME_FORGETPASSWORD_GETCODE);
	//auto tf_phone = ( TextField*)forgetPasswordScene->getChildByName(FIND_TEXTFIELD_BY_NAME_FORGETPASSWORD_PHONENUMBER);
	//auto tf_code = ( TextField*)forgetPasswordScene->getChildByName(FIND_TEXTFIELD_BY_NAME_FORGETPASSWORD_CODE);
	//auto tf_password = ( TextField*)forgetPasswordScene->getChildByName(FIND_TEXTFIELD_BY_NAME_FORGETPASSWORD_PASSWORD);

	auto tf_phone = MyEditBox::create(Size(280, 45), App::getString("EDITBOX_PHONE"));
	tf_phone->setMyFontSize(20);
	tf_phone->setBoxMaxLength(11);
	tf_phone->setMyInputMode(cocos2d::ui::EditBox::InputMode::NUMERIC);
	tf_phone->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	tf_phone->setPosition(Vec2(590, 346));
	forgetPasswordScene->addChild(tf_phone);

	auto tf_code = MyEditBox::create(Size(105, 45), App::getString("EDITBOX_SURE_CODE"));
	tf_code->setMyFontSize(20);
	tf_code->setBoxMaxLength(6);
	tf_code->setMyInputMode(cocos2d::ui::EditBox::InputMode::NUMERIC);
	tf_code->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	tf_code->setPosition(Vec2(505, 280));
	forgetPasswordScene->addChild(tf_code);

	auto tf_password = MyEditBox::create(Size(280, 45), App::getString("EDITBOX_PASSWORD_CONDITION"));
	tf_password->setMyFontSize(20);
	tf_password->setBoxMaxLength(20);
	tf_password->setMyInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	tf_password->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	tf_password->setPosition(Vec2(590, 215));
	forgetPasswordScene->addChild(tf_password);
	

	if (App::getCurrentTime() < (App::GetInstance()->m_Time + 60))
	{
		b_getCode->setTouchEnabled(false);
		schedule([=](float f) {
			int times = App::getCurrentTime() - App::GetInstance()->m_Time;
			char str[100];
			sprintf(str, "%d%s", 60 - times, App::getChars("seconds"));
			b_getCode->setTitleText(str);
			if (times > 60)
			{
				App::GetInstance()->m_Time = 0;
				b_getCode->setTouchEnabled(true);
				b_getCode->setTitleText(App::getString("GetCode"));
				unschedule("Codeinit");
			}
		}, 0.5f, "Codeinit");
	}

	//返回按钮
	b_back->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "LoginSceneb_backTime", [=]() {
			closeTime();
			this->removeChild(forgetPasswordScene);
			this->addChild(loginInit());
		});
	});
	//修改密码
	b_forgetpassword->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "LoginSceneb_forgetpasswordTime", [=]() {
			//string xmlCode = "";
			//xmlCode = UserDefault::getInstance()->getStringForKey("yanzhengCode");
			//int xmlCodeTime = 0;
			//xmlCodeTime = UserDefault::getInstance()->getIntegerForKey("codeTime");

			string xmlCode = YYXLayer::getFileValue("yanzhengCode", "");
			string xmlCodeTimestr = YYXLayer::getFileValue("codeTime", "");
			int xmlCodeTime = atoi(xmlCodeTimestr.c_str());

			auto movT = xmlCodeTime - (int)App::getCurrentTime();
			if (xmlCode == "" || movT > 1800)
			{
				Toast::create(App::getString("GET_CODE"));
				return;
			}

			auto code = tf_code->getString();
			if (code != xmlCode)
			{
				Toast::create(App::getString("MESAAGEBOX_CODE_ERROR"));
				return;
			}
			auto phone = tf_phone->getString();
			App::log("phone.length()=", phone.length());
			if (phone.length() != 11)
			{
				Toast::create(App::getString("MESAAGEBOX_USERNAME_ERROR"));
				return;
			}

			auto password = tf_password->getString();
			if (password.length() < 6)
			{
				Toast::create(App::getString("MESAAGEBOX_PASSWORD_ERROR"));
				return;
			}
			forgetPassword(phone, password, forgetPasswordScene);
		});
	});

	//获取验证码 按钮
	b_getCode->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "LoginSceneb_getCodeTime", [=]() {
			//if (!CocosAndroidJni::IsNetConnect())
			//{
			//	Toast::create(App::getString("NETEXCEPTION"));
			//	return;
			//}
			if (!NetIntface::IsNetConnect(true))
				return;
			auto phone = tf_phone->getString();
			if (phone.length() != 11)
			{
				Toast::create(App::getString("MESAAGEBOX_USERNAME_ERROR"));
				return;
			}
			getCode(phone, "0");
			b_getCode->setTouchEnabled(false);
			schedule([=](float f) {
				int times = App::getCurrentTime() - App::GetInstance()->m_Time;
				char str[100];
				sprintf(str, "%d%s", 60 - times, App::getString("seconds"));
				b_getCode->setTitleText(str);
				if (times > 60)
				{
					App::GetInstance()->m_Time = 0;
					b_getCode->setTouchEnabled(true);
					b_getCode->setTitleText(App::getString("GetCode"));
					unschedule("Code");
				}
			}, 0.5f, "Code");
		});
	});
	return (Layer*)forgetPasswordScene;
}

//注册 对话框初始化
Layer* Login::registerInit()
{
	auto regiserLayer = CSLoader::createNode(REGISTERMESSAGEBOX_CSB);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	regiserLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
	regiserLayer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	//------------------------------------获取控件-----------------------------------------------------
	auto b_back = (Button*)regiserLayer->getChildByName(FIND_BUTTON_BY_NAME_REGISTER_BACK);
	auto b_registering = (Button*)regiserLayer->getChildByName(FIND_BUTTON_BY_NAME_REGISTER_REGISTERING);
	auto b_getCode = (Button*)regiserLayer->getChildByName(FIND_BUTTON_BY_NAME_CODE);
	/*auto b_name = (TextField*)regiserLayer->getChildByName(FIND_TEXTFIELD_BY_NAME_USERNAME);
	auto b_code = (TextField*)regiserLayer->getChildByName(FIND_TEXTFIELD_BY_NAME_CODE);
	auto b_password = (TextField*)regiserLayer->getChildByName(FIND_TEXTFIELD_BY_NAME_PASSWORD);*/

	auto b_name = MyEditBox::create(Size(265+35, 45+15), App::getString("EDITBOX_PHONE"));
	b_name->setMyFontSize(27);
	b_name->setBoxMaxLength(11);
	b_name->setMyInputMode(cocos2d::ui::EditBox::InputMode::NUMERIC);
	b_name->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	b_name->setPosition(Vec2(585+10, 354+17));
	regiserLayer->addChild(b_name);

	auto b_code = MyEditBox::create(Size(100+20, 45+10), App::getString("EDITBOX_SURE_CODE"));
	b_code->setMyFontSize(27);
	b_code->setBoxMaxLength(6);
	b_code->setMyInputMode(cocos2d::ui::EditBox::InputMode::NUMERIC);
	b_code->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	b_code->setPosition(Vec2(505, 284+7));
	regiserLayer->addChild(b_code);

	auto b_password = MyEditBox::create(Size(265+35, 45+15), App::getString("EDITBOX_PASSWORD_CONDITION"));
	b_password->setMyFontSize(27);
	b_password->setBoxMaxLength(16);
	b_password->setMyInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	b_password->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	b_password->setPosition(Vec2(585+10, 215-4));
	regiserLayer->addChild(b_password);

	if (App::getCurrentTime() < (App::GetInstance()->m_Time + 60))
	{
		b_getCode->setTouchEnabled(false);
		schedule([=](float f) {
			int times = App::getCurrentTime() - App::GetInstance()->m_Time;
			char str[100];
			sprintf(str, "%d%s", 60 - times, App::getString("seconds"));
			b_getCode->setTitleText(str);
			if (times > 60)
			{
				App::GetInstance()->m_Time = 0;
				b_getCode->setTouchEnabled(true);
				b_getCode->setTitleText(App::getString("GetCode"));
				unschedule("Codeinit");
			}
		}, 0.5f, "Codeinit");
	}
	//--------------------------------------点击事件--------------------------------------------------------------
	//返回按钮
	b_back->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "LoginSceneb_backTime", [=]() {
			closeTime();
			this->removeChild(regiserLayer);
			this->addChild(loginInit());
		});
	});
	//注册按钮
	b_registering->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "LoginSceneb_registeringTime", [=]() {
			auto code = b_code->getString();
			//xmlCode = UserDefault::getInstance()->getStringForKey("yanzhengCode");
			string xmlCode = YYXLayer::getFileValue("yanzhengCode", "");
			//xmlCodeTime = UserDefault::getInstance()->getIntegerForKey("codeTime");
			string xmlCodeTimestr = YYXLayer::getFileValue("codeTime", "");
			int xmlCodeTime = atoi(xmlCodeTimestr.c_str());
			App::log("输入=" + code + "验证码=" + xmlCode + "验证码获取时间=", xmlCodeTime);
			if (code.length() != 6)
			{
				Toast::create(App::getString("MESAAGEBOX_CODE_ERROR"));
				return;
			}

			auto movT = xmlCodeTime - (int)App::getCurrentTime();
			if (xmlCode == "" || movT > 1800)
			{
				Toast::create(App::getString("GET_CODE"));
				return;
			}

			if (xmlCode != code)
			{
				Toast::create(App::getString("MESAAGEBOX_CODE_ERROR"));
				return;
			}

			auto name = b_name->getString();
			if (name.length() != 11)
			{
				Toast::create(App::getString("MESAAGEBOX_USERNAME_ERROR"));
				return;
			}

			auto password = b_password->getString();
			if (password.length() < 1)
			{
				Toast::create(App::getString("MESAAGEBOX_PASSWORD_ERROR"));
				return;
			}
			b_registering->setTitleText(App::getString("ZHUCEZHONG"));
			//通知:注册成功
			/*Director::getInstance()->getEventDispatcher()->addCustomEventListener("registerSuccess", [=](EventCustom* eve) {
				string data = (const char*)eve->getUserData();
				CallBackRegisterAccount(data, regiserLayer);
			});*/
			//注册过程
			string runKey = "LoginScenehttpAccountRegisteSuccess";
			string errorKey = "LoginScenehttpAccountRegisteFail";
			NetIntface::httpAccountRegiste(name, password, runKey, [=](string json) 
			{
				//注册json回调
				NetIntface::httpAccountRegisteCallBack(json, [=](int memberId, int memberSex, string memberName, string memberProvince, string memberCity, string couponSum)
				{
					//解析后成功的业务处理
					Toast::create(App::getString("MESAAGEBOX_REGISTER_SUCCESS"));
					//1.写入内存
					if (App::GetInstance()->m_me == nullptr)
					{
						App::GetInstance()->m_me = new MyAccount();
						App::GetInstance()->m_me->id = memberId;
						App::GetInstance()->m_me->momey = 0;
					}
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userAccount", -999, name);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userPassword", -999, password);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userProvince", -999, memberProvince);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userCity", -999, memberCity);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userSex", memberSex);
					//2.缓存xml
					YYXLayer::setFileValue("userAccount", name);
					YYXLayer::setFileValue("userPassword", password);
					YYXLayer::setFileValue("userProvince", memberProvince);
					YYXLayer::setFileValue("userCity", memberCity);
					YYXLayer::setFileValue("userSex", StringUtils::format("%d", memberSex));
					YYXLayer::setFileValue("userId", StringUtils::format("%d", memberId));
					YYXLayer::setFileValue("accountRegisteUserId", StringUtils::format("%d", memberId));
					//UserDefault::getInstance()->setStringForKey("userAccount", name);
					//UserDefault::getInstance()->setStringForKey("userPassword", password);
					//UserDefault::getInstance()->setStringForKey("userProvince", memberProvince);
					//UserDefault::getInstance()->setStringForKey("userCity", memberCity);
					//UserDefault::getInstance()->setIntegerForKey("userSex", memberSex);
					//UserDefault::getInstance()->setIntegerForKey("userId", memberId);
					//UserDefault::getInstance()->setIntegerForKey("accountRegisteUserId", memberId);//注册成功标记, 首页提示红包
					//4.获取用户红包
					httpGetUserRedPackets(memberId);
					//5.获取红包活动
					httpRedPacketActivity();
					//3.确定孩子头像
					App::log("注册成功,请求孩子", memberId);
					getChildList(memberId, []() {
						auto id = YYXStruct::getMapInt64(App::GetInstance()->myData, "firstChildID", 14116);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", id);
						YYXLayer::setFileValue("ShowChildID", StringUtils::format("%d", id));
						//UserDefault::getInstance()->setIntegerForKey("ShowChildID", id);
						App::GetInstance()->m_read->childrenId = id;
					});
					Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
						YYXLayer::sendNotify("LoginScenehttpAccountRegisteOver");
						closeTime();
						App::GetInstance()->popBack();
						Index::BackPreviousScene();
						thread runable([=]() {	
							App::ccsleep(1000);
							int count = __String(couponSum).intValue();
							if(count > 0)
								XZLayer::showShareRedPacket(couponSum + App::getString("YUAN")); 
						});
						runable.detach();
					});
				}, [](string error) 
				{//解析失败或者返回失败
					if (error == "")
						Toast::create(App::getString("ZHUCESHIBAI"));
					else
						Toast::create(error.c_str());
					YYXLayer::sendNotify("LoginScenehttpAccountRegisteOver");
				});
			}, errorKey, [](string str) {
				//注册网络失败
				Toast::create(App::getString("NETEXCEPTION"));
				YYXLayer::sendNotify("LoginScenehttpAccountRegisteOver");
			});
		});
	});
	auto b_registeringListerOver = EventListenerCustom::create("LoginScenehttpAccountRegisteOver", [=](Ref* sender) {
		b_registering->setTitleText(App::getString("ZHUCEBINGDENGLU"));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(b_registeringListerOver, b_registering);
	//获取验证码 按钮
	b_getCode->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "LoginSceneb_getCodeTime", [=]() {
			//if (!CocosAndroidJni::IsNetConnect())
			//{
			//	Toast::create(App::getString("NETEXCEPTION"));
			//	return;
			//}
			if (!NetIntface::IsNetConnect(true))
				return;
			auto name = b_name->getString();
			if (name.length() != 11)
			{
				Toast::create(App::getString("MESAAGEBOX_USERNAME_ERROR"));
				return;
			}
			getCode(name, "1");
			b_getCode->setTouchEnabled(false);
			schedule([=](float f) {
				int times = App::getCurrentTime() - App::GetInstance()->m_Time;
				char str[100];
				sprintf(str, "%d%s", 60 - times, App::getString("seconds"));
				b_getCode->setTitleText(str);
				if (times > 60)
				{
					App::GetInstance()->m_Time = 0;
					b_getCode->setTouchEnabled(true);
					b_getCode->setTitleText(App::getString("GetCode"));
					unschedule("Code");
				}
			}, 0.5f, "Code");
		});
	});
	return (Layer*)regiserLayer;
}

//获取到充值送红包的活动内容
void Login::httpRedPacketActivity()
{
	string url = string(IP) + NET_GETCHARGEACITIVITY;
	string runkey = "httpRedPacketActivitySuccess";
	NetIntface::httpGet(url, runkey, [](string json) {
		NetIntface::httpGetRechargeActivityCallBack(json, [](int index, int redPacketCount) {
			string redPacketActivityKey = StringUtils::format("RechargeIndex=%d", index);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, redPacketActivityKey, redPacketCount);
		}, []() {}, []() {});
	}, "", [](string str) {});
}

//添加宝贝 对话框初始化
/*Layer* Login::addChildInit()
{
	auto addchildMessagebox = CSLoader::createNode(ADDCHILD_CSB);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	addchildMessagebox->setAnchorPoint(Vec2(0.5f, 0.5f));
	addchildMessagebox->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	auto b_add = (Button*)addchildMessagebox->getChildByName(FIND_BUTTON_BY_NAME_ADDCHILD_ADD);
	//auto ttf_name = (TextField*)addchildMessagebox->getChildByName(FIND_TEXTFIELD_BY_NAME_ADDCHILD_BABYNAME);
	//auto ttf_day = (TextField*)addchildMessagebox->getChildByName(FIND_TEXTFIELD_BY_NAME_ADDCHILD_BABYDAY);

	auto ttf_name = MyEditBox::create(Size(260, 40), App::getString("EDITBOX_BABYNAME"));
	ttf_name->setMyFontSize(20);
	ttf_name->setBoxMaxLength(12);
	ttf_name->setMyInputMode(ui::EditBox::InputMode::SINGLE_LINE);
	ttf_name->setMyInputFlag(ui::EditBox::InputFlag::SENSITIVE);
	ttf_name->setPosition(Vec2(581, 348));
	addchildMessagebox->addChild(ttf_name);

	auto ttf_day = DropBox::create(Vec2(455, 207), Size(260, 25), "wryh.TTF", TextHAlignment::CENTER);
	addchildMessagebox->addChild(ttf_day);
	ttf_day->setDropEnable(true);

	auto man = (CheckBox*)addchildMessagebox->getChildByName(FIND_CHECKBOX_BY_NAME_ADDCHILD_MAN);
	auto woman = (CheckBox*)addchildMessagebox->getChildByName(FIND_CHECKBOX_BY_NAME_ADDCHILD_WOMAN);
	auto Pic_man = (ImageView*)addchildMessagebox->getChildByName(FIND_IMAGEVIEW_BY_NAME_ADDCHILD_MAN);
	auto Pic_woman = (ImageView*)addchildMessagebox->getChildByName(FIND_IMAGEVIEW_BY_NAME_ADDCHILD_WOMAN);	
	//--------------------------------点击事件-------------------------------------------------
	man->setTouchEnabled(true);
	woman->setTouchEnabled(true);
	Pic_man->setTouchEnabled(true);
	Pic_woman->setTouchEnabled(true);
	auto lister_man = [=](Ref* sender) {
		woman->setSelectedState(false);
		man->setSelectedState(true);
	};
	man->addClickEventListener(lister_man);
	Pic_man->addClickEventListener(lister_man);
	auto lister_woman = [=](Ref* sender) {
		woman->setSelectedState(true);
		man->setSelectedState(false);
	};
	woman->addClickEventListener(lister_woman);
	Pic_woman->addClickEventListener(lister_woman);
	//添加宝贝按钮
	b_add->addClickEventListener([=](Ref* sender) {
		auto IsMan = false;
		if (man->getSelectedState())
			IsMan = true;
		auto name = ttf_name->getString();
		if (name.length() <= 0)
		{
			Toast::create(App::getString("MESAAGEBOX_ADDCHILD_NAME_ERROR"));
			return;
		}
		auto day = ttf_day->getDateText();
		/*if (day.length() < 10)
		{
			Toast::create(App::getString("MESAAGEBOX_ADDCHILD_DAY_ERROR"));
			return;
		}
		//addChildInfo(name,IsMan?1:2, day);
		NetIntface::httpAddChild(App::GetInstance()->m_me->id, name, IsMan ? 1 : 2, day, StringUtils::format("httpAddChild%d",(int)YYXLayer::getRandom()), [](string json) {
			
		}, "", [](string str) {});
		//跳转到父母选项
		//Index::BackPreviousScene();
	});
	return (Layer*)addchildMessagebox;
}*/
//----------------------------网络接口---------------------------------------------
//登录 对话框初始化
Layer* Login::loginInit()
{
	Node* loginmessagebox;
	Data data;
	if (App::GetInstance()->getData(LOGINMESSAGEBOX_CSB, data))
		loginmessagebox = CSLoader::createNode(data);
	else
		loginmessagebox = CSLoader::createNode(LOGINMESSAGEBOX_CSB);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	loginmessagebox->setAnchorPoint(Vec2(0.5f, 0.5f));
	loginmessagebox->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	//------------------------------------获取控件-----------------------------------------------------
	auto b_close = (Button*)loginmessagebox->getChildByName(FIND_BUTTON_BY_NAME_LOGINMESSAGEBOX_CLOSE);
	auto b_register = (Button*)loginmessagebox->getChildByName(FIND_BUTTON_BY_NAME_LOGINMESSAGEBOX_Register);
	auto b_login = (Button*)loginmessagebox->getChildByName(FIND_BUTTON_BY_NAME_LOGINMESSAGEBOX_LOGIN);
	auto t_forgetpassword = (Text*)loginmessagebox->getChildByName(FIND_TEXT_BY_NAME_LOGINMESSAGEBOX_FORGETPASSWORD);
	//auto account = (ui::TextField*)loginmessagebox->getChildByName(FIND_TEXTFIELD_BY_NAME_LOGINMESSAGEBOX_ACCOUNT);
	/*auto password = (ui::TextField*)loginmessagebox->getChildByName(FIND_TEXTFIELD_BY_NAME_LOGINMESSAGEBOX_PASSWORD);
	password->setPasswordStyleText("*");
	password->setEnabled(false);*/
	auto account = MyEditBox::create(Size(300, 60), App::getString("EDITBOX_PHONE"));
	account->setMyFontSize(25);
	account->setBoxMaxLength(11);
	account->setMyInputMode(cocos2d::ui::EditBox::InputMode::NUMERIC);
	account->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	account->setPosition(Vec2(620, 350));
	loginmessagebox->addChild(account);

	auto password = MyEditBox::create(Size(300, 60), App::getString("EDITBOX_PASSWORD"));
	password->setMyFontSize(25);
	password->setBoxMaxLength(16);
	password->setMyInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	password->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::PASSWORD);
	password->setPosition(Vec2(620, 265));
	loginmessagebox->addChild(password);
	
	//--------------------------------------点击事件--------------------------------------------------------------
	b_close->addClickEventListener([=](Ref* sender) {
		//关闭 返回
		YYXLayer::controlTouchTime(1, "LoginSceneb_closeTime", [=]() {
			App::GetInstance()->popBack();
			Index::BackPreviousScene();
		});
	});
	b_register->addClickEventListener([=](Ref* sender) {
		//注册图层
		YYXLayer::controlTouchTime(1, "LoginSceneb_registerTime", [=]() {
			closeTime();
			this->removeChild(loginmessagebox);
			auto registermessagebox = registerInit();
			this->addChild(registermessagebox);
		});
	});

	//---------------------------------------通知------------------------------------------------------------------------------------
	//通知: 登录成功
	_eventDispatcher->removeCustomEventListeners("USER_LOGIN_SUCCESS");
	auto listenLoginSuccess = EventListenerCustom::create("USER_LOGIN_SUCCESS", [=](EventCustom* event) {
		App::log("************************YYXLayer::sendNotify(USER_LOGIN_SUCCESS);");
		//验证成功,返回前场景
		App::GetInstance()->popBack();
		Index::BackPreviousScene();
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listenLoginSuccess, 1);

	//通知: 登录失败
	_eventDispatcher->removeCustomEventListeners("USER_LOGIN_FAILED");
	auto listenLoginFailed = EventListenerCustom::create("USER_LOGIN_FAILED", [=](EventCustom* event) {
			//开启所有触发事件
			Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(Director::getInstance()->getRunningScene(), true);			
			b_login->setTitleText(App::getString("DENGLU"));
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listenLoginFailed, 1);
	//---------------------------------------通知END------------------------------------------------------------------------------------

		//登录按钮
	b_login->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "LoginSceneb_loginTime", [=]() {			
			auto phone = account->getString();
			auto _password = password->getString();
			if (phone == "888" && _password == "debug")
			{
				App::m_debug = 0;
				Toast::create("TEST MODE");
			}
			if (phone.length() != 11)
			{
				Toast::create(App::getString("MESAAGEBOX_USERNAME_ERROR"));
				return;
			}
			if (_password.empty())
			{
				Toast::create(App::getString("QINGSHURUMIMA"));
				return;
			}
			//暂停所有触发事件
			//Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(Director::getInstance()->getRunningScene(), true);
			b_login->setTitleText(App::getString("DEMGLUZHONG"));
			//网络请求登录		
			//Load::verification(phone, _password);
			LogIn(phone, _password);
		});
	});
	auto listener1 = EventListenerCustom::create("loginFail", [=](EventCustom* e) {
		b_login->setTitleText(App::getString("DENGLU"));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener1, b_login);
	t_forgetpassword->setTouchEnabled(true);
	t_forgetpassword->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "LoginScenet_forgetpasswordTime", [=]() {
			closeTime();
			this->removeChild(loginmessagebox);
			this->addChild(forgetPasswordInit());
		});
	});
	//---------------------------------------点击事件END------------------------------------------------------------------------------------
	
	return (Layer*)loginmessagebox;
}

//获取注册验证码
void Login::getCode(std::string account,string type)
{
	//if (!CocosAndroidJni::IsNetConnect())
	//{
	//	Toast::create(App::getString("NETEXCEPTION"));
	//	return;
	//}
	if (!NetIntface::IsNetConnect(true))
		return;
	Toast::create(App::getString("GetCode"));
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_GETCODE).c_str());
	auto postData = std::string("mobileNum=").append(account).append("&type=").append(type)
		.append("&resource=").append(App::m_resource);
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));
	App::log(std::string(IP).append(NET_GETCODE).append("\n").append(postData).c_str());
	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		std::string str4result(App::getString("MESAAGEBOX_GETCODE_SUCCESS"));
		if (!response)
		{
			Toast::create(App::getString("NETEXCEPTION"));
			return;
		}
		if (!response->isSucceed())
		{
			log("response failed");
			log("error buffer: %s", response->getErrorBuffer());
			Toast::create(App::getString("NETEXCEPTION"));
			return;
		}
		//解析json
		//{"num":"480473","desc":"success","mobileNum":"18888702717","code":0}
		//{"desc":"mobilenum has registed：18888702717", "code" : 10003}
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		rapidjson::Document doc;
		doc.Parse<0>(str.c_str());
		if (doc.HasParseError())
		{
			CCLOG("json parse error");
		}
		else
		{
			if (doc.IsObject() && doc.HasMember("code") )
			{
				rapidjson::Value &code = doc["code"];
				if (code.IsInt() && code.GetInt() == 0)
				{
					//验证码发送成功
					if (doc.HasMember("num") )
					{
						rapidjson::Value &num = doc["num"];
						if (num.IsString())
						{
							string code = num.GetString();
							if (code.length() == 6)
							{
								YYXLayer::setFileValue("yanzhengCode", code);
								YYXLayer::setFileValue("codeTime", StringUtils::format("%d",(int)YYXLayer::getCurrentTime4Second()));
								//UserDefault::getInstance()->setStringForKey("yanzhengCode", code);
								//UserDefault::getInstance()->setIntegerForKey("codeTime", (int)App::getCurrentTime());
							}
						}
					}
					Toast::create(App::getString("MESAAGEBOX_GETCODE_SUCCESS"));
					App::startTime();
				}
				else
				{
					//验证码发送失败
					rapidjson::Value &desc = doc["desc"];
					App::log( desc.GetString());
					if (code.GetInt() == 10003)
					{
						Toast::create(App::getString("YONGHUYIZHUCEQINGDENGLU"));
					}
					else
					{
						Toast::create(App::getString("YANZHENGMACUOWUQINGCHONGSHI"));
					}
				}
			}
		}
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
}

//注册账号
/*void Login::registerAccount(std::string memberName,std::string memberPasswd)
{
	if (!CocosAndroidJni::IsNetConnect())
	{
		Toast::create(App::getString("NETEXCEPTION"));
		return;
	}
	Toast::create("注册中,请耐心等待...");
	//网络注册账号
	CocosAndroidJni::AccountRegiste(memberName.c_str() , memberPasswd.c_str());
	/*HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_REGISTER).c_str());
	//memberName	String	用户名
	//memberPasswd	String	密码
	//rePasswd	String	确认密码
	//inviteCode	String	邀请码，用户有邀请码时注册接口需传值
	auto postData = std::string("memberName=").append(memberName).append
		("&memberPasswd=").append(memberPasswd).append("&rePasswd=").append(rePasswd)
		.append("&resource=").append(App::m_resource);
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));
	//CCLOG(std::string(IP).append(NET_REGISTER).append("\n").append(postData).c_str());
	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		std::string str4result(App::getString("MESAAGEBOX_REGISTER_SUCCESS"));
		if (!response)
		{
			Toast::create("网络异常");
			return;
		}
		if (!response->isSucceed())
		{
			log("response failed");
			log("error buffer: %s", response->getErrorBuffer());
			Toast::create("网络异常");
			return;
		}
		//解析json
		//{"result":false, "code" : 10001, "id" : null, "errorMessage" : "该用户名已经存在！", "toURL" : null, "data" : null}
		//{"result":true,"code":0,"id":null,"errorMessage":null,"toURL":"/member/login?username=18888702718","data":{"memberCity":null,"memberName":"18888702718","memberProvince":null,"memberSex":"1","memberId":3618}}
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		//CCLOG(str.c_str());
		rapidjson::Document doc;
		doc.Parse<0>(str.c_str());
		if (doc.HasParseError())
		{
			CCLOG("json parse error : %s", doc.GetParseError());
		}
		else
		{
			if (doc.IsObject() && doc.HasMember("result"))
			{
				rapidjson::Value &value = doc["result"];
				if (value.IsBool() && value.GetBool())
				{
					//注册成功 切换到添加宝贝		
					closeTime();
					App::GetInstance()->m_me = new MyAccount();
					//解析data节点将数据装入m_me
					if (doc.HasMember("data"))
						App::analysisMemberInfo(doc["data"]);
					else
						delete App::GetInstance()->m_me;
					closeTime();
					this->removeChild(regiserLayer);
					this->addChild(addChildInit());
				}
				else
				{
					//注册失败
					rapidjson::Value &errorMessage = doc["errorMessage"];
					if (doc.HasMember("errorMessage") && errorMessage.IsString())
					{
						str4result = errorMessage.GetString();
					}
				}
			}
			Toast::create(str4result.c_str());
		}
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(3);
	request->release();
}*/

//注册账号成功的回调
/*void Login::CallBackRegisterAccount(string str,Node* regiserLayer)
{
	App::log("注册回调str=" + str);
	rapidjson::Document doc;	
	doc.Parse<0>(str.c_str());
	if (doc.HasParseError())
	{
		CCLOG("json parse error : %s", doc.GetParseError());
	}
	else
	{
		if (doc.IsObject() && doc.HasMember("result"))
		{
			rapidjson::Value &value = doc["result"];
			if (value.IsBool() && value.GetBool())
			{				
				//注册成功 切换到添加宝贝
				closeTime();
				App::GetInstance()->m_me = new MyAccount();
				//解析data节点将数据装入m_me
				if (doc.HasMember("data"))
				{
					if (data.IsObject())
					{
						if (data.HasMember("memberId"))
						{
							rapidjson::Value &memberId = data["memberId"];
							if (memberId.IsInt())
							{
								App::GetInstance()->m_me->id = memberId.GetInt();
								CocosAndroidJni::setMemberId(memberId.GetInt());
								App::log("memberId = ", App::GetInstance()->m_me->id);
							}
							else
								App::log("[analysisMemberInfo]function error = memberId ");
						}
						if (data.HasMember("memberSex"))
						{
							rapidjson::Value &memberSex = data["memberSex"];
							if (memberSex.IsString())
								App::GetInstance()->m_me->sex = __String(memberSex.GetString()).intValue();
							else
								App::log("[analysisMemberInfo]function error = memberSex ");
						}
						if (data.HasMember("memberName"))
						{
							rapidjson::Value &memberName = data["memberName"];
							if (memberName.IsString())
								App::GetInstance()->m_me->memberName = memberName.GetString();
							else
								App::log("[analysisMemberInfo]function error = memberName ");
						}
						if (data.HasMember("memberProvince"))
						{
							rapidjson::Value &memberProvince = data["memberProvince"];
							if (memberProvince.IsString())
								App::GetInstance()->m_me->memberProvince = memberProvince.GetString();
							else
								App::log("[analysisMemberInfo]function error = memberProvince ");
						}
						if (data.HasMember("memberCity"))
						{
							rapidjson::Value &memberCity = data["memberCity"];
							if (memberCity.IsString())
								App::GetInstance()->m_me->memberCity = memberCity.GetString();
							else
								App::log("[analysisMemberInfo]function error = memberCity ");
						}
					}
					//App::analysisMemberInfo(doc["data"]);
				}
				else
					delete App::GetInstance()->m_me;				
				closeTime();
				this->removeChild(regiserLayer);
				addChildInfo("Baby",1,"2008.08.08");
				Index::BackPreviousScene();
			}
		}
	}
}

//网络请求添加宝贝
void Login::addChildInfo(std::string childrenName, int childrenSex, std::string childrenBirth)
{
	if (!CocosAndroidJni::IsNetConnect())
	{
		Toast::create(App::getString("NETEXCEPTION"));
		return;
	}
	//TODO 网络注册账号
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_ADDCHILD).c_str());
	//memberId	Integer	父母id
	char id[10];
	sprintf(id,"%d", App::GetInstance()->m_me->id);	
	auto postData = std::string("memberId=").append(id).append("&resource=").append(App::m_resource);
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));
	//CCLOG(std::string(IP).append(NET_ADDCHILD).append("?").append(postData).c_str());
	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		std::string str4result(App::getString("MESAAGEBOX_ADDCHILD_SUCCESS"));
		if (!response)
		{
			Toast::create(App::getString("NETEXCEPTION"));
			return;
		}
		if (!response->isSucceed())
		{
			log("response failed");
			log("error buffer: %s", response->getErrorBuffer());
			Toast::create(App::getString("NETEXCEPTION"));
			return;
		}
		//{"code":0, "data" : [{"childrenName":"小王子", "childrenBirth" : "2015.01.01", "memberId" : 3651, "childrenId" : 3182, 
			//"childrenSex" : 1, "avatarUrl" : null, "compressAvaterUrl" : null, "childrenAddtime" : 1459151376}], "desc" : "success"}
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		CCLOG(str.c_str());
		rapidjson::Document doc;
		doc.Parse<0>(str.c_str());
		auto success = false;
		if (doc.HasParseError())
		{
			CCLOG("json parse error : %s", doc.GetParseError());
		}
		else
		{
			if (doc.IsObject() && doc.HasMember("code"))
			{
				rapidjson::Value &code = doc["code"];
				if (code.IsInt() && code.GetInt() == 0 && doc.HasMember("data"))
				{
					rapidjson::Value &data = doc["data"];
					if (data.IsArray())
					{
						rapidjson::Value &child = data[0];
						if (child.IsObject() && child.HasMember("childrenId"))
						{
							rapidjson::Value &childrenId = child["childrenId"];
							if (childrenId.IsInt())
							{
								//创建孩子成功后 修改孩子信息
								success = true;
								//changeChildInfo(childrenId.GetInt(), childrenName, childrenSex, childrenBirth);
							}
						}
					}
				}
			}
		}
		if (!success)
		{
			Toast::create(App::getString("MESAAGEBOX_ADDCHILD_ERROR"));
			return;
		}
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(3);
	request->release();
}
*/
//修改孩子信息
/*void Login::changeChildInfo(int childrenId,std::string childrenName, int childrenSex, std::string childrenBirth)
{
	if (!CocosAndroidJni::IsNetConnect())
	{
		Toast::create(App::getString("NETEXCEPTION"));
		return;
	}
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_CHANGECHILDINFO).c_str());
	//memberId	Integer	父母id
	//	childrenName	String	孩子姓名
	//	childrenSex	Byte	孩子性别1男，2女
	//	childrenBirth	byte	孩子生日
	char id[10];
	sprintf(id, "%d", childrenId);
	char sex[10];
	sprintf(sex,"%d", childrenSex);
	auto postData = std::string("childrenId=").append(id).append("&childrenName=").append(childrenName).append("&childrenSex=").append
		(sex).append("&childrenBirth=").append(childrenBirth).append("&resource=").append(App::m_resource);;
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));
	CCLOG(std::string(IP).append(NET_ADDCHILD).append("\n").append(postData).c_str());
	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		std::string str4result(App::getString("MESAAGEBOX_ADDCHILD_SUCCESS"));
		if (!response)
		{
			Toast::create(App::getString("NETEXCEPTION"));
			return;
		}
		if (!response->isSucceed())
		{
			log("response failed");
			log("error buffer: %s", response->getErrorBuffer());
			Toast::create(App::getString("NETEXCEPTION"));
			return;
		}
		//{"code":0, "data" : [{"childrenName":"小王子", "childrenBirth" : "2015.01.01", "memberId" : 3651, "childrenId" : 3182, 
		//"childrenSex" : 1, "avatarUrl" : null, "compressAvaterUrl" : null, "childrenAddtime" : 1459151376}], "desc" : "success"}
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		//CCLOG(str.c_str());
		auto success = false;
		rapidjson::Document doc;
		doc.Parse<0>(str.c_str());
		if (doc.HasParseError())
		{
			CCLOG("json parse error : %s", doc.GetParseError());
		}
		else
		{
			if (doc.IsObject() && doc.HasMember("code"))
			{
				rapidjson::Value &code = doc["code"];
				if (code.IsInt() && code.GetInt() == 0)
				{
					//修改孩子信息
					success = true;
					Index::BackPreviousScene();
				}
			}
		}
		if(!success)
		{
			Toast::create(App::getString("MESAAGEBOX_ADDCHILD_ERROR"));
			return;
		}
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(3);
	request->release();
}*/

//修改密码
void Login::forgetPassword(std::string memberName,std::string password, Node* forgetPasswordScene)
{
	//if (!CocosAndroidJni::IsNetConnect())
	//{
	//	Toast::create(App::getString("NETEXCEPTION"));
	//	return;
	//}
	if (!NetIntface::IsNetConnect(true))
		return;
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_CHANGEPASSWORD).c_str());
	auto postData = std::string("memberName=").append(memberName).append("&password=").append(password)
		.append("&resource=").append(App::m_resource);;
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));
	App::log(std::string(IP).append(NET_CHANGEPASSWORD).append("\n").append(postData).c_str());
	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		std::string str4result(App::getString("MESAAGEBOX_ADDCHILD_SUCCESS"));
		if (!response)
		{
			Toast::create(App::getString("NETEXCEPTION"));
			return;
		}
		if (!response->isSucceed())
		{
			log("response failed");
			log("error buffer: %s", response->getErrorBuffer());
			Toast::create(App::getString("NETEXCEPTION"));
			return;
		}
		//{"desc":"success", "code" : 0}
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());
		App::log(str);
		auto success = false;
		auto message = App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR");
		rapidjson::Document doc;
		doc.Parse<0>(str.c_str());
		if (doc.HasParseError())
		{
			CCLOG("json parse error : %s", doc.GetParseError());
		}
		else
		{
			if (doc.IsObject() && doc.HasMember("code"))
			{
				rapidjson::Value &code = doc["code"];
				if (code.IsInt() && code.GetInt() == 0)
				{
					//修改密码成功
					success = true;
					message = App::getString("MESAAGEBOX_FORGETPASSWORD_SUCCESS");
					//返回登录对话框
					closeTime();
					this->removeChild(forgetPasswordScene);
					this->addChild(loginInit());
				}
			}
		}
		Toast::create(message);
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(3);
	request->release();
}

//void Login::prestrain()
//{
//	auto cache = SpriteFrameCache::getInstance();
//	cache->addSpriteFramesWithFile(PLIST_LOGIN);
//}

void Login::closeTime()
{
	unschedule("Codeinit");
	unschedule("Code");
}

//手动登陆
void Login::LogIn(string member_name, string member_passwd)
{
	if (member_name != "" && member_passwd != "") {
		NetIntface::httpLogIn(member_name, member_passwd,"LoginSceneHttpLogInSuccess", [=](string json) {
			NetIntface::httpLogInCallBack(json, [=](int memberId, int memberSex, int memberGrade, string memberCity, string memberProvince) {
				//登陆成功
				if (App::GetInstance()->m_me == nullptr)
					App::GetInstance()->m_me = new MyAccount();
				App::GetInstance()->m_me->id = memberId;
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userAccount", -999, member_name);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userSex", memberSex);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userCity", -999, memberCity);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userProvince", -999, memberProvince);
				Toast::create(App::getString("DENGLUCHENGGONG"));
				//用户信息存储本地
				YYXLayer::setFileValue("userAccount", member_name);
				YYXLayer::setFileValue("userPassword", member_passwd);
				YYXLayer::setFileValue("userId", StringUtils::format("%d", memberId));
				YYXLayer::setFileValue("userSex", StringUtils::format("%d", memberSex));
				YYXLayer::setFileValue("userCity", memberCity);
				YYXLayer::setFileValue("userProvince", memberProvince);
				/*UserDefault::getInstance()->setStringForKey("userAccount", member_name);
				UserDefault::getInstance()->setStringForKey("userPassword", member_passwd);
				UserDefault::getInstance()->setIntegerForKey("userId", memberId);
				UserDefault::getInstance()->setIntegerForKey("userSex", memberSex);
				UserDefault::getInstance()->setStringForKey("userCity", memberCity);
				UserDefault::getInstance()->setStringForKey("userProvince", memberProvince);*/
				//本地读取最近下载 最近阅读
				string rfilename = "readBook/download_" + StringUtils::format("%d", memberId) + ".json";
				string dfilename = "downloadBook/download_" + StringUtils::format("%d", memberId) + ".json";
				string downloadPath = FileUtils::getInstance()->getWritablePath() + dfilename;
				string readPath = FileUtils::getInstance()->getWritablePath() + rfilename;
				map<string, string> data;
				App::getMapFromFile(downloadPath, data);
				App::GetInstance()->bookDownLoad.clear();
				for (auto it : data)
				{
					int d = atoi(it.first.c_str());
					if (d != 0)
						App::GetInstance()->bookDownLoad[d] = atoi(it.second.c_str());
				}
				map<string, string> dat;
				App::getMapFromFile(readPath, dat);
				App::GetInstance()->bookRead.clear();
				for (auto its : dat)
				{
					int d = atoi(its.first.c_str());
					if (d != 0)
						App::GetInstance()->bookRead[d] = atoi(its.second.c_str());
				}
				//查询vip
				App::httpCheckVIP(memberId);
				//查完从本地读取租书json
				App::getLocalRentJson();				
				//确定需要展示的头像和孩子ID
				getShowIDandShowHeadPortrait(memberId);
				//获取用户红包
				httpGetUserRedPackets(memberId);
				//获取当前用户已购列表
				//Load::getBuyBook();
				NetIntface::httpGetUserBuyBooks(memberId, "LoginSceneHttpGetUserBuyBooksSuccess", [=](string json) {	
					NetIntface::httpGetUserBuyBooksCallBack(json, []() {
						//json成功, array前执行
						App::GetInstance()->myBuyBookMap.clear();
						App::GetInstance()->myBuyBookVector.clear();
					}, [](int bookId, int orderId, string bookCoverUrl, string bookPlayUrl, string bookName) {
						//解析过程
						if (App::GetInstance()->myBuyBookMap.find(bookId) == App::GetInstance()->myBuyBookMap.end())
						{
							App::GetInstance()->myBuyBookMap[bookId] = bookPlayUrl;
							App::GetInstance()->myBuyBookVector.push_back(bookId);
						}
					}, [memberId]() {
						//解析成功
						YYXLayer::buyBooksJsonWriteFile(App::GetInstance()->myBuyBookVector, memberId, FileUtils::getInstance()->getWritablePath()+"books.json");
						YYXLayer::sendNotify("loginSceneHttpGetUserBuyBooksSuccess");
					}, []() {
						//解析错误
						Toast::create(App::getString("YIGOUSHUJILIEBIAOGENGXINSHIBAI"));
					});
				}, "LoginSceneHttpGetUserBuyBooksFail", [](string str) {
					//网络错误
					Toast::create(App::getString("YIGOUSHUJILIEBIAOGENGXINSHIBAI"));
				});
				YYXLayer::sendNotify("USER_LOGIN_SUCCESS");
			}, [](string str) {
				YYXLayer::sendNotify("USER_LOGIN_FAILED");
				Toast::create(str.c_str());
			});
		}, "LoginSceneHttpLogInFail", [](string str) {
			YYXLayer::sendNotify("loginFail");
			Toast::create(App::getString("NETEXCEPTION"));
			YYXLayer::sendNotify("USER_LOGIN_FAILED");
		});
	}
}

//确定需要展示的头像和孩子ID
void Login::getShowIDandShowHeadPortrait(int member_id)
{
	if (member_id != -999)
	{
		if (App::GetInstance()->m_me == nullptr)
			App::GetInstance()->m_me = new MyAccount();
		App::GetInstance()->m_me->id = member_id;
		string ShowChildIDstr = YYXLayer::getFileValue("ShowChildID", "");
		//int ShowChildID = UserDefault::getInstance()->getIntegerForKey("ShowChildID", -999);
		if (ShowChildIDstr != "")
		{//有孩子ID
			int ShowChildID = atoi(ShowChildIDstr.c_str());
			App::GetInstance()->m_read->childrenId = ShowChildID;
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", ShowChildID);
			string ShowChildHeadPortrait = YYXLayer::getFileValue("ShowChildHeadPortrait", "");
			//string ShowChildHeadPortrait = UserDefault::getInstance()->getStringForKey("ShowChildHeadPortrait", "");
			if (ShowChildHeadPortrait != "")
			{//有孩子ID有头像
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, ShowChildHeadPortrait);				
			}
			else
			{//有孩子ID没有头像
				getChildList(member_id, [=]() {});
			}
		}
		else//没孩子ID
			getDefaultHeadPortrait(member_id);
	}
}

void Login::getDefaultHeadPortrait(int member_id)
{
	//无记录情况下,默认获取第一个孩子的头像作为头像
	auto runkey = "loginScenehttpGetFirstChildHeadPortraitSuccess";
	//auto errorkey = "loginScenehttpGetFirstChildHeadPortraitFail";
	//auto DownLoadImageerrorkey = StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom());
	NetIntface::httpGetFirstChildHeadPortrait(member_id, runkey, [=](string json) {
		NetIntface::httpGetFirstChildHeadPortraitCallBack(json, [=](int id, string url) {
			App::log("获取到默认第一个孩子ID", id);
			App::GetInstance()->m_read->childrenId = id;
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", id);
			YYXLayer::setFileValue("ShowChildID", StringUtils::format("%d", id));
			NetIntface::DownLoadImage(url, FileUtils::getInstance()->getWritablePath() + "temp", StringUtils::format("3headportrait_%d.png", id),
				StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom()), [=](string path) {
				if (path != "")
				{
					string savePath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("childHead_%d.png", id);
					App::makeRoundImage(path, savePath);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, savePath);
					YYXLayer::setFileValue("ShowChildHeadPortrait", savePath);
				}
			}, "", [](string str) {});
		}, []() {});
	}, "", [](string str) {});
}

void Login::getChildList(int member_id, function<void()> downloadHeadPortrait)
{
	auto runkey = StringUtils::format("httpGetChildDetails%d", (int)YYXLayer::getRandom());
	NetIntface::httpGetChildDetails(member_id, runkey, [=](string json) {
		NetIntface::httpGetChildDetailsCallBack(json, [=](int index, int childrenId, int childrenSex, string childrenName, string childrenBirth, string url, long long uptime) {
			if (index == 0)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "firstChildID", childrenId);
			if (index == 1)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "secondChildID", childrenId);
			if (index == 2)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "threeChildID", childrenId);
			string namekey = StringUtils::format("name+childID=%d", childrenId);
			string pathkey = StringUtils::format("path+childID=%d", childrenId);
			string birthdaykey = StringUtils::format("birthday+childID=%d", childrenId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, namekey, childrenId, childrenName, nullptr);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathkey, uptime, "", nullptr);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, birthdaykey, childrenSex, childrenBirth, nullptr);
			string urlkey = StringUtils::format("url+childID=%d", childrenId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, urlkey, -999, url);
			//手动登录 全部重新下载 不判断是否存在头像
			NetIntface::DownLoadImage(url, FileUtils::getInstance()->getWritablePath() + "temp", StringUtils::format("9HeadPortrait_%d.png", childrenId),
				StringUtils::format("DownLoadImage%d", (int)YYXLayer::getRandom()), [=](string path) {
				if (path != "" && FileUtils::getInstance()->isFileExist(path))
				{
					string savePath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("childHead_%d.png", childrenId);
					App::makeRoundImage(path, savePath);
					string pathkey = StringUtils::format("path+childID=%d", childrenId);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathkey, YYXLayer::getCurrentTime4Second(), savePath);
					YYXLayer::sendNotify("IndexSceneReferShowHeadPortrait");
				}
			}, "", [](string str) {});
		}, [=](int b) {
			if (b == 1)
			{
				if (downloadHeadPortrait)
					downloadHeadPortrait();
			}
		});
	}, "", nullptr);
}

//获取有效红包
void Login::httpGetUserRedPackets(int memberId)
{
	string url = string(IP).append(NET_USERREDPACKET).append("?memberId=").append(StringUtils::format("%d", memberId));
	string runKey = "LoginSceneHttpGetUserRedPacketSuccess";
	//string errorKey = "BookInfoSceneHttpGetUserRedPacketFail";
	NetIntface::httpGet(url, runKey, [](string json) {
		NetIntface::httpGetUserRedPacketsCallBack(json, []() {
			App::GetInstance()->m_redPacket.clear();
		}, [](int coupon_id, int coupon_amount100, string coupon_expire_time) {
			if (coupon_id != -999 || coupon_amount100 != -99900) {
				map<string, YYXStruct> mapresult;
				YYXStruct::initMapYYXStruct(mapresult, "coupon_id", coupon_id);
				YYXStruct::initMapYYXStruct(mapresult, "coupon_amount", coupon_amount100);
				YYXStruct::initMapYYXStruct(mapresult, "coupon_expire_time", 0, coupon_expire_time);
				App::GetInstance()->m_redPacket.push_back(mapresult);
			}
		}, [](int expiring_coupon_count) {
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "expiring_coupon_count", expiring_coupon_count);
			//YYXLayer::sendNotify("referRedPackets", "BookInfo");
		}, []() {
			//Toast::create(App::getString("HONGBAOHUOQUSHIBAI"));
		});
	}, "", [](string str) {
		//Toast::create(App::getString("NETEXCEPTION"));
	});
}