package org.cocos2dx.cpp.newPay;

import android.util.Log;

import com.android.volley.VolleyError;
import com.google.gson.Gson;
import com.tencent.mm.sdk.modelpay.PayReq;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.WXAPIFactory;

import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.MacroCode;
import org.cocos2dx.cpp.http.JsonObjectPostRequest;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.SortedMap;
import java.util.TreeMap;

/**
 * Created by java on 2017/5/7.
 */

public class MyWeixinPay {
    private IWXAPI api;
    private String appid = "wx612d474d79028d9b";

    public void Pay(String json) {
        try {
            // 通过WXAPIFactory工厂，获取IWXAPI的实例
            api = WXAPIFactory.createWXAPI(AppActivity.m_context, appid, false);
            //检查是否有微信
            if (!api.openWXApp()) {//没有微信客户端
                JSONObject jsonObject = new JSONObject();
                jsonObject.put("call", "4");
                jsonObject.put("error", "没有微信APP");
                AppActivity.CrossPlatformCallback(jsonObject.toString());
                return;
            }
            // 在支付之前，如果应用没有注册到微信，应该先调用IWXMsg.registerApp将应用注册到微信
            if (api.registerApp(appid)) {
                Log.i("show", "微信APP注册成功");
            }
            httpSign(json);
        } catch (Exception e) {
            Log.e("201705071444", e.toString());
        }
    }

    //先获取预付订单和支付的字符串
    private void httpSign(String json) {
        Gson gson = new Gson();
        try {
            JSONObject jsonObject = new JSONObject(json);
            String memberid = jsonObject.getString("memberid");
            String paytype = jsonObject.getString("paytype");
            String contenttpye = jsonObject.getString("contenttpye");
            String resource = jsonObject.getString("resource");
            String version = jsonObject.getString("version");
            Parameter parameter = new Parameter(memberid, paytype, contenttpye, "wxpay", resource, version);
            publicParameter publicParameter = new publicParameter("ellabook.pay.create", "json", "appkey", "1.0", "sign", "md5", "2017-05-04 12:32:58");
            publicParameter.setContent(gson.toJson(parameter));
            String pJson = gson.toJson(publicParameter);
            Log.e("show", pJson);

            final Map<String, String> paramter = new HashMap<String, String>();
            JSONObject ob = new JSONObject(pJson);
            Iterator it = ob.keys();
            while (it.hasNext()) {
                String key = (String) it.next();
                String value = ob.getString(key);
                paramter.put(key, value);
            }
            JsonObjectPostRequest request = new JsonObjectPostRequest(MacroCode.IP+"/ellabook-server/rest/api/service",
                    paramter, new com.android.volley.Response.Listener<JSONObject>() {

                @Override
                public void onResponse(JSONObject response) {
                    Log.e("201705071445", response.toString());
                    try {
                        JSONObject data = response.getJSONObject("data");
                        String order_id = data.getString("out_trade_no");
                        JSONObject orderInfoJson = data.getJSONObject("sign");
                        String _appid = orderInfoJson.getString("appid");
                        String _nonceStr = orderInfoJson.getString("noncestr");
                        String _packageValue = orderInfoJson.getString("package");
                        String _partnerId = orderInfoJson.getString("partnerid");
                        String _prepayId = orderInfoJson.getString("prepayid");
                        String _sign = orderInfoJson.getString("sign");
                        String _timestamp = orderInfoJson.getString("timestamp");
                        //存微信订单
                        JSONObject jsonObject = new JSONObject();
                        jsonObject.put("call", "5");
                        jsonObject.put("order_id", "");
                        AppActivity.CrossPlatformCallback(jsonObject.toString());

                        PayReq req = new PayReq();
                        req.appId =_appid;
                        req.partnerId = _partnerId;
                        req.prepayId = _prepayId;
                        req.nonceStr = _nonceStr;
                        req.timeStamp = _timestamp;
                        req.packageValue = _packageValue;
                        req.sign = _sign;
                        req.extData = "app data"; // optional
                        startWeixinPay(req);
                    } catch (Exception e) {
                        Log.e("201705071448", e.toString());
                    }
                }
            }, new com.android.volley.Response.ErrorListener() {

                @Override
                public void onErrorResponse(VolleyError error) {
                    Log.e("201705071447", error.toString());
                }
            });
            AppActivity.mQueue.add(request);
        } catch (Exception e) {
            Log.e("201705071446", e.toString());
        }
    }

    private void startWeixinPay(PayReq req) {
        api.sendReq(req);
    }
}
