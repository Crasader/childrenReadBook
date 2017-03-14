#include "ParentScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "NetIntface.h"
#include "XZLayer.h"
#include "YYXVisitor.h"
#include "Charger.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#endif
USING_NS_CC;

using namespace cocostudio::timeline;

Parent::~Parent()
{
	//if (m_voids.find("momey") != m_voids.end())
	//{
		//auto m = (Text*)(m_voids["momey"]);
		//m->release();
	//}	
	App::log("Parent::~Parent()");
}

Scene* Parent::createScene()
{
    auto scene = Scene::create();
    
    auto layer = Parent::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool Parent::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
	 
	App::m_RunningScene = MySceneName::ParentScene;
	////安卓返回键
	//auto androidListener = EventListenerKeyboard::create();
	//androidListener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event) {
	//	if (!App::GetInstance()->isBack)
	//		return;
	//	switch (keyCode)
	//	{
	//	case EventKeyboard::KeyCode::KEY_ESCAPE:
	//		if (YYXLayer::getBoolFromXML(SOUND_KEY))
	//			YYXLayer::PLAYBUTTON;
	//		Index::GoToIndexScene();
	//		App::backThreading();
	//		break;
	//	default:
	//		break;
	//	}
	//};
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(androidListener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//整体场景
	Data data;
	if (App::GetInstance()->getData(PARENT_BACKGROUND_CSB, data))
		m_parentNode = CSLoader::createNode(data);
	else
		m_parentNode = CSLoader::createNode(PARENT_BACKGROUND_CSB);
	m_parentNode->setAnchorPoint(Vec2(0.5f, 0.5f));
	m_parentNode->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    addChild(m_parentNode);

	//--------------------------按钮--------------------------------------------------------
	//返回按钮
	auto homeButton = (Button*)m_parentNode->getChildByName(FIND_BUTTON_BY_NAME_HOME);
	homeButton->setPositionX((1094 - visibleSize.width) / 2 + 14);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		//返回首页
		Index::GoToIndexScene();
	});

	//账号信息按钮
	auto img_zhanghaoxinxi = (ImageView*)m_parentNode->getChildByName(FIND_IMAGEVIEW_BY_NAME_ACCOUNT);
	auto position = img_zhanghaoxinxi->getPosition();
	img_zhanghaoxinxi->setPosition(Vec2(homeButton->getPosition().x + 459*0.4, img_zhanghaoxinxi->getPosition().y));
	auto movex = img_zhanghaoxinxi->getPosition() - position;
	img_zhanghaoxinxi->setTag(1);
	img_zhanghaoxinxi->setTouchEnabled(false);

	//设置按钮
	auto img_shezhi = (ImageView*)m_parentNode->getChildByName(FIND_IMAGEVIEW_BY_NAME_SET);
	img_shezhi->setPosition(img_shezhi->getPosition() + movex);
	img_shezhi->setTag(2);
	img_shezhi->setTouchEnabled(true);

	//联系我们按钮
	auto img_lianxiwomen = (ImageView*)m_parentNode->getChildByName(FIND_IMAGEVIEW_BY_NAME_CONTACT);
	img_lianxiwomen->setPosition(img_lianxiwomen->getPosition() + movex);
	img_lianxiwomen->setTag(3);
	img_lianxiwomen->setTouchEnabled(true);
	
	//---------------------------界面--------------------------------------------------------------
	//场景初始化
	//string member_name = YYXStruct::getMapString(App::GetInstance()->myData, "userAccount", "");
	YYXVisitor::getInstance()->parentSceneInit([=](){
		App::cancelData();
		m_show = initNode_Login();
	}, [=]() {
		m_show = initNode_Account(); 
	});
	m_parentNode->addChild(m_show);

	//---------------------------按钮点击事件--------------------------------------------------------------------
	//每页的切换
	auto listenEvent = [=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		img_zhanghaoxinxi->loadTexture(PICTURE_PARENT_BUTTON_ACCOUNT_UNSEL, TextureResType::PLIST);
		img_shezhi->loadTexture(PICTURE_PARENT_BUTTON_SET_UNSEL, TextureResType::PLIST);
		img_lianxiwomen->loadTexture(PICTURE_PARENT_BUTTON_CONTACT_UNSEL, TextureResType::PLIST);
		if (!img_zhanghaoxinxi->isTouchEnabled()) {
			_eventDispatcher->removeCustomEventListeners("REFRESH_USER_INFO_OVER");
			_eventDispatcher->removeCustomEventListeners("MODIFY_USER_INFO_SUCCESS");
			_eventDispatcher->removeCustomEventListeners("MODIFY_USER_INFO_FAILED");
		}
		img_zhanghaoxinxi->setTouchEnabled(true);
		img_shezhi->setTouchEnabled(true);
		img_lianxiwomen->setTouchEnabled(true);
		m_show->removeFromParentAndCleanup(true);
		string member_name = YYXStruct::getMapString(App::GetInstance()->myData, "userAccount", "");
		switch (((ImageView*)sender)->getTag())
		{
		case 1:
			((ImageView*)sender)->loadTexture(PICTURE_PARENT_BUTTON_ACCOUNT_SEL, TextureResType::PLIST);
			//账号信息界面
			if (App::GetInstance()->m_me == nullptr || member_name == "")
			{
				App::cancelData();
				m_show = initNode_Login();
			}
			else
				m_show = initNode_Account();
			break;
		case 2:
			((ImageView*)sender)->loadTexture(PICTURE_PARENT_BUTTON_SET_SEL, TextureResType::PLIST);
			//设置界面
			m_show = initNode_Set();
			break;
		case 3:
			((ImageView*)sender)->loadTexture(PICTURE_PARENT_BUTTON_CONTACT_SEL, TextureResType::PLIST);
			//联系我们
			m_show = initNode_Contact();
			break;
		};
		((ImageView*)sender)->setTouchEnabled(false);
		m_parentNode->addChild(m_show);
	};
	img_zhanghaoxinxi->addClickEventListener(listenEvent);
	img_shezhi->addClickEventListener(listenEvent);
	img_lianxiwomen->addClickEventListener(listenEvent);

	//-----------------------监听事件----------------------------------
	thread([=]() {
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "LocalCacheSize", getLocalCacheSize()*100);
	}).detach();
	//网络请求余额
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("refershBalanceAndShowRedPacket", [=](EventCustom* sender) {
		getUserMoney();
		auto money = YYXStruct::getMapInt64(App::GetInstance()->myData, "showRedPacketMoney", -999);
		if (money != -999)
			XZLayer::showShareRedPacket(StringUtils::format("%d%s",money,App::getString("YUAN")));
	});
	/*Director::getInstance()->getEventDispatcher()->addCustomEventListener("httpBuyBookWithRedPacket_success", [=](EventCustom* e) {
		auto data = (YYXStruct*)e->getUserData();
		auto stringdata = YYXStruct::getStringData(*data, "");
		if (&stringdata && stringdata != "")
			httpGetRedpacketCallback(stringdata);
		if (data)
			delete data;
	});
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("httpBuyBookWithRedPacket_fail", [=](EventCustom* e) {
		auto data = (YYXStruct*)e->getUserData();
		Toast::create(App::getString("HONGBAOHUOQUSHIBAI"));
		if (data)
			delete data;
	});*/	
	//-----------------------监听事件END----------------------------------	
    return true;
}

void Parent::onEnterTransitionDidFinish()
{
	App::log("Parent::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	App::log("Parent::onEnterTransitionDidFinish---END");
}

//账号信息界面初始化
Layer* Parent::initNode_Account()
{
	//-------------------------------------------获取本地数据----------------------------------------------------
	/*vector<string> param;
	param.push_back("member_name");
	param.push_back("member_sex");
	param.push_back("memberProvince");
	param.push_back("memberCity");
	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_USERINFO, param, "where member_id=" + App::getMemberID());
	App::sqliteClose();*/

	//-------------------------------------------获取控件--------------------------------------------------------
	//图层
	Node* account;
	Data data;
	if (App::GetInstance()->getData(ACCOUNT_CSB, data))
		account = CSLoader::createNode(data);
	else
		account = CSLoader::createNode(ACCOUNT_CSB);	
	//账号文本
	auto account_text = (Text*)account->getChildByName(FIND_TEXT_BY_NAME_ACOUNT);
	//性别文本
	auto sex_text = (Text*)account->getChildByName(FIND_TEXT_BY_NAME_SEX);
	sex_text->setTag(SELECT);
	//奶爸控件
	auto boy_img = (ImageView*)account->getChildByName(FIND_IMAGEVIEW_BY_NAME_BOY);
	boy_img->setTouchEnabled(false);
	boy_img->setTag(SELECT);
	//辣妈控件
	auto girl_img = (ImageView*)account->getChildByName(FIND_IMAGEVIEW_BY_NAME_GIRL);
	girl_img->setTouchEnabled(false);
	girl_img->setTag(UNSELECT);
	//城市文本选择框
	auto city_text = (Text*)account->getChildByName(FIND_TEXT_BY_NAME_CITY);
	//城市选择框底图
	auto city_img = (Sprite*)account->getChildByName(FIND_SPRITE_BY_NAME_CITY);
	//城市选择框
	auto city_drop = DropBox::create(city_img->getPosition(), Size(430, 25), "Address.plist", "wryh.TTF", TextHAlignment::CENTER);
	account->addChild(city_drop);
	city_drop->setDropEnable(false);
	city_drop->setVisible(false);
	//编辑按钮
	auto edit_button = (Button*)account->getChildByName(FIND_BUTTON_BY_NAME_EDIT);
	edit_button->setTag(0);
	//完成按钮
	/*auto over_button = (Button*)account->getChildByName(FIND_BUTTON_BY_NAME_OVER);
	over_button->setTouchEnabled(false);*/
	//注销按钮
	auto logoff = (Button*)account->getChildByName(FIND_BUTTON_BY_NAME_LOGOFF);
	//修改密码按钮
	auto changepassword = (Button*)account->getChildByName(FIND_BUTTON_BY_NAME_CHANGEPASSWORD);
	//充值
	auto rechange = (Button*)account->getChildByName(FIND_BUTTON_BY_NAME_RECHARGE);
	//我的红包
	auto myRedPacket = (Button*)account->getChildByName(FIND_BUTTON_BY_NAME_PARENTSCENE_MYREDPACKET);
	//充值VIP
	auto VIPButton = (Button*)account->getChildByName(FIND_BUTTON_BY_NAME_PARENTSCENE_VIP);
	if (VIPButton) {
		if (App::GetInstance()->m_me->vip)
			VIPButton->setTitleText(App::getString("OPENVIPAGAIN"));
		else
			VIPButton->setTitleText(App::getString("OPENVIP"));
		VIPButton->addClickEventListener([=](Ref* sender) {
			if (App::GetInstance()->m_me->vip)
			{
				auto layers = XZLayer::showVIPRenew(nullptr);
				if (layers)
					addChild(layers, 9);
			}
			else {				
				auto layer = XZLayer::OpenVIPCardService(2);
				if (layer)
					addChild(layer, 9);
			}
		});
	}
	
	//余额
	auto momey = (Text*)account->getChildByName(FIND_TEXT_BY_NAME_RECHARGE);
	//momey->retain();
	//m_voids["momey"] = momey;
	//初始化余额
	int myMomey = -999;
	//vip
	auto viptext = (Text*)account->getChildByName("jibie_content");
	if (App::GetInstance()->m_me)
	{
		myMomey = App::GetInstance()->m_me->momey;
		if (viptext)
		{
			if (App::GetInstance()->m_me->vip)
				viptext->setText(App::getString("NIANKAFUWUJIEZHI") + App::GetInstance()->m_me->endvip);
		}
	}
	if (myMomey <= 0)
		myMomey = 0;
	string str = StringUtils::format("%.02f", myMomey/100.0) + App::getString("YUAN");
	if (momey)
		momey->setString(str);
	if (myRedPacket)
	{
		myRedPacket->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "ParentSceneClickmyRedPacketTime",[=]() {
				//获取用户红包
				//NetIntface::httpGetUserRedPackets(App::GetInstance()->m_me->id,"httpBuyBookWithRedPacket_success","httpBuyBookWithRedPacket_fail");
				showRedPacket();
				httpGetUserRedPacket();
			});
		});
	}
	//--------------------------------根据数据库信息初始化账号信息---------------------------------
	string member_name = YYXStruct::getMapString(App::GetInstance()->myData, "userAccount","");
	string userCity = YYXStruct::getMapString(App::GetInstance()->myData, "userCity", App::getString("UNKNOEPRICE"));
	string userProvince = YYXStruct::getMapString(App::GetInstance()->myData, "userProvince", App::getString("UNKNOEPRICE"));
	int userSex = YYXStruct::getMapInt64(App::GetInstance()->myData, "userSex", 0);
	if(account_text)
		account_text->setString(member_name.substr(0, 3) + "****" + member_name.substr(member_name.length() - 4, member_name.length() - 1));
	if (city_text && city_drop) {
		city_text->setString(userProvince + userCity);
		city_drop->setSiteText(userProvince, userCity);
	}
	//判断性别
	if (userSex == 1) {
		sex_text->setString(App::getString("SUPER_FATHER"));
		sex_text->setTag(SELECT);
		boy_img->setTag(SELECT);
		girl_img->setTag(UNSELECT);
		boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_SEL, TextureResType::PLIST);
		girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_UNSEL, TextureResType::PLIST);
	}
	else {
		sex_text->setString(App::getString("SUPER_MOTHER"));
		sex_text->setTag(UNSELECT);
		girl_img->setTag(SELECT);
		boy_img->setTag(UNSELECT);
		girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_SEL, TextureResType::PLIST);
		boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_UNSEL, TextureResType::PLIST);
	}

	//------------------------------------------点击事件----------------------------------------------------
	//性别选择 点击事件
	boy_img->addClickEventListener([=](Ref* sender) {

		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;

		if (boy_img->getTag() == SELECT) {
			return;
		}
		//sex_text->setString(App::getString("SUPER_FATHER"));
		boy_img->setTag(SELECT);
		girl_img->setTag(UNSELECT);
		boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_SEL, TextureResType::PLIST);
		girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_UNSEL, TextureResType::PLIST);
	});
	girl_img->addClickEventListener([=](Ref* sender) {

		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;

		//sex_text->setString(App::getString("SUPER_MOTHER"));
		if (girl_img->getTag() == SELECT) {
			return;
		}
		girl_img->setTag(SELECT);
		boy_img->setTag(UNSELECT);
		girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_SEL, TextureResType::PLIST);
		boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_UNSEL, TextureResType::PLIST);
	});
	//编辑按钮 点击事件
	edit_button->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		if (edit_button->getTag() == 1)
		{
			this->addChild(Index::WaitLayer(), 5, "waitLayer");
			int selectedSex = 1;
			if (girl_img->getTag() == SELECT) {
				selectedSex = 2;
			}
			vector<string> siteStr = city_drop->getSiteText();
			//updateInfo(selectedSex, siteStr[0], siteStr[1]);
			//修改用户信息
			NetIntface::httpAmendUserInfo(App::GetInstance()->m_me->id, selectedSex, siteStr[1], siteStr[0],
				StringUtils::format("httpAmendUserInfo%d", (int)YYXLayer::getRandom()), [=](string json) {
				NetIntface::httpAmendUserInfoCallBack(json, [=]() {
					//正确结果
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userSex", selectedSex);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userCity", -999, siteStr[1]);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userProvince", -999, siteStr[0]);
					//UserDefault::getInstance()->setStringForKey("userCity", siteStr[1]);
					//UserDefault::getInstance()->setStringForKey("userProvince", siteStr[0]);
					//UserDefault::getInstance()->setIntegerForKey("userSex", selectedSex);
					YYXLayer::setFileValue("userCity", siteStr[1]);
					YYXLayer::setFileValue("userProvince", siteStr[0]);
					YYXLayer::setFileValue("userSex", StringUtils::format("%d", selectedSex));
					Toast::create(App::getString("XIUGAIXINXICHENGGONG"));
					YYXLayer::sendNotify("MODIFY_USER_INFO_SUCCESS");
					YYXLayer::sendNotify("REFRESH_USER_INFO_OVER");
				}, []() {
					//解析错误
					Toast::create(App::getString("MODIFY_INFO_FAILED"));
					YYXLayer::sendNotify("MODIFY_USER_INFO_FAILED");
				});
			}, StringUtils::format("httpAmendUserInfo%d", (int)YYXLayer::getRandom()), [](string str) {
				//网络错误
				Toast::create(App::getString("NETEXCEPTION"));
			});
		}
		else {
			edit_button->setTag(1);
			edit_button->setTitleText(App::getString("WANCHENG"));
			//edit_button->setVisible(false);
			//edit_button->setTouchEnabled(false);
			//over_button->setVisible(true);
			//over_button->setTouchEnabled(true);
			sex_text->setVisible(false);
			boy_img->setVisible(true);
			boy_img->setTouchEnabled(true);
			girl_img->setVisible(true);
			girl_img->setTouchEnabled(true);
			city_text->setVisible(false);
			city_img->setVisible(true);
			city_drop->setVisible(true);
			city_drop->setDropEnable(true);
		}
	});
	//完成按钮 点击事件
	//over_button->addClickEventListener([=](Ref* sender) {
		//if (YYXLayer::getBoolFromXML(SOUND_KEY))
			//YYXLayer::PLAYBUTTON;
	//});

	auto visibleSize = Director::getInstance()->getVisibleSize();
	//注销按钮 点击事件
	logoff->addClickEventListener([=](Ref* sender) {

		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		//弹框确认
		auto messagebox = CSLoader::createNode(MESSAGEBOX_LOGOFF_YESORNO_CSB);
		messagebox->setAnchorPoint(Vec2(0.5f, 0.5f));
		messagebox->setPosition(visibleSize.width / 2, visibleSize.height / 2);
		this->addChild(messagebox);
		auto bgImg = (ImageView*)messagebox->getChildByTag(64);
		bgImg->setTouchEnabled(true);
		auto b_yes = (Button*)messagebox->getChildByName(FIND_BUTTON_BY_NAME_LOGOFF_YES);
		auto b_no = (Button*)messagebox->getChildByName(FIND_BUTTON_BY_NAME_LOGOFF_NO);
		//确认按钮 添加点击事件
		b_yes->addClickEventListener([=](Ref* sender) {
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			this->removeChild(messagebox);
			App::cancelData();
			// 删除当前图层,添加登陆图层
			m_parentNode->removeChild(m_show);
			m_show = initNode_Login();
			m_parentNode->addChild(m_show);	
			YYXVisitor::getInstance()->parentSceneLogout();
		});
		//取消按钮 添加点击事件
		b_no->addClickEventListener([=](Ref* sender) {
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;

			this->removeChild(messagebox);			
		});
	});
	//修改密码 点击事件
	changepassword->addClickEventListener([=](Ref* sender) {

		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;

		//弹框 修改密码
		auto changepasswordMessagebox = CSLoader::createNode(MESSAGEBOX_CHANGEPASSWORD_CSB);
		changepasswordMessagebox->setAnchorPoint(Vec2(0.5f, 0.5f));
		changepasswordMessagebox->setPosition(visibleSize.width / 2, visibleSize.height / 2);
		this->addChild(changepasswordMessagebox);

		auto bgImg = (ImageView*)changepasswordMessagebox->getChildByTag(55);
		bgImg->setTouchEnabled(true);

		////手机号输入框
		//auto inpit_phoneNum = (TextField*)changepasswordMessagebox->getChildByName(FIND_TEXTFIELD_BY_NAME_MODIFY_PHONENUM);
		////验证码输入框
		//auto inpit_code = (TextField*)changepasswordMessagebox->getChildByName(FIND_TEXTFIELD_BY_NAME_MODIFY_YANZHENGMA);
		////新密码输入框
		//auto inpit_newPassword = (TextField*)changepasswordMessagebox->getChildByName(FIND_TEXTFIELD_BY_NAME_MODIFY_PASSWORD);

		auto inpit_phoneNum = MyEditBox::create(Size(260, 45), App::getString("EDITBOX_PHONE"));
		inpit_phoneNum->setMyFontSize(20);
		inpit_phoneNum->setBoxMaxLength(11);
		inpit_phoneNum->setMyInputMode(cocos2d::ui::EditBox::InputMode::NUMERIC);
		inpit_phoneNum->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
		inpit_phoneNum->setPosition(Vec2(590, 343));
		changepasswordMessagebox->addChild(inpit_phoneNum);

		auto inpit_code = MyEditBox::create(Size(100, 45), App::getString("EDITBOX_SURE_CODE"));
		inpit_code->setMyFontSize(20);
		inpit_code->setBoxMaxLength(6);
		inpit_code->setMyInputMode(cocos2d::ui::EditBox::InputMode::ANY);
		inpit_code->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
		inpit_code->setPosition(Vec2(507, 281));
		changepasswordMessagebox->addChild(inpit_code);

		auto inpit_newPassword = MyEditBox::create(Size(260, 45), App::getString("EDITBOX_NEW_PASSWORD"));
		inpit_newPassword->setMyFontSize(20);
		inpit_newPassword->setBoxMaxLength(20);
		inpit_newPassword->setMyInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		inpit_newPassword->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
		inpit_newPassword->setPosition(Vec2(590, 218));
		changepasswordMessagebox->addChild(inpit_newPassword);
		
		//确认修改密码 点击事件
		auto b_change = (Button*)changepasswordMessagebox->getChildByName(FIND_BUTTON_BY_NAME_CHANGEPASSWORD_CHANGE);
		b_change->addClickEventListener([=](Ref* sender) {
			//if (!CocosAndroidJni::IsNetConnect())
			//{
			//	Toast::create(App::getString("NETEXCEPTION"));
			//	return;
			//}
			if (!NetIntface::IsNetConnect(true))
				return;
			//string xmlCode = "";
			//xmlCode = UserDefault::getInstance()->getStringForKey("yanzhengCode");
			int xmlCodeTime = 0;
			//xmlCodeTime = UserDefault::getInstance()->getIntegerForKey("codeTime");
			string xmlCode = YYXLayer::getFileValue("yanzhengCode", "");
			string xmlCodeTimestr = YYXLayer::getFileValue("codeTime","");
			xmlCodeTime = atoi(xmlCodeTimestr.c_str());
			auto movT = xmlCodeTime - (int)App::getCurrentTime();
			if (xmlCode == "" || movT > 1800)
			{
				Toast::create(App::getString("GET_CODE"));
				return;
			}
			auto code = inpit_code->getString();
			if (xmlCode != code)
			{
				Toast::create(App::getString("MESAAGEBOX_CODE_ERROR"));
				return;
			}
			auto userAccount = YYXStruct::getMapString(App::GetInstance()->myData, "userAccount", "");
			if (inpit_phoneNum->getString() != userAccount){
				Toast::create( App::getString("MESAAGEBOX_USERNAME_ERROR"));
				return;
			}
			if (code.length()!= 6){
				Toast::create( App::getString("MESAAGEBOX_CODE_ERROR"));
				return;
			}
			if (inpit_newPassword->getString().length() < 6){
				Toast::create( App::getString("MESAAGEBOX_PASSWORD_ERROR"));
				return;
			}
			unschedule("ModifyPsd_Code");
			modifyPassword(inpit_phoneNum->getString(), inpit_newPassword->getString());
			this->removeChild(changepasswordMessagebox);
		});
		//获取验证码 点击事件
		auto b_getcode = (Button*)changepasswordMessagebox->getChildByName(FIND_BUTTON_BY_NAME_CHANGEPASSWORD_GETCODE);
		m_times = 60;
		b_getcode->addClickEventListener([=](Ref* sender) {
			if (inpit_phoneNum->getString().length() != 11){
				Toast::create(App::getString("MESAAGEBOX_USERNAME_ERROR"));
				return;
			}
			getCode(inpit_phoneNum->getString());
			b_getcode->setTouchEnabled(false);
			b_getcode->setTitleText(StringUtils::format("%d%s", m_times, App::getString("seconds")));
			schedule([=](float f) {
				m_times--;
				b_getcode->setTitleText(StringUtils::format("%d%s", m_times, App::getString("seconds")));
				if (m_times == 0)
				{
					m_times = 60;
					b_getcode->setTouchEnabled(true);
					b_getcode->setTitleText(App::getString("GetCode"));
					unschedule("ModifyPsd_Code");
				}
			}, 1.0f, "ModifyPsd_Code");
		});
		//关闭按钮 点击事件
		auto b_close = (Button*)changepasswordMessagebox->getChildByName(FIND_BUTTON_BY_NAME_CHANGEPASSWORD_CLOSE);
		b_close->addClickEventListener([=](Ref* sender) {

			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;

			this->removeChild(changepasswordMessagebox);
			unschedule("ModifyPsd_Code");
		});
	});
	//充值 点击事件
	rechange->addClickEventListener([=](Ref* sender) {
		auto recharge = Charger::create();
		addChild(recharge);
	});
	
	//-----------------------------------------------通知-------------------------------------------------------
	//通知刷新用户信息
	auto listenRefreshUserInfo = EventListenerCustom::create("REFRESH_USER_INFO_OVER", [=](EventCustom* event) {
		string userCity = YYXStruct::getMapString(App::GetInstance()->myData, "userCity", App::getString("UNKNOEPRICE"));
		string userProvince = YYXStruct::getMapString(App::GetInstance()->myData, "userProvince", App::getString("UNKNOEPRICE"));
		int userSex = YYXStruct::getMapInt64(App::GetInstance()->myData, "userSex", 0);
		if (city_text && city_drop) {
			city_text->setString(userProvince + userCity);
			city_drop->setSiteText(userProvince, userCity);
		}
		//判断性别
		if (userSex == 1) {
			sex_text->setString(App::getString("SUPER_FATHER"));
			sex_text->setTag(SELECT);
			boy_img->setTag(SELECT);
			girl_img->setTag(UNSELECT);
			boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_SEL, TextureResType::PLIST);
			girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_UNSEL, TextureResType::PLIST);
		}
		else {
			sex_text->setString(App::getString("SUPER_MOTHER"));
			sex_text->setTag(UNSELECT);
			girl_img->setTag(SELECT);
			boy_img->setTag(UNSELECT);
			girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_SEL, TextureResType::PLIST);
			boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_UNSEL, TextureResType::PLIST);
		}
		/*CCLOG("--------------------------------------refresh");
		vector<string> param2;
		param2.push_back("member_name");
		param2.push_back("member_sex");
		param2.push_back("memberProvince");
		param2.push_back("memberCity");
		vector<unordered_map<string, ParaType>> result2 = SqliteManager::SelectData(App::sqliteOpen(), DB_USERINFO, param2, "where member_id=" + App::getMemberID());
		App::sqliteClose();
		if (result2.size() > 0) {
			//初始化账号--号码
			string acountStr = result2[0]["member_name"].stringPara;
			if (acountStr.length() >= 7) {
				account_text->setString(acountStr.substr(0, 3) + "****" + acountStr.substr(acountStr.length() - 4, acountStr.length() - 1));
			}
			//判断性别
			if (result2[0]["member_sex"].intPara == 1) {
				sex_text->setString(App::getString("SUPER_FATHER"));
				sex_text->setTag(SELECT);
				boy_img->setTag(SELECT);
				girl_img->setTag(UNSELECT);
				boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_SEL, TextureResType::PLIST);
				girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_UNSEL, TextureResType::PLIST);
			}
			else {
				sex_text->setString(App::getString("SUPER_MOTHER"));
				sex_text->setTag(UNSELECT);
				girl_img->setTag(SELECT);
				boy_img->setTag(UNSELECT);
				girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_SEL, TextureResType::PLIST);
				boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_UNSEL, TextureResType::PLIST);
			}
			//判断本地是否缓存地点信息
			string provinceStr = result2[0]["memberProvince"].stringPara;
			string cityStr = result2[0]["memberCity"].stringPara;
			if (provinceStr.length() > 0) {
				city_text->setString(provinceStr + cityStr);
				city_drop->setSiteText(provinceStr, cityStr);
			}
		}*/
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenRefreshUserInfo, account);

	//通知修改用户信息成功
	auto listenModifyUserInfoSuccess = EventListenerCustom::create("MODIFY_USER_INFO_SUCCESS", [=](EventCustom* event) {
		//edit_button->setVisible(true);
		//edit_button->setTouchEnabled(true);
		//over_button->setVisible(false);
		//over_button->setTouchEnabled(false);
		edit_button->setTitleText(App::getString("BIANJI"));
		edit_button->setTag(0);
		sex_text->setVisible(true);
		boy_img->setVisible(false);
		boy_img->setTouchEnabled(false);
		girl_img->setVisible(false);
		girl_img->setTouchEnabled(false);
		city_text->setVisible(true);
		city_img->setVisible(false);
		city_drop->setVisible(false);
		city_drop->setDropEnable(false);

		//删除waitlayer
		if (this->getChildByName("waitLayer") != nullptr) {
			this->removeChildByName("waitLayer");
		}

		if (boy_img->getTag() == SELECT) {
			sex_text->setString(App::getString("SUPER_FATHER"));
			sex_text->setTag(SELECT);
		}
		else {
			sex_text->setString(App::getString("SUPER_MOTHER"));
			sex_text->setTag(UNSELECT);
		}

		vector<string> siteStr = city_drop->getSiteText();
		city_text->setString(siteStr[0] + siteStr[1]);
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenModifyUserInfoSuccess, account);

	//通知修改用户信息失败
	auto listenModifyFailed = EventListenerCustom::create("MODIFY_USER_INFO_FAILED", [=](EventCustom* event) {
		//edit_button->setVisible(true);
		//edit_button->setTouchEnabled(true);
		//over_button->setVisible(false);
		//over_button->setTouchEnabled(false);
		edit_button->setTitleText(App::getString("BIANJI"));
		edit_button->setTag(0);
		sex_text->setVisible(true);
		boy_img->setVisible(false);
		boy_img->setTouchEnabled(false);
		girl_img->setVisible(false);
		girl_img->setTouchEnabled(false);
		city_text->setVisible(true);
		city_img->setVisible(false);
		city_drop->setVisible(false);
		city_drop->setDropEnable(false);

		//删除waitlayer
		if (this->getChildByName("waitLayer") != nullptr) {
			this->removeChildByName("waitLayer");
		}

		if (sex_text->getTag() == SELECT) {
			sex_text->setString(App::getString("SUPER_FATHER"));
			boy_img->setTag(SELECT);
			girl_img->setTag(UNSELECT);
			boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_SEL, TextureResType::PLIST);
			girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_UNSEL, TextureResType::PLIST);
		}
		else {
			sex_text->setString(App::getString("SUPER_MOTHER"));
			girl_img->setTag(SELECT);
			boy_img->setTag(UNSELECT);
			girl_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_GIRL_SEL, TextureResType::PLIST);
			boy_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_BOY_UNSEL, TextureResType::PLIST);
		}
		Toast::create( App::getString("MODIFY_INFO_FAILED"));
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenModifyFailed, account);
	//网络刷新余额
	getUserMoney();
	App::httpCheckVIP();
	//通知 展示余额
	auto listenshowyue = EventListenerCustom::create("ParentScenehttpGetUserBalanceReferBalance", [=](EventCustom* sender) {
		App::log("展示余额", App::GetInstance()->m_me->momey);
		int myMomey = 0;
		if (App::GetInstance()->m_me)
			myMomey = App::GetInstance()->m_me->momey;
		string str = StringUtils::format("%.02f", myMomey / 100.0) + App::getString("YUAN");
		if (momey) {
			momey->setText(str);
			//momey->setTextColor(Color4B::RED);
		}
		App::log("展示余额END", momey->getReferenceCount());
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenshowyue, momey);
	//通知 升级VIP
	auto listner = EventListenerCustom::create("refershMemberIDVIP", [=](EventCustom* e) {
		if (App::GetInstance()->m_me)
		{
			if (App::GetInstance()->m_me->vip)
			{
				if (VIPButton)
					VIPButton->setTitleText(App::getString("OPENVIPAGAIN"));
				if (viptext)
					viptext->setText(App::getString("NIANKAFUWUJIEZHI") + App::GetInstance()->m_me->endvip);
			}
			else
			{
				if (VIPButton)
					VIPButton->setTitleText(App::getString("OPENVIP"));
			}
		}
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listner, VIPButton);
	//网络请求用户信息可以不刷新,登录会自动获取到,刷新意义不大
	return (Layer*)account;
}
//联系我们界面的初始化
Layer* Parent::initNode_Contact()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	//图层
	Node* layer;
	Data data;
	if (App::GetInstance()->getData(CONTACT_CSB, data))
		layer = CSLoader::createNode(data);
	else
		layer = CSLoader::createNode(CONTACT_CSB);
	//ScrollView
	auto sv = (cocos2d::ui::ScrollView*)layer->getChildByName(FIND_ScrollView_BY_NAME);
	sv->setScrollBarEnabled(false);
	//关于咿啦看书文本
	auto aboutText = (Text*)sv->getChildByTag(65);
	auto gzText = (Text*)sv->getChildByTag(74);
	//保存二维码
	auto savetoQRcode = (Button*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_BUTTON_BY_NAME_SAVETOQRCODE);
	savetoQRcode->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON; 
		//将图片保存到相册
		CocosAndroidJni::saveQRcode();
	});
	//意见反馈
	auto feedback = (Button*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_BUTTON_BY_NAME_FEEDBACK);
	feedback->addClickEventListener([=](Ref* sender) {

		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;

		//弹框
		auto messagebox = CSLoader::createNode(MESSAGEBOX_FEEDBACK_CSB);
		messagebox->setAnchorPoint(Vec2(0.5f, 0.5f));
		messagebox->setPosition(visibleSize.width / 2, visibleSize.height / 2);
		this->addChild(messagebox);
		//imageview,屏蔽触摸
		auto background = (ImageView*)messagebox->getChildByName(FIND_IMAGEVIEW_BY_NAME_BACKGROUND);
		background->setTouchEnabled(true);
		background->setSwallowTouches(true);
		background->addTouchEventListener([=](cocos2d::Ref * sender, cocos2d::ui::Widget::TouchEventType type) {
			return true;
		});
		//输入框
		/*auto textFeed = (ui::TextField*)messagebox->getChildByName(FIND_TEXT_BY_NAME_FEEDBACK);
		auto textPhone = (ui::TextField*)messagebox->getChildByName(FIND_TEXT_BY_NAME_PHONE_NUM);*/

		auto textFeed = MyEditBox::create(Size(390, 170), App::getString("EDITBOX_FEEDBOX"));
		textFeed->setMyFontSize(20);
		textFeed->setMyInputMode(cocos2d::ui::EditBox::InputMode::ANY);
		textFeed->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
		textFeed->setPosition(Vec2(547, 315));
		messagebox->addChild(textFeed);

		auto textPhone = MyEditBox::create(Size(390, 45), App::getString("EDITBOX_PHONE"));
		textPhone->setMyFontSize(20);
		textPhone->setBoxMaxLength(11);
		textPhone->setMyInputMode(cocos2d::ui::EditBox::InputMode::NUMERIC);
		textPhone->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
		textPhone->setPosition(Vec2(548, 176));
		messagebox->addChild(textPhone);

		//反馈按钮
		auto feedback_button = (Button*)messagebox->getChildByName(FIND_BUTTON_BY_NAME_BACK);
		feedback_button->addClickEventListener([=](Ref* sender) {
			if (strlen(textFeed->getString().c_str()) == 0 || strlen(textPhone->getString().c_str()) == 0) {
				Toast::create( App::getString("FEEDMESSAGE_NULL"));
			}
			else if (strlen(textFeed->getString().c_str()) < 10) {
				Toast::create( App::getString("FEEDMESSAGE_FEEDLENGTH"));
			}
			else if (strlen(textPhone->getString().c_str()) != 11) {
				Toast::create(App::getString("MESAAGEBOX_USERNAME_ERROR"));
			}
			else
			{
				FeedBack(textFeed->getString(), textPhone->getString());
				this->removeChild(messagebox);
			}
		});
		//关闭按钮
		auto close = (Button*)messagebox->getChildByName(FIND_BUTTON_BY_NAME_CLOSE);
		close->addClickEventListener([=](Ref* sender) {

			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;

			this->removeChild(messagebox);
		});
	});
	//qq客服
	auto qq = (Button*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_BUTTON_BY_NAME_QQ2);
	qq->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		//弹框
		MessageBox(App::getString("QQKEFU"), "");
	});

	//qq版权合作
	auto qq2 = (Button*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_BUTTON_BY_NAME_QQ3);
	qq2->addClickEventListener([=](Ref* sender) {
		//弹框
		MessageBox(App::getString("BANBENHEZUO"), "");
	});

	//qq商务合作
	auto qq3 = (Button*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_BUTTON_BY_NAME_QQ);
	qq3->addClickEventListener([=](Ref* sender) {
		//弹框
		MessageBox(App::getString("SHAGNWUHEZUO"), "");
	});
	return (Layer *)layer;
}
//设置界面的初始化
Layer* Parent::initNode_Set()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	//图层
	Node* layer;
	Data data;
	if (App::GetInstance()->getData(SET_CSB, data))
		layer = CSLoader::createNode(data);
	else
		layer = CSLoader::createNode(SET_CSB);
	//ScrollView
	auto sv = (cocos2d::ui::ScrollView*)layer->getChildByName(FIND_ScrollView_BY_NAME);
	sv->setScrollBarEnabled(false);
	//版本号
	auto versionText = (Text*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_TEXT_PARENTSCENE_SET_VERSION);
	/*vector<string> paraStr;
	paraStr.push_back("appversionA");
	paraStr.push_back("appversionB");
	paraStr.push_back("appversionC");
	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_APPINFO, paraStr, "");
	App::sqliteClose;
	if (result.size() > 0) {
		//当前APP的版本号
		int A = result[0]["appversionA"].intPara;
		int B = result[0]["appversionB"].intPara;
		int C = result[0]["appversionC"].intPara;
		auto appVersion = StringUtils::format("%s%d.%d.%d", App::getString("BANBENHAO"),A, B, C);*/
	if(versionText)
		versionText->setText(NetIntface::getAppVersion());
	//}
	//设置时间的部分
	auto time0 = (ImageView*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_IMAGEVIEW_BY_NAME_NOTIME);
	time0->setTag(0);
	auto time20 = (ImageView*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_IMAGEVIEW_BY_NAME_20MIN);
	time20->setTag(20);
	auto time30 = (ImageView*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_IMAGEVIEW_BY_NAME_30MIN);;
	time30->setTag(30);
	auto time40 = (ImageView*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_IMAGEVIEW_BY_NAME_40MIN);;
	time40->setTag(40);
	auto time60 = (ImageView*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_IMAGEVIEW_BY_NAME_60MIN);;
	time60->setTag(60);
	time20->loadTexture(PICTURE_PARENT_IMAGEVIEW_20MIN_UNSEL, TextureResType::PLIST);
	time30->loadTexture(PICTURE_PARENT_IMAGEVIEW_30MIN_UNSEL, TextureResType::PLIST);
	time40->loadTexture(PICTURE_PARENT_IMAGEVIEW_40MIN_UNSEL, TextureResType::PLIST);
	time60->loadTexture(PICTURE_PARENT_IMAGEVIEW_60MIN_UNSEL, TextureResType::PLIST);
	//根据本地的设置信息初始化选中项
	if(App::GetInstance()->protectTime == 0)
		time0->loadTexture(PICTURE_PARENT_IMAGEVIEW_NOTIME_SEL, TextureResType::PLIST);
	else if(App::GetInstance()->protectTime == 20)
		time20->loadTexture(PICTURE_PARENT_IMAGEVIEW_20MIN_SEL, TextureResType::PLIST);
	else if (App::GetInstance()->protectTime == 30)
		time30->loadTexture(PICTURE_PARENT_IMAGEVIEW_30MIN_SEL, TextureResType::PLIST);
	else if (App::GetInstance()->protectTime == 40)
		time40->loadTexture(PICTURE_PARENT_IMAGEVIEW_40MIN_SEL, TextureResType::PLIST);
	else if (App::GetInstance()->protectTime == 60)
		time60->loadTexture(PICTURE_PARENT_IMAGEVIEW_60MIN_SEL, TextureResType::PLIST);
	//添加点击事件
	auto listen4Time = [=](Ref* sender) {

		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;

		time0->loadTexture(PICTURE_PARENT_IMAGEVIEW_NOTIME_UNSEL, TextureResType::PLIST);
		time20->loadTexture(PICTURE_PARENT_IMAGEVIEW_20MIN_UNSEL, TextureResType::PLIST);
		time30->loadTexture(PICTURE_PARENT_IMAGEVIEW_30MIN_UNSEL, TextureResType::PLIST);
		time40->loadTexture(PICTURE_PARENT_IMAGEVIEW_40MIN_UNSEL, TextureResType::PLIST);
		time60->loadTexture(PICTURE_PARENT_IMAGEVIEW_60MIN_UNSEL, TextureResType::PLIST);
		switch (((ImageView*)sender)->getTag())
		{
		case 0:
			time0->loadTexture(PICTURE_PARENT_IMAGEVIEW_NOTIME_SEL,TextureResType::PLIST);
			//UserDefault::getInstance()->setIntegerForKey("PROTECT_TIME", 0);
			YYXLayer::setFileValue("PROTECT_TIME", "0");
			App::GetInstance()->protectTime = 0;
			break;
		case 20:
			time20->loadTexture(PICTURE_PARENT_IMAGEVIEW_20MIN_SEL, TextureResType::PLIST);
			//UserDefault::getInstance()->setIntegerForKey("PROTECT_TIME", 20);
			YYXLayer::setFileValue("PROTECT_TIME", "20");
			App::GetInstance()->protectTime = 20;
			break;
		case 30:
			time30->loadTexture(PICTURE_PARENT_IMAGEVIEW_30MIN_SEL, TextureResType::PLIST);
			//UserDefault::getInstance()->setIntegerForKey("PROTECT_TIME", 30);
			YYXLayer::setFileValue("PROTECT_TIME", "30");
			App::GetInstance()->protectTime = 30;
			break;
		case 40:
			time40->loadTexture(PICTURE_PARENT_IMAGEVIEW_40MIN_SEL, TextureResType::PLIST);
			//UserDefault::getInstance()->setIntegerForKey("PROTECT_TIME", 40);
			YYXLayer::setFileValue("PROTECT_TIME", "40");
			App::GetInstance()->protectTime = 40;
			break;
		case 60:
			time60->loadTexture(PICTURE_PARENT_IMAGEVIEW_60MIN_SEL, TextureResType::PLIST);
			//UserDefault::getInstance()->setIntegerForKey("PROTECT_TIME", 60);
			YYXLayer::setFileValue("PROTECT_TIME", "60");
			App::GetInstance()->protectTime = 60;
			break;
		}
		//CCLOG("------------protectedTime: %d", App::GetInstance()->protectTime);
	};
	time0->setTouchEnabled(true);
	time20->setTouchEnabled(true);
	time30->setTouchEnabled(true);
	time40->setTouchEnabled(true);
	time60->setTouchEnabled(true);
	time0->addClickEventListener(listen4Time);
	time20->addClickEventListener(listen4Time);
	time30->addClickEventListener(listen4Time);
	time40->addClickEventListener(listen4Time);
	time60->addClickEventListener(listen4Time);
	//下载设置
	auto wifi_img = (ImageView*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_IMAGEVIEW_BY_NAME_WIFI);
	auto wifi_text = (Text*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_TEXT_BY_NAME_WIFI);
	wifi_img->setTouchEnabled(true);
	//根据本地保存设置初始化选择
	if (App::GetInstance()->isOnlyWifi) {
		wifi_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_WIFI_SEL, TextureResType::PLIST);
		wifi_text->setString(App::getString("JINWIFIXIAZAI"));
		wifi_img->setTag(SELECT);
	}
	else
	{
		wifi_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_WIFI_UNSEL, TextureResType::PLIST);
		wifi_text->setString(App::getString("4GHEWIFIXIAZAI"));
		wifi_img->setTag(UNSELECT);
	}
	//下载设置点击事件
	wifi_img->addClickEventListener([=](Ref *sender) {

		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;

		if (wifi_img->getTag() == SELECT)
		{
			//4G和wifi
			wifi_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_WIFI_UNSEL, TextureResType::PLIST);
			wifi_text->setString(App::getString("4GHEWIFIXIAZAI"));
			wifi_img->setTag(UNSELECT);
			//UserDefault::getInstance()->setIntegerForKey("IS_ONLY_WIFI", 1);
			YYXLayer::setFileValue("IS_ONLY_WIFI", "1");
			App::GetInstance()->isOnlyWifi = false;
		}
		else
		{
			//仅wifi
			wifi_img->loadTexture(PICTURE_PARENT_IMAGEVIEW_WIFI_SEL, TextureResType::PLIST);
			wifi_text->setString(App::getString("JINWIFIXIAZAI"));
			wifi_img->setTag(SELECT);
			//UserDefault::getInstance()->setIntegerForKey("IS_ONLY_WIFI", 0);
			YYXLayer::setFileValue("IS_ONLY_WIFI", "0");
			App::GetInstance()->isOnlyWifi = true;
		}
		if (App::GetInstance()->isOnlyWifi)
		{
			App::log("---------------isOnlyWifi");
		}
		else
		{
			App::log("---------------3G/4G/Wifi");
		}
	});
	//清除缓存
	auto wipecache_text = (Text*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_TEXT_BY_NAME_WIPECACHE);
	int count = (int)YYXStruct::getMapInt64(App::GetInstance()->myData, "LocalCacheSize", 0);
	wipecache_text->setString(StringUtils::format("%.02fM", count/100.0));// getLocalCacheSize()
	auto wipecache_button = (Button*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_BUTTON_BY_NAME_WIPECACHE);
	wipecache_button->setTouchEnabled(true);
	wipecache_button->addClickEventListener([=](Ref *sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		//弹框确认
		auto messagebox = CSLoader::createNode(MESSAGEBOX_WIPECACHE_CSB);
		messagebox->setAnchorPoint(Vec2(0.5f, 0.5f));
		messagebox->setPosition(visibleSize.width / 2, visibleSize.height / 2);
		this->addChild(messagebox);
		auto background = (ImageView*)messagebox->getChildByName(FIND_IMAGEVIEW_BY_NAME_BACKGROUND);
		background->setTouchEnabled(true);
		background->setSwallowTouches(true);
		background->addTouchEventListener([=](Ref* sender,cocos2d::ui::Widget::TouchEventType type) {
			return true;
		});
		auto yes = (Button*)messagebox->getChildByName(FIND_BUTTON_BY_NAME_YES);
		yes->addClickEventListener([=](Ref* sender) {
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			//清除缓存
			std::thread([=]() {
				deleteCache();
			}).detach();
			this->removeChild(messagebox);
		});
		auto no = (Button*)messagebox->getChildByName(FIND_BUTTON_BY_NAME_NO);		
		no->addClickEventListener([=](Ref* sender) {

			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			this->removeChild(messagebox);
		});
	});
	//邀请注册
	auto invite = (Button*)cocos2d::ui::Helper::seekWidgetByName(sv, "Button_1");
	if (invite)
	{
		invite->addClickEventListener([](Ref* sender) {
			YYXLayer::controlTouchTime(1, "yaoqingzhuceTime", []() {
				if (App::GetInstance()->m_me)
				{
					string url = string("http://ellabook.cn/ellaBook-invite-red/index.html?memberId=").append(App::getMemberID());
					NetIntface::inviteRegister(App::GetInstance()->m_me->id, url, "", [](string json) {}, "", [](string str) {});
				}
				else
					Toast::create(App::getString("STR_YOUDONTLOGIN"));
			});
		});
	}
	return (Layer *)layer;
}
//登陆界面的初始化
Layer* Parent::initNode_Login()
{
	Node* layer;
	Data data;
	if (App::GetInstance()->getData(LOGIN_CSB, data))
		layer = CSLoader::createNode(data);
	else
		layer = CSLoader::createNode(LOGIN_CSB);
	auto login = (Button*)layer->getChildByName(FIND_BUTTON_BY_NAME_LOGIN);
	login->addClickEventListener([=](Ref* sender){

		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;

		//跳转登陆
		App::GetInstance()->pushScene(ParentScene);
		Index::GoToLoginScene();
	});
	return (Layer *)layer;
}

//预加载
//void Parent::prestrain()
//{
//	auto cache = SpriteFrameCache::getInstance();
//	cache->addSpriteFramesWithFile(PLIST_PARENT1);
//	cache->addSpriteFramesWithFile(PLIST_PARENT2);
//}

//获取本地缓存大小
float Parent::getLocalCacheSize() {
	vector<string> param;
	param.push_back("*");

	float size = 0;//缓存总大小
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "temp");
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "bookCover");
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "bookUNZip");
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "bookZip");
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "bookTryReadunZip");
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "bookTryReadZip");
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "readBook");
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "downloadBook");
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "bookCity");
	size += getDirectorySize(FileUtils::getInstance()->getWritablePath() + "voiceComment");
	size = size / 1024.0f / 1024.0f;//以M为单位

	if (size < 0)
		size = 0;
	
	return size;
}

//获取文件夹大小
long Parent::getDirectorySize(string filePath) {
	long file_size = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_finddata_t FileInfo;
	string strfind = filePath + "/*";

	long Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L) {
		//找不到改路径下文件夹
		return 0;
	}
	do {
		//判断是否有子目录
		if (FileInfo.attrib & _A_SUBDIR)
		{
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				//文件夹-递归
				string newPath = filePath + "/" + FileInfo.name;
				file_size += getDirectorySize(newPath);
			}
		}
		else
		{
			//文件
			string filename = (filePath + "/" + FileInfo.name);
			file_size += FileUtils::getInstance()->getFileSize(filename);
		}
	} while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);

	//最后返回所有文件大小的总和
	return file_size;
#else
	struct dirent* ent = NULL;
	DIR *pDir;
	pDir = opendir(filePath.c_str());
	char FileName[512];
	char FolderName[512];
	strcpy(FolderName, filePath.c_str());
	strcat(FolderName, "/%s");
	if (pDir == NULL) {
		return 0;
	}
	while (NULL != (ent = readdir(pDir))) {
		if (ent->d_type == 8) {
			//文件
			sprintf(FileName, FolderName, ent->d_name);
			file_size += FileUtils::getInstance()->getFileSize(string(FileName));
		}
		else {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
				continue;
			}
			//文件夹-递归
			sprintf(FileName, FolderName, ent->d_name);
			file_size += getDirectorySize(string(FileName));
		}
	}
	//最后返回所有文件大小的总和
	return file_size;
#endif // (CC_PLATFORM_ANDROID == CC_TARGET_PLATFORM)
}

//清空缓存
void Parent::deleteCache() {
	//清空temp文件夹
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "temp");
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "bookCover");
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "bookUNZip");
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "bookZip");
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "bookTryReadunZip");
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "bookTryReadZip");
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "readBook");
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "downloadBook");
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "bookCity");
	DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "voiceComment");
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "LocalCacheSize", getLocalCacheSize() * 100);
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		auto sv = (cocos2d::ui::ScrollView*)m_show->getChildByName(FIND_ScrollView_BY_NAME);
		auto wipecache_text = (Text*)cocos2d::ui::Helper::seekWidgetByName(sv, FIND_TEXT_BY_NAME_WIPECACHE);
		wipecache_text->setString(StringUtils::format("%.1fM", getLocalCacheSize()));
	});
}

//删除文件夹
void Parent::DeleteDirectory(string filePath)
{
	App::log("delete Dir = " + filePath);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	_finddata_t FileInfo;
	string strfind = filePath + "/*";

	long Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L) {
		//CCLOG("can not match the folder path");
		return;
	}
	do {
		//判断是否有子目录
		if (FileInfo.attrib & _A_SUBDIR)
		{
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
			{
				//文件夹-递归
				string newPath = filePath + "/" + FileInfo.name;
				DeleteDirectory(newPath);
			}
		}
		else
		{
			//文件-删除
			string filename = (filePath + "/" + FileInfo.name);
			remove(filename.c_str());
		}
	} while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);

	//最后删除空文件夹
	rmdir(filePath.c_str());
#else
	struct dirent* ent = NULL;
	DIR *pDir;
	pDir = opendir(filePath.c_str());
	char FileName[512];
	char FolderName[512];
	strcpy(FolderName, filePath.c_str());
	strcat(FolderName, "/%s");
	if (pDir == NULL) {
		return;
	}
	while (NULL != (ent = readdir(pDir))) {
		if (ent->d_type == 8) {
			//文件-删除
			sprintf(FileName, FolderName, ent->d_name);
			remove(FileName);
		}
		else {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
				continue;
			}
			//文件夹-递归
			sprintf(FileName, FolderName, ent->d_name);
			DeleteDirectory(FileName);
		}
	}
	//删除空文件夹
	rmdir(filePath.c_str());
#endif // (CC_PLATFORM_ANDROID == CC_TARGET_PLATFORM)
	if (!FileUtils::getInstance()->isDirectoryExist(filePath))
	{
		FileUtils::getInstance()->createDirectory(filePath);
	}
}

//意见反馈
void Parent::FeedBack(string feed, string num) {
	string url = std::string(IP).append(NET_FEEDBACKSAVE);
	map<string, string> p;
	p["memberId"] = App::getMemberID();
	p["feedbackMember_name"] = num;
	p["feedbackContent"] = num;
	p["feedbackMember_name"] = feed;
	p["feedbackAdd_time"] = App::GetStringTime();
	p["resource"] = App::m_resource;
	NetIntface::httpPost(url, p, "", [](string json) {
		Toast::create(App::getString("FEEDBACK_INFO"));
	}, "", [](string str) {});
	//CCLOG("%s----------%s", feed.c_str(), num.c_str());
	//HttpRequest* request = new HttpRequest();
	//request->setRequestType(HttpRequest::Type::POST);
	//request->setUrl(std::string(IP).append(NET_FEEDBACKSAVE).c_str());

	//auto postData = std::string("memberId=").append(App::getMemberID()).append("&feedbackMember_name=").append(num).append
	//	("&feedbackContent=").append(feed).append("&feedbackAdd_time=").append(App::GetStringTime()).append
	//	("&resource=").append(App::m_resource);
	//request->setRequestData(postData.c_str(), strlen(postData.c_str()));

	////CCLOG(postData.c_str());

	////请求的回调函数
	//request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
	//	if (!response)
	//	{
	//		return;
	//	}
	//	if (!response->isSucceed())
	//	{
	//		log("response failed! error buffer: %s", response->getErrorBuffer());
	//		return;
	//	}
	//	else
	//	{
	//		//CCLOG("feedback successful");
	//		MessageBox(App::getString("FEEDBACK_INFO"), App::getString("FEEDBACK_TITLE"));
	//	}
	//});
	//cocos2d::network::HttpClient::getInstance()->send(request);
	//cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(3);
	//request->release();
}

//网络请求修改用户信息
/*void Parent::updateInfo(int sex, string site1, string site2) {
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_MODIFY_CITY).c_str());

	auto postData = std::string("memberId=").append(App::getMemberID()).append("&sex=").append(StringUtils::format("%d", sex)).append
	("&city=").append(site2).append("&province=").append(site1);
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));

	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		if (!response){
			notifyModifyFailed();
			return;
		}
		if (!response->isSucceed()){
			notifyModifyFailed();
			log("response failed! error buffer: %s", response->getErrorBuffer());
			return;
		}

		//json解析
		std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
		thread callback(&Parent::updateInfoCallback, this, str, sex, site1, site2);
		callback.detach();
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(3);
	request->release();
}
void Parent::updateInfoCallback(string str, int sex, string site1, string site2) {
	rapidjson::Document doc;
	doc.Parse<0>(str.c_str());
	if (doc.HasParseError()) {
		notifyModifyFailed();
		return;
	}

	if (!doc.IsObject() || !doc.HasMember("code")) {
		notifyModifyFailed();
		return;
	}
	rapidjson::Value &value = doc["code"];
	if (value.IsInt() && value.GetInt() == 0) {//请求修改正常
		unordered_map<string, string> paraInfoSTR;
		unordered_map<string, long long> paraInfoINT;
		paraInfoINT["member_sex"] = sex;
		paraInfoINT["upTime"] = App::getCurrentTime();
		paraInfoSTR["memberProvince"] = site1;
		paraInfoSTR["memberCity"] = site2;
		SqliteManager::UpdateData(App::sqliteOpen(), DB_USERINFO, paraInfoSTR, paraInfoINT, "where member_id=" + App::getMemberID());
		App::sqliteClose();
		notifyModifySuccess();
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
			Director::getInstance()->getEventDispatcher()->setEnabled(true);
			EventCustom eventModifySuccess("REFRESH_USER_INFO_OVER");
			Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventModifySuccess);
		});
	}
	else{
		notifyModifyFailed();
	}
}*/

//通知修改用户信息成功
/*void Parent::notifyModifySuccess() {
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		if (!Director::getInstance()->getEventDispatcher()->isEnabled()) {
			Director::getInstance()->getEventDispatcher()->setEnabled(true);
		}
		EventCustom eventModifySuccess("MODIFY_USER_INFO_SUCCESS");
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventModifySuccess);
	});
}

//通知修改用户信息失败
void Parent::notifyModifyFailed() {
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		if (!Director::getInstance()->getEventDispatcher()->isEnabled()) {
			Director::getInstance()->getEventDispatcher()->setEnabled(true);
		}
		EventCustom eventModifyFailed("MODIFY_USER_INFO_FAILED");
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventModifyFailed);
	});
}*/

//获取修改密码的验证码
void Parent::getCode(std::string account)
{
	App::log(StringUtils::format("code--------------%s", account.c_str()));
	string url = string(IP).append(NET_GETCODE);
	map<string, string> p;
	p["mobileNum"] = account;
	p["type"] = "2";
	p["resource"] = App::m_resource;
	NetIntface::httpPost(url, p, "parentSceneGetCodeSuccess", [](string json) {
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			auto code = YYXLayer::getIntForJson(1, doc, "code");
			if (code == 0)
			{
				auto num = YYXLayer::getStringForJson("", doc, "num");
				YYXLayer::setFileValue("yanzhengCode", num);
				YYXLayer::setFileValue("codeTime", StringUtils::format("%d", (int)YYXLayer::getCurrentTime4Second()));
				Toast::create(App::getString("MESAAGEBOX_GETCODE_SUCCESS"));
				App::startTime();
				return;
			}
			else
			{
				auto desc = YYXLayer::getStringForJson("", doc, "desc");
				Toast::create(desc.c_str());
				return;
			}
		}
		Toast::create(App::getString("YANZHENGMAQINGQIUSHIBAIQINGCHONGSHI"));
	}, "parentSceneGetCodeFail", [](string str) {
		Toast::create(App::getString("YANZHENGMAQINGQIUSHIBAIQINGCHONGSHI"));
	});
	//if (!CocosAndroidJni::IsNetConnect())
	//{
	//	Toast::create(App::getString("NETEXCEPTION"));
	//	return;
	//}
	//if (!NetIntface::IsNetConnect(true))
	//	return;
	//Toast::create(App::getString("GetCode"));
	//HttpRequest* request = new HttpRequest();
	//request->setRequestType(HttpRequest::Type::POST);
	//request->setUrl(std::string(IP).append(NET_GETCODE).c_str());
	//auto postData = std::string("mobileNum=").append(account).append("&type=2")
	//	.append("&resource=").append(App::m_resource);
	//request->setRequestData(postData.c_str(), strlen(postData.c_str()));
	//App::log(std::string(IP).append(NET_GETCODE).append("\n").append(postData).c_str());
	////请求的回调函数
	//request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
	//	std::string str4result(App::getString("MESAAGEBOX_GETCODE_SUCCESS"));
	//	if (!response)
	//	{
	//		return;
	//	}
	//	if (!response->isSucceed())
	//	{
	//		log("response failed");
	//		log("error buffer: %s", response->getErrorBuffer());
	//		return;
	//	}
	//	//解析json
	//	//{"num":"480473","desc":"success","mobileNum":"18888702717","code":0}
	//	//{"desc":"mobilenum has registed：18888702717", "code" : 10003}
	//	std::vector<char> *buffer = response->getResponseData();
	//	std::string str(buffer->begin(), buffer->end());
	//	rapidjson::Document doc;
	//	doc.Parse<0>(str.c_str());
	//	if (doc.HasParseError())
	//	{
	//		//CCLOG("json parse error : %s", doc.GetParseError());
	//	}
	//	else
	//	{
	//		if (doc.IsObject() && doc.HasMember("code"))
	//		{
	//			rapidjson::Value &code = doc["code"];
	//			if (code.IsInt() && code.GetInt() == 0)
	//			{
	//				//验证码发送成功
	//				if (doc.HasMember("num"))
	//				{
	//					rapidjson::Value &num = doc["num"];
	//					if (num.IsString())
	//					{
	//						string code = num.GetString();
	//						if (code.length() == 6)
	//						{
	//							//UserDefault::getInstance()->setStringForKey("yanzhengCode", code);
	//							//UserDefault::getInstance()->setIntegerForKey("codeTime", (int)App::getCurrentTime());
	//							YYXLayer::setFileValue("yanzhengCode", code);
	//							YYXLayer::setFileValue("codeTime", StringUtils::format("%d",(int)YYXLayer::getCurrentTime4Second()));
	//						}
	//					}
	//				}
	//				Toast::create(App::getString("MESAAGEBOX_GETCODE_SUCCESS"));
	//				App::startTime();
	//			}
	//			else
	//			{
	//				//验证码发送失败
	//				rapidjson::Value &desc = doc["desc"];
	//				Toast::create( desc.GetString());
	//			}
	//		}
	//	}
	//});
	//cocos2d::network::HttpClient::getInstance()->send(request);
	//request->release();
}

//用户修改密码
void Parent::modifyPassword(string acount, string newPsd) {
	string url = std::string(IP).append(NET_MODIFY_PASSWORD);
	map<string, string> p;
	p["memberName"] = acount;
	p["password"] = newPsd;
	NetIntface::httpPost(url, p, "parentSceneModifyPasswordSuccess", [](string json) {
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			auto code = YYXLayer::getIntForJson(1, doc, "code");
			if (code == 0)
			{
				Toast::create(App::getString("MESAAGEBOX_FORGETPASSWORD_SUCCESS"));
				return;
			}
		}
		Toast::create(App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR"));
	}, "parentSceneModifyPasswordFail", [](string str) {
		Toast::create(App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR"));
	});
	//HttpRequest* request = new HttpRequest();
	//request->setRequestType(HttpRequest::Type::POST);
	//request->setUrl(std::string(IP).append(NET_MODIFY_PASSWORD).c_str());

	//auto postData = std::string("memberName=").append(acount).append("&password=").append(newPsd);
	//request->setRequestData(postData.c_str(), strlen(postData.c_str()));

	////CCLOG(postData.c_str());

	////请求的回调函数
	//request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
	//	if (!response)
	//	{
	//		Toast::create( App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR"));
	//		return;
	//	}
	//	if (!response->isSucceed())
	//	{
	//		Toast::create( App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR"));
	//		log("response failed! error buffer: %s", response->getErrorBuffer());
	//		return;
	//	}

	//	//json解析
	//	std::string str(response->getResponseData()->begin(), response->getResponseData()->end());
	//	rapidjson::Document doc;
	//	doc.Parse<0>(str.c_str());
	//	if (doc.HasParseError()) {//json parse error
	//		Toast::create( App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR"));
	//		return;
	//	}

	//	if (!doc.IsObject() || !doc.HasMember("code")) {
	//		Toast::create( App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR"));
	//		return;
	//	}
	//	rapidjson::Value &value = doc["code"];
	//	if (value.IsInt() && value.GetInt() == 0) {//请求修改正常
	//		Toast::create( App::getString("MESAAGEBOX_FORGETPASSWORD_SUCCESS"));
	//		unordered_map<string, string> paraInfoSTR;
	//		unordered_map<string, long long> paraInfoINT;
	//		paraInfoSTR["member_passwd"] = newPsd;
	//		//SqliteManager::UpdateData(App::sqliteOpen(), DB_USERINFO, paraInfoSTR, paraInfoINT, "where member_name=" + acount);
	//		//App::sqliteClose();
	//	}
	//	else{
	//		Toast::create(App::getString("MESAAGEBOX_FORGETPASSWORD_ERROR"));
	//	}
	//});
	//cocos2d::network::HttpClient::getInstance()->send(request);
	//cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(3);
	//request->release();
}

//网络请求账户余额
void Parent::getUserMoney()
{
	string runKey = "ParentSceneHttpGetUserBalanceSuccess";
	string errorKey = "ParentSceneHttpGetUserBalanceFail";
	NetIntface::httpGetUserBalance(App::GetInstance()->m_me->id, runKey, [](string json) {
		//解析json 
		//写入内存和xml
		NetIntface::httpGetUserBalanceCallBack(json, [](int id, int userBalance, long long uptime) {
			//正确解析
			if (App::GetInstance()->m_me && userBalance >= 0)
			{
				App::GetInstance()->m_me->momey = userBalance;
				//UserDefault::getInstance()->setIntegerForKey("userBalance", userBalance);
				YYXLayer::setFileValue("userBalance", StringUtils::format("%d", userBalance));
				YYXLayer::sendNotify("ParentScenehttpGetUserBalanceReferBalance");
			}
		}, []() {
			//返回错误,或者解析错误
		});
	}, errorKey, [](string str) {
		//网络错误
		Toast::create(App::getString("NETEXCEPTION"));
	});
//	//发出请求,回调之后会发送showyue的通知,showyue事件(获取app里的数据然后更新到界面)
//	CocosAndroidJni::GetUserBalance(memberid.c_str(), App::GetInstance()->m_resource.c_str());
//	////string url = "http://cloud.ellabook.cn/ellabook-server/order/queryMemberAmount.do?memberId=4030";
//	//string url = string(IP).append(NET_GETRECHARGE).append("?memberId=").append(memberid).append("&resource=").append(App::m_resource);
//	//map<string, string> par;
//	//stringHttpRequest(HttpRequest::Type::GET, url, par, 0, [=](string json) {
//	//	rapidjson::Document doc;
//	//	if (YYXLayer::getJsonObject4Json(doc, json))
//	//	{
//	//		if (YYXLayer::getBoolForJson(false, doc, "result"))
//	//		{
//	//			auto recharge = YYXLayer::getDouble4Json(-1, doc, "data","balance_amount");
//	//			if (recharge == -1)
//	//				App::log("余额不需要更新");
//	//			else
//	//			{
//	//				App::log("更新余额");
//	//				if (App::GetInstance()->m_me)
//	//					App::GetInstance()->m_me->momey = recharge;
//	//				Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
//	//					Director::getInstance()->getEventDispatcher()->setEnabled(true);
//	//					EventCustom event("showyue");
//	//					Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
//	//				});
//	//			}
//	//		}
//	//	}
//	//}, []() {
//	//	Toast::create("余额获取失败");
//	//});
}

//void Parent::stringHttpRequest(HttpRequest::Type type, string url, map<string, string> paramter, int outTime, std::function<void(string json)> runable, std::function<void()> errorRunable)
//{
//	if (!&url || !&paramter)
//	{
//		App::log("参数异常");
//		return;
//	}
//	//if (!CocosAndroidJni::IsNetConnect())
//	//{
//	//	Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
//	//		Toast::create(App::getString("NETEXCEPTION"));
//	//	});
//	//	return;
//	//}
//	if (!NetIntface::IsNetConnect(true))
//		return;
//	HttpRequest* request = new HttpRequest();
//	request->setRequestType(type);
//	request->setUrl(url.c_str());
//	//CCLOG("%s", url.c_str());
//	App::log(url);
//	if (type == HttpRequest::Type::POST)
//	{
//		string postData;
//		if (paramter.size() > 0)
//		{
//			for (auto it : paramter)
//			{
//				auto key = it.first;
//				auto value = it.second;
//				postData.append(StringUtils::format("&%s=%s", key.c_str(), value.c_str()));
//			}
//			postData.erase(0, 1);
//			request->setRequestData(postData.c_str(), postData.length());
//		}
//		//CCLOG("%s", postData.c_str());
//		App::log(postData);
//	}
//	//请求的回调函数
//	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
//		if (!response)
//		{
//			if (errorRunable)
//				errorRunable();
//			return;
//		}
//		if (!response->isSucceed())
//		{
//			if (errorRunable)
//				errorRunable();
//			CCLOG("http failed \n code = %d \n error = %s", response->getResponseCode(), response->getErrorBuffer());
//			return;
//		}
//		std::vector<char> *buffer = response->getResponseData();
//		std::string str(buffer->begin(), buffer->end());
//		App::log(str);
//		if (runable)
//			runable(str);
//	});
//	cocos2d::network::HttpClient::getInstance()->send(request);
//	if (outTime > 0)
//		cocos2d::network::HttpClient::getInstance()->setTimeoutForConnect(outTime);
//	request->release();
//}

void Parent::showRedPacket()
{
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "BookInfo");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_SELECTREDPACKET_CSB);
	auto redPacketLayer = YYXLayer::create(paramter);
	if (!redPacketLayer)
		return;
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
	refer->setVisible(false);
	auto listview = (ListView*)redPacketLayer->findControl(FIND_LISTVIEW_BY_NAME_SELECTREDPACKET_REDPACKET);
	listview->setScrollBarAutoHideEnabled(false);
	listview->setScrollBarEnabled(true);
	//不使用红包
	auto without = (ImageView*)redPacketLayer->findControl(FIND_IMAGEVIEW_BY_NAME_SELECTREDPACKET_DONOTWITHREDPACKET);
	without->setVisible(false);
	//红包说明
	auto message = (ImageView*)redPacketLayer->findControl(FIND_IMAGEVIEW_BY_NAME_REDPACKETMESSAGE);
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
					showRedPacket();
				};
				mypacket->setTouchEnabled(true);
				mypacket->addClickEventListener(listeners);
				backbutton->setTouchEnabled(true);
				backbutton->addClickEventListener(listeners);
				messageLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
				Size visibleSize = Director::getInstance()->getVisibleSize();
				messageLayer->setPosition(visibleSize / 2);
				addChild(messageLayer);
			});
		});
	}
	//返回上级
	auto listerfunction = [=](Ref* sneder) {
		redPacketLayer->removeFromParentAndCleanup(true);
	};
	if (myredpacket && backbutton)
	{
		backbutton->setTouchEnabled(true);
		myredpacket->setTouchEnabled(true);
		backbutton->addClickEventListener(listerfunction);
		myredpacket->addClickEventListener(listerfunction);
	}
	redPacketLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	redPacketLayer->setPosition(Director::getInstance()->getVisibleSize() / 2);
	listViewShowRedPacket(listview);
	auto eventlist = EventListenerCustom::create("httpUserRedPacketSuccess", [=](EventCustom* e) {
		listViewShowRedPacket(listview);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventlist, redPacketLayer);
	addChild(redPacketLayer);
}

void Parent::listViewShowRedPacket(ListView* listview)
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
		YYXLayer::insertMap4ParaType(paramter, "className", -999, "BookInfo");
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

//加载红包数据到节点上
void Parent::loadRedPacketData(Node* node, int index)
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
			text_coupon_amount->setText(StringUtils::format("%d", (int)(coupon_amount / 100)) + App::getString("YUAN"));
		//打钩
		auto img_couponSelect = (ImageView*)node->getChildByName(FIND_IMAGEVIE_BY_NAME_EVERYREDPACKET_SELECT);
		img_couponSelect->setVisible(false);
		//红包封面
		auto redPacket = (ImageView*)node->getChildByName(FIND_IMAGEVIE_BY_NAME_EVERYREDPACKET_COVER);
	}
}
//获取有效红包
void Parent::httpGetUserRedPacket()
{
	string url = string(IP).append(NET_USERREDPACKET).append("?memberId=").append(App::GetInstance()->getMemberID());
	string runKey = "ParentSceneHttpGetUserRedPacketSuccess";
	string errorKey = "ParentSceneHttpGetUserRedPacketFail";
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
			YYXLayer::sendNotify("httpUserRedPacketSuccess");
		}, []() {
			Toast::create(App::getString("HONGBAOHUOQUSHIBAI"));
		});
	}, errorKey, [](string str) {
		Toast::create(App::getString("NETEXCEPTION"));
	});
}

////获取有效红包的回调
//void Parent::httpGetRedpacketCallback(string json)
//{
//	App::log("Parent::httpGetRedpacketCallback");
//	rapidjson::Document doc;
//	YYXLayer::getJsonObject4Json(doc, json);
//	bool result = YYXLayer::getBool4Json(false, doc, "result");
//	int  expiring_coupon_count = YYXLayer::getIntForJson(0, doc, "expiring_coupon_count");
//	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "expiring_coupon_count", expiring_coupon_count);
//	if (result)
//	{
//		App::GetInstance()->m_redPacket.clear();
//		rapidjson::Value arraylist;
//		if (YYXLayer::getJsonArray4Json(arraylist, doc, "data"))
//		{
//			map<string, YYXStruct> result;
//			YYXLayer::getData4JsonArray(arraylist, result, [](rapidjson::Value & item, map<string, YYXStruct>& mapresult) {
//				int coupon_id = YYXLayer::getInt4Json(-999, item, "coupon_id");
//				int coupon_amount100 = YYXLayer::getDouble4Json(-999.00, item, "coupon_amount") * 100;
//				string coupon_expire_time = YYXLayer::getString4Json("", item, "coupon_expire_time");
//				if (coupon_id != -999 || coupon_amount100 != -99900) {
//					YYXStruct::initMapYYXStruct(mapresult, "coupon_id", coupon_id);
//					YYXStruct::initMapYYXStruct(mapresult, "coupon_amount", coupon_amount100);
//					YYXStruct::initMapYYXStruct(mapresult, "coupon_expire_time", 0, coupon_expire_time);
//					App::GetInstance()->m_redPacket.push_back(mapresult);
//				}
//			});
//		}
//		else
//		{
//			Toast::create(App::getString("HONGBAOHUOQUSHIBAI"));
//		}
//	}
//	else
//	{
//		Toast::create(App::getString("HONGBAOHUOQUSHIBAI"));
//	}
//	YYXLayer::sendNotify("referRedPackets", "Parent");
//}