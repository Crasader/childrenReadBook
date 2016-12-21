#ifndef __App_SCENE_H__
#define __App_SCENE_H__
#include "cocos2d.h"
#include "FileNames.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "json/rapidjson.h"
//#include "SqliteManager.h"
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

enum MySceneName {
	LoadScene//加载场景
	,BookRoomScene//书房
	, ParentScene//父母设置
	, IndexScene//首页
	, LoginScene//登陆
	, BabyCenterScene//宝贝中心
	, BookCity//书城
	, BookInfoScene//书籍详细
	, BookCityCHILD//书城中的书店	
	, PictureBook//绘本
	, Recommend//咿啦推荐
	, Free//限时免费
	, GoodReputation//五星好评
	, NewBook//咿啦新书
	, KangXuanStore//康轩书店
	, VIPBOOK//vip专属书店
};

struct ReadStart
{
	std::string startTime;
	int childrenId = -999;
	int bookId;
};

struct MyAccount
{
	int id;//用户id
	//int sex;//用户性别
	//std::string memberName;//用户名
	//std::string memberProvince;//省份
	//std::string memberCity;//城市
	int momey=0;//账户余额:单位 (分)
	//string showPortraitPath ="";//展示的头像全路径	
	bool vip = false;
	string startvip = "";
	string endvip = "";
	long long vipTime = 0;
};

struct AcountInfo
{
	std::string password;//密码
	std::string memberName;//用户名
};

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

	//static sqlite3* m_db;//数据库指针

public:
	static int m_debug ;//0=测试版本  1=正式版本
	static int m_PayTest;//0=正式  1=测试  (支付价格0.01)
	static void runTestFunction();

	static TTFConfig m_ttf;//公用字体

	static std::string m_resource;//设备

	string phoneModel = "android";

	string systemVersion = "";

	string version = "1.7.2";//应用版本

	int musicID = -999;//背景音乐ID

	vector<int> deleteMusicID;//记录需要停止的音乐ID

	//int DataBaseVersion = 6;//数据库版本

	vector<map<std::string ,YYXStruct>> m_SceneOrder;//记录场景的顺序

	MySceneName m_showScene;//前一个场景

	YYXStruct m_showSceneData;//前一个场景的参数

	static MySceneName m_RunningScene;//当前场景

	static string m_photsName;//照片路径

	MyAccount* m_me;//当前登录用户

	AcountInfo* m_acount;//账号信息

	ReadStart* m_read;//阅读记录

	//MyNotification* m_notification;//消息推送信息

	long long m_Time;//验证码时间

	int protectTime;//视力保护时间

	bool isBack;//安卓返回标识

	bool isOnlyWifi;//是否仅限WiFi下载

	bool isMusicPlay = true;//背景音是否播放

	bool isSoundEffect = true;//音效是否播放

	map<string, long long> timeMap;//时刻集合

	std::queue<YYXStruct> RefVector;//需要释放的界面集合

	map<string, YYXStruct> myData;//数据临时存储
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

	//最近下载
	map<int, int> bookDownLoad;

	static void addRecordBookDownload(int bookid);

	//收藏
	map<int, int> bookCollect;

	static void addRecordBookCollect(int bookid);

	//删除
	map<int, int> bookDelete;

	static void addRecordBookDelete(int bookid);


	//最近阅读
	map<int, int> bookRead;

	//记录阅读记录
	static void addRecordBookRead(int bookid);

	static void getMapFromFile(string path, map<string, string>& data);//读出文件里的json, 写入map

	static void cancelData();//注销后,内存数据处理

	std::vector<map<string, YYXStruct> > m_redPacket;//红包列表

	vector<int> myBuyBookVector;//已购列表顺序

	map<int, string> myBuyBookMap;//已购列表下载地址
								 
	//vector<int> myRentBookVector;//租书列表顺序

	map<int, int> myRentBookMap;//租书列表 租书时间

	static void addRentBook(int bookid);//加入租书列表, 写入本地

	static void deleteRentBook(int bookid);//(删除租书不修改本地, 在loadscene中根据本地书籍调整载入)

	static void getLocalRentJson();//读取本地租书json
	
	static string replaceChar(string str, string oldChar, string newChar);	//修改字符串, 全部替换

	void stopBackGroundMusic();//关闭

	void pauseBackGroundMusic();//暂停

	void resumeBackGroundMusic();//恢复

	void playBackGroundMusic();//播放背景音乐

	void stopOtherVoice();//关闭所有记录的音乐

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

	static void httpCheckVIP(int memberID);	

	static void httpComment(int bookid, function<void()> runFunction);

	//---------------csb文件缓存-----------------------------------------------------------------------------
	map<string,Data> CsbCache;
	bool addData(string path, Data& d);
	bool getData(string path, Data& d);
	//------------------csb文件缓存END----------------------------------------------------------------
	//---------------------------------工具方法----------------------------------------------
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
	static const char* getChars(const char* key);//获取字符串
	//static bool analysisMemberInfo(rapidjson::Value &data);//解析data节点
	static std::string analysisJsonString(rapidjson::Value &data, std::string key);//解析单个节点的key对应的值
	static int analysisJsonInt(rapidjson::Value &data, std::string key);
	static long long analysisJsonLongLong(rapidjson::Value & data, std::string key);
	static double analysisJsonDouble(rapidjson::Value &data, std::string key);
	//static Texture2D* readImage(std::string path);//读取本地路径的图片
	static std::string getMemberID();//获取用户ID
	static void examineIndex(int size, int index);//检查下标
	//static void initParaType(std::map<std::string, ParaType>& map, std::string key, long long number, std::string str);//初始化ParaType添加到map中
	static void ccsleep(int times);//休眠时间 ms
	static bool IsHaveFile(std::string path);//判断文件是否存在
	static string getFormatTime(time_t t);
	//static vector<map<string, myParaType>> analysisJson4SQLite(const char * json, vector<string> stringType, vector<string> longlongType);//解析原生数据库返回的json字符串
	//static string getString4map(map<string, myParaType> maps, string key,string default_str = "");
	//static long long getlonglong4map(map<string, myParaType> maps, string key, long long default_int=-999);
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

	//----------------------------------------场景跳转栈------------------------------------------------------
	void pushScene(MySceneName name, int data=-999,std::string str = "");

	void popBack(MySceneName dangqianScene = LoadScene);

	MySceneName getFromScene();
};

#endif // __App_SCENE_H__
