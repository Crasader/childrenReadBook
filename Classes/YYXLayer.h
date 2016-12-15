/****************************************************************************
YYX框架
针对cocos2dx封装图层类
俞越星
2016/06/12
****************************************************************************/

#ifndef __YYXLAYER_H__
#define __YYXLAYER_H__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui\UIVideoPlayer.h"
#include "json/rapidjson.h"
#include "network/HttpClient.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <fstream>
#include "App.h"
#include "YYXStruct.h"
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"

using namespace experimental;
using namespace CocosDenshion;
USING_NS_CC;
using namespace cocos2d::network; 
using namespace cocostudio::timeline;
using namespace std;
using namespace cocos2d::ui;

typedef struct int64String
{
	long long intPara;
	string stringPara;
};

class YYXLayer : public cocos2d::Layer
{
public:
	//数据源
	map<string, map<string, int64String>> sourceOfData;
	map<string, int64String> m_paramter;
	map<string, function<void(YYXLayer*)>> m_runable;
public:
	YYXLayer();
	virtual ~YYXLayer();
	//启动定时器
	void addSchedule(const string scheduleTimeName, float interval, const std::function<void()>&);
	//主线程回调函数
	virtual void MainThreadCallBack(string ,const std::function<void(string)>&);
	
	//获取控件
	Ref* findControl(string controlName);
	//离开场景
	virtual void leave();
	virtual void onEnterTransitionDidFinish();

	Node * getParentNode();

private:
	//定时器名称组
	vector<string> timeNames;
	//处于的当前场景
	string runScene;
	//csb文件路径
	string csbFilePath;
	//父节点
	Node* parentNode;
	//控件集合
	map<string, Ref*> controls;
	//类名
	string className;

private:	
	//图层初始化
	virtual bool init(map<string, int64String> paramter);
	virtual bool init(map<string, int64String> paramter, map<string, function<void(YYXLayer*)>> runable);
	//停止定时器
	virtual void stopSchedule();

//带资源封装
public:
	//模式对话框
	static YYXLayer* MyMessageBox(string titile,string yesString, function<void()> yesRunable, string noString, function<void()> noRunable);

//无资源封装
public:
	//时间戳转标准格式
	static string getStringTimeFromInt64Time(long long t);
	//网络请求
	static void stringHttpRequest(HttpRequest::Type type, string url, map<string, string> paramter, int outTime, std::function<void(string json)> runable, std::function<void()> errorRunable);
	//网络下载
	static void DownLoadNotify(string url, string dir, string fileName, string runKey, string errorKey);
	static void DownLoad(string url, string dir, string fileName, std::function<void(string json)> runable, std::function<void()> errorRunable);
	//使用转场场景进行转场
	static void ChangeScene(std::function<void()> runable);
	//创建等待图层
	static Layer* WaitLayer();
	//创建一个loading图层
	static YYXLayer* LoadingLayer(int dtimme, function<void()> runable);
	//删除loading图层
	static void deleteLoadingLayer(int t);
	//创建场景,设置图层的tag
	static Scene * createScene(map<string, int64String> paramter, int  tag, string sceneName);
	//创建图层
    static YYXLayer *create(map<string, int64String> paramter);
	//创建场景,设置图层的tag
	static Scene * createScene(map<string, int64String> paramter, map<string, function<void(YYXLayer*)>> runable, int  tag, string sceneName);
	//创建图层
	static YYXLayer *create(map<string, int64String> paramter, map<string, function<void(YYXLayer*)>> runable);
	//发送通知自定义事件
	static void sendNotify(string eventName, string className="",int data = -999);
	//发送通知带数据
	static void sendNotify4YYXStruct(string eventName, long long luserdata = -999, string userdata = "", string className = "");
	//点击间隔控制函数
	static void controlTouchTime(float outTime, string keyOfSaveTime, std::function<void()> runable, std::function<void()> unTouchRun = nullptr);
	//打开数据库
	//static sqlite3* OpenDataBase();
	//关闭数据库
	//static void CloseDataBase(sqlite3*);
	//插入记录到表
	//static bool InsertData(sqlite3*, string, unordered_map<string, string>, unordered_map<string, long long>);
	//更新记录
	//static bool UpdateData(sqlite3*, string, unordered_map<string, string>, unordered_map<string, long long>, string);
	//删除记录
	//static bool DeleteData(sqlite3*, string, string);
	//查找记录或记录中的某些字段
	//static vector<unordered_map<string, int64String>> SelectData(sqlite3*, string, vector<string>, string);
	//判断记录是否存在
	//static bool IsRecExist(sqlite3*, string, string);	
	//判断获取时间 单位:秒
	static long long getCurrentTime4Second();
	//每次获取不同的随机数
	static long long getRandom();
	//删除文件夹
	static void DeleteDirectory(string filePath);
	static void WStrToUTF8(std::string& dest, const wstring& src);
	static std::string ws2s(const std::wstring& ws);
	static std::wstring s2ws(const std::string& s);
	//字符串读写文件
	static void writeFile(string text, string path);
	static string readFile(string path);
	static void writeFilepp(string str, string path);//追加写文件;
	//键值对本地存入
	static void setFileValue(string key , string value);
	//键值对本地读出
	static string getFileValue(string key, string defaultstr);
	//删除键值对本地
	static void deleteFileValue(string key);
	//声音宏转换过来的函数 兼容性
	static bool getBoolFromXML(string str);
	static void PLAYBUTTON();
	static void CopyDirectory(string sourceDir, string destDir);
	//写json
	static string getStringFormMap(map<string, string>);
	//解析json
	static bool getJsonObject4Json(rapidjson::Document& doc, string json);
	static string getString4Json(string defaultResult, rapidjson::Value & doc, string key1, string key2 = "", string key3 = "");
	static int getInt4Json(int defaultResult, rapidjson::Value & doc, string key1, string key2 = "", string key3 = "");
	static long long getInt644Json(long long defaultResult, rapidjson::Value & doc, string key1, string key2 = "", string key3 = "");
	static bool getBool4Json(bool defaultResult, rapidjson::Value & doc, string key1, string key2 = "", string key3 = "");
	static double getDouble4Json(double defaultResult, rapidjson::Value & doc, string key1, string key2 = "", string key3 = "");
	static double getDoubleForJson(double defaultResult, rapidjson::Document & doc, string key1, string key2 = "", string key3 = "");
	static bool getJsonArray4Json(rapidjson::Value & result, rapidjson::Value & doc, string key1, string key2 = "", string key3 = "");
	static void getData4JsonArray(rapidjson::Value & jsonarray, map<string, YYXStruct>& result, std::function<void(rapidjson::Value&item, map<string, YYXStruct>&)> runable);
	static void getDataForJsonArray(rapidjson::Value & jsonarray, std::function<void(rapidjson::Value & item, int i)> runable);
	static bool getBoolForJson(bool defaultResult, rapidjson::Document& doc, string key1, string key2 = "", string key3 = "");
	static string getStringForJson(string defaultResult, rapidjson::Document & doc, string key1, string key2 = "", string key3 = "");
	static int getIntForJson(int defaultResult, rapidjson::Document & doc, string key1, string key2 = "", string key3 = "");
	static long long getInt64ForJson(long long defaultResult, rapidjson::Value & doc, string key1, string key2 = "", string key3 = "");
	static void buyBooksJsonWriteFile(vector<int> mapValue, int memberID, string path);//字符串写文件
	//paratype结构体赋值插入map
	static void insertMap4ParaType(std::map<std::string, int64String>& map, std::string key, long long number, std::string str);
	//打印函数运行状态
	static void logb(string str);
	static void loge(string str);
	//展示评论列表
	static void showCommentListView(ListView* listview, int bookid, int memberid=-1, int titleMaxLength = 15 * 3, int listviewMaxWidth = 320, int contentTextMaxLength = 135, int voiceMaxLength = 250);
	//遍历json字符串
	static void TraversingJson(string json, map<string, string>& data);
};
#endif // __YYXLAYER_H__