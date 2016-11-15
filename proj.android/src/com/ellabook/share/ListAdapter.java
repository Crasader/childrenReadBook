package com.ellabook.share;

import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.ellabook.R;

import android.annotation.SuppressLint;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class ListAdapter extends BaseAdapter {

	private LayoutInflater mInflater = null;  
	public Context context;
	List<Child> feedBackList;

	public ListAdapter(Context context,JSONArray data)  
	{  
		this.context=context;
		this.mInflater = LayoutInflater.from(context);
		feedBackList = new ArrayList<ListAdapter.Child>();
		try {
			for (int i = 0; i < data.length(); i++) {
				Child ch = new Child();
				JSONObject obj = (JSONObject) data.get(i);
				ch.memberid = obj.getString("memberid");
				ch.name = obj.getString("name");
				ch.phone = obj.getString("phone");

				ch.URL = obj.getString("URL");
				feedBackList.add(ch);
			}
		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}


	}
	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return feedBackList.size();
	}

	@Override
	public Object getItem(int arg0) {
		// TODO Auto-generated method stub
		return feedBackList.get(arg0);
	}

	@Override
	public long getItemId(int arg0) {
		// TODO Auto-generated method stub
		return arg0;
	}

	public  void addItem(Child item) {  
		feedBackList.add(item);  
	}
	@SuppressLint("ViewHolder")
	@Override
	public View getView(int arg0, View arg1, ViewGroup arg2) {
		// TODO 自动生成的方法存根
		arg1 = mInflater.inflate(R.layout.childlist_item,arg2,false); 
		TextView tv_name = (TextView) arg1.findViewById(R.id.tv_name);
		TextView tv_num = (TextView) arg1.findViewById(R.id.tv_num);
		tv_name.setText(feedBackList.get(arg0).name);
		tv_num.setText(feedBackList.get(arg0).phone);
		return arg1;
	}

	public class Child{
		//		"phone":"18758143120","memberid":"20180","name":"小王子","URL":"null"
		String name ;
		String phone ;
		String memberid;
		String URL;
	}

}
