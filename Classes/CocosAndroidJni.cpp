#include "CocosAndroidJni.h"
#include "App.h"

CocosAndroidJni::CocosAndroidJni(){}
CocosAndroidJni::~CocosAndroidJni(){}



//// JNI 打开数据库
//bool CocosAndroidJni::openDataBase( )
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	//原型函数public static boolean openDataBase()
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "openDataBase", "()Z");
//	if (!isHave)
//	{
//		App::log("jni:openDataBase did not exist");
//		return false;
//	}
//	else
//	{
//		App::log("jni:openDataBase exist");
//		jboolean result = function.env->CallStaticBooleanMethod(function.classID, function.methodID);
//		function.env->DeleteLocalRef(function.classID);
//		return result;
//	}
//#endif
//}
//
//// JNI 判断数据库是否正常,判断数据库的版本是否最新,否则重建数据库
//int CocosAndroidJni::examineDataBase(int dataBaseVersion)
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	//原型函数public static int examineDataBase(int dataBaseVersion)
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "examineDataBase", "(I)I");
//	if (!isHave)
//	{
//		App::log("jni:examineDataBase did not exist");
//		return 999;
//	}
//	else
//	{
//		App::log("jni:examineDataBase exist");
//		jint _dataBaseVersion = dataBaseVersion;
//		jint result = function.env->CallStaticIntMethod(function.classID, function.methodID, _dataBaseVersion);
//		function.env->DeleteLocalRef(function.classID);
//		return result;
//	}
//#endif
//}


// JNI 邀请注册送红包
void CocosAndroidJni::inviteRegister(long memberId, const char*  url, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void inviteRegister(int memberid, String url, final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "inviteRegister", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:inviteRegister did not exist");
	}
	else
	{
		//App::log("jni:inviteRegister exist");
		jint _memberid = memberId;
		jstring _url = function.env->NewStringUTF(url);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID,  _memberid, _url,_runKey, _errorKey);
		function.env->DeleteLocalRef(function.classID);
		function.env->DeleteLocalRef(_url);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
	}
#endif
}

// JNI 发表录音评论
void CocosAndroidJni::commentTheRecording(long bookId, long memberId, long types, const char * membername, const char * orderid, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数	public static void commentTheRecording(int bookId, int memberId, int types,	String membername, String orderid, String runkey, String errorkey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "commentTheRecording", "(IIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:commentTheRecording did not exist");
	}
	else
	{
		//App::log("jni:commentTheRecording exist");
		jint _bookId = bookId;
		jint _memberid = memberId;
		jint _types = types;
		jstring _membername = function.env->NewStringUTF(membername);
		jstring _orderid = function.env->NewStringUTF(orderid);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _bookId, _memberid, _types, _membername, _orderid, _runKey, _errorKey);
		function.env->DeleteLocalRef(function.classID);
		function.env->DeleteLocalRef(_membername);
		function.env->DeleteLocalRef(_orderid);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
	}
#endif
}

// JNI 获取毫秒时间
long long CocosAndroidJni::getMillsTime()
{
	long long result = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数	public static long getMillsTime()
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "getMillsTime", "()J");
	if (!isHave)
	{
		App::log("jni:getMillsTime did not exist");
	}
	else
	{
		//App::log("jni:getMillsTime exist");
		jlong time = function.env->CallStaticLongMethod(function.classID, function.methodID);
		result = time;
		function.env->DeleteLocalRef(function.classID);
	}
#endif
	return result;
}

// JNI 网络请求少量参数的Post
void CocosAndroidJni::httpPost(const char* url, const char* json, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpPost(String url, String jsonData,final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpPost", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpPost did not exist");
	}
	else
	{
		App::log("jni:httpPost exist");
		jstring _url = function.env->NewStringUTF(url);
		jstring _json = function.env->NewStringUTF(json);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _url, _json, _runKey, _errorKey);
		function.env->DeleteLocalRef(_url);
		function.env->DeleteLocalRef(_json);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 网络请求GET
void CocosAndroidJni::httpGet(const char* url, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpGet(String url, final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpGet", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpGet did not exist");
	}
	else
	{
		App::log("jni:httpGet exist");
		jstring _url = function.env->NewStringUTF(url);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _url, _runKey, _errorKey);
		function.env->DeleteLocalRef(_url);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 充值订单
void CocosAndroidJni::httpGetRechargeOrderID(long memberid, long rechargeCount, long price100, const char*  payType, const char*  payInfo, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpGetRechargeOrderID(int memberid, int rechargeCount,
	//int price100, String payType, String payInfo, final String runKey,
		//final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpGetRechargeOrderID", "(IIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpGetRechargeOrderID did not exist");
	}
	else
	{
		App::log("jni:httpGetRechargeOrderID exist");
		jint _memberid = memberid;
		jint _rechargeCount = rechargeCount;
		jint _price100 = price100;
		jstring _payType = function.env->NewStringUTF(payType);
		jstring _payInfo = function.env->NewStringUTF(payInfo);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberid, _rechargeCount, _price100, _payType, _payInfo, _runKey, _errorKey);
		function.env->DeleteLocalRef(_payType);
		function.env->DeleteLocalRef(_payInfo);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
}
#endif
}

// JNI 获取分享成功的红包
void CocosAndroidJni::httpShareWithCoupon(long memberID, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpShareWithCoupon(int memberID, String runKey, String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpShareWithCoupon", "(ILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpShareWithCoupon did not exist");
	}
	else
	{
		App::log("jni:httpShareWithCoupon exist");
		jint _memberID = memberID;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberID, _runKey, _errorKey);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 获取书店当前页的书籍列表信息
void CocosAndroidJni::httpBookStoreSceneCurrentlyPageBookListInfo(long memberID, long BookStoreId, long pageIndex, const char* visitFrom, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpBookStoreSceneCurrentlyPageBookListInfo(int memberID, int BookStoreId, int pageIndex, String visitFrom,	String runKey, String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpBookStoreSceneCurrentlyPageBookListInfo", "(IIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpBookStoreSceneCurrentlyPageBookListInfo did not exist");
	}
	else
	{
		App::log("jni:httpBookStoreSceneCurrentlyPageBookListInfo exist");
		jint _memberID = memberID;
		jint _BookStoreId = BookStoreId;
		jint _pageIndex = pageIndex;
		jstring _visitFrom = function.env->NewStringUTF(visitFrom);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberID, _BookStoreId, _pageIndex, _visitFrom, _runKey, _errorKey);
		function.env->DeleteLocalRef(_visitFrom);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 获取当前版本号
void CocosAndroidJni::getAppVersion(std::string& version)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static String getAppVersion()
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "getAppVersion", "()Ljava/lang/String;");
	if (!isHave)
	{
		App::log("jni:getAppVersion did not exist");
	}
	else
	{
		App::log("jni:getAppVersion exist");
		jstring jret = (jstring)function.env->CallStaticObjectMethod(function.classID, function.methodID);
		version = JniHelper::jstring2string(jret);
		function.env->DeleteLocalRef(jret);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 获取用户书籍的已购列表
void CocosAndroidJni::httpGetUserBuyBooks(long memberID, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpGetUserBuyBooks(int memberID, String runKey, String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpGetUserBuyBooks", "(ILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpGetUserBuyBooks did not exist");
	}
	else
	{
		App::log("jni:httpGetUserBuyBooks exist");
		jint _memberID = memberID;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberID, _runKey, _errorKey);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 获取用户安卓的账户余额
void CocosAndroidJni::httpGetUserBalance(long memberID, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpGetUserBalance(int memberID, String runKey, String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpGetUserBalance", "(ILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpGetUserBalance did not exist");
	}
	else
	{
		App::log("jni:httpGetUserBalance exist");
		jint _memberID = memberID;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberID,  _runKey, _errorKey);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 注册
void CocosAndroidJni::httpAccountRegiste( const char* memberName, const char* memberPasswd, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpAccountRegiste(String memberName, String memberPasswd, String runKey, String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpAccountRegiste", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpAccountRegiste did not exist");
	}
	else
	{
		App::log("jni:httpAccountRegiste exist");
		jstring _memberName = function.env->NewStringUTF(memberName);
		jstring _memberPasswd = function.env->NewStringUTF(memberPasswd);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberName, _memberPasswd, _runKey, _errorKey);
		function.env->DeleteLocalRef(_memberName);
		function.env->DeleteLocalRef(_memberPasswd);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 修改用户信息
void CocosAndroidJni::httpAmendUserInfo(long memberId, long sex, const char* city, const char* province, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpAmendUserInfo(int memberId, int sex, String city,String province, String runKey, String errorKey) 
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpAmendUserInfo", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpAmendUserInfo did not exist");
	}
	else
	{
		App::log("jni:httpAmendUserInfo exist");
		jint _memberId = memberId;
		jint _sex = sex;
		jstring _city = function.env->NewStringUTF(city);
		jstring _province = function.env->NewStringUTF(province);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberId, _sex, _city, _province, _runKey, _errorKey);
		function.env->DeleteLocalRef(_city);
		function.env->DeleteLocalRef(_province);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI UTF-8转GBK
void CocosAndroidJni::UTF82GBK(const char* GBK, const char* UTF)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void UTF82GBK(String GBK, String UTF)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "UTF82GBK", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:UTF82GBK did not exist");
	}
	else
	{
		App::log("jni:UTF82GBK exist");
		jstring _GBK = function.env->NewStringUTF(GBK);
		jstring _UTF = function.env->NewStringUTF(UTF);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _GBK, _UTF);
		function.env->DeleteLocalRef(_GBK);
		function.env->DeleteLocalRef(_UTF);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI GBK转UTF-8
void CocosAndroidJni::GBK2UTF8(const char* GBK, const char* UTF)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void GBK2UTF8(String GBK, String UTF)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GBK2UTF8", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:GBK2UTF8 did not exist");
	}
	else
	{
		App::log("jni:GBK2UTF8 exist");
		jstring _GBK = function.env->NewStringUTF(GBK);
		jstring _UTF = function.env->NewStringUTF(UTF);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _GBK, _UTF);
		function.env->DeleteLocalRef(_GBK);
		function.env->DeleteLocalRef(_UTF);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 修改孩子信息
void CocosAndroidJni::httpAmendBabyInfo(long childrenId, const char* childrenName, long childrenSex,  const char* childrenBirth, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpAmendBabyInfo(int childrenId, String childrenName, int childrenSex,String childrenBirth, final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpAmendBabyInfo", "(ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpAmendBabyInfo did not exist");
	}
	else
	{
		App::log("jni:httpAmendBabyInfo exist");
		jint _childrenId = childrenId;
		jstring _childrenName = function.env->NewStringUTF(childrenName);
		jint _childrenSex = childrenSex;
		jstring _childrenBirth = function.env->NewStringUTF(childrenBirth);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _childrenId, _childrenName, _childrenSex, _childrenBirth, _runKey, _errorKey);
		function.env->DeleteLocalRef(_childrenName);
		function.env->DeleteLocalRef(_childrenBirth);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 拍照,图片保存到指定位置
void CocosAndroidJni::photographAlbumSelectImage(const char* name, const char* dir, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void photographAlbumSelectImage(String name, String dir, String runKey, String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "photographAlbumSelectImage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:photographAlbumSelectImage did not exist");
	}
	else
	{
		App::log("jni:photographAlbumSelectImage exist");
		jstring _name = function.env->NewStringUTF(name);
		jstring _dir = function.env->NewStringUTF(dir);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _name, _dir, _runKey, _errorKey);
		function.env->DeleteLocalRef(_name);
		function.env->DeleteLocalRef(_dir);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 打开相册选中图片,图片保存到指定位置
void CocosAndroidJni::openPhotoAlbumSelectImage(const char* name, const char* dir, long width, long height, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void openPhotoAlbumSelectImage(String name, String dir,int width, int height, String runKey, String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "openPhotoAlbumSelectImage", "(Ljava/lang/String;Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:openPhotoAlbumSelectImage did not exist");
	}
	else
	{
		App::log("jni:openPhotoAlbumSelectImage exist");
		jstring _name = function.env->NewStringUTF(name);
		jstring _dir = function.env->NewStringUTF(dir); 
		jint _width = width;
		jint _height = height;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _name, _dir, _width, _height, _runKey, _errorKey);
		function.env->DeleteLocalRef(_name);
		function.env->DeleteLocalRef(_dir);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 上传picture文件夹里的图片
void CocosAndroidJni::UpImage(long childID, const char* ImageFullPath, long memberId, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void UpImage(int childID, String ImageFullPath, final int memberId, String runKey, String errorRunKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "UpImage", "(ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:UpImage did not exist");
	}
	else
	{
		App::log("jni:UpImage exist");
		jint _childID = childID;
		jstring _ImageFullPath = function.env->NewStringUTF(ImageFullPath);
		jint _memberId = memberId;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _childID, _ImageFullPath, _memberId, _runKey, _errorKey);
		function.env->DeleteLocalRef(_ImageFullPath);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 图片切成圆形
void CocosAndroidJni::cutTheRounded(const char* path, const char* savePath, long width, long height, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void cutTheRounded(String path, String savePath, int width, int height, String runKey, String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "cutTheRounded", "(Ljava/lang/String;Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:cutTheRounded did not exist");
	}
	else
	{
		App::log("jni:cutTheRounded exist");
		jstring _path = function.env->NewStringUTF(path);
		jstring _savePath = function.env->NewStringUTF(savePath);
		jint _width = width;
		jint _height = height;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _path, _savePath, _width, _height, _runKey, _errorKey);
		function.env->DeleteLocalRef(_path);
		function.env->DeleteLocalRef(_savePath);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 登陆
void CocosAndroidJni::httpLogIn(const char* account, const char* password, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpLogIn(String account, String password, final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpLogIn", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpLogIn did not exist");
	}
	else
	{
		App::log("jni:httpLogIn exist");
		jstring _account = function.env->NewStringUTF(account);
		jstring _password = function.env->NewStringUTF(password);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _account, _password,  _runKey, _errorKey);
		function.env->DeleteLocalRef(_account);
		function.env->DeleteLocalRef(_password);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 删除孩子
void CocosAndroidJni::httpDeleteChild(long memberId, long childrenId, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpDeleteChild(int memberId, int childrenId, final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpDeleteChild", "(IILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpDeleteChild did not exist");
	}
	else
	{
		App::log("jni:httpDeleteChild exist");
		jint _memberId = memberId;
		jint _childrenId = childrenId;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberId, _childrenId, _runKey, _errorKey);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 添加孩子
void CocosAndroidJni::httpAddChild(long memberId, const char* childrenName, long childrenSex, const char* childrenBirth, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpAddChild(int memberId, String childrenName, int childrenSex, String childrenBirth, final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpAddChild", "(ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpAddChild did not exist");
	}
	else
	{
		App::log("jni:httpAddChild exist");
		jint _memberId = memberId;
		jstring _childrenName = function.env->NewStringUTF(childrenName);
		jint _childrenSex = childrenSex;
		jstring _childrenBirth = function.env->NewStringUTF(childrenBirth);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberId, _childrenName, _childrenSex, _childrenBirth, _runKey, _errorKey);
		function.env->DeleteLocalRef(_childrenName);
		function.env->DeleteLocalRef(_childrenBirth);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 下载结束发通知回调
void CocosAndroidJni::DownLoadImage(const char* url, const char* dir, const char* fileName, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void DownLoadImage(String url, String dir, String fileName, final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "DownLoadImage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:DownLoadImage did not exist");
	}
	else
	{
		App::log("jni:DownLoadImage exist");
		jstring _url = function.env->NewStringUTF(url);
		jstring _dir = function.env->NewStringUTF(dir);
		jstring _fileName = function.env->NewStringUTF(fileName);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _url, _dir, _fileName, _runKey, _errorKey);
		function.env->DeleteLocalRef(_url);
		function.env->DeleteLocalRef(_dir);
		function.env->DeleteLocalRef(_fileName);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 获取孩子列表详情
void CocosAndroidJni::httpGetChildDetails(long memberId, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpGetChildDetails(int memberId, final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpGetChildDetails", "(ILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpGetChildDetails did not exist");
	}
	else
	{
		App::log("jni:httpGetChildDetails exist");
		jint _memberId = memberId;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberId, _runKey, _errorKey);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 下载用户第一个孩子的头像
void CocosAndroidJni::httpGetFirstChildHeadPortrait(long memberID,  const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpGetFirstChildHeadPortrait( int memberID,  String runKey,  String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpGetFirstChildHeadPortrait", "(ILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpGetFirstChildHeadPortrait did not exist");
	}
	else
	{
		App::log("jni:httpGetFirstChildHeadPortrait exist");
		jint _memberID = memberID;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberID, _runKey, _errorKey);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 获取书籍详情
void CocosAndroidJni::httpGetBookInfo(long bookID, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpGetBookInfo(int bookID, final String runKey,final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpGetBookInfo", "(ILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpGetBookInfo did not exist");
	}
	else
	{
		App::log("jni:httpGetBookInfo exist");
		jint _bookID = bookID;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID,  _bookID, _runKey, _errorKey);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 分享
void CocosAndroidJni::Share(const char* filePath, const char* bookName, const char* targetUrl, const char* headUrl, const char* title, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void Share(String filePath, String bookName,final String targetUrl, String headUrl, final String title, final String runKey, final String errorKey);
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "Share", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:Share did not exist");
	}
	else
	{
		App::log("jni:Share exist");
		jstring _filePath = function.env->NewStringUTF(filePath);
		jstring _bookName = function.env->NewStringUTF(bookName);
		jstring _targetUrl = function.env->NewStringUTF(targetUrl);
		jstring _headUrl = function.env->NewStringUTF(headUrl);
		jstring _title = function.env->NewStringUTF(title);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _filePath, _bookName, _targetUrl, _headUrl, _title, _runKey, _errorKey);
		function.env->DeleteLocalRef(_filePath);
		function.env->DeleteLocalRef(_bookName);
		function.env->DeleteLocalRef(_targetUrl);
		function.env->DeleteLocalRef(_headUrl);
		function.env->DeleteLocalRef(_title);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 使用红包购书
void CocosAndroidJni::httpBuyBookWithRedPacket(long memberId, long redPacketID, long bookID, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpBuyBookWithRedPacket(int memberId, int redPacketID,int bookID, final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpBuyBookWithRedPacket", "(IIILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpBuyBookWithRedPacket did not exist");
	}
	else
	{
		App::log("jni:httpBuyBookWithRedPacket exist");
		jint _memberId = memberId;
		jint _redPacketID = redPacketID;
		jint _bookID = bookID;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberId, _redPacketID, _bookID, _runKey, _errorKey);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}


//JNI 获取用户红包列表
void CocosAndroidJni::httpGetUserRedPackets(long memberId, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpGetUserRedPackets(int memberId,final String runKey, final String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpGetUserRedPackets", "(ILjava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpGetUserRedPackets did not exist");
	}
	else
	{
		App::log("jni:httpGetUserRedPackets exist");
		jint _memberId = memberId;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberId, _runKey, _errorKey);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 上传下载记录
void CocosAndroidJni::AddDownLoadRecord(long memberId, long bookId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void AddDownLoadRecord(int memberId, int bookId)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "AddDownLoadRecord", "(II)V");
	if (!isHave)
	{
		App::log("jni:AddDownLoadRecord did not exist");
	}
	else
	{
		App::log("jni:AddDownLoadRecord exist");
		jint _memberId = memberId;
		jint _bookId = bookId;
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberId, _bookId);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 发表评论
void CocosAndroidJni::Comment(long memberId, long bookId,const char* memberName, const char* comment, const char* title,  long score)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void Comment(final int memberId,final int bookId,final String memberName,final String comment,final String title,final int score)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "Comment", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
	if (!isHave)
	{
		App::log("jni:Comment did not exist");
	}
	else
	{
		App::log("jni:Comment exist");
		jint _memberId = memberId;
		jint _bookId = bookId;
		jstring _memberName = function.env->NewStringUTF(memberName);
		jstring _comment = function.env->NewStringUTF(comment);
		jstring _title = function.env->NewStringUTF(title);
		jint _score = score;
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberId, _bookId, _memberName, _comment, _title, _score);
		function.env->DeleteLocalRef(_memberName);
		function.env->DeleteLocalRef(_comment);
		function.env->DeleteLocalRef(_title);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 注册
void CocosAndroidJni::AccountRegiste(const char* memberName, const char* memberPasswd)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void AccountRegiste(String memberName, String memberPasswd)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "AccountRegiste", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:AccountRegiste did not exist");
	}
	else
	{
		App::log("jni:AccountRegiste exist");
		jstring _memberName = function.env->NewStringUTF(memberName);
		jstring _memberPasswd = function.env->NewStringUTF(memberPasswd);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberName, _memberPasswd);
		function.env->DeleteLocalRef(_memberName);
		function.env->DeleteLocalRef(_memberPasswd);
		function.env->DeleteLocalRef(function.classID);
}
#endif
}

// JNI 余额购书
void CocosAndroidJni::RechargeBuyBook(const char* memberid, long bookid, long bookPrice)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void RechargeBuyBook(String memberid, int bookid)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "RechargeBuyBook", "(Ljava/lang/String;II)V");
	if (!isHave)
	{
		App::log("jni:RechargeBuyBook did not exist");
	}
	else
	{
		App::log("jni:RechargeBuyBook exist");
		jstring _memberid = function.env->NewStringUTF(memberid);
		jint _bookid = bookid;
		jint _bookPrice = bookPrice;
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberid, _bookid, _bookPrice);
		function.env->DeleteLocalRef(_memberid);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 查询余额
void CocosAndroidJni::GetUserBalance(const char* memberId, const char* resource)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void GetUserBalance(String memberId, String resource)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GetUserBalance", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:GetUserBalance did not exist");
	}
	else
	{
		App::log("jni:GetUserBalance exist");
		jstring _memberId = function.env->NewStringUTF(memberId);
		jstring _resource = function.env->NewStringUTF(resource);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberId, _resource);
		function.env->DeleteLocalRef(_memberId);
		function.env->DeleteLocalRef(_resource);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}


//// JNI 判断数据库是否异常,并且重建数据库
//bool CocosAndroidJni::SQLiteIsException()
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	//原型函数public static boolean SQLiteIsException()
//	bool re = true;
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "SQLiteIsException", "()Z");
//	if (!isHave)
//	{
//		App::log("jni:SQLiteIsException did not exist");
//	}
//	else
//	{
//		App::log("jni:SQLiteIsException exist");
//		jboolean b = function.env->CallStaticBooleanMethod(function.classID, function.methodID);
//		re = b;
//		function.env->DeleteLocalRef(function.classID);
//	}
//	return re;
//#endif
//}



// JNI 资源图片保存到相册
void CocosAndroidJni::saveQRcode()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void saveQRcode()
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "saveQRcode", "()V");
	if (!isHave)
	{
		App::log("jni:saveQRcode did not exist");
	}
	else
	{
		App::log("jni:saveQRcode exist");
		function.env->CallStaticVoidMethod(function.classID, function.methodID);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

/*// JNI 上传picture文件夹里的图片
long CocosAndroidJni::UpImage(long childID,  const char* ImageName, long memberId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	long _result = -999;
	//原型函数public static int UpImage(int childID, String ImageName,final int memberId)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "UpImage", "(ILjava/lang/String;I)I");
	if (!isHave)
	{
		App::log("jni:UpImage did not exist");
	}
	else
	{
		App::log("jni:UpImage exist");
		jint childID2 = childID;
		jstring ImageName2 = function.env->NewStringUTF(ImageName);
		jint memberId2 = memberId;
		jint result = function.env->CallStaticIntMethod(function.classID, function.methodID, childID2, ImageName2, memberId2);
		_result = result;
		function.env->DeleteLocalRef(ImageName2);
		function.env->DeleteLocalRef(function.classID);
}
	return _result;
#endif
}*/

//JNI 设置memberId
void CocosAndroidJni::setMemberId(long memberId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void setMemberId(int memberId)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "setMemberId", "(I)V");
	if (!isHave)
	{
		App::log("jni:setMemberId did not exist");
	}
	else
	{
		App::log("jni:setMemberId exist");
		jint _bookid = memberId;
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _bookid);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 跳转书籍详情activity
void CocosAndroidJni::GotoBookInfoActivity(long bookId)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void GotoBookInfoActivity(int bookId)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GotoBookInfoActivity", "(I)V");
	if (!isHave)
	{
		App::log("jni:GotoBookInfoActivity did not exist");
	}
	else
	{
		App::log("jni:GotoBookInfoActivity exist");
		jint _bookid = bookId;
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _bookid);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 请求书籍详情信息
//long CocosAndroidJni::GetBookInfo(long bookid)
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	long _result = -999;
//	//原型函数public static int GetBookInfo(int bookid)
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GetBookInfo", "(I)I");
//	if (!isHave)
//	{
//		App::log("jni:GetBookInfo did not exist");
//	}
//	else
//	{
//		App::log("jni:GetBookInfo exist");
//		jint _bookid = bookid;
//		jint result = function.env->CallStaticIntMethod(function.classID, function.methodID, _bookid);
//		_result = result;
//		function.env->DeleteLocalRef(function.classID);
//	}
//	return _result;
//#endif
//}

////JNI 书店请求当前页列表
//long CocosAndroidJni::GetBookStorePageInfo(long BookStoreId, long pageIndex, const char* visitFrom)
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	long _result = -999;
//	//原型函数public static int GetBookStorePageInfo(int BookStoreId, int pageIndex, String visitFrom)
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GetBookStorePageInfo", "(IILjava/lang/String;)I");
//	if (!isHave)
//	{
//		App::log("jni:GetBookStorePageInfo did not exist");
//	}
//	else
//	{
//		App::log("jni:GetBookStorePageInfo exist");
//		jint _BookStoreId = BookStoreId;
//		jint _pageIndex = pageIndex;
//		jstring _visitFrom = function.env->NewStringUTF(visitFrom);
//		jint result = function.env->CallStaticIntMethod(function.classID, function.methodID, _BookStoreId, _pageIndex, _visitFrom);
//		_result = result;
//		function.env->DeleteLocalRef(_visitFrom);
//		function.env->DeleteLocalRef(function.classID);
//	}
//	return _result;
//#endif
//}

////JNI 网络请求书城的按钮列表	bookcity场景
//long CocosAndroidJni::GetBookCityButtonsInfo4BookCity()
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	long _result = -999;
//	//原型函数public static int GetBookCityButtonsInfo4BookCity()
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GetBookCityButtonsInfo4BookCity", "()I");
//	if (!isHave)
//	{
//		App::log("jni:GetBookCityButtonsInfo4BookCity did not exist");
//	}
//	else
//	{
//		App::log("jni:GetBookCityButtonsInfo4BookCity exist");
//		jint result = function.env->CallStaticIntMethod(function.classID, function.methodID);
//		_result = result;	
//		function.env->DeleteLocalRef(function.classID);
//	}
//	return _result;
//#endif
//}
//
////JNI 网络请求书城的按钮列表
//long CocosAndroidJni::GetBookCityButtonsInfo()
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	//原型函数public static int GetBookCityButtonsInfo()
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GetBookCityButtonsInfo", "()I");
//	if (!isHave)
//	{
//		App::log("jni:GetBookCityButtonsInfo did not exist");
//	}
//	else
//	{
//		App::log("jni:GetBookCityButtonsInfo exist");
//		jint result = function.env->CallStaticIntMethod(function.classID, function.methodID);
//		long _result = result;
//		function.env->DeleteLocalRef(function.classID);
//		return _result;
//	}
//#endif
//}
//
////JNI 网络获取已购书籍列表
//long CocosAndroidJni::GetBuyBook(long memberId)
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	//原型函数public static int GetBuyBook(int memberId)
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GetBuyBook", "(I)I");
//	if (!isHave)
//	{
//		App::log("jni:GetBuyBook did not exist");
//	}
//	else
//	{
//		App::log("jni:GetBuyBook exist");
//		jint _memberId = memberId;
//		jint result = function.env->CallStaticIntMethod(function.classID, function.methodID, _memberId);
//		long _result = result;
//		function.env->DeleteLocalRef(function.classID);
//		return _result;
//	}
//#endif
//}

//JNI 打印
void CocosAndroidJni::Log(const char* tag, const char* msg)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void Log(String tag,String msg)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "Log", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (isHave)
	{
		jstring _account = function.env->NewStringUTF(tag);
		jstring _password = function.env->NewStringUTF(msg);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _account, _password);
		function.env->DeleteLocalRef(_account);
		function.env->DeleteLocalRef(_password);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

////JNI 获取孩子列表信息
//long CocosAndroidJni::getChildDetails(long memberId)
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	//原型函数public static int getChildDetails(int memberId)
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "getChildDetails", "(I)I");
//	if (!isHave)
//	{
//		App::log("jni:getChildDetails did not exist");
//	}
//	else
//	{
//		App::log("jni:getChildDetails exist");
//		jint _memberId = memberId;
//		jint result = function.env->CallStaticIntMethod(function.classID, function.methodID, _memberId);
//		long _result = result;
//		function.env->DeleteLocalRef(function.classID);
//		return _result;
//}
//#endif
//}

//JNI 队列控制
void CocosAndroidJni::stopRequestByTag(int tag)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void stopRequestByTag(int tag)//-1取消全部请求,-2开始全部请求
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "stopRequestByTag", "(I)V");
	if (!isHave)
	{
		App::log("jni:stopRequestByTag did not exist");
	}
	else
	{
		App::log("jni:stopRequestByTag exist");
		jint _tag = tag;
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _tag);
	}
#endif
}

//账号登陆
//long CocosAndroidJni::AccountLogin(const char* account,const char* password)
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
//	JniMethodInfo function;
//	//原型函数public static int AccountLogin(String account, String password)
//	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "AccountLogin", "(Ljava/lang/String;Ljava/lang/String;)I");
//	if (!isHave)
//	{
//		App::log("jni:AccountLogin did not exist");
//	}
//	else
//	{
//		App::log("jni:AccountLogin exist");
//		jstring _account = function.env->NewStringUTF(account);
//		jstring _password = function.env->NewStringUTF(password);
//		jint result = function.env->CallStaticIntMethod(function.classID, function.methodID, _account,_password);
//		long _result = result;
//		function.env->DeleteLocalRef(_account);
//		function.env->DeleteLocalRef(_password);
//		function.env->DeleteLocalRef(function.classID);
//		return _result;
//	}
//#endif
//}

//JNI 账号信息读入数据库
void CocosAndroidJni::AccountWrite(long long member_id,const char* member_name,const char* member_passwd,
		long long member_sex, const char* memberProvince, const char* memberCity, long long upTime)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void AccountWrite(long member_id, String member_name, String member_passwd,
//	long member_sex, String memberProvince, String memberCity, long upTime)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "AccountWrite",
			"(JLjava/lang/String;Ljava/lang/String;JLjava/lang/String;Ljava/lang/String;J)V");
	if (!isHave)
	{
		App::log("jni:AccountWrite did not exist");
	}
	else
	{
		App::log("jni:AccountWrite exist");
		jlong _member_id = member_id;
		jstring _member_name = function.env->NewStringUTF(member_name);
		jstring _member_passwd = function.env->NewStringUTF(member_passwd);
		jlong _member_sex = member_sex;
		jstring _memberProvince = function.env->NewStringUTF(memberProvince);
		jstring _memberCity = function.env->NewStringUTF(memberCity);
		jlong _upTime = upTime;
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _member_id,_member_name,
				_member_passwd,_member_sex,_memberProvince,_memberCity,_upTime);
		function.env->DeleteLocalRef(_member_name);
		function.env->DeleteLocalRef(_member_passwd);
		function.env->DeleteLocalRef(_memberProvince);
		function.env->DeleteLocalRef(_memberCity);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

//JNI 账号信息读出数据库
void CocosAndroidJni::AccountRead(std::string& ret)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static String AccountRead()
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "AccountRead",
			"()Ljava/lang/String;");
	if (!isHave)
	{
		App::log("jni:AccountRead did not exist");
	}
	else
	{
		App::log("jni:AccountRead exist");
		jstring jret =(jstring)function.env->CallStaticObjectMethod(function.classID, function.methodID);
		ret = JniHelper::jstring2string(jret);
		function.env->DeleteLocalRef(jret);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

//JNI 程序退出
void CocosAndroidJni::AppExit()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void exit()
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "exit", "()V");
	if (!isHave)
	{
		App::log("jni:exit did not exist");
	}
	else
	{
		App::log("jni:exit exist");
		function.env->CallStaticVoidMethod(function.classID, function.methodID);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

//JNI Toast提示信息
void CocosAndroidJni::ToastAndroid(const char* str)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void ToastAndroid(String context)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "ToastAndroid", "(Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:ToastAndroid did not exist");
	}
	else
	{
		App::log("jni:ToastAndroid exist");
		jstring _portartName = function.env->NewStringUTF(str);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _portartName);
		function.env->DeleteLocalRef(_portartName);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

//JNI 设置m_result支付结果
void CocosAndroidJni::setResult(const char* str)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void setM_result(String m_result)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "setM_result", "(Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:setM_result did not exist");
	}
	else
	{
		App::log("jni:setM_result exist");
		jstring _portartName = function.env->NewStringUTF(str);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _portartName);
		function.env->DeleteLocalRef(_portartName);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

//JNI 获取m_result查询支付结果
void CocosAndroidJni::getResult(std::string& ret)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static String getM_result()
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "getM_result", "()Ljava/lang/String;");
	if (!isHave)
	{
		App::log("jni:getM_result did not exist");
	}
	else
	{
		App::log("jni:getM_result exist");
		jstring jret = (jstring)function.env->CallStaticObjectMethod(function.classID, function.methodID);
		ret = JniHelper::jstring2string(jret);		
		function.env->DeleteLocalRef(jret);
		function.env->DeleteLocalRef(function.classID);
		if (App::m_debug == 0)
			App::log(StringUtils::format("CocosAndroidJni::getResult()返回值是：%s", ret.c_str()).c_str());
	}
#endif
}

//JNI 拍完照片根据照片名称处理成头像
void CocosAndroidJni::CompressPictureByTakePhoto(const char* portartName)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void GetPortartByTakePhotoPath(String photoName)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GetPortartByTakePhotoPath", "(Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:GetPortartByTakePhotoPath did not exist");
	}
	else
	{
		App::log("jni:GetPortartByTakePhotoPath exist");
		jstring _portartName = function.env->NewStringUTF(portartName);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _portartName);
		function.env->DeleteLocalRef(_portartName);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

/*//选头像
void CocosAndroidJni::SelectPortart(const char* portartName)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void SelectPortart(String name)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "SelectPortart", "(Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:SelectPortart did not exist");
	}
	else
	{
		App::log("jni:SelectPortart exist");
		jstring _portartName = function.env->NewStringUTF(portartName);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _portartName);
		function.env->DeleteLocalRef(_portartName);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}*/


//拍照
void CocosAndroidJni::TakePhotos(const char* photoName)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void TakePhotos(String photoName)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "TakePhotos", "(Ljava/lang/String;)V");
	bool result;
	if (!isHave)
	{
		App::log("jni:TakePhotos did not exist");
	}
	else
	{
		jstring _photoName = function.env->NewStringUTF(photoName);
		App::log("jni:TakePhotos exist");
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _photoName);
		function.env->DeleteLocalRef(_photoName);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

//是否联网
bool CocosAndroidJni::IsNetConnect(bool hint, const char* runKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static boolean IsNetConnect(boolean hint, String runKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "IsNetConnect", "(ZLjava/lang/String;)Z");
	bool result;
	if (!isHave)
	{
		App::log("jni:IsNetConnect did not exist");
		return false;
	}
	else
	{
		App::log("jni:IsNetConnect exist");
		jboolean _hint = hint;
		jstring _runKey = function.env->NewStringUTF(runKey);
		jboolean jret = (jboolean)function.env->CallStaticBooleanMethod(function.classID, function.methodID, _hint, _runKey);
		result = jret;
		function.env->DeleteLocalRef(function.classID);
		function.env->DeleteLocalRef(_runKey);
		return result;
	}
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return true;
#endif
}

//是否有wifi
bool CocosAndroidJni::IsThereWifi()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
    JniMethodInfo function;
    //原型函数public static String Pay(String payMode,String PARTNER,String SELLER,String RSA_PRIVATE,String orderId, String bookName, String bookIntroduction, String bookPrice)
    bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "IsWifi", "()Z");
    bool result;
    if (!isHave)
    {
		App::log("jni:IsWifi did not exist");
        return false;
    }
    else
    {
		App::log("jni:IsWifi exist");
        jboolean jret =(jboolean)function.env->CallStaticBooleanMethod(function.classID, function.methodID);
        result = jret;
		function.env->DeleteLocalRef(function.classID);
        return result;
    }
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return true;
#endif
}

//支付
void CocosAndroidJni::CocosAndroidJniAliPay(std::string& ret,const char* payMode,const char* PARTNER,const char* SELLER,const char* RSA_PRIVATE,const char* orderId, const char * bookName, const char * bookIntroduction, const char * bookPrice)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
    // 定义JNI函数信息结构体
    JniMethodInfo function;
    /**
     * @param 第一个参数：JNI函数信息结构体
     * @param 第二个参数：与你Android项目中的主Activity所在的包名对应，Demo中为：com.youmi.android.cocos2dx.demo.MainActivity，需要改为com/youmi/android/cocos2dx/denmo/MainActivity
     * @param 第三个参数：Android项目中对应的Activity里面所定义的JAVA静态函数名，Demo中为：showSpotAd
     * @param 第四个参数：该JAVA方法所需要传入的值，格式为"(JAVA方法中需要传入的值)c++中本函数返回值"，Demo中，showSpotAd的JAVA方法不需要传入值，而YoumiAd::showSpotAd()返回值为void，所以这里写 "()V"
     * @return 返回bool值表示是否找到静态函数
     */

    //原型函数public static String Pay(String payMode,String PARTNER,String SELLER,String RSA_PRIVATE,String orderId, String bookName, String bookIntroduction, String bookPrice)
    bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "Pay", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
    if (!isHave)
    {
		App::log("jni:Pay did not exist");
    }
    else
    {
		App::log("jni:Pay exist");
        jstring _payMode = function.env->NewStringUTF(payMode);
        jstring _PARTNER = function.env->NewStringUTF(PARTNER);
        jstring _SELLER = function.env->NewStringUTF(SELLER);
        jstring _RSA_PRIVATE = function.env->NewStringUTF(RSA_PRIVATE);
        jstring _orderId = function.env->NewStringUTF(orderId);
        jstring _bookName = function.env->NewStringUTF(bookName);
        jstring _bookIntroduction = function.env->NewStringUTF(bookIntroduction);
		jstring _bookPrice = function.env->NewStringUTF(bookPrice);
		jstring jret =(jstring)function.env->CallStaticObjectMethod(function.classID, function.methodID, _payMode, _PARTNER, _SELLER, _RSA_PRIVATE, _orderId, _bookName, _bookIntroduction, _bookPrice);
        ret = JniHelper::jstring2string(jret);
		function.env->DeleteLocalRef(_payMode);
		function.env->DeleteLocalRef(_PARTNER);
		function.env->DeleteLocalRef(_SELLER);
		function.env->DeleteLocalRef(_RSA_PRIVATE);
		function.env->DeleteLocalRef(_orderId);
		function.env->DeleteLocalRef(_bookName);
		function.env->DeleteLocalRef(_bookIntroduction);
		function.env->DeleteLocalRef(_bookPrice);
		function.env->DeleteLocalRef(jret);
		function.env->DeleteLocalRef(function.classID);
    }
#endif
}

//获取照片存放路径
void CocosAndroidJni::GetPhotoPath(std::string& ret)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static String GetPhotoPath()
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GetPhotoPath", "()Ljava/lang/String;");
	if (!isHave)
	{
		App::log("jni:GetPhotoPath did not exist");
	}
	else
	{
		App::log("jni:GetPhotoPath exist");
		jstring jret = (jstring)function.env->CallStaticObjectMethod(function.classID, function.methodID);
		ret = JniHelper::jstring2string(jret);
		function.env->DeleteLocalRef(jret);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 获取下载的目录
void CocosAndroidJni::GetDownloadPath(std::string& ret)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static String GetDownloadPath()
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GetDownloadPath", "()Ljava/lang/String;");
	if (!isHave)
	{
		App::log("jni:GetDownloadPath did not exist");
	}
	else
	{
		App::log("jni:GetDownloadPath exist");
		jstring jret = (jstring)function.env->CallStaticObjectMethod(function.classID, function.methodID);
		ret = JniHelper::jstring2string(jret);
		function.env->DeleteLocalRef(jret);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 安装apk
void CocosAndroidJni::InstallApk(const char* path)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void InstallApk(String path)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "InstallApk", "(Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:InstallApk did not exist");
	}
	else
	{
		App::log("jni:InstallApk exist");
		jstring _path = function.env->NewStringUTF(path);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _path);
		function.env->DeleteLocalRef(_path);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

// JNI 跳转浏览器
void CocosAndroidJni::GotoUrl(const char* url)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void GotoUrl(String url)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "GotoUrl", "(Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:GotoUrl did not exist");
	}
	else
	{
		App::log("jni:GotoUrl exist");
		jstring _path = function.env->NewStringUTF(url);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _path);
		function.env->DeleteLocalRef(_path);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}

void CocosAndroidJni::saveReadRecord(long memberId, long childrenId, long bookId, const char * readStartTime, const char * readEndTime, const char* runKey, const char* errorKey)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台
	JniMethodInfo function;
	//原型函数public static void httpSaveReadRecord(int memberID,int childrenId, int bookId, String readStartTime, String readEndTime, String runKey, String errorKey)
	bool isHave = JniHelper::getStaticMethodInfo(function, "org/cocos2dx/cpp/AppActivity", "httpSaveReadRecord", "(IIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!isHave)
	{
		App::log("jni:httpSaveReadRecord did not exist");
	}
	else
	{
		App::log("jni:httpSaveReadRecord exist");
		jint _memberID = memberId;
		jint _childrenId = childrenId;
		jint _bookId = bookId;
		jstring _readStartTime = function.env->NewStringUTF(readStartTime);
		jstring _readEndTime = function.env->NewStringUTF(readEndTime);
		jstring _runKey = function.env->NewStringUTF(runKey);
		jstring _errorKey = function.env->NewStringUTF(errorKey);
		function.env->CallStaticVoidMethod(function.classID, function.methodID, _memberID,_childrenId, _bookId, _readStartTime, _readEndTime,_runKey, _errorKey);
		function.env->DeleteLocalRef(_readStartTime);
		function.env->DeleteLocalRef(_readEndTime);
		function.env->DeleteLocalRef(_runKey);
		function.env->DeleteLocalRef(_errorKey);
		function.env->DeleteLocalRef(function.classID);
	}
#endif
}




