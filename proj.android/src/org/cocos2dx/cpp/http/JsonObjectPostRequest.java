package org.cocos2dx.cpp.http;

import android.util.Log;

import com.android.volley.AuthFailureError;
import com.android.volley.NetworkResponse;
import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.toolbox.HttpHeaderParser;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;


public class JsonObjectPostRequest extends Request<JSONObject> {
    private Map<String, String> mMap;
    private Response.Listener<JSONObject> mListener;
    public String cookieFromResponse;
    private Map<String, String> sendHeader=new HashMap<String, String>();
    
    
    public JsonObjectPostRequest(String url,Map<String, String> map, Response.Listener<JSONObject> listener, Response.ErrorListener errorListener) {
        super(Request.Method.POST, url, errorListener);
        mListener = listener;
        mMap = map;
    }

    //当http请求是post时，则需要该使用该函数设置往里面添加的键值对
    @Override
    protected Map<String, String> getParams() throws AuthFailureError {
        return mMap;
    }
    @Override
    protected Response<JSONObject> parseNetworkResponse(NetworkResponse response) {
        JSONObject jsonObject = null;
        try {
            String jsonString = new String(response.data, HttpHeaderParser.parseCharset(response.headers));
            if (jsonString.charAt(0)=='[')
            {
                JSONArray jsonArray = new JSONArray(jsonString);
                jsonObject = new JSONObject();
                jsonObject.put("DataType","Array");
                jsonObject.put("Array",jsonArray);
            }
            else {
                jsonObject = new JSONObject(jsonString);
            }            
            Map<String,String> hashMap = response.headers;
//            Log.i("show","get headers in parseNetworkResponse "+response.headers.toString());
            //使用正则表达式从reponse的头中提取cookie内容的子串
            // 将cookie字符串添加到jsonObject中，该jsonObject会被deliverResponse递交，调用请求时则能在onResponse中得到
            String setcookie = hashMap.get("Set-Cookie");
            if (setcookie == null)
            {
                Log.i("正常", "接收端Set-Cookie=NULL");
            }
            else if (setcookie.isEmpty())
            {
                Log.i("正常", "接收端Set-Cookie=\"\"");
            }
            else
            {
                //jsonObject.put("Set-Cookie",setcookie);
            }
        }
        catch (Exception e)
        {
            Log.e("201511091749",e.toString());
        }
        return Response.success(jsonObject, HttpHeaderParser.parseCacheHeaders(response));
    }

    @Override
    protected void deliverResponse(JSONObject response) {
        mListener.onResponse(response);
    }

    @Override
    public Map<String, String> getHeaders() throws AuthFailureError {
        return sendHeader;
    }
    public void setSendCookie(String cookie){
        //sendHeader.put("Cookie",cookie);
    }
}
