#include "LoginControl.h"
#include "CrossPlatform.h"
#include "XZLayer.h"
#include "YYXVisitor.h"
#include "Charger.h"
#include "YYXDownloadImages.h"
#include "User.h"
#include "AppHttp.h"
#include "HttpWaiting.h"
#include "vipNotifyLayer.h"
#include "BuyVip.h"
#include "MyBook.h"
#include "ReadBook.h"
#include "DownloadBook.h"

LoginControl* LoginControl::instance = nullptr;

LoginControl::LoginControl()
{

}

LoginControl::~LoginControl()
{

}

LoginControl* LoginControl::getInstance()
{
	if (instance == nullptr)
	{
		instance = new LoginControl();
	}
	return instance;
}

void LoginControl::Login(CallBack_String call1, CallBack_String call2)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)	
	map<string, string> p;
	auto cData = CrossPlatformData::create();
	if (call1)
		cData->setCall1Key(call1);
	if (call2)
		cData->setCall2Key(call2);
	p["task"] = Value(cData->getTaskName()).asString();
	auto strJson = YYXLayer::getStringFormMap(p);
	CrossPlatform::getInstance()->addTask(cData);
	string result = "";
	CocosAndroidJni::callJavaFunction("org/cocos2dx/cpp/AppActivity", "loginActivity", strJson, result);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	auto control = ControlScene::getInstance();
	control->replaceScene(ControlScene::getInstance()->getSceneInfo(ParentScene), ControlScene::getInstance()->getSceneInfo(LoginScene));
#endif
}

//原始返回json解析 进行赋值登录
void LoginControl::LoginCallback(string json)
{
	rapidjson::Document doc;
	bool result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		int memberId = YYXLayer::getIntForJson(-999, doc, "memberId");
		string balanceAmount = YYXLayer::getStringForJson("0", doc, "balanceAmount");
		string memberProvince = YYXLayer::getStringForJson(App::getString("BEIJING"), doc, "memberProvince");
		string memberCity = YYXLayer::getStringForJson(App::getString("BEIJING"), doc, "memberCity");
		string memberName = YYXLayer::getStringForJson("", doc, "memberName");
		int memberSex = YYXLayer::getIntForJson(0, doc, "memberSex");
		string qq = YYXLayer::getStringForJson("", doc, "qq");
		string weixin = YYXLayer::getStringForJson("", doc, "weixin");
		string sina = YYXLayer::getStringForJson("", doc, "sina");
		if (memberId > 0)
		{
			YYXVisitor::getInstance()->setVisitorMode(false);
			auto user = User::getInstance();
			user->setMemberId(memberId);
			user->setUserCity(memberCity);
			user->setUserProvince(memberProvince);
			user->setSex(memberSex);
			user->setUserAccount(memberName);
			user->setMoney(Value(balanceAmount).asDouble() * 100);
			if (!qq.empty())
				user->setQqBind(qq);
			else
				user->setQqBind("0");
			if (!weixin.empty())
				user->setWeixinBind(weixin);
			else
				user->setWeixinBind("0");
			if (!sina.empty())
				user->setSinaBind(sina);
			else
				user->setSinaBind("0");
			App::loginCallback();
		}
	}
}

void LoginControl::Logout()
{
	User::deleteInstance();
	string musicClose = YYXLayer::getFileValue("musicClose", "true");
	string vipnotifyDay = YYXLayer::getFileValue("vipnotify", "0");
	//删除本地及内存账号信息记录
	App::GetInstance()->myData.clear();
	//删除全部红包
	App::GetInstance()->m_redPacket.clear();
	//暂停所有下载任务
	YYXDownload::getInstance()->pauseAll();
	YYXDownload::getInstance()->deleteAll();
	YYXLayer::DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "temp");
	//删除本地用户信息
	YYXLayer::DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "Data");
	//清空已购列表, 收藏 , 包年图书
	App::GetInstance()->myBookURLMap.clear();
	App::GetInstance()->myBuyBook.clear();
	App::GetInstance()->VIPbook.clear();
	//App::GetInstance()->bookCollect.clear();
	BookCache::getInstance()->clear();
	//清空书房
	DownloadBook::getInstance()->clearBook();
	ReadBook::getInstance()->clearBook();
	MyBook::getInstance()->clearBook();
	//内存
	BuyVip::destoryInstance();
	VipNotify::destoryInstance();
	AppHttp::getInstance()->clearAllReadyTask();
	//创建temp目录
	Load::initDir();
	YYXLayer::setFileValue("musicClose", musicClose);
	YYXLayer::setFileValue("vipnotify", vipnotifyDay);

	YYXVisitor::getInstance()->loginVisitor();
}
