/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.cpp;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.mime.MultipartEntity;
import org.apache.http.entity.mime.content.FileBody;
import org.apache.http.entity.mime.content.StringBody;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.cocos2dx.cpp.http.BabyCenter;
import org.cocos2dx.cpp.http.BookCityScene;
import org.cocos2dx.cpp.http.BookInfoScene;
import org.cocos2dx.cpp.http.BookStoreScene;
import org.cocos2dx.cpp.http.CallBackFunction;
import org.cocos2dx.cpp.http.CallBackFunction.DownLoadImage;
import org.cocos2dx.cpp.http.CallBackFunction.FindFunctionForKey;
import org.cocos2dx.cpp.http.HttpTool;
import org.cocos2dx.cpp.http.JsonObjectPostRequest;
import org.cocos2dx.cpp.http.LoginScene;
import org.cocos2dx.cpp.http.NetInterface;
import org.cocos2dx.cpp.http.Recharge;
import org.cocos2dx.cpp.sqlite.MySQLiteOpenHelper;
import org.cocos2dx.lib.Cocos2dxActivity;
import org.json.JSONException;
import org.json.JSONObject;

import com.alipay.sdk.app.PayTask;
import com.android.volley.DefaultRetryPolicy;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.Response.ErrorListener;
import com.android.volley.Response.Listener;
import com.android.volley.VolleyError;
import com.android.volley.Request.Method;
import com.android.volley.toolbox.StringRequest;
import com.ellabook.record.RecordPresenter;
import com.ellabook.share.MySharePresenter;
import com.ellabook.wxapi.WXPayEntryActivity;
import com.tencent.weibo.sdk.android.api.util.SharePersistent;
import com.umeng.analytics.MobclickAgent;
import com.umeng.analytics.MobclickAgent.EScenarioType;
import com.umeng.analytics.MobclickAgent.UMAnalyticsConfig;
import com.umeng.analytics.game.UMGameAgent;
import com.umeng.socialize.Config;
import com.umeng.socialize.PlatformConfig;
import com.umeng.socialize.UMShareAPI;
import com.umeng.socialize.UMShareListener;
import com.umeng.socialize.bean.SHARE_MEDIA;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.AudioManager;
import android.media.MediaScannerConnection;
import android.media.MediaScannerConnection.MediaScannerConnectionClient;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;

public class AppActivity extends Cocos2dxActivity {
	public static AppActivity m_context;

	public static String m_result;

	public static String m_portartName;

	public static RequestQueue mQueue;

	public static int RequestId = 0;
	
	static WindowManager mag;
	public static HashMap<String, String> m_callbackKey = new HashMap<String, String>();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		m_context = this;
		mQueue = HttpTool.getRequestQueue(m_context);
		//暂停音乐
		AudioManager am = (AudioManager) this.getApplicationContext().getSystemService(Context.AUDIO_SERVICE);
		am.requestAudioFocus(null, AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN_TRANSIENT);
		UMGameAgent.setDebugMode(true);
		UMGameAgent.init(this);
		MobclickAgent.setScenarioType(this, EScenarioType.E_UM_GAME);
		//微信 wx12342956d1cab4f9,a5ae111de7d9ea137e88a5e02c07c94d
        PlatformConfig.setWeixin("wx612d474d79028d9b", "dc5ceae49a56971ed7c9ac7cf227548d");
        //新浪微博
        PlatformConfig.setSinaWeibo("3921700954", "04b48b094faeb16683c32669824ebdad");
        //QQ
        PlatformConfig.setQQZone("100424468", "c7394704798a158208a74ab60104f0ba");
        Config.IsToastTip = false;
        //友盟分包
        String s =SignUtils.getChannel(this);
		MobclickAgent.startWithConfigure(new UMAnalyticsConfig(this,"57482800e0f55a57700010dd", s, EScenarioType.E_UM_GAME));
		mag = getWindowManager();
		super.onCreate(savedInstanceState);
	}

	@Override
    public void onResume() {
        super.onResume();
        // 集成基本统计分析,初始化 Session
        UMGameAgent.onResume(this);
    }

    @Override
    public void onPause() {
        super.onPause();
        // //集成基本统计分析, 结束 Session
        UMGameAgent.onPause(this);
    }
    
    //本地方法 网络接口回调函数
    public native static void NetInterfaceCallback(String functionKey,String json);
    
    //本地方法 发送通知
    public native static void sendNotify4YYXStruct(String eventName, long longdata, String stringdata);

	// 本地方法 cocos的Toast弹框
	public native static void cocosToast(String str);

	// 本地方法 拍照
	public native static void takePotos();

	// 本地方法 获取图片
	public native static void getPhoto();

	// 本地方法 支付结果
	public native static void getPayResult();

	// 本地方法 自动登录的回调
	public native static void CallBackLoadSceneAccountLogin();

	// 本地方法 请求孩子列表信息回调
	public native static void CallBackLoadScenegetChildDetails();

	// 本地方法 书城按钮列表信息回调
	public native static void CallBackBookCitySceneGetBookCityButtonsInfo();

	// 本地方法 书城下载图片完成后的回调
	public native static void CallBackBookCitySceneDownLoadPicture();

	// 本地方法 书店当前页列表下载的回调
	public native static void CallBackBookStoreSceneGetBookStorePageInfo();

	// 本地方法 书城封面下载完成后的回调
	public native static void CallBackBookStoreSceneDownLoadCover();

	// 本地方法 书籍详情的回调
	public native static void CallBackBookInfoSceneGetBookInfo();

	// 本地方法 书籍封面的回调
	public native static void CallBackBookInfoSceneDownLoadCover();

	// 本地方法 跳转书
	public native static void GoToBook();

	// 本地方法 返回上级
	public native static void GoBack();

	// 本地方法 刷新孩子头像
	public native static void CallBackBabySceneReferChildPortrait();

	// 本地方法 上传头像的回调
	public native static void CallBackBabySceneUpPortrait();

	// 本地方法 已购列表的回调
	public native static void CallBackLoadSceneGetBuyBook(String str);

	// 本地方法 查询余额的回调
	public native static void CallBackRechargeGetUserBalance();

	// 本地方法 余额购书成功的回调
	public native static void CallBackRechargeBuyBookSuccess();

	// 本地方法 余额不足的回调
//	public native static void CallBackRechargeBuyBookMyBalanceIsNotEnough();

	// 本地方法 余额购书失败的回调
	public native static void CallBackRechargeBuyBookFail();

	// 本地方法 注册成功的回调
	public native static void CallBackLoginSceneAccountRegisteSuccess();

	// 本地方法 发表评论成功的回调
	public native static void CallBackBookInfoSceneSendComment();	

	// 本地方法 测试账号登陆的回调
	public native static void CallBackTest();

	// 本地方法 充值成功后的回调,处理:在书籍详情则自动继续购书
	public native static void CallBackPaySuccessGoToBuyBook();
	
	// 本地方法 下载成功后的路径回调
	public native static void CallBackhttpGetFirstChildHeadPortrait(String path);
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		UMShareAPI.get(this).onActivityResult(requestCode, resultCode, data);
		// ------------------------------//打开相册选中图片,裁切成圆形图片保存到指定位置-----------------------------------------------
		if (requestCode == MacroCode.openPhotoAlbumSelectImage) 
		{
			if (resultCode == RESULT_OK && data != null) {
				Uri uri = data.getData();
				String imageName = m_callbackKey
						.get("openPhotoAlbumSelectImageName");
				String imageDir = m_callbackKey
						.get("openPhotoAlbumSelectImageDir");
				String width = m_callbackKey
						.get("openPhotoAlbumSelectImageWidth");
				String height = m_callbackKey
						.get("openPhotoAlbumSelectImageHeight");
				Bitmap bp = ImageTool.readUri(uri, m_context, 600, 600, false);
//				Bitmap rp = ImageTool.toRoundBitmap(bp);
//				ImageTool.saveFile(rp, imageName, imageDir);
//				Bitmap bp = ImageTool.readUri(uri, m_context,Integer.parseInt(width), Integer.parseInt(height),false);
				ImageTool.saveFile(bp, imageName, imageDir);
				String runKey = m_callbackKey.get("openPhotoAlbumSelectImageRunKey");
				NetInterfaceCallback(runKey, imageDir + File.separator + imageName);
			} else {
				try {
					String errorKey = m_callbackKey.get("openPhotoAlbumSelectImageErrorKey");
					if (!errorKey.equals(""))
						NetInterfaceCallback(errorKey, "");
				} catch (Exception e) {
					Log.e("201608111727", e.toString());
				}
			}
			// ------------------------------//拍照,图片保存到指定位置-----------------------------------------------
		} else if (requestCode == MacroCode.photographAlbumSelectImage) 
		{
			Log.e("show","resultCode= "+resultCode);
			if (resultCode == RESULT_OK) {
				String name = m_callbackKey
						.get("photographAlbumSelectImageName");
				String dir = m_callbackKey.get("photographAlbumSelectImageDir");
				String runKey = m_callbackKey
						.get("photographAlbumSelectImageRunKey");
//				Bitmap bp = ImageTool.getBitmapformPicturePathByCompaction(dir
//						+ File.separator + name, m_context, 600, 600, false);
//				Bitmap rp = ImageTool.toRoundBitmap(bp);
//				ImageTool.saveFile(rp, "save_" + name, dir);
//				NetInterfaceCallback(runKey, dir + File.separator + "save_"
				NetInterfaceCallback(runKey, dir + File.separator 
						+ name);
			} else {
				try {
					String errorKey = m_callbackKey
							.get("photographAlbumSelectImageErrorKey");
					if (!errorKey.equals(""))
						NetInterfaceCallback(errorKey, "");
				} catch (Exception e) {
					Log.e("201608111728", e.toString());
				}
			}
		} else if (requestCode == MacroCode.BOOKInfoActivity) {
			if (resultCode == MacroCode.BookInfoScene_Back_ReturnCode)
				GoBack();
			else
				GoToBook();
		} 
//		else if(resultCode == MacroCode.WeixinPay && data.getStringExtra("key").equals("weixinpay"))
//		{
//			int res = data.getIntExtra("errCode", -1);
//			if(res == 0)
//			{
//				m_result = "9000";				
//			}
//			else
//			{
//				m_result = "-1";
//			}
//			getPayResult();
//			Log.e("show","支付完成会的回调字符串 = " + m_result);
//		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	// ------------------mHandler的处理函数-------------------------------------------------
	private static final int SDK_PAY_FLAG = 1;

	private static final int SDK_CHECK_FLAG = 2;

	public static Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			/*case SDK_PAY_FLAG: {
				PayResult payResult = new PayResult((String) msg.obj);

				// 支付宝返回此次支付结果及加签，建议对支付宝签名信息拿签约时支付宝提供的公钥做验签
				// String resultInfo = payResult.getResult();

				String resultStatus = "-1";
				resultStatus = payResult.getResultStatus();
				m_result = resultStatus;
				getPayResult();
				// 判断resultStatus 为“9000”则代表支付成功，具体状态码代表含义可参考接口文档
				if (TextUtils.equals(resultStatus, "9000")) {
					//Toast.makeText(m_context, "支付成功", Toast.LENGTH_SHORT).show();
					//cocosToast("支付成功");
				} else {
					// 判断resultStatus 为非“9000”则代表可能支付失败
					// “8000”代表支付结果因为支付渠道原因或者系统原因还在等待支付结果确认，最终交易是否成功以服务端异步通知为准（小概率状态）
					if (TextUtils.equals(resultStatus, "8000")) {
						//Toast.makeText(m_context, "支付结果确认中", Toast.LENGTH_SHORT).show();						
						cocosToast("支付结果确认中");
					} else {
						// 其他值就可以判断为支付失败，包括用户主动取消支付，或者系统返回的错误
						//Toast.makeText(m_context, "支付失败", Toast.LENGTH_SHORT).show();						
						cocosToast("支付失败");
					}
				}
				break;
			}
			case SDK_CHECK_FLAG: {
				Toast.makeText(m_context, "检查结果为：" + msg.obj,
						Toast.LENGTH_SHORT).show();
				break;
			}*/
			case 3:
				Exit();
				break;
			case 4:
				//Toast.makeText(m_context, "" + msg.obj, Toast.LENGTH_SHORT).show();
				/*new AlertDialog.Builder(m_context).setMessage(msg.obj.toString()).setTitle("系统提示")
				.setNegativeButton("确定", new DialogInterface.OnClickListener() {					
					@Override
					public void onClick(DialogInterface arg0, int arg1) {

					}
				}).show();*/
				cocosToast(msg.obj.toString());
				break;
			case MacroCode.MessageBox:
				try{
					JSONObject json = new JSONObject(msg.obj.toString());
					String title = json.getString("title");
					String message = json.getString("message");
					new AlertDialog.Builder(m_context).setMessage(message).setTitle(title).setNegativeButton("确定", new DialogInterface.OnClickListener() {

						@Override
						public void onClick(DialogInterface arg0, int arg1) {

						}
					}).show();
				}catch (Exception e) {
					Log.e("201607011027",e.toString());
				}
				break;
			case MacroCode.LoadScene_AccountLogin:
				// 登录后回调
				CallBackLoadSceneAccountLogin();
				break;
			case MacroCode.LoadScene_getChildDetails:
				// 请求孩子列表信息回调
				CallBackLoadScenegetChildDetails();
				break;
			case MacroCode.BookCityScene_GetBookCityButtonsInfo:
				// 书城回调
				CallBackBookCitySceneGetBookCityButtonsInfo();
				Log.i("show", "书城信息回调");
				break;
			case MacroCode.BookCityScene_DownLoadPicture:
				// 书城图片下载完成后的回调
				CallBackBookCitySceneDownLoadPicture();
				break;
			case MacroCode.BookStoreScene_GetBookStorePageInfo:
				// 书店当前页列表下载回调
				CallBackBookStoreSceneGetBookStorePageInfo();
				break;
			case MacroCode.BookStoreScene_DownLoadCover:
				// 书城图片下载完成后的回调
				CallBackBookStoreSceneDownLoadCover();
				break;
			case MacroCode.BookInfoScene_GetBookInfo:
				// 书籍详情的回调
				Log.i("show", "handle:BookInfoScene_GetBookInfo");
				CallBackBookInfoSceneGetBookInfo();
				break;
			case MacroCode.BookInfoScene_DownLoadCover:
				// 书籍封面下载完成的回调
				CallBackBookInfoSceneDownLoadCover();
				break;
			case MacroCode.LoadScene_AccountLoginException:
				// 登陆异常回调
				CallBackLoadSceneAccountLogin();
				break;
			case MacroCode.BabyCenterScene_UpImage:
				// 上传头像回调
				CallBackBabySceneUpPortrait();				
				break;
			case MacroCode.BabyCenterScene_REFERCHILDPORTRAIT:
				// 刷新头像
				CallBackBabySceneReferChildPortrait();
				break;
			case MacroCode.LoadScene_GetBuyBook:
				//已购列表的回调
				CallBackLoadSceneGetBuyBook(msg.obj.toString());
				break;
			/*case MacroCode.WeixinPayResult:
				//微信支付回调
				try{
					int errCode = msg.arg1;
					if(errCode == 0)
					{
						m_result = "9000";
					}
					else
					{
						m_result = "-1";
					}
					getPayResult();
				}catch (Exception e) {
					Log.e("201606211113",e.toString());
				}
				break;*/
			/*case MacroCode.RechargeGetUserBalance:
				//查询余额回调
				CallBackRechargeGetUserBalance();
				break;*/
//			case MacroCode.RechargeBuyBookSuccess:
				//余额购书成功的回调
//				CallBackRechargeBuyBookSuccess();
//				break;	
//			case MacroCode.RechargeBuyBookMyBalanceIsNotEnough:
				//余额不足回调
//				MyBalanceIsNotEnough();
//				break;
			case MacroCode.RechargeBuyBookFail:
				//余额购书失败的回调
				CallBackRechargeBuyBookFail();
				break;
			case MacroCode.LoadScene_AccountRegiste:
				//注册成功的回调
				CallBackLoginSceneAccountRegisteSuccess();
				break;
			case MacroCode.BookInfoScene_SendComment:
				//发表评论的回调
				CallBackBookInfoSceneSendComment();
				break;
			case MacroCode.BookShare:
				JSONObject data = (JSONObject) msg.obj;		
				String filePath = HttpTool.GetString(data,"filePath", "201608031129");
				String bookName = HttpTool.GetString(data,"bookName", "201608031128");
				String targetUrl = HttpTool.GetString(data,"targetUrl", "201608031127");
				String headUrl = HttpTool.GetString(data,"headUrl", "201608031126");
				String title = HttpTool.GetString(data,"title", "201608031125");
				final String runKey = HttpTool.GetString(data,"runKey", "201608031124");
				final String errorKey = HttpTool.GetString(data,"errorKey", "201608031123");
				MySharePresenter.showSharePop(m_context, filePath, bookName, targetUrl, headUrl, title, new UMShareListener() {
					
					@Override
					public void onResult(SHARE_MEDIA arg0) {
//						cocosToast("分享成功");
						NetInterfaceCallback(runKey, "");
					}
					
					@Override
					public void onError(SHARE_MEDIA arg0, Throwable arg1) {
//						cocosToast("分享失败");
						NetInterfaceCallback(errorKey, "");
					}
					
					@Override
					public void onCancel(SHARE_MEDIA arg0) {
//						cocosToast("分享取消");
						NetInterfaceCallback(errorKey, "");
					}
				});
				break;
			case MacroCode.InviteFriendsToRegister:
				JSONObject data2 = (JSONObject) msg.obj;		
				final String url = HttpTool.GetString(data2,"url", "201611081046");
				int memberid = HttpTool.GetInt(data2,"memberid", "201611081047");
				final String runKey2 = HttpTool.GetString(data2,"runkey", "201611081048");
				final String errorKey2 = HttpTool.GetString(data2,"errorkey", "201611081049");
				MySharePresenter.showInviteregisterPop(m_context, mQueue, url, "咿啦看书新人礼,首次成功注册登录,立享10元购书红包", memberid,
						"咿啦看书是一款专为儿童打造的动画书阅读APP,在这里有孩子喜欢的儿童故事和绘本.", new UMShareListener() {
					
					@Override
					public void onResult(SHARE_MEDIA arg0) {
//						cocosToast("分享成功");
						NetInterfaceCallback(runKey2, "");
					}
					
					@Override
					public void onError(SHARE_MEDIA arg0, Throwable arg1) {
//						cocosToast("分享失败");
						NetInterfaceCallback(errorKey2, "");
					}
					
					@Override
					public void onCancel(SHARE_MEDIA arg0) {
//						cocosToast("分享取消");
						NetInterfaceCallback(errorKey2, "");
					}
				});
				break;
			case MacroCode.commentTheRecording:
				JSONObject Jdata = (JSONObject) msg.obj;		
				int bookid = HttpTool.GetInt(Jdata,"bookid", "201610291536");
				String membername = HttpTool.GetString(Jdata,"membername", "201610291537");
				int memberId = HttpTool.GetInt(Jdata,"memberId", "201610291538");
				int types = HttpTool.GetInt(Jdata,"types", "201610291539");
				String orderid = HttpTool.GetString(Jdata,"orderid", "201610291528");
				String runkey = HttpTool.GetString(Jdata,"runkey", "201610291527");
				String errorkey = HttpTool.GetString(Jdata,"errorkey", "201610291526");
				String closekey = HttpTool.GetString(Jdata,"closekey", "201610291525");
				new RecordPresenter().show(m_context,bookid, membername, memberId,orderid,types,runkey,errorkey,closekey,mag);
				break;
			default:
				break;
			}
		};
	};

	// ------------------mHandler的处理函数END-------------------------------------------------
	
	// JNI 打开数据库
	public static boolean openDataBase()
	{
//		new DbFromJSONEX(m_context, "");
		return true;
	}
	
	// JNI 判断数据库是否正常,判断数据库的版本是否最新,否则重建数据库
	public static int examineDataBase(int dataBaseVersion) {
		/*String dir = m_context.getApplicationContext().getFilesDir()
				.getAbsolutePath();
		String path = dir + "/data.db";
		try {
			SQLiteDatabase db = OpenDataBase();
			long dbversion = MySQLiteOpenHelper.getDataBaseVersion(db);
			CloseDataBase(db);
			if (dbversion == dataBaseVersion) {
				return 0;
			} else {
				Log.e("show", dbversion + " updata DataBase:" + dataBaseVersion);
				MySQLiteOpenHelper.copyDataBase(m_context, dir, path);
				return (int) (dataBaseVersion - dbversion);
			}
		} catch (Exception e) {
			Log.e("201606011803", e.toString());
			MySQLiteOpenHelper.copyDataBase(m_context, dir, path);
			return 999;
		}*/
		return 0;
	}
	
	//获取毫秒时间
	public static long getMillsTime()
	{
		return System.currentTimeMillis();
	}

	//网络请求上传文件接口
	public static void httpUpFile(final String url, String jsonData,String filepath, final String runKey, final String errorKey)
	{
		HttpClient httpclient = new DefaultHttpClient();
		HttpPost httppost = new HttpPost(url);
		try {
			FileBody fileStream = new FileBody(new File(filepath));
			MultipartEntity reqEntity = new MultipartEntity();
			reqEntity.addPart("file", fileStream);
			JSONObject ob = new JSONObject(jsonData);
			Iterator it = ob.keys();
			while(it.hasNext())
			{
				String key = (String) it.next();
				String value = ob.getString(key);
				reqEntity.addPart(key, new StringBody(value));
			}
			httppost.setEntity(reqEntity);
			HttpResponse response = httpclient.execute(httppost);
			HttpEntity resEntity = response.getEntity();
			int statusCode = response.getStatusLine().getStatusCode();
			if (statusCode == HttpStatus.SC_OK)
			{
				if (resEntity != null)
				{
					String json = EntityUtils.toString(resEntity, "utf-8");
					NetInterfaceCallback(runKey,json);
					if (resEntity != null)
					{
						resEntity.consumeContent();
					}
					httpclient.getConnectionManager().shutdown();
				}
			}
			else
				NetInterfaceCallback(errorKey, "");
		} catch (Exception e) {
			Log.e("201612121539", e.toString());
			NetInterfaceCallback(errorKey, e.toString());
		}
	}

	//网络请求get
	public static void httpGet(final String url, final String runKey, final String errorKey)
	{		
		StringRequest request = new StringRequest(Method.GET, url,
				new Response.Listener<String>() {
					@Override
					public void onResponse(String response) {
						Log.e("show", url+"                  "+response);
						NetInterfaceCallback(runKey, response);
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("show", url+"                  "+error.toString());
						Log.e("201609101414", error.toString());
						NetInterfaceCallback(errorKey, "");
					}
				});
		request.setRetryPolicy(new DefaultRetryPolicy(100000,3,DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));
		mQueue.add(request);
	}
	
	//网络请求post
	public static void httpPost(final String url, String jsonData,final String runKey, final String errorKey)
	{
		try {
			final Map<String, String> paramter = new HashMap<String, String>();
			JSONObject ob = new JSONObject(jsonData);
			Iterator it = ob.keys();
			while(it.hasNext())
			{
				String key = (String) it.next();
				String value = ob.getString(key);
				paramter.put(key, value);
			}
			JsonObjectPostRequest request = new JsonObjectPostRequest(url,
					paramter, new Listener<JSONObject>() {

						@Override
						public void onResponse(JSONObject response) {
							Log.e("show", url+"                  "+paramter.toString()+"                "+response.toString());
							NetInterfaceCallback(runKey, response.toString());
						}

					}, new ErrorListener() {

						@Override
						public void onErrorResponse(VolleyError error) {
							Log.e("show", url+"                  "+paramter.toString()+"                  "+error.toString());
							NetInterfaceCallback(errorKey, error.toString());
						}
					});
			request.setRetryPolicy(new DefaultRetryPolicy(100000,3,DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));
			mQueue.add(request);
		} catch (JSONException e) {
			e.printStackTrace();
		}	
	}
	
	// JNI 充值订单
	public static void httpGetRechargeOrderID(int memberid, int rechargeCount,
			int price100, String payType, String payInfo, final String runKey,
			final String errorKey) {
		m_callbackKey.put("RechargeRunKey", runKey);
		m_callbackKey.put("RechargeErrorKey", errorKey);
		NetInterface.httpGetOrderId(mQueue, memberid, rechargeCount, price100,
				payType, payInfo, runKey, errorKey);
	}
	// JNI VIP订单
	public static void httpGetVIPOrderID(int memberid, int rechargeCount,
	                                          int price100, String payType, String payInfo, final String runKey,
	                                          final String errorKey) {
		m_callbackKey.put("RechargeRunKey", runKey);
		m_callbackKey.put("RechargeErrorKey", errorKey);
		NetInterface.httpGetVIPOrderId(mQueue, memberid, rechargeCount, price100,
				payType, payInfo, runKey, errorKey);
	}
	
	// 获取分享成功的红包
	public static void httpShareWithCoupon(int memberID, String runKey, String errorKey)
	{
		NetInterface.httpShareWithCoupon(mQueue, memberID, runKey, errorKey, new FindFunctionForKey() {
			@Override
			public void doSomething(String functionKey, String json) {
				NetInterfaceCallback(functionKey, json);
			}
		});
	}
	
	// 获取书店当前页的书籍列表信息
	public static void httpBookStoreSceneCurrentlyPageBookListInfo(
			int memberID, int BookStoreId, int pageIndex, String visitFrom,
			String runKey, String errorKey) {
		try {
			NetInterface.httpBookStoreSceneCurrentlyPageBookListInfo(mQueue,
					memberID, BookStoreId, pageIndex, visitFrom, runKey,
					errorKey, new FindFunctionForKey() {
						@Override
						public void doSomething(String functionKey, String json) {
							NetInterfaceCallback(functionKey, json);
						}
					});
		} catch (Exception e) {
			Log.e("201608241529", e.toString());
		}
	}
	
	// JNI 获取当前版本号
	public static String getAppVersion()
	{
		return Tool.getVersion(m_context);
	}
	
	// JNI 获取用户书籍的已购列表
	public static void httpGetUserBuyBooks(int memberID, String runKey, String errorKey)
	{
		try {
			NetInterface.httpGetUserBuyBooks(mQueue, m_context, memberID, runKey, errorKey, new FindFunctionForKey() {
				@Override
				public void doSomething(String functionKey, String json) {
					NetInterfaceCallback(functionKey, json);
				}
			});
		} catch (Exception e) {
			Log.e("201608171509", e.toString());
		}
	}
	
	

// JNI 记录阅读记录
	public static void httpSaveReadRecord(int memberID,int childrenId, int bookId, String readStartTime, String readEndTime, String runKey, String errorKey)
	{
		try {
			NetInterface.httpSaveReadRecord(mQueue, m_context,  memberID, childrenId,  bookId, readStartTime,  readEndTime, runKey, errorKey, new FindFunctionForKey() {
				@Override
				public void doSomething(String functionKey, String json) {
					NetInterfaceCallback(functionKey, json);
				}
			});
		} catch (Exception e) {
			Log.e("201608171509", e.toString());
		}
	}
	
	// JNI 获取用户安卓的账户余额
	public static void httpGetUserBalance(int memberID, String runKey, String errorKey)
	{
		try{
			NetInterface.httpGetUserBalance(mQueue, memberID, runKey, errorKey, new FindFunctionForKey() {
						@Override
						public void doSomething(String functionKey, String json) {
							NetInterfaceCallback(functionKey, json);
						}
					});
		}
		catch(Exception e){
			Log.e("201608170937", e.toString());
		}
	}
	
	// JNI 注册
	public static void httpAccountRegiste(String memberName, String memberPasswd, String runKey, String errorKey)
	{
		try {
			NetInterface.httpAccountRegiste(mQueue, memberName, memberPasswd, runKey, errorKey, new FindFunctionForKey() {

						@Override
						public void doSomething(String functionKey, String json) {
							NetInterfaceCallback(functionKey, json);
						}
					});
		} catch (Exception e) {
			Log.e("201608161708", e.toString());
		}
	}
	
	// JNI 修改用户信息
	public static void httpAmendUserInfo(int memberId, int sex, String city,String province, String runKey, String errorKey) {
		try {
			NetInterface.httpAmendUserInfo(mQueue, memberId, sex, city,
					province, runKey, errorKey, new FindFunctionForKey() {

						@Override
						public void doSomething(String functionKey, String json) {
							NetInterfaceCallback(functionKey, json);
						}
					});
		} catch (Exception e) {
			Log.e("201608161444", e.toString());
		}
	}
	
	// JNI UTF-8转GBK
	public static void UTF82GBK(String GBK, String UTF)
	{
		try {
			Log.e("show","GBK="+GBK);
			Log.e("show","UTF="+UTF);
			GBK = new String(UTF.getBytes("utf-8"),"ISO-8859-1");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
	}
	
	// JNI GBK转UTF-8
	public static void GBK2UTF8(String GBK, String UTF)
	{
		try {
			Log.e("show","GBK="+GBK);
			Log.e("show","UTF="+UTF);
			UTF = new String(GBK.getBytes("ISO-8859-1"),"utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
	}
	
	// JNI 修改孩子信息
	public static void httpAmendBabyInfo(int childrenId, String childrenName, int childrenSex, 
			String childrenBirth,final String runKey, final String errorKey)
	{
		try {
			NetInterface.httpAmendBabyInfo(mQueue, childrenId, childrenName, 
					childrenSex, childrenBirth, runKey, errorKey,new FindFunctionForKey() {
			
			@Override
			public void doSomething(String functionKey, String json) {
				NetInterfaceCallback(functionKey, json);
			}
		});
		} catch (Exception e) {
			Log.e("201608111439", e.toString());
		}
	}
	
	// JNI 拍照,图片保存到指定位置
	public static void photographAlbumSelectImage(String name,String dir, String runKey,String errorKey) {
		try {
			m_callbackKey.put("photographAlbumSelectImageName", name);
			m_callbackKey.put("photographAlbumSelectImageDir", dir);
			m_callbackKey.put("photographAlbumSelectImageRunKey", runKey);
			if (!errorKey.equals(""))
				m_callbackKey.put("photographAlbumSelectImageErrorKey",
						errorKey);
			File dirFile = new File(dir);
			if (!dirFile.exists()) {
				dirFile.mkdir();
			}
			File storageDir = new File(dir, name);
			Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
			intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(storageDir));
			m_context.startActivityForResult(intent,
					MacroCode.photographAlbumSelectImage);
		} catch (Exception e) {
			Log.e("201608111438", e.toString());
		}
	}
	
	// JNI 打开相册选中图片,图片保存到指定位置
	public static void openPhotoAlbumSelectImage(String name, String dir,
			int width, int height, String runKey, String errorKey) {
		try {
			m_callbackKey.put("openPhotoAlbumSelectImageName", name);
			m_callbackKey.put("openPhotoAlbumSelectImageDir", dir);
			m_callbackKey.put("openPhotoAlbumSelectImageRunKey", runKey);
			m_callbackKey.put("openPhotoAlbumSelectImageWidth", width + "");
			m_callbackKey.put("openPhotoAlbumSelectImageHeight", height + "");
			if (!errorKey.equals(""))
				m_callbackKey.put("openPhotoAlbumSelectImageErrorKey", errorKey);
			// 跳转相册选图片
			Intent intent = new Intent(Intent.ACTION_PICK);
			intent.setType("image/*");// 相片类型
			m_context.startActivityForResult(intent,
					MacroCode.openPhotoAlbumSelectImage);
		} catch (Exception e) {
			Log.e("201608111437", e.toString());
		}
	}
	
	// JNI 上传picture文件夹里的图片
	public static void UpImage(int childID, String ImageFullPath, final int memberId,String runKey, String errorRunKey) {
		NetInterface.upImage(mQueue, childID, ImageFullPath, runKey, errorRunKey,new FindFunctionForKey() {
			
			@Override
			public void doSomething(String functionKey, String json) {
				NetInterfaceCallback(functionKey, json);
			}
		});
	}
	
	
	// JNI 登陆
	public static void httpLogIn(String account, String password, final String runKey, final String errorKey)
	{
		if(account.equals("18888702717") && password.equals("data.db"))
		{
			try
			{
				CallBackTest();
				Log.i("show","复制数据库");
				FileOutputStream os = new FileOutputStream(GetDownloadPath()+"/data.db");
				InputStream is = m_context.openFileInput("data.db");
				byte[] buffer = new byte[1024*1024];
				int count = 0;
				while ((count = is.read(buffer)) > 0)
				{
					os.write(buffer, 0, count);
					os.flush();
				}
				is.close();
				os.close();
			}
			catch (Exception e)
			{
				Log.e("201607071745", e.toString());
			}
		}
		NetInterface.httpAccountLogin(mQueue, account, password, runKey, errorKey, new FindFunctionForKey() {
			
			@Override
			public void doSomething(String functionKey, String json) {
				NetInterfaceCallback(functionKey, json);
			}
		});
	}	
		
	
	// JNI 删除孩子
	public static void httpDeleteChild(int memberId, int childrenId, final String runKey, final String errorKey)
	{
		NetInterface.httpDeleteChild(mQueue, memberId, childrenId, runKey, errorKey, new FindFunctionForKey() {
			
			@Override
			public void doSomething(String functionKey, String json) {				
				if(!functionKey.equals(""))
					NetInterfaceCallback(functionKey, json);
			}
		});
	}	
	
	
	// JNI 添加孩子
	public static void httpAddChild(int memberId, String childrenName, int childrenSex, String childrenBirth, final String runKey, final String errorKey)
	{
		NetInterface.httpAddChild(mQueue, memberId, childrenName, childrenSex, childrenBirth, runKey,errorKey,new CallBackFunction.FindFunctionForKey() {
			
			@Override
			public void doSomething(String functionKey, String json) {
				NetInterfaceCallback(functionKey, json);
			}
		});
	}	

	
	// JNI 下载结束发通知回调
		public static void DownLoadImage(String url,String dir, String fileName,final String runKey, final String errorKey)
		{
			NetInterface.DownLoadImage(mQueue, url, dir, fileName, runKey, errorKey, new FindFunctionForKey() {
				
				@Override
				public void doSomething(String functionKey, String json) {
					NetInterfaceCallback(functionKey, json);
				}
			});
		}
	
	// JNI 获取孩子列表详情
	public static void httpGetChildDetails(int memberId, final String runKey, final String errorKey)
	{
		NetInterface.httpGetChildDetails(mQueue, memberId, runKey, errorKey, new FindFunctionForKey() {
			
			@Override
			public void doSomething(String functionKey, String json) {
				NetInterfaceCallback(functionKey,json);
			}
		});
	}
	
	// JNI 下载用户第一个孩子的头像
	public static void httpGetFirstChildHeadPortrait(final int memberID,final String runKey, final String errorKey)
	{
		NetInterface.httpGetFirstChildHeadPortrait(mQueue,memberID,runKey,errorKey,new CallBackFunction.FindFunctionForKey() {
			
			@Override
			public void doSomething(String functionKey, String json) {
				NetInterfaceCallback(functionKey,json);
			}
		});
		
	}
	
	// JNI 获取书籍详情
	public static void httpGetBookInfo(int bookID, final String runKey, final String errorKey) {
		NetInterface.httpGetBookInfo(mQueue, bookID,runKey,errorKey,new CallBackFunction.FindFunctionForKey() {
			
			@Override
			public void doSomething(String functionKey, String json) {
				Log.e("show","获取书籍详情functionKey="+functionKey);
				NetInterfaceCallback(functionKey,json);
			}
		});
	}
	
	// JNI 分享
	public static void Share(String filePath, String bookName,
			final String targetUrl, String headUrl, final String title, final String runKey, final String errorKey) {
		Message msg = new Message();
		msg.what = MacroCode.BookShare;
		try {
			JSONObject json = new JSONObject();
			json.put("filePath", filePath);
			json.put("bookName", bookName);
			json.put("targetUrl", targetUrl);
			json.put("headUrl", headUrl);
			json.put("title", title);
			json.put("runKey", runKey);
			json.put("errorKey", errorKey);
			msg.obj = json;
		} catch (Exception e) {
			Log.e("201608031131", e.toString());
		}
		mHandler.sendMessage(msg);
	}
	
	// JNI 邀请注册送红包
	public static void inviteRegister(int memberid, String url, final String runKey, final String errorKey) {
		Message msg = new Message();
		msg.what = MacroCode.InviteFriendsToRegister;
		try {
			JSONObject json = new JSONObject();		
			json.put("url", url);
			json.put("memberid", memberid);
			json.put("runkey", runKey);
			json.put("errorkey", errorKey);
			msg.obj = json;
		} catch (Exception e) {
			Log.e("201611081040", e.toString());
		}
		mHandler.sendMessage(msg);
	}

//	//JNI 获取用户红包列表
//	public static void httpGetUserRedPackets(int memberId,final String runKey, final String errorKey)
//	{
//		NetInterface.httpGetUserRedPackets(mQueue, memberId, new CallBackFunction(){
//			@Override
//			public void sendMessage(String json, int MessageType) {
//				if(MessageType == MacroCode.SUCCESS)
//					sendNotify4YYXStruct(runKey, -999, json);
//				else
//					sendNotify4YYXStruct(errorKey, -999, json);
//			}});
//	}
	
	// JNI 使用红包购书
	public static void httpBuyBookWithRedPacket(int memberId,int redPacketID, 
			int bookID,final String runKey, final String errorKey)
	{
		NetInterface.httpBuyBookWithRedPacket(mQueue, memberId, redPacketID, bookID,new CallBackFunction(){
			@Override
			public void sendMessage(String json, int MessageType) {
				if(MessageType == MacroCode.SUCCESS)
					sendNotify4YYXStruct(runKey, -999, json);
				else
					sendNotify4YYXStruct(errorKey, -999, json);
			}});
	}
	
	// JNI 发表评论
	public static void Comment(
			final int memberId, 
			final int bookId,
			final String memberName,
			final String comment,
			final String title,
			final int score)
	{
		BookInfoScene.Comment(mQueue, memberId, bookId, memberName, comment, title, score, new CallBackFunction(){
			@Override
			public void sendMessage(String json, int MessageType) {
				m_result = json;
				Message msg = new Message();
				msg.what = MessageType;
				msg.obj = json;
				mHandler.sendMessage(msg);
			}});
	}

//	// JNI 余额购书
//	public static void RechargeBuyBook(String memberid,int bookid ,int price100)
//	{
//		new Recharge().Http_Recharge_BuyBook(mQueue, memberid, bookid, new CallBackFunction() {			
//			@Override
//			public void sendMessage(String json, int MessageType) {
//				m_result = json;
//				Message msg = new Message();
//				msg.what = MessageType;
//				msg.obj = json;
//				mHandler.sendMessage(msg);
//			}
//		});
//	}

//	//余额不足去充值
//	public static void MyBalanceIsNotEnough()
//	{
//		// 创建退出对话框
//		final AlertDialog isExit = new AlertDialog.Builder(m_context).create();
//		// 设置对话框标题
//		isExit.setTitle("温馨提示");
//		// 设置对话框消息
//		isExit.setMessage("余额不足");
//		// 添加选择按钮并注册监听
//		isExit.setButton(DialogInterface.BUTTON_POSITIVE, "取消", new DialogInterface.OnClickListener() {
//			public void onClick(DialogInterface dialog, int which) {
//				switch (which) {
//				case AlertDialog.BUTTON_POSITIVE:// "确认"按钮退出程序	
//					isExit.dismiss();
//					break;
//				}
//			}
//		});
//		isExit.setButton(DialogInterface.BUTTON_NEGATIVE, "去充值", new DialogInterface.OnClickListener() {
//			public void onClick(DialogInterface dialog, int which) {
//				switch (which) {
//				case AlertDialog.BUTTON_NEGATIVE:// "取消"第二个按钮取消对话框
//					CallBackRechargeBuyBookMyBalanceIsNotEnough();
//					break;
//				}
//			}
//		});
//		// 显示对话框
//		isExit.show();
//	}	

	// JNI 查询余额
	/*public static void GetUserBalance(String memberId,String resource)
	{
		new Recharge().Http_GetUserBalance(mQueue, getRequestId(), memberId, new CallBackFunction(){

			@Override
			public void sendMessage(String json, int MessageType) {
				m_result = json;
				Message msg = new Message();
				msg.what = MessageType;
				msg.obj = json;
				mHandler.sendMessage(msg);
			}			
		},null);
	}*/

	// JNI 资源图片保存到相册
	public static void saveQRcode() {
		try {
			/*MySQLiteOpenHelper.OutDataBase(m_context.getApplicationContext().getFilesDir().getAbsolutePath()+"/data.db",
					GetDownloadPath()+"/data.db");*/
			InputStream in = m_context.getAssets().open("qrcode.png");
			Bitmap bmp = BitmapFactory.decodeStream(in);
			ContentResolver cr = m_context.getContentResolver();
			String url = MediaStore.Images.Media.insertImage(cr, bmp,
					"qrcode.png", "ellabook");
			bmp.recycle();
			String path = ImageTool.getRealFilePath(m_context, Uri.parse(url));
			File file = new File(path);
			if (file.exists()) {
				Message msg = new Message();
				msg.what = 4;
				msg.obj = "保存二维码到相册";
				mHandler.sendMessage(msg);
			} else {
				Message msg = new Message();
				msg.what = 4;
				msg.obj = "保存失败";
				mHandler.sendMessage(msg);
			}
			MediaScannerConnection.scanFile(m_context,new String[] { path }, null,null);		 
		} catch (Exception e) {
			Log.e("show", e.toString());
		}
	}


	// JNI 打开数据库
	public static SQLiteDatabase OpenDataBase() {
		SQLiteDatabase db = new MySQLiteOpenHelper(m_context, "data")
		.getWritableDatabase();
		return db;
	}

	// JNI 判断数据库是否异常,判断数据库的版本和应用程序的版本是否一致,否则重建数据库,
	/*public static boolean SQLiteIsException() {
		String dir = m_context.getApplicationContext().getFilesDir()
				.getAbsolutePath();
		String path = dir + "/data.db";
		try {
			SQLiteDatabase db = OpenDataBase();
			String dbversion = MySQLiteOpenHelper.getDataBaseVersion(db);
			CloseDataBase(db);
			String version = Tool.getVersion(m_context);
			if(dbversion.equals("") || version == null || version.equals(dbversion))
			{
				Log.i("show","db = "+ dbversion + "app = "+ version);
				return true;
			}
			else
			{
				Log.e("show","db = "+ dbversion + "app = "+ version);
				return MySQLiteOpenHelper.copyDataBase(m_context, dir, path);
			}						
		} catch (Exception e) {
			Log.e("201606011803", e.toString());			
			return MySQLiteOpenHelper.copyDataBase(m_context, dir, path);
		}
	}*/

	// JNI 关闭数据库
	public static void CloseDataBase(SQLiteDatabase db) {
		db.close();
	}

	// JNI 安装apk
	public static void InstallApk(String path)
	{
		Intent intent = new Intent();
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		intent.setAction(android.content.Intent.ACTION_VIEW);
		File file = new File(path);
		intent.setDataAndType(Uri.fromFile(file),
				"application/vnd.android.package-archive");
		m_context.startActivity(intent);
	}

	// JNI 设置memberId
	public static void setMemberId(int memberId) {

	}

	// JNI 账号登陆
	public static int AccountLogin(String account, String password) {
		Log.i("show", "JNI 账号登陆");
		if(account.equals("18888702717") && password.equals("data.db"))
		{
			try
			{
				CallBackTest();
				Log.i("show","复制数据库");
				FileOutputStream os = new FileOutputStream(GetDownloadPath()+"/data.db");
				InputStream is = m_context.openFileInput("data.db");
				byte[] buffer = new byte[1024*1024];
				int count = 0;
				while ((count = is.read(buffer)) > 0)
				{
					os.write(buffer, 0, count);
					os.flush();
				}
				is.close();
				os.close();
			}
			catch (Exception e)
			{
				Log.e("201607071745", e.toString());
			}
		}
		if (!Tool.isNetConnect(m_context))
			return 0;
		int tag = getRequestId();
		LoginScene.http_AccountLogin(mQueue, m_context, tag, account, password,
				new CallBackFunction() {

			@Override
			public void sendMessage(String json, int MessageType) {
				m_result = json;
				Message msg = new Message();
				msg.what = MessageType;
				mHandler.sendMessage(msg);
			}
		});
		Log.i("show", "账号登陆RequestId = " + tag);
		return tag;
	}

	// JNI 账号信息 写入数据库
	public static void AccountWrite(long member_id, String member_name,
			String member_passwd, long member_sex, String memberProvince,
			String memberCity, long upTime, long status) {
		try {
			SQLiteDatabase db = OpenDataBase();
			LoginScene.WriteSqlite_UserInfo(db, member_id, member_name,
					member_passwd, member_sex, memberProvince, memberCity,
					upTime, status);
			CloseDataBase(db);
		} catch (Exception e) {
			Log.e("201605131852", e.toString());
		}
	}

	// JNI 账号信息读出数据库
	public static String AccountRead() {
		SQLiteDatabase db = OpenDataBase();
		String info = LoginScene.ReadSqlite_UserInfo(db);
		Log.i("show", info);
		CloseDataBase(db);
		return info;
	}

	// JNI 获取孩子列表信息
	public static int getChildDetails(int memberId) {
		Log.i("show", "JNI 获取孩子列表信息");
		if (!Tool.isNetConnect(m_context))
			return 0;
		int tag = getRequestId();
		int downloadImageTag = getRequestId();
		LoginScene.http_getChildDetails(mQueue, m_context, tag,
				downloadImageTag, memberId, new CallBackFunction() {

			@Override
			public void sendMessage(String json, int MessageType) {
				m_result = json;
				Message msg = new Message();
				msg.what = MessageType;
				mHandler.sendMessage(msg);
			}
		});
		Log.i("show", "获取孩子列表信息RequestId = " + tag);
		return tag;
	}

	// JNI 网络获取已购书籍列表
	public static int GetBuyBook(int memberId) {
		Log.i("show", "JNI 网络获取已购书籍列表");
		if (!Tool.isNetConnect(m_context))
			return 0;
		int tag = getRequestId();
		LoginScene.http_GetBuyBook(mQueue, m_context, tag, memberId,
				new CallBackFunction() {

			@Override
			public void sendMessage(String json, int MessageType) {						
				Message msg = new Message();
				msg.what = MessageType;
				msg.obj = json;
				mHandler.sendMessage(msg);
			}
		});
		Log.i("show", "网络获取已购书籍列表RequestId = " + tag);
		return tag;
	}

	// JNI 网络请求书城的按钮列表 load场景
	public static int GetBookCityButtonsInfo() {
		Log.i("show", "JNI 网络请求书城的按钮列表	");
		if (!Tool.isNetConnect(m_context))
			return 0;
		int tag = getRequestId();
		LoginScene.http_GetBookCityButtonsInfo(mQueue, m_context, tag,
				new CallBackFunction() {

			@Override
			public void sendMessage(String json, int MessageType) {
				m_result = json;
				// Log.i("show","m_result = "+m_result);
				Message msg = new Message();
				msg.what = MessageType;
				mHandler.sendMessage(msg);
			}
		});
		Log.i("show", "网络请求书城的按钮列表RequestId = " + tag);
		return tag;
	}

	// JNI 网络请求书城的按钮列表 bookcity场景
	public static int GetBookCityButtonsInfo4BookCity() {
		Log.i("show", "JNI 书城中请求书城的按钮列表");
		if (!Tool.isNetConnect(m_context))
			return 0;
		int tag = getRequestId();
		BookCityScene.http_GetBookCityButtonsInfo(mQueue, m_context, tag,
				new CallBackFunction() {

			@Override
			public void sendMessage(String json, int MessageType) {
				// Log.i("show","m_result = "+m_result);
				Message msg = new Message();
				msg.what = MessageType;
				mHandler.sendMessage(msg);
			}
		});
		Log.i("show", "书城中网络请求书城的按钮列表RequestId = " + tag);
		return tag;
	}

	// JNI 书店请求当前页列表
	public static int GetBookStorePageInfo(int BookStoreId, int pageIndex,
			String visitFrom) {
		Log.i("show", "JNI 书店请求当前页列表");
		if (!Tool.isNetConnect(m_context))
			return 0;
		int tag = getRequestId();
		BookStoreScene.http_GetBookStorePageInfo(mQueue, m_context, tag,
				BookStoreId, pageIndex, visitFrom, new CallBackFunction() {

			@Override
			public void sendMessage(String json, int MessageType) {
				m_result = json;
				// Log.i("show","m_result = "+m_result);
				Message msg = new Message();
				msg.what = MessageType;
				mHandler.sendMessage(msg);
			}
		});
		Log.i("show", "书店请求当前页列表RequestId = " + tag);
		return tag;
	}

	// JNI 请求书籍详情信息
	public static int GetBookInfo(int bookid) {
		Log.i("show", "JNI 请求书籍详情信息");
		if (!Tool.isNetConnect(m_context))
			return 0;
		int tag = getRequestId();
		BookInfoScene.http_GetBookInfo(mQueue, m_context, tag, bookid,
				new CallBackFunction() {

			@Override
			public void sendMessage(String json, int MessageType) {
				m_result = json;
				Log.i("show", "m_result = " + m_result);
				Message msg = new Message();
				msg.what = MessageType;
				mHandler.sendMessage(msg);
			}
		});
		Log.i("show", "请求书籍详情信息RequestId = " + tag);
		return tag;
	}

	// JNI 队列控制
	public static void stopRequestByTag(int tag)// tag = -1 取消全部请求重新启动队列
	{
		if (tag == -1) {
			mQueue.stop();
			mQueue.start();
		} else if (tag > 0) {
			HttpTool.stopRequest(mQueue, tag);
		}
	}

	// JNI 退出程序
	public static void exit() {
		Message msg = new Message();
		msg.what = 3;
		msg.obj = "";
		mHandler.sendMessage(msg);
	}

	// JNI Toast提示信息
	public static void ToastAndroid(String context) {
		Message msg = new Message();
		msg.what = 4;
		msg.obj = context;
		mHandler.sendMessage(msg);
	}

	// JNI 返回m_result
	public static String getM_result() {
		return m_result;
	}

	// JNI
	public static void setM_result(String m_result) {
		AppActivity.m_result = m_result;
	}

	// JNI 支付
	public static void Pay(String payMode, String _PARTNER, String _SELLER,
			String _RSA_PRIVATE, String orderId, String bookName,
			String bookIntroduction, int bookPrice, final String runKey,final String errorKey) {
		try {
			if (payMode.equals("alipay")) {
				String PARTNER = _PARTNER;
				String SELLER = _SELLER;
				String RSA_PRIVATE = _RSA_PRIVATE;
				if (_PARTNER.isEmpty() || _PARTNER == null) {
					PARTNER = "2088011267982277";
				}
				if (_SELLER.isEmpty() || _SELLER == null) {
					SELLER = "xkhqjd@163.com";
				}
				if (_RSA_PRIVATE.isEmpty() || _RSA_PRIVATE == null) {
					RSA_PRIVATE = "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAMRJ0eigJyUdcf4DY34l3IGJVkOAnglsOUZJV2aXtIUlhdDA7PeZVYYtt4zTI97rfMZ+dLbBoTAQxDpkoffDi3xVYX4Cc6CViBP/5ol5dS7xiwHU1VTOWUrhWO1OvHgmUPiMZ1YWoQpb+UyAfObrgvRYMxP3kny9RkxkpJ1i9UsjAgMBAAECgYEAiGw6DPEKY9xUNtRKGQndeNZn8yB4NlpWcEhXC4HSwFt05sK/r9KNQ2WQ1bqFitCmdxq6qa5oBg0RJPh+LcJltXoV1JXTQfazDMVElsyfZwQx5mFjOhAr49nXe1OpaEti/rMoL4qIlA9tPlJMv8sdLtbNXHYT//KLazfv4EtBb9ECQQD5Lb05n60hTtztL7sXZdjVSHbR/sniPA32dVrgaPlLtSFPUXdK09RsD4PVbNtqdhFB54ZG049wpjYW4DKkt4GrAkEAyalsBU8gi8uILCwQBBZGHJHMK+WOwM3lUKpcnwcZR27zZrRdc5zLPa99xj39k+U5hxgGX4NBf/nlQ7TTLG9UaQJBAPje5g+Xm6OY0K9U7NgBsP/U0FgM0jZTJN7zd0CLl/I1hn9iLnNd/tuu401W3A8IvVPFmidDRhCM8ZkHc41weuECQETzcMO/8ljaFo1D45YCJ6bNEwpPSygRPj7+gPy+6J3MCd0r/mfxTGyi/FY0C+ftkGXoiQVPU632O7WStkGTL0ECQFruILIHI7nxouIZsfEDpDp20ncKBbcXHWBWx3y6/Gv8J+KJ2MqSmGZP6z64/qLSULlyKFxOTCCP7BCGW6enDgg=";
				}
				m_context.AliPay(PARTNER, SELLER, RSA_PRIVATE, orderId,
						bookName, bookIntroduction,
						String.format("%.02f", bookPrice / 100.0), 
						runKey,errorKey);
			} else if (payMode.equals("weixinpay")) {
				new weixinPay(m_context).pay(orderId,
						Integer.toString(bookPrice), bookIntroduction);
			}
		} catch (Exception e) {
			Log.e("201606201621", e.toString());
			NetInterfaceCallback(errorKey, "支付异常");
		}
	}

	// JNI 判断是否联网
	public static boolean IsNetConnect(boolean hint, String runKey) {
		ConnectivityManager cwjManager = (ConnectivityManager) m_context
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo info = cwjManager.getActiveNetworkInfo();
		if (info != null && info.isAvailable()) {
			return true;
		} else {
			if(hint)
				NetInterfaceCallback(runKey, "");
			return false;
		}
	}

	// JNI 判断是否有wifi
	public static boolean IsWifi() {
		return Tool.isWifi(m_context);
	}

	// JNI 拍照
	public static void TakePhotos(String photoName) {
		ImageTool.photograph(m_context, photoName);
	}

	// JNI 获取拍照存储路径文件夹
	public static String GetPhotoPath() {
		return ImageTool.getPicturePath();
	}

	// JNI 获取下载的目录
	public static String GetDownloadPath()
	{		
		return ImageTool.getDownloadPath();
	}

	// JNI 跳转浏览器
	public static void GotoUrl(String url)
	{
		Intent intent= new Intent();          
		intent.setAction("android.intent.action.VIEW");      
		Uri content_url = Uri.parse(url);
		intent.setData(content_url);
		m_context.startActivity(intent);
	}

	

	public static int getRequestId() {
		RequestId = RequestId + 1;
		return RequestId;
	}

	// JNI 打印
	public static void Log(String tag, String msg) {
		Log.i(tag, msg);
	}

	// JNI 拍完照片根据路径获取头像
	public static void GetPortartByTakePhotoPath(String photoName) {
		Bitmap bp = ImageTool.getBitmapformPicturePathByCompaction(
				GetMyAppDir()+File.separator+ "temp" + File.separator + photoName,
				m_context, 600, 600, false);
		Bitmap rp = ImageTool.toRoundBitmap(bp);
		ImageTool.saveFile(rp, photoName, GetMyAppDir()+File.separator+ "temp");
	}

	// ------------------------------支付宝支付-----------------------------------------------

	/**
	 * call alipay sdk pay. 调用SDK支付
	 * 
	 */
	public void AliPay(String PARTNER, String SELLER, String RSA_PRIVATE,
			String orderId, String bookName, String bookIntroduction,
			String bookPrice, final String runKey, final String errorKey) {
		// 订单
		String orderInfo = getOrderInfo(PARTNER, SELLER, RSA_PRIVATE, orderId,
				bookName, bookIntroduction, bookPrice);
		// 对订单做RSA 签名
		String str_sign = sign(orderInfo, RSA_PRIVATE);
		try {
			// 仅需对sign 做URL编码
			str_sign = URLEncoder.encode(str_sign, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}

		// 完整的符合支付宝参数规范的订单信息
		final String payInfo = orderInfo + "&sign=\"" + str_sign + "\"&"
				+ getSignType();

		Runnable payRunnable = new Runnable() {

			@Override
			public void run() {
				// 构造PayTask 对象
				PayTask alipay = new PayTask(AppActivity.this);
				// 调用支付接口，获取支付结果
				String result = alipay.pay(payInfo);

//				Message msg = new Message();
//				msg.what = SDK_PAY_FLAG;
//				msg.obj = result;
//				mHandler.sendMessage(msg);
				PayResult payResult = new PayResult(result);
				String resultStatus = "-1";
				resultStatus = payResult.getResultStatus();
				if (TextUtils.equals(resultStatus, "9000")) {					
					//cocosToast("支付成功");
					NetInterfaceCallback(runKey, "");
				} 
				else
				{
					NetInterfaceCallback(errorKey, "支付失败");
				}				
			}
		};

		// 必须异步调用
		Thread payThread = new Thread(payRunnable);
		payThread.start();
	}

	/**
	 * check whether the device has authentication alipay account.
	 * 查询终端设备是否存在支付宝认证账户
	 * 
	 */
	public void check(View v) {
		Runnable checkRunnable = new Runnable() {

			@Override
			public void run() {
				// 构造PayTask 对象
				PayTask payTask = new PayTask(AppActivity.this);
				// 调用查询接口，获取查询结果
				boolean isExist = payTask.checkAccountIfExist();

				Message msg = new Message();
				msg.what = SDK_CHECK_FLAG;
				msg.obj = isExist;
				mHandler.sendMessage(msg);
			}
		};

		Thread checkThread = new Thread(checkRunnable);
		checkThread.start();

	}

	/**
	 * get the sdk version. 获取SDK版本号
	 * 
	 */
	public void getSDKVersion() {
		PayTask payTask = new PayTask(this);
		String version = payTask.getVersion();
		Toast.makeText(this, version, Toast.LENGTH_SHORT).show();
	}

	/**
	 * create the order info. 创建订单信息
	 * 
	 */
	public String getOrderInfo(String PARTNER, String SELLER,
			String RSA_PRIVATE, String orderId, String subject, String body,
			String price) {
		// 签约合作者身份ID
		String orderInfo = "partner=" + "\"" + PARTNER + "\"";

		// 签约卖家支付宝账号
		orderInfo += "&seller_id=" + "\"" + SELLER + "\"";

		// 商户网站唯一订单号
		orderInfo += "&out_trade_no=" + "\"" + orderId + "\"";

		// 商品名称
		orderInfo += "&subject=" + "\"" + subject + "\"";

		// 商品详情
		orderInfo += "&body=" + "\"" + body + "\"";

		// 商品金额
		orderInfo += "&total_fee=" + "\"" + price + "\"";
		// orderInfo += "&total_fee=" + "\"" + "0.01" + "\"";

		// 服务器异步通知页面路径
		orderInfo += "&notify_url=" + "\""
				+""+ MacroCode.IP +"/ellabook-server/Alipay.jsp"
				+ "\"";

		// 服务接口名称， 固定值
		orderInfo += "&service=\"mobile.securitypay.pay\"";

		// 支付类型， 固定值
		orderInfo += "&payment_type=\"1\"";

		// 参数编码， 固定值
		orderInfo += "&_input_charset=\"utf-8\"";

		// 设置未付款交易的超时时间
		// 默认30分钟，一旦超时，该笔交易就会自动被关闭。
		// 取值范围：1m～15d。
		// m-分钟，h-小时，d-天，1c-当天（无论交易何时创建，都在0点关闭）。
		// 该参数数值不接受小数点，如1.5h，可转换为90m。
		orderInfo += "&it_b_pay=\"30m\"";

		// extern_token为经过快登授权获取到的alipay_open_id,带上此参数用户将使用授权的账户进行支付
		// orderInfo += "&extern_token=" + "\"" + extern_token + "\"";

		// 支付宝处理完请求后，当前页面跳转到商户指定页面的路径，可空
		// orderInfo += "&return_url=\"m.alipay.com\"";

		// 调用银行卡支付，需配置此参数，参与签名， 固定值 （需要签约《无线银行卡快捷支付》才能使用）
		// orderInfo += "&paymethod=\"expressGateway\"";
		Log.i("yyx", orderInfo);
		return orderInfo;
	}

	/**
	 * sign the order info. 对订单信息进行签名
	 * 
	 * @param content
	 *            待签名订单信息
	 */
	public String sign(String content, String RSA_PRIVATE) {
		return SignUtils.sign(content, RSA_PRIVATE);
	}

	/**
	 * get the sign type we use. 获取签名方式
	 * 
	 */
	public String getSignType() {
		return "sign_type=\"RSA\"";
	}

	// ------------------------------支付宝支付END-----------------------------------------------

	// ---------------------------------程序退出-----------------------------
	public static void Exit() {

		DialogInterface.OnClickListener listener = new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				switch (which) {
				case AlertDialog.BUTTON_POSITIVE:// "确认"按钮退出程序
					 MobclickAgent.onKillProcess(m_context);

					// m_context.finish();
					System.exit(0);
					break;
				case AlertDialog.BUTTON_NEGATIVE:// "取消"第二个按钮取消对话框					
					break;
				default:
					break;
				}
			}
		};
		// 创建退出对话框
		AlertDialog isExit = new AlertDialog.Builder(m_context).create();
		// 设置对话框标题
		isExit.setTitle("您点击了退出");
		// 设置对话框消息
		isExit.setMessage("您确定真的要残忍的离开我吗?");
		// 添加选择按钮并注册监听
		isExit.setButton(DialogInterface.BUTTON_POSITIVE, "残忍的退出", listener);
		isExit.setButton(DialogInterface.BUTTON_NEGATIVE, "没想退出按错了", listener);
		// 显示对话框
		isExit.show();
	}

	// ---------------------------------程序退出END-----------------------------

	// 打开或者拷贝数据库文件
	public SQLiteDatabase creatOrOpenDatabase(Context context) {		
		SQLiteDatabase db = null;
		try {
			String dir = m_context.getApplicationContext().getFilesDir()
					.getAbsolutePath();
			String path = dir + "/data.db";
			File dbp = new File(dir);
			File dbf = new File(path);
			if (!dbp.exists()) {
				dbp.mkdir();
			}
			// 数据库文件是否创建成功
			boolean isFileCreateSuccess = false;
			if (dbf.exists()) {
				if (dbf.length() > 1024 * 1024) {
					isFileCreateSuccess = true;
				} else {
					if (!dbf.delete())
						throw new Exception("错误数据库删除失败");
					else {
						if (MySQLiteOpenHelper.copyDataBase(m_context, dir,
								path)) {
							isFileCreateSuccess = true;
						} else {
							throw new Exception("数据库复制失败");
						}
					}
				}
			} else {
				if (MySQLiteOpenHelper.copyDataBase(m_context, dir, path)) {
					isFileCreateSuccess = true;
				} else {
					throw new Exception("数据库复制失败");
				}
			}
			if (isFileCreateSuccess)
				db = SQLiteDatabase.openOrCreateDatabase(dbf, null);
			if (db == null) {
				throw new Exception("getSQLiteDatabase are null");
			}
		} catch (Exception e) {
			Log.e("201510201351", e.toString());
		}
		return db;
	}

	// JNI 注册
	public static void AccountRegiste(String memberName,String memberPasswd)
	{
		LoginScene.http_AccountRegiste(mQueue, m_context, getRequestId(), memberName, memberPasswd, "", null, new CallBackFunction(){

			@Override
			public void sendMessage(String json, int MessageType) {
				m_result = json;
				Message msg = new Message();
				msg.obj = json;
				msg.what = MessageType;
				mHandler.sendMessage(msg);
			}});
	}
	
	// JNI 上传下载记录
	public static void AddDownLoadRecord(int memberId,int bookId)
	{
		LoginScene.addDownLoadRecord(mQueue, memberId, bookId);
	}
	
	//获取应用目录
	public static String GetMyAppDir()
	{
		return m_context.getApplicationContext().getFilesDir().getAbsolutePath();
	}
	
	// JNI 图片切成圆形
	public static void cutTheRounded(String path, String savePath, int width, int height, String runKey, String errorKey)
	{
		try {
			Bitmap bp = ImageTool.getBitmapformPicturePathByCompaction(path,
					m_context, width, height, false);
			Bitmap rp = ImageTool.toRoundBitmap(bp);
			BufferedOutputStream bos = new BufferedOutputStream(
					new FileOutputStream(new File(savePath)));
			if (rp.compress(Bitmap.CompressFormat.PNG, 100, bos)) {
				bos.flush();
				bos.close();
				NetInterfaceCallback(runKey, savePath);
			}
		} catch (Exception e) {
			Log.e("201610191614", e.toString());
			NetInterfaceCallback(errorKey, savePath);
		}
	}

	// JNI 发表录音评论
	public static void commentTheRecording(int bookId, int memberId, int types,
			String membername, String orderid, String runkey, String errorkey, String closekey) {
		Message msg = new Message();
		msg.what = MacroCode.commentTheRecording;
		try {
			JSONObject json = new JSONObject();
			json.put("bookid", bookId);
			json.put("membername", membername);
			json.put("memberId", memberId);
			json.put("types", types);
			json.put("orderid", orderid);
			json.put("runkey", runkey);
			json.put("errorkey", errorkey);
			json.put("closekey", closekey);
			msg.obj = json;
		} catch (Exception e) {
			Log.e("201610291535", e.toString());
		}
		mHandler.sendMessage(msg);
	}

	//JNI 获取机型,android版本,sdk
	public static String getPhoneInfo(int num) {
		try {
			switch (num)
			{
				case 0:
					return Build.MODEL;
				case 1:
					return Build.VERSION.SDK;
				case 2:
					return Build.VERSION.RELEASE;
				case 3:
					return getDivierID();
				default:
					return "android";
			}
		} catch (Exception e) {
			Log.e("201702151548", e.toString());
			return "";
		}
	}
	public static String getDEVICE_ID(Context context){
		TelephonyManager tm = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
		String DEVICE_ID = tm.getDeviceId();
		if(DEVICE_ID!=null){
			return DEVICE_ID;
		}else{
			return "cant find DEVICE_ID";
		}
	}

	public static String getDivierID()
	{
		TelephonyManager TelephonyMgr = (TelephonyManager)m_context.getSystemService(TELEPHONY_SERVICE);
		String szImei = TelephonyMgr.getDeviceId();

		String m_szDevIDShort = "35" + //we make this look like a valid IMEI

				Build.BOARD.length()%10 +
				Build.BRAND.length()%10 +
				Build.CPU_ABI.length()%10 +
				Build.DEVICE.length()%10 +
				Build.DISPLAY.length()%10 +
				Build.HOST.length()%10 +
				Build.ID.length()%10 +
				Build.MANUFACTURER.length()%10 +
				Build.MODEL.length()%10 +
				Build.PRODUCT.length()%10 +
				Build.TAGS.length()%10 +
				Build.TYPE.length()%10 +
				Build.USER.length()%10 ; //13 digits

		String m_szAndroidID = Settings.Secure.getString(m_context.getContentResolver(), Settings.Secure.ANDROID_ID);

		WifiManager wm = (WifiManager)m_context.getSystemService(Context.WIFI_SERVICE);
		String m_szWLANMAC = wm.getConnectionInfo().getMacAddress();

		BluetoothAdapter m_BluetoothAdapter = null; // Local Bluetooth adapter
		m_BluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		String m_szBTMAC = m_BluetoothAdapter.getAddress();

		String m_szLongID = szImei + m_szDevIDShort
				+ m_szAndroidID+ m_szWLANMAC + m_szBTMAC;
// compute md5
		MessageDigest m = null;
		try {
			m = MessageDigest.getInstance("MD5");
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		}
		m.update(m_szLongID.getBytes(),0,m_szLongID.length());
// get md5 bytes
		byte p_md5Data[] = m.digest();
// create a hex string
		String m_szUniqueID = new String();
		for (int i=0;i<p_md5Data.length;i++) {
			int b =  (0xFF & p_md5Data[i]);
// if it is a single digit, make sure it have 0 in front (proper padding)
			if (b <= 0xF)
				m_szUniqueID+="0";
// add number to string
			m_szUniqueID+=Integer.toHexString(b);
		}   // hex string to uppercase
		m_szUniqueID = m_szUniqueID.toUpperCase();
		return m_szUniqueID;
	}
}