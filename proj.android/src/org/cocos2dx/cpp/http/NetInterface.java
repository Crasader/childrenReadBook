package org.cocos2dx.cpp.http;

import java.io.File;
import java.util.HashMap;
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
import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.ImageTool;
import org.cocos2dx.cpp.MacroCode;
import org.cocos2dx.cpp.Tool;
import org.cocos2dx.cpp.http.CallBackFunction.DownLoadImage;
import org.cocos2dx.cpp.http.CallBackFunction.FindFunctionForKey;
import org.cocos2dx.cpp.sqlite.MySQLiteOpenHelper;
import org.cocos2dx.cpp.weixinPay;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.ContentValues;
import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.util.Log;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.Request.Method;
import com.android.volley.Response.ErrorListener;
import com.android.volley.Response.Listener;
import com.android.volley.toolbox.ImageRequest;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.StringRequest;

public class NetInterface {

	// 充值订单
	public static void httpGetOrderId(final RequestQueue mQueue,
			int memberid,
			final int rechargeCount,// 充值额度
			final int price100, // 支付额度
			final String payType, final String payInfo, final String runKey,
			final String errorKey) {
		if (memberid == -999)
			return;
		String url = MacroCode.IP + MacroCode.NET_GETRECHARGEORDERID;
		Map<String, String> paramter = new HashMap<String, String>();
		paramter.put("Recharge", Double.toString(rechargeCount / 100.0));
		paramter.put("memberId", memberid + "");
		paramter.put("rechargePlatformName", payType);
		paramter.put("rechargePlatformType", "0");
		paramter.put("resource", "android");
		Log.e("show", url + paramter.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url,
				paramter, new Listener<JSONObject>() {

					@Override
					public void onResponse(JSONObject response) {
						try {
							Log.e("show", response.toString());
							boolean result = response.getBoolean("result");
							if (result) {
								int order_id = response.getInt("order_id");
								//TODO 
								//测试支付"yyx"+order_id 
								//正式支付""+order_id
								AppActivity.Pay(payType, "", "", "",
										""+order_id, payInfo,
										payInfo, price100, runKey, errorKey);
							} else
								AppActivity.NetInterfaceCallback(errorKey,
										"充值异常");
						} catch (Exception e) {
							Log.e("201606241550", e.toString());
							AppActivity.NetInterfaceCallback(errorKey, "充值异常");
						}
					}
				}, new ErrorListener() {

					@Override
					public void onErrorResponse(VolleyError error) {
						AppActivity.NetInterfaceCallback(errorKey, "网络错误");
					}
				});
		mQueue.add(request);
	}
	//{"result":true,"toURL":"","code":"","out_trade_no":"VIP4a873f4a391481619220","data":"","errorMessage":"","id":""}
	// VIP充值订单
	public static void httpGetVIPOrderId(final RequestQueue mQueue,
	                                  int memberid,
	                                  final int rechargeCount,// 充值额度
	                                  final int price100, // 支付额度
	                                  final String payType, final String payInfo, final String runKey,
	                                  final String errorKey) {
		if (memberid == -999)
			return;
		String url = MacroCode.IP + MacroCode.NET_GETVIPRECHARGEORDERID;
		Map<String, String> paramter = new HashMap<String, String>();
		paramter.put("price", String.valueOf(price100));
		paramter.put("memberId", memberid + "");
		if (payType.equals("alipay")) {
			paramter.put("platform_id", "2");
		} else if (payType.equals("weixinpay")) {
			paramter.put("platform_id", "3");
		}

		Log.e("show", url + paramter.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url,
				paramter, new Listener<JSONObject>() {

			@Override
			public void onResponse(JSONObject response) {
				try {
					Log.e("show", response.toString());
					boolean result = response.getBoolean("result");
					if (result) {
						String order_id = response.getString("out_trade_no");
						//TODO
						//测试支付"yyx"+order_id
						//正式支付""+order_id
						AppActivity.Pay(payType, "", "", "",
								""+order_id, payInfo,
								payInfo, price100, runKey, errorKey);
					} else
						AppActivity.NetInterfaceCallback(errorKey,
								"充值异常");
				} catch (Exception e) {
					Log.e("201606241550", e.toString());
					AppActivity.NetInterfaceCallback(errorKey, "充值异常");
				}
			}
		}, new ErrorListener() {

			@Override
			public void onErrorResponse(VolleyError error) {
				AppActivity.NetInterfaceCallback(errorKey, "网络错误");
			}
		});
		mQueue.add(request);
	}
	
	//获取分享成功的红包
	public static void httpShareWithCoupon(RequestQueue mQueue, final int memberid, final String runKey, final String errorKey, final FindFunctionForKey callback){
		String url = MacroCode.IP + "/ellabook-server/coupon_samuel/ShareCoupon?memberId="+memberid+"&resource=android ";
		Log.e("show", url);		
		StringRequest request = new StringRequest(Method.GET, url,new Response.Listener<String>() {
			@Override
			public void onResponse(String response) {
				Log.e("show", response);
				callback.doSomething(runKey, response);
			}
		}, new Response.ErrorListener() {
			@Override
			public void onErrorResponse(VolleyError error) {
				Log.e("201608291757", error.toString());
				callback.doSomething(errorKey, "");
			}
		});
		mQueue.add(request);
	}
	
	//获取书店当前页的书籍列表信息
	public static void httpBookStoreSceneCurrentlyPageBookListInfo(RequestQueue mQueue, int memberID, int BookStoreId, int pageIndex, String visitFrom, 
			final String runKey, final String errorKey, final FindFunctionForKey callback)
	{
		String url = MacroCode.IP + MacroCode.NET_BOOKSID + BookStoreId
				+ "?page=" + pageIndex + "&pagesize=8" + "&resource="
				+ MacroCode.RESOURCE + "&memberId=" + memberID + "&visitFrom="
				+ visitFrom;
		Log.e("show", url);
		StringRequest request = new StringRequest(Method.GET, url,
				new Listener<String>() {
					@Override
					public void onResponse(String response) {
						Log.e("show", response.toString());
						callback.doSomething(runKey, response.toString());
					}

				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201608161705", error.toString());
						callback.doSomething(errorKey, "");
					}
				});
		mQueue.add(request);
	}
	
	// 网络获取已购书籍列表
	public static void httpGetUserBuyBooks(RequestQueue mQueue,
			final AppActivity context,final int memberId,
			final String runKey, final String errorKey,
			final FindFunctionForKey callback) {
		// "memberId=").append(App::getMemberID()).append("&page=1&pagesize=1000&resource=").append(App::m_resource);
		String url = MacroCode.IP + MacroCode.NET_BOOKROOMBOOKLIST
				+ "?memberId=" + memberId + "&page=1&pagesize=1000&resource="+ MacroCode.RESOURCE;
		Log.e("show", url);
		JsonObjectRequest request = new JsonObjectRequest(Method.GET, url,
				null, new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						Log.e("show", response.toString());
						callback.doSomething(runKey, response.toString());
						try {
							int code = HttpTool.GetInt(response, "code","201605151456");
							if (code == 0) {					
								JSONArray array = HttpTool.GetJSONArray(response, "data", "201605151457");
								SQLiteDatabase db = new MySQLiteOpenHelper(context, "data").getWritableDatabase();
								for (int i = 0; i < array.length(); i++) {

									JSONObject item = array.getJSONObject(i);
									int member_id = memberId;
									int bookId = HttpTool.GetInt(item,"bookId", "201605151506");
									int orderId = HttpTool.GetInt(item,"orderId", "201605151507");
									String bookCoverUrl = HttpTool.GetString(item, "bookCoverUrl","201605151508");
									String bookPlayUrl = HttpTool.GetString(item, "bookPlayUrl","201605151509");
									long upTime = Tool.getTime() / 1000;
									// 插入数据库
									Log.e("show", "已购书籍列表信息插入数据库" + bookId);
									ContentValues value = new ContentValues();
									value.put("member_id", member_id);
									value.put("bookId", bookId);
									value.put("orderId", orderId);
									value.put("bookCoverUrl", bookCoverUrl);
									value.put("bookPlayUrl", bookPlayUrl);
									value.put("upTime", upTime);
									Tool.WriteSqlite(
											db,
											MacroCode.DB_USERBOOKLIST,
											value,
											"bookId=? and member_id=?",
											new String[] {
													Integer.toString(bookId),
													Integer.toString(member_id) },
											i + 1);
								}
								db.close();
							}
						} catch (Exception e) {
							Log.e("201605151455", e.toString());
						}
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605151446", error.toString());
						callback.doSomething(errorKey, "");
					}
				});
		mQueue.add(request);
	}
	// NET_SAVEREADHISTORY 保存阅读记录
	public static void httpSaveReadRecord(RequestQueue mQueue,
			final AppActivity context,final int memberId,final int childrenId, final int bookId,final String readStartTime,final String readEndTime,
			final String runKey, final String errorKey,
			final FindFunctionForKey callback) {
		// 	string postData = "memberId=" + App::getMemberID() + StringUtils::format("&childrenId=%d", App::GetInstance()->m_read->childrenId) +
		//StringUtils::format("&bookId=%d", App::GetInstance()->m_read->bookId) + "&readStartTime=" + App::GetInstance()->m_read->startTime + 
		//"&readEndTime=" + App::GetStringTime2();
		String url = MacroCode.IP + MacroCode.NET_SAVEREADHISTORY;
		Log.e("show", url);
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("memberId", memberId+"");
		map.put("childrenId", childrenId+"");
		map.put("bookId", bookId+"");
		map.put("readStartTime", readStartTime);
		map.put("readEndTime", readEndTime);
		map.put("resource", MacroCode.RESOURCE);
		Log.e("show",url+map.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
			@Override
			public void onResponse(JSONObject response) {
				Log.e("show", response.toString());
				callback.doSomething(runKey, response.toString());
			}
		}, new Response.ErrorListener() {
			@Override
			public void onErrorResponse(VolleyError error) {
				Log.e("201609081608", error.toString());
				callback.doSomething(errorKey, "");
			}

		});
		mQueue.add(request);
		
	}
	// 查询余额
	public static void httpGetUserBalance(RequestQueue mQueue, int memberId,
			final String runKey, final String errorKey,
			final FindFunctionForKey callback) {
		String url = MacroCode.IP + MacroCode.NET_GETRECHARGE + "?memberId="
				+ memberId + "&resource=" + MacroCode.RESOURCE;
		StringRequest request = new StringRequest(Method.GET, url,
				new Listener<String>() {
					@Override
					public void onResponse(String response) {
						Log.e("show", response.toString());
						callback.doSomething(runKey, response.toString());
					}

				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201608161705", error.toString());
						callback.doSomething(errorKey, "");
					}
				});
		mQueue.add(request);
	}

	//注册
	public static void httpAccountRegiste(RequestQueue mQueue,	String memberName, String memberPasswd,
				final String runKey, final String errorKey,final CallBackFunction.FindFunctionForKey callback)
	{			
//			String url = MacroCode.IP + MacroCode.NET_REGISTER;
//			HashMap<String, String> map = new HashMap<String, String>();
//			map.put("memberName", memberName);
//			map.put("memberPasswd", memberPasswd);
//			map.put("resource", MacroCode.RESOURCE);
//			Log.e("show",url.toString()+map.toString());			
//			JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
//					new Response.Listener<JSONObject>() {
//						@Override
//						public void onResponse(JSONObject response) {
//							Log.e("show",response.toString());
//							callback.doSomething(runKey, response.toString());					
//						}
//					}, new Response.ErrorListener() {
//						@Override
//						public void onErrorResponse(VolleyError error) {
//							Log.e("201608161705", error.toString());
//							callback.doSomething(errorKey, "");
//						}
//
//					});
//			mQueue.add(request);
			String url = MacroCode.IP + MacroCode.NET_REGISTER + "?memberName="+memberName
			+ "&memberPasswd="+memberPasswd+"&resource=" + MacroCode.RESOURCE;
			Log.e("show", url);
			JsonObjectRequest request = new JsonObjectRequest(Method.GET, url,
					null, new Response.Listener<JSONObject>() {
				@Override
				public void onResponse(JSONObject response) {
					Log.e("show", response.toString());
					callback.doSomething(runKey, response.toString());					
				}
	
			}, new Response.ErrorListener() {
				@Override
				public void onErrorResponse(VolleyError error) {
					Log.e("201608261606", error.toString());
					callback.doSomething(errorKey, "");
				}
			});

	mQueue.add(request);
	}
	//修改用户信息
	public static void httpAmendUserInfo(RequestQueue mQueue, int memberId, int sex, String city,String province,
			final String runKey, final String errorKey,final CallBackFunction.FindFunctionForKey callback)
	{
		String url =MacroCode.IP+MacroCode.NET_MODIFY_CITY;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("memberId", memberId+"");
		map.put("sex", sex+"");
		map.put("city", city);
		map.put("province", province);
		map.put("resource", MacroCode.RESOURCE);
		Log.e("show",url+map.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						Log.e("show", response.toString());
						callback.doSomething(runKey, response.toString());
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201608161442", error.toString());
						callback.doSomething(errorKey, "");
					}

				});
		mQueue.add(request);
	}
	
	//修改宝贝信息
	public static void httpAmendBabyInfo(RequestQueue mQueue, int childrenId, String childrenName, int childrenSex, String childrenBirth,
			final String runKey, final String errorKey,final CallBackFunction.FindFunctionForKey callback)
	{
		String url = MacroCode.IP + MacroCode.NET_MODIFY_CHILD_INFO;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("childrenId", childrenId+"");
		map.put("childrenName", childrenName);
		map.put("childrenSex", childrenSex+"");
		map.put("childrenBirth", childrenBirth);
		map.put("resource", MacroCode.RESOURCE);
		Log.e("show",url+map.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						Log.e("show", response.toString());
						callback.doSomething(runKey, response.toString());
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605121029", error.toString());
						callback.doSomething(errorKey, "");
					}

				});
		mQueue.add(request);
	}
	
	// 上传头像
	public static void upImage(RequestQueue mQueue, final int childID, String ImageFullPath,final String runKey, final String errorRunKey, final CallBackFunction.FindFunctionForKey callback) 
	{
		String url = MacroCode.IP + "/ellabook-server/children/setAvatar_android.do";
		HttpClient httpclient = new DefaultHttpClient();
		HttpPost httppost = new HttpPost(url);
		try {
			FileBody pictureStream = new FileBody(new File(ImageFullPath));
			StringBody childrenId = new StringBody(Integer.toString(childID));
			StringBody resource = new StringBody(MacroCode.RESOURCE);
			MultipartEntity reqEntity = new MultipartEntity();
			reqEntity.addPart("pictureStream", pictureStream);
			reqEntity.addPart("childrenId", childrenId);
			reqEntity.addPart("resource", resource);
			httppost.setEntity(reqEntity);
			HttpResponse response = httpclient.execute(httppost);
			HttpEntity resEntity = response.getEntity();
			int statusCode = response.getStatusLine().getStatusCode();
			if (statusCode == HttpStatus.SC_OK) 
			{
				System.out.println("服务器正常响应.....");
				if (resEntity != null) 
				{
					String json = EntityUtils.toString(resEntity, "utf-8");
					int code = -999;
					JSONObject Json = new JSONObject(json);
					code = Json.getInt("code");
					if (code == 0) 
					{
						callback.doSomething(runKey, ImageFullPath);
					} else {
						callback.doSomething(errorRunKey, "");
					}
					if (resEntity != null) 
					{
						resEntity.consumeContent();
					}
					httpclient.getConnectionManager().shutdown();
				}
			}
		} catch (Exception e) {
			Log.e("201606061356", e.toString());
		}
	}
	
	// 账号登录
	public static void httpAccountLogin(RequestQueue mQueue,String account, String password,final String runKey, final String errorKey, final CallBackFunction.FindFunctionForKey callback) {
		String url = MacroCode.IP + MacroCode.NET_LOGIN;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("username", account);
		map.put("password", password);
		map.put("resource", MacroCode.RESOURCE);
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						Log.e("show", response.toString());
						callback.doSomething(runKey, response.toString());
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605121029", error.toString());
						callback.doSomething(errorKey, "");
					}

				});
		mQueue.add(request);
	}
	
	// 删除孩子
	public static void httpDeleteChild(RequestQueue mQueue, int memberId, int childrenId,final String runKey, final String errorKey,final CallBackFunction.FindFunctionForKey callback) {
		String url = MacroCode.IP + MacroCode.NET_DELETECHILD;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("memberId", memberId + "");
		map.put("childrenId", childrenId+"");
		map.put("resource", MacroCode.RESOURCE);
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						Log.e("show", response.toString());
						callback.doSomething(runKey,response.toString());
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605121029", error.toString());
						callback.doSomething(errorKey,"");
					}

				});
		mQueue.add(request);
	}
	
	// 添加孩子
	public static void httpAddChild(RequestQueue mQueue, int memberId, String childrenName, int childrenSex, String childrenBirth,final String runKey, final String errorKey, final CallBackFunction.FindFunctionForKey callback)
	{
		String url = MacroCode.IP + MacroCode.NET_ADDCHILD;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("memberId", memberId + "");
		map.put("childrenName", childrenName);
		map.put("childrenSex", childrenSex + "");
		map.put("childrenBirth", childrenBirth);
		map.put("resource", MacroCode.RESOURCE);
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						Log.e("show", response.toString());
						callback.doSomething(runKey,response.toString());
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605121029", error.toString());
						callback.doSomething(errorKey,"");
					}

				});
		mQueue.add(request);
	}
	
	/*// 下载结束发通知回调
	public static void DownLoadImage(RequestQueue mQueue,String url,String dir, String fileName, final CallBackFunction callback) 
	{
		HttpTool.DownLoadImage(mQueue, url, dir, fileName, callback, null);
	}
	*/
	// 下载
	public static void DownLoadImage(RequestQueue mQueue,String url,final String dir, final String fileName, final String runKey, final String errorKey,final CallBackFunction.FindFunctionForKey callback) 
	{
		try {
			ImageRequest request = new ImageRequest(url,
					new Listener<Bitmap>() {
						@Override
						public void onResponse(Bitmap response) {
							ImageTool.saveFile(response, fileName, dir);
							Log.e("show", dir + "/" + fileName + "下载文件成功");
							callback.doSomething(runKey,dir + "/" + fileName);
						}
					}, 0, 0, Config.ARGB_8888, new ErrorListener() {
						@Override
						public void onErrorResponse(VolleyError error) {
							Log.e("201608041654", error.toString());
							Log.e("show", dir + "/" + fileName + "下载文件失败");;
							callback.doSomething(errorKey,"");
						}
					});
			if (!url.equals("") || !url.equals("null")) {
				mQueue.add(request);
				Log.e("show","开始下载"+url);
			}
		} catch (Exception e) {
			Log.e("201608041653", e.toString());
			callback.doSomething(errorKey,"");
		}
	}
	
	// 网络获取孩子详情
	public static void httpGetChildDetails(RequestQueue mQueue,int memberId, final String runKey, final String errorKey, final CallBackFunction.FindFunctionForKey callback) 
	{
		String url = MacroCode.IP + MacroCode.NET_CHILD_DETAILS;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("memberId", Integer.toString(memberId));
		map.put("resource", MacroCode.RESOURCE);
		Log.e("show",url);
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						Log.e("show",response.toString());
						callback.doSomething(runKey, response.toString());
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201608051140", error.toString());
						callback.doSomething(errorKey, "");
					}
				});
		mQueue.add(request);
	}
	
	// 下载用户第一个孩子的头像
	public static void httpGetFirstChildHeadPortrait(RequestQueue mQueue,final int memberID, final String runKey, final String errorKey,final CallBackFunction.FindFunctionForKey callback) 
	{
		String url = MacroCode.IP + MacroCode.NET_GETFIRSTCHILDHEADPORTRAIT + "?memberId=" + memberID + "&resource=" + MacroCode.RESOURCE;
		Log.i("show", url);
		StringRequest request = new StringRequest(Method.GET, url,
				new Response.Listener<String>() 
				{
					@Override
					public void onResponse(String response) 
					{
						Log.e("show",response);
						callback.doSomething(runKey, response);
					}
						
				}, new Response.ErrorListener() 
				{
					@Override
					public void onErrorResponse(VolleyError error) 
					{
						Log.e("201608041648", error.toString());
						callback.doSomething(errorKey, "");
					}
				});
		mQueue.add(request);
	}

	// 获取书籍详情
	public static void httpGetBookInfo(RequestQueue mQueue,final int BookId,final String runKey, final String errorKey,final CallBackFunction.FindFunctionForKey callback) {
		String url = MacroCode.IP + MacroCode.NET_BOOKINFO + BookId + "?memberId=-999" + "&resource=" + MacroCode.RESOURCE;
		Log.i("show", url);
		StringRequest request = new StringRequest(Method.GET, url,
				new Response.Listener<String>() {
					@Override
					public void onResponse(String response) {
						callback.doSomething(runKey, response);
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201608041005", error.toString());
						callback.doSomething(errorKey, "");
					}
				});
		mQueue.add(request);
	}

	// 获取用户有效红包列表
	public static void httpGetUserRedPackets(RequestQueue mQueue, int memberId,
			final CallBackFunction callback) {
		String url = MacroCode.IP + MacroCode.NET_USERREDPACKET + "?memberId="
				+ memberId + "&resource" + MacroCode.RESOURCE;
		Log.e("show", url);
		StringRequest mStringRequest = new StringRequest(url,
				new Response.Listener<String>() {
					@Override
					public void onResponse(String response) {
						Log.e("show", response);
						callback.sendMessage(response, MacroCode.SUCCESS);
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						callback.sendMessage("", MacroCode.FAIL);
					}
				});
		mQueue.add(mStringRequest);
	}

	// 使用红包购书
	public static void httpBuyBookWithRedPacket(RequestQueue mQueue,
			final int memberId, int redPacketID, int bookID,
			final CallBackFunction callback) {
		String url = MacroCode.IP + MacroCode.NET_BUYBOOKWITHREDPACKET;
		Map<String, String> paramter = new HashMap<String, String>();
		paramter.put("memberId", memberId + "");
		paramter.put("couponId", redPacketID + "");
		paramter.put("bookId", bookID + "");
		paramter.put("resource", MacroCode.RESOURCE);
		paramter.put("chargePlatformType", "0");
		paramter.put("chargePlatformName", MacroCode.RESOURCE);
		Log.e("show", url + paramter.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url,
				paramter, new Listener<JSONObject>() {

					@Override
					public void onResponse(JSONObject response) {
						Log.e("show", response.toString());
						callback.sendMessage(response.toString(),
								MacroCode.SUCCESS);
						AppActivity.GetBuyBook(memberId);
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						callback.sendMessage("", MacroCode.FAIL);
					}
				});
		mQueue.add(request);
	}

	// 余额购书一次性购书
	public void Http_Recharge_BuyBook(final RequestQueue mQueue,
			final String memberid, final int bookid,
			final CallBackFunction callback) {
		if (memberid.equals("-999"))
			return;
		String url = MacroCode.IP + MacroCode.NET_RECHARG_BUYBOOK;
		// Log.e("show", url);
		Map<String, String> paramter = new HashMap<String, String>();
		paramter.put("bookId", Integer.toString(bookid));
		paramter.put("memberId", memberid);
		paramter.put("resource", MacroCode.RESOURCE);
		paramter.put("chargePlatformName", "android");
		paramter.put("chargePlatformType", 0 + "");
		paramter.put("goodsType", 1 + "");
		paramter.put("promotionsId", "");
		paramter.put("orderMessage", "");
		paramter.put("payType", 2 + "");
		// Log.e("show", paramter.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url,
				paramter, new Listener<JSONObject>() {

					@Override
					public void onResponse(JSONObject response) {
						try {
							// "errorMessage":"","balance":1999982,"result":true,
							// Log.e("show", response.toString());
							int code = response.getInt("code");
							if (code == 0) {
								String desc = response.getString("desc");
								callback.sendMessage("购书成功", 4);
								callback.sendMessage("",
										MacroCode.RechargeBuyBookSuccess);
							} else if (code == 1) {
								// 余额不足
								callback.sendMessage(
										"",
										MacroCode.RechargeBuyBookMyBalanceIsNotEnough);
							} else {
								String errorMessage = response
										.getString("errorMessage");
								callback.sendMessage("购书失败", 4);
								callback.sendMessage("",
										MacroCode.RechargeBuyBookFail);
								AppActivity.GetBuyBook(Integer
										.parseInt(memberid));
							}
							// int orderId = response.getInt("orderId");
						} catch (Exception e) {
							Log.e("201606301101", e.toString());
							callback.sendMessage("购书异常", 4);
							callback.sendMessage("",
									MacroCode.RechargeBuyBookFail);
						}
					}
				}, new ErrorListener() {

					@Override
					public void onErrorResponse(VolleyError error) {
						try {
							callback.sendMessage("购书失败", 4);
							callback.sendMessage("",
									MacroCode.RechargeBuyBookFail);
						} catch (Exception e) {
							Log.e("201607011034", e.toString());
						}
					}
				});
		mQueue.add(request);
	}

}
