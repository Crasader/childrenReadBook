package org.cocos2dx.cpp.http;

import java.io.File;

import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.MacroCode;
import org.cocos2dx.cpp.Tool;
import org.cocos2dx.cpp.sqlite.MySQLiteOpenHelper;
import org.json.JSONArray;
import org.json.JSONObject;

import android.content.ContentValues;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

import com.android.volley.Request.Method;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;

public class BookCityScene {

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
									;
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
												&& times < 86400) {
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
														callback.sendMessage("", MacroCode.BookCityScene_DownLoadPicture);
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
												&& times < 86400) {
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
														callback.sendMessage("", MacroCode.BookCityScene_DownLoadPicture);
													}
												}, new JSONObject());
									}
								}
								db.close();
							}
						} catch (Exception e) {
							Log.e("201605151726", e.toString());
						}
						callback.sendMessage("", MacroCode.BookCityScene_GetBookCityButtonsInfo);
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
}
