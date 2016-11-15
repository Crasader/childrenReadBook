package org.cocos2dx.cpp;

import java.util.ArrayList;
import java.util.Map;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

public class commentAdapter extends BaseAdapter {
	Context context;
	ArrayList<Map<String, Object>> data;
	Typeface typeface;
	double scale;

	public commentAdapter(Context context,double scale) {
		this.context = context;
		data = new ArrayList<Map<String, Object>>();
		typeface = Typeface.createFromAsset(context.getAssets(),
				"res/FZHLJW.TTF");
		this.scale = scale;
	}

	public void LoadData(ArrayList<Map<String, Object>> list) {
		if(list == null)
			return;
		data = list;		
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
		CommentItem it;
		if (convertView == null) {
			it = new CommentItem(context,scale);
		}
		else {
			it = (CommentItem) convertView;
		}		
		it.bin((Double)data.get(arg0).get("starcount"), 
				(String)data.get(arg0).get("phone"), 
				(Long)data.get(arg0).get("time"), 
				(String)data.get(arg0).get("title"), 
				(String)data.get(arg0).get("content"));		
		return it;
	}

}
