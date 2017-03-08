#include "YYXVisitor.h"
#include "NetIntface.h"
#include "App.h"
#include "YYXDownloadImages.h"

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
		instance = new YYXVisitor();
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

void YYXVisitor::httpGetVisitorMemberId(string deviceStr)
{
	string url = string(IP).append("/ellabook-server/member/TouristRegister");
	map<string, string> parameter;
	parameter["equipmentCode"] = deviceStr;
	parameter["resource"] = App::m_resource;
	NetIntface::httpPost(url, parameter, "", [=](string json) {
		rapidjson::Document doc;
		YYXLayer::getJsonObject4Json(doc, json);
		auto result = YYXLayer::getBoolForJson(false, doc, "result");
		if (result)
		{
			setVisitorMode(true);
			auto memberId = YYXLayer::getIntForJson(-999, doc, "memberId");
			if (memberId >0)
			{
				App::GetInstance()->m_me = new MyAccount();
				App::GetInstance()->m_me->id = memberId;
				YYXLayer::setFileValue("visitorMemberId", App::getMemberID(), m_dirpath);
				//网络获取用户全部信息
				httpGetVisitorInfo(false);
			}
		}
		else
			setVisitorMode(false);
	}, "", [=](string str) {
		setVisitorMode(false);
	});
}



void YYXVisitor::httpGetVisitorInfo(bool hint)
{
	//查询用户包年服务信息
	httpCheckVIP(hint);
	//本地读取vip包年图书列表(租书列表),
	httpGetVipBook(hint);
	//网络请求收藏列表
	httpGetCollectBook(hint);
	//获取用户已购列表
	httpGetBuyBook(hint);
	//获取孩子信息
	httpGetChildren(hint);
	//查询余额
	httpGetBalance(hint);
}

//获取包年服务信息
void YYXVisitor::httpCheckVIP(bool hint)
{
	string url = string(IP).append(NET_CHECHVIP);
	map<string, string> par;
	par["memberId"] = App::getMemberID();
	NetIntface::httpPost(url, par, "", [=](string json) {
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			if (YYXLayer::getBoolForJson(false, doc, "result"))
			{
				if (App::GetInstance()->m_me == nullptr)
					return;
				if (YYXLayer::getBoolForJson(false, doc, "isVip"))
				{					
					App::GetInstance()->m_me->vip = true;
					auto startTime = YYXLayer::getInt64ForJson(0, doc, "startTime");
					auto expireTime = YYXLayer::getInt64ForJson(0, doc, "expireTime");
					auto times = YYXLayer::getInt64ForJson(0, doc, "times");
					App::GetInstance()->m_me->startvip = YYXLayer::getStringTimeFromInt64Time(startTime);
					App::GetInstance()->m_me->endvip = YYXLayer::getStringTimeFromInt64Time(expireTime);
					App::GetInstance()->m_me->vipTime = times;
					YYXLayer::sendNotify("showVIPRenew");//提示续费
					YYXLayer::sendNotify("refershMemberIDVIP");//提示刷新父母设置界面
				}
				else
				{
					App::GetInstance()->m_me->vip = false;
					string rentpath = FileUtils::getInstance()->getWritablePath() + StringUtils::format("vipBook/vipbook_%d.json", App::GetInstance()->m_me->id);
					FileUtils::getInstance()->removeFile(rentpath);
					App::GetInstance()->VIPbook.clear();
				}
			}
		}
	}, "", [=](string str) {
		if (hint)
			Toast::create(App::getString("HUOQUBAONIANFUWUXINXISHIBAI"));
	});
}

//获取包年图书列表
void YYXVisitor::httpGetVipBook(bool hint)
{
	NetIntface::httpBookCollectAndVipList(2, [=](string json) {
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			bool result2 = YYXLayer::getBool4Json(false, doc, "result");
			if (result2) {
				App::GetInstance()->VIPbook.clear();
				rapidjson::Value arrayData;
				YYXLayer::getJsonArray4Json(arrayData, doc, "data");
				YYXLayer::getDataForJsonArray(arrayData, [=](rapidjson::Value & item, int index) {
					auto bookid = YYXLayer::getInt4Json(-999, item, "bookId");
					auto url = YYXLayer::getString4Json("", item, "bookPlayUrl");
					auto bookCoverUrl = YYXLayer::getString4Json("", item, "bookCoverUrl");
					auto bookName = YYXLayer::getString4Json("", item, "bookName");
					App::GetInstance()->VIPbook[bookid] = (int)YYXLayer::getCurrentTime4Second();
					App::GetInstance()->myBookURLMap[bookid] = url;
					//封面下载
					string fileName = StringUtils::format("%d", bookid) + ".png";
					if (!FileUtils::getInstance()->isFileExist(App::getBookCoverPngPath(bookid)))
					{
						YYXDownloadImagesPriority priority = low;
						if (index < 6)
							priority = high;
						YYXDownloadImages::GetInstance()->newDownloadImage(bookCoverUrl, App::getCoverDir(), fileName, priority, 0, [=](string downPath) {
							YYXLayer::sendNotify("bookRoomCoverDownloadSuccess");
						},  [=](string str) {
							if (hint)
							{
								string sstr = string("<<" + bookName + ">>").append(App::getString("FENGMIANXIAZAISHIBAI"));
								Toast::create(sstr.c_str(), false);
							}
						});
						/*NetIntface::DownLoadImage(bookCoverUrl, App::getCoverDir(), fileName, "", [=](string downPath) {
							YYXLayer::sendNotify("bookRoomCoverDownloadSuccess");
						}, "", [=](string str) {
							if (hint)
							{
								string sstr = string("<<" + bookName + ">>").append(App::getString("FENGMIANXIAZAISHIBAI"));
								Toast::create(sstr.c_str(), false);
							}
						});*/
					}
				});
				YYXLayer::sendNotify("bookRoomSceneCompileChange", "", -1);
				map<string, string> data;
				for (auto it : App::GetInstance()->VIPbook)
				{
					data[StringUtils::format("%d", it.first)] = StringUtils::format("%d", it.second);
				}
				auto stri = YYXLayer::getStringFormMap(data);
				string path = FileUtils::getInstance()->getWritablePath() + "vipBook/vipbook_" + App::getMemberID() + ".json";
				YYXLayer::writeFile(stri, path);
			}
		}
	}, [=](string json) {
		if (hint)
			Toast::create(App::getString("HUOQUBAONIANSHUJILIEBIAOSHIBAI"));
	});
}

//获取收藏列表
void YYXVisitor::httpGetCollectBook(bool hint)
{
	NetIntface::httpBookCollectAndVipList(1, [=](string json) {
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			bool result2 = YYXLayer::getBool4Json(false, doc, "result");
			if (result2) {
				map<string, string> data;
				App::GetInstance()->bookCollect.clear();
				rapidjson::Value arrayData;
				YYXLayer::getJsonArray4Json(arrayData, doc, "data");
				YYXLayer::getDataForJsonArray(arrayData, [&](rapidjson::Value & item, int index) {
					auto bookid = YYXLayer::getInt4Json(-999, item, "bookId");
					App::GetInstance()->bookCollect[bookid] = (int)YYXLayer::getCurrentTime4Second();
					data[StringUtils::format("%d", bookid)] = StringUtils::format("%d", (int)YYXLayer::getCurrentTime4Second());
				});
				auto stri = YYXLayer::getStringFormMap(data);
				string path = FileUtils::getInstance()->getWritablePath() + "collectBook/collect_" + App::getMemberID() + ".json";
				YYXLayer::writeFile(stri, path);
			}
		}
	}, [=](string json) {
		if (hint)
			Toast::create(App::getString("HUOQUSHOUCANGLIEBIAOSHIBAI"));
	});
}

//网络获取购书列表
void YYXVisitor::httpGetBuyBook(bool hint)
{
	NetIntface::httpGetUserBuyBooks(App::GetInstance()->m_me->id, "", [=](string json) {
		NetIntface::httpGetUserBuyBooksCallBack(json, []() {
			//json成功, array前执行
			App::GetInstance()->myBuyBook.clear();
		}, [=](int idx, int bookId, int orderId, string bookCoverUrl, string bookPlayUrl, string bookName) {
			//解析过程
			App::GetInstance()->myBookURLMap[bookId] = bookPlayUrl;
			App::GetInstance()->myBuyBook[bookId] = orderId;
			//封面下载
			string fileName = StringUtils::format("%d", bookId) + ".png";
			if (!FileUtils::getInstance()->isFileExist(App::getBookCoverPngPath(bookId)))
			{
				YYXDownloadImagesPriority priority = low;
				if (idx < 6)
					priority = high;
				YYXDownloadImages::GetInstance()->newDownloadImage(bookCoverUrl, App::getCoverDir(), fileName, priority, 2000, [](string downpath) {
					YYXLayer::sendNotify("bookRoomCoverDownloadSuccess");
					App::log("httpGetBuyBook ===>>>" + downpath);
				}, [=](string error) {
					if (hint)
					{
						string sstr = string("<<" + bookName + ">>").append(App::getString("FENGMIANXIAZAISHIBAI"));
						Toast::create(sstr.c_str(), false);
					}
				});
				/*NetIntface::DownLoadImage(bookCoverUrl, App::getCoverDir(), fileName, "", [=](string downPath) {
					YYXLayer::sendNotify("bookRoomCoverDownloadSuccess");
				}, "", [=](string str) {
					if (hint)
					{
						string sstr = string("<<" + bookName + ">>").append(App::getString("FENGMIANXIAZAISHIBAI"));
						Toast::create(sstr.c_str(), false);
					}
				});*/
			}
		}, []() {
			//解析成功
			map<string, string> data;
			for (auto it : App::GetInstance()->myBuyBook)
			{
				data[StringUtils::format("%d", it.first)] = StringUtils::format("%d", it.second);
			}
			string str = YYXLayer::getStringFormMap(data);
			YYXLayer::writeFile(str, FileUtils::getInstance()->getWritablePath() + "buyBook/buybook_" + App::getMemberID() + ".json");
			YYXLayer::sendNotify("loginSceneHttpGetUserBuyBooksSuccess");
		}, [=]() {
			//解析错误
			if (hint)
				Toast::create(App::getString("YIGOUSHUJILIEBIAOGENGXINSHIBAI"));
		});
	}, "", [=](string str) {
		//网络错误
		if (hint)
			Toast::create(App::getString("YIGOUSHUJILIEBIAOGENGXINSHIBAI"));
	});
}

//获取孩子信息
void YYXVisitor::httpGetChildren(bool hint)
{
	NetIntface::httpGetChildDetails(App::GetInstance()->m_me->id, "", [=](string json) {
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
			string savePath = m_dirpath + StringUtils::format("/childHead_%d.png", childrenId);
			if (!FileUtils::getInstance()->isFileExist(savePath))
			{
				YYXDownloadImages::GetInstance()->newDownloadImage(url, m_dirpath, StringUtils::format("9HeadPortrait_%d.png", childrenId),
					high, 0, [=](string path) {
					if (path != "" && FileUtils::getInstance()->isFileExist(path))
					{
						string savePath = m_dirpath + StringUtils::format("/childHead_%d.png", childrenId);
						App::makeRoundImage(path, savePath);
						string pathkey = StringUtils::format("path+childID=%d", childrenId);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathkey, YYXLayer::getCurrentTime4Second(), savePath);
						YYXLayer::sendNotify("IndexSceneReferShowHeadPortrait");
					}
				}, [](string str) {});
				/*NetIntface::DownLoadImage(url, m_dirpath, StringUtils::format("9HeadPortrait_%d.png", childrenId),
					"", [=](string path) {
					if (path != "" && FileUtils::getInstance()->isFileExist(path))
					{
						string savePath = m_dirpath + StringUtils::format("/childHead_%d.png", childrenId);
						App::makeRoundImage(path, savePath);
						string pathkey = StringUtils::format("path+childID=%d", childrenId);
						YYXStruct::initMapYYXStruct(App::GetInstance()->myData, pathkey, YYXLayer::getCurrentTime4Second(), savePath);
						YYXLayer::sendNotify("IndexSceneReferShowHeadPortrait");
					}
				}, "", [](string str) {});*/
			}
		}, [=](int b) {
			if (b == 1)
			{
				auto run = [=](string key) {
					//找出数据
					auto childrenId = YYXStruct::getMapInt64(App::GetInstance()->myData, key, -999);
					string namekey = StringUtils::format("name+childID=%d", childrenId);
					string birthdaykey = StringUtils::format("birthday+childID=%d", childrenId);
					string childrenName = YYXStruct::getMapString(App::GetInstance()->myData, namekey, "");
					int childrenSex = YYXStruct::getMapInt64(App::GetInstance()->myData, birthdaykey, 0);
					string childrenBirth = YYXStruct::getMapString(App::GetInstance()->myData, birthdaykey, "");
					//赋值
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildName", -999, childrenName);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildSex", childrenSex);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildBirthday", -999, childrenBirth);
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", childrenId);
					//写入本地
					YYXLayer::setFileValue("ShowChildName", childrenName, m_dirpath);
					YYXLayer::setFileValue("ShowChildSex", StringUtils::format("%d", childrenSex), m_dirpath);
					YYXLayer::setFileValue("ShowChildBirthday", childrenBirth, m_dirpath);
					YYXLayer::setFileValue("ShowChildID", StringUtils::format("%d", childrenId), m_dirpath);
				};
				int ShowChildID = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
				int childrenId1 = YYXStruct::getMapInt64(App::GetInstance()->myData, "firstChildID", -999);
				int childrenId2 = YYXStruct::getMapInt64(App::GetInstance()->myData, "secondChildID", -999);
				int childrenId3 = YYXStruct::getMapInt64(App::GetInstance()->myData, "threeChildID", -999);
				if (childrenId3 != -999 && ShowChildID == childrenId3)
					run("threeChildID");
				else
				{
					if (childrenId2 != -999 && ShowChildID == childrenId2)
						run("secondChildID");
					else
						run("firstChildID");
				}
			}
		});
	}, "", [](string stt) {});
}

//查询余额
void YYXVisitor::httpGetBalance(bool hint)
{
	NetIntface::httpGetUserBalance(App::GetInstance()->m_me->id, "", [=](string json) {
		//解析json 
		//写入内存和xml
		NetIntface::httpGetUserBalanceCallBack(json, [=](int id, int userBalance, long long uptime) {
			//正确解析
			if (App::GetInstance()->m_me)
				App::GetInstance()->m_me->momey = userBalance;			
			YYXLayer::setFileValue("visitorBalance", StringUtils::format("%d", userBalance), m_dirpath);
		}, [=]() {
			//返回错误,或者解析错误
			if (hint)
				Toast::create(App::getString("CHAXUNDANGQIANZHANGHUEYUSHIBAI"));
		});
	}, "", [=](string str) {
		//网络错误
		if(hint)
			Toast::create(App::getString("NETEXCEPTION"));
	});
}

void YYXVisitor::loadLocationVisitorData()
{
	//载入用户 余额 孩子
	string userAccount = YYXLayer::getFileValue("visitorAccount", "", m_dirpath);
	if (userAccount != "")
	{
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "visitorAccount", -999, userAccount);
	}
	else
	{
		string name = getVisitorName();
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "visitorAccount", -999, name);
		YYXLayer::setFileValue("visitorAccount", name, m_dirpath);
	}

	//加载vip包年书籍列表
	App::loadvipBookCache();
	//载入购书信息
	App::loadBuyBookCache();
	//载入收藏列表
	App::loadCollectBookCache();
	//载入下载列表
	App::loadDownloadBookCache();
	auto userBalancestr = YYXLayer::getFileValue("visitorBalance", "", m_dirpath);
	int money = atoi(userBalancestr.c_str());
	App::GetInstance()->m_me->momey = money;
	auto userSexstr = YYXLayer::getFileValue("visitorSex", "", m_dirpath);
	if (userSexstr != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userSex", atoi(userSexstr.c_str()));
	auto userCity = YYXLayer::getFileValue("visitorCity", App::getString("BEIJING"), m_dirpath);
	if (userCity != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userCity", -999, userCity);
	auto userProvince = YYXLayer::getFileValue("visitorProvince", "", m_dirpath);
	if (userProvince != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "userProvince", -999, userProvince);
	string ShowChildName = YYXLayer::getFileValue("ShowChildName", "", m_dirpath);
	if (ShowChildName != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildName", -999, ShowChildName);
	string ShowChildSexstr = YYXLayer::getFileValue("ShowChildSex", "", m_dirpath);
	if (ShowChildSexstr != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildSex", atoi(ShowChildSexstr.c_str()));
	string ShowChildBirthday = YYXLayer::getFileValue("ShowChildBirthday", "", m_dirpath);
	if (ShowChildBirthday != "") YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildBirthday", -999, ShowChildBirthday);
	//孩子信息
	string ShowChildIDstr = YYXLayer::getFileValue("ShowChildID", "", m_dirpath);
	if (ShowChildIDstr != "")
	{//有孩子ID
		int ShowChildID = atoi(ShowChildIDstr.c_str());
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildID", ShowChildID);
		string ShowChildHeadPortrait = YYXLayer::getFileValue("ShowChildHeadPortrait", "");
		if (ShowChildHeadPortrait != "" && FileUtils::getInstance()->isFileExist(ShowChildHeadPortrait))
		{//有孩子ID有头像
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, ShowChildHeadPortrait);
		}
		else
		{//有孩子ID无头像
			string savePath = m_dirpath + StringUtils::format("/childHead_%d.png", ShowChildID);
			if (FileUtils::getInstance()->isFileExist(savePath))
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, savePath);
		}
	}
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
	if (getVisitorMode() || App::GetInstance()->m_me == nullptr)
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
	if (getVisitorMode() || App::GetInstance()->m_me == nullptr)
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
	if (getVisitorMode() || App::GetInstance()->m_me == nullptr)
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
	auto userIdstr = YYXLayer::getFileValue("userId", "-999");
	if (userIdstr == "-999")
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
	if (visitorMemberId == "-999")
	{
		string str = NetIntface::getPhoneModel(3);
		if (!str.empty() && str.size() > 5)
		{
			App::log(" YYXVisitor::loginVisitor() phone Str= >" + str);
			httpGetVisitorMemberId(str);
		}
	}
	else
	{
		if (App::GetInstance()->m_me == nullptr)
			App::GetInstance()->m_me = new MyAccount();
		App::GetInstance()->m_me->id = atoi(visitorMemberId.c_str());		
		loadLocationVisitorData();
		httpGetVisitorInfo(false);
	}
	//2.有,加载进去
}

void YYXVisitor::logoutVisitor()
{
	setVisitorMode(false);
	//删除本地及内存账号信息记录
	if (App::GetInstance()->m_me) {
		delete App::GetInstance()->m_me;
		App::GetInstance()->m_me = nullptr;
	}
	App::GetInstance()->myData.clear();
	//删除全部红包
	App::GetInstance()->m_redPacket.clear();
	//暂停所有下载任务
	YYXDownload::GetInstance()->pauseAll();
	//清空已购列表, 收藏 , 包年图书
	App::GetInstance()->myBookURLMap.clear();
	App::GetInstance()->myBuyBook.clear();
	App::GetInstance()->VIPbook.clear();
	App::GetInstance()->bookCollect.clear();	
}

//判断本地账号是否是空的, 如果是空的就登入游客
void YYXVisitor::meIsNull()
{
	if (!App::GetInstance()->m_me)
	{
		App::log("************************* ====YYXVisitor::meIsNull()=====>"+App::getMemberID());
		loginVisitor();
	}
}

string YYXVisitor::getVisitorName()
{
	return App::getString("YOUKE") + App::getMemberID();
}