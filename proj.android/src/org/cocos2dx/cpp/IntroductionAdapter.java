package org.cocos2dx.cpp;

import java.util.ArrayList;
import java.util.Map;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.TextView;

public class IntroductionAdapter extends BaseAdapter {
	Context context;
	ArrayList<String> data;
	Typeface typeface;

	public IntroductionAdapter(Context context) {
		this.context = context;
		data = new ArrayList<String>();
		typeface = Typeface.createFromAsset(context.getAssets(),
				"res/FZHLJW.TTF");
	}

	public void LoadData(Map<String, Object> map) {
		String bookIntroduction = Tool.GetString4Map("bookIntroduction", map);
		String bookPress = Tool.GetString4Map("bookPress", map);
		long bookPage = Tool.GetLong4Map("bookPage", map);
		String bookAuthor = Tool.GetString4Map("bookAuthor", map);
		data.clear();
		data.add("\t\t\t\t" + bookIntroduction);
		data.add("_____________________________________________________________________");
		data.add(bookPress);
		data.add("页数:\t" + Long.toString(bookPage));
		data.add("电子书作者:" + bookAuthor);		
	}

	@Override
	public int getCount() {
		return data.size();
	}

	@Override
	public Object getItem(int arg0) {
		return data.get(arg0);
	}

	@Override
	public long getItemId(int arg0) {
		return arg0;
	}

	@Override
	public View getView(int arg0, View convertView, ViewGroup arg2) {
		TextView it;
		if (convertView == null) {
			it = new TextView(context);
			it.setTextSize(10);
			it.setTypeface(typeface);
			it.setPadding(0, 0, 0, 30);
			it.setTextColor(Color.GRAY);
			switch (arg0) {
			case 0:
				it.setTextSize(15);
				break;
			case 1:
				it.setSingleLine();
				break;
			case 2:
				it.setTextColor(Color.rgb(40, 110, 170));
				break;
			}
		}
		else {
			it = (TextView) convertView;
		}
		it.setText(data.get(arg0));		
		return it;
	}

}
