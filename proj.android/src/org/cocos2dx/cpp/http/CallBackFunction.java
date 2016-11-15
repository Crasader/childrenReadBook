package org.cocos2dx.cpp.http;

import org.json.JSONObject;

public interface CallBackFunction {
	public void sendMessage(String json,int MessageType);
	
	public interface DownLoadPortart
	{
		public void doSomething(JSONObject json);
	}
	
	public interface DownLoadImage
	{
		public void doSomething(String str);
	}
	
	public interface FindFunctionForKey
	{
		public void doSomething(String functionKey,String json);
	}
}
