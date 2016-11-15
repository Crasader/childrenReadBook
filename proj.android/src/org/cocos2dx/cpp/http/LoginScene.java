package org.cocos2dx.cpp.http;

import java.io.File;
import java.io.FileOutputStream;
import java.util.HashMap;

import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.ImageTool;
import org.cocos2dx.cpp.MacroCode;
import org.cocos2dx.cpp.Tool;
import org.cocos2dx.cpp.http.CallBackFunction.DownLoadPortart;
import org.cocos2dx.cpp.sqlite.MySQLiteOpenHelper;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.os.SystemClock;
import android.util.Log;

import com.android.volley.Request.Method;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.Response.ErrorListener;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;

public class LoginScene {

	// 账号登陆
	public static void http_AccountLogin(RequestQueue mQueue,
			final AppActivity context, final int requestID,
			final String account, final String password,
			final CallBackFunction callback) {
		String url = MacroCode.IP + MacroCode.NET_LOGIN;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("username", account);
		map.put("password", password);
		map.put("resource", MacroCode.RESOURCE);
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						//Log.e("show",response.toString());
						boolean result = HttpTool.GetBool(response, "result",
								"201605121037");
						String errorMessage = HttpTool.GetString(response,
								"errorMessage", "201605121036");
						Log.i("show", "result=" + result);
						Log.i("show", "errorMessage=" + errorMessage);
						callback.sendMessage(response.toString(),
								MacroCode.LoadScene_AccountLogin);
						try {
							if (result) {
								JSONObject data = HttpTool.GetJSONObject(
										response, "data", "201605121035");
								int memberId = HttpTool.GetInt(data,
										"memberId", "201605121034");
								String memberName = HttpTool.GetString(data,
										"memberName", "201605121033");
								int memberSex = HttpTool.GetInt(data,
										"memberSex", "201605121032");
								String memberCity = HttpTool.GetString(data,
										"memberCity", "201605121031");
								String memberProvince = HttpTool.GetString(
										data, "memberProvince", "201605121030");
								Log.i("show", "requestID=" + requestID);
								Log.i("show", "memberId=" + memberId);
								Log.i("show", "memberName=" + memberName);
								Log.i("show", "memberSex=" + memberSex);
								Log.i("show", "memberCity=" + memberCity);
								Log.i("show", "memberProvince="
										+ memberProvince);
								Log.i("show", "插入数据库");
								SQLiteDatabase db = new MySQLiteOpenHelper(
										context, "data").getWritableDatabase();
								WriteSqlite_UserInfo(db, memberId, memberName,
										password, memberSex, memberProvince,
										memberCity, Tool.getTime() / 1000, 1);
								db.close();
							}
						} catch (Exception e) {
							Log.e("201605141323", e.toString());							
						}
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605121029", error.toString());
						HttpTool.ReturnExceptionJSON(callback,
								MacroCode.LoadScene_AccountLoginException);
					}

				});
		request.setTag(requestID);
		mQueue.add(request);
	}

	// 账号信息写入数据库
	public static void WriteSqlite_UserInfo(SQLiteDatabase db, long member_id,
			String member_name, String member_passwd, long member_sex,
			String memberProvince, String memberCity, long upTime, long status) {
		SQLiteDatabase mydb = db; 
		try{
			mydb.delete(MacroCode.DB_USERINFO, "member_id=?",
				new String[] { Long.toString(member_id) });
		}catch(Exception e)
		{
			Log.e("201605301549",e.toString());
			mydb.close();
			String dir = AppActivity.m_context.getApplicationContext().getFilesDir().getAbsolutePath();
        	Log.i("show",dir);
            String path =dir+ "/data.db";
			boolean isFileCreateSuccess=MySQLiteOpenHelper.copyDataBase(AppActivity.m_context,dir,path);
            if(!isFileCreateSuccess)
            	Log.i("show","复制数据库失败");
            else
            {
            	Log.i("show","重新建立数据库");
            	mydb = new MySQLiteOpenHelper(AppActivity.m_context,"data").getWritableDatabase();
            }
		}
		ContentValues value = new ContentValues();
		value.put("member_id", member_id);
		value.put("member_name", member_name);
		value.put("member_passwd", member_passwd);
		value.put("member_sex", member_sex);
		value.put("memberProvince", memberProvince);
		value.put("memberCity", memberCity);
		value.put("upTime", upTime);
		value.put("status", status);
		long result = mydb.insert(MacroCode.DB_USERINFO, null, value);
		if (result == 1)
			Log.i("show", "userinfo insert success " + member_id);
	}

	// 查询最近一次登录的用户信息
	public static String ReadSqlite_UserInfo(SQLiteDatabase db) {
		Cursor cursor = db.query(MacroCode.DB_USERINFO, null, "status=1",
				new String[] {}, null, null, null);
		JSONArray array = new JSONArray();
		while (cursor.moveToNext()) {
			long member_id = cursor.getInt(cursor.getColumnIndex("member_id"));
			String member_name = cursor.getString(cursor
					.getColumnIndex("member_name"));
			String member_passwd = cursor.getString(cursor
					.getColumnIndex("member_passwd"));
			long member_sex = cursor
					.getInt(cursor.getColumnIndex("member_sex"));
			String memberProvince = cursor.getString(cursor
					.getColumnIndex("memberProvince"));
			String memberCity = cursor.getString(cursor
					.getColumnIndex("memberCity"));
			long upTime = cursor.getInt(cursor.getColumnIndex("upTime"));
			JSONObject userinfo = new JSONObject();
			HttpTool.Put(userinfo, "member_id", member_id);
			HttpTool.Put(userinfo, "member_name", member_name);
			HttpTool.Put(userinfo, "member_passwd", member_passwd);
			HttpTool.Put(userinfo, "member_sex", member_sex);
			HttpTool.Put(userinfo, "memberProvince", memberProvince);
			HttpTool.Put(userinfo, "memberCity", memberCity);
			HttpTool.Put(userinfo, "upTime", upTime);
			array.put(userinfo);
		}
		return array.toString();
	}

	// 网络获取孩子详情
	public static void http_getChildDetails(final RequestQueue mQueue,
			final AppActivity context, final int requestID,
			final int downloadImageRequestID, final int memberId,
			final CallBackFunction callback) {
		String url = MacroCode.IP + MacroCode.NET_CHILD_DETAILS;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("memberId", Integer.toString(memberId));
		map.put("resource", MacroCode.RESOURCE);
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						try {
							//Log.e("show", response.toString());
							int code = HttpTool.GetInt(response, "code",
									"201605141710");
							String desc = HttpTool.GetString(response, "desc","201605141711");
							callback.sendMessage(response.toString(),
									MacroCode.LoadScene_getChildDetails);
							if (code == 0) {
								JSONArray data = HttpTool.GetJSONArray(
										response, "data", "201605141711");
								for (int i = 0; i < data.length(); i++) {
									JSONObject item = data.getJSONObject(i);
									int children_id = HttpTool.GetInt(item,
											"childrenId", "201605141712");
									int children_sex = HttpTool.GetInt(item,
											"childrenSex", "201605141713");
									String children_name = HttpTool.GetString(
											item, "childrenName",
											"201605141716");
									String children_birth = HttpTool.GetString(
											item, "childrenBirth",
											"201605141715");
									String avatarUrl = HttpTool.GetString(item,
											"avatarUrl", "201605141714");
									Log.i("show", "children_id=" + children_id);
									Log.i("show", "children_sex="
											+ children_sex);
									Log.i("show", "children_name="
											+ children_name);
									Log.i("show", "children_birth="
											+ children_birth);
									Log.i("show", "avatarUrl=" + avatarUrl);
									// -----------------------下载孩子头像--------------------------------------
									Log.i("show", "下载孩子头像");
									final String dir = context
											.getApplicationContext()
											.getFilesDir().getAbsolutePath()+File.separator+"temp";
									File dbf=new File(dir);
							           if(!dbf.exists()){
							        	   dbf.mkdir();
							           }
									String name = "por_" + Tool.getTime()
											+ ".png";
									JSONObject sj = new JSONObject();
									sj.put("name", name);
									sj.put("children_id", children_id);
									HttpTool.DownLoadImage(mQueue,downloadImageRequestID,avatarUrl,name,dir,
											new CallBackFunction.DownLoadPortart() {

												@Override
												public void doSomething(JSONObject json) {
													// 将资源写入数据库													
													String name = null;
													int children_id = -999;													
													try {
														children_id = json.getInt("children_id");} catch (JSONException e) {	Log.e("201605261856",e.toString());}													
													try {														
														name = json.getString("name");} catch (JSONException e) {	Log.e("201605261856",e.toString());}
													SQLiteDatabase db = new MySQLiteOpenHelper(context, "data").getWritableDatabase();
													ContentValues value = new ContentValues();
													value.put("bookId",children_id);
													value.put("status", 8);
													value.put("downloadtime",Tool.getTime() / 1000);
													value.put("path", dir + File.separator + name);
													Tool.WriteSqlite(db,MacroCode.DB_RES,value,"bookId=? and status=8",
															new String[] { Integer.toString(children_id) },1);
													db.close();
													Log.i("show","孩子头像资源路径插入数据库"+value.toString());
													Bitmap bp = ImageTool.getBitmapformPicturePathByCompaction(dir+ File.separator+ name,AppActivity.m_context,600, 600,false);
													Bitmap rp = ImageTool.toRoundBitmap(bp);
													ImageTool.saveFile(rp,name, dir);
													JSONObject jsonstr = new JSONObject();
													try {
														jsonstr.put("ChildId", children_id);} catch (JSONException e) { Log.e("201606071757",e.toString());}
													try {
														jsonstr.put("Path", dir+File.separator+name);} catch (JSONException e) {	Log.e("201606071758",e.toString()); }
													callback.sendMessage(jsonstr.toString(), MacroCode.BabyCenterScene_REFERCHILDPORTRAIT);						
												}
											}, sj);
									// -----------------------下载孩子头像END--------------------------------------
									// -----------------------插入数据库--------------------------------------
									Log.i("show", "孩子详细信息插入数据库");
									SQLiteDatabase db = new MySQLiteOpenHelper(
											context, "data")
											.getWritableDatabase();
									ContentValues value = new ContentValues();
									value.put("children_id", children_id);
									value.put("children_name", children_name);
									value.put("member_id", memberId);
									value.put("children_sex", children_sex);
									value.put("children_birth", children_birth);
									value.put("avatarUrl", avatarUrl);
									value.put("upTime", Tool.getTime() / 1000);
									Tool.WriteSqlite(
											db,
											MacroCode.DB_CHILDREN,
											value,
											"children_id=? and member_id=?",
											new String[] {
													Integer.toString(children_id),
													Integer.toString(memberId) },
											1);
									db.close();
									// -----------------------插入数据库END--------------------------------------									
								}
							}
						} catch (Exception e) {
							Log.e("201605141718", e.toString());
						}
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605141717", error.toString());
					}
				});
		request.setTag(requestID);
		mQueue.add(request);
	}

	// 网络获取已购书籍列表
	public static void http_GetBuyBook(RequestQueue mQueue,
			final AppActivity context, final int requestID, final int memberId,
			final CallBackFunction callback) {
		// "memberId=").append(App::getMemberID()).append("&page=1&pagesize=1000&resource=").append(App::m_resource);
		String url = MacroCode.IP + MacroCode.NET_BOOKROOMBOOKLIST
				+ "?memberId=" + memberId + "&page=1&pagesize=1000&resource="
				+ MacroCode.RESOURCE;
		Log.i("show", url);
		JsonObjectRequest request = new JsonObjectRequest(Method.GET, url,
				null, new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						Log.i("show", response.toString());
						try {
							int code = HttpTool.GetInt(response, "code",
									"201605151456");
							if (code == 0) {
								JSONObject xml = new JSONObject();
								xml.put("memberId", memberId);
								JSONArray mybookids = new JSONArray();
								JSONArray array = HttpTool.GetJSONArray(
										response, "data", "201605151457");
								SQLiteDatabase db = new MySQLiteOpenHelper(
										context, "data").getWritableDatabase();								
								for (int i = 0; i < array.length(); i++) {

									JSONObject item = array.getJSONObject(i);
									int member_id = memberId;
									int bookId = HttpTool.GetInt(item,
											"bookId", "201605151506");
									mybookids.put(bookId);
									int orderId = HttpTool.GetInt(item,
											"orderId", "201605151507");
									String bookCoverUrl = HttpTool.GetString(
											item, "bookCoverUrl",
											"201605151508");
									String bookPlayUrl = HttpTool
											.GetString(item, "bookPlayUrl",
													"201605151509");
									long upTime = Tool.getTime() / 1000;
									// 插入数据库
									Log.i("show", "已购书籍列表信息插入数据库" + bookId);
									ContentValues value = new ContentValues();
									value.put("member_id", member_id);
									value.put("bookId", bookId);
									value.put("orderId", orderId);
									value.put("bookCoverUrl", bookCoverUrl);
									value.put("bookPlayUrl", bookPlayUrl);
									value.put("upTime", upTime);
									Tool.WriteSqlite(db,MacroCode.DB_USERBOOKLIST, value,
										"bookId=? and member_id=?", 
										new String[]{ Integer.toString(bookId),Integer.toString(member_id) }, i + 1);
								}
								db.close();
								xml.put("books", mybookids);
								Tool.writeFileData("books.json",xml.toString(),context);
								callback.sendMessage("true",
										MacroCode.LoadScene_GetBuyBook);
							}
						} catch (Exception e) {
							Log.e("201605151455", e.toString());							
							callback.sendMessage("Error",
									MacroCode.LoadScene_GetBuyBook);
						}
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605151446", error.toString());
						callback.sendMessage("false",
								MacroCode.LoadScene_GetBuyBook);
					}
				});
		request.setTag(requestID);
		mQueue.add(request);
	}

	// 网络请求书城的按钮列表
	public static void http_GetBookCityButtonsInfo(RequestQueue mQueue,
			final AppActivity context, final int requestID,
			final CallBackFunction callback) {
		String url = MacroCode.IP + MacroCode.NET_BOOKCITY + "?memberId=-999"
				+ "&page=1&pagesize=1000&resource=" + MacroCode.RESOURCE;
		Log.i("show", url);
		JsonObjectRequest request = new JsonObjectRequest(Method.GET, url,
				null, new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						Log.i("show", response.toString());
						try {
							int code = HttpTool.GetInt(response, "code",
									"201605151558");
							if (code == 0) {
								JSONArray array = HttpTool.GetJSONArray(
										response, "data", "201605151557");
								SQLiteDatabase db = new MySQLiteOpenHelper(
										context, "data").getWritableDatabase();
								callback.sendMessage(
										response.toString(),
										MacroCode.LoadScene_GetBookCityButtonsInfo);
								for (int i = 0; i < array.length(); i++) {
									JSONObject item = array.getJSONObject(i);
									final int castleId = HttpTool.GetInt(item,
											"castleId", "201605151541");
									String castleName = HttpTool.GetString(
											item, "castleName", "201605151542");
									String borderUrl = HttpTool.GetString(item,
											"borderUrl", "201605151543");
									String bgUrl = HttpTool.GetString(item,
											"bgUrl", "201605151544");
									int storeBorder = HttpTool.GetInt(item,
											"storeBorder", "201605151545");
									long upTime = Tool.getTime() / 1000;
									int sort = i + 1;

									// 插入数据库
									Log.i("show", "书城的按钮列表信息插入数据库" + castleId);
									ContentValues value = new ContentValues();
									value.put("castleId", castleId);
									value.put("castleName", castleName);
									value.put("borderUrl", borderUrl);
									value.put("bgUrl", bgUrl);
									value.put("storeBorder", storeBorder);
									value.put("upTime", upTime);
									value.put("sort", sort);
									Tool.WriteSqlite(db,
											MacroCode.DB_STOREINFO, value,
											"sort=?", new String[] { Integer
													.toString(sort) }, i + 1);
									// 先获取资源信息,然后判断是否要下载更新
									// 6是背景
									boolean bgdown = true;
									JSONArray sqlArray = Tool.ReadSqlite(db,
											MacroCode.DB_RES,
											new String[] { "path" },
											new String[] { "downloadtime" },
											"bookId=? and status=6",
											new String[] { Integer
													.toString(castleId) });
									if (sqlArray.length() > 0) {
										JSONObject sqlItem = sqlArray
												.getJSONObject(0);
										String path = HttpTool
												.GetString(sqlItem, "path",
														"201605151649");
										long downloadtime = HttpTool.GetLong(
												sqlItem, "downloadtime",
												"201605151650");
										long times = Tool.getTime() / 1000
												- downloadtime;
										if (new File(path).exists()
												&& times < 566400) {
											Log.i("show", path + "不需要更新书城按钮背景图");
											bgdown = false;
										}
									}
									// 下载图片,写入数据库
									if (bgdown) {
										final String dir = AppActivity.m_context
												.getApplicationContext()
												.getFilesDir()
												.getAbsolutePath();
										final String name = "bg_"
												+ Tool.getTime() + ".png";
										HttpTool.DownLoadImage(
												AppActivity.mQueue,
												AppActivity.getRequestId(),
												bgUrl,
												name,
												dir,
												new CallBackFunction.DownLoadPortart() {
													@Override
													public void doSomething(
															JSONObject json) {
														ContentValues value = new ContentValues();
														value.put("bookId",
																castleId);
														value.put("status", 6);
														value.put(
																"path",
																dir
																		+ File.separator
																		+ name);
														value.put(
																"downloadtime",
																Tool.getTime() / 1000);
														SQLiteDatabase db = new MySQLiteOpenHelper(
																context, "data")
																.getWritableDatabase();
														Tool.WriteSqlite(
																db,
																MacroCode.DB_RES,
																value,
																"bookId=? and status=6",
																new String[] { Integer
																		.toString(castleId) },
																1);
														db.close();
													}
												}, new JSONObject());
									}
									// /7是边框
									boolean borderb = true;
									JSONArray sqlArray_border = Tool
											.ReadSqlite(
													db,
													MacroCode.DB_RES,
													new String[] { "path" },
													new String[] { "downloadtime" },
													"bookId=? and status=7",
													new String[] { Integer
															.toString(castleId) });
									if (sqlArray_border.length() > 0) {
										JSONObject sqlItem = sqlArray_border
												.getJSONObject(0);
										String path = HttpTool
												.GetString(sqlItem, "path",
														"201605151742");
										long downloadtime = HttpTool.GetLong(
												sqlItem, "downloadtime",
												"201605151743");
										long times = Tool.getTime() / 1000
												- downloadtime;
										if (new File(path).exists()
												&& times < 566400) {
											Log.i("show", path + "不需要更新书城按钮边框图");
											borderb = false;
										}
									}
									// 下载图片,写入数据库
									if (borderb) {
										final String dir_border = AppActivity.m_context
												.getApplicationContext()
												.getFilesDir()
												.getAbsolutePath();
										final String name_border = "bor_"
												+ Tool.getTime() + ".png";
										HttpTool.DownLoadImage(
												AppActivity.mQueue,
												AppActivity.getRequestId(),
												borderUrl,
												name_border,
												dir_border,
												new CallBackFunction.DownLoadPortart() {
													@Override
													public void doSomething(
															JSONObject json) {
														ContentValues value = new ContentValues();
														value.put("bookId",
																castleId);
														value.put("status", 7);
														value.put(
																"path",
																dir_border
																		+ File.separator
																		+ name_border);
														value.put(
																"downloadtime",
																Tool.getTime() / 1000);
														SQLiteDatabase db = new MySQLiteOpenHelper(
																context, "data")
																.getWritableDatabase();
														Tool.WriteSqlite(
																db,
																MacroCode.DB_RES,
																value,
																"bookId=? and status=7",
																new String[] { Integer
																		.toString(castleId) },
																1);
														db.close();
													}
												}, new JSONObject());
									}
								}
								db.close();
							}
						} catch (Exception e) {
							Log.e("201605151726", e.toString());
						}
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605151725", error.toString());
					}
				});
		request.setTag(requestID);
		mQueue.add(request);
	}
	
	//注册
	public static void http_AccountRegiste(RequestQueue mQueue,
			final AppActivity context, final int requestID,
			final String memberName, final String memberPasswd,
			final String rePasswd,final String inviteCode,
			final CallBackFunction callback){
		
		String url = MacroCode.IP + MacroCode.NET_REGISTER;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("memberName", memberName);
		map.put("memberPasswd", memberPasswd);
		map.put("rePasswd", rePasswd);
		if(inviteCode!=null){
			map.put("inviteCode", inviteCode);
		}		
		map.put("resource", MacroCode.RESOURCE);
		//Log.e("show",url.toString());
		//Log.e("show",map.toString());
		
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,
				new Response.Listener<JSONObject>() {
					@Override
					public void onResponse(JSONObject response) {
						//Log.e("show",response.toString());
						try{
							boolean result = HttpTool.GetBool(response, "result","201606291647");
							if(result)
							{
								callback.sendMessage("注册成功", 4);
								callback.sendMessage(response.toString(), MacroCode.LoadScene_AccountRegiste);
							}
							else
							{
								String errorMessage = HttpTool.GetString(response, "errorMessage","201606291723");
								callback.sendMessage("注册失败:"+errorMessage, 4);
							}
						}catch(Exception e)
						{
							callback.sendMessage("注册异常", 4);
						}						
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605121029", error.toString());
						callback.sendMessage("网络异常", 4);
					}

				});
		request.setTag(requestID);
		mQueue.add(request);
	}
	
	//添加下载记录
	public static void addDownLoadRecord(RequestQueue mQueue ,
			int memberId,
			int bookId			
			)
	{
		String url = MacroCode.IP +MacroCode.NET_ADDDOWNLOADRECODE;
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("memberId", Integer.toString(memberId));
		map.put("bookId", Integer.toString(bookId));
		map.put("resource", MacroCode.RESOURCE);
		Log.e("show", url+map.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map,new Response.Listener<JSONObject>() {

			@Override
			public void onResponse(JSONObject response) {
				Log.e("201607211627", response.toString());
				try{
					int code = response.getInt("code");
					if(code==0)
					{
						String desc = response.getString("desc");
					}
				}catch (Exception e) {
					Log.e("201607211046",e.toString());
				}
			}
		},  new ErrorListener() {

			@Override
			public void onErrorResponse(VolleyError error) {
				Log.e("201607211742", error.toString());
			}
		});
		mQueue.add(request);
	}
}
