package org.cocos2dx.cpp.http;

import java.io.File;
import java.io.FileOutputStream;
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
import org.json.JSONObject;

import android.content.Context;
import android.os.Message;
import android.util.Log;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.Response.ErrorListener;
import com.android.volley.VolleyError;

public class BabyCenter {

	// 上传头像
	public static void upImage(RequestQueue mQueue, final int childID, String ImageName,final String runKey, final String errorRunKey, final CallBackFunction.FindFunctionForKey callback) 
	{
		String url = MacroCode.IP + "/ellabook-server/children/setAvatar_android.do";
		final String path = AppActivity.GetMyAppDir()+File.separator+ "temp" + File.separator + ImageName;
		HttpClient httpclient = new DefaultHttpClient();
		HttpPost httppost = new HttpPost(url);
		try {
			FileBody pictureStream = new FileBody(new File(path));
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
						callback.doSomething(runKey, path);
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

	public static void writeFileData(String fileName, String message) {
		try {
			FileOutputStream fout = new FileOutputStream(fileName);
			byte[] bytes = message.getBytes();
			fout.write(bytes);
			fout.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
