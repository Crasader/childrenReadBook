//
//  CocosAndroidJni.h
//  cocosAlipay
//
//
//

#ifndef __cocosAlipay__CocosAndroidJni__
#define __cocosAlipay__CocosAndroidJni__
#include "cocos2d.h"
#include <stdio.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#endif


USING_NS_CC;
using namespace std;

class CocosAndroidJni
{
public:    
    CocosAndroidJni();
    virtual ~CocosAndroidJni();

	
	// JNI 打开数据库
	//static bool openDataBase();

	// JNI 判断数据库是否正常,判断数据库的版本是否最新,否则重建数据库
	//static int examineDataBase(int dataBaseVersion);

	// JNI 	获取机型,android版本,sdk
	static void getPhoneInfo(long selectNum, std::string & info);

	// JNI 邀请注册送红包
	static void inviteRegister(long memberId, const char* url, const char* runKey, const char* errorKey);

	// JNI 发表录音评论
	static void commentTheRecording(long bookId, long memberId, long types, const char * membername, const char * orderid, const char* runKey, const char* errorKey, const char* closeKey);

	// JNI 获取毫秒时间
	static long long getMillsTime();

	//网络请求上传文件接口
	static void httpUpFile(const char * url, const char * json, const char * filepath, const char * runKey, const char * errorKey);

	// JNI 网络请求Post
	static void httpPost(const char * url, const char * json, const char * runKey, const char * errorKey);

	// JNI 网络请求GET
	static void httpGet(const char* url, const char* runKey, const char* errorKey);

	// JNI 充值订单
	static void httpGetRechargeOrderID(long memberid, long rechargeCount, long price100, const char* payType, const char* payInfo, const char* runKey, const char* errorKey);

	//JNI 包年订单
	static void httpGetVIPOrderID(long memberid, long rechargeCount, long price100, const char * payType, const char * payInfo, const char * runKey, const char * errorKey);

	// JNI 获取分享成功的红包
	static void httpShareWithCoupon(long memberID, const char* runKey, const char* errorKey);

	// JNI 获取书店当前页的书籍列表信息
	static void httpBookStoreSceneCurrentlyPageBookListInfo(long memberID, long BookStoreId, long pageIndex, const char* visitFrom, const char* runKey, const char* errorKey);

	// JNI 获取当前版本号
	static void getAppVersion(std::string& version);

	// JNI 获取用户书籍的已购列表
	static void httpGetUserBuyBooks(long memberID, const char* runKey, const char* errorKey);

	// JNI 获取用户安卓的账户余额
	static void httpGetUserBalance(long memberID, const char* runKey, const char* errorKey);

	// JNI 注册
	static void httpAccountRegiste(const char* memberName, const char* memberPasswd, const char* runKey, const char* errorKey);

	// JNI 修改用户信息
	static void httpAmendUserInfo(long memberId, long sex, const char * city, const char * province, const char * runKey, const char * errorKey);

	// JNI UTF-8转GBK
	static void UTF82GBK(const char* GBK, const char* UTF);

	// JNI GBK转UTF-8
	static void GBK2UTF8(const char* GBK, const char* UTF);

	// JNI 修改孩子信息
	static void httpAmendBabyInfo(long childrenId, const char* childrenName, long childrenSex, const char* childrenBirth, const char* runKey, const char* errorKey);

	// JNI 拍照,裁切成圆形图片保存到指定位置
	static void photographAlbumSelectImage(const char* name, const char* dir, const char* runKey, const char* errorKey);


	// JNI 打开相册选中图片
	static void openPhotoAlbumSelectImage(const char * name, const char * dir, long width, long height, const char * runKey, const char * errorKey);

	// JNI 上传picture文件夹里的图片
	static void UpImage(long childID, const char* ImageName, long memberId, const char* runKey, const char* errorKey);

	// JNI 图片切成圆形
	static void cutTheRounded(const char* path, const char* savePath, long width, long height, const char* runKey, const char* errorKey);

	// JNI 登陆
	static void httpLogIn(const char * account, const char * password, const char * runKey, const char * errorKey);

	// JNI 删除孩子
	static void httpDeleteChild(long memberId, long childrenId, const char * runKey, const char * errorKey);

	// JNI 添加孩子
	static void httpAddChild(long memberId, const char * childrenName, long childrenSex, const char * childrenBirth, const char * runKey, const char * errorKey);

	// JNI 下载结束发通知回调
	static void DownLoadImage(const char * url, const char * dir, const char * fileName, const char * runKey, const char * errorKey);

	// JNI 获取孩子列表详情
	static void httpGetChildDetails(long memberId, const char * runKey, const char * errorKey);

	// JNI 下载用户第一个孩子的头像
	static void httpGetFirstChildHeadPortrait(long memberID, const char* runKey, const char* errorKey);

	// JNI 获取书籍详情
	static void httpGetBookInfo(long bookID, const char* runKey, const char* errorKey);

	// JNI 分享
	//static void Share(const char* filePath, const char* bookName, const char* targetUrl, const char* headUrl, const char* title);
	static void Share(const char* filePath, const char* bookName, const char* targetUrl, const char* headUrl, const char* title, const char* runKey, const char* errorKey);

	// JNI 使用红包购书
	static void httpBuyBookWithRedPacket(long memberId, long redPacketID, long bookID, const char* runKey, const char* errorKey);

	//JNI 获取用户红包列表
	static void httpGetUserRedPackets(long memberId, const char* runKey, const char* errorKey);
	/*--------------------------以上采用跨平台消息循环机制------------------------------*/
	// JNI 上传下载记录
	static void AddDownLoadRecord(long memberId, long bookId);

	//JNI 发表评论
	static void Comment(long memberId, long bookId, const char * memberName, const char * comment, const char * title, long score);
	
	// JNI 注册
	static void AccountRegiste(const char * memberName, const char * memberPasswd);

	// JNI 余额购书
	static void RechargeBuyBook(const char * memberid, long bookid, long bookPrice);

	// JNI 充值订单
	//static void GetRechargeOrderID(const char* memberid, long rechargeCount, long price100, const char*  payType, const char*  payInfo);

	// JNI 查询余额
	static void GetUserBalance(const char* memberId, const char* resource);

    //JNI 检查数据库损坏,进行修复
    //static bool SQLiteIsException();

	// JNI 资源图片保存到相册
	static void saveQRcode();

	// JNI 上传picture文件夹里的图片
	//static long UpImage(long childID, const char * ImageName, long memberId);

	//JNI 设置memberId
	static void setMemberId(long memberId);

	// JNI 跳转书籍详情activity
	static void GotoBookInfoActivity(long bookid);

	// JNI 请求书籍详情信息
	//static long GetBookInfo(long bookid);

	//JNI 书店请求当前页列表
	//static long GetBookStorePageInfo(long BookStoreId, long pageIndex, const char * visitFrom);

	//JNI 网络请求书城的按钮列表	bookcity场景
	//static long GetBookCityButtonsInfo4BookCity();

	//JNI 网络请求书城的按钮列表
	//static long GetBookCityButtonsInfo();

	//JNI 网络获取已购书籍列表
	//static long GetBuyBook(long memberId);

	//JNI 打印
	static void Log(const char* tag, const char* msg);

	//JNI 获取孩子列表信息
	//static long getChildDetails(long memberId);

    //JNI 队列控制//tag = -1 取消全部请求重新启动队列
	static void stopRequestByTag(int tag);

	//JNI 账号登陆
	//static long AccountLogin(const char* account,const char* password);

	//JNI 账号信息读入数据库
	static void AccountWrite(long long member_id,const char* member_name,const char* member_passwd,
			long long member_sex, const char* memberProvince, const char* memberCity, long long upTime);

	//JNI 账号信息读出数据库
	static void AccountRead(std::string& ret);

	//JNI 程序退出
	static void AppExit();

	//JNI Toast提示信息
	static void ToastAndroid(const char* str);

	//JNI 设置m_result支付结果
	static void setResult(const char* str);
	
	//JNI 获取m_result查询支付结果
	static void getResult(std::string & ret);

	//JNI 拍完照片根据路径获取头像
	static void CompressPictureByTakePhoto(const char * portartName);

	//选头像
	//static void SelectPortart(const char* portartName);

	//拍照
	static void TakePhotos(const char* photoName);

	//是否联网
	static bool IsNetConnect(bool hint, const char* runKey);
	//static bool IsNetConnect();
    
	//是否有wifi
    static bool IsThereWifi();

    //支付方式 alipay ,签约合作者身份ID,签约卖家支付宝账号,RSA私钥psk8,订单ID 商品标题 商品描述 商品价格
    static void CocosAndroidJniAliPay(std::string& ret ,const char* payMode,const char* PARTNER,const char* SELLER,const char* RSA_PRIVATE,const char * orderId, const char * bookName, const char * bookIntroduction, const char * bookPrice);

	//获取照片存放路径
	static void GetPhotoPath(std::string& ret);

	//获取下载的目录
	static void GetDownloadPath(std::string & ret);

	//安装apk
	static void InstallApk(const char* path);

	//跳转浏览器
	static void GotoUrl(const char* url);

	//网络请求-记录阅读记录
	static void saveReadRecord(long memberId, long childrenId, long bookId, const char* readStartTime, const char* readEndTime,const char* runKey, const char* errorKey);
};

#endif /* defined(__cocosAlipay__CocosAndroidJni__) */
