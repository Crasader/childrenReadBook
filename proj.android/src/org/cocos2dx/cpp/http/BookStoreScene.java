package org.cocos2dx.cpp.http;

import java.io.File;

import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.MacroCode;
import org.cocos2dx.cpp.Tool;
import org.cocos2dx.cpp.sqlite.MySQLiteOpenHelper;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.ContentValues;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

import com.android.volley.Request.Method;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.StringRequest;

public class BookStoreScene {

	// 网络请求书店当前页信息
	public static void http_GetBookStorePageInfo(RequestQueue mQueue,
			final AppActivity context, final int requestID,
			final int BookStoreId, final int pageIndex, 
			String visitFrom, final CallBackFunction callback) {
		String url = MacroCode.IP + MacroCode.NET_BOOKSID + BookStoreId
				+ "?page=" + pageIndex + "&pagesize=8" + "&resource="
				+ MacroCode.RESOURCE + "&memberId=-999" + "&visitFrom="
				+ visitFrom;
		Log.i("show", url);
		StringRequest request = new StringRequest(Method.GET, url,
				 new Response.Listener<String>() {
					@Override
					public void onResponse(String responsestr) {
						JSONObject response = null;
						try {
							response = new JSONObject(responsestr);
						} catch (JSONException e1) {
							Log.e("201605181200",e1.toString());
						}
						Log.i("show", response.toString());
						try {
							int code = HttpTool.GetInt(response, "code",
									"201605171808");
							if (code == 0) {
								JSONArray array = HttpTool.GetJSONArray(
										response, "data", "201605171807");
								SQLiteDatabase db = new MySQLiteOpenHelper(
										context, "data").getWritableDatabase();
								callback.sendMessage(
										responsestr,
										MacroCode.BookStoreScene_GetBookStorePageInfo);
								for (int i = 0; i < array.length(); i++) {
									JSONObject item = array.getJSONObject(i);
									int sort = (pageIndex - 1) * 8 + i;
									int bookId = HttpTool.GetInt(item,
											"bookId", "201605171806");
									String bookCoverUrl = HttpTool.GetString(
											item, "bookCoverUrl",
											"201605171800");
									int bookPrice100 = (int) (100 * HttpTool
											.GetDouble(item, "bookPrice",
													"201605171801"));
									long upTime = Tool.getTime();

									// 插入数据库
									Log.i("show", "书店内的书籍列表信息插入数据库"
											+ BookStoreId);
									ContentValues value = new ContentValues();
									value.put("castleId", BookStoreId);
									value.put("bookId", bookId);
									value.put("sort", sort);
									value.put("upTime", upTime);
									value.put("coverURL", bookCoverUrl);
									value.put("price", bookPrice100);
									Tool.WriteSqlite(
											db,
											MacroCode.DB_BOOKLIST,
											value,
											"sort=? and castleId=?",
											new String[] {
													Integer.toString(sort),
													Integer.toString(BookStoreId) },
											i + 1);
									// 先获取资源信息,然后判断是否要下载更新

									// 0是封面
									JSONArray sqlArray_border = Tool
											.ReadSqlite(
													db,
													MacroCode.DB_RES,
													new String[] { "path" },
													new String[] { "downloadtime" },
													"bookId=? and status=0",
													new String[] { Integer
															.toString(bookId) });
									if (sqlArray_border.length() > 0) {
										JSONObject sqlItem = sqlArray_border
												.getJSONObject(0);
										String path = HttpTool
												.GetString(sqlItem, "path",
														"201605171802");
										long downloadtime = HttpTool.GetLong(
												sqlItem, "downloadtime",
												"201605171803");
										long times = Tool.getTime() / 1000
												- downloadtime;
										if (new File(path).exists()
												&& times < 86400) {
											Log.i("show", path + "不需要更新书籍封面");
											continue;
										}
									}
									// 下载图片,写入数据库
									final String dir = AppActivity.m_context
											.getApplicationContext()
											.getFilesDir().getAbsolutePath();
									final String name = "cov_" + Tool.getTime()
											+ ".png";
									JSONObject json = new JSONObject();
									json.put("bookId", bookId);
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
													int bookid = HttpTool.GetInt(json, "bookId", "2016051711709");
													if(bookid == -999)
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
															"",
															MacroCode.BookStoreScene_DownLoadCover);
												}
											},json );
								}
								db.close();
							}
						} catch (Exception e) {
							Log.e("201605171804", e.toString());
						}
					}
				}, new Response.ErrorListener() {
					@Override
					public void onErrorResponse(VolleyError error) {
						Log.e("201605171805", error.toString());
					}
				});
		request.setTag(requestID);
		mQueue.add(request);
	}
}
