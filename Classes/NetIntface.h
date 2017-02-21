#ifndef __NETINTFACE_SCENE_H__
#define __NETINTFACE_SCENE_H__
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//2016.04.09 俞越星*/
#include "IndexScene.h"
#include "App.h"
#include "YYXLayer.h"
USING_NS_CC;
using namespace std;
class NetIntface : public YYXLayer
{
private:
	static NetIntface *m_pInstance;
public:
	static map<string, function<void(string)>> m_functionMap;//代码存储
	/*
	下载回调 key= "download%d" ,YYXLayer::getRandom() 
	*/
	
public:
	NetIntface * getInstance();
	static function<void(string)> getMapFunction(string key);
	static void setMapFunction(string key, function<void(string)>);
	static void deleteMapFunction(string key);
	static bool IsNetConnect(bool hint = false);
	// 打开数据库
	//static bool openDataBase();
	// 判断数据库是否正常,判断数据库的版本是否最新,否则重建数据库
	//static int examineDataBase();
	// JNI 获取毫秒时间, 用于运算时间差
	static long long getMillsTime();
	//网络请求上传文件接口
	static void httpUpFile(string url, string jsonparater, string filepath, function<void(string)> runFunction, function<void(string)> errorRunFunction);
	//网络请求POST方法
	static void httpPost(string url, map<string, string> parameter, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpPost(string url, map<string, string> parameter, string runKey, string errorKey);
	// 网络请求GET
	static void httpGet(string url, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpGet(string url, string runKey, string errorKey);
	//遍历文件夹
	static void TraversingFiles(string path, function<void(string filePath, string name)> fileRun, function<void(string fileDir, string name)> dirRun);
	//判断wifi是否存在
	static bool isWifi();
	//获取下载的目录
	static string getDownloadDir();
	//上传下载记录
	static void AddDownLoadRecord(int memberId, int bookId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//获取网络版本号
	static void httpAppVersion(string resourst, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpAppVersion(string resourst, string runKey, string errorKey);
	static void httpAppVersionCallBack(string json, function<void(string)> runable, function<void(string)> errorRun);
	//安装 安装包
	static void installInstallationPackage(string path);
	////获取评论
	static void httpGetComments(int bookid, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void httpBookCommentsCallback(std::string json, const function<void(int, string, string, string, string, string, string, string, string, string, string, string, string)> itemRun, const function<void()> runable, const function<void()> errorRunable);
	//获取充值红包活动
	static void httpGetRechargeActivityCallBack(string json, const function<void(int, int, int)> hongbaoList, const function<void(int, int)> chargerList, const function<void()> runable, const function<void()> errorRunable);
	// 支付
	static void httpPay(int memberId, int rechargeCount, int payMoney, string payType, string payinfo, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//开通包年服务
	static void httpVIPPay(int memberId, int rechargeCount, int payMoney, string payType, string payinfo, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	// 获取书城信息json解析函数
	static void httpBookCityInfoCallBack(string json, const function<void(int, int, int, int, int, int, string, string, string, string)> itemRun, const function<void(int)> runable, const function<void()> errorRunable);
	// 获取分享成功的红包
	static void httpShareWithCoupon(int memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpShareWithCoupon(int memberID, string runKey, string errorKey);
	static void httpShareWithCouponCallBack(string json, const function<void(int)> itemRun, const function<void()> runable, const function<void()> errorRunable);
	// 获取书店当前页的书籍列表信息
	static void httpBookStoreSceneCurrentlyPageBookListInfo(int memberID, int BookStoreId, int pageIndex, string m_visitFrom, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpBookStoreSceneCurrentlyPageBookListInfo(int memberID, int BookStoreId, int pageIndex, string m_visitFrom, string runKey, string errorKey);
	static void httpBookStoreSceneCurrentlyPageBookListInfoCallBack(string json, const function<void(int, int, int, int, int, int, int, int, string, string, string, string, string)> itemRun, const function<void()> runable, const function<void()> errorRunable);
	//获取当前版本号
	static string getAppVersion();
	//用户书籍的已购列表
	static void httpGetUserBuyBooks(long memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpGetUserBuyBooks(int memberID, string runKey, string errorKey);
	static void httpGetUserBuyBooksCallBack(string json, function<void()> beginable, const function<void(int, int, string, string, string)> itemable, function<void()> runableSuccessOver, const function<void()> errorRunable);
	//获取用户安卓的账户余额
	static void httpGetUserBalance(long memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpGetUserBalance(int memberID, string runKey, string errorKey);
	static void httpGetUserBalanceCallBack(string json, const function<void(int, int, long long)> runable, const function<void()> errorRunable);
	//注册
	static void httpAccountRegiste(string memberName, string memberPasswd, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpAccountRegiste(string memberName, string memberPasswd, string runKey, string errorKey);
	static void httpAccountRegisteCallBack(string json, const function<void(int, int, string, string, string, string)> runable, const function<void(string errorstr)> errorRunable);
	//修改用户信息
	static void httpAmendUserInfo(int memberId, int sex, string city, string province, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpAmendUserInfo(int memberId, int sex, string city, string province, string runKey, string errorKey);
	static void httpAmendUserInfoCallBack(string json, const function<void()> runable, const function<void()> errorRunable);
	//字符集转换
	static string UTF82GBK(string UTF);
	static string GBK2UTF8(string GBK);
	//获取本地相册的绝对路径
	static string getAlbumAbsolutePath();
	//修改宝贝信息
	static void httpAmendBabyInfo(int childrenId, string childrenName, int childrenSex, string childrenBirth, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpAmendBabyInfo(int childrenId, string childrenName, int childrenSex, string childrenBirth, string runKey, string errorKey);
	//拍照,裁切成圆形图片保存到指定位置
	//static void photographAlbumSelectImage(string fileName, string dir, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//打开相册选中图片,图片保存到指定位置
	static void openPhotoAlbumSelectImage(string fileName, string dir, long width, long height, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//上传头像
	static void httpUpImage(int childID, string ImageFullPath, int memberId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//拍照, 保存到指定图片路径
	static void photograph(string fileName, string dir, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//图片切成圆形
	static void cutTheRounded(string path, string savePath, long width, long height, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//登陆
	static void httpLogIn(string account, string password, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpLogIn(string account, string password, string runKey, string errorKey);
	static void httpLogInCallBack(string json, const function<void(int, int, int, string, string)> runable, const function<void(string)> errorRunable);
	//删除孩子
	static void httpDeleteChild(int memberId, int childrenId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpDeleteChild(int memberId, int childrenId, string runKey, string errorKey);
	static bool httpDeleteChildCallBack(string json);
	//添加孩子
	static void httpAddChild(int memberId, string childrenName, int childrenSex, string childrenBirth, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpAddChild(int memberId, string childrenName, int childrenSex, string childrenBirth, string runKey, string errorKey);
	static void httpAddChildCallBack(string json);
	//下载文件
	static void DownLoadFile(string url, string dir, string fileName, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//下载图片
	static void DownLoadImage(string url, string dir, string fileName, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_DownLoad(string url, string dir, string fileName, string runKey, string errorKey);
	//static void DownLoadImage(string url, string dir, string fileName, string runKey="", string errorKey="");
	//获取孩子列表
	static void httpGetChildDetails(int memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpGetChildDetails(int memberID, string runKey, string errorKey);
	static void httpGetChildDetailsCallBack(string json, const std::function<void(int, int, int, string, string, string, long long)> runable, const std::function<void(int)> resultRunBegin);
	static void httpGetChildDetails_businessLogic(int index, int childrenId, int childrenSex, string childrenName, string childrenBirth, string url,long long uptime);
	//获取第一个孩子的头像
	static void httpGetFirstChildHeadPortrait(int memberID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpGetFirstChildHeadPortrait(int memberID,  string runKey, string errorKey);
	static void httpGetFirstChildHeadPortraitCallBack(string json, function<void(int, string)> runable, function<void()> errorRunable);
	//获取书籍详情
	static void httpGetBookInfo(int bookInfoID, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void WIN32_httpGetBookInfo(int bookInfoID, string runKey, string errorKey);
	static void httpGetBookInfoCallBack(string json, const function<void(bool, int, int, int, int, int, string, string, string, string, string, string, string, string)> runable, const function<void()> errorRunable);
	//分享
	static void share(string filePath, string bookName, string targetUrl, string headUrl, string title, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//获取用户有效红包列表
	static void httpGetUserRedPacketsCallBack(string json, const function<void()> runBegin, const function<void(int, int, string)> itemRun, const function<void(int)> runable, const function<void()> errorRunable);
	//static void httpGetUserRedPackets(int memberID, string runKey, string errorKey);
	//static void WIN32_httpGetUserRedPackets(int memberID, string runKey, string errorKey);
	

	//保存阅读记录
	static void saveReadRecord(long memberId, long childrenId, long bookId, string readStartTime, string readEndTime, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void saveReadRecordCallBack(string json, const function<void()> runable, const function<void()> errorRunable);

	//***********************************************
	//***********************************************
	//***********************************************
	//***************使用httpPost httpGet改造跨平台********************************
	//使用红包购书
	static void httpBuyBookWithRedPacket(int memberId, int couponId, int bookId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void httpBuyBookWithRedPacketCallback(string json, function<void(string str)> runable, function<void(string error)> errorRun);
	//static void WIN32_httpBuyBookWithRedPacket(int memberId, int couponId, int bookId, string runKey, string errorKey);
	//无红包纯余额支付购书
	static void httpBuyBookWithOutRedPacket(int memberId, int bookId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void httpBuyBookWithOutRedPacketCallback(string json, function<void()> runable, function<void()> yuebuzu, function<void()> errorRun);
	//发表评论
	static void httpSendComment(int types, int bookId, int memberId, int score, string orderId, string memberName, string title, string comment, string runKey, function<void(string str)> runFunction, string errorKey, function<void(string strs)> errorRunFunction);
	//获取当前书籍的订单情况
	static void httpGetBookIsBuy(int bookId, int memberId, int orderId, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	static void httpGetBookIsBuyCallBack(string json, function<void(string order, int types)> runable, function<void(string)> errorRun);
	//发表语音评论(带界面)
	static void goToSendRecording(int bookId, int memberId, int types, string membername, string orderid, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction, string closeKey, function<void(string)> closeFunction);
	//删除评论
	static void httpDeleteComment(int id, function<void(int commentID)> runable, function<void(string errorStr)> errorRun);
	//邀请注册送红包
	static void inviteRegister(int memberId, string url, string runKey, function<void(string)> runFunction, string errorKey, function<void(string)> errorRunFunction);
	//获取机型
	static string getPhoneModel(int um);
	static void httpBookCollect(int bookId, int type, function<void(string)> runFunction, function<void(string)> errorRunFunction);
	//***********************************************
	//***********************************************
	//***********************************************

	//书房 收藏/取消收藏	type 1.代表收藏 2.代表取消收藏

	//显示书房收藏和vip书籍
	static void httpBookCollectAndVipList( int type, function<void(string)> runFunction, function<void(string)> errorRunFunction);
};

#endif // __NETINTFACE_SCENE_H__