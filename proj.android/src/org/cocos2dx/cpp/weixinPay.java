package org.cocos2dx.cpp;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.entity.mime.MultipartEntity;
import org.apache.http.entity.mime.content.FileBody;
import org.apache.http.entity.mime.content.StringBody;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.cocos2dx.cpp.http.CallBackFunction;
import org.cocos2dx.cpp.http.HttpTool;
import org.cocos2dx.cpp.http.JsonObjectPostRequest;
import org.cocos2dx.cpp.sqlite.MySQLiteOpenHelper;
import org.cocos2dx.cpp.util.MD5Util;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.xmlpull.v1.XmlPullParser;

import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.Request.Method;
import com.android.volley.toolbox.StringRequest;
import com.ellabook.R;
import com.ellabook.wxapi.WXPayEntryActivity;
import com.tencent.mm.sdk.modelpay.PayReq;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.WXAPIFactory;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ContentValues;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;
import android.util.Xml;
import android.widget.Toast;

public class weixinPay{
	private IWXAPI api;
	private Context context;	

	public weixinPay(Context context)
	{
		this.context = context;
	}


	// 支付
	public String pay(String orderID, String price, String info) {
		// 统一下单
		UnifiedOrderRequest data = new UnifiedOrderRequest();
		data.appid = "wx612d474d79028d9b";
		data.body = info;
		data.mch_id = "1263697101";
		data.nonce_str = getNonceStr();
		data.notify_url = MacroCode.IP + "/ellabook-server/Weixinpay.jsp";
		data.out_trade_no = orderID;
		data.spbill_create_ip = Tool.getLocalIpAddress();
		data.total_fee = price;
		data.trade_type = "APP";
		
		// 通过WXAPIFactory工厂，获取IWXAPI的实例
    	api = WXAPIFactory.createWXAPI(context, data.appid, false);
    	//检查是否有微信
    	if(!api.openWXApp())
    	{
    		Message msg = new Message();
			msg.what = 4;
			msg.obj = "没有微信客户端";
			AppActivity.mHandler.sendMessage(msg);
    		return "没有微信客户端";
    	}	    	
		if(api.registerApp(data.appid))
    		Log.i("show", "注册成功");
		SortedMap<String, String> packageParams = new TreeMap<String, String>();
		packageParams.put("appid",data.appid);
		packageParams.put("body",data.body);
		packageParams.put("mch_id",data.mch_id);
		packageParams.put("nonce_str",data.nonce_str);
		packageParams.put("notify_url",data.notify_url);
		packageParams.put("out_trade_no",data.out_trade_no);
		packageParams.put("spbill_create_ip",data.spbill_create_ip);
		packageParams.put("total_fee",data.total_fee);
		packageParams.put("trade_type",data.trade_type);
		data.sign = createSign(packageParams);
		String postData = buildXMLUnifiedOrder(data);
		Log.e("show", postData);
		String str = httpOrder(postData);
		// <xml><return_code><![CDATA[SUCCESS]]></return_code><return_msg><![CDATA[OK]]></return_msg><appid><![CDATA[wx612d474d79028d9b]]></appid><mch_id><![CDATA[1263697101]]></mch_id><nonce_str><![CDATA[eSSmS5x5Nqz8gNpV]]></nonce_str><sign><![CDATA[93A16A735F6AC5E153EDB54B2E7F2F2A]]></sign><result_code><![CDATA[SUCCESS]]></result_code><prepay_id><![CDATA[wx201606171436233977d4d2990896228770]]></prepay_id><trade_type><![CDATA[APP]]></trade_type></xml>
		UnifiedOrderRespose respose = null;
		try {
			// 由android.util.Xml创建一个XmlPullParser实例
			XmlPullParser xpp = Xml.newPullParser();
			// 设置输入流 并指明编码方式
			xpp.setInput(new StringReader(str));
			// 产生第一个事件
			int eventType = xpp.getEventType();
			while (eventType != XmlPullParser.END_DOCUMENT) {
				switch (eventType) {
				// 判断当前事件是否为文档开始事件
				case XmlPullParser.START_DOCUMENT:
					respose = new UnifiedOrderRespose();
					break;
				// 判断当前事件是否为标签元素开始事件
				case XmlPullParser.START_TAG:
					if (xpp.getName().equals("return_code")) {
						eventType = xpp.next();
						respose.return_code = xpp.getText();
					} else if (xpp.getName().equals("return_msg")) {
						eventType = xpp.next();
						respose.return_msg = xpp.getText();
					} else if (xpp.getName().equals("appid")) {
						eventType = xpp.next();
						respose.appid = xpp.getText();
					} else if (xpp.getName().equals("mch_id")) {
						eventType = xpp.next();
						respose.mch_id = xpp.getText();
					} else if (xpp.getName().equals("nonce_str")) {
						eventType = xpp.next();
						respose.nonce_str = xpp.getText();
					} else if (xpp.getName().equals("sign")) {
						eventType = xpp.next();
						respose.sign = xpp.getText();
					} else if (xpp.getName().equals("result_code")) {
						eventType = xpp.next();
						respose.result_code = xpp.getText();
					} else if (xpp.getName().equals("prepay_id")) {
						eventType = xpp.next();
						respose.prepay_id = xpp.getText();
					} else if (xpp.getName().equals("trade_type")) {
						eventType = xpp.next();
						respose.trade_type = xpp.getText();
					}
					break;

				// 判断当前事件是否为标签元素结束事件
				case XmlPullParser.END_TAG:
					break;
				}
				eventType = xpp.next();
			}
		} catch (Exception e) {
			Log.e("201606171514", e.toString());
		}
		if (respose != null)
			Log.e("show", respose.toString());
		//调起微信 用户进行支付
    	try{
			PayReq req = new PayReq();
			SortedMap<String, String> par = new TreeMap<String, String>();
			par.put("appid",data.appid);
			par.put("partnerid",data.mch_id);
			par.put("prepayid",respose.prepay_id);
			par.put("noncestr",data.nonce_str);
			par.put("timestamp","1466156165");
			par.put("package","Sign=WXPay");
			req.appId			= data.appid;
			req.partnerId		= data.mch_id;
			req.prepayId		= respose.prepay_id;
			req.nonceStr		= data.nonce_str;
			req.timeStamp		= "1466156165";
			req.packageValue	= "Sign=WXPay";
			req.sign			= createSign(par);
			req.extData			= "app data"; // optional
			Log.i("show","正常调起支付"+req.toString());
			// 在支付之前，如果应用没有注册到微信，应该先调用IWXMsg.registerApp将应用注册到微信
			api.sendReq(req);
    	}catch(Exception e)
    	{
    		Log.e("201606171515", e.toString());
    	}
		return str;
		//
	}

	/**
	 * 构造出xml文件的内容，用于发送给“统一下单”获取到prepay_id
	 * 
	 * @return XML的String输出
	 */
	public String buildXMLUnifiedOrder(UnifiedOrderRequest data) {

		List<TwoTuple<String, String>> paramList = new ArrayList<TwoTuple<String, String>>();
		paramList.add(new TwoTuple<String, String>("appid", data.appid));
		paramList.add(new TwoTuple<String, String>("body", data.body));
		paramList.add(new TwoTuple<String, String>("mch_id", data.mch_id));
		paramList
				.add(new TwoTuple<String, String>("nonce_str", data.nonce_str));
		paramList.add(new TwoTuple<String, String>("notify_url",
				data.notify_url));
		paramList.add(new TwoTuple<String, String>("out_trade_no",
				data.out_trade_no));
		paramList.add(new TwoTuple<String, String>("spbill_create_ip",
				data.spbill_create_ip));
		paramList
				.add(new TwoTuple<String, String>("total_fee", data.total_fee));
		paramList.add(new TwoTuple<String, String>("trade_type", "APP"));
		paramList.add(new TwoTuple<String, String>("sign", data.sign));

		StringBuilder xmlBuilder = new StringBuilder();
		xmlBuilder.append("<xml>");
		for (TwoTuple<String, String> paramTuple : paramList) {
			xmlBuilder.append("<").append(paramTuple.first).append(">");
			xmlBuilder.append(paramTuple.second);
			xmlBuilder.append("</").append(paramTuple.first).append(">");
		}
		xmlBuilder.append("</xml>");
		return xmlBuilder.toString();
	}

	/**
	 * 
	 * 两个元素的元组，用于在一个方法里返回两种类型的值
	 * 
	 * @param <A>
	 * @param <B>
	 */
	public class TwoTuple<A, B> {

		public final A first;
		public final B second;

		public TwoTuple(A a, B b) {
			first = a;
			second = b;
		}

	}

	/**
	 * 调统一下单API
	 * 
	 * @param orderInfo
	 * @return
	 */
	public String httpOrder(String orderInfo) {
		String url = "https://api.mch.weixin.qq.com/pay/unifiedorder";
		String result = "";
		try {
			HttpURLConnection conn = (HttpURLConnection) new URL(url)
					.openConnection();
			// 加入数据
			conn.setRequestMethod("POST");
			conn.setDoOutput(true);

			BufferedOutputStream buffOutStr = new BufferedOutputStream(
					conn.getOutputStream());
			buffOutStr.write(orderInfo.getBytes());
			buffOutStr.flush();
			buffOutStr.close();

			// 获取输入流
			BufferedReader reader = new BufferedReader(new InputStreamReader(
					conn.getInputStream()));

			String line = null;
			StringBuffer sb = new StringBuffer();
			while ((line = reader.readLine()) != null) {
				sb.append(line);
			}
			Log.e("show", sb.toString());
			return sb.toString();			     
		} catch (Exception e) {
			Log.e("201606221440",e.toString());
		}
		Log.e("show", result);
		return result;
	}

	/**
	 * 创建随机字符串
	 * 
	 * @return
	 */
	public static String getNonceStr() {
		Random random = new Random();
		return MD5Util.MD5Encode(String.valueOf(random.nextInt(10000)), "GBK");
	}

	/**
	 * 生成签名
	 * 
	 * @param appid_value
	 * @param mch_id_value
	 * @param productId
	 * @param nonce_str_value
	 * @param trade_type
	 * @param notify_url
	 * @param spbill_create_ip
	 * @param total_fee
	 * @param out_trade_no
	 * @return
	 */
	public String createSign(SortedMap<String, String> packageParams) {
		// 根据规则创建可排序的map集合
		try {			
			StringBuffer sb = new StringBuffer();
			Set es = packageParams.entrySet();// 字典序
			Iterator it = es.iterator();
			while (it.hasNext()) {
				Map.Entry entry = (Map.Entry) it.next();
				String k = (String) entry.getKey();
				String v = (String) entry.getValue();
				// 为空不参与签名、参数名区分大小写
				if (null != v && !"".equals(v) && !"sign".equals(k)
						&& !"key".equals(k)) {
					sb.append(k + "=" + v + "&");
				}
			}
			// 第二步拼接key，key设置路径：微信商户平台(pay.weixin.qq.com)-->账户设置-->API安全-->密钥设置
			sb.append("key=" + "ellabookappforweixinpaysince2015");
			String sign = MD5Util.MD5Encode(sb.toString(), "utf-8")
					.toUpperCase();// MD5加密
			return sign;
		} catch (Exception e) {
			Log.e("201606171520", e.toString());
			return "";
		}
	}

	/**
	 * 统一下单返回参数
	 * 
	 * @author Y
	 * 
	 */
	public class UnifiedOrderRespose {
		public String return_code; // 返回状态码
		public String return_msg; // 返回信息
		public String appid; // 公众账号ID
		public String mch_id; // 商户号
		public String device_info; // 设备号
		public String nonce_str; // 随机字符串
		public String sign; // 签名
		public String result_code; // 业务结果
		public String trade_type; // 交易类型
		public String prepay_id; // 预支付交易会话标识

		@Override
		public String toString() {
			return "UnifiedOrderRespose [return_code=" + return_code
					+ ", return_msg=" + return_msg + ", appid=" + appid
					+ ", mch_id=" + mch_id + ", device_info=" + device_info
					+ ", nonce_str=" + nonce_str + ", sign=" + sign
					+ ", result_code=" + result_code + ", trade_type="
					+ trade_type + ", prepay_id=" + prepay_id + "]";
		}

	}

	/**
	 * 统一下单请求参数(必填)
	 * 
	 * @author Y
	 * 
	 */
	public class UnifiedOrderRequest {
		public String appid; // 公众账号ID
		public String mch_id; // 商户号
		public String nonce_str; // 随机字符串
		public String sign; // 签名
		public String body; // 商品描述
		public String out_trade_no; // 商户订单号
		public String total_fee; // 总金额
		public String spbill_create_ip; // 终端IP
		public String notify_url; // 通知地址
		public String trade_type; // 交易类型
	}

}
