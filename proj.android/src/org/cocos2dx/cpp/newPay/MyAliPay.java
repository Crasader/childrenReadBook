package org.cocos2dx.cpp.newPay;

import android.text.TextUtils;
import android.util.Log;

import com.alipay.sdk.app.PayTask;
import com.android.volley.VolleyError;
import com.google.gson.Gson;

import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.MacroCode;
import org.cocos2dx.cpp.PayResult;
import org.cocos2dx.cpp.http.JsonObjectPostRequest;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;


/**
 * Created by java on 2017/5/6.
 */

public class MyAliPay {

    //先获取预付订单和支付的字符串
    public void httpAliPayOrderId(String json) {
        Gson gson = new Gson();
        try {
            JSONObject jsonObject = new JSONObject(json);
            String memberid = jsonObject.getString("memberid");
            String paytype = jsonObject.getString("paytype");
            String contenttpye = jsonObject.getString("contenttpye");
            String resource = jsonObject.getString("resource");
            String version = jsonObject.getString("version");
            final String task = jsonObject.getString("task");
            Parameter parameter = new Parameter(memberid, paytype, contenttpye, "alipay", resource, version);
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
                    Log.e("201705061507", response.toString());
                    try {
                        JSONObject data = response.getJSONObject("data");
                        String order_id = data.getString("out_trade_no");
                        String orderInfo = data.getString("sign");
                        pay(orderInfo, task, order_id);
                    } catch (Exception e) {
                        Log.e("201705061515", e.toString());
                    }
                }
            }, new com.android.volley.Response.ErrorListener() {

                @Override
                public void onErrorResponse(VolleyError error) {
                    Log.e("201705061506", error.toString());
                }
            });
            AppActivity.mQueue.add(request);
        } catch (Exception e) {
            Log.e("201705061505", e.toString());
        }
    }

    public void pay(final String orderInfo, final String task, final String orderid) {
        /**
         * 支付宝支付业务
         *
         * @param v
         */
        Log.e("yyx",orderInfo);
        Log.e("yyx",orderid);
        Runnable payRunnable = new Runnable() {

            @Override
            public void run() {
                try {
                    PayTask alipay = new PayTask(AppActivity.m_context);
                    String result = alipay.pay(orderInfo);
                    PayResult payResult = new PayResult(result);
                    JSONObject jsonObject = null;
                    jsonObject = new JSONObject();
                    String resultStatus = "-1";
                    resultStatus = payResult.getResultStatus();
                    if (TextUtils.equals(resultStatus, "9000")) {
                        jsonObject.put("call", "1");
                    } else {
                        jsonObject.put("call", "2");
                    }
                    jsonObject.put("orderid", orderid);
                    jsonObject.put("task", task);
                    AppActivity.CrossPlatformCallback(jsonObject.toString());

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        };

        Thread payThread = new Thread(payRunnable);
        payThread.start();
    }
}
