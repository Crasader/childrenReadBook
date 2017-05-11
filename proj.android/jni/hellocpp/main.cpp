#include "../../Classes/AppDelegate.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>
#include "../../Classes/BabyCenter.h"
#include "../../Classes/FileNames.h"
#include "../../Classes/BookInfoScene.h"
#include "../../Classes/LoadScene.h"
#include "../../Classes/BookCityScene.h"
#include "../../Classes/App.h"
#include "../../Classes/BookStoreScene.h"
#include "../../Classes/Toast.h"
#include "../../Classes/YYXLayer.h"
#include "../../Classes/YYXStruct.h"
#include "../../Classes/NetIntface.h"

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

void cocos_android_app_init(JNIEnv* env) {
	LOGD("cocos_android_app_init");
	AppDelegate *pAppDelegate = new AppDelegate();
}

extern "C"

{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	
//本地方法 网络接口回调函数
//原型函数public native static void NetInterfaceCallback(String functionKey,String json);
void Java_org_cocos2dx_cpp_AppActivity_NetInterfaceCallback(JNIEnv *env, jobject thiz, jstring functionKey, jstring json) {
	const char* _functionKey = env->GetStringUTFChars(functionKey, NULL);
	const char* _json = env->GetStringUTFChars(json, NULL);
	auto runable = NetIntface::getMapFunction(_functionKey);
	if (runable)
	{
		runable(_json);
	}
}

// 本地方法 获取图片
//原型函数public native static void getPhoto();
void Java_org_cocos2dx_cpp_AppActivity_getPhoto(JNIEnv *env, jobject thiz) {
	Toast::create(App::getString("STR_PICTUREING"));
	std::thread thread_1([]() {
		CocosAndroidJni::CompressPictureByTakePhoto(App::m_photsName.c_str());
		YYXLayer::sendNotify("PhotoLayerShowFullPathImage");
	});
	thread_1.detach();
}
	
// 本地方法 下载成功后的路径回调
//原型函数public native static void CallBackhttpGetFirstChildHeadPortrait(String path);
void Java_org_cocos2dx_cpp_AppActivity_CallBackhttpGetFirstChildHeadPortrait(JNIEnv *env, jobject thiz, jstring path)
{
	const char* _path = env->GetStringUTFChars(path, NULL);
	YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "firstChildHeadPortrait", -999, _path);
}


// 本地方法 通知
//原型函数public native static void sendNotify4YYXStruct(string eventName, long longdata, string stringdata);
void Java_org_cocos2dx_cpp_AppActivity_sendNotify4YYXStruct(JNIEnv *env, jobject thiz, jstring eventName, jlong longdata, jstring stringdata)
{
	const char* _eventName = env->GetStringUTFChars(eventName, NULL);
	const char* _stringdata = env->GetStringUTFChars(stringdata, NULL);
	long long _longdata = longdata;
	YYXLayer::sendNotify4YYXStruct(_eventName, _longdata, _stringdata);
}

// 本地方法 充值成功后的回调,处理:在书籍详情则自动继续购书
//原型函数public native static void CallBackPaySuccessGoToBuyBook();
void Java_org_cocos2dx_cpp_AppActivity_CallBackPaySuccessGoToBuyBook(JNIEnv *env, jobject thiz)
{
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
		Director::getInstance()->getEventDispatcher()->setEnabled(true);
		EventCustom event("CallBackPaySuccessGoToBuyBook");
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
	});
}

// 本地方法 cocos的Toast弹框
//原型函数public native static void cocosToast(String str);
void Java_org_cocos2dx_cpp_AppActivity_cocosToast(JNIEnv *env, jobject thiz, jstring objectstring)
{
	const char* resultstring = env->GetStringUTFChars(objectstring, NULL);
	Toast::create(resultstring);
}

// 本地方法 书店当前页列表下载的回调
//原型函数 public native static void CallBackBookStoreSceneGetBookStorePageInfo();
void Java_org_cocos2dx_cpp_AppActivity_CallBackBookStoreSceneGetBookStorePageInfo(JNIEnv *env, jobject thiz)
{
	std::string str;
	CocosAndroidJni::getResult(str);
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([str] {
		Director::getInstance()->getEventDispatcher()->setEnabled(true);
		EventCustom event("booksInfoCallBack");
		char* data = new char[str.length()];
		sprintf(data, "%s", str.c_str());
		event.setUserData(data);
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
		CC_SAFE_DELETE_ARRAY(data);
	});
}

// 本地方法 测试账号登陆的回调
//原型函数	public native static void CallBackTest();
	void Java_org_cocos2dx_cpp_AppActivity_CallBackTest(JNIEnv *env, jobject thiz)
	{
		App::m_debug = 0;
		App::log("m_debug=", App::m_debug);
	}

// 本地方法 注册成功的回调
//原型函数public native static void CallBackLoginSceneAccountRegisteSuccess();
void Java_org_cocos2dx_cpp_AppActivity_CallBackLoginSceneAccountRegisteSuccess(JNIEnv *env, jobject thiz)
{
	std::string str;
	CocosAndroidJni::getResult(str);
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([str] {
		Director::getInstance()->getEventDispatcher()->setEnabled(true);
		EventCustom event("registerSuccess");
		char* data = new char[str.length()];
		sprintf(data,"%s",str.c_str());
		event.setUserData(data);
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
		CC_SAFE_DELETE_ARRAY(data);
	});
}

// 本地方法 发表评论成功的回调
//原型函数public native static void CallBackBookInfoSceneSendComment();
void Java_org_cocos2dx_cpp_AppActivity_CallBackBookInfoSceneSendComment(JNIEnv *env, jobject thiz)
{
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([] {
		Director::getInstance()->getEventDispatcher()->setEnabled(true);
		EventCustom event("httpBookComments");
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
	});
}

void Java_org_cocos2dx_cpp_AppActivity_takePotos(JNIEnv *env, jobject thiz) {
	CCLOG("%s", "拍照返回");
	//开启一个线程找头像然后发消息
	//CocosAndroidJni::ToastAndroid(App::getString("STR_PICTUREING"));
	Toast::create(App::getString("STR_PICTUREING"));
	std::thread thread_1(
			[]() {
				int i = 0;
				string path;
				CocosAndroidJni::GetPhotoPath(path);
				string fullpath = path + "/" + App::m_photsName;
				while (!FileUtils::getInstance()->isFileExist(fullpath))
				{
					if (i > 10)
					return;
					App::ccsleep(300);
					++i;
					App::log("拍照wait = %d*300ms", i);
				}
				App::log("拍照找到图片");
				CocosAndroidJni::CompressPictureByTakePhoto(App::m_photsName.c_str());
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
							//if (App::GetInstance()->m_RunningScene == BabyCenterScene)
							//{
								auto scene = (BabyCenter*)Director::getInstance()->getRunningScene()->getChildByTag(9);
								scene->m_BigPhoto->loadTexture(fullpath);
								scene->m_img_photo->loadTexture(fullpath);
							//}
						});
			});
	thread_1.detach();
}

void Java_org_cocos2dx_cpp_AppActivity_getPayResult(JNIEnv *env, jobject thiz) {
	std::string result;
	CocosAndroidJni::getResult(result);
	App::log("CocosAndroidJni::getResult(result) = " + result);
	//resultStatus={9000};memo={处理成功};result={partner="2088011267982277"&seller_id="xkhqjd@163.com"&out_trade_no="2876"&subject="浣熊妈妈"&body="每到星期天，浣熊妈妈就洗啊洗的。她洗被罩、床单、窗帘，洗所有能洗的东西。她还洗了什么呢？小朋友们到故事里面找一找吧！"&total_fee="0.01"&notify_url="http://121.40.212.208/ellabook-server/notify_url.jsp"&service="mobile.securitypay.pay"&payment_type="1"&_input_charset="utf-8"&it_b_pay="30m"&success="true"&sign_type="RSA"&sign="ca0xKEFk5WmgXWY5wHcdZ3it72zhXLPp/VIwzgAkHlcz9MLLeSBzq9v9K7i11lp4SxF8U295HYSQ9pOnPwJgYTNMEoMlvu++eKovNu/M0fQsP67v8lm2L53jsoleDcOJb9JRLD2ss1dO5YfJb/p32wl8hRZX6cK1AKKncQbytkI="}
	if (result == "9000") 
	{
		Toast::create(App::getString("CHONGZHICHENGGONG"));
	}
	//刷新余额
	Director::getInstance()->getEventDispatcher()->setEnabled(true);
	EventCustom event1("refershBalanceAndShowRedPacket");
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event1);
	EventCustom event2("CallBackPaySuccessGoToBuyBook");
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event2);
	CocosAndroidJni::setResult("-1");
}
#endif
}
