#include "BabyCenter.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "IndexScene.h"
#include "platform/android/CCFileUtils-android.h"
#include "NetIntface.h"

USING_NS_CC;
using namespace cocostudio::timeline;

//静态成员变量初始化
int BabyCenter::m_readBooks = 0;
long BabyCenter::m_readTimes = 0;
std::map<int, map<std::string, YYXStruct>>  BabyCenter::m_data;//数据源

BabyCenter::BabyCenter() {
	m_childrenId = -999;
}

BabyCenter::~BabyCenter() {
	unschedule("babyCenterSceneUpdataShowHeadPortrait");
	App::log("BabyCenter::~BabyCenter()");
}

Scene* BabyCenter::createScene()
{
	auto scene = Scene::create();
	auto layer = BabyCenter::create();
	scene->addChild(layer);
	layer->setTag(9);
	return scene;
}

bool BabyCenter::init()
{
	App::log("BabyCenter::init");
	if (!Layer::init())
		return false;
	 
	App::m_RunningScene = MySceneName::BabyCenterScene;
	////删除头像
	//auto t = App::getCurrentTime() - App::GetInstance()->getTime("deleteTime", 0);
	//if (t > 60*60)
	//{
	//	Parent::DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "temp");
	//	App::GetInstance()->addTime("deleteTime", App::getCurrentTime());
	//}
	//判断用户是否登录
	if (App::GetInstance()->m_me == nullptr) {
		initLoginLayer();
	}
	else {
		initBabyCenter();
	}
	initHttp();
	initEvent();
	App::log("BabyCenter::init---END");
	return true;
}

void BabyCenter::initEvent()
{	
}

void BabyCenter::initHttp()
{
}

void BabyCenter::onEnterTransitionDidFinish()
{
	App::log("BabyCenter::onEnterTransitionDidFinish");
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
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	App::log("BabyCenter::onEnterTransitionDidFinish---END");
}

//初始化宝贝中心
void BabyCenter::initBabyCenter() {
	App::log("BabyCenter::initBabyCenter");
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Data csb;
	Node* babyinfo;
	if (App::GetInstance()->getData(BABYCENTER_CENTERMAIN_CSB, csb))
		babyinfo = (Layer*)CSLoader::createNode(csb);
	else
		babyinfo = (Layer*)CSLoader::createNode(BABYCENTER_CENTERMAIN_CSB);
	auto lsit = EventListenerCustom::create("babyCenterSceneReLogin", [=](EventCustom* e) {
		babyinfo->removeFromParentAndCleanup(true);
		initLoginLayer();
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lsit, babyinfo);
	//网络请求宝贝列表
	if(App::GetInstance()->m_me && App::GetInstance()->m_me->id > 0)
	{
		string runkey = StringUtils::format("BabyCenterScenehttpGetChildDetailsSuccess_%d", (int)YYXLayer::getRandom());
		string errorkey = StringUtils::format("BabyCenterScenehttpGetChildDetailsFail_%d", (int)YYXLayer::getRandom());
		NetIntface::httpGetChildDetails(App::GetInstance()->m_me->id, runkey, [=](string json) {
			getChildDetailsBusinessLogic(json, [=]() {
				//解析错误
				App::cancelData();
				YYXLayer::sendNotify("babyCenterSceneReLogin");
			});
		}, errorkey, [](string str) {
			Toast::create(App::getString("SHUAXINSHIBAI"));
		});
	}
	else
	{
		App::cancelData();
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
			babyinfo->removeFromParentAndCleanup(true);
			initLoginLayer();
		});
	}
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
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		initPhotoLayer();
		babyinfo->removeFromParentAndCleanup(true);
	});
	//更新头像的定时器
	string key = "babyCenterSceneUpdataShowHeadPortrait";
	unschedule(key);
	schedule([=](float f) {
		auto ShowChildHeadPortrait = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildHeadPortrait", "");
		if (ShowChildHeadPortrait != "" && FileUtils::getInstance()->isFileExist(ShowChildHeadPortrait))
		{
			YYXLayer::sendNotify("BaByCenterSceneChildInfoReferHeadPortrait");
			unschedule(key);
		}
		else
		{
			int id = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
			if (id > 0)
			{
				string savePath = FileUtils::getInstance()->getWritablePath() + StringUtils::format("temp/childHead_%d.png", id);
				if (FileUtils::getInstance()->isFileExist(savePath))
				{
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, savePath);
					YYXLayer::sendNotify("BaByCenterSceneChildInfoReferHeadPortrait");
					unschedule(key);
				}
			}
		}
	}, 1, key);
	//编辑控件节点-----------
	auto editNode = (Node*)babyinfo->getChildByName(BABYCENTE_FIND_EDITNODE);
	//切换孩子
	auto changChild = (Button*)babyinfo->getChildByName(FIND_BUTTON_BYNAME_BABYCENTER_BABYCENTE_CHANGECHILD);
	if (changChild)
		changChild->addClickEventListener([=](Ref* sender) {
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
	select_boy->setTouchEnabled(false);
	select_boy->setTag(SELECT);
	//女孩选择
	auto select_girl = (ImageView*)editNode->getChildByTag(253);
	select_girl->setTouchEnabled(true);
	select_girl->setTag(UNSELECT);
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
			App::log("BabyCentercommitButtonTime");
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			//判断名字是否为空
			if (input_name->getString().length() == 0) {
				Toast::create(App::getString("MESAAGEBOX_ADDCHILD_NAME_ERROR"));
				return;
			}
			int sexNum = 1;
			if (select_girl->getTag() == 0) {
				sexNum = 2;
			}
			int id = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
			//网络请求修改孩子信息
			if (id != -999)
			{
				auto name = input_name->getString();
				auto birthday = drop_birth->getDateText();
				App::log("id = ", id);
				App::log("name = "+ name);
				App::log("birthday = " + birthday);
				App::log("sexNum = " , sexNum);
				string runkey2 = StringUtils::format("httpAmendBabyInfoSuccess_%d", (int)YYXLayer::getRandom());
				string errorkey2 = StringUtils::format("httpAmendBabyInfoFail_%d", (int)YYXLayer::getRandom());
				NetIntface::httpAmendBabyInfo(id, name , sexNum, birthday,
					runkey2, [=](string json) {
					rapidjson::Document doc;
					auto result = YYXLayer::getJsonObject4Json(doc, json);
					if (result)
					{
						auto code = YYXLayer::getIntForJson(-999, doc, "code");
						if (code == 0)
						{
							YYXLayer::sendNotify("MODIFY_BABY_INFO_SUCCESS");
							setBabyInfoToXml(input_name->getString(), drop_birth->getDateText(), sexNum, id);
							string namekey = StringUtils::format("name+childID=%d", id);
							string pathkey = StringUtils::format("path+childID=%d", id);
							string birthdaykey = StringUtils::format("birthday+childID=%d", id);
							string urlkey = StringUtils::format("url+childID=%d", id);
							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, namekey, id, name);
							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, birthdaykey, sexNum, birthday);
						}
						else
							YYXLayer::sendNotify("MODIFY_BABY_INFO_FAILED");
					}
				}, errorkey2, [](string str) {
					YYXLayer::sendNotify("MODIFY_BABY_INFO_FAILED");
				});
			}
			//this->addChild(Index::WaitLayer(), 5, "waitLayer");
			App::log("BabyCentercommitButtonTime---END");
		});
	});

	//男孩选择-触发事件
	select_boy->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		select_boy->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_TWO_PNG, TextureResType::PLIST);
		select_boy->setTouchEnabled(false);
		select_girl->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_ONE_PNG, TextureResType::PLIST);
		select_girl->setTouchEnabled(true);
		select_boy->setTag(SELECT);
		select_girl->setTag(UNSELECT);
		/*text_sex->setString(App::getString("LITTLE_PRINCE"));*/
	});
	//女孩选择-触发事件
	select_girl->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG, TextureResType::PLIST);
		select_boy->setTouchEnabled(true);
		select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG, TextureResType::PLIST);
		select_girl->setTouchEnabled(false);
		select_boy->setTag(UNSELECT);
		select_girl->setTag(SELECT);
		//text_sex->setString(App::getString("LITTLE_PRINCESS"));
	});
	//-----编辑控件---结束---

	//编辑按钮触发事件
	editButton->setTouchEnabled(true);
	editButton->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
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
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		Index::GoToIndexScene();
	});
	App::log("222222222222222222222222222");
	//通知刷新阅读记录
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("REFRESH_READ_RECORD");
	auto listenRefreshReadRecord = EventListenerCustom::create("REFRESH_READ_RECORD", [=](EventCustom* event) {
		CCLOG("----------------%d--------------%d", m_readBooks, m_readTimes);
		if (m_readBooks > 0) {
			text_read_books->setString(StringUtils::format("%d", m_readBooks));
		}
		if (m_readTimes > 0) {
			text_read_hours->setString(StringUtils::format("%d", m_readTimes / 60));
			text_read_mins->setString(StringUtils::format("%d", m_readTimes % 60));
		}
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenRefreshReadRecord, babyinfo);

	/*//通知刷新宝贝信息
	auto listenRefreshBabyInfo = EventListenerCustom::create("REFRESH_CHILD_INFO_OVER", [=](EventCustom* event) {
		App::log("通知刷新宝贝信息");
		vector<string> param2;
		param2.push_back("children_name");
		param2.push_back("children_sex");
		param2.push_back("children_birth");
		param2.push_back("children_id");
		vector<unordered_map<string, ParaType>> result2 = SqliteManager::SelectData(App::sqliteOpen(), DB_CHILDREN, param2, "where member_id=" + App::getMemberID());
		App::sqliteClose();
		//获取本地数据为空
		if (result2.size() <= 0)
			return;
		m_childrenId = result2[0]["children_id"].intPara;
		//请求阅读记录
		getReadRecord(m_childrenId);

		//根据数据库信息初始化孩子信息
		if (result2[0]["children_name"].stringPara.length() >= 0) {
			text_name->setString(result2[0]["children_name"].stringPara);
			input_name->setString(result2[0]["children_name"].stringPara);
		}
		if (result2[0]["children_sex"].intPara == 1) {
			text_sex->setString(App::getString("LITTLE_PRINCE"));
			text_sex->setTag(1);
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(false);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(true);
			select_boy->setTag(SELECT);
			select_girl->setTag(UNSELECT);
		}
		else {
			text_sex->setString(App::getString("LITTLE_PRINCESS"));
			text_sex->setTag(2);
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(true);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(false);
			select_boy->setTag(UNSELECT);
			select_girl->setTag(SELECT);
		}

		if (result2[0]["children_birth"].stringPara.length() >= 0) {
			text_birth->setString(result2[0]["children_birth"].stringPara);
			drop_birth->setDateText(result2[0]["children_birth"].stringPara);
		}
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listenRefreshBabyInfo, 1);*/

	/*//通知根据网络数据提供的数据源刷新宝贝信息
	auto listenRefreshBabyInfo4http = EventListenerCustom::create("REFRESH_CHILD_INFO4HTTP", [=](EventCustom* event) {
		App::log("通知刷新宝贝信息");	
		//请求阅读记录
		auto childid = App::GetInstance()->m_read->childrenId;
		if (childid > 0)
		{
			getReadRecord(childid);
		}
		auto data = m_data[childid];
		auto children_name = App::getString4map(data, "ChildName", "小宝宝");
			text_name->setString(children_name);
			input_name->setString(children_name);
		auto children_sex = App::getlonglong4map(data, "ChildSex", 1);
		if (children_sex == 1) {
			text_sex->setString(App::getString("LITTLE_PRINCE"));
			text_sex->setTag(1);
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(false);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(true);
			select_boy->setTag(SELECT);
			select_girl->setTag(UNSELECT);
		}
		else {
			text_sex->setString(App::getString("LITTLE_PRINCESS"));
			text_sex->setTag(2);
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(true);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(false);
			select_boy->setTag(UNSELECT);
			select_girl->setTag(SELECT);
		}
		auto children_birth = App::getString4map(data, "ChildBirth", "2008.08.08");
		text_birth->setString(children_birth);
		drop_birth->setDateText(children_birth);
	});
	_eventDispatcher->addEventListenerWithFixedPriority(listenRefreshBabyInfo4http, 1);*/

	////通知根据网络数据提供的数据源刷新宝贝信息
	//auto listenRefreshBabyInfo4portrait = EventListenerCustom::create("REFRESH_PORTRAIT", [=](EventCustom* event) {
	//	App::log("通知刷新头像");
	//	auto childid = App::GetInstance()->m_read->childrenId;
	//	if (childid > 0)
	//	{
	//		auto data = m_data[childid];
	//		auto fullpath = App::getString4map(data, "Path", "");
	//		if (FileUtils::getInstance()->isFileExist(fullpath))
	//			m_img_photo->loadTexture(fullpath);
	//	}
	//});
	//_eventDispatcher->addEventListenerWithFixedPriority(listenRefreshBabyInfo4portrait, 1);

	//通知修改宝贝信息成功
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("MODIFY_BABY_INFO_SUCCESS");
	auto listenModifySuccess = EventListenerCustom::create("MODIFY_BABY_INFO_SUCCESS", [=](EventCustom* event) {
		App::log("MODIFY_BABY_INFO_SUCCESS");
		m_img_photo->setTouchEnabled(true);
		editButton->setVisible(true);
		editButton->setTouchEnabled(true);
		text_name->setVisible(true);
		text_sex->setVisible(true);
		text_birth->setVisible(true);

		editNode->setPosition(Vec2(2736, 0));

		//删除waitlayer
	/*	auto wait = (Layer*)this->getChildByName("waitLayer");
		if (wait != nullptr  && wait->getParent() != nullptr) {
			wait->removeFromParentAndCleanup(true);
		}*/
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
		Toast::create(App::getString("XIUGAIXINXICHENGGONG"));
		App::log("MODIFY_BABY_INFO_SUCCESS---END");
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenModifySuccess, babyinfo);

	//通知修改宝贝信息失败
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("MODIFY_BABY_INFO_FAILED");
	auto listenModifyFailed = EventListenerCustom::create("MODIFY_BABY_INFO_FAILED", [=](EventCustom* event) {
		m_img_photo->setTouchEnabled(true);
		editButton->setVisible(true);
		editButton->setTouchEnabled(true);
		text_name->setVisible(true);
		text_sex->setVisible(true);
		text_birth->setVisible(true);

		editNode->setPosition(Vec2(2736, 0));
		//删除waitlayer
	/*	auto wait = (Layer*)this->getChildByName("waitLayer");
		if (wait != nullptr  && wait->getParent() != nullptr) {
			wait->removeFromParentAndCleanup(true);
		}*/

		input_name->setString(text_name->getString());
		drop_birth->setDateText(text_birth->getString());
		if (text_sex->getTag() == 1) {
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(false);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(true);
			select_boy->setTag(SELECT);
			select_girl->setTag(UNSELECT);
		}
		else {
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(true);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(false);
			select_boy->setTag(UNSELECT);
			select_girl->setTag(SELECT);
		}
		Toast::create(App::getString("MODIFY_INFO_FAILED"));
	});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenModifyFailed, babyinfo);
	//------------------------获取本地数据并初始化孩子信息----------------------
	//先根据mydata数据初始化孩子信息
	auto name = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildName", "");
	if (text_name && input_name)
	{
		text_name->setString(name);
		input_name->setString(name);
	}
	int id = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
	if (id != -999)
		getReadRecord(id);//请求阅读记录
	int sex = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildSex", -999);
	if (sex == 1) {
		text_sex->setString(App::getString("LITTLE_PRINCE"));
		text_sex->setTag(1);
	}
	else {
		text_sex->setString(App::getString("LITTLE_PRINCESS"));
		text_sex->setTag(2);
		select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG, TextureResType::PLIST);
		select_boy->setTouchEnabled(true);
		select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG, TextureResType::PLIST);
		select_girl->setTouchEnabled(false);
		select_boy->setTag(UNSELECT);
		select_girl->setTag(SELECT);
	}
	auto birthday = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildBirthday", "2008.08.08");
	text_birth->setString(birthday);
	drop_birth->setDateText(birthday);
	//展示头像  初始化请求下载头像
	showPortrait(m_img_photo);
	//更新信息
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("BaByCenterSceneChildInfoshowFirstChild");
	auto listenerShowbabycenterinfo = EventListenerCustom::create("BaByCenterSceneChildInfoshowFirstChild", [=](EventCustom* e) {
		App::log("babycenterInfo更新信息");
		auto name = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildName", "");
		if (text_name && input_name)
		{
			text_name->setString(name);
			input_name->setString(name);
		}
		int id = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
		if (id != -999)
			getReadRecord(id);//请求阅读记录
		int sex = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildSex", -999);
		if (sex == 1) {
			text_sex->setString(App::getString("LITTLE_PRINCE"));
			text_sex->setTag(1);
		}
		else {
			text_sex->setString(App::getString("LITTLE_PRINCESS"));
			text_sex->setTag(2);
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(true);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(false);
			select_boy->setTag(UNSELECT);
			select_girl->setTag(SELECT);
		}
		auto birthday = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildBirthday", "2008.08.08");
		text_birth->setString(birthday);
		drop_birth->setDateText(birthday);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerShowbabycenterinfo, babyinfo);
	//更新头像
	Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("BaByCenterSceneChildInfoReferHeadPortrait");
	auto listner2 = EventListenerCustom::create("BaByCenterSceneChildInfoReferHeadPortrait", [=](EventCustom* e) {
		App::log("babycenter更新头像");
		showPortrait(m_img_photo);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listner2, babyinfo);
	babyinfo->setAnchorPoint(Vec2(0.5f, 0.5f));
	babyinfo->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(babyinfo);
	/*vector<string> param;
	param.push_back("children_name");
	param.push_back("children_sex");
	param.push_back("children_birth");
	param.push_back("children_id");
	vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_CHILDREN, param, "where member_id=" + App::getMemberID());
	App::sqliteClose();
	//获取本地数据为空
	if (result.size() > 0) {
		m_childrenId = result[0]["children_id"].intPara;
		//请求阅读记录
		getReadRecord(m_childrenId);

		//根据数据库信息初始化孩子信息
		if (result[0]["children_name"].stringPara.length() >= 0) {
			text_name->setString(result[0]["children_name"].stringPara);
			input_name->setString(result[0]["children_name"].stringPara);
		}

		if (result[0]["children_sex"].intPara == 1) {
			text_sex->setString(App::getString("LITTLE_PRINCE"));
			text_sex->setTag(1);
		}
		else {
			text_sex->setString(App::getString("LITTLE_PRINCESS"));
			text_sex->setTag(2);
			select_boy->loadTexture(BABYCENTE_FIND_USERINFO_NOT_SELECTED_TWO_PNG, TextureResType::PLIST);
			select_boy->setTouchEnabled(true);
			select_girl->loadTexture(BABYCENTE_FIND_USERINFO_SELECTED_ONE_PNG, TextureResType::PLIST);
			select_girl->setTouchEnabled(false);
			select_boy->setTag(UNSELECT);
			select_girl->setTag(SELECT);
		}

		if (result[0]["children_birth"].stringPara.length() >= 0) {
			text_birth->setString(result[0]["children_birth"].stringPara);
			drop_birth->setDateText(result[0]["children_birth"].stringPara);
		}
	}*/
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
	loginButton->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		//跳转登陆
		App::GetInstance()->pushScene(BabyCenterScene);
		Index::GoToLoginScene();
	});

	//返回按钮
	auto homeButton = (Button*)loginNode->getChildByName(BABYCENTE_FIND_BACK);
	homeButton->setPositionX((1094 - visibleSize.width) / 2 + 14);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		Index::GoToIndexScene();
	});
	App::log("BabyCenter::initLoginLayer---END");
}

//初始化头像界面
void BabyCenter::initPhotoLayer() {
	App::log("BabyCenter::initPhotoLayer");
	auto visibleSize = Director::getInstance()->getVisibleSize();

	Data data;
	Node* SelcetHeadPortrait;
	if (App::GetInstance()->getData(BABYCENTER_PHOTOSHOW_CSB, data))
		SelcetHeadPortrait = (Layer*)CSLoader::createNode(data);
	else
		SelcetHeadPortrait = (Layer*)CSLoader::createNode(BABYCENTER_PHOTOSHOW_CSB);
	SelcetHeadPortrait->setAnchorPoint(Vec2(0.5f, 0.5f));
	SelcetHeadPortrait->setPosition(visibleSize / 2);
	addChild(SelcetHeadPortrait);

	// 大头像
	m_BigPhoto = (ImageView*)SelcetHeadPortrait->getChildByName(PHOTOSHOW_FIND_BIGPHOTO);
	showPortrait(m_BigPhoto);
	auto listenerShowPhoto = EventListenerCustom::create("PhotoLayerShowFullPathImage", [=](EventCustom* e) {
		string selectCutRoundPath = YYXStruct::getMapString(App::GetInstance()->myData, "selectCutRoundPath", "");
		if (selectCutRoundPath != "" && FileUtils::getInstance()->isFileExist(selectCutRoundPath))
			m_BigPhoto->loadTexture(selectCutRoundPath);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerShowPhoto, m_BigPhoto);
	//调用本地相册
	auto localPhoto = (Button*)SelcetHeadPortrait->getChildByName(PHOTOSHOW_FIND_LOCALPHOTO);
	localPhoto->setTouchEnabled(true);
	localPhoto->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "PHOTOSHOW_FIND_LOCALPHOTOTime", [=]() {
			//selectPath = "";
			YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "selectPath");
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			string dir = FileUtils::getInstance()->getWritablePath() + "temp";
			string fileName = "PhotoAlbum.png";
			string thispath = dir + "/" + fileName;
			if (FileUtils::getInstance()->isFileExist(thispath))
			{
				FileUtils::getInstance()->removeFile(thispath);
			}
			string runkey3 = StringUtils::format("openPhotoAlbumSelectImageSuccess_%d", (int)YYXLayer::getRandom());
			string errorkey3 = StringUtils::format("openPhotoAlbumSelectImageFail_%d", (int)YYXLayer::getRandom());
			NetIntface::openPhotoAlbumSelectImage(fileName, dir, 600, 600, runkey3, [=](string path) {
				//selectPath = path;
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "selectPath", -999, path);
				int id = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
				string savePath = FileUtils::getInstance()->getWritablePath() + StringUtils::format("temp/ShowchildHead_%d.png", id);
				App::makeRoundImage(path, savePath);
				//selectCutRoundPath = savePath;
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "selectCutRoundPath", -999, savePath);
				YYXLayer::sendNotify("PhotoLayerShowFullPathImage");
			}, errorkey3, [](string str) {
				Toast::create(App::getString("QUXIAOCAOZUO"));
			});
		});
	});

	//打开相机拍照
	auto takePhoto = (Button*)SelcetHeadPortrait->getChildByName(PHOTOSHOW_FIND_TAKEPHOTO);
	takePhoto->setScale9Enabled(true);
	takePhoto->setTouchEnabled(true);
	takePhoto->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(1, "PHOTOSHOW_FIND_LOCALPHOTOTime", [=]() {
			//selectPath = "";
			YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "selectPath");
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			string dir = NetIntface::getAlbumAbsolutePath();
			string fileName = "photograph.png";
			string imgpath = dir + "/" + fileName;
			if (!FileUtils::getInstance()->isDirectoryExist(dir))
				FileUtils::getInstance()->createDirectory(dir);
			if (FileUtils::getInstance()->isFileExist(imgpath))
				FileUtils::getInstance()->removeFile(imgpath);
			string runkey4 = StringUtils::format("photographSuccess_%d", (int)YYXLayer::getRandom());
			string errorkey4 = StringUtils::format("photographFail_%d", (int)YYXLayer::getRandom());
			NetIntface::photograph(fileName, dir, runkey4, [=](string path) {
				//selectPath = path;
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "selectPath", -999, path);
				int id = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
				string savePath = FileUtils::getInstance()->getWritablePath() + StringUtils::format("temp/ShowchildHead_%d_%d.png", id, (int)YYXLayer::getRandom());
				App::makeRoundImage(path, savePath);
				//selectCutRoundPath = savePath;
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "selectCutRoundPath", -999, savePath);
				YYXLayer::sendNotify("PhotoLayerShowFullPathImage");
			}, errorkey4, [](string str) {
				Toast::create(App::getString("QUXIAOCAOZUO"));
			});
		});
	});

	//点击按钮,上传头像
	auto photoEnd = (Button*)SelcetHeadPortrait->getChildByName(PHOTOSHOW_FIND_PHOTOEND);
	photoEnd->setScale9Enabled(true);
	photoEnd->setTouchEnabled(true);
	photoEnd->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(3, "upimagetime", [=]() {
			Toast::create(App::getString("UPIMAGE"));
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
				YYXLayer::PLAYBUTTON;
			uploadAvatar();
		});
	});

	//返回按钮
	auto homeButton = (Button*)SelcetHeadPortrait->getChildByName(BABYCENTE_FIND_BACK);
	homeButton->setTouchEnabled(true);
	homeButton->setPositionX((1094 - visibleSize.width) / 2 + 14);
	homeButton->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		SelcetHeadPortrait->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		SelcetHeadPortrait->removeFromParentAndCleanup(true);
		initBabyCenter();
	});
	App::log("BabyCenter::initPhotoLayer--END");
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
		back->removeFromParent();
		back->setAnchorPoint(Vec2(0, 1));
		back->setPosition(Vec2(orgpor.x + 14, orgpor.y + sizew.height - 14));
		back->setZOrder(10);
		changeChildLayer->addChild(back);
		back->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_BACK_Time", [=]() {
				if(changeChildLayer)
				changeChildLayer->removeFromParentAndCleanup(true);
				initBabyCenter();
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
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BabyCenterChangeChildEditButton", 0);
		//现在的位置是根据网络获取排序的,找出展示孩子的ID 换位
		auto showID = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
		if (showID != -999)
		{
			//换位置
			auto firstChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "firstChildID", -999);
			auto secondChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "secondChildID", -999);
			auto threeChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "threeChildID", -999);
			if (secondChildID != -999 && showID == secondChildID)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "secondChildID", firstChildID);
			else if (threeChildID != -999 && showID == threeChildID)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "threeChildID", firstChildID);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "firstChildID", showID);			
		}
		setNodeChild(readChild, 0);
		setNodeChild(leftChild, 1);
		setNodeChild(rightChild, 2);
		auto listenerreadChild = EventListenerCustom::create("qiehuanhaiziAction", [=](EventCustom* e) {
			//现在的位置是根据网络获取排序的,找出展示孩子的ID 换位
			auto showID = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
			if (showID != -999)
			{
				auto firstChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "firstChildID", -999);
				auto secondChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "secondChildID", -999);
				auto threeChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "threeChildID", -999);
				if (secondChildID != -999  && showID == secondChildID)
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "secondChildID", firstChildID);
				else if (threeChildID != -999 && showID == threeChildID)
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "threeChildID", firstChildID);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "firstChildID", showID);				
			}
			//删除监听
			Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(readChild);
			Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(leftChild);
			Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(rightChild);
			setNodeChild(readChild, 0);
			setNodeChild(leftChild, 1);
			setNodeChild(rightChild, 2);
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
				if (finishbutton->getTag() == 0)
				{
					finishbutton->setTag(1);//编辑状态 展示关闭按钮
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BabyCenterChangeChildEditButton", 1);
					finishbutton->setTitleText(App::getString("WANCHENG"));
					YYXLayer::sendNotify("BabyCenterSceneChangChildShowDeleteButton");
				}
				else
				{
					finishbutton->setTag(0);//完成状态 不展示关闭按钮
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "BabyCenterChangeChildEditButton", 0);
					finishbutton->setTitleText(App::getString("BIANJI"));
					YYXLayer::sendNotify("BabyCenterSceneChangChildSetUNvisibleDeleteButton");
				}
			});
		});
	}	
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
	//auto ttf_name = (TextField*)addchildMessagebox->getChildByName(FIND_TEXTFIELD_BY_NAME_ADDCHILD_BABYNAME);
	//auto ttf_day = (TextField*)addchildMessagebox->getChildByName(FIND_TEXTFIELD_BY_NAME_ADDCHILD_BABYDAY);
	if (b_close)
	{
		b_close->addClickEventListener([addchildMessagebox](Ref* sender) {
			YYXLayer::controlTouchTime(1, "FIND_BUTTON_BY_NAME_ADDCHILD_CLOSETime", [addchildMessagebox]() {
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
		YYXLayer::controlTouchTime(1, "FIND_BUTTON_BY_NAME_ADDCHILD_ADDTime", [=]() {
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
			string runkey = StringUtils::format("babyCenterScenehttpAddChildSuccess_%d", (int)YYXLayer::getRandom());
			string errorkey = StringUtils::format("babyCenterScenehttpAddChildFail_%d", (int)YYXLayer::getRandom());
			NetIntface::httpAddChild(App::GetInstance()->m_me->id, name, IsMan ? 1 : 2, day, runkey, [=](string json) {
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
					Toast::create(App::getString("MESAAGEBOX_ADDCHILD_SUCCESS"));
					addchildMessagebox->removeFromParentAndCleanup(true);
					NetIntface::httpGetChildDetails(App::GetInstance()->m_me->id, "babyCenterScenehttpGetChildDetailsSuccess", [=](string json) {
						getChildDetailsBusinessLogic(json, []() {});
					}, "babyCenterScenehttpGetChildDetailsFail", [](string str) {
						Toast::create(App::getString("SHUAXINSHIBAI"));
					});
				});
			}, errorkey, [](string str) {
				Toast::create(App::getString("MESAAGEBOX_ADDCHILD_ERROR"));
			});
		});
	});	
	auto visibleSize = Director::getInstance()->getVisibleSize();
	addchildMessagebox->getParentNode()->setAnchorPoint(Vec2(0.5f, 0.5f));
	addchildMessagebox->getParentNode()->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	return (Layer*)addchildMessagebox;
}

void BabyCenter::setNodeChild(Node* node, int index)
{
	App::log("setNodeChild",index);
	auto firstChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "firstChildID", 111111);
	auto secondChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "secondChildID", 222222);
	auto threeChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "threeChildID", 333333);
	App::log("showChildRen_ firstChildID = ", firstChildID);
	App::log("showChildRen_ secondChildID = ", secondChildID);
	App::log("showChildRen_ threeChildID = ", threeChildID);
	//控件
	auto deleteButton = (Button*)node->getChildByName(FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_CHILD_DELETE);
	auto headPortrait = (ImageView*)node->getChildByName(FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_CHILD_HEADPORTRAIT);
	auto textName = (Text*)node->getChildByName(FIND_TEXT_BYNAME_BABYCENTER_CHANGECHILD_CHILD_NAME);
	//数据id
	int id = -999;
	if (index == 0)
	{
		id = YYXStruct::getMapInt64(App::GetInstance()->myData, "firstChildID", -999);
		//ShowChildSex 选中的孩子性别
		//ShowChildBirthday 选中的孩子生日
		string birthdaykey = StringUtils::format("birthday+childID=%d", id);
		int sex = YYXStruct::getMapInt64(App::GetInstance()->myData, birthdaykey, -999);
		string birthday = YYXStruct::getMapString(App::GetInstance()->myData, birthdaykey, "");
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildSex", sex);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildBirthday", -999, birthday);
		App::GetInstance()->m_read->childrenId = id;		
		YYXLayer::setFileValue("ShowChildBirthday", birthday);
		YYXLayer::setFileValue("ShowChildSex", StringUtils::format("%d", sex));
	}
	else if (index == 1)
		id = YYXStruct::getMapInt64(App::GetInstance()->myData, "secondChildID", -999);
	else
		id = YYXStruct::getMapInt64(App::GetInstance()->myData, "threeChildID", -999);
	//名字
	string name = App::getString("DIANJITIANJIAHAIZI");
	if (id != -999)
	{
		auto nameKey = StringUtils::format("name+childID=%d", id);
		name = YYXStruct::getMapString(App::GetInstance()->myData, nameKey, "");
		if (index == 0) {
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildName", -999, name);
			YYXLayer::setFileValue("ShowChildName", name);
		}
	}
	//头像url
	string url = "";
	if (id != -999)
	{
		string urlKey = StringUtils::format("url+childID=%d", id);
		url = YYXStruct::getMapString(App::GetInstance()->myData, urlKey, "");		
	}
	//头像path
	string path = "";
	if (id != -999)
	{
		string pathKey = StringUtils::format("path+childID=%d", id);
		path = YYXStruct::getMapString(App::GetInstance()->myData, pathKey, "");
		if (index == 0) {
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, path);
			YYXLayer::setFileValue("ShowChildHeadPortrait", path);
		}
	}
	App::log("------------------------------------*------*---*-*-*-*-*-*-*-index  = " , index);
	App::log("id = ", id);
	App::log("name = " + name);
	App::log("path = " + path);
	App::log("url = " + url);
	//删除按钮
	if (deleteButton)
	{
		auto editStats = YYXStruct::getMapInt64(App::GetInstance()->myData, "BabyCenterChangeChildEditButton", 1);
		if (editStats == 0)
			deleteButton->setVisible(false);
		else
		{
			if (index != 0)
				deleteButton->setVisible(true);			
		}
		if (id == -999)
			deleteButton->setVisible(false);
		if (index != 0) {
			deleteButton->addClickEventListener([=](Ref* sender) {
				YYXLayer::controlTouchTime(1, "FIND_BUTTON_BYNAME_BABYCENTER_CHANGECHILD_CHILD_DELETETime", [=]() {
					auto messagebox = YYXLayer::MyMessageBox(App::getString("QUEDINGSHANCHU"), App::getString("QUEDING"), [=]()
					{
						string runkey = StringUtils::format("babyCenterScenehttpDeleteChildSuccess_%d", (int)YYXLayer::getRandom());
						string errorkey = StringUtils::format("babyCenterScenehttpDeleteChildFail_%d", (int)YYXLayer::getRandom());
						NetIntface::httpDeleteChild(App::GetInstance()->m_me->id, id, runkey, [=](string json) {
							//宝贝删除成功
							Toast::create(App::getString("SHANCHUBAOBEICHENGGONG"));
							string runkey1 = StringUtils::format("BabyCenterSceneHttpGetChildDetailsSuccess2_%d", (int)YYXLayer::getRandom());
							string errorkey1 = StringUtils::format("BabyCenterSceneHttpGetChildDetailsFail2_%d", (int)YYXLayer::getRandom());
							NetIntface::httpGetChildDetails(App::GetInstance()->m_me->id, runkey1, [=](string json) {
								getChildDetailsBusinessLogic(json, []() {});
							}, errorkey1, [](string str) {
								Toast::create(App::getString("SHUAXINSHIBAI"));
							});
						}, errorkey, [](string str) {
							Toast::create(App::getString("SHANCHUBAOBEISHIBAI"));
						});
					}, App::getString("QUXIAO"), []() {
						Toast::create(App::getString("QUXIAOCAOZUO"));
					});
					if(messagebox)
						addChild(messagebox,11);
				});
			});
			auto lister1 = EventListenerCustom::create("BabyCenterSceneChangChildShowDeleteButton", [=](EventCustom* e) {
				if (id == -999)
					return;
				if (deleteButton)
					deleteButton->setVisible(true);
			});
			auto lister2 = EventListenerCustom::create("BabyCenterSceneChangChildSetUNvisibleDeleteButton", [=](EventCustom* e) {
				if (deleteButton)
					deleteButton->setVisible(false);
			});
			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister1, node);
			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lister2, node);
		}
	}
	//名字
	if (textName)
		textName->setText(name);
	//头像	
	if (headPortrait)
	{
		headPortrait->loadTexture(IAMGE_BABYCENTER_CHANGECHILD_CHILD_HAVENOTCHILD);
		if (id == -999)
		{
			
		}
		else
		{
			if (path != "" && FileUtils::getInstance()->isFileExist(path))
					headPortrait->loadTexture(path);
			else
			{
				string savePath = FileUtils::getInstance()->getWritablePath() + StringUtils::format("temp/childHead_%d.png", id);
				if (FileUtils::getInstance()->isFileExist(savePath)) 
				{
					headPortrait->loadTexture(savePath);
					string pathKey = StringUtils::format("path+childID=%d", id);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathKey, YYXLayer::getCurrentTime4Second(), savePath);
				}
				else
				{
					headPortrait->loadTexture(IAMGE_BABYCENTER_CHANGECHILD_CHILD_HEADPORTRAIT);
					if (url != "")
					{
						auto dir = FileUtils::getInstance()->getWritablePath() + "temp";
						auto fileName = StringUtils::format("7HeadPortrait_%d.png", id);
						NetIntface::DownLoadImage(url, dir, fileName, StringUtils::format("Download%d", (int)YYXLayer::getRandom()), [=](string path) {							
							if (path != "" && FileUtils::getInstance()->isFileExist(path))
							{
								string savePath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("childHead_%d.png", id);
								App::makeRoundImage(path, savePath);
								auto key = StringUtils::format("childID=%d_ChangeChildSceneDownLoadSuccess", id);
								string pathKey = StringUtils::format("path+childID=%d", id);
								YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathKey, YYXLayer::getCurrentTime4Second(), savePath);
								YYXLayer::sendNotify(key);
							}
						}, "", [](string str) {});
					}
					else
					{
						App::log(StringUtils::format("ChildID = %d, Index = %d, HeadPortrait4URL = null", id, index));
						string pathKey = StringUtils::format("path+childID=%d", id);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathKey, YYXLayer::getCurrentTime4Second(), IAMGE_BABYCENTER_CHANGECHILD_CHILD_HEADPORTRAIT);
					}
				}
			}
		}
		auto listener1 = EventListenerCustom::create(StringUtils::format("childID=%d_ChangeChildSceneDownLoadSuccess", id), [=](EventCustom* e) {
			string pathKey = StringUtils::format("path+childID=%d", id);
			auto path = YYXStruct::getMapString(App::GetInstance()->myData, pathKey, "");
			if (path != "" && FileUtils::getInstance()->isFileExist(path))
			{
				headPortrait->loadTexture(path);
				string pathkey = StringUtils::format("path+childID=%d", id);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathkey, YYXLayer::getCurrentTime4Second(), path);
				if (index == 0)
				{
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, path);
					YYXLayer::setFileValue("ShowChildHeadPortrait", path);
				}
			}			
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener1, node);
		headPortrait->setTouchEnabled(true);
		headPortrait->addClickEventListener([=](Ref* sender) {
			YYXLayer::controlTouchTime(1, "ChangeHeadPortraitClickTime", [=]() {				
				if (id == -999)
				{
					auto addchildLayer = initAddChild();
					addChild(addchildLayer);
				}
				else
				{//选中的孩子 将信息全部赋值给show系列变量并保存到本地xml
					//id
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", id);
					YYXLayer::setFileValue("ShowChildID", StringUtils::format("%d", id));
					YYXLayer::sendNotify("qiehuanhaiziAction");
				}
			});
		});
	}	
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

////网络请求修改宝贝信息
//void BabyCenter::modifyBabyInfo(int child_id, string newName, int newSex, string newBirthday) {
//	App::log("BabyCenter::modifyBabyInfo");
//	if (!CocosAndroidJni::IsNetConnect())
//	{
//		Toast::create("网络异常");
//		notifyModifyFailed();
//		return;
//	}
//	HttpRequest* request = new HttpRequest();
//	request->setRequestType(HttpRequest::Type::POST);
//	request->setUrl(std::string(IP).append(NET_MODIFY_CHILD_INFO).c_str());
//	string postData = "childrenId=" + StringUtils::format("%d", App::GetInstance()->m_read->childrenId) + "&childrenName=" + newName + "&childrenSex=" +
//		StringUtils::format("%d", newSex) + "&childrenBirth=" + newBirthday;
//	request->setRequestData(postData.c_str(), strlen(postData.c_str()));
//
//	//请求的回调函数
//	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
//		if (!response) {
//			notifyModifyFailed();
//			return;
//		}
//		if (!response->isSucceed()) {
//			notifyModifyFailed();
//			log("response failed! error buffer: %s", response->getErrorBuffer());
//			return;
//		}
//
//		//json解析
//		std::vector<char> *buffer = response->getResponseData();
//		std::string str(buffer->begin(), buffer->end());
//		std::thread callback(&BabyCenter::modifyBabyInfoCallback, this, str, child_id, newName, newSex, newBirthday);
//		callback.detach();
//	});
//	cocos2d::network::HttpClient::getInstance()->send(request);
//	request->release();
//	App::log("BabyCenter::modifyBabyInfo----END");
//}
//
//void BabyCenter::modifyBabyInfoCallback(string str, int child_id, string newName, int newSex, string newBirthday) {
//	App::log("BabyCenter::modifyBabyInfoCallback");
//	App::log(str);
//	rapidjson::Document doc;
//	doc.Parse<0>(str.c_str());
//	if (doc.HasParseError()) {//json parse error
//		notifyModifyFailed();
//		return;
//	}
//
//	if (!doc.IsObject() || !doc.HasMember("code")) {
//		notifyModifyFailed();
//		return;
//	}
//
//	rapidjson::Value &value = doc["code"];
//	if (value.IsInt() && value.GetInt() == 0) {//请求修改正常
//											   //更新数据库
//		/*sqlite3* myDB = SqliteManager::OpenDataBase();
//		if (SqliteManager::IsRecExist(myDB, DB_CHILDREN, StringUtils::format("where children_id=%d", child_id))) {
//			unordered_map<string, string> paraInfoSTR;
//			unordered_map<string, long long> paraInfoINT;
//			paraInfoINT["children_sex"] = newSex;
//			paraInfoINT["upTime"] = App::getCurrentTime();
//			paraInfoSTR["children_name"] = newName;
//			paraInfoSTR["children_birth"] = newBirthday;
//			SqliteManager::UpdateData(myDB, DB_CHILDREN, paraInfoSTR, paraInfoINT, StringUtils::format("where children_id=%d", child_id));
//		}
//		SqliteManager::CloseDataBase(myDB);*/
//
//		//修改成功
//		notifyModifySuccess();
//	}
//	else {
//		notifyModifyFailed();
//	}
//	App::log("BabyCenter::modifyBabyInfoCallback---END");
//}

//上传头像
void BabyCenter::uploadAvatar() {
	App::log("BabyCenter::uploadAvatar");
	auto showID = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
	if (showID == -999)
	{
		App::GetInstance()->pushScene(BabyCenterScene);
		Index::GoToLoginScene();
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([]() {
			Toast::create(App::getString("DENGLUSHIXIAOCHONGXINDENGLU"));
		});
	}
	string selectPath = YYXStruct::getMapString(App::GetInstance()->myData, "selectPath", "");
	if (selectPath == "" || !FileUtils::getInstance()->isFileExist(selectPath))
	{
		Toast::create(App::getString("SHANGCHUANDETOUXIANGTUPIANYICHANG"));
		return;
	}
	auto memberid = App::GetInstance()->m_me->id;
	string runkey = StringUtils::format("httpUpImageSuccess_%d",(int)YYXLayer::getRandom());
	string errorkey = StringUtils::format("httpUpImageFail_%d", (int)YYXLayer::getRandom());
	NetIntface::httpUpImage(showID, selectPath, memberid, runkey, [=](string fullpath) {
		string selectCutRoundPath = YYXStruct::getMapString(App::GetInstance()->myData, "selectCutRoundPath", "");
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, selectCutRoundPath);
		YYXLayer::setFileValue("ShowChildHeadPortrait", selectCutRoundPath);
		string pathkey = StringUtils::format("path+childID=%d",showID);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathkey, YYXLayer::getCurrentTime4Second(), selectCutRoundPath);
		Toast::create(App::getString("SHANGCHUANTOUXIANGCHEGNGONG"));
	}, errorkey, [](string str) {
		Toast::create(App::getString("STR_UPPICTURE_ERROR"));
	});
	App::log("BabyCenter::uploadAvatar---END");
}

////通知修改宝贝信息成功
//void BabyCenter::notifyModifySuccess() {
//	App::log("BabyCenter::notifyModifySuccess");
//	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
//		if (!Director::getInstance()->getEventDispatcher()->isEnabled()) {
//			Director::getInstance()->getEventDispatcher()->setEnabled(true);
//		}
//		EventCustom eventModifySuccess("MODIFY_BABY_INFO_SUCCESS");
//		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventModifySuccess);
//	});
//	App::log("BabyCenter::notifyModifySuccess----END");
//}
//
////通知修改宝贝信息失败
//void BabyCenter::notifyModifyFailed() {
//	App::log("BabyCenter::notifyModifyFailed");
//	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
//		if (!Director::getInstance()->getEventDispatcher()->isEnabled()) {
//			Director::getInstance()->getEventDispatcher()->setEnabled(true);
//		}
//		EventCustom eventModifyFailed("MODIFY_BABY_INFO_FAILED");
//		Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventModifyFailed);
//	});
//	App::log("BabyCenter::notifyModifyFailed----END");
//}


/*void BabyCenter::getChildDetailsCallback(std::string json)
{
	App::log("BabyCenter::getChildDetailsCallback");
	if (App::GetInstance()->m_RunningScene != BabyCenterScene)
	{
		return;
	}
	rapidjson::Document doc;
	doc.Parse<0>(json.c_str());
	if (doc.HasParseError())//json parse error
	{
		return;
	}
	if (!doc.IsObject() || !doc.HasMember("code") || !doc.HasMember("data"))
	{
		return;
	}
	rapidjson::Value &value = doc["code"];
	if (value.IsInt() && value.GetInt() == 0) 
	{//请求正常
		rapidjson::Value &data = doc["data"];
		if (!data.IsArray())
		{
			return;
		}
		//解析出孩子列表信息 然后赋值到内存
		m_data.clear();
		for (rapidjson::SizeType i = 0; i < data.Size(); i++) 
		{
			rapidjson::Value &pData = data[i];
			auto children_id = App::analysisJsonInt(pData, "childrenId");
			auto children_sex = App::analysisJsonInt(pData, "childrenSex");
			auto children_name = App::analysisJsonString(pData, "childrenName");
			auto children_birth = App::analysisJsonString(pData, "childrenBirth");
			auto avatarUrl = App::analysisJsonString(pData, "avatarUrl");
			if (i == 0)
			{
				App::GetInstance()->m_read->childrenId = children_id;
				App::log("+++++++++++++++++++++++++childid = ", children_id);
			}
			map<string, ParaType> data;
			App::initParaType(data, "ChildId", children_id, "");
			App::initParaType(data, "ChildSex", children_sex, "");
			App::initParaType(data, "ChildName", -999, children_name);
			App::initParaType(data, "ChildBirth", -999, children_birth);
			App::initParaType(data, "ChildAvatarUrl", -999, avatarUrl);
			m_data[children_id] = data;
		}
		App::log("BabyCenter::getChildDetailsCallback---childrenId = ", App::GetInstance()->m_read->childrenId);
		//线程安全,babycenter刷新		
		App::log("babycenter刷新");
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([]
		{
			EventCustom eventRefreshChildInfo("REFRESH_CHILD_INFO4HTTP");
			Director::getInstance()->getEventDispatcher()->setEnabled(true);
			Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventRefreshChildInfo);
		});
	}
	App::log("BabyCenter::getChildDetailsCallback---END");
}*/

//下载孩子头像的回调
void BabyCenter::CallBackDownLoadChildPortrait(string json)
{
	App::log(" BabyCenter::CallBackDownLoadChildPortrait" );
	/*if (App::GetInstance()->m_RunningScene != MySceneName::BabyCenterScene)
	{
		return;
	}
	rapidjson::Document doc;
	doc.Parse<0>(json.c_str());
	if (doc.HasParseError())//json parse error
	{
		return;
	}
	if (!doc.IsObject() || !doc.HasMember("ChildId") || !doc.HasMember("Path"))
	{
		return;
	}
	rapidjson::Value &ChildId = doc["ChildId"];
	rapidjson::Value &Path = doc["Path"];
	int childid = -999;
	string path = "";
	if (ChildId.IsInt())
	{
		childid = ChildId.GetInt();
	}
	if (Path.IsString())
	{
		path = Path.GetString();
	}
	if (FileUtils::getInstance()->isFileExist(path)  && childid != -999)
	{
		App::GetInstance()->m_me->showPortraitPath = path;
		if (m_data.find(childid) != m_data.end())
		{
			map<string, ParaType> data;
			App::initParaType(data, "Path", -999, path);
			App::initParaType(data, "ChildId", childid, "");
			m_data[childid] = data;
		}
		else
		{
			auto data = m_data[childid];
			App::initParaType(data, "Path", -999, path);
		}*/
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([]
		{
			EventCustom eventRefreshChildInfo("showPortrait");
			Director::getInstance()->getEventDispatcher()->setEnabled(true);
			Director::getInstance()->getEventDispatcher()->dispatchEvent(&eventRefreshChildInfo);
		});
	//}
	App::log(" BabyCenter::CallBackDownLoadChildPortrait---END");
}

bool BabyCenter::showPortrait(ImageView* imgview)
{
	App::log("showPortrait");
	auto ShowChildHeadPortrait = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildHeadPortrait","");
	//展示头像
	if (ShowChildHeadPortrait != "" && FileUtils::getInstance()->isFileExist(ShowChildHeadPortrait))
	{
		if(imgview)
			imgview->loadTexture(ShowChildHeadPortrait);
		return true;
	}
	else
	{
		if(imgview)
			imgview->loadTexture(IAMGE_BABYCENTER_CHANGECHILD_CHILD_HEADPORTRAIT);
		return false;
	}
	/*//头像先数据源,然后找数据库
	bool findDataSour = false;
	if (App::GetInstance()->m_me == nullptr) {
		imgview->loadTexture(PICTURE_INDEX_PORTRAIT_NO_LOGIN, TextureResType::PLIST);
	}
	else
	{
		auto path = YYXStruct::getMapString(App::GetInstance()->myData, "firstChildHeadPortrait", "");
		if (&path && FileUtils::getInstance()->isFileExist(path))
		{
			imgview->loadTexture(path);
			findDataSour = true;
		}
		auto data = App::GetInstance()->m_me->showPortraitPath;
		if (!findDataSour && &data && FileUtils::getInstance()->isFileExist(data))
		{
			imgview->loadTexture(data);
			findDataSour = true;
		}
		if (!findDataSour)
		{
			int childId = -9999;
			if (App::GetInstance()->m_read)
				childId = App::GetInstance()->m_read->childrenId;
			string strid = StringUtils::format("%d", childId);
			vector<string> paraStr;
			paraStr.push_back("path");
			vector<unordered_map<string, ParaType>> result = SqliteManager::SelectData(App::sqliteOpen(), DB_RES, paraStr, "where bookId=" + strid + " and status=8");
			if (result.size() > 0)
			{
				auto fullpath = result[0]["path"].stringPara;
				if (FileUtils::getInstance()->isFileExist(fullpath))
				{
					App::GetInstance()->m_me->showPortraitPath = fullpath;
					imgview->loadTexture(fullpath);
					findDataSour = true;
				}
			}
		}
		if (!findDataSour)
		{

			imgview->loadTexture(PICTURE_INDEX_PORTRAIT, TextureResType::PLIST);
			App::GetInstance()->m_me->showPortraitPath = PICTURE_INDEX_PORTRAIT;
		}
	}*/
}

void BabyCenter::getChildDetailsBusinessLogic(string json,function<void()> errorRunable)
{
	//孩子详情回调
	App::log("BaByCenterSceneGetChildDetailsSuccess ---Begin");
	NetIntface::httpGetChildDetailsCallBack(json, [=](int index, int childrenId, int childrenSex, string childrenName, string childrenBirth, string url, long long uptime)
	{
		string namekey = StringUtils::format("name+childID=%d", childrenId);
		string pathkey = StringUtils::format("path+childID=%d", childrenId);
		string birthdaykey = StringUtils::format("birthday+childID=%d", childrenId);
		string urlkey = StringUtils::format("url+childID=%d", childrenId);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, namekey, childrenId, childrenName);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathkey, uptime);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, birthdaykey, childrenSex, childrenBirth);
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, urlkey, 0, url);
		if (index == 0)
		{
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "firstChildID", childrenId);
			auto id = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
			if (id == -999)
			{
				//如果没有设置选中的孩子 默认第一个孩子就是选中的孩子
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", childrenId);	
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildName", -999, childrenName);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildSex", childrenSex);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildBirthday", -999,childrenBirth);
				setBabyInfoToXml(childrenName, childrenBirth, childrenSex, childrenId);
				string dir = FileUtils::getInstance()->getWritablePath() + "temp";
				string fileName = StringUtils::format("6HeadPortrait_%d.png", childrenId);
				if (!FileUtils::getInstance()->isFileExist(dir + "/" + fileName))
				{
					NetIntface::DownLoadImage(url, dir, fileName, StringUtils::format("DownLoadImage%d", YYXLayer::getRandom()), [=](string path) {
						if (path != "" && FileUtils::getInstance()->isFileExist(path))
						{
							string savePath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("childHead_%d.png", childrenId);
							App::makeRoundImage(path, savePath);
							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, savePath);
							YYXLayer::setFileValue("ShowChildHeadPortrait", savePath);
							YYXLayer::sendNotify("BaByCenterSceneChildInfoReferHeadPortrait");
							string pathKey = StringUtils::format("path+childID=%d", childrenId);
							YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathKey, YYXLayer::getCurrentTime4Second(), savePath);
						}
					}, "", [](string str) {});
				}
			}
			else
			{
				//如果有设置选中的孩子 将数据更新一次
				if (id == childrenId)
				{
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildName", -999, childrenName);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildSex", childrenSex);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildBirthday", -999, childrenBirth);
				}
			}
		}
		if (index == 1)
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "secondChildID", childrenId);
		if (index == 2)
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "threeChildID", childrenId);
		App::log("解析孩子列表回调" + childrenName, childrenId);
	}, [=](int result) {
		//解析前
		if (result == 0)
		{
			YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "firstChildID");
			YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "secondChildID");
			YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "threeChildID");
		}
		//解析后
		else if (result  == 1)
		{
			auto id = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
			if (id != -999)
			{
				auto name = getStringFrommyData(id, "name");
				auto birthday = getStringFrommyData(id, "birthday");
				auto sex = getIntFrommyData(id, "birthday");
				setBabyInfoToXml(name, birthday, sex, id);
			}
		}
		//解析错误
		else if (result ==2)
		{
			App::log("解析错误");
			if (errorRunable)
				errorRunable();
		}
	});
	YYXLayer::sendNotify("BaByCenterSceneChildInfoshowFirstChild");
	YYXLayer::sendNotify("qiehuanhaiziAction");
	App::log("BaByCenterSceneGetChildDetailsSuccess ---END");
}

string BabyCenter::getStringFrommyData(int id,string key)
{
	string pathKey = StringUtils::format("%s+childID=%d", key.c_str(),id);
	auto path = YYXStruct::getMapString(App::GetInstance()->myData, pathKey,  "");
	return path;
}

int BabyCenter::getIntFrommyData(int id, string key)
{
	string pathKey = StringUtils::format("%s+childID=%d", key.c_str(), id);
	auto path = YYXStruct::getMapInt64(App::GetInstance()->myData, pathKey, -999);
	return path;
}

void BabyCenter::setBabyInfoToXml(string name, string birthday, int sex, int id)
{
	if (id != -999)
	{
		YYXLayer::setFileValue("ShowChildID", StringUtils::format("%d", id));
	}
	YYXLayer::setFileValue("ShowChildName", name);
	YYXLayer::setFileValue("ShowChildSex", StringUtils::format("%d", sex));
	YYXLayer::setFileValue("ShowChildBirthday", birthday);
}