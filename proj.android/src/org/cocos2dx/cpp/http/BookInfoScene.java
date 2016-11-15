package org.cocos2dx.cpp.http;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.MacroCode;
import org.cocos2dx.cpp.Tool;
import org.cocos2dx.cpp.http.CallBackFunction.DownLoadPortart;
import org.cocos2dx.cpp.sqlite.MySQLiteOpenHelper;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.ContentValues;
import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

import com.android.volley.Request.Method;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.Response.ErrorListener;
import com.android.volley.Response.Listener;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;

public class BookInfoScene {
	// auto strUrl =
	// std::string(IP).append(NET_BOOKINFO).append(StringUtils::format("%d",
	// BookId))
	// .append("?memberId=").append(App::getMemberID()).append("&resource=").append(App::GetInstance()->m_resource);
	// 网络请求书籍详细信息
	public static void http_GetBookInfo(RequestQueue mQueue,
			final Context context, final int requestID, final int BookId,
			final CallBackFunction callback) {
		String url = MacroCode.IP + MacroCode.NET_BOOKINFO + BookId
				+ "?memberId=-999" + "&resource=" + MacroCode.RESOURCE;
		//Log.i("show", url);
		StringRequest request = new StringRequest(Method.GET, url,
				new Response.Listener<String>() {
					@Override
					public void onResponse(String responsestr) {
						JSONObject response = null;
						boolean isDown = true;
						try {
							response = new JSONObject(responsestr);
						} catch (JSONException e1) {
							Log.e("201605191105", e1.toString());
						}
						//Log.i("show", response.toString());
						int code = HttpTool.GetInt(response, "code",
								"201605191106");
						if (code == 0) {
							JSONObject item = HttpTool.GetJSONObject(response,
									"data", "201605191107");
							SQLiteDatabase db = new MySQLiteOpenHelper(context,
									"data").getWritableDatabase();
							callback.sendMessage(responsestr,
									MacroCode.BookInfoScene_GetBookInfo);
							int bookid = HttpTool.GetInt(item, "bookId",
									"201605191110");
							int bookDownloadSize = HttpTool.GetInt(item,
									"bookDownloadSize", "201605191111");
							double avgScore100 = 100 * HttpTool.GetDouble(item,
									"avgScore", "201605191112");
							int bookPage = HttpTool.GetInt(item, "bookPage",
									"201605191113");
							String bookAge = HttpTool.GetString(item,
									"bookAge", "201605191114");
							int remainTime = HttpTool.GetInt(item,
									"remainTime", "201605191115");
							double bookPrice100 = 100 * HttpTool.GetDouble(
									item, "bookPrice", "201605191116");
							double bookmarketPrice100 = 100 * HttpTool
									.GetDouble(item, "bookmarketPrice",
											"201605191117");
							String bookName = HttpTool.GetString(item,
									"bookName", "201605191118");
							String bookPress = HttpTool.GetString(item,
									"bookPress", "201605191119");
							String bookCoverUrl = HttpTool.GetString(item,
									"bookCoverUrl", "201605191120");
							String bookPlayUrl = HttpTool.GetString(item,
									"bookPlayUrl", "201605191121");
							String bookViewUrl = HttpTool.GetString(item,
									"bookViewUrl", "201605191122");
							String bookIntroduction = HttpTool.GetString(item,
									"bookIntroduction", "201605191123");
							String bookAuthor = HttpTool.GetString(item,
									"bookAuthor", "201605191124");
							try {
								// 插入数据库
								//Log.i("show", "书籍详情插入数据库" + bookid);
								ContentValues value = new ContentValues();
								value.put("bookId", bookid);
								value.put("bookDownloadSize", bookDownloadSize);
								value.put("evaluation_good_star", avgScore100);
								value.put("bookPage", bookPage);
								value.put("bookAge", bookAge);
								value.put("remainTime", remainTime);
								value.put("bookPrice", bookPrice100);
								value.put("bookmarketPrice", bookmarketPrice100);
								value.put("bookName", bookName);
								value.put("bookPress", bookPress);
								value.put("bookPic", bookCoverUrl);
								value.put("bookPlayUrl", bookPlayUrl);
								value.put("bookViewUrl", bookViewUrl);
								value.put("upTime", Tool.getTime() / 1000);
								value.put("bookIntroduction", bookIntroduction);
								value.put("bookAuthor", bookAuthor);
								Tool.WriteSqlite(
										db,
										MacroCode.DB_BOOKINFO,
										value,
										"bookId=?",
										new String[] { Integer.toString(bookid) },
										1);
							} catch (Exception e) {
								Log.e("201605191159", e.toString());
							}
							// 先获取资源信息,然后判断是否要下载更新

							// 0是封面
							try {
								JSONArray sqlArray_border = Tool
										.ReadSqlite(
												db,
												MacroCode.DB_RES,
												new String[] { "path" },
												new String[] { "downloadtime" },
												"bookId=? and status=0",
												new String[] { Integer
														.toString(bookid) });
								if (sqlArray_border.length() > 0) {
									JSONObject sqlItem = sqlArray_border
											.getJSONObject(0);
									String path = HttpTool.GetString(sqlItem,
											"path", "201605191152");
									long downloadtime = HttpTool.GetLong(
											sqlItem, "downloadtime",
											"201605191153");
									long times = Tool.getTime() / 1000
											- downloadtime;
									if (new File(path).exists()
											&& times < 86400) {
										//Log.i("show", path + "不需要更新书籍封面");
										isDown = false;
									}
								}
								// 下载图片,写入数据库
								if (isDown) {
									final String dir = context
											.getApplicationContext()
											.getFilesDir().getAbsolutePath();
									final String name = "cov_" + Tool.getTime()
											+ ".png";
									JSONObject json = new JSONObject();
									json.put("bookId", bookid);
									HttpTool.DownLoadImage(
											AppActivity.mQueue,
											AppActivity.getRequestId(),
											bookCoverUrl,
											name,
											dir,
											new CallBackFunction.DownLoadPortart() {
												@Override
												public void doSomething(
														JSONObject json) {
													ContentValues value = new ContentValues();
													int bookid = HttpTool
															.GetInt(json,
																	"bookId",
																	"201605191158");
													if (bookid == -999)
														return;
													value.put("bookId", bookid);
													value.put("status", 0);
													value.put("path", dir
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
															"bookId=? and status=0",
															new String[] { Integer
																	.toString(bookid) },
															1);
													db.close();
													callback.sendMessage(
															dir
																	+ File.separator
																	+ name,
															MacroCode.BookInfoScene_DownLoadCover);
												}
											}, json);
								}
							} catch (Exception e) {
								Log.e("201605191159", e.toString());
							}
							db.close();
						}
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605191160", error.toString());
					}
				});
		request.setTag(requestID);
		mQueue.add(request);
	}

	//发表评论
	public static void Comment(final RequestQueue mQueue,
			final int memberId, 
			final int bookId,
			final String memberName,
			final String comment,
			final String title,
			final int score,
			final CallBackFunction callback)
	{
		callback.sendMessage("发表评论", 4);
		http_DidYouBuyThisBook(mQueue, bookId, memberId, -1, callback, new DownLoadPortart() {
			
			@Override
			public void doSomething(JSONObject json) {
				//{ "result": true, "num": 1, "code": "0", "order_id": [ "8497" ] }
				try{
					boolean result =json.getBoolean("result"); 
					if(result)
					{
						int num = json.getInt("num");
						if(num>0)
						{
							JSONArray ids = json.getJSONArray("order_id");
							String orderid= (String) ids.get(0);
							http_SendComment(mQueue, orderid, memberId, bookId, memberName, comment, title, score, callback);
						}	
						else
						{
							callback.sendMessage("尚未购买此书,购买后评论", 4);
						}
					}
				}catch(Exception e)
				{
					Log.e("201607041648",e.toString());
					callback.sendMessage("查询订单异常", 4);
				}
			}
		});
		
	}
	

	//网络请求发表评论
	public static void http_SendComment(RequestQueue mQueue,
			String orderId, 
			final int memberId, 
			final int bookId,
			String memberName,
			String comment,
			String title,
			int score,
			final CallBackFunction callback)
	{		
		//memberId=3606&memberName=18668125701&orderId=-999&bookId=325&comment=法师打发士大夫士大夫的说法 &title=返回时尚的回复&score=5
		String url = MacroCode.IP + MacroCode.NET_BOOKINFOSENDCOMMENT;
		Log.e("show", url);
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("bookId", Integer.toString(bookId));
		map.put("memberId", Integer.toString(memberId));
		map.put("orderId", orderId);
		map.put("resource", MacroCode.RESOURCE);
		map.put("memberName", memberName);
		map.put("comment", comment);
		map.put("title", title);
		map.put("score", Integer.toString(score));
		Log.e("show", map.toString());
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map, new Listener<JSONObject>() {

			@Override
			public void onResponse(JSONObject response) {
				Log.e("201607041741", response.toString());
				try{
					boolean result =response.getBoolean("result"); 
					if(result)
					{
						callback.sendMessage("发表成功", 4);
						callback.sendMessage("", MacroCode.BookInfoScene_SendComment);
					}
					else
					{
						int code = response.getInt("code");
						if(code == 3)
							callback.sendMessage("重复评论", 4);
						else
						{
							String errorMessage = response.getString("errorMessage");
							Log.e("201607050946",errorMessage);
							callback.sendMessage("发表失败", 4);
						}
					}
				}catch (Exception e) {
					Log.e("201607041709",e.toString());
					callback.sendMessage("发表异常", 4);
				}
			}
		}, new ErrorListener() {

			@Override
			public void onErrorResponse(VolleyError error) {
				Log.e("201607041742", error.toString());
				callback.sendMessage("网络异常", 4);
			}
		});
		mQueue.add(request);
	}
	
	//查询书籍是否购买 -1代表为空
	public static void http_DidYouBuyThisBook(RequestQueue mQueue,int bookid,
			int memberId,int orderid,final CallBackFunction callback,final CallBackFunction.DownLoadPortart finishDoSomething)
	{
		//http://cloud.ellabook.cn/ellabook-server/order/QueryOrderStatus.do		
		String url = MacroCode.IP + MacroCode.NET_ISBUY;
		//Log.i("show", url);
		HashMap<String, String> map = new HashMap<String, String>();
		map.put("bookid", Integer.toString(bookid));
		map.put("memberId", Integer.toString(memberId));
		map.put("orderid", Integer.toString(orderid));
		map.put("resource", MacroCode.RESOURCE);
		JsonObjectPostRequest request = new JsonObjectPostRequest(url, map, new Listener<JSONObject>() {

			@Override
			public void onResponse(JSONObject response) {
				Log.e("201605281217", response.toString());
				finishDoSomething.doSomething(response);
			}
		}, new ErrorListener() {

			@Override
			public void onErrorResponse(VolleyError error) {
				Log.e("201605281218", error.toString());
				callback.sendMessage("网络异常", 4);
			}
		});
		mQueue.add(request);
	}	
	
}
