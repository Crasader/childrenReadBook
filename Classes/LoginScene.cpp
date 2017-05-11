#include "LoginScene.h"
#include "NetIntface.h"
#include "XZLayer.h"
#include "YYXVisitor.h"
#include "Charger.h"
#include "YYXDownloadImages.h"
#include "User.h"
#include "AppHttp.h"

Scene* Login::createScene(SceneInfo *sceneInfo)
{
    auto scene = Scene::create();
    
    auto layer = Login::create(sceneInfo);

    scene->addChild(layer);
	layer->setTag(9);
    return scene;
}

Login* Login::create(SceneInfo* data /*= nullptr*/)
{
	Login *pRet = new(std::nothrow) Login();
	if (pRet && pRet->init(data))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool Login::init(SceneInfo *sceneInfo)
{
    if ( !Layer::init() )
    {
        return false;
    }

	this->addChild(loginInit());
    return true;
}

void Login::onEnterTransitionDidFinish()
{
	App::log("Login::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	App::log("Login::onEnterTransitionDidFinish---END");
}

void Login::cleanup()
{
	unschedule("LoginSuccessToast");
	unschedule("LoginSuccess");
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
			string xmlCode = YYXLayer::getFileValue("yanzhengCode", "");
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
			//注册过程
			AppHttp::getInstance()->httpAccountRegiste(name, password, [=](string hongbao) {
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
					YYXLayer::sendNotify("LoginScenehttpAccountRegisteOver");
					closeTime();
					auto control = ControlScene::getInstance();
					control->replaceScene(control->getCurrentScene(), control->getFromScene(), false);
					thread runable([=]() {
						App::ccsleep(1000);
						int count = __String(hongbao).intValue();
						if (count > 0)
							XZLayer::safe_ShowShareRedPacket(hongbao + App::getString("YUAN"));
					});
					runable.detach();
				});
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
			//App::GetInstance()->popBack();
			//Index::BackPreviousScene();
			auto control = ControlScene::getInstance();
			control->replaceScene(control->getCurrentScene(), control->getFromScene(),false);
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
		scheduleOnce([](float f) {
			Toast::create(App::getString("DENGLUCHENGGONG"));
		}, 2.0f, "LoginSuccessToast");
		scheduleOnce([](float f) {
			//验证成功,返回前场景
			auto control = ControlScene::getInstance();
			control->replaceScene(control->getCurrentScene(), control->getFromScene(),false);
		}, 5.0f, "LoginSuccess");
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenLoginSuccess, loginmessagebox);

	//通知: 登录失败
	_eventDispatcher->removeCustomEventListeners("USER_LOGIN_FAILED");
	auto listenLoginFailed = EventListenerCustom::create("USER_LOGIN_FAILED", [=](EventCustom* event) {
			//开启所有触发事件
		b_login->setTouchEnabled(true);
		b_login->setTitleText(App::getString("DENGLU"));
		YYXLayer::sendNotify(TAG_DELETEWAITLAYER);
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenLoginFailed, b_login);
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
			b_login->setTitleText(App::getString("DEMGLUZHONG"));
			AppHttp::getInstance()->httpLogIn(phone, _password);
			b_login->setTouchEnabled(false);
			Director::getInstance()->getRunningScene()->addChild(Index::WaitLayer());
		});
	});
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

//修改密码
void Login::forgetPassword(std::string memberName,std::string password, Node* forgetPasswordScene)
{
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
	request->release();
}

void Login::closeTime()
{
	unschedule("Codeinit");
	unschedule("Code");
}

Login::~Login()
{
	ControlScene::getInstance()->end();
}

////获取有效红包
//void Login::httpGetUserRedPackets(int memberId)
//{
//	string url = string(IP).append(NET_USERREDPACKET).append("?memberId=").append(StringUtils::format("%d", memberId));
//	string runKey = "LoginSceneHttpGetUserRedPacketSuccess";
//	NetIntface::httpGet(url, runKey, [](string json) {
//		NetIntface::httpGetUserRedPacketsCallBack(json, []() {
//			App::GetInstance()->m_redPacket.clear();
//		}, [](int coupon_id, int coupon_amount100, string coupon_expire_time) {
//			if (coupon_id != -999 || coupon_amount100 != -99900) {
//				map<string, YYXStruct> mapresult;
//				YYXStruct::initMapYYXStruct(mapresult, "coupon_id", coupon_id);
//				YYXStruct::initMapYYXStruct(mapresult, "coupon_amount", coupon_amount100);
//				YYXStruct::initMapYYXStruct(mapresult, "coupon_expire_time", 0, coupon_expire_time);
//				App::GetInstance()->m_redPacket.push_back(mapresult);
//			}
//		}, [](int expiring_coupon_count) {
//			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "expiring_coupon_count", expiring_coupon_count);
//		}, []() {
//		});
//	}, "", [](string str) {
//	});
//}