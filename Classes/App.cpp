#include "App.h"
#include "CocosAndroidJni.h"
//#include "DownloadRes.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "curl/include/win32/curl/curl.h"
#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "curl/include/android/curl/curl.h"
#endif
#include "NetIntface.h"

using namespace std;
/*
$(EngineRoot)external\win32-specific\zlib\include
libcurl_imp.lib

void App::addErrorLog(string error, string filename, int type)   type 1=网络错误  2=文件错误  3= 日志文件上传, 4=支付错误
*/
App* App::m_pInstance = nullptr;

__Dictionary* App::m_strings = nullptr;

TTFConfig App::m_ttf = TTFConfig("hkhb.TTF", 24);

std::string App::m_resource = "android";// "iphone6";

//sqlite3* App::m_db = nullptr;//数据库指针

int App::m_debug = 0;//0=测试版本  1=正式版本

void App::runTestFunction()
{
	Toast::create("测试");
	std::thread([]() {
		string soursedir = FileUtils::getInstance()->getWritablePath() + "temp";
		string destdir = NetIntface::getDownloadDir() + "/ellabook";
		if (soursedir == destdir)
			destdir = FileUtils::getInstance()->getWritablePath() + "ellabook";
		YYXLayer::CopyDirectory(soursedir, destdir);
	}).detach();
}

MySceneName App::m_RunningScene = LoadScene;

string App::m_photsName ="0";

App::App()   //构造函数是私有的
{	
	m_me = nullptr;
	//m_notification = nullptr;
	m_acount = new AcountInfo();
	m_Time = 0;
	protectTime = 0;
	isOnlyWifi = true;
	m_read = new ReadStart();
	m_read->bookId = -999;
	m_read->childrenId = -999;
	m_read->startTime = "";
	isBack = true;
	isMusicPlay = false;
	CsbCache = *(new map<string, Data>);
}

App * App::GetInstance()
{
	if (m_pInstance == nullptr)  //判断是否第一次调用
		m_pInstance = new App();
	//Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
	return m_pInstance;
}

//获取用户ID
std::string App::getMemberID()
{
	std::string str="-999";
	if (App::GetInstance()->m_me != nullptr)
	{
		str = StringUtils::format("%d", App::GetInstance()->m_me->id);
	}
	return str;
}

//sqlite3* App::sqliteOpen()
//{
//	if (m_db == nullptr)
//	{
//		m_db = SqliteManager::OpenDataBase();
//	}
//	return m_db;
//}
//
//void App::sqliteClose()
//{
//	if (m_db != nullptr)
//	{
//		SqliteManager::CloseDataBase(m_db);
//		m_db = nullptr;
//	}
//}

void App::writeFile(const char* str)
{
	App::log("writeFile");
	string path;
	CocosAndroidJni::GetPhotoPath(path);
	FILE* fp = fopen(path.append("a.png").c_str(), "wb+");
	fwrite(str, strlen(str), 1, fp);
	fclose(fp);
}

string App::getCoverDir()
{
	return string(FileUtils::getInstance()->getWritablePath() + "bookCover");
}

string App::getBookCoverPngPath(int bookid)
{
	return string(getCoverDir() + "/" + StringUtils::format("%d", bookid) + ".png");
}

string App::getTryReadZipDir()
{
	return string(FileUtils::getInstance()->getWritablePath() + "bookTryReadZip");
}

string App::getBookTryReadZipPath(int bookid)
{
	return string(getTryReadZipDir() + "/" + StringUtils::format("%d.zip", bookid));
}

string App::getTryReadDir()
{
	return string(FileUtils::getInstance()->getWritablePath() + "bookTryReadUNZip");
}

string App::getBookTryReadPath(int bookid)
{
	return string(getTryReadDir() + "/" + StringUtils::format("%d", bookid));
}

string App::getBookTryRead4Json_txtPath(int bookid)
{
	return string(getBookTryReadPath(bookid) + "/Iphone1334_view/json.txt");
}

string App::getReadZipDir()
{
	return string(FileUtils::getInstance()->getWritablePath() + "bookZip");
}

string App::getBookReadZipPath(int bookid)
{
	return string(getReadZipDir() + "/" + StringUtils::format("%d.zip", bookid));
}

string App::getReadDir()
{
	return string(FileUtils::getInstance()->getWritablePath() + "bookUNZip");
}

string App::getBookReadPath(int bookid)
{
	return string(getReadDir() + "/" + StringUtils::format("%d", bookid));
}

string App::getBookRead4Json_txtPath(int bookid)
{
	return string(getBookReadPath(bookid)+"/Iphone1334/json.txt");
}

void App::downloadFailDeleteFiles(string tryReadZipPath, string tryReadUNZipBookDir)
{
	YYXLayer::logb("downloadFailDeleteFiles" + tryReadZipPath);
	if (FileUtils::getInstance()->isFileExist(tryReadZipPath))
	{
		remove(tryReadZipPath.c_str());
	}
	if (FileUtils::getInstance()->isDirectoryExist(tryReadUNZipBookDir))
	{
		YYXLayer::DeleteDirectory(tryReadUNZipBookDir);
	}
	YYXLayer::loge("downloadFailDeleteFiles" + tryReadUNZipBookDir);
}

//下载前,根据设置判断网络情况是否符合下载要求, 并且提示下载设置修改
bool App::getNetSetAndHintSet()
{
	YYXLayer::logb("BookInfo::getNetSetAndHintSet");
	//0.先判断是否联网
	//1.获取wifi设置
	//2.判断仅wifi情况下
	//3.是否提示过, 如果未曾提示过就提示一下是否用流量下
	bool net = NetIntface::IsNetConnect(true);
	if (!net)
		return false;
	if (App::GetInstance()->isOnlyWifi)//无设置或者仅wifi
	{
		App::log("jin wifi----------");
		bool wifistauts = NetIntface::isWifi();
		if (!wifistauts)
		{//无wifi
			Toast::create(App::getString("WEIJIANCEDAOWIFI"));
			string hintS = YYXLayer::getFileValue("wifiHint", "");
			if (hintS == "1")
			{//已经提示过, 不再提示				
				return wifistauts;
			}
			else
			{//第一次提示, 是否愿意用流量下载
				auto lay = YYXLayer::MyMessageBox(App::getString("YUNXULIULIANGXIAZAI"), "", []() {
					//允许
					YYXLayer::setFileValue("wifiHint", "1");
					App::GetInstance()->isOnlyWifi = false;
					YYXLayer::setFileValue("IS_ONLY_WIFI", "1");
					return true;
				}, "", []() {
					//不允许
					App::GetInstance()->isOnlyWifi = true;
					YYXLayer::setFileValue("IS_ONLY_WIFI", "0");
					YYXLayer::setFileValue("wifiHint", "1");
					return false;
				});
				Director::getInstance()->getRunningScene()->addChild(lay);
			}
		}
	}
	return true;
}

void App::getVisible()
{
	auto org = Director::getInstance()->getVisibleOrigin();
	auto visisize = Director::getInstance()->getVisibleSize();
	App::log("--------------------------visisize.width = ", visisize.width);
	App::log("--------------------------visisize.height = ", visisize.height);
	App::log("--------------------------org.x = ", org.x);
	App::log("--------------------------org.y = ", org.y);
}

void App::httpCheckVIP(int memberID)
{
	string url = string(IP).append(NET_CHECHVIP);
	map<string, string> par;
	par["memberId"] = StringUtils::format("%d", memberID);
	NetIntface::httpPost(url, par, "checkVIPSuccess", [=](string json) {
		/*{
		"expireTime": 1508947200,
		"startTime": 1477411200,
		"errorMessage": "",
		"id": "",
		"result": true,
		"isVip": true,
		"times": 31536000,
		"code": 0,
		"toURL": ""
		}*/
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			if (YYXLayer::getBoolForJson(false, doc, "result"))
			{
				if (YYXLayer::getBoolForJson(false, doc, "isVip"))
				{
					if (!App::GetInstance()->m_me)
						App::GetInstance()->m_me = new MyAccount();
					App::GetInstance()->m_me->id = memberID;
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
					string rentpath = FileUtils::getInstance()->getWritablePath() + StringUtils::format("rentBook/RentBook_%d.json", App::GetInstance()->m_me->id);
					FileUtils::getInstance()->removeFile(rentpath);
				}
			}
		}
	}, "", [](string str) {});
}

//获取评论
void App::httpComment(int bookid, function<void()> runFunction)
{
	string runkey = StringUtils::format("httpCommentSuccess_%d", (int)YYXLayer::getRandom());
	string errorkey = "";
	NetIntface::httpGetComments(bookid, runkey, [=](string json) {
		NetIntface::httpBookCommentsCallback(json, [=](int index, string gevalId, string gevalType, string score, string memberName, string gevalState,
			string memberId, string commentTime, string title, string content, string url, string gevalTime) {
			int id = atoi(gevalId.c_str());//评论ID
			int xingji = atoi(score.c_str());//评星
			time_t icommentTime = atoi(commentTime.c_str());//评论时间
			int memnerid = atoi(memberId.c_str());//用户ID
			int voiceLength = atoi(gevalTime.c_str());//声音时长
			if (!content.empty())
			{//文字评论
				string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);//bookid + 下标顺序
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, idKey, id, "TEXT");//评论id + 类型
				string contentKey = StringUtils::format("comment_gevalId=%d+score+content", id);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, contentKey, xingji, content);
				string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", id);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, timeKey, icommentTime, memberName, (Ref*)memnerid);
				string titleKey = StringUtils::format("comment_gevalId=%d+title", id);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, titleKey, -999, title);
			}
			else if (!url.empty())
			{//语音评论
				string dir = FileUtils::getInstance()->getWritablePath() + "voiceComment";
				string filename = gevalId + ".mp3";
				string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);//bookid + 下标顺序
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, idKey, id, "VOICE");//评论id + 类型
				string voiceKey = StringUtils::format("comment_gevalId=%d+score+url+gevalTime", id);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, voiceKey, xingji, url, (Ref*)voiceLength);
				string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", id);
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, timeKey, icommentTime, memberName, (Ref*)memnerid);
				if (!FileUtils::getInstance()->isFileExist(dir + "/" + filename))
				{
					NetIntface::DownLoadFile(url, dir, filename, "", [](string path) {}, "", [](string str) {});
				}
			}
			string commentCountKey = StringUtils::format("comment_bookid=%d", bookid);//book评论的数量
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, commentCountKey, index + 1);
		}, runFunction, []() {});
	}, errorkey, [](string str) {});
}

bool App::addData(string path, Data& result)
{
	if (&CsbCache == nullptr)
		return false;
	if (!FileUtils::getInstance()->isFileExist(path))
		return false;
	auto data = FileUtils::getInstance()->getDataFromFile(FileUtils::getInstance()->fullPathForFilename(path));
	if (data.getSize() <= 0)
		return false;
	CsbCache[path] = data;
	result = data;
	//App::log(path + " add csb File cache");
	return true;
}

bool App::getData(string path, Data& data)
{	
	if (&CsbCache == nullptr)
		return false;
	if (CsbCache.find(path) != CsbCache.end())
	{
		data = CsbCache[path];
		//App::log(path+" get csb File cache");
	}
	else
	{
		return addData(path, data);
	}
	return true;
}

bool App::copyFile(string source, string ToDesc)
{
	// 目标路径
	//if (FileUtils::getInstance()->isFileExist(ToDesc))
		//return true;
	//源路径
	Data data = FileUtils::getInstance()->getDataFromFile(source);
	FILE *fp = fopen(ToDesc.c_str(), "wb");
	if (fp)
	{
		size_t size = fwrite(data.getBytes(), sizeof(unsigned char), data.getSize(), fp);
		fclose(fp);
		if (size > 0)
			return true;
	}
	return false;
}

/*bool App::copyFile(string SourceFile, string NewFile)
{
	ifstream in;
	ofstream out;
	in.open(SourceFile, ios::binary);//打开源文件
	if (in.fail())//打开源文件失败
	{
		App::log( "Error 1: Fail to open the source file." );
		in.close();
		out.close();
		return false;
	}
	out.open(NewFile, ios::binary);//创建目标文件 
	if (out.fail())//创建文件失败
	{
		App::log("Error 2: Fail to create the new file.");
		out.close();
		in.close();
		return false;
	}
	else//复制文件
	{
		out << in.rdbuf();
		out.close();
		in.close();
		return true;
	}
}*/

bool App::createDataBase()
{
// 目标路径
	std::string destPath = FileUtils::getInstance()->getWritablePath() + "data.db";
	if (FileUtils::getInstance()->isFileExist(destPath))
		return true;
	//源路径
	std::string sourcePath = FileUtils::getInstance()->fullPathForFilename("data.db");
	Data data = FileUtils::getInstance()->getDataFromFile(sourcePath);

	FILE *fp = fopen(destPath.c_str(), "wb");
	if (fp)
	{
		size_t size = fwrite(data.getBytes(), sizeof(unsigned char), data.getSize(), fp);
		fclose(fp);
		if (size > 0)
		{
			return true;
		}
	}
	log("copy file failed.");	
	return false;
}

void App::startTime()
{
	if (App::GetInstance()->m_Time == 0)
	{
		App::GetInstance()->m_Time = getCurrentTime();
	}
	else if (getCurrentTime() > (App::GetInstance()->m_Time + 60))
	{
		App::GetInstance()->m_Time = getCurrentTime();
	}
}

long long App::getCurrentTime()
{
	return time(0);
}

const char* App::getChars(const char* key)
{
	if (m_strings == nullptr)
		m_strings = Dictionary::createWithContentsOfFile("strings.xml");
	m_strings->retain();
	//读取Hello键中的值 objectForKey根据key，获取对应的string
	auto value = ((__String*)m_strings->objectForKey(key))->_string.c_str();
	return value;
}

const char* App::getString(const char* key)
{
	if(m_strings ==nullptr)
		m_strings = Dictionary::createWithContentsOfFile("strings.xml");
	m_strings->retain();
	//读取Hello键中的值 objectForKey根据key，获取对应的string
	return ((__String*)m_strings->objectForKey(key))->_string.c_str();
}

//"data":{"memberCity":null, "memberName" : "18888702718", "memberProvince" : null, "memberSex" : "1", "memberId" : 3618}
/*bool App::analysisMemberInfo(rapidjson::Value &data)
{
		if(data.IsObject())
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
		else
			return false;
}*/

//Texture2D* App::readImage(std::string path)
//{
//	auto fileNameFullPath = FileUtils::getInstance()->getWritablePath() + path;
//	auto image = TextureCache::getInstance()->addImage(fileNameFullPath);
//	return image;
//}

////数据库查询书城里面的按钮列表
//vector<unordered_map<string, ParaType>> App::sqliteBookCity4StoreList(sqlite3* db)
//{
//	std::vector<std::string> vec;
//	vec.push_back("*");
//	auto result = SqliteManager::SelectData(db, DB_STOREINFO, vec, "");
//	return result;
//}
//
////数据库查询书店信息
//vector<unordered_map<string, ParaType>> App::sqliteStoreInfo(sqlite3* db,int BookStoreId)
//{
//	char str[100];
//	sprintf(str, "where castleId = %d", BookStoreId);
//	std::vector<std::string> vec;
//	vec.push_back("castleName");
//	auto result = SqliteManager::SelectData(db, DB_STOREINFO, vec, str);
//	return result;
//}
//
////数据库查询书店里书籍列表
//vector<unordered_map<string, ParaType>> App::sqliteBookList(sqlite3* db, int BookStoreId)
//{
//	char str[100];
//	sprintf(str, "where castleId = %d order by sort asc", BookStoreId);
//	std::vector<std::string> vec;
//	vec.push_back("*");
//	auto result = SqliteManager::SelectData(db, DB_BOOKLIST, vec, str);
//	return result;
//}
//
////数据库查询书籍详情
//vector<unordered_map<string, ParaType>> App::sqliteBookInfo(sqlite3* db, int BookId)
//{
//	char str[100];
//	sprintf(str, "where bookId = %d", BookId);
//	std::vector<std::string> vec;
//	vec.push_back("*");
//	auto result = SqliteManager::SelectData(db, DB_BOOKINFO, vec, str);
//	//CCLOG("sqliteBookInfo[数据库查询书籍详情]sqliteBookInfo");
//	return result;
//}
//
////数据库查询书城背景图片
//vector<unordered_map<string, ParaType>> App::sqliteBookCity_bg(sqlite3* db, int castleId)
//{
//	std::vector<std::string> vec;
//	vec.push_back("*");
//	auto result = SqliteManager::SelectData(db, DB_RES, vec, StringUtils::format("where bookId = %d and status = 6", castleId));
//	return result;
//}
//
////数据库查询书城 边框图片
//vector<unordered_map<string, ParaType>> App::sqliteBookCity_border(sqlite3* db, int castleId)
//{
//	std::vector<std::string> vec;
//	vec.push_back("*");
//	auto result = SqliteManager::SelectData(db, DB_RES, vec, StringUtils::format("where bookId = %d and status = 7", castleId));
//	return result;
//}
//
////数据库查询书籍封面
//vector<unordered_map<string, ParaType>> App::sqliteBookPicture(sqlite3* db, int BookId)
//{
//	char str[100];
//	sprintf(str, "where bookId = %d and status = 0", BookId);
//	std::vector<std::string> vec;
//	vec.push_back("*");
//	auto result = SqliteManager::SelectData(db, DB_RES, vec, str);
//	return result;
//}
//
////查询书籍详情更新时间
//bool App::sqliteBookInfoUptime(sqlite3* db, int BookId) {
//	bool tag = false;
//	string condition =StringUtils::format("where bookId=%d", BookId);
//	std::vector<std::string> vec;
//	vec.push_back("upTime");
//	auto result = SqliteManager::SelectData(db, DB_BOOKINFO, vec, condition);
//	if (NET_UPDATE_TIME_INTERVAL * 60 < getCurrentTime() - result[0]["upTime"].intPara) {
//		tag = true;
//	}
//	return tag;
//}

std::string App::analysisJsonString(rapidjson::Value &data,std::string key)
{
	string result = "";
	if (data.HasMember(key.c_str()))
	{
		rapidjson::Value &bookCoverUrl = data[key.c_str()];
		if (bookCoverUrl.IsString())
		{
			//CCLOG(StringUtils::format("analysisJsonString[%s] = %s",key.c_str(), bookCoverUrl.GetString()).c_str());
			result = bookCoverUrl.GetString();
		}
	}
	App::log(key +" = " + result);
	return result;
}

int App::analysisJsonInt(rapidjson::Value &data, std::string key)
{
	int result = -999;
	if (data.HasMember(key.c_str()))
	{
		rapidjson::Value &bookCoverUrl = data[key.c_str()];
		if (bookCoverUrl.IsInt())
		{
			//CCLOG(StringUtils::format("analysisJsonInt[%s] = %d", key.c_str(), bookCoverUrl.GetInt()).c_str());
			result = bookCoverUrl.GetInt();
		}
	}
	App::log(key + " = " , result);
	return result;
}

long long App::analysisJsonLongLong(rapidjson::Value &data, std::string key)
{
	long long result = -999;
	if (data.HasMember(key.c_str()))
	{
		rapidjson::Value &bookCoverUrl = data[key.c_str()];
		if (bookCoverUrl.IsInt64())
		{
			//CCLOG(StringUtils::format("analysisJsonInt[%s] = %d", key.c_str(), bookCoverUrl.GetInt()).c_str());
			result = bookCoverUrl.GetInt64();
		}
	}
	App::log(key + " = ", (int)result);
	return result;
}

double App::analysisJsonDouble(rapidjson::Value &data, std::string key)
{
	if (data.HasMember(key.c_str()))
	{
		rapidjson::Value &bookCoverUrl = data[key.c_str()];
		if (bookCoverUrl.IsDouble())
		{
			//CCLOG(StringUtils::format("analysisJsonDouble[%s] = %lf", key.c_str(), bookCoverUrl.GetDouble()).c_str());
			return bookCoverUrl.GetDouble();
		}
		else
		{
			string str = key+"double类型JSON解析错误";
			App::log(str.c_str());
			int ret = analysisJsonInt(data, key);
			return ret;
		}
	}
	return -999.0;
}

//检查下标
void App::examineIndex(int size, int index)
{
	if (index <0 || index >size - 1)
	{
		App::log("[下标异常]");
	}
}

//获取系统时间
std::string App::GetStringTime()
{
	time_t tt;
	time(&tt);
	struct tm * now;
	now = localtime(&tt);
	string timeStr = StringUtils::format("%d/%02d/%02d %02d:%02d:%02d", now->tm_year + 1900, now->tm_mon + 1,now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	return timeStr;
}

//获取系统时间2
std::string App::GetStringTime2()
{
	time_t tt;
	time(&tt);
	struct tm * now;
	now = localtime(&tt);
	string timeStr = StringUtils::format("%d-%02d-%02d %02d:%02d:%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	return timeStr;
}
//获取系统时间3
std::string App::GetStringTime3()
{
	time_t tt;
	time(&tt);
	struct tm * now;
	now = localtime(&tt);
	string timeStr = StringUtils::format("%d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	return timeStr;
}
//根据系统时间判断白天或夜晚
bool App::isNight() {
	time_t tt;
	time(&tt);
	struct tm * now;
	now = localtime(&tt);
	if ((now->tm_hour >= 0 && now->tm_hour < 7) || (now->tm_hour >= 19 && now->tm_hour < 24)) {
		return true;
	}
	return false;
}

//void App::initParaType(std::map<std::string, ParaType>& map,std::string key,long long number , std::string str)
//{
//	//App::log("App::initParaType/"+ key);
//	if (&map == nullptr)
//		return;
//	ParaType ty =ParaType();
//	ty.intPara = number;
//	ty.stringPara = str;	
//	map[key] = ty;
//	//App::log("App::initParaType--END");
//}

void App::ccsleep(int times)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
{
	usleep(times*1000);
}
#endif
#if  (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
{
	Sleep(times);
}
#endif
}

//开始视力保护计时
void App::protectedTiming() {
	std::thread pthread(threadTiming, App::getCurrentTime());
	pthread.detach();
}

void App::threadTiming(long long startAppTime) {
	while (true) 
	{	//释放资源
		if (App::GetInstance()->RefVector.size() > 0)
		{
			for (int i = 0; i < App::GetInstance()->RefVector.size(); i++)
			{
				auto it = App::GetInstance()->RefVector.front();
				if( (App::getCurrentTime() - it.intData) >10)
				{
					App::log(it.stringData + "::getReferenceCount", it.refData->getReferenceCount());
					App::log(it.stringData , (int)it.refData);
					while (it.refData != nullptr && it.refData->getReferenceCount() > 1 && it.refData->getReferenceCount() < 100)
					{
						it.refData->release();						
					}
					if (it.refData->getReferenceCount() == 1)
					{
						it.refData->release();
						it.refData == nullptr;
					}
					if (it.refData != nullptr)
						App::GetInstance()->RefVector.pop();
					App::log(it.stringData + "::getReferenceCount end", it.refData->getReferenceCount());
					App::log(it.stringData + "end", (int)it.refData);
				}
				else
					break;
			}
		}
		//视力保护
		if (App::GetInstance()->protectTime != 0 &&App::GetInstance()->getCurrentTime() - startAppTime >= App::GetInstance()->protectTime * 60) {
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				auto pScene = Director::getInstance()->getRunningScene();
				auto layer = RestRemind::create();
				pScene->addChild(layer, 10000);
			});
			break;
		}
		//Toast提示
		if (Toast::GetInstance()->getRuning())
		{
			auto t = App::getCurrentTime() - Toast::GetInstance()->m_time;
			if (t >= Toast::GetInstance()->getShowTime())
			{
				if (Toast::GetInstance()->m_dataQueue.size() > 0)
				{
					Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
						Toast::GetInstance()->unvisiable();						
						if (Toast::GetInstance()->m_dataQueue.size() > 0)
							Toast::GetInstance()->showNext();
					});
				}
			}
		}
		//删除文件
		if (App::GetInstance()->unDeleteFile.size() > 0)
		{
			vector<string> data;
			for (auto it : App::GetInstance()->unDeleteFile)
			{
				App::log("removeFile  "+it.first);
				if (FileUtils::getInstance()->removeFile(it.first))
					data.push_back(it.first);
			}
			for (auto da : data)
			{
				App::GetInstance()->unDeleteFile.erase(da);
			}
		}
		App::ccsleep(1000);
	}
}

void App::searchReadRecordJson()
{
	NetIntface::TraversingFiles(FileUtils::getInstance()->getWritablePath() + "readBookRecord", [=](string filePath, string name) {
		string str = YYXLayer::readFile(filePath);
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, str))
		{
			string readBookId = YYXLayer::getStringForJson("", doc, "readBookId");
			string readChildren = YYXLayer::getStringForJson("", doc, "readChildren");
			string readEndTime = YYXLayer::getStringForJson("", doc, "readEndTime");
			string readMemberId = YYXLayer::getStringForJson("", doc, "readMemberId");
			string readStartTime = YYXLayer::getStringForJson("", doc, "readStartTime");
			map<string, string> paramter;
			paramter["memberId"] = readMemberId;
			paramter["childrenId"] = readChildren;
			paramter["bookId"] = readBookId;
			paramter["readStartTime"] = readStartTime;
			paramter["readEndTime"] = readEndTime;
			paramter["resource"] = App::m_resource;
			string url = string(IP).append(NET_SAVE_READHISTORY);
			string successKey = StringUtils::format("shangchuanReadRecordSuccess_%d", (int)YYXLayer::getRandom());
			NetIntface::httpPost(url, paramter, successKey, [=](string json) {
				NetIntface::saveReadRecordCallBack(json, [=]() {
					if (FileUtils::getInstance()->isFileExist(filePath))
					{
						if (!FileUtils::getInstance()->removeFile(filePath))
							App::GetInstance()->unDeleteFile[filePath] = "";
					}
				}, []() {
					//YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "httpReadRecordJsonPath");
				});
			}, "", [](string str) {
				//YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, "httpReadRecordJsonPath");
			});
		}
	}, [](string dirPath, string name) {
		App::log("NetIntface::TraversingFiles dir =" + dirPath);
	});
}

//跳转场景的时候 记录离开的场景
void App::pushScene(MySceneName name,int data,std::string  str)
{
	//如果是首页 清空栈
	if (name == IndexScene)
	{
		m_SceneOrder.clear();
		m_showSceneData.intData = 0;
		m_showSceneData.stringData = "0";
	}
	map<string, YYXStruct> mapvalue;
	YYXStruct namePara;
	namePara.intData = name;
	switch (name)
	{
	case LoadScene:
		namePara.stringData = "LoadScene";
		break;
	case BookRoomScene:
		namePara.stringData = "BookRoomScene";
		break;
	case ParentScene:
		namePara.stringData = "ParentScene";
		break;
	case IndexScene:
		namePara.stringData = "IndexScene";
		break;
	case LoginScene:
		namePara.stringData = "LoginScene";
		break;
	case BabyCenterScene:
		namePara.stringData = "BabyCenterScene";
		break;
	case BookCity:
		namePara.stringData = "BookCity";
		break;
	case BookInfoScene:
		namePara.stringData = "BookInfoScene";
		break;
	case BookCityCHILD:
		namePara.stringData = "BookCityCHILD";
		break;
	case PictureBook:
		namePara.stringData = "PictureBook";
		break;
	case Recommend:
		namePara.stringData = "Recommend";
		break;
	case Free:
		namePara.stringData = "Free";
		break;
	case GoodReputation:
		namePara.stringData = "GoodReputation";
		break;
	case NewBook:
		namePara.stringData = "NewBook";
		break;
	case KangXuanStore:
		namePara.stringData = "KangXuanStore";
		break;
	case VIPBOOK:
		namePara.stringData = "VIPBOOK";
		break;
	default:
		break;
	}	
	mapvalue["MySceneName"] = namePara;
	YYXStruct val;
	val.intData = data;
	val.stringData =str;		
	mapvalue["Data"] = val;
	if (!m_SceneOrder.empty())
	{
		for (int i = 0; i < m_SceneOrder.size(); i++)
		{			
			if (m_SceneOrder[i]["MySceneName"].intData == name)
			{
				auto it = m_SceneOrder[i]["MySceneName"].intData;				
				m_SceneOrder.erase(m_SceneOrder.begin() + i);
			}
		}
	}
	m_SceneOrder.push_back(mapvalue);
	//FileUtils::getInstance()->removeFile(FileUtils::getInstance()->getWritablePath() + "push.txt");
	//for (auto it : m_SceneOrder)
	//{
	//	string str = string("\n\r----------").append(it["MySceneName"].stringData).append("---------------- ").append(StringUtils::format("%d\n\r", it["MySceneName"].intData));
	//	YYXLayer::writeFilepp(str, FileUtils::getInstance()->getWritablePath()+"push.txt");
	//}
}

void App::popBack(MySceneName dangqianScene)
{
	if (m_SceneOrder.empty())
	{
		m_showScene = IndexScene;
		return;
	}
	if (dangqianScene != LoadScene)
	{
		if (getFromScene() == dangqianScene)
		{
			m_SceneOrder.pop_back();
		}
	}
	auto mapvalue = m_SceneOrder.back();
	m_SceneOrder.pop_back();
	m_showScene = (MySceneName)mapvalue["MySceneName"].intData;
	m_showSceneData.intData = 0;
	m_showSceneData.stringData = "";
	if (mapvalue.find("Data") != mapvalue.end())
	{
		m_showSceneData = mapvalue["Data"];
	}
}

MySceneName App::getFromScene()
{
	MySceneName result ;
	if (m_SceneOrder.empty())
	{
		result= MySceneName::IndexScene;
	}
	else
	{
		auto mapvalue = m_SceneOrder.back();
		result=(MySceneName)mapvalue["MySceneName"].intData;
	}
	App::log("MySceneName App::getFromScene() = ", result);
	return result;
}

bool App::IsHaveFile(std::string fullpath)
{
	std::fstream thisfile;
	thisfile.open(fullpath, ios::in);
	if (thisfile)
	{
		thisfile.close();
		return true;
	}
	else
	{
		CCLOG("%s this picture is NULL", fullpath.c_str());
		return false;
	}
}

////网络请求-记录阅读记录
//void App::saveReadRecord() {	
//	/*HttpRequest* request = new HttpRequest();
//	request->setRequestType(HttpRequest::Type::POST);
//	request->setUrl(std::string(IP).append(NET_SAVE_READHISTORY).c_str());
//	string postData = "memberId=" + App::getMemberID() + StringUtils::format("&childrenId=%d", App::GetInstance()->m_read->childrenId) +
//		StringUtils::format("&bookId=%d", App::GetInstance()->m_read->bookId) + "&readStartTime=" + App::GetInstance()->m_read->startTime + 
//		"&readEndTime=" + App::GetStringTime2();
//	request->setRequestData(postData.c_str(), strlen(postData.c_str()));
//
//	//请求的回调函数
//	request->setResponseCallback([=](HttpClient* client, HttpResponse* response) {
//		if (!response) {
//			return;
//		}
//		if (!response->isSucceed()) {
//			log("response failed! error buffer: %s", response->getErrorBuffer());
//			return;
//		}
//	});
//	cocos2d::network::HttpClient::getInstance()->send(request);
//	request->release();*/
//	if (App::GetInstance()->m_me == nullptr || App::GetInstance()->m_read->childrenId == -999 ||
//		App::GetInstance()->m_read->bookId == -999 || App::GetInstance()->m_read->startTime.length() == 0) {
//		return;
//	}
//	NetIntface::saveReadRecord(App::GetInstance()->m_me->id, 
//		App::GetInstance()->m_read->childrenId, 
//		App::GetInstance()->m_read->bookId, 
//		App::GetInstance()->m_read->startTime.c_str, 
//		App::GetStringTime2().c_str, 
//		StringUtils::format("saveReadRecord%d", (int)YYXLayer::getRandom()), [=](string json) {
//		map<int, string> books;
//		NetIntface::saveReadRecordCallBack(json, [] {
//			//解析成功
//		}, [] {
//			//解析失败
//		}
//			);
//	}, StringUtils::format("saveReadRecord%d", (int)YYXLayer::getRandom()), [](string str) {
//		//网络错误
//	});
//}

string App::getFormatTime(time_t t)
{	
	struct tm *p;
	p = gmtime(&t);
	char s[80];
	strftime(s, 80, "%Y-%m-%d", p);
	return s;
}

//vector<map<string, YYXStruct>> App::analysisJson4SQLite(const char* json,vector<string> stringType,vector<string> longlongType)
//{
//	vector<map<string, YYXStruct>> values;
//	rapidjson::Document doc;
//	doc.Parse<0>(json);
//	if (doc.HasParseError()) {
//		CCLOG("%s","201605131600 doc Has Parse Error");
//		return values;
//	}
//	if (!doc.IsArray()) {
//		CCLOG("%s", "201605131601 doc Is not Array");
//		return values;
//	}
//	rapidjson::Value & array = doc;
//	for (rapidjson::SizeType i = 0; i < array.Size(); i++)
//	{
//		rapidjson::Value & item = array[i];
//		if (item.IsObject())
//		{
//			map<string, YYXStruct> value;
//			for (int i = 0; i < stringType.size();i++)
//			{
//				auto key = stringType[i];
//				App::initParaType(value, key, -999, App::analysisJsonString(item, key));
//			}
//			for (int i = 0; i < longlongType.size(); i++)
//			{
//				auto key = longlongType[i];
//				App::initParaType(value, key, App::analysisJsonLongLong(item, key), "");
//			}
//			values.push_back(value);
//		}
//	}
//	return values;
//}
//
//string App::getString4map(map<string,myParaType> maps, string key,string default_str)
//{
//	string result = default_str;
//	if (key.length() !=0)
//	{
//		if (maps.find(key) != maps.end())
//		{
//			result = maps[key].stringPara;
//		}
//	}
//	return result;
//}
//
//long long App::getlonglong4map(map<string, myParaType> maps, string key,long long default_int)
//{
//	long long result = default_int;
//	if (key.length() != 0)
//	{
//		if (maps.find(key) != maps.end())
//		{
//			result = maps[key].intPara;
//		}
//	}
//	return result;
//}

void App::log(string str,long long count)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (count == -999)
		CocosAndroidJni::Log("show", str.c_str());
	else
		CocosAndroidJni::Log("show", StringUtils::format("%s / %d", str.c_str(), count).c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if(count == -999)
		CCLOG("%s", str.c_str());
	else
		CCLOG("%s / %lld", str.c_str(), count);
#endif
	//写日志
	if(App::m_debug == 0)
		writeLog(StringUtils::format("(%lld)%s", count, str.c_str()), FileUtils::getInstance()->getWritablePath() + "temp/Log","LogFileName");
}

void App::writeLog(string str, string dir, string pahtKey)
{
	if (str.empty())
		return;
	if (!FileUtils::getInstance()->isDirectoryExist(dir))
	{
		FileUtils::getInstance()->createDirectory(dir);
	}
	string path = YYXLayer::getFileValue(pahtKey, "");
	if (path == "")
	{
		auto fileName = StringUtils::format("/%lld.dat", YYXLayer::getRandom());
		path = dir + fileName;
		YYXLayer::setFileValue(pahtKey, path);
	}
	if (FileUtils::getInstance()->isFileExist(path))
	{
		auto fsize = FileUtils::getInstance()->getFileSize(path);
		if (fsize > 500000)
		{
			auto fileName =StringUtils::format("/%lld.dat", YYXLayer::getRandom());
			path = dir + fileName;
			YYXLayer::setFileValue(pahtKey, path);
		}
	}
	YYXLayer::writeFilepp(str+"\r\n", path);
}

void App::backThreading() {
	App::GetInstance()->isBack = false;
	std::thread pthread(backWating);
	pthread.detach();
}

void App::backWating() {
	ccsleep(3000);
	App::GetInstance()->isBack = true;
}

void App::addRecordBookDownload(int bookid)
{
	App::GetInstance()->bookDownLoad[bookid] = (int)YYXLayer::getCurrentTime4Second();
	if (App::GetInstance()->bookDownLoad.size() > 60)
	{
		auto vit = sortMapToVector(App::GetInstance()->bookDownLoad);
		while (vit.size() > 60)
		{
			auto itb = vit.back();
			App::GetInstance()->bookDownLoad.erase(itb.first);
			vit.pop_back();
		}
	}
	map<string, string> data;
	for (auto it : App::GetInstance()->bookDownLoad)
	{
		string key = StringUtils::format("%d", it.first);
		string value = StringUtils::format("%d", it.second);
		data[key] = value;
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = "downloadBook/download_"+App::getMemberID()+".json";
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
}

void App::addRecordBookCollect(int bookid)
{
	App::GetInstance()->bookCollect[bookid] = (int)YYXLayer::getCurrentTime4Second();
	if (App::GetInstance()->bookCollect.size() > 60)
	{
		auto vit = sortMapToVector(App::GetInstance()->bookCollect);
		while (vit.size() > 60)
		{
			auto itb = vit.back();
			App::GetInstance()->bookCollect.erase(itb.first);
			vit.pop_back();
		}
	}
	map<string, string> data;
	for (auto it : App::GetInstance()->bookCollect)
	{
		string key = StringUtils::format("%d", it.first);
		string value = StringUtils::format("%d", it.second);
		data[key] = value;
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = "collectBook/collect_" + App::getMemberID() + ".json";
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
}

void App::addRecordBookDelete(int bookid)
{
	App::GetInstance()->bookDelete[bookid] = (int)YYXLayer::getCurrentTime4Second();
	map<string, string> data;
	for (auto it : App::GetInstance()->bookDelete)
	{
		string key = StringUtils::format("%d", it.first);
		string value = StringUtils::format("%d", it.second);
		data[key] = value;
	}
	if (App::GetInstance()->bookDelete.size() > 60)
	{
		auto vit = sortMapToVector(App::GetInstance()->bookDelete);
		while (vit.size() > 60)
		{
			auto itb = vit.back();
			App::GetInstance()->bookDelete.erase(itb.first);
			vit.pop_back();
		}
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = "deleteBook/delete_" + App::getMemberID() + ".json";
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
}

bool cmp_by_value(const PAIR& lhs, const PAIR& rhs) {
	return lhs.second > rhs.second;
}

string App::getOnlyKey()
{
	auto str = StringUtils::format("%lld", YYXLayer::getRandom());
	return str;
}

void App::addErrorLog(string error, string filename, int type)
{/*type 1=网络错误  2=文件错误  3= 日志文件上传, 4=支付错误*/
	map<string, string> parater;
	parater["logInfo"] = error;
	parater["createTime"] = StringUtils::format("%lld", YYXLayer::getCurrentTime4Second());
	parater["models"] = App::GetInstance()->phoneModel;
	parater["memberid"] = App::getMemberID();
	parater["resource"] = App::m_resource;
	parater["systemInfo"] = App::GetInstance()->systemVersion;
	parater["logType"] = StringUtils::format("%d", type);
	string josn = YYXLayer::getStringFormMap(parater);
	//writeLog(josn+"\r\n", FileUtils::getInstance()->getWritablePath() + "errorLog", "ErrorLogKey");
	string path = FileUtils::getInstance()->getWritablePath() + "errorLog/" + StringUtils::format("%d.dat", (int)YYXLayer::getRandom());
	YYXLayer::writeFile(josn, path);
}

void App::upLogFiles()
{
	NetIntface::TraversingFiles(FileUtils::getInstance()->getWritablePath() + "temp/Log", [=](string filePath, string name) {
		App::log("upLoadingErrorLog  " + filePath);
		auto cocaltionfile = YYXLayer::getFileValue("LogFileName", "");
		if (cocaltionfile == filePath)
			return;
		string url = "";
		if (App::m_debug == 0)
			url = string("http://192.168.10.10:8089").append(NET_UPLOGFILE);
		else
			url = string(IP).append(NET_UPLOGFILE);
		map<string, string> parater;
		parater["createTime"] = StringUtils::format("%lld", YYXLayer::getCurrentTime4Second());
		parater["models"] = App::GetInstance()->phoneModel;
		parater["memberid"] = App::getMemberID();
		parater["resource"] = App::m_resource;
		parater["systemInfo"] = App::GetInstance()->systemVersion;
		parater["logType"] = "3";
		NetIntface::httpUpFile(url, YYXLayer::getStringFormMap(parater), filePath, [=](string json) {
			rapidjson::Document doc;
			bool result = YYXLayer::getJsonObject4Json(doc, json);
			if (result)
			{
				auto sres = YYXLayer::getBoolForJson(false, doc, "result");
				if (sres)
					App::GetInstance()->unDeleteFile[filePath] = "";
			}
		}, [](string error) {
		});
	}, [](string dirPath, string name) {//遍历到文件夹
	});
}

void App::upLoadingErrorLog()
{
	NetIntface::TraversingFiles(FileUtils::getInstance()->getWritablePath() + "errorLog", [=](string filePath, string name) {
		App::log("upLoadingErrorLog  " + filePath);
		string str = YYXLayer::readFile(filePath);
		map<string, string> paramter;
		YYXLayer::TraversingJson(str, paramter);
		string url = "";
		if (App::m_debug == 0)
			url = string("http://192.168.10.10:8089").append(NET_UPERRORLOG);
		else
			url = string(IP).append(NET_UPERRORLOG);
		NetIntface::httpPost(url, paramter, "", [=](string json) {//post成功
			rapidjson::Document doc;
			bool result = YYXLayer::getJsonObject4Json(doc, json);
			if (result)
			{
				auto sres=  YYXLayer::getBoolForJson(false, doc, "result");
				if (sres)					
					App::GetInstance()->unDeleteFile[filePath] = "";			
			}
		}, "", [](string posterror) {});		
	}, [](string dirPath, string name) {//遍历到文件夹
	});
}

vector<PAIR> App::sortMapToVector(map<int , int> mapData)
{
	vector<PAIR> vec(mapData.begin(), mapData.end());
	sort(vec.begin(), vec.end(), cmp_by_value);
	return vec;
}

//vector<PAIR> App::sortRecordBookDownload()
//{
//	vector<PAIR> vec(App::GetInstance()->bookDownLoad.begin(), App::GetInstance()->bookDownLoad.end());
//	sort(vec.begin(), vec.end(), cmp_by_value);
//	return vec;
//}

void App::addRecordBookRead(int bookid)
{
	App::GetInstance()->bookRead[bookid] = (int)YYXLayer::getCurrentTime4Second();
	map<string, string> data;
	for (auto it : App::GetInstance()->bookRead)
	{
		string key = StringUtils::format("%d", it.first);
		string value = StringUtils::format("%d", it.second);
		data[key] = value;
	}
	if (App::GetInstance()->bookRead.size() > 60)
	{
		auto vit = sortMapToVector(App::GetInstance()->bookRead);
		while (vit.size() > 60)
		{
			auto itb = vit.back();
			App::GetInstance()->bookRead.erase(itb.first);
			vit.pop_back();
		}
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = "readBook/download_" + App::getMemberID() + ".json";
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
}

//vector<PAIR> App::sortRecordBookRead()
//{
//	vector<PAIR> vec(App::GetInstance()->bookRead.begin(), App::GetInstance()->bookRead.end());
//	sort(vec.begin(), vec.end(), cmp_by_value);
//	return vec;
//}
//
//vector<PAIR> App::sortRecordMyRentBook()
//{
//	vector<PAIR> vec(App::GetInstance()->myRentBookMap.begin(), App::GetInstance()->myRentBookMap.end());
//	sort(vec.begin(), vec.end(), cmp_by_value);
//	return vec;
//}

void App::getMapFromFile(string path, map<string, string>& data)
{
	ssize_t size = 0;
	unsigned char* titlech = FileUtils::getInstance()->getFileData(path, "r", &size);
	if (titlech)
	{
		string json = std::string((const char*)titlech, size);
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
			{
				string name = it->name.GetString();
				string value = it->value.GetString();
				data[name] = value;
			}
		}
	}
}

void App::cancelData()
{
	//删除本地及内存账号信息记录
	if (App::GetInstance()->m_me) {
		delete App::GetInstance()->m_me;
		App::GetInstance()->m_me = NULL;
	}
	App::GetInstance()->myData.clear();
	//删除本地用户信息
	YYXLayer::deleteFileValue("userAccount");
	YYXLayer::deleteFileValue("userPassword");
	YYXLayer::deleteFileValue("userId");
	YYXLayer::deleteFileValue("userSex");
	YYXLayer::deleteFileValue("userCity");
	YYXLayer::deleteFileValue("userProvince");
	YYXLayer::deleteFileValue("userBalance");
	YYXLayer::deleteFileValue("ShowChildID");
	YYXLayer::deleteFileValue("ShowChildHeadPortrait");
	YYXLayer::deleteFileValue("ShowChildName");
	YYXLayer::deleteFileValue("ShowChildSex");
	YYXLayer::deleteFileValue("ShowChildBirthday");
	//删除全部红包
	App::GetInstance()->m_redPacket.clear();
	//暂停所有下载任务
	YYXDownload::GetInstance()->pauseAll();
	YYXLayer::DeleteDirectory(FileUtils::getInstance()->getWritablePath()+"temp");
	//清空已购列表
	App::GetInstance()->myBuyBookMap.clear();
	App::GetInstance()->myBuyBookVector.clear();
	//清空租书,最近下载 最近阅读
	App::GetInstance()->myRentBookMap.clear();
	App::GetInstance()->bookRead.clear();
	App::GetInstance()->bookDownLoad.clear();
	App::GetInstance()->bookCollect.clear();
	App::GetInstance()->bookDelete.clear();
	//创建temp目录
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "temp"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "temp");
}

void App::addTime(string key, long long data)
{
	if (!&key)
	{
		CCLOGERROR("参数异常");
		return;
	}
	timeMap[key.c_str()] = data;
}

long long App::getTime(string key, long long defaultTime)
{
	if (!&key)
	{
		CCLOGERROR("参数异常");
		return defaultTime;
	}
	long long result = defaultTime;
	if (timeMap.find(key.c_str()) != timeMap.end())
		result = timeMap[key.c_str()];
	return result;
}

//处理成圆形图片
void App::makeRoundImage(string path, string savePath)
{
	if (FileUtils::getInstance()->isFileExist(savePath))
	{
		FileUtils::getInstance()->removeFile(savePath);
	}
	NetIntface::cutTheRounded(path, savePath, 600, 600, "", [=](string path) {}, "", [](string str) {});
}

void App::addRentBook(int bookid)
{
	App::GetInstance()->myRentBookMap[bookid] = YYXLayer::getCurrentTime4Second();
	map<string, string> data;
	for (auto it : App::GetInstance()->myRentBookMap)
	{
		string key = StringUtils::format("%d", it.first);
		data[key] = StringUtils::format("%d",it.second);
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = StringUtils::format("rentBook/RentBook_%d.json", App::GetInstance()->m_me->id);
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
}

void App::deleteRentBook(int bookid)
{
	App::GetInstance()->myRentBookMap.erase(bookid);
}

//本地读取租书列举
void App::getLocalRentJson()
{
	if (App::GetInstance()->m_me)
	{
		string filename = StringUtils::format("rentBook/RentBook_%d.json", App::GetInstance()->m_me->id);
		string rentbookjsonpath = FileUtils::getInstance()->getWritablePath() + filename;
		if (FileUtils::getInstance()->isFileExist(rentbookjsonpath))
		{
			//如果存在, 假定是vip 后面通过网络请求刷新
			App::GetInstance()->m_me->vip = true;
			App::GetInstance()->m_me->startvip = "";
			App::GetInstance()->m_me->endvip = "";
			App::GetInstance()->m_me->vipTime = 0;
			map<string, string> rentbookdata;
			App::getMapFromFile(rentbookjsonpath, rentbookdata);
			App::GetInstance()->myRentBookMap.clear();
			for (auto it : rentbookdata)
			{
				int bookid = atoi(it.first.c_str());
				int time = atoi(it.second.c_str());
				if (bookid != 0)
				{
					string path = App::getBookRead4Json_txtPath(bookid);
					if (FileUtils::getInstance()->isFileExist(path))
						App::GetInstance()->myRentBookMap[bookid] = time;
				}
			}
		}
	}
}
//修改字符串, 全部替换
string App::replaceChar(string str, string oldChar,string newChar)
{
	string result = "";
	for (auto charstr : str)
	{
		string n = string(1, charstr);
		if (strcmp(n.c_str(), oldChar.c_str()) == 0)
			n = newChar;
		result = result + n;
	}
	return result;
}