package com.ellabook.share;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Calendar;

import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.MacroCode;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ActionBar.LayoutParams;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.database.DataSetObserver;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.os.AsyncTask;
import android.view.Display;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.view.View.OnTouchListener;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.Request.Method;
import com.android.volley.Response.Listener;
import com.android.volley.toolbox.StringRequest;
import com.ellabook.R;
import com.ellabook.share.PullToRefreshView.OnFooterRefreshListener;
import com.ellabook.share.PullToRefreshView.OnHeaderRefreshListener;
import com.umeng.socialize.PlatformConfig.QQZone;
import com.umeng.socialize.ShareAction;
import com.umeng.socialize.UMShareAPI;
import com.umeng.socialize.UMShareListener;
import com.umeng.socialize.bean.SHARE_MEDIA;
import com.umeng.socialize.media.UMImage;
import com.umeng.socialize.utils.Log;


public class MySharePresenter {

	public static final int SINA = 1;
	public static final int QQ = 2;
	public static final int QZONE = 3;
	public static final int WEIXIN = 4;
	public static final int WEIXIN_CIRCLE = 5;
	public static final int WEIXIN_FAVORITE = 6;
	public static final int ALL = 0;
	static PopupWindow popupWindow ;
	static Animation clickAnimation ;

	private static String bookname;

	/*
	 * 邀请分享
	 */
	public static void showInviteregisterPop(final Context context,final RequestQueue mQueue,final String targetUrl , final String title,final int memberID ,final String text,final UMShareListener umShareListener){
		final Dialog dialog = new Dialog(context , R.style.dialog);

		final View contentView =LayoutInflater.from(context).inflate(R.layout.inviteregisterpopwindow, null);

		final RelativeLayout rela1 = (RelativeLayout) contentView.findViewById(R.id.rela1);
		final RelativeLayout rela2 = (RelativeLayout) contentView.findViewById(R.id.rela2);
		rela2.setVisibility(View.GONE);
		final RelativeLayout rela3 = (RelativeLayout) contentView.findViewById(R.id.rela3);
		rela3.setVisibility(View.GONE);
		final Button close = (Button) contentView.findViewById(R.id.close);
		clickAnimation= AnimationUtils.loadAnimation(context, R.anim.button_anim);  

		Button weixin = (Button) contentView.findViewById(R.id.weixin);
		Button pengyouquan = (Button) contentView.findViewById(R.id.penyouquan);
		Button qq = (Button) contentView.findViewById(R.id.qq);
		Button qqzone = (Button) contentView.findViewById(R.id.qqzone);

		dialog.setContentView(contentView);

		dialog.setCanceledOnTouchOutside(true);

		Animation anim = AnimationUtils.loadAnimation(context,
				R.anim.small_2_big);
		contentView.startAnimation(anim);
		dialog.show();
		close.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub

				dialog.dismiss();
			}
		});
		weixin.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				shareWithFilePath(WEIXIN, context, targetUrl, title, text, umShareListener);
				dialog.dismiss();
			}
		});

		pengyouquan.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				shareWithFilePath(WEIXIN_CIRCLE, context, targetUrl, title, text, umShareListener);
				dialog.dismiss();
			}
		});
		qq.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				shareWithFilePath(QQ, context, targetUrl, title, text, umShareListener);
				dialog.dismiss();
			}
		});
		qqzone.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				shareWithFilePath(QZONE, context, targetUrl, title, text, umShareListener);
				dialog.dismiss();
			}
		});
		RelativeLayout detailedrules = (RelativeLayout) contentView.findViewById(R.id.detailedrules);
		RelativeLayout myinvitation = (RelativeLayout) contentView.findViewById(R.id.myinvitation);

		detailedrules.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				rela1.setVisibility(View.GONE);
				rela2.setVisibility(View.VISIBLE);
			}
		});
		Button back = (Button) contentView.findViewById(R.id.back);
		final PullToRefreshView mPullToRefreshView = (PullToRefreshView) contentView.findViewById(R.id.pull_to_refresh);
		final ListView listView = (ListView) contentView.findViewById(R.id.list_view);
		final TextView tv_text = (TextView) contentView.findViewById(R.id.tv_text);
		mPullToRefreshView.setOnHeaderRefreshListener(new OnHeaderRefreshListener() {

			@Override
			public void onHeaderRefresh(PullToRefreshView view) {
				// TODO Auto-generated method stub
				view.postDelayed(new Runnable() {
					public void run() {

						mPullToRefreshView.onHeaderRefreshComplete();
					}
				}, 1000);
			}
		});
		mPullToRefreshView.setOnFooterRefreshListener(new OnFooterRefreshListener() {

			@Override
			public void onFooterRefresh(PullToRefreshView view) {
				// TODO Auto-generated method stub
				view.postDelayed(new Runnable() {
					public void run() {

						mPullToRefreshView.onFooterRefreshComplete();
					}
				}, 1000);
			}
		});
		mPullToRefreshView.setFooter(true);
		back.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				rela2.setVisibility(View.GONE);
				rela1.setVisibility(View.VISIBLE);
			}
		});
		myinvitation.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				final String url = MacroCode.IP + MacroCode.NET_GETAVATAR + "?memberId=" + memberID;
				Log.e("show", url);
//				new Thread(new Runnable() {
//
//					public void run() {
						StringRequest request = new StringRequest(Method.GET, url,
								new Listener<String>() {
							@Override
							public void onResponse(String response) {
								System.out.println(response);
								JSONObject obj;
								try {
									obj = new JSONObject(response);
									if(obj.getBoolean("result")){
										JSONArray data = obj.getJSONArray("data");
										listView.setAdapter(new com.ellabook.share.ListAdapter(context , data));
										rela1.setVisibility(View.GONE);
										rela3.setVisibility(View.VISIBLE);
										tv_text.setText("已成功邀请"+data.length()+"人，累计红包"+data.length()*10+"元");
									}
								} catch (JSONException e) {
									// TODO Auto-generated catch block
									e.printStackTrace();
								}
								
								
							}

						}, new Response.ErrorListener() {
							@Override
							public void onErrorResponse(VolleyError error) {
								Log.e("201608161705", error.toString());
							}
						});
						mQueue.add(request);
//					}
//					
//				}).start();
				

			}
		});
		Button back2 = (Button) contentView.findViewById(R.id.back2);
		back2.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				rela3.setVisibility(View.GONE);
				rela1.setVisibility(View.VISIBLE);
			}
		});
		
	}

	private static void getListFromOnline(int memberID) {
		// TODO Auto-generated method stub
		
	}

	/*
	 * 页面分享主方法
	 */
	public static void showSharePop(final Context context,String filePath,String bookName ,final String targetUrl ,String headUrl, final String title,final UMShareListener umShareListener){
		bookname = bookName;
		final Dialog dialog = new Dialog(context , R.style.dialog);

		final View contentView =LayoutInflater.from(context).inflate(R.layout.sharepopwindow, null);

		final RelativeLayout rela1 = (RelativeLayout) contentView.findViewById(R.id.rela1);
		final Button close = (Button) contentView.findViewById(R.id.close);
		clickAnimation= AnimationUtils.loadAnimation(context, R.anim.button_anim);  

		Button weixin = (Button) contentView.findViewById(R.id.weixin);
		Button pengyouquan = (Button) contentView.findViewById(R.id.pengyouquan);
		Button qq = (Button) contentView.findViewById(R.id.qq);
		Button qqzone = (Button) contentView.findViewById(R.id.qqzone);

		TextView booknane = (TextView) contentView.findViewById(R.id.bookname);
		booknane.setText("书名是《"+bookName+"》");

		ImageView head = (RoundImageView) contentView.findViewById(R.id.head);
		FileInputStream f;
		try {
			f = new FileInputStream(headUrl);
			Bitmap bm = null; 
			BitmapFactory.Options options = new BitmapFactory.Options(); 
			options.inSampleSize = 1;//图片的长宽都是原来的1/8 
			BufferedInputStream bis = new BufferedInputStream(f); 
			bm = BitmapFactory.decodeStream(bis, null, options); 
			head.setImageBitmap(bm); 
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 


		RoundImageView img =(RoundImageView) contentView.findViewById(R.id.img2);
		img.setRectAdius(10);
		FileInputStream f1;
		try {
			f1 = new FileInputStream(filePath);
			Bitmap bm = null; 
			BitmapFactory.Options options = new BitmapFactory.Options(); 
			options.inSampleSize = 1;//图片的长宽都是原来的1/8 
			BufferedInputStream bis = new BufferedInputStream(f1); 
			bm = BitmapFactory.decodeStream(bis, null, options); 
			img.setImageBitmap(bm);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 



		dialog.setContentView(contentView);

		dialog.setCanceledOnTouchOutside(true);

		Animation anim = AnimationUtils.loadAnimation(context,
				R.anim.small_2_big);
		contentView.startAnimation(anim);
		dialog.show();
		close.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub

				dialog.dismiss();
			}
		});
		weixin.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				close.setVisibility(View.GONE);
				shareBigImg(WEIXIN, context, rela1, targetUrl, title, umShareListener);
				close.setVisibility(View.VISIBLE);
				dialog.dismiss();
			}
		});

		pengyouquan.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				close.setVisibility(View.GONE);
				shareBigImg(WEIXIN_CIRCLE, context, rela1, targetUrl, title, umShareListener);
				close.setVisibility(View.VISIBLE);
				dialog.dismiss();
			}
		});
		qq.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				close.setVisibility(View.GONE);
				shareBigImg(QQ, context, rela1, targetUrl, title, umShareListener);
				close.setVisibility(View.VISIBLE);
				dialog.dismiss();
			}
		});
		qqzone.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				close.setVisibility(View.GONE);
				shareBigImg(QQ, context, rela1, targetUrl, title, umShareListener);
				close.setVisibility(View.VISIBLE);
				dialog.dismiss();
			}
		});
	}

	/*
	 *普通分享，图片为本地图片
	 */
	public static void shareWithFilePath(int type ,Context context , String targetUrl , String title,String text ,UMShareListener umShareListener){
		UMImage image = new UMImage(context, R.drawable.icon);
		switch (type) {
		case ALL:
			new ShareAction((Activity) context).setDisplayList(SHARE_MEDIA.SINA,SHARE_MEDIA.QQ,SHARE_MEDIA.QZONE,SHARE_MEDIA.WEIXIN,SHARE_MEDIA.WEIXIN_CIRCLE,SHARE_MEDIA.WEIXIN_FAVORITE)
			.withTitle(title)
			.withText(text)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.setCallback(umShareListener)
			.open();
			break;
		case SINA:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.SINA).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case QQ:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.QQ).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case QZONE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.QZONE).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case WEIXIN:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case WEIXIN_CIRCLE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN_CIRCLE).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case WEIXIN_FAVORITE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN_FAVORITE).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		default:
			break;
		}


	}

	/*
	 *普通分享，图片为网络
	 */
	public static void shareWithURL (int type ,Context context ,String imgUrl , String targetUrl , String title,String text ,UMShareListener umShareListener){

		UMImage image = new UMImage(context, imgUrl);
		switch (type) {
		case ALL:
			new ShareAction((Activity) context).setDisplayList(SHARE_MEDIA.SINA,SHARE_MEDIA.QQ,SHARE_MEDIA.QZONE,SHARE_MEDIA.WEIXIN,SHARE_MEDIA.WEIXIN_CIRCLE,SHARE_MEDIA.WEIXIN_FAVORITE)
			.withTitle(title)
			.withText(text)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.setCallback(umShareListener)
			.open();
			break;
		case SINA:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.SINA).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case QQ:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.QQ).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case QZONE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.QZONE).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case WEIXIN:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case WEIXIN_CIRCLE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN_CIRCLE).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case WEIXIN_FAVORITE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN_FAVORITE).setCallback(umShareListener)
			.withText(text)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		default:
			break;
		}



	}

	/*
	 *大图分享 直接截图分享
	 */
	public static void shareBigImg(int type, Context context , View view,String targetUrl , String title,UMShareListener umShareListener){
		UMImage image = new UMImage(context, myShot(view));
		UMShareAPI mShareAPI = UMShareAPI.get(context);
		switch (type) {
		case ALL:
			new ShareAction((Activity) context).setDisplayList(SHARE_MEDIA.SINA,SHARE_MEDIA.QQ,SHARE_MEDIA.QZONE,SHARE_MEDIA.WEIXIN,SHARE_MEDIA.WEIXIN_CIRCLE,SHARE_MEDIA.WEIXIN_FAVORITE)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.setCallback(umShareListener)
			.open();
			break;
		case SINA:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.SINA).setCallback(umShareListener)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case QQ:
			if(mShareAPI.isInstall((Activity) context, SHARE_MEDIA.QQ)){
				new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.QQ).setCallback(umShareListener)
				.withTitle(title)
				.withMedia(image)
				.withTargetUrl(targetUrl)
				.share();
			}else{
				AppActivity.cocosToast("没有QQ客户端");
			}

			break;
		case QZONE:
			if(mShareAPI.isInstall((Activity) context, SHARE_MEDIA.QZONE)){
				System.out.println(targetUrl);
				new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.QZONE).setCallback(umShareListener)
				.withTitle("咿啦看书")
				.withMedia(image)
				.withText("我家宝贝发现了一本好书，分享给你看！"+"书名是《"+bookname+"》")
				.withTargetUrl(targetUrl)
				.share();
			}else{
				AppActivity.cocosToast("没有QZONE客户端");
			}

			break;
		case WEIXIN:
			if(mShareAPI.isInstall((Activity) context, SHARE_MEDIA.WEIXIN)){
				new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN).setCallback(umShareListener)
				.withTitle(title)
				.withMedia(image)
				.withTargetUrl(targetUrl)
				.share();
			}else{
				AppActivity.cocosToast("没有微信客户端");
			}


			break;
		case WEIXIN_CIRCLE:
			if(mShareAPI.isInstall((Activity) context, SHARE_MEDIA.WEIXIN)){
				new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN_CIRCLE).setCallback(umShareListener)
				.withTitle(title)
				.withMedia(image)
				.withTargetUrl(targetUrl)
				.share();
			}else{
				AppActivity.cocosToast("没有微信客户端");
			}

			break;
		case WEIXIN_FAVORITE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN_FAVORITE).setCallback(umShareListener)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		default:
			break;
		}
	}

	/*
	 *大图分享，图片为本地图片 ,看了需求 ，暂时用这个
	 */
	public static void shareBigImgWithFilePath(int type ,Context context ,String filePath , String targetUrl , String title,UMShareListener umShareListener){
		UMImage image = new UMImage(context, new File(filePath));
		switch (type) {
		case ALL:
			new ShareAction((Activity) context).setDisplayList(SHARE_MEDIA.SINA,SHARE_MEDIA.QQ,SHARE_MEDIA.QZONE,SHARE_MEDIA.WEIXIN,SHARE_MEDIA.WEIXIN_CIRCLE,SHARE_MEDIA.WEIXIN_FAVORITE)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.setCallback(umShareListener)
			.open();
			break;
		case SINA:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.SINA).setCallback(umShareListener)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case QQ:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.QQ).setCallback(umShareListener)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case QZONE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.QZONE).setCallback(umShareListener)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case WEIXIN:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN).setCallback(umShareListener)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case WEIXIN_CIRCLE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN_CIRCLE).setCallback(umShareListener)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		case WEIXIN_FAVORITE:
			new ShareAction((Activity) context).setPlatform(SHARE_MEDIA.WEIXIN_FAVORITE).setCallback(umShareListener)
			.withTitle(title)
			.withMedia(image)
			.withTargetUrl(targetUrl)
			.share();
			break;
		default:
			break;
		}
	}

	/*
	 * 截图
	 */
	@SuppressLint("NewApi") public static Bitmap myShot(View view) {
		// 获取windows中最顶层的view
		view.buildDrawingCache();

//		int statusBarHeights = 0;
//		Display display =view.getDisplay();
//
//		int widths = display.getWidth();
//		int heights = display.getHeight();

		// 允许当前窗口保存缓存信息
		view.setDrawingCacheEnabled(true);

		// 去掉状态栏
		Bitmap bmp = Bitmap.createBitmap(view.getDrawingCache());

		// 销毁缓存信息
		view.destroyDrawingCache();

		return bmp;
	}


}
