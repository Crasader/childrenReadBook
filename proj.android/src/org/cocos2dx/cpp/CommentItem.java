package org.cocos2dx.cpp;

import java.text.SimpleDateFormat;
import java.util.Date;

import com.ellabook.R;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Typeface;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

public class CommentItem extends View {
	TextView title, phone, time, content;
	ImageView star1, star2, star3, star4, star5;
	ImageView[] imgs;
	double scale;
	static Bitmap star_s;// 选择的星星
	static Bitmap star;// 未选中的星星
	static Typeface tf;// 电话字体
	static Typeface tftime;// 时间字体

	public CommentItem(Context context, double scale) {
		super(context);
		inflate(context, R.layout.commentitem, null);
		title = (TextView) findViewById(R.id.title);
		star1 = (ImageView) findViewById(R.id.star1);
		star2 = (ImageView) findViewById(R.id.star2);
		star3 = (ImageView) findViewById(R.id.star3);
		star4 = (ImageView) findViewById(R.id.star4);
		star5 = (ImageView) findViewById(R.id.star5);
		phone = (TextView) findViewById(R.id.phone);
		time = (TextView) findViewById(R.id.time);
		content = (TextView) findViewById(R.id.content);
		imgs = new ImageView[] { star1, star2, star3, star4, star5 };
		this.scale = scale;
		Resources res = context.getResources();
		if (star_s == null)
			star_s = getBitmap(res, R.drawable.comment_star_s);
		if (star == null)
			star = getBitmap(res, R.drawable.comment_star);
		if (tf == null)
			tf = Typeface
					.createFromAsset(context.getAssets(), "res/FZHLJW.TTF");
		if (tftime == null)
			tftime = Typeface.createFromAsset(context.getAssets(),
					"res/wryh.TTF");
	}

	public Bitmap getBitmap(Resources res, int resId) {
		try {
			final BitmapFactory.Options options = new BitmapFactory.Options();
			options.inPreferredConfig = Bitmap.Config.RGB_565;
			options.inJustDecodeBounds = true;
			BitmapFactory.decodeResource(res, resId, options);
			options.inSampleSize = (int) scale;
			options.inJustDecodeBounds = false;
			Bitmap bmp = BitmapFactory.decodeResource(res, resId, options);
			return bmp;
		} catch (Exception e) {
			Log.e("201605231745", e.toString());
			return null;
		}
	}

	@SuppressLint("SimpleDateFormat") public void bin(double starcount, String phonetext, long times,
			String titletext, String contenttext) {
		try {
			// 标题
			if (titletext == null)
				titletext = "";
			title.setText(titletext);
			title.setTextColor(Color.rgb(255, 165, 0));
			title.setTextSize(20);
			title.setTypeface(tf);
			// 电话
			if (phonetext == null)
				phonetext = "136******27";
			else
				phonetext = phonetext.replaceAll(phonetext.substring(3, 7),
						"****");
			phone.setText(phonetext);
			phone.setTextColor(Color.rgb(25, 95, 135));
			phone.setTextSize(10);
			phone.setTypeface(tf);
			// 时间
			String t = timesOne(times);
			time.setText(t);
			time.setTextColor(Color.rgb(127, 127, 127));
			time.setTextSize(10);
			time.setTypeface(tftime);
			//内容
			content.setText(contenttext);
			content.setTextColor(Color.rgb(40, 40, 40));
			content.setTextSize(10);
			content.setTypeface(tf);
			// 星级
			int st = 5;
			if (starcount < 0 || starcount > 5)
				starcount = 5;
			else
				st = (int) Math.ceil(starcount);
			for (int i = 0; i < 5; i++) {
				if (i < st)
					imgs[i].setImageBitmap(star_s);
				else
					imgs[i].setImageBitmap(star);
			}
		} catch (Exception e) {
			Log.e("201605231746", e.toString());
		}
	}

	public String timesOne(long time) {
		try {
			SimpleDateFormat sdr = new SimpleDateFormat("yyyy-MM-dd");
			String times = sdr.format(new Date(time * 1000L));
			return times;
		} catch (Exception e) {
			Log.e("201605231747", e.toString());
			return "2016-01-02";
		}
	}
}
