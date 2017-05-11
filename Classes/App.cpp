#include "App.h"
#include "CocosAndroidJni.h"
//#include "DownloadRes.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "curl/include/win32/curl/curl.h"
#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "curl/include/android/curl/curl.h"
#endif
#include "CrossPlatform.h"
#include "YYXDownloadImages.h"
#include "DownloadBook.h"
#include "ReadBook.h"
#include "MyBook.h"
#include "FKBookParser.h"
#include "BookCache.h"
#include "User.h"
#include "YYXVisitor.h"
#include "YYXTime.h"
#include "AppHttp.h"
#include "HttpWaiting.h"
#include "BuyVip.h"
#include "vipNotifyLayer.h"
#include "LoadScene.h"

using namespace std;
USING_NS_FK;
/*
$(EngineRoot)external\win32-specific\zlib\include
libcurl_imp.lib

void App::addErrorLog(string error, string filename, int type)   type 1=网络错误  2=文件错误  3= 日志文件上传, 4=支付错误
*/
App* App::m_pInstance = nullptr;

__Dictionary* App::m_strings = nullptr;

std::atomic_int App::message(0);
std::atomic_bool App::threadRuning(false);

TTFConfig App::m_ttf = TTFConfig("hkhb.TTF", 24);

std::string App::m_resource = "android";// "iphone6";

//sqlite3* App::m_db = nullptr;//数据库指针

int App::m_debug = 1;//0=测试版本  1=正式版本
int App::m_PayTest = 0;
void App::showRef(Ref* r, int time)
{
	if(r)	ref = r;
	thread([=]() {
		ccsleep(time);
		if (ref)
		{
			int count = ref->getReferenceCount();
			string str = "\nref = " + Value(count).asString();
			YYXLayer::writeFilepp(str, FileUtils::getInstance()->getWritablePath() + "temp/OOM.txt");
			App::log(str);
		}
	}).detach();
}

cocos2d::Ref* App::getRef()
{
	return ref;
}

//0=正式  1=测试  支付价格0.01
void App::runTestFunction()
{
	//auto con = AppHttp::getInstance();
	//con->setControlRun(!con->getControlRun());
	//if (con->getControlRun())
	//	Toast::create("run");
	//else
	//	Toast::create("stop");
	//Director::getInstance()->end();
	//auto show = ShowNotification::create(true);
	//if (show)
	//	Director::getInstance()->getRunningScene()->addChild(show);
	thread([]() {
		auto http = AppHttp::getInstance();
		http->httpAppVersion();
		http->httpBookCityInfo();
		for (int i = 1; i < 300; i += 1)
		{
			http->httpBookInfo(i);
			ccsleep(500);
		}
		for (int i = 0; i < 200; i++)
		{
			string str = "111";
			http->httpLogIn(str, "111");
			ccsleep(500);
		}
	}).detach();
}

string App::m_photsName ="0";

App::App()   //构造函数是私有的
{	
	//m_notification = nullptr;
	//m_acount = new AcountInfo();
	m_Time = 0;
	protectTime = 0;
	isOnlyWifi = true;
	//m_read = new ReadStart();
	//m_read->bookId = -999;
	//m_read->childrenId = -999;
	//m_read->startTime = "";
	isBack = true;
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
	int memberid = -999;
	if (YYXVisitor::getInstance()->getVisitorMode())
		memberid = YYXVisitor::getInstance()->getMemberId();
	else
		memberid = User::getInstance()->getMemberId();
	return Value(memberid).asString();
}

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
	bool net = CrossPlatform::IsNetConnect(true);
	if (!net)
		return false;
	if (App::GetInstance()->isOnlyWifi)//无设置或者仅wifi
	{
		App::log("jin wifi----------");
		bool wifistauts = CrossPlatform::isWifi();
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

bool App::addData(string path, Data& result)
{
	if (&CsbCache == nullptr)
		return false;
	//if (CsbCache.size() > 10)
	//{		
	//	App::log("--------------------------CsbCache.size()  = ", CsbCache.size());
	//	CsbCache.clear();
	//}
	if (!FileUtils::getInstance()->isFileExist(path))
		return false;
	auto data = FileUtils::getInstance()->getDataFromFile(FileUtils::getInstance()->fullPathForFilename(path));
	if (data.getSize() <= 0)
		return false;
	CsbCache[path] = data;
	result = data;
	App::log(path + " add csb File cache");
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

bool App::initImage(string path)
{
	auto image = new (std::nothrow) Image();
	bool bRet = image->initWithImageFile(path);
	if (image)
		delete image;
	return bRet;
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

const char* App::getString(const char* key)
{
	if(m_strings ==nullptr)
		m_strings = Dictionary::createWithContentsOfFile("strings.xml");
	m_strings->retain();
	//读取Hello键中的值 objectForKey根据key，获取对应的string
	auto val = m_strings->objectForKey(key);
	if (val)
	{
		auto res = ((__String*)val)->_string.c_str();
		return res;
	}
	else
	{
		App::log(string(key) + " nothingness in string.xml");
		return "";
	}
}

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
					App::log(it.stringData , (long)it.refData);
					while (it.refData != nullptr && it.refData->getReferenceCount() > 1 && it.refData->getReferenceCount() < 100)
					{
						it.refData->release();						
					}
					if (it.refData->getReferenceCount() == 1)
					{
						it.refData->release();
						it.refData = nullptr;
					}
					if (it.refData != nullptr)
						App::GetInstance()->RefVector.pop();
					App::log(it.stringData + "::getReferenceCount end", it.refData->getReferenceCount());
					App::log(it.stringData + "end", (long)it.refData);
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

//将剩下的阅读记录上传到服务器
void App::searchReadRecordJson()
{
	CrossPlatform::TraversingFiles(FileUtils::getInstance()->getWritablePath() + "readBookRecord", [=](string filePath, string name) {
		string str = YYXLayer::readFile(filePath);
		rapidjson::Document doc;
		if (YYXLayer::getJsonObject4Json(doc, str))
		{
			string readBookId = YYXLayer::getStringForJson("", doc, "readBookId");
			string readChildren = YYXLayer::getStringForJson("", doc, "readChildren");
			string readEndTime = YYXLayer::getStringForJson("", doc, "readEndTime");
			string readMemberId = YYXLayer::getStringForJson("", doc, "readMemberId");
			string readStartTime = YYXLayer::getStringForJson("", doc, "readStartTime");
			auto record = ReadBookRecord::create();
			record->setReadBookId(Value(readBookId).asInt());
			record->setReadChildren(Value(readBookId).asInt());
			record->setReadMemberId(Value(readMemberId).asInt());
			record->setReadStartTime(readStartTime);
			record->setReadEndTime(readEndTime);
			AppHttp::getInstance()->httpUploadReadBookRecord(record, [=](ReadBookRecord* r) {
				ReadBookRecord::del(r);
				if (FileUtils::getInstance()->isFileExist(filePath))
				{
					if (!FileUtils::getInstance()->removeFile(filePath))
						App::GetInstance()->unDeleteFile[filePath] = "";
				}
			}, [](ReadBookRecord* r) {
				ReadBookRecord::del(r);
			});
		}
	}, [](string dirPath, string name) {
		App::log("NetIntface::TraversingFiles dir =" + dirPath);
	});
}

string App::getFormatTime(time_t t)
{	
	struct tm *p;
	p = gmtime(&t);
	char s[80];
	strftime(s, 80, "%Y-%m-%d", p);
	return s;
}

void App::log(string str,long long count)
{
	if (App::m_debug == 1)
		return;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (count == -999)
		CocosAndroidJni::Log("show", str.c_str());
	else
		CocosAndroidJni::Log("show", StringUtils::format("%s / %lld", str.c_str(), count).c_str());
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if(count == -999)
		CCLOG("%s", str.c_str());
	else
		CCLOG("%s / %lld", str.c_str(), count);
#endif
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
		auto fileName = StringUtils::format("/%lld.dat", YYXTime::getInstance()->getRandom());
		path = dir + fileName;
		YYXLayer::setFileValue(pahtKey, path);
	}
	if (FileUtils::getInstance()->isFileExist(path))
	{
		auto fsize = FileUtils::getInstance()->getFileSize(path);
		if (fsize > 500000)
		{
			auto fileName =StringUtils::format("/%lld.dat", YYXTime::getInstance()->getRandom());
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

void App::manageThread()
{
	//1.开启一个线程
	//2.管理网络接口
	//3.管理下载图片
	//5.管理message信号
	if (App::threadRuning)
		return;
	thread mythread([]() {
		App::log("mmmmmmmmmmmmmmmmmmmmmm  < manageThread >    mmmmmmmmmmmmmmmmmmmmmmmmm");
		App::threadRuning = true;
		do
		{
			YYXDownloadImages::GetInstance()->startTask();
			AppHttp::getInstance()->httpListControl();
			App::GetInstance()->delMessage();
			App::ccsleep(2000);
		} while (App::message > 0);
		App::threadRuning = false;
		App::log("wwwwwwwwwwwwwwwwwwwwwwwwwww  < manageThread > ******************   wwwwwwwwwwwwwwwwwwwwwwwww");
	});
	mythread.detach();
}

int App::getMemberId()
{
	int memberid = -999;
	if (YYXVisitor::getInstance()->getVisitorMode())
		memberid = YYXVisitor::getInstance()->getMemberId();
	else
		memberid = User::getInstance()->getMemberId();
	return memberid;
}

string App::getOnlyKey()
{
	auto str = StringUtils::format("%lld", YYXTime::getInstance()->getRandom());
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
	string path = FileUtils::getInstance()->getWritablePath() + "errorLog/" + StringUtils::format("%d.dat", YYXTime::getInstance()->getRandomL());
	YYXLayer::writeFile(josn, path);
}

void App::upLogFiles()
{
	CrossPlatform::TraversingFiles(FileUtils::getInstance()->getWritablePath() + "temp/Log", [=](string filePath, string name) {
		App::log("upLoadingErrorLog  " + filePath);
		auto cocaltionfile = YYXLayer::getFileValue("LogFileName", "");
		if (cocaltionfile == filePath)
			return;
		string url = "";
		//if (App::m_debug == 0)
		//	url = string("http://192.168.10.10:8089").append(NET_UPLOGFILE);
		//else
			url = string(IP).append(NET_UPLOGFILE);
		map<string, string> parater;
		parater["createTime"] = StringUtils::format("%lld", YYXLayer::getCurrentTime4Second());
		parater["models"] = App::GetInstance()->phoneModel;
		parater["memberid"] = App::getMemberID();
		parater["resource"] = App::m_resource;
		parater["systemInfo"] = App::GetInstance()->systemVersion;
		parater["logType"] = "3";
		CrossPlatform::httpUpFile(url, YYXLayer::getStringFormMap(parater), filePath, [=](string json) {
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
	Parent::DeleteDirectory(FileUtils::getInstance()->getWritablePath() + "errorLog");
	/*NetIntface::TraversingFiles(FileUtils::getInstance()->getWritablePath() + "errorLog", [=](string filePath, string name) {
		App::log("upLoadingErrorLog  " + filePath);
		string str = YYXLayer::readFile(filePath);
		map<string, string> paramter;
		YYXLayer::TraversingJson(str, paramter);
		string url = "";
		//if (App::m_debug == 0)
		//	url = string("http://192.168.10.10:8089").append(NET_UPERRORLOG);
		//else
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
	});*/
}

bool cmp_by_value(const PAIR& lhs, const PAIR& rhs) {
	return lhs.second > rhs.second;
}

vector<PAIR> App::sortMapToVector(map<int , int> mapData)
{
	vector<PAIR> vec(mapData.begin(), mapData.end());
	sort(vec.begin(), vec.end(), cmp_by_value);
	return vec;
}

vector<PAIR> App::sortMapToVector(std::unordered_map<int, int> mapData)
{
	vector<PAIR> vec(mapData.begin(), mapData.end());
	sort(vec.begin(), vec.end(), cmp_by_value);
	return vec;
}

void App::getMapFromFile(string path, map<string, string>& data)
{
	if (!FileUtils::getInstance()->isFileExist(path))
		return;
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

//离线用户本地信息预加载
void App::preLoad()
{
	//是否仅WiFi下载
	string result = YYXLayer::getFileValue("IS_ONLY_WIFI", "");
	if (result == "" || result == "0")
		App::GetInstance()->isOnlyWifi = true;
	else
		App::GetInstance()->isOnlyWifi = false;
	//获取视力保护时间
	string PROTECT_TIMEstr = YYXLayer::getFileValue("PROTECT_TIME", "0");
	App::GetInstance()->protectTime = atoi(PROTECT_TIMEstr.c_str());
	//获取通知
	auto doingFunction = [](string key, string typeName) {
		if (typeName == "INT")
		{
			auto number = YYXLayer::getFileValue(key, "-999");
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, key, atoi(number.c_str()));
		}
		else if (typeName == "STRING")
		{
			auto valu = YYXLayer::getFileValue(key, "");
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, key, -999, valu);
		}
	};
	doingFunction("pushId", "INT");
	doingFunction("pushType", "INT");
	doingFunction("imageUrl", "STRING");
	doingFunction("pushUrl", "STRING");
	doingFunction("pushTitle", "STRING");
	doingFunction("pushString", "STRING");
	doingFunction("pushVersion", "STRING");
}

//登录后的网络信息获取
void App::loginCallback(bool hint ,function<void ()>  runable)
{
	//载入购买书籍时间
	MyBook::getInstance()->clearBook();
	MyBook::getInstance()->loadCache();
	//载入最近阅读时间
	ReadBook::getInstance()->clearBook();
	ReadBook::getInstance()->loadCache();
	//载入最近下载时间
	DownloadBook::getInstance()->clearBook();
	DownloadBook::getInstance()->loadCache();
	//加载vip包年书籍列表
	loadvipBookCache();
	//载入购书信息
	loadBuyBookCache();
	//载入收藏列表
	//loadCollectBookCache();

	//查询用户包年服务信息
	AppHttp::getInstance()->httpCheckVIP();
	//本地读取vip包年图书列表(租书列表),
	//httpGetVipBook(hint);
	//网络请求收藏列表
	//httpGetCollectBook(hint);
	//获取用户已购列表
	//httpGetBuyBook(hint);
	//获取用户红包
	AppHttp::getInstance()->httpUserRedPackets();
}

//注销
void App::cancelData()
{
	App::log("**************************************************************************** App::cancelData()**");
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
}

void App::loadvipBookCache()
{
	string path = FileUtils::getInstance()->getWritablePath() + "vipBook/vipbook_" + App::getMemberID() + ".json";
	map<string, string> data;
	App::getMapFromFile(path, data);
	for (auto it : data)
	{
		int bookid = atoi(it.first.c_str());
		int time = atoi(it.second.c_str());
		if (bookid > 0)
			App::GetInstance()->VIPbook[bookid] = time;
	}
}

void App::loadBuyBookCache()
{
	//载入购书信息
	string bookjsonPath = FileUtils::getInstance()->getWritablePath() + "buyBook/buybook_" + App::getMemberID() + ".json";
	map<string, string> data;
	App::getMapFromFile(bookjsonPath, data);
	for (auto it : data)
	{
		int bookid = atoi(it.first.c_str());
		int orderid = atoi(it.second.c_str());
		if (bookid > 0)
			App::GetInstance()->myBuyBook[bookid] = orderid;
	}
}
void App::addTime(string key, long long data)
{
	if (!&key)
		return;
	timeMap[key.c_str()] = data;
	if (timeMap.size() > 50)
		timeMap.clear();
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
	CrossPlatform::cutTheRounded(path, savePath, 600, 600, "", [=](string path) {}, "", [](string str) {});
}

//加入本地包年图片的列表(租书列表)
void App::addvipBook(int bookid)
{
	App::GetInstance()->VIPbook[bookid] = (int)YYXLayer::getCurrentTime4Second();
	map<string, string> data;
	for (auto it : App::GetInstance()->VIPbook)
	{
		string key = StringUtils::format("%d", it.first);
		data[key] = StringUtils::format("%d",it.second);
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = StringUtils::format("vipBook/vipbook_%d.json", App::getMemberId());
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
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