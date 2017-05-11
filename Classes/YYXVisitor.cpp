#include "YYXVisitor.h"
#include "CrossPlatform.h"
#include "App.h"
#include "YYXDownloadImages.h"
#include "DownloadBook.h"
#include "ReadBook.h"
#include "MyBook.h"
#include "BookCache.h"
#include "User.h"
#include "AppHttp.h"
#include "YYXTime.h"

YYXVisitor* YYXVisitor::instance = nullptr;

YYXVisitor::YYXVisitor()
{
	VisitorMode = 1;//0=游客模式 1=非游客模式
	m_dirpath = FileUtils::getInstance()->getWritablePath() + "visitor";
}


YYXVisitor::~YYXVisitor()
{
}

YYXVisitor* YYXVisitor::getInstance()
{
	if (instance == nullptr)
	{
		instance = new YYXVisitor();
	}
	return instance;
}

void YYXVisitor::setVisitorMode(bool sets)
{
	if (sets)
		VisitorMode = 0;
	else
		VisitorMode = 1;
}

bool YYXVisitor::getVisitorMode()
{
	bool sets = false;
	if (VisitorMode == 0)
	{
		sets = true;
		App::log(App::getString("YOUKE") + string(" Mode"));
	}
	else
		sets = false;
	return sets;
}

void YYXVisitor::loadLocationVisitorData()
{
	//加载vip包年书籍列表
	App::loadvipBookCache();
	//载入购书信息
	App::loadBuyBookCache();

	MyBook::getInstance()->loadCache();
	//载入最近阅读时间
	ReadBook::getInstance()->loadCache();
	//载入最近下载时间
	DownloadBook::getInstance()->loadCache();
}

void YYXVisitor::initDir()
{
	if (!FileUtils::getInstance()->isDirectoryExist(m_dirpath))
		FileUtils::getInstance()->createDirectory(m_dirpath);
}

void YYXVisitor::hintLogin(function<void()> youke, function<void()> toLogin, function<void()> user,string visitorStr, bool Notime)
{
	if (getVisitorMode())
	{
		auto time = YYXStruct::getMapInt64(App::GetInstance()->myData, "hintLoginTime", 0);
		auto nowtime = YYXLayer::getCurrentTime4Second() - time;
		if (nowtime < 30 && Notime)
		{
			if (youke)
				youke();
			return;
		}
		map<string, int64String> parameter;
		YYXLayer::insertMap4ParaType(parameter, "className", -999, "hintVisitorLogin");
		YYXLayer::insertMap4ParaType(parameter, "csb", -999, "MessageBox/csb/hintVisitorLogin.csb");
		auto layer = YYXLayer::create(parameter);
		if (!layer)
			return;

		auto background = (ImageView*)layer->findControl("Image_1");
		background->setTouchEnabled(true);
		if (background)
		{
			background->addClickEventListener([=](Ref* sender) {
				layer->removeFromParentAndCleanup(true);
			});
		}
		auto visitor = (Button*)layer->findControl("Button_1");
		if (visitorStr != "")
		{
			visitor->setTitleText(visitorStr);
		}
		if (visitor)
			visitor->addClickEventListener([=](Ref* sender) {
			layer->removeFromParentAndCleanup(true);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "hintLoginTime", YYXLayer::getCurrentTime4Second());
			if (youke)
				youke();
		});
		auto login = (Button*)layer->findControl("Button_2");
		if (login)
			login->addClickEventListener([=](Ref* sender) {
			layer->removeFromParentAndCleanup(true);
			if (toLogin)
				toLogin();
		});
		auto close = (Button*)layer->findControl("Button_5");
		if (close)
			close->addClickEventListener([=](Ref* sender) {
			layer->removeFromParentAndCleanup(true);
		});
		layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
		layer->getParentNode()->setPosition(Director::getInstance()->getVisibleSize() / 2);
		if (layer)
		{
			auto lay = Director::getInstance()->getRunningScene()->getChildByTag(9); 
			if(lay)
				lay->addChild(layer);
			else
				Director::getInstance()->getRunningScene()->addChild(layer);
		}
	}
	else
	{//正常账号
		if (user)
			user();
	}
}

void YYXVisitor::indexSceneInit(ImageView* portrait)
{
	meIsNull();
	if (getVisitorMode())
	{
		portrait->loadTexture(PICTURE_INDEX_PORTRAIT_NO_LOGIN, TextureResType::PLIST);
	}
}

void YYXVisitor::bookRoomSceneInit()
{
	meIsNull();
}

void YYXVisitor::bookCitySceneInit()
{
	meIsNull();
}

void YYXVisitor::bookStoreSceneInit()
{
	meIsNull();
}

void YYXVisitor::baoNianBookStoreSceneInit(function<void()> visitorRun, function<void()> user)
{
	if (getVisitorMode())
	{
		if(visitorRun)
			visitorRun();
	}
	else
	{
		if (user)
			user();
	}
}

void YYXVisitor::bookInfoSceneInit()
{
	meIsNull();
}

void YYXVisitor::babyCenterSceneInit(function<void()> youke, function<void()> user)//run 游客处理
{
	meIsNull();
	if (getVisitorMode())
	{
		if(youke)
			youke();
	}
	else
	{
		if (user)
			user();
	}
}

void YYXVisitor::parentSceneInit(function<void()> youke, function<void()> user)
{
	meIsNull();
	if (getVisitorMode())
	{
		if (youke)
			youke();
	}
	else
	{
		if (user)
		{
			user();
		}
	}
}

void YYXVisitor::parentSceneLogout()
{
	loginVisitor();
}

void YYXVisitor::loadSceneInit()
{	
	if (!FileUtils::getInstance()->isDirectoryExist(m_dirpath))
		FileUtils::getInstance()->createDirectory(m_dirpath);
	int memberid = -999;
	memberid = User::getInstance()->getMemberId();
	if (memberid < 0)
	{
		loginVisitor();
	}
}

void YYXVisitor::loginVisitor()
{
	//本地读取
	//1.没有本地信息网络获取	
	setVisitorMode(true);
	auto visitorMemberId = YYXLayer::getFileValue("visitorMemberId", "-999", m_dirpath);
	int memid = Value(visitorMemberId).asInt();
	if (memid < 0)
	{
		string str = CrossPlatform::getPhoneModel(3);
		if (!str.empty() && str.size() > 5)
		{
			App::log(" YYXVisitor::loginVisitor() phone Str= >" + str);
			AppHttp::getInstance()->httpVisitorMemberId(str, [=](int memberId) {
				setVisitorMode(true);
				setMemberId(memberId);
				User::getInstance()->setMemberId(-999);
			}, [=]() {
				setVisitorMode(false);
			});
		}
	}
	else
	{
		setMemberId(memid);
		User::getInstance()->setMemberId(-999);
		loadLocationVisitorData();
	}
	//2.有,加载进去
}

void YYXVisitor::logoutVisitor()
{
	setVisitorMode(false);
	string musicClose = YYXLayer::getFileValue("musicClose", "true");
	//删除本地及内存账号信息记录
	App::GetInstance()->myData.clear();
	//删除全部红包
	App::GetInstance()->m_redPacket.clear();
	//暂停所有下载任务
	YYXDownload::getInstance()->pauseAll();
	//清空已购列表, 收藏 , 包年图书
	App::GetInstance()->myBookURLMap.clear();
	App::GetInstance()->myBuyBook.clear();
	App::GetInstance()->VIPbook.clear();	
	//书籍缓存
	BookCache::getInstance()->clear();
	//清空书房
	DownloadBook::getInstance()->clearBook();
	ReadBook::getInstance()->clearBook();
	MyBook::getInstance()->clearBook();

	YYXLayer::setFileValue("musicClose", musicClose);
}

//判断本地账号是否是空的, 如果是空的就登入游客
void YYXVisitor::meIsNull()
{
	if (User::getInstance()->getMemberId() < 0)
	{
		App::log("************************* ====YYXVisitor::meIsNull()=====>"+ User::getInstance()->getMemberId());
		loginVisitor();
	}
}

string YYXVisitor::getVisitorName()
{
	string name = "Y"+Value(getMemberId()).asString();
	int lenth = 11- name.size();
	if(lenth > 0)
	{
		int length = pow(10, lenth);
		name.append(Value(YYXTime::getInstance()->getRandomL() % length).asString());
	}
	else if(lenth < 0)
	{
		name = name.substr(0, 11);
	}
	return name;
}

int YYXVisitor::getMemberId()
{
	if (memberId < 0)
	{
		memberId = Value(YYXLayer::getFileValue("visitorMemberId", "0", m_dirpath)).asInt();
	}
	return memberId;
}

void YYXVisitor::setMemberId(int val)
{
	memberId = val;
	YYXLayer::setFileValue("visitorMemberId", Value(memberId).asString(), m_dirpath);
}

int YYXVisitor::getMoney()
{
	if (money < 0)
	{
		money = Value(YYXLayer::getFileValue("visitorBalance", "0", m_dirpath)).asInt();
	}
	return money;
}

void YYXVisitor::setMoney(int val)
{
	money = val;
	YYXLayer::setFileValue("visitorBalance", Value(money).asString(), m_dirpath);
}
