#include "BabyCenter.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "IndexScene.h"
#include "platform/android/CCFileUtils-android.h"
#include "CrossPlatform.h"
#include "YYXVisitor.h"
#include "YYXDownloadImages.h"
#include "YYXSound.h"
#include "AppHttp.h"
#include "User.h"
#include "YYXTime.h"
#include "HttpWaiting.h"
#include "LoginControl.h"

USING_NS_CC;
using namespace cocostudio::timeline;
using namespace std;

//静态成员变量初始化
int BabyCenter::m_readBooks = 0;
long BabyCenter::m_readTimes = 0;

BabyCenter::BabyCenter() {
}

BabyCenter::~BabyCenter() {
	unschedule("babyCenterSceneUpdataShowHeadPortrait");
	ControlScene::getInstance()->end();
	App::log("BabyCenter::~BabyCenter()");
}

Scene* BabyCenter::createScene(SceneInfo* sceneInfo)
{
	auto scene = Scene::create();
	auto layer = BabyCenter::create(sceneInfo);
	if (layer)
	{
		scene->addChild(layer);
		layer->setTag(9);
	}
	return scene;
}


BabyCenter* BabyCenter::create(SceneInfo* data)
{
	BabyCenter *pRet = new(std::nothrow) BabyCenter();
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

bool BabyCenter::init(SceneInfo* sceneInfo)
{
	App::log("BabyCenter::init");
	if (!Layer::init())
		return false;
	//判断用户是否登录
	YYXVisitor::getInstance()->babyCenterSceneInit([=]() {
		initLoginLayer();
	}, [=]() {
		initBabyCenter();
	});
	App::log("BabyCenter::init---END");
	return true;
}

void BabyCenter::onEnterTransitionDidFinish()
{
	App::log("BabyCenter::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	schedule([](float f) {
		YYXLayer::sendNotify(TAG_BABYCENTERSCENEREFERHEAD);
	}, 2, "changerHead");
	scheduleOnce([](float f) {
		YYXLayer::sendNotify(TAG_BABYCENTERSCENESHOWCHILD);
	}, 3,"refershBabyInfo");
	App::log("BabyCenter::onEnterTransitionDidFinish---END");
}

void BabyCenter::cleanup()
{
	App::log("BabyCenter::cleanup()");
	if (babyinfo)
	{
		babyinfo->removeAllChildrenWithCleanup(true);
		babyinfo->removeFromParentAndCleanup(true);
	}
	//更换头像回来之后 定时刷新 
	unschedule("changerHead");
	unschedule("refershBabyInfo");
}

//初始化宝贝中心
void BabyCenter::initBabyCenter() {
	App::log("BabyCenter::initBabyCenter");
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Data csb;	
	if (App::GetInstance()->getData(BABYCENTER_CENTERMAIN_CSB, csb))
		babyinfo = (Layer*)CSLoader::createNode(csb);
	else
		babyinfo = (Layer*)CSLoader::createNode(BABYCENTER_CENTERMAIN_CSB);
	//网络请求宝贝列表
	auto wlayer = HttpWaiting::getInstance()->newWaitingLayer();
	if (wlayer)
		addChild(wlayer, 1000);
	AppHttp::getInstance()->httpChildren();

	babyinfo->setAnchorPoint(Vec2(0.5, 0.5));
	babyinfo->setPosition(Director::getInstance()->getVisibleSize() / 2);
	//文本-昵称
	auto text_name = ( Text*)babyinfo->getChildByName(BABYCENTE_FIND_NAME);

	//文本-性别
	auto text_sex = ( Text*)babyinfo->getChildByName(BABYCENTE_FIND_GENDER);
	text_sex->setTag(1);

	//文本-生日
	auto text_birth = ( Text*)babyinfo->getChildByName(BABYCENTE_FIND_BIRTHDAY);

	//文本-阅读书籍数量
	auto text_read_books = ( Text*)babyinfo->getChildByName(BABYCENTE_FIND_BOOKNUMBER);

	//文本-阅读时间-小时
	auto text_read_hours = ( Text*)babyinfo->getChildByName(BABYCENTE_FIND_HOUR);

	//文本-阅读时间-分钟
	auto text_read_mins = ( Text*)babyinfo->getChildByName(BABYCENTE_FIND_MINUTE);

	//编辑按钮及其触发事件
	auto editButton = ( Button*)babyinfo->getChildByName(BABYCENTE_FIND_EDITBUTTON);
	//头像ImageView及其触发事件
	m_img_photo = ( ImageView*)babyinfo->getChildByName(BABYCENTE_FIND_GOPHOTO);
	m_img_photo->setTouchEnabled(true);
	//点击更换头像
	m_img_photo->addClickEventListener([=](Ref* sender) {
		YYXSound::getInstance()->playButtonSound();
		auto childid = User::getInstance()->getChildId();
		auto child = User::getInstance()->getChild(childid);
		if (child)
		{
			string old = child->getPath();
			string newpath = child->getDir() + "/" + YYXTime::getInstance()->getRandomFileName() + ".png";
			if (newpath != old && FileUtils::getInstance()->isFileExist(newpath))
				FileUtils::getInstance()->removeFile(newpath);
			CrossPlatform::getInstance()->newAlbumChangeHead(childid, old, newpath, [](string json) {
				App::log("--------------********------------newAlbumChangeHeadCall1-------");
				rapidjson::Document doc;
				bool result = YYXLayer::getJsonObject4Json(doc, json);
				if (result)
				{
					auto childId = YYXLayer::getStringForJson("", doc, "childId");
					auto oldHead = YYXLayer::getStringForJson("", doc, "oldHead");
					auto newHead = YYXLayer::getStringForJson("", doc, "newHead");
					auto child = User::getInstance()->getChild(Value(childId).asInt());
					if (child)
						child->setPath(newHead);
					YYXLayer::sendNotify(TAG_BABYCENTERSCENECHANGECHILDREN);
					Toast::create(App::getString("SHANGCHUANTOUXIANGCHEGNGONG"));
				}
			}, [](string json) {
				Toast::create(App::getString("TOUXIANGGEGNXINSHIBAI"));
			});
		}
		//initPhotoLayer();
	});		
	//编辑控件节点-----------
	auto editNode = (Node*)babyinfo->getChildByName(BABYCENTE_FIND_EDITNODE);
	//切换孩子
	auto changChild = (Button*)babyinfo->getChildByName(FIND_BUTTON_BYNAME_BABYCENTER_BABYCENTE_CHANGECHILD);
	if (changChild)
		changChild->addClickEventListener([=](Ref* sender) {
		YYXLayer::sendNotify(TAG_BABYCENTERSCENEAMENDBABYINFO);
		YYXSound::getInstance()->playButtonSound();
		initChangeChild();
	});
	//输入框-姓名
	//auto input_name = (ui::TextField*)editNode->getChildByTag(258);
	auto input_name = MyEditBox::create(Size(250, 30), App::getString("EDITBOX_YILA"));
	input_name->setMyFontSize(20);
	input_name->setBoxMaxLength(12);
	input_name->setMyInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	input_name->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	input_name->setPosition(Vec2(595, 498));
	editNode->addChild(input_name);
	//男孩选择
	auto select_boy = (ImageView*)editNode->getChildByTag(252);
	select_boy->setTouchEnabled(true);
	select_boy->setTag(SELECT);
	//女孩选择
	auto select_girl = (ImageView*)editNode->getChildByTag(253);
	select_girl->setTouchEnabled(true);
	select_girl->setTag(UNSELECT);
	auto c_child = User::getInstance()->getChild(User::getInstance()->getChildId());
	if (c_child) {
		int sex = c_child->getChildrenSex();
		if (sex == 1) {
			select_boy->setTag(SELECT);
			select_girl->setTag(UNSELECT);
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(true);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(true);
		}
		else
		{
			select_boy->setTag(UNSELECT);
			select_girl->setTag(SELECT);
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(true);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(true);
		}
	}
	//选择框-生日
	auto drop_birth = DropBox::create(Vec2(479, 390), Size(235, 20), "wryh.TTF", TextHAlignment::CENTER);
	editNode->addChild(drop_birth);
	drop_birth->setDropEnable(true);
	//完成按钮
	auto commitButton = (Button*)editNode->getChildByTag(106);
	commitButton->setTouchEnabled(true);
	commitButton->addClickEventListener([=](Ref* sender) 
	{
		YYXLayer::controlTouchTime(1, "BabyCentercommitButtonTime", [=]() {
			tempName = text_name->getString();
			App::log("BabyCentercommitButtonTime");
			YYXSound::getInstance()->playButtonSound();
			auto name = input_name->getString();
			name = App::check_input_str(name);
			input_name->setString(name);
			text_name->setString(name);
			//判断名字是否为空
			if (name.length() == 0) {
				Toast::create(App::getString("MESAAGEBOX_ADDCHILD_NAME_ERROR"));
				return;
			}		
			int sexNum = 1;
			if (select_girl->getTag() == 0) {
				sexNum = 2;
			}
			int id = User::getInstance()->getChildId();
			//网络请求修改孩子信息
			auto birthday = drop_birth->getDateText();
			AppHttp::getInstance()->httpAmendBabyInfo(id, ChildInfo::create()->setChildrenName(name)->setChildrenBirth(birthday)->setChildrenSex(sexNum));
			auto layer = HttpWaiting::getInstance()->newWaitingLayer();
			if (layer)
				addChild(layer);
			App::log("BabyCentercommitButtonTime---END");
		});
	});

	//男孩选择-触发事件
	select_boy->addClickEventListener([=](Ref* sender) {
		YYXSound::getInstance()->playButtonSound();
		select_boy->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_TWO_PNG, TextureResType::PLIST);
		select_boy->setTouchEnabled(true);
		select_girl->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_ONE_PNG, TextureResType::PLIST);
		select_girl->setTouchEnabled(true);
		select_boy->setTag(SELECT);
		select_girl->setTag(UNSELECT);
		/*text_sex->setString(App::getString("LITTLE_PRINCE"));*/
	});
	//女孩选择-触发事件
	select_girl->addClickEventListener([=](Ref* sender) {
		YYXSound::getInstance()->playButtonSound();
		select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG, TextureResType::PLIST);
		select_boy->setTouchEnabled(true);
		select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG, TextureResType::PLIST);
		select_girl->setTouchEnabled(true);
		select_boy->setTag(UNSELECT);
		select_girl->setTag(SELECT);
		//text_sex->setString(App::getString("LITTLE_PRINCESS"));
	});
	//-----编辑控件---结束---

	//编辑按钮触发事件
	editButton->setTouchEnabled(true);
	editButton->addClickEventListener([=](Ref* sender) {
		YYXSound::getInstance()->playButtonSound();
		m_img_photo->setTouchEnabled(false);
		editButton->setVisible(false);
		editButton->setTouchEnabled(false);
		text_name->setVisible(false);
		text_sex->setVisible(false);
		text_birth->setVisible(false);
		editNode->setPosition(Vec2(0, 0));
	});

	//返回按钮及其触发事件
	auto homeButton = (Button*)babyinfo->getChildByName(BABYCENTE_FIND_BACK);
	homeButton->setPositionX((1094 - visibleSize.width) / 2 + 14);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		auto control = ControlScene::getInstance();
		control->backFromScene();
	});
	//通知刷新阅读记录
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("REFRESH_READ_RECORD");
	auto listenRefreshReadRecord = EventListenerCustom::create("REFRESH_READ_RECORD", [=](EventCustom* event) {
		CCLOG("----------------%d--------------%d", m_readBooks, m_readTimes);
		if (m_readBooks >= 0) {
			text_read_books->setString(StringUtils::format("%d", m_readBooks));
		}
		if (m_readTimes >= 0) {
			text_read_hours->setString(StringUtils::format("%d", m_readTimes / 60));
			text_read_mins->setString(StringUtils::format("%d", m_readTimes % 60));
		}
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenRefreshReadRecord, babyinfo);

	//通知修改宝贝信息成功
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(TAG_BABYCENTERSCENEAMENDBABYINFO);
	auto listenModifySuccess = EventListenerCustom::create(TAG_BABYCENTERSCENEAMENDBABYINFO, [=](EventCustom* event) {
		App::log("MODIFY_BABY_INFO_SUCCESS");
		m_img_photo->setTouchEnabled(true);
		editButton->setVisible(true);
		editButton->setTouchEnabled(true);
		text_name->setVisible(true);
		text_sex->setVisible(true);
		text_birth->setVisible(true);
		editNode->setPosition(Vec2(2736, 0));
		text_name->setString(input_name->getString());
		text_birth->setString(drop_birth->getDateText());
		if (select_boy->getTag() == SELECT) {
			text_sex->setString(App::getString("LITTLE_PRINCE"));
			text_sex->setTag(1);
		}
		else {
			text_sex->setString(App::getString("LITTLE_PRINCESS"));
			text_sex->setTag(2);
		}
		App::log("MODIFY_BABY_INFO_SUCCESS---END");
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenModifySuccess, babyinfo);
	//修改失败
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(TAG_BABYCENTERSCENEAMENDBABYINFOFAIL);
	auto listenModifyFail = EventListenerCustom::create(TAG_BABYCENTERSCENEAMENDBABYINFOFAIL, [=](EventCustom* event) {
		App::log("MODIFY_BABY_INFO_SUCCESS");
		text_name->setString(tempName);
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenModifyFail, babyinfo);
	//------------------------获取本地数据并初始化孩子信息----------------------
	//先根据mydata数据初始化孩子信息
	auto _child = User::getInstance()->getChild(User::getInstance()->getChildId());
	string name = "*****";
	if (_child)
		name = _child->getChildrenName();
	if (text_name && input_name)
	{
		text_name->setString(name);
		input_name->setString(name);
	}
	int id = -999;
	if (_child)
		id = _child->getChildrenId();
	if (id != -999)
		getReadRecord(id);//请求阅读记录
	int sex = 1;
	if (_child)
		sex = _child->getChildrenSex();
	if (sex == 1) {
		text_sex->setString(App::getString("LITTLE_PRINCE"));
		text_sex->setTag(1);
	}
	else {
		text_sex->setString(App::getString("LITTLE_PRINCESS"));
		text_sex->setTag(2);
		select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG);
		select_boy->setTouchEnabled(true);
		select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG);
		select_girl->setTouchEnabled(true);
		select_boy->setTag(UNSELECT);
		select_girl->setTag(SELECT);
	}
	string birthday = "-----";
	if(_child)
		birthday = _child->getChildrenBirth();
	text_birth->setString(birthday);
	drop_birth->setDateText(birthday);
	//展示头像  初始化请求下载头像
	if (_child)
		showPortrait(m_img_photo, _child->getPath());
	else
		showPortrait(m_img_photo, "");
	//更新信息
	auto listenerShowbabycenterinfo = EventListenerCustom::create(TAG_BABYCENTERSCENESHOWCHILD, [=](EventCustom* e) {
		App::log("babycenterInfo更新信息");
		auto _child = User::getInstance()->getChild(User::getInstance()->getChildId());
		assert(_child != nullptr);
		string name = "*****";
		if (_child)
			name = _child->getChildrenName();
		if (text_name && input_name)
		{
			text_name->setString(name);
			input_name->setString(name);
		}
		int id = -999;
		if (_child)
			id = _child->getChildrenId();
		if (id != -999)
			getReadRecord(id);//请求阅读记录
		int sex = 1;
		if (_child)
			sex = _child->getChildrenSex();
		if (sex == 1) {
			text_sex->setString(App::getString("LITTLE_PRINCE"));
			text_sex->setTag(1);
		}
		else {
			text_sex->setString(App::getString("LITTLE_PRINCESS"));
			text_sex->setTag(2);
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG);
			select_boy->setTouchEnabled(true);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG);
			select_girl->setTouchEnabled(false);
			select_boy->setTag(UNSELECT);
			select_girl->setTag(SELECT);
		}
		string birthday = "-----";
		if (_child)
			birthday = _child->getChildrenBirth();
		text_birth->setString(birthday);
		drop_birth->setDateText(birthday);
		if (_child)
			showPortrait(m_img_photo, _child->getPath());
		else
			showPortrait(m_img_photo, "");
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerShowbabycenterinfo, babyinfo);
	//更新头像
	auto listner2 = EventListenerCustom::create(TAG_BABYCENTERSCENEREFERHEAD, [=](EventCustom* e) {
		auto _child = User::getInstance()->getChild(User::getInstance()->getChildId());
		if (_child)
			showPortrait(m_img_photo, _child->getPath());
		else
			showPortrait(m_img_photo, "");
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listner2, m_img_photo);
	babyinfo->setAnchorPoint(Vec2(0.5f, 0.5f));
	babyinfo->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(babyinfo);	
	//m_dels.push_back(babyinfo);
	App::log("BabyCenter::initBabyCenter--END");
}

//初始化登录界面
void BabyCenter::initLoginLayer() {
	App::log("BabyCenter::initLoginLayer");
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Data data;
	Node* loginNode;
	if( App::GetInstance()->getData(BABYCENTER_TIPLOGIN_CSB,data))
		loginNode = (Layer*)CSLoader::createNode(data);
	else
		loginNode = (Layer*)CSLoader::createNode(BABYCENTER_TIPLOGIN_CSB);
	loginNode->setAnchorPoint(Vec2(0.5f, 0.5f));
	loginNode->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(loginNode);
	//登录按钮
	auto loginButton = (Button*)loginNode->getChildByName(BABYCENTE_FIND_LOGIN_BUTTON);
	loginButton->setTouchEnabled(true);
	//loginButton->addClickEventListener([=](Ref* sender) {
	//	loginNode->removeFromParentAndCleanup(true);
	//	auto control = ControlScene::getInstance();
	//	control->replaceScene(ControlScene::getInstance()->getSceneInfo(BabyCenterScene), ControlScene::getInstance()->getSceneInfo(LoginScene));
	//});

	//返回按钮
	auto homeButton = (Button*)loginNode->getChildByName(BABYCENTE_FIND_BACK);
	homeButton->setPositionX((1094 - visibleSize.width) / 2 + 14);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		loginNode->removeFromParentAndCleanup(true);
		auto control = ControlScene::getInstance();
		control->backFromScene();
	});

	loginButton->addClickEventListener([=](Ref* sender) {
		ControlScene::getInstance()->setDangqianScene(BabyCenterScene);
		LoginControl::getInstance()->Login([](string json) {
			/* * balanceAppleAmount : 204.00
			* memberCity : 七台河市
			* memberGrade : 0
			* memberId : 15383
			* memberName : 13123901077
			* memberProvince : 黑龙江省
			* memberSex : 1
			* qq :
			* weixin :*/
			App::log("-----------------------------------LoginControl call1          vvvv-----------------------------------------");
			App::log(json);
			LoginControl::getInstance()->LoginCallback(json);
			App::log("-----------------------------------LoginControl call1          ^^^-----------------------------------------");
		}, nullptr);
	});

	auto listener1 = EventListenerCustom::create(TAG_REFERSHEVERYSCENE, [](EventCustom* e) {
		auto control = ControlScene::getInstance();
		control->replaceScene(ControlScene::getInstance()->getCurrentScene(), ControlScene::getInstance()->getSceneInfo(BabyCenterScene), false);
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, loginNode);
	App::log("BabyCenter::initLoginLayer---END");
}

//切换孩子界面
void BabyCenter::initChangeChild()
{
	auto orgpor = Director::getInstance()->getVisibleOrigin();
	auto sizew = Director::getInstance()->getVisibleSize();
	map<string, int64String> parameter;
	YYXLayer::insertMap4ParaType(parameter, "className", -999, "changeChild");
	YYXLayer::insertMap4ParaType(parameter, "csb", -999, BABYCENTER_CHANGECHILD_CSB);
	auto changeChildLayer = YYXLayer::create(parameter);
	auto back = (Button*)changeChildLayer->findControl(FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_BACK);
	//返回按钮
	if (back)
	{
		auto visibleSize = Director::getInstance()->getVisibleSize();
		back->setAnchorPoint(Vec2(0, 1));
		back->setPositionX((1094 - visibleSize.width) / 2 + 14);
		back->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_BACK_Time", [=]() {
				YYXSound::getInstance()->playButtonSound();
				if (changeChildLayer)
				{
					changeChildLayer->removeAllChildrenWithCleanup(true);
					changeChildLayer->removeFromParentAndCleanup(true);
				}
				YYXLayer::sendNotify(TAG_BABYCENTERSCENESHOWCHILD);
				//initBabyCenter();
			});
		});
	}
	//第一个孩子
	auto readChild = (Node*)changeChildLayer->findControl(FIND_NODE_BYNAME_BABYCENTER_CHANGECHILD_NUM1CHILD);
	//左孩子
	auto leftChild = (Node*)changeChildLayer->findControl(FIND_NODE_BYNAME_BABYCENTER_CHANGECHILD_LEFTCHILD);
	//右孩子
	auto rightChild = (Node*)changeChildLayer->findControl(FIND_NODE_BYNAME_BABYCENTER_CHANGECHILD_RIGHTCHILD);
	if (readChild && leftChild  && rightChild)
	{
		int readChildId = User::getInstance()->getChildId();
		int leftId = -999;
		int rightId = -999;
		auto childids = User::getInstance()->getAllChildId();
		for (auto id : childids)
		{
			if (id != readChildId)
			{
				if (leftId < 0)
					leftId = id;
				else
					rightId = id;
			}
		}
		setNodeChild(readChild, 0, User::getInstance()->getChild(readChildId));
		setNodeChild(leftChild, 1, User::getInstance()->getChild(leftId));
		setNodeChild(rightChild, 2, User::getInstance()->getChild(rightId));

		auto listenerreadChild = EventListenerCustom::create(TAG_BABYCENTERSCENECHANGECHILDREN, [=](EventCustom* e) {
			int readChildId = User::getInstance()->getChildId();
			int leftId = -999;
			int rightId = -999;
			auto childids = User::getInstance()->getAllChildId();
			for (auto id : childids)
			{
				if (id != readChildId)
				{
					if (leftId < 0)
						leftId = id;
					else
						rightId = id;
				}
			}
			setNodeChild(readChild, 0, User::getInstance()->getChild(readChildId));
			setNodeChild(leftChild, 1, User::getInstance()->getChild(leftId));
			setNodeChild(rightChild, 2, User::getInstance()->getChild(rightId));
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerreadChild, changeChildLayer);
	}
	//编辑按钮
	auto finishbutton = (Button*)changeChildLayer->findControl(FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_FINISH);
	finishbutton->setTag(0);
	if (finishbutton)
	{
		finishbutton->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_BACK_Time", [=]() {
				YYXSound::getInstance()->playButtonSound();
				if (finishbutton->getTag() == 0)
				{
					finishbutton->setTag(1);//编辑状态 展示关闭按钮
					finishbutton->setTitleText(App::getString("WANCHENG"));
					YYXLayer::sendNotify(TAG_BABYCENTERSCENESHOWDELETEBUTTON);
				}
				else
				{
					finishbutton->setTag(0);//完成状态 不展示关闭按钮
					finishbutton->setTitleText(App::getString("BIANJI"));
					YYXLayer::sendNotify(TAG_BABYCENTERSCENECHANGECHILDREN);
				}
			});
		});
	}
	auto listner32 = EventListenerCustom::create(TAG_BABYCENTERSCENECHANGECHILDREN, [=](EventCustom* e) {
		finishbutton->setTag(0);//编辑状态 展示关闭按钮
		finishbutton->setTitleText(App::getString("BIANJI"));
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listner32, finishbutton);
	changeChildLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	changeChildLayer->getParentNode()->setPosition(Director::getInstance()->getVisibleSize() / 2);
	addChild(changeChildLayer);
}

//添加宝贝 对话框初始化
Layer* BabyCenter::initAddChild()
{
	map<string, int64String> parameter;
	YYXLayer::insertMap4ParaType(parameter, "className", -999, "changeChild");
	YYXLayer::insertMap4ParaType(parameter, "csb", -999, ADDCHILD_CSB);
	auto addchildMessagebox = YYXLayer::create(parameter);
	auto b_add = (Button*)addchildMessagebox->findControl(FIND_BUTTON_BY_NAME_ADDCHILD_ADD);
	auto b_close = (Button*)addchildMessagebox->findControl(FIND_BUTTON_BY_NAME_ADDCHILD_CLOSE);
	if (b_close)
	{
		b_close->addClickEventListener([addchildMessagebox](Ref* sender) {
			YYXLayer::controlTouchTime(1, "FIND_BUTTON_BY_NAME_ADDCHILD_CLOSETime", [addchildMessagebox]() {
				YYXSound::getInstance()->playButtonSound();
				addchildMessagebox->removeAllChildrenWithCleanup(true);
				addchildMessagebox->removeFromParentAndCleanup(true);
			});
		});
	}
	auto ttf_name = MyEditBox::create(Size(260, 40), App::getString("EDITBOX_BABYNAME"));
	ttf_name->setMyFontSize(20);
	ttf_name->setBoxMaxLength(12);
	ttf_name->setMyInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
	ttf_name->setMyInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
	ttf_name->setPosition(Vec2(581, 348));
	addchildMessagebox->addChild(ttf_name);

	auto ttf_day = DropBox::create(Vec2(455, 207), Size(260, 25), "wryh.TTF", TextHAlignment::CENTER);
	addchildMessagebox->addChild(ttf_day);
	ttf_day->setDropEnable(true);

	auto man = (CheckBox*)addchildMessagebox->findControl(FIND_CHECKBOX_BY_NAME_ADDCHILD_MAN);
	auto woman = (CheckBox*)addchildMessagebox->findControl(FIND_CHECKBOX_BY_NAME_ADDCHILD_WOMAN);
	auto Pic_man = (ImageView*)addchildMessagebox->findControl(FIND_IMAGEVIEW_BY_NAME_ADDCHILD_MAN);
	auto Pic_woman = (ImageView*)addchildMessagebox->findControl(FIND_IMAGEVIEW_BY_NAME_ADDCHILD_WOMAN);
	//--------------------------------点击事件-------------------------------------------------
	man->setTouchEnabled(true);
	woman->setTouchEnabled(true);
	Pic_man->setTouchEnabled(true);
	Pic_woman->setTouchEnabled(true);
	auto lister_man = [woman, man](Ref* sender) {
		YYXLayer::controlTouchTime(1, "selectChildSex", [woman, man]() {
			YYXSound::getInstance()->playButtonSound();
			woman->setSelectedState(false);
			man->setSelectedState(true);
		});
	};
	man->addClickEventListener(lister_man);
	Pic_man->addClickEventListener(lister_man);
	auto lister_woman = [woman, man](Ref* sender) {
		YYXLayer::controlTouchTime(1, "selectChildSex", [woman, man]() {
			YYXSound::getInstance()->playButtonSound();
			woman->setSelectedState(true);
			man->setSelectedState(false);
		});
	};
	woman->addClickEventListener(lister_woman);
	Pic_woman->addClickEventListener(lister_woman);
	//添加宝贝按钮
	b_add->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "FIND_BUTTON_BY_NAME_ADDCHILD_ADDTime", [=]() {
			YYXSound::getInstance()->playButtonSound();
			auto IsMan = false;
			if (man->getSelectedState())
				IsMan = true;
			auto name = ttf_name->getString();
			name = App::check_input_str(name);
			ttf_name->setString(name);
			if (name.length() <= 0)
			{
				Toast::create(App::getString("MESAAGEBOX_ADDCHILD_NAME_ERROR"));
				return;
			}
			auto day = ttf_day->getDateText();
			ChildInfo* child = ChildInfo::create();
			child->setChildrenName(name);
			child->setChildrenBirth(day);
			child->setChildrenSex(IsMan ? 1 : 2);
			AppHttp::getInstance()->httpAddChild(child);
			auto wlayer = HttpWaiting::getInstance()->newWaitingLayer();
			if (wlayer)
				addChild(wlayer, 100);
		});
	});
	auto listener1 = EventListenerCustom::create(TAG_BABYCENTERSCENECHANGECHILDREN, [=](EventCustom* e) {
		addchildMessagebox->removeAllChildrenWithCleanup(true);
		addchildMessagebox->removeFromParentAndCleanup(true);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener1, b_add);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	addchildMessagebox->getParentNode()->setAnchorPoint(Vec2(0.5f, 0.5f));
	addchildMessagebox->getParentNode()->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	return (Layer*)addchildMessagebox;
}

void BabyCenter::setNodeChild(Node* node, int index, ChildInfo* child)
{
	if (node == nullptr)
		return;
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(node);
	//控件
	auto deleteButton = (Button*)node->getChildByName(FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_CHILD_DELETE);
	auto headPortrait = (ImageView*)node->getChildByName(FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_CHILD_HEADPORTRAIT);
	auto textName = (Text*)node->getChildByName(FIND_TEXT_BYNAME_BABYCENTER_CHANGECHILD_CHILD_NAME);
	if (deleteButton == nullptr || headPortrait == nullptr || textName == nullptr)
		return;
	string path = "";
	string name = "";
	if (child == nullptr || child->getChildrenId() <=0)//没有孩子信息
	{
		path = IAMGE_BABYCENTER_CHANGECHILD_CHILD_HAVENOTCHILD;
		name = App::getString("DIANJITIANJIAHAIZI");
		headPortrait->setTouchEnabled(true);
		headPortrait->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "ChangeHeadPortraitClickTime", [=]() {
				YYXSound::getInstance()->playButtonSound();
				auto addchildLayer = initAddChild();
				addChild(addchildLayer);
			});
		});
	}
	else
	{
		name = child->getChildrenName();
		path = child->getPath();
		int id = child->getChildrenId();
		headPortrait->setTouchEnabled(true);
		if (index != 0)
		{
			headPortrait->addClickEventListener([=](Ref* sender) {
				YYXLayer::controlTouchTime(1, "ChangeHeadPortraitClickTime", [=]() {
					YYXSound::getInstance()->playButtonSound();
					//选中的孩子 
					User::getInstance()->setChildId(id);
					YYXLayer::sendNotify(TAG_BABYCENTERSCENECHANGECHILDREN);
				});
			});
		}
		auto listener31 = EventListenerCustom::create(TAG_BABYCENTERSCENESHOWDELETEBUTTON, [=](EventCustom* e) {
			//删除按钮
			if (id < 0 || index == 0)
				deleteButton->setVisible(false);
			else
				deleteButton->setVisible(true);
			deleteButton->addClickEventListener([=](Ref* sender) {
				YYXLayer::controlTouchTime(1, "FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_CHILD_DELETETime", [=]() {
					YYXSound::getInstance()->playButtonSound();
					auto messagebox = YYXLayer::MyMessageBox(App::getString("QUEDINGSHANCHU"), App::getString("QUEDING"), [=]()
					{
						AppHttp::getInstance()->httpDeleteChild(id);
					}, App::getString("QUXIAO"), []() {});
					if (messagebox)
						addChild(messagebox, 11);
				});
			});
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener31, node);
	}
	deleteButton->setVisible(false);
	//名字
	if (textName)
		textName->setText(name);
	//头像
	if (!path.empty() && FileUtils::getInstance()->isFileExist(path))
		headPortrait->loadTexture(path);
	else
		headPortrait->loadTexture(IAMGE_BABYCENTER_CHANGECHILD_CHILD_HEADPORTRAIT);	
	App::log("setNodeChild--END", index);
}

//网络请求阅读记录
void BabyCenter::getReadRecord(int child_id) {
	App::log("BabyCenter::getReadRecord-");
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::POST);
	request->setUrl(std::string(IP).append(NET_QUERYREADHISTORY).c_str());
	string postData = "memberId=" + App::getMemberID() + "&childrenId=" + StringUtils::format("%d", child_id);
	request->setRequestData(postData.c_str(), strlen(postData.c_str()));

	//请求的回调函数
	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
		if (!response) {
			return;
		}
		if (!response->isSucceed()) {
			log("response failed! error buffer: %s", response->getErrorBuffer());
			return;
		}

		//json解析
		std::vector<char> *buffer = response->getResponseData();
		std::string str(buffer->begin(), buffer->end());

		rapidjson::Document doc;
		doc.Parse<0>(str.c_str());
		if (doc.HasParseError()) {//json parse error
			return;
		}

		if (!doc.IsObject() || !doc.HasMember("code") || !doc.HasMember("data")) {
			return;
		}

		rapidjson::Value &value = doc["code"];
		if (value.IsInt() && value.GetInt() == 0) {//请求修改正常
			rapidjson::Value &data = doc["data"];
			m_readBooks = App::analysisJsonInt(data, "bookCount");
			m_readTimes = App::analysisJsonDouble(data, "timeCount");
			//请求刷新阅读记录
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				if (!Director::getInstance()->getEventDispatcher()->isEnabled()) {
					Director::getInstance()->getEventDispatcher()->setEnabled(true);
				}
				EventCustom eventRefreshReadRecord("REFRESH_READ_RECORD");
				Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventRefreshReadRecord);
			});
		}
	});
	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
	App::log("BabyCenter::getReadRecord----END");
}

bool BabyCenter::showPortrait(ImageView* imgview,string path)
{
	//展示头像
	App::log("--------------********------------showPortrait(m_img_photo, child_->getPath());-------"+ path);
	if (!path.empty() && FileUtils::getInstance()->isFileExist(path))
	{
		if(imgview)
			imgview->loadTexture(path);
		return true;
	}
	else
	{
		if(imgview)
			imgview->loadTexture(IAMGE_BABYCENTER_CHANGECHILD_CHILD_HEADPORTRAIT);
		return false;
	}
}