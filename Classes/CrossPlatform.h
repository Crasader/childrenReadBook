#ifndef __NETINTFACE_SCENE_H__
#define __NETINTFACE_SCENE_H__
//2016.04.09 俞越星*/
#include "App.h"

using namespace std;

class CrossPlatformData
{
public:
	CrossPlatformData();
	~CrossPlatformData();
	static CrossPlatformData* create();
	static void del(CrossPlatformData* data);

	CallBack_String getCall1Key();
	void setCall1Key(const CallBack_String& val);
	CallBack_String getCall2Key();
	void setCall2Key(const CallBack_String& val);
	int getTaskName() const { return m_taskName; }
	void setTaskName(int val) { m_taskName = val; }
private:
	int m_call1Key = -999;
	int m_call2Key = -999;
	int m_taskName = -999;
};

class CrossPlatform
{
private:
	static CrossPlatform *m_pInstance;
	std::unordered_map<int, CallBack_String> m_allFunc;
	std::unordered_map<int, CrossPlatformData*> m_allTask;
public:
	static CrossPlatform* getInstance();
	//函数方法
	mutex m_MallFunc;
	CallBack_String getFunc(int key);
	void addFunc(int key, const CallBack_String& val);
	void delFunc(int key);
	//处理的对象
	mutex m_MTask;
	CrossPlatformData* getTask(int key);
	void addTask(CrossPlatformData * data);
	void delTask(int key);
public:
	static map<string, function<void(string)>> m_functionMap;//代码存储
	/*
	下载回调 key= "download%d" ,YYXLayer::getRandom() 
	*/
	static function<void(string)> getMapFunction(string key);
	static void setMapFunction(string key, function<void(string)>);
	static void deleteMapFunction(string key);
	static bool IsNetConnect(bool hint = false);

	// JNI 获取毫秒时间, 用于运算时间差
	static long long getMillsTime();
	//网络请求上传文件接口
	static void httpUpFile(string url, string jsonparater, string filepath, function<void(string)> runFunction, function<void(string)> errorRunFunction);
	//遍历文件夹
	static void TraversingFiles(string path, function<void(string filePath, string name)> fileRun, function<void(string fileDir, string name)> dirRun);
	//判断wifi是否存在
	static bool isWifi();
	//获取下载的目录
	static string getDownloadDir();
	//安装 安装包
	static void installInstallationPackage(string path);
	// 支付
	//static void httpPay(int memberId, int rechargeCount, int payMoney, string payType, string payinfo, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);

	//开通包年服务
	static void httpVIPPay(int memberId, int rechargeCount, int payMoney, string payType, string payinfo, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//获取当前版本号
	static string getAppVersion();
	//获取本地相册的绝对路径
	static string getAlbumAbsolutePath();
	//打开相册选中图片,图片保存到指定位置
	static void openPhotoAlbumSelectImage(string fileName, string dir, long width, long height, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//上传头像
	static void httpUpImage(int childID, string ImageFullPath, int memberId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//拍照, 保存到指定图片路径
	static void photograph(string fileName, string dir, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//图片切成圆形
	static void cutTheRounded(string path, string savePath, long width, long height, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//下载文件
	static void DownLoadFile(string url, string dir, string fileName, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//下载图片
	static void DownLoadImage(string url, string dir, string fileName, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_DownLoad(string url, string dir, string fileName, string runKey, string errorKey);
	//分享
	static void share(string filePath, string bookName, string targetUrl, string headUrl, string title, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//获取用户有效红包列表
	static void httpGetUserRedPacketsCallBack(string json, const function<void()> runBegin, const function<void(int, int, string)> itemRun, const function<void(int)> runable, const function<void()> errorRunable);
	//发表语音评论(带界面)
	static void goToSendRecording(int bookId, int memberId, int types, string membername, string orderid, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction, string closeKey, function<void(string)> closeFunction);
	//邀请注册送红包
	static void inviteRegister(int memberId, string url, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//获取机型
	static string getPhoneModel(int um);


	//最新的支付宝支付接口
	void pay(int memberId, int payObjectId, string card_or_recharge, string payPlatform, const CallBack_String & call1 = nullptr, const CallBack_String & call2 = nullptr);
	//新的相册换头像
	void newAlbumChangeHead(int childId, string oldHead, string newHead, const CallBack_String & call1, const CallBack_String & call2);
	//新的绑定手机
	void newBindPhone(string uid, string platform, const CallBack_String & call1, const CallBack_String & call2);
	//新的绑定第三方
	void newBindThird(string phone, string uid, int memberId, string platform, const CallBack_String & call1, const CallBack_String & call2);
	//新的注销
	void newAccountCancel(string memberAccount, int memberId, const CallBack_String & call1, const CallBack_String & call2);
	//新的解绑第三方
	void newUnBindThird(int memberId, const CallBack_String & call1, const CallBack_String & call2);
	//新的修改密码
	void newChangePassword(const CallBack_String & call1, const CallBack_String & call2);
	//新的踢下线
	void newOffLine();
	//设置memberid
	void newSetMemberId(string memberid);
	//分享借阅
	void ShareWeb(int memberid, int bookid);
	//删除渠道标记
	void deletequdao();
	//跳转QQ
	void gotoQQ(string qq);
	//复制字符串
	void CtrlC(string qq);
};

#endif // __NETINTFACE_SCENE_H__