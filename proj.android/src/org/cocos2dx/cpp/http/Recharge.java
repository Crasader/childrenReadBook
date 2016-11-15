package org.cocos2dx.cpp.http;

import java.util.HashMap;
import java.util.Map;

import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.MacroCode;
import org.cocos2dx.cpp.weixinPay;
import org.cocos2dx.cpp.http.CallBackFunction.DownLoadPortart;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.Context;
import android.util.Log;

import com.android.volley.Request.Method;
import com.android.volley.RequestQueue;
import com.android.volley.Response.ErrorListener;
import com.android.volley.Response.Listener;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;

public class Recharge {
	// 查询余额
	public void Http_GetUserBalance(final RequestQueue mQueue,
			final int requestID, String memberId,
			final CallBackFunction callback,
			final CallBackFunction.DownLoadPortart finishDoSomething) {
		if(memberId.equals("-999"))
			return;
		String url = MacroCode.IP + MacroCode.NET_GETRECHARGE + "?memberId="
				+ memberId + "&resource=" + MacroCode.RESOURCE;
		StringRequest request = new StringRequest(url, new Listener<String>() {

			@Override
			public void onResponse(String response) {
				//Log.e("show", response);
				callback.sendMessage(response, MacroCode.RechargeGetUserBalance);
				try {
					if (finishDoSomething != null)
						finishDoSomething.doSomething(new JSONObject(response));
				} catch (JSONException e) {
					Log.e("201606271407", e.toString());
					callback.sendMessage("异常", 4);
					callback.sendMessage("", MacroCode.RechargeBuyBookFail);
				}
			}
		}, new ErrorListener() {

			@Override
			public void onErrorResponse(VolleyError error) {
				callback.sendMessage("网络异常", 4);
				callback.sendMessage("", MacroCode.RechargeBuyBookFail);
			}
		});
		request.setTag(requestID);
		mQueue.add(request);
	}

	// 充值订单
	public void Http_GetOrderId(final RequestQueue mQueue,
			final Context context, final int requestID, String memberid,
			final int rechargeCount,//充值额度
			final int price100, //支付额度
			final String payType, final String payInfo,
			final CallBackFunction callback) {
		if(memberid.equals("-999"))
			return;
		String url = MacroCode.IP + MacroCode.NET_GETRECHARGEORDERID;
		Map<String, String> paramter = new HashMap<String, String>();
		paramter.put("Recharge", Double.toString(rechargeCount / 100.0));
		paramter.put("memberId", memberid);
		paramter.put("rechargePlatformName", payType);
		paramter.put("rechargePlatformType", "0");
		paramter.put("resource", "android");
		//Log.e("show",paramter.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url,
				paramter, new Listener<JSONObject>() {

					@Override
					public void onResponse(JSONObject response) {						
						try {
							//Log.e("show",response.toString());
							boolean result = response.getBoolean("result");
							if (result) {
								int order_id = response.getInt("order_id");
								//callback.sendMessage("生成订单成功", 4);
								if (payType.equals("weixinpay")) {
									//callback.sendMessage("微信支付", 4);
									new weixinPay(context).pay(
											Integer.toString(order_id),
											Integer.toString(price100), payInfo);
								} else if (payType.equals("alipay")) {
									//callback.sendMessage("支付宝支付", 4);
//									AppActivity.Pay("alipay","","","",Integer.toString(order_id),
//											payInfo,payInfo,String.format("%.02f", price100 / 100.0));
								}
							} else
								//callback.sendMessage("生成订单失败", 4);
								callback.sendMessage("充值失败", 4);
						} catch (Exception e) {
							Log.e("201606241550", e.toString());
						}
					}
				}, new ErrorListener() {

					@Override
					public void onErrorResponse(VolleyError error) {
						//callback.sendMessage("生成订单失败", 4);
						callback.sendMessage("充值失败", 4);
					}
				});
		request.setTag(requestID);
		mQueue.add(request);
	}

	// 余额购书函数
	public void Http_RechargeBuyBook(final RequestQueue mQueue,
			final String memberid, 
			final int bookid,
			final int price100, 
			final CallBackFunction callback) {		
		//callback.sendMessage("查询余额", 4);
		// 查询余额
		if(memberid.equals("-999"))
			return;
		Http_GetUserBalance(mQueue, AppActivity.getRequestId(), memberid,
				new CallBackFunction() {
					@Override
					public void sendMessage(String json, int MessageType) {
						callback.sendMessage(json, MessageType);
					}
				}, new DownLoadPortart() {
					@Override
					public void doSomething(JSONObject json) {
						try {
							boolean result = json.getBoolean("result");
							if (result) {
								JSONObject data = json.getJSONObject("data");
								double balance_amount = data.getDouble("balance_amount");
								// 扣款
								final double balance = balance_amount
										- price100 / 100.0;
								if (balance < 0) {
									callback.sendMessage("余额不足", 4);
									callback.sendMessage("",MacroCode.RechargeBuyBookMyBalanceIsNotEnough);
									callback.sendMessage("", MacroCode.RechargeBuyBookFail);
								} else {
									//callback.sendMessage("余额" + balance_amount + "元", 4);
									// 下单
									//callback.sendMessage("生成购书订单", 4);
									Http_RechargeOrderId(mQueue, memberid,
											bookid, callback,
											new DownLoadPortart() {

												@Override
												public void doSomething(
														JSONObject json) {
													try {
														final int orderId = json.getInt("orderId");
														//callback.sendMessage("余额结算",4);
														// 余额扣款
														Http_SetRecharge(mQueue,memberid,price100 / 100.0,orderId,callback,
																new DownLoadPortart() {

																	@Override
																	public void doSomething(
																			JSONObject json) {
																		//callback.sendMessage("完成订单",4);
																		// 修改订单
																		Http_SetOrderId(
																				mQueue,
																				memberid,
																				orderId,
																				callback,
																				new DownLoadPortart() {

																					@Override
																					public void doSomething(
																							JSONObject json) {
																						callback.sendMessage("",MacroCode.RechargeBuyBookSuccess);
																					}
																				});
																	}
																});
													} catch (Exception e) {
														Log.e("201606271408",e.toString());
														callback.sendMessage("订单异常", 4);
														callback.sendMessage("", MacroCode.RechargeBuyBookFail);
													}
												}
											});
								}
							}
						} catch (Exception e) {
							Log.e("201606271415", e.toString());
							callback.sendMessage("查询余额异常", 4);
							callback.sendMessage("", MacroCode.RechargeBuyBookFail);
						}
					}
				});
	}

	// 余额购书下单
	public void Http_RechargeOrderId(final RequestQueue mQueue,
			final String memberid, final int bookid,
			final CallBackFunction callback,
			final DownLoadPortart finishDoSomething) {
		if(memberid.equals("-999"))
			return;
		String url = MacroCode.IP + MacroCode.NET_RECHARG_BUYBOOK_ORDERID;
		//Log.e("show", url);
		Map<String, String> paramter = new HashMap<String, String>();
		paramter.put("bookId", Integer.toString(bookid));
		paramter.put("memberId", memberid);
		paramter.put("resource", MacroCode.RESOURCE);
		//Log.e("show", paramter.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url,
				paramter, new Listener<JSONObject>() {

					@Override
					public void onResponse(JSONObject response) {
						try {
							//Log.e("show", response.toString());
							boolean result = response.getBoolean("result");
							if (result) {
								JSONObject data = response
										.getJSONObject("data");
								int orderId = data.getInt("orderId");
								callback.sendMessage("订单成功"+orderId, 4);
								JSONObject json = new JSONObject();
								json.put("orderId", orderId);
								finishDoSomething.doSomething(json);
							} else {
								callback.sendMessage("购书下单失败", 4);
								callback.sendMessage("", MacroCode.RechargeBuyBookFail);
							}
						} catch (Exception e) {
							Log.e("201606241550", e.toString());
							callback.sendMessage("购书下单异常", 4);
							callback.sendMessage("", MacroCode.RechargeBuyBookFail);
						}
					}
				}, new ErrorListener() {

					@Override
					public void onErrorResponse(VolleyError error) {
						callback.sendMessage("购书下单失败", 4);
						callback.sendMessage("", MacroCode.RechargeBuyBookFail);
					}
				});
		mQueue.add(request);
	}

	// 余额购书扣款
	public void Http_SetRecharge(final RequestQueue mQueue, String memberid,
			final double Recharge, final int orderId,final CallBackFunction callback,
			final DownLoadPortart finishDoSomething) {
		if(memberid.equals("-999"))
			return;
		String url = MacroCode.IP
				+ MacroCode.NET_RECHARG_BUYBOOK_RECHARGECHARGE + "?Charge="
				+ Double.toString(Recharge) + "&memberId=" + memberid
				+ "&chargePlatformName=android" + "&chargePlatformType=0"
				+ "&resource=" + MacroCode.RESOURCE + "&orderId=" + orderId;
		//Log.e("show", url);

		StringRequest request = new StringRequest(Method.GET, url,
				new Listener<String>() {

					@Override
					public void onResponse(String response) {
//						 {"result":true,"balance":999.0}
						//Log.e("show", response);
						try {
							JSONObject json = new JSONObject(response);
							boolean result = json.getBoolean("result");
							if (result) {
								callback.sendMessage("购书扣款成功", 4);
								finishDoSomething.doSomething(json);
							} else {
								callback.sendMessage("购书扣款失败", 4);
								callback.sendMessage("", MacroCode.RechargeBuyBookFail);
							}
						} catch (Exception e) {
							Log.e("201606271152", e.toString());
							callback.sendMessage("购书扣款异常", 4);
							callback.sendMessage("", MacroCode.RechargeBuyBookFail);
						}
					}
				}, new ErrorListener() {

					@Override
					public void onErrorResponse(VolleyError error) {
						callback.sendMessage("购书下单失败", 4);
						callback.sendMessage("", MacroCode.RechargeBuyBookFail);
					}
				});
		mQueue.add(request);
	}

	// 余额购书修改订单状态
	public void Http_SetOrderId(final RequestQueue mQueue, String memberid,
			int orderId, final CallBackFunction callback,
			final DownLoadPortart finishDoSomething) {
		if(memberid.equals("-999"))
			return;
		String url = MacroCode.IP + MacroCode.NET_RECHARG_BUYBOOK_ORDERID + "/"
				+ memberid + "/" + orderId + "/" + MacroCode.RESOURCE;
		//Log.e("show", url);
		StringRequest request = new StringRequest(Method.GET, url,
				new Listener<String>() {

					@Override
					public void onResponse(String response) {
						// {"result":true,"balance":999.0}
						Log.i("show", response);
						try {
							JSONObject json = new JSONObject(response);
							int code = json.getInt("code");
							if (code == 0) {
								callback.sendMessage("购书成功", 4);
								finishDoSomething.doSomething(json);
							} else {
								callback.sendMessage("购书失败", 4);
								callback.sendMessage("", MacroCode.RechargeBuyBookFail);
							}
						} catch (Exception e) {
							Log.e("201606271152", e.toString());
							callback.sendMessage("购书失败", 4);
							callback.sendMessage("", MacroCode.RechargeBuyBookFail);
						}
					}
				}, new ErrorListener() {

					@Override
					public void onErrorResponse(VolleyError error) {
						callback.sendMessage("购书失败", 4);
						callback.sendMessage("", MacroCode.RechargeBuyBookFail);
					}
				});
		mQueue.add(request);
	}	
	
// 余额购书一次性购书
//	public void Http_Recharge_BuyBook(final RequestQueue mQueue,
//			final String memberid, 
//			final int bookid,
//			final CallBackFunction callback) {
//		if(memberid.equals("-999"))
//			return;
//		String url = MacroCode.IP + MacroCode.NET_RECHARG_BUYBOOK;
//		//Log.e("show", url);
//		Map<String, String> paramter = new HashMap<String, String>();
//		paramter.put("bookId", Integer.toString(bookid));
//		paramter.put("memberId", memberid);
//		paramter.put("resource", MacroCode.RESOURCE);
//		paramter.put("chargePlatformName", "android");
//		paramter.put("chargePlatformType", 0+"");
//		paramter.put("goodsType", 1+"");
//		paramter.put("promotionsId", "");
//		paramter.put("orderMessage", "");
//		paramter.put("payType", 2+"");
//		//Log.e("show", paramter.toString());
//		JsonObjectPostRequest request = new JsonObjectPostRequest(url,
//				paramter, new Listener<JSONObject>() {
//
//					@Override
//					public void onResponse(JSONObject response) {
//						try {
//							//"errorMessage":"","balance":1999982,"result":true,
//							//Log.e("show", response.toString());
//							int code = response.getInt("code");
//							if(code==0)
//							{
//								String desc = response.getString("desc");								
//								callback.sendMessage("购书成功",4);
//								callback.sendMessage("",MacroCode.RechargeBuyBookSuccess);
//							}
//							else if(code == 1)
//							{
//								//余额不足
//								callback.sendMessage("",MacroCode.RechargeBuyBookMyBalanceIsNotEnough);
//							}
//							else
//							{
//								String errorMessage = response.getString("errorMessage");
//								callback.sendMessage("购书失败",4);
//								callback.sendMessage("", MacroCode.RechargeBuyBookFail);								
//								AppActivity.GetBuyBook(Integer.parseInt(memberid));
//							}
//							//int orderId = response.getInt("orderId");							
//						} catch (Exception e) {
//							Log.e("201606301101", e.toString());
//							callback.sendMessage("购书异常", 4);
//							callback.sendMessage("", MacroCode.RechargeBuyBookFail);
//						}
//					}
//				}, new ErrorListener() {
//
//					@Override
//					public void onErrorResponse(VolleyError error) {
//						try{
//							callback.sendMessage("购书失败",4);
//							callback.sendMessage("", MacroCode.RechargeBuyBookFail);
//						} catch (Exception e) {
//							Log.e("201607011034", e.toString());
//						}
//					}
//				});
//		mQueue.add(request);
//	}

}
