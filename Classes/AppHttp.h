#ifndef __App_HTTP_H__
#define __App_HTTP_H__

#include "User.h"
#include "network\HttpRequest.h"
#include "FileNames.h"
#include "SetBook.h"
#include <string>

using namespace std;

enum HttpPriority
{
	_high,
	_normal,
	_low
};
class HttpData :public Ref
{
private:
	cocos2d::network::HttpRequest::Type m_httpType = cocos2d::network::HttpRequest::Type::GET;
	bool sendImmediate = false;
	int m_taskId = -999;
	string m_url = "";
	long long m_startTime = -1;
	HttpPriority m_priority = _normal;
	int m_delayTime = 0;
	map<string, string> paramter;
	string  m_callbackKey = "";
	string  m_callbackerrorKey = "";
	string json = "";
	void* m_userData = nullptr;//用户传参用的自定义1
	Value m_userValue = Value(0);//用户传参用的自定义2
public:
	HttpData();
	~HttpData();
	static HttpData* create();
	static void del(HttpData*);

	string toString();
	std::string getUrl() const { return m_url; }
	void setUrl(std::string val) { m_url = val; }
	long long getStartTime() const { return m_startTime; }
	void setStartTime(long long val) { m_startTime = val; }
	HttpPriority getPriority() const { return m_priority; }
	void setPriority(HttpPriority val) { m_priority = val; }
	int getDelayTime() const { return m_delayTime; }
	void setDelayTime(int val) { m_delayTime = val; }
	HttpCallBack getCallback();
	void setCallback(const HttpCallBack& val);
	HttpCallBack getCallbackerror();
	void setCallbackerror(const HttpCallBack& val);
	cocos2d::network::HttpRequest::Type getHttpType() const { return m_httpType; }
	void setHttpType(cocos2d::network::HttpRequest::Type val) { m_httpType = val; }
	std::map<std::string, std::string> getParamter() const { return paramter; }
	void setParamter(std::map<std::string, std::string> val) { paramter = val; }
	bool getSendImmediate() const { return sendImmediate; }
	void setSendImmediate(bool val) { sendImmediate = val; }
	int getTaskId() const { return m_taskId; }
	std::string getJson() const { return json; }
	void setJson(std::string val) { json = val; }
	void* getUserData() const { return m_userData; }
	void setUserData(void* val) { m_userData = val; }
	cocos2d::Value getUserValue() const { return m_userValue; }
	void setUserValue(cocos2d::Value val) { m_userValue = val; }
};

class AppHttp
{
public:
	AppHttp();
	~AppHttp();
	static AppHttp* getInstance();
	//***********************************************************       网络请求        ****************************************************
	void httpVipAndBuyBooks();//获取租书和购书的全部书籍
	void httpChildren();//获取所有孩子信息
	void httpAddChild(ChildInfo* child);//添加一个孩子
	void httpDeleteChild(int childId);//删除一个孩子
	void httpAmendBabyInfo(int id, ChildInfo* child);//修改孩子信息
	void httpUploadDownloadRecord(int bookId);//上传下载记录
	void httpAppVersion();//网络获取APP版本号
	void httpComments(int bookid, const function<void()>& runFunction = nullptr);//获取评论
	void httpShareHongBao();//获取分享送红包
	void httpBookStoreSceneCurrentlyPageBookListInfo(int BookStoreId, int pageIndex, string visitFrom);//书店中获取当前页的书籍列表信息
	void httpUserBalance(const function<void(int)>& runable = nullptr);//获取用户的账号余额
	void httpAccountRegiste(string memberName, string memberPasswd, const function<void(string hongbao)>& callback = nullptr);//注册
	void httpUserRedPackets(YYXCallBack callback1 = nullptr);//获取用户红包
	void httpChargerInfo(int key = -999);//获取充值信息和充值活动
	void httpAmendUserInfo(int sex, string city, string province);//修改用户信息
	void httpLogIn(string& account, string password);//登录
	void httpBookInfo(int bookId, const function<void(string)>& callback1= nullptr);//获取书籍详情
	void httpBuyBookWithRedPacket(int couponId, int bookId, const function<void(int)>& callback = nullptr);//使用红包购书
	void httpBuyBookWithOutRedPacket(int bookId, const function<void(int)>& callback1 = nullptr, YYXCallBack callback2 = nullptr);//不使用红包购书
	void httpSendComment(int bookId, int score,  string comment);//发表评论
	void httpBookOrderId(int bookId, const function<void(string, int)>& callback1, YYXCallBack callback2 = nullptr);//获取当前书籍的订单情况
	void httpUploadReadBookRecord(ReadBookRecord* record, const function<void(ReadBookRecord*)>& callback1 = nullptr, const function<void(ReadBookRecord*)>& callback2 = nullptr);//上传阅读记录
	void httpDeleteComment(int id, YYXCallBack callback1);//删除评论
	void httpCheckVIP(YYXCallBack callback1 = nullptr);//获取用户的vip信息
	void httpThe_relationship_between_human_and_books(int bookId, const function<void(int)>& runable = nullptr);//人与书是什么关系
	void httpBookCityInfo(YYXCallBack callback1 = nullptr);//获取书城信息
	void httpNotification();//获取推送消息
	void httpCAPTCHA(std::string account);//获取验证码
	void httpChangePassword(string acount, string newPsd);//修改密码
	void httpRentBook(int bookid, YYXCallBack callback1 = nullptr);//租书
	void httpVisitorMemberId(string deviceStr, const function<void(int)>& callback1, YYXCallBack callback2);//获取游客ID
	void httpVipHint_VipType(int memberId, int key = -999); //会员提醒到期，购买会员类型列表
	void httpMyMoneyPayVip(int memberId, int cardType, int waitkey);//账户余额购买会员
	void httpUserIsOffLine();//账户是否离线
	void httpUserHaveBooks();//新的用户拥有书籍的接口
	//***********************************************************       管理队列        ****************************************************
	void deleteTask(int taskTag, bool del=true);//删除任务
	void clearAllReadyTask();//准备队列的任务全部取消
	void newHttp(HttpData* data);//网络请求加入队列
	void httpListControl();//网络请求队列管理
	bool getRuning() const { return m_runing; }
	void setRuning(bool val) { m_runing = val; }
	int createId();
	HttpCallBack getMycallback(string key);
	void addMycallback(string, const HttpCallBack&);
	void deleteMycallback(string key);
	void clearMycallback();
	bool getControlRun() const { return controlRun; }
	void setControlRun(bool val) { controlRun = val; }
private:
	static AppHttp* Instance;
//***********************************************************       管理队列        ****************************************************
	//全部任务
	map<int, HttpData*> m_Tasks;
	mutex m_Mtasks;
	void addTask(HttpData* );
	void delTask(int id);
	HttpData* getTask(int id);
	void delAllTask();
	int getTaskSize();

	//3条准备队列
	vector<int> highList;
	mutex m_MhighList;
	void pushHighList(int);
	int popHighList();
	int getHighListSize();

	vector<int> normalList;
	mutex m_MnormalList;
	void pushNormalList(int);
	int popNormalList();
	int getNormalListSize();

	vector<int> lowList;
	mutex m_MlowList;
	void pushLowList(int);
	int popLowList();
	int getLowListSize();

	//1条正在网络请求队列
	map<int, long long> httpList;//<id, startTime>
	mutex m_MhttpList;
	void addHttpList(int);
	void delHttpList(int);
	int getHttpListSize();

	bool m_runing = false;//运行中
	int maxTask = 20;//最大任务数  超过最大任务 不接请求 并且清除准备队列
	float outTime = 10;//超时时间
	int m_concurrence = 5;//并发数
	int m_MaxThread = 10;//最大并发数
	int m_endId = 0;//使用的最后一个ID

	std::unordered_map<string, HttpCallBack> m_mycallback;//全部回调的容器
	mutex m_Mmycallback;

	bool controlRun =true;//控制网络请求是否启动
	string m_appKey = "appkey";

	bool pushDownloadListFromHighList();
	bool pushDownloadListFromNormalList();
	void pushDownloadListFromLowList();


	void addMaxTask();//增加并发
	void decreaseMaxTask();//减少并发
	void addOutTime();//增加超时时间
	void decreaseOutTime();//减少超时时间
	void change4OutTime();//超时一次
	void change4GoodTime();//快速下载一次
	void http(HttpData * data);//核心代码

	string createSign(map<string,string> p);//签名
	map<string, string> signPostData(string methodName, map<string, string> pContent, string version);//方法名 接口参数 接口版本
};

#endif // __App_SCENE_H__
