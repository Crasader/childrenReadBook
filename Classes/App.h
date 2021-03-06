﻿#ifndef __App_SCENE_H__
#define __App_SCENE_H__
#include "cocos2d.h"
#include "FileNames.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "json/rapidjson.h"
#include "network/HttpClient.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "json/document.h"
#include "RestRemind.h"
#include <fstream>
#include "YYXStruct.h"
#include "YYXDownload.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <unistd.h>
#endif

USING_NS_CC;
using namespace cocos2d::network;
using namespace std;

typedef pair<int, int> PAIR;

struct chargerDataStruct
{
	int index;
	int money;
	int hongbao;
	int hongbaoId;
};
struct ReadStart
{
	std::string startTime;
	int childrenId = -999;
	int bookId;
};

//struct MyAccount
//{
//	//int id;//用户id
//	//int sex;//用户性别
//	//std::string memberName;//用户名
//	//std::string memberProvince;//省份
//	//std::string memberCity;//城市
//	//int momey=0;//账户余额:单位 (分)
//	//string showPortraitPath ="";//展示的头像全路径	
//	bool vip = false;
//	string startvip = "";
//	string endvip = "";
//	long long vipTime = 0;
//};
//
//struct AcountInfo
//{
//	std::string password;//密码
//	std::string memberName;//用户名
//};

struct MyTime
{
	int verification;//短信验证码获取时间
	int readbook;//阅读时间
};

//struct MyNotification
//{
//	int pushId;//消息ID
//	int pushType;//1网络页面，2新书，3版本更新
//	std::string imageUrl;//图片URL
//	std::string pushUrl;//跳转URL
//	std::string pushString;//消息内容
//	std::string pushTitle;//消息标题
//	std::string pushVersion;//版本号
//};

class App 
{
private:
	App();   //构造函数是私有的	

	static App* m_pInstance;//类的单例

	static __Dictionary* m_strings;//字符串表

	static atomic_int message;
	static atomic_bool threadRuning;

public:
	Ref* ref = nullptr;
	void showRef(Ref* r= nullptr, int time =0);
	Ref* getRef();

	static int m_debug ;//0=测试版本  1=正式版本
	static int m_PayTest;//0=正式  1=测试  (支付价格0.01)
	static void runTestFunction();

	static TTFConfig m_ttf;//公用字体

	static std::string m_resource;//设备

	string phoneModel = "android";

	string systemVersion = "";

	string version = "1.9.5";//应用版本

	long long versioncode = 187;

	static string m_photsName;//照片路径

	long long m_Time;//验证码时间

	int protectTime;//视力保护时间

	bool isBack;//安卓返回标识

	bool isOnlyWifi;//是否仅限WiFi下载

	map<string, long long> timeMap;//时刻集合

	std::queue<YYXStruct> RefVector;//需要释放的界面集合

	map<string, YYXStruct> myData;//数据临时存储

	//管理网络接口和下载图片的线程
	void addMessage() {
		message += 1;
		manageThread();
	}
	void delMessage() {
		message -=1;
		if (message < 0)
			message = 0;
	}
	void manageThread();
	/*
	//APP版本数据
	<key> netVersion <value>(A+B+C, versionNum, nullptr);
	//当前选择需要展示的孩子
	std::string	ShowChildHeadPortrait;	//我需要展示的第一个孩子的头像 
	int64	ShowChildID;	//我需要展示的第一个孩子的ID
	int64	firstChildID;	//第一个位置孩子的ID
	int64	secondChildID;	//第2个位置孩子的ID
	int64	threeChildID;	//第3个位置孩子的ID
	//孩子的信息
	<key> name+childID=5454 <value>(childID, childName, nullptr);
	<key> path+childID=5454 <value>(uptime, childPath, nullptr);
	<key> birthday+childID=5454 <value>(sex, childBirthday, nullptr);
	<key> url+childID=5454 <value>(downState, url, nullptr); downState = 0 未下载 downState =1 下载
	int BabyCenterChangeChildEditButton 宝贝中心切换孩子场景, 记录切换孩子的编辑按钮状态  0=编辑  1 = 完成
	//用户信息
	string userAccount 账号
	string userPassword 密码
	int userSex 用户性别
	string userCity 用户城市
	string userProvince 用户省份
	BookCityScrollPosition //书城listview的位置记录
	//红包
	int rechargeSuccessIndex //充值成功的金额位置,对应可以找到红包活动
	<key> RechargeIndex=%d <value> (redPacketCount)//充值送红包 位置对应送红包价值
	<key> bookId=%d+redpacket <value> YYXStruct(coupon_amount,"",(Ref*)optimalID)
	//书籍信息 , 书店信息
	//书店书籍总数
	string totalCountKey = StringUtils::format("totalCount+bookStoreID=%d", bookStoreID);
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, totalCountKey, totalCount);
	//书店的排序
	string sortKey = StringUtils::format("sort=%d+bookStoreID=%d", pageIndex * 8 + index, bookStoreID);
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, sortKey, bookId);
			//书籍信息 原价+书籍名称+ 书页数
			string BookNameKey = StringUtils::format("bookName+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookNameKey, bookmarketPrice100, bookName, (Ref*)bookPage);
			//现价+封面url+新书标记
			string BookPriceKey = StringUtils::format("bookPrice+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPriceKey, bookPrice100, bookCoverUrl);
			//购书标记+下载URL
			string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, -999, bookPlayUrl);
			//活动倒计时 + 书籍作者
			string BookAuthorKey = StringUtils::format("bookAuthor+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookAuthorKey, remainTime, bookAuthor);
			//出版社
			string BookPressKey = StringUtils::format("bookPress+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPressKey, -999, bookPress);
			//书籍资源大小
			string BookSizeKey = StringUtils::format("bookSize+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookSizeKey, -999, bookSize);
			//书籍介绍
			string bookIntroductionKey = StringUtils::format("bookIntroduction+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookIntroductionKey, -999, bookSize);
			//试读url
			string bookViewUrlKey = StringUtils::format("bookViewUrl+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookViewUrlKey, -999, bookViewUrl)
	*/
	/*
	应用程序下的xml文件格式 使用自己的文件存储
	//文件名:UserInfo.json
	userAccount 账号
	userPassword 密码
	userId 用户ID
	userSex 用户性别
	userCity 用户城市
	userProvince 用户省份
	userBalance 用户账户余额
	ShowChildID 选中的孩子ID
	ShowChildHeadPortrait 选中孩子的头像
	ShowChildName 选中的孩子名字
	ShowChildSex 选中的孩子性别
	ShowChildBirthday 选中的孩子生日

	//记录本地阅读时间
	readBookId
	readStartTime
	readEndTime
	readMemberId
	readChildrenId
	*/
	static int getMemberId();

	//获取唯一字符串标记
	static string getOnlyKey();

	//记录报错记录
	static void addErrorLog(string error, string filename, int type);

	//上传日志
	static void upLogFiles();

	//遍历上传报错记录
	static void upLoadingErrorLog();

	//map排序
	static vector<PAIR> sortMapToVector(map<int, int> mapData);
	static vector<PAIR> sortMapToVector(std::unordered_map<int, int> mapData);

	//VIP包年服务
	map<int, int> VIPbook;
	static void addvipBook(int bookid);//写入本地包年图书
	static void loadvipBookCache();//本地缓存文件读入内存

	//购书列表
	map<int, int> myBuyBook;//已购列表
	static void loadBuyBookCache();//本地缓存文件读入内存

	//借书列表
	map<int, int> myBorrowBook;//借书列表
	static void loadBorrowBookCache();//本地缓存文件读入内存

	map<int, string> myBookURLMap;//我的书籍下载地址(购书+包年)

	static void getMapFromFile(string path, map<string, string>& data);//读出文件里的json, 写入map

	static void preLoad();//离线用户本地信息预加载

	static void loginCallback(bool hint = true, function<void()> runable = nullptr);//登录后的网络信息获取

	static void cancelData();//注销后,内存数据处理

	std::vector<map<string, YYXStruct> > m_redPacket;//红包列表
	
	static string replaceChar(string str, string oldChar, string newChar);	//修改字符串, 全部替换

	static string check_input_str(const string& str);
	//static void whetherForVipDownloadJudgeInCharge(int memberId, int bookId, function<void(int str)> runable, function<void(string error)> errorable);//判断包年用户能否进行vip下载

	void addTime(string key, long long data); //添加时刻
	
	long long getTime(string key, long long defaultTime);//获取时刻

	static void makeRoundImage(string path, string savePath);//图片切成圆形

	static App* GetInstance();

	static void writeFile(const char* str);

	static string getCoverDir();//封面目录

	static string getBookCoverPngPath(int bookid);//书籍封面绝对路径

	static string getTryReadZipDir();//试读书籍zip目录

	static string getBookTryReadZipPath(int bookid);//试读书籍zip绝对路径

	static string getTryReadDir();//试读书籍目录

	static string getBookTryReadPath(int bookid);//试读书籍绝对路径

	static string getBookTryRead4Json_txtPath(int bookid);//试读书籍的Iphone1334_view/json.txt绝对路径

	static string getReadZipDir();//完整书籍zip目录

	static string getBookReadZipPath(int bookid);//完整书籍zip绝对路径

	static string getReadDir();//完整书籍目录

	static string getBookReadPath(int bookid);//完整书籍绝对路径

	static string getBookRead4Json_txtPath(int bookid);//完整书籍的Iphone1334_view/json.txt绝对路径

	static void downloadFailDeleteFiles(string tryReadZipPath, string tryReadUNZipBookDir);//删除zip 和 解压的书籍文件夹

	static bool getNetSetAndHintSet();

	static void getVisible();

	//---------------csb文件缓存-----------------------------------------------------------------------------
	map<string,Data> CsbCache;
	bool addData(string path, Data& d);
	bool getData(string path, Data& d);
	//------------------csb文件缓存END----------------------------------------------------------------
	//---------------------------------工具方法----------------------------------------------
	static bool initImage(string path);
	static bool copyFile(string source , string ToDesc);
	static bool createDataBase();//保存数据库
	static void startTime();//启动定时器
	static long long getCurrentTime();//获取当前时间 秒
	static std::string GetStringTime();//获取系统时间，格式，例：2016/04/06 18:06:48
	static std::string GetStringTime2();
	static std::string GetStringTime3();
	//获取系统时间，格式，例：2016-04-06 18:06:48
	static bool isNight();//根据系统时间判断白天或夜晚
	static const char* getString(const char* key);
	static std::string analysisJsonString(rapidjson::Value &data, std::string key);//解析单个节点的key对应的值
	static int analysisJsonInt(rapidjson::Value &data, std::string key);
	static long long analysisJsonLongLong(rapidjson::Value & data, std::string key);
	static double analysisJsonDouble(rapidjson::Value &data, std::string key);
	static std::string getMemberID();//获取用户ID
	static void examineIndex(int size, int index);//检查下标
	static void ccsleep(int times);//休眠时间 ms
	static string getFormatTime(time_t t);
	//时间戳转成时间 16-02-12
	static void log(string str,long long count = -999);

	static void writeLog(string str, string dir, string pahtKey);

	static void protectedTiming();//开线程 视力保护

	static void threadTiming(long long);//根据视力保护时间开始计时

	//遍历上传阅读记录
	static void searchReadRecordJson();

	map<string, string> unDeleteFile;

	static void backThreading();//开启安卓返回键计时

	static void backWating();//安卓返回键计时
};
#endif // __App_SCENE_H__
