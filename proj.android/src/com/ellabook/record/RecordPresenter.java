package com.ellabook.record;

import java.io.File;
import java.io.IOException;
import java.util.Collections;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.mime.MultipartEntity;
import org.apache.http.entity.mime.content.FileBody;
import org.apache.http.entity.mime.content.StringBody;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.cocos2dx.cpp.AppActivity;
import org.cocos2dx.cpp.MacroCode;
import org.json.JSONObject;

import com.ellabook.R;
import com.ellabook.record.AudioRecordView.AudioRecordFinishListener;
import com.ellabook.share.TileButton;
import android.view.WindowManager;
import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnKeyListener;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

public class RecordPresenter {
	//在线播放mp3格式
	MediaPlayer mediaPlayer ;
	private static boolean isPause;
	private RecorderAndPlayUtil mRecorderUtil = null;

	int isonpage = 0;
	/** 录音文件路径 */
	private String mAudioRecordFilePath = null;
	private StreamingMediaPlayer audioStreamer;
	static String RECORD_FILENAME = "ellabook/voiceRecord/"; // 录音文件名
	private Context context;
	private TileButton back,fork,star1,star2,star3,star4,star5,button1,button2,voice,text,audioRecordViewbutton;
	private RoundProgressBar mCirclePercentView;
	private TextView textview;
	private VoiceView voiceview;
	private AudioRecordView mAudioRecordView; 
	private int starNum = 5;
	private int bookId, memberId ,tyPe ;
	private String gevalTime, comment, membername , orderId,runkey,errorkey,closekey;
	private Dialog dialog ;
	private RelativeLayout rela,choserela;
	private LinearLayout starlin;
	private FrameLayout voicefra ;
	int wi ,hi;
	private WindowManager mag;
	public void show(Context context ,int bookId,String membername,int memberId ,String orderId ,int tyPe,String runKey, String errorKey,String closeKey,WindowManager mag){
		this.context=context;
		this.bookId =bookId;
		this.memberId=memberId;
		this.gevalTime="1";
		this.comment="";
		this.membername=membername;
		this.orderId=orderId;
		this.tyPe = tyPe;
		this.runkey = runKey;
		this.errorkey = errorKey;
		this.closekey = closeKey;
		this.mag = mag;
		DisplayMetrics displayMetrics = new DisplayMetrics();
		mag.getDefaultDisplay().getMetrics(displayMetrics);
		dialog = new Dialog(context , R.style.dialog);

		final View contentView =LayoutInflater.from(context).inflate(R.layout.voicepopwindow, null);

		//总布局
		rela = (RelativeLayout)contentView.findViewById(R.id.rela);
		LinearLayout.LayoutParams line= (android.widget.LinearLayout.LayoutParams) rela.getLayoutParams();
		line.width = (int)(displayMetrics.widthPixels*0.45);
		wi = line.width;
		line.height = (int)(wi/241.5*252.5);
		hi = line.height;
		rela.setLayoutParams(line);

		//星星lin
		starlin = (LinearLayout) contentView.findViewById(R.id.starlin);
		RelativeLayout.LayoutParams starlinline= (android.widget.RelativeLayout.LayoutParams) starlin.getLayoutParams();
		starlinline.height = (int)(wi/241.5*29);
		starlinline.setMargins((int)(wi/241.5*35),(int)(wi/241.5*75),(int)(wi/241.5*35),(int)(wi/241.5*0));
		starlin.setLayoutParams(starlinline);

		//俩按钮rela
		choserela = (RelativeLayout)contentView.findViewById(R.id.choserela);
		RelativeLayout.LayoutParams choserelaline= (android.widget.RelativeLayout.LayoutParams) choserela.getLayoutParams();
		choserelaline.height = (int)(wi/241.5*30.5);
		choserelaline.setMargins((int)(wi/241.5*27),(int)(wi/241.5*203.5),(int)(wi/241.5*27),(int)(wi/241.5*0));
		choserela.setLayoutParams(choserelaline);

		//大按钮fra
		voicefra =(FrameLayout) contentView.findViewById(R.id.voicefra);
		RelativeLayout.LayoutParams voicefraline = (android.widget.RelativeLayout.LayoutParams) voicefra.getLayoutParams();
		voicefraline.width=(int)(wi/241.5*70.25);
		voicefraline.height=(int)(wi/241.5*73.5);
		voicefraline.setMargins((int)(wi/241.5*0),(int)(wi/241.5*117.5),(int)(wi/241.5*0),(int)(wi/241.5*0));
		voicefra.setLayoutParams(voicefraline);

		//数字
		textview = (TextView) contentView.findViewById(R.id.tv);
		RelativeLayout.LayoutParams textviewline= (android.widget.RelativeLayout.LayoutParams) textview.getLayoutParams();
		textviewline.setMargins(0,(int)(wi/241.5*104.165),0,0);
		textview.setLayoutParams(textviewline);


		mAudioRecordView = (AudioRecordView) contentView.findViewById(R.id.audioRecordView);
		mAudioRecordView.setAudioRecordFinishListener(new MyAudioRecordFinishListener());

		//进度条
		mCirclePercentView=(RoundProgressBar) contentView.findViewById(R.id.circleview);
		FrameLayout.LayoutParams mCirclePercentViewline= (android.widget.FrameLayout.LayoutParams) mCirclePercentView.getLayoutParams();
		mCirclePercentViewline.height = (int)(wi/241.5*64);
		mCirclePercentViewline.width = (int)(wi/241.5*64);
		mCirclePercentViewline.setMargins((int)(wi/241.5*0),(int)(wi/241.5*0),(int)(wi/241.5*0),(int)(wi/241.5*2));
		mCirclePercentView.setLayoutParams(mCirclePercentViewline);

		//录音波浪
		voiceview = (VoiceView) contentView.findViewById(R.id.voiceview);
		FrameLayout.LayoutParams voiceviewline= (android.widget.FrameLayout.LayoutParams) voiceview.getLayoutParams();
		voiceviewline.height = (int)(wi/241.5*63.58);
		voiceviewline.width = (int)(wi/241.5*63.58);
		voiceviewline.setMargins((int)(wi/241.5*3.33),(int)(wi/241.5*0),(int)(wi/241.5*0),(int)(wi/241.5*3.33));
		voiceview.setLayoutParams(voiceviewline);

		//返回
		back = (TileButton) contentView.findViewById(R.id.back);
		RelativeLayout.LayoutParams backline= (android.widget.RelativeLayout.LayoutParams) back.getLayoutParams();
		backline.height = (int)(wi/241.5*29.5);
		backline.width = (int)(wi/241.5*34);
		backline.setMargins((int)(wi/241.5*13.325),(int)(wi/241.5*10),(int)(wi/241.5*0),(int)(wi/241.5*0));
		back.setLayoutParams(backline);
		back.setVisibility(View.GONE);

		//关闭
		fork = (TileButton) contentView.findViewById(R.id.close);
		RelativeLayout.LayoutParams forkline= (android.widget.RelativeLayout.LayoutParams) fork.getLayoutParams();
		forkline.height = (int)(wi/241.5*29.5);
		forkline.width = (int)(wi/241.5*34);
		forkline.setMargins((int)(wi/241.5*0),(int)(wi/241.5*10),(int)(wi/241.5*13.325),(int)(wi/241.5*0));
		fork.setLayoutParams(forkline);

		//星星
		star1 = (TileButton) contentView.findViewById(R.id.star1);
		LinearLayout.LayoutParams star1line= (android.widget.LinearLayout.LayoutParams) star1.getLayoutParams();
		star1line.height = (int)(wi/241.5*29);
		star1line.width = (int)(wi/241.5*29);
		star1.setLayoutParams(star1line);

		star2 = (TileButton) contentView.findViewById(R.id.star2);
		LinearLayout.LayoutParams star2line= (android.widget.LinearLayout.LayoutParams) star2.getLayoutParams();
		star2line.height = (int)(wi/241.5*29);
		star2line.width = (int)(wi/241.5*29);
		star2line.setMargins((int)(wi/241.5*6.65),(int)(wi/241.5*0),(int)(wi/241.5*0),(int)(wi/241.5*0));
		star2.setLayoutParams(star2line);
		star3 = (TileButton) contentView.findViewById(R.id.star3);
		star3.setLayoutParams(star2line);
		star4 = (TileButton) contentView.findViewById(R.id.star4);
		star4.setLayoutParams(star2line);
		star5 = (TileButton) contentView.findViewById(R.id.star5);
		star5.setLayoutParams(star2line);

		button1 = (TileButton) contentView.findViewById(R.id.button1);
//		android:layout_width="75dp"
//		android:layout_height="30.5dp"
		RelativeLayout.LayoutParams button1line= (android.widget.RelativeLayout.LayoutParams) button1.getLayoutParams();
		button1line.height = (int)(wi/241.5*30.5);
		button1line.width = (int)(wi/241.5*75);
		button1.setLayoutParams(button1line);

		button2 = (TileButton) contentView.findViewById(R.id.button2);
		RelativeLayout.LayoutParams button2line= (android.widget.RelativeLayout.LayoutParams) button2.getLayoutParams();
		button2line.height = (int)(wi/241.5*30.5);
		button2line.width = (int)(wi/241.5*75);
		button2.setLayoutParams(button2line);


		voice = (TileButton) contentView.findViewById(R.id.voice);
//		android:layout_width="70.25dp"
//		android:layout_height="73.5dp"
		FrameLayout.LayoutParams voiceline= (android.widget.FrameLayout.LayoutParams) voice.getLayoutParams();
		voiceline.height = (int)(wi/241.5*73.5);
		voiceline.width = (int)(wi/241.5*70.25);
		voice.setLayoutParams(voiceline);

		audioRecordViewbutton = (TileButton) contentView.findViewById(R.id.audioRecordViewbutton);
		LinearLayout.LayoutParams audioRecordViewbuttonline= (android.widget.LinearLayout.LayoutParams) audioRecordViewbutton.getLayoutParams();
		audioRecordViewbuttonline.height = (int)(wi/241.5*73.5);
		audioRecordViewbuttonline.width = (int)(wi/241.5*70.25);
		audioRecordViewbutton.setLayoutParams(audioRecordViewbuttonline);

		text = (TileButton) contentView.findViewById(R.id.text);
//		android:layout_width="117.5dp"
//		android:layout_height="8.75dp"
//		android:layout_centerHorizontal="true"
//		android:layout_marginTop="203.5dp"
		RelativeLayout.LayoutParams textline= (android.widget.RelativeLayout.LayoutParams) text.getLayoutParams();
		textline.height = (int)(wi/241.5*8.75);
		textline.width = (int)(wi/241.5*117.5);
		textline.setMargins(0,(int)(wi/241.5*203.5),0,0);
		text.setLayoutParams(textline);
		back.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				back();
				AppActivity.NetInterfaceCallback(closekey, "");
			}
		});
		fork.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				back();
				AppActivity.NetInterfaceCallback(closekey, "");
			}
		});
		show1();
		dialog.setContentView(contentView);

		dialog.setCanceledOnTouchOutside(false);

		Animation anim = AnimationUtils.loadAnimation(context,
				R.anim.small_2_big);
		contentView.startAnimation(anim);


		dialog.show();
		dialog.setOnKeyListener(new OnKeyListener() {

			@Override
			public boolean onKey(DialogInterface arg0, int arg1, KeyEvent arg2) {
				// TODO Auto-generated method stub
				if (arg1 == KeyEvent.KEYCODE_BACK  && arg2.getRepeatCount() == 0) {  
					return true; 
				}
				return false; 
			}
		});
	}

	private void show1(){
		isonpage=1;
		voice.setBackgroundResource(R.drawable.voicecomment_voice_button);
		voice.setEnabled(true);
		textview.setVisibility(View.GONE);
		mAudioRecordView.setVisibility(View.GONE);
		text.setVisibility(View.VISIBLE);
		mCirclePercentView.setVisibility(View.GONE);
		button1.setVisibility(View.GONE);
		button2.setVisibility(View.GONE);
		voiceview.setVisibility(View.GONE);
		starPlay();


		voice.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mAudioRecordView.down(voiceview);
				show2();
				System.out.println(".");
			}
		});
	}

	private void show2() {
		// TODO Auto-generated method stub
		isonpage=2;
		stopAllOtherVoice();
		voice.setBackgroundResource(R.drawable.voicecomment_voice_button_press);
		textview.setVisibility(View.GONE);
		voice.setEnabled(false);
		mAudioRecordView.setVisibility(View.VISIBLE);
		mCirclePercentView.setVisibility(View.GONE);
		button1.setVisibility(View.VISIBLE);
		button2.setVisibility(View.VISIBLE);
		voiceview.setVisibility(View.VISIBLE);
		button1.setBackgroundResource(R.drawable.voicecomment_voice_cancel_button);
		button2.setBackgroundResource(R.drawable.voicecomment_voice_finish_button_press);
		button1.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mAudioRecordView.up();
				mAudioRecordView.deleteRecorderPath();
				show1();
			}
		});
		button2.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mAudioRecordView.up();
				show3();

			}
		});
		text.setVisibility(View.GONE);
	}


	public Handler recordHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			//            setDialogImage();
			switch (msg.what) {
			case 2:
				toOnline();
				break;
			case 3:
//				Toast.makeText(context, "评论成功", Toast.LENGTH_SHORT).show();	
				back();
				AppActivity.NetInterfaceCallback(runkey, "");
				break;
			case 4:
//				Toast.makeText(context, "上传评论失败,请重试", Toast.LENGTH_SHORT).show();
				AppActivity.NetInterfaceCallback(errorkey, "");
				break;
			default:
				break;
			}
		}
	};
	private void show3() {
		// TODO Auto-generated method stub
		isonpage=3;
		mediaPlayer = new MediaPlayer();
		mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
		voiceview.setVisibility(View.VISIBLE);
		mAudioRecordView.setVisibility(View.GONE);
		mCirclePercentView.setVisibility(View.GONE);
		textview.setVisibility(View.VISIBLE);
		voice.setEnabled(true);
		voice.setBackgroundResource(R.drawable.voicecomment_voice_button2);
		voice.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				voice.setEnabled(false);
				stopAllOtherVoice();
				mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
					@Override
					public void onCompletion(MediaPlayer mp) {
						Log.d("test","播放结束") ;
						voice.setBackgroundResource(R.drawable.voicecomment_voice_button2);
						mCirclePercentView.setVisibility(View.GONE);
						voice.setEnabled(true);
						mediaPlayer.reset();
						restartAllOtherVoice();

					}
				});
				try {
					if(mAudioRecordFilePath != null)
						mediaPlayer.setDataSource(mAudioRecordFilePath);
					mediaPlayer.prepare();voice.setBackgroundResource(R.drawable.voicecomment_voice_button3);
					mCirclePercentView.setVisibility(View.VISIBLE);
					Timer mTimer = new Timer();
					TimerTask mTimerTask = new TimerTask() {
						@Override
						public void run() {
							try {
								mCirclePercentView.setProgress(mediaPlayer.getCurrentPosition());
							} catch (Exception e) {
								// TODO: handle exception
							}

						}
					};
					mTimer.schedule(mTimerTask, 0, 10);
					mCirclePercentView.setMax(mediaPlayer.getDuration());
					mediaPlayer.start();
					mediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
						@Override
						public boolean onError(MediaPlayer mp, int what, int extra) {
							mediaPlayer.reset();
							return false;
						}
					});

				} catch (Exception e) {
					e.printStackTrace();
				}



			}
		});
		button1.setVisibility(View.VISIBLE);
		button2.setVisibility(View.VISIBLE);
		button1.setBackgroundResource(R.drawable.voicecomment_voice_say_it_againl_button);
		button2.setBackgroundResource(R.drawable.voicecomment_voice_publish_button);

		button1.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				mAudioRecordView.deleteRecorderPath();
				pause();
				mediaPlayer.release();
				show1();

			}
		});
		button2.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				pause();
				mediaPlayer.release();
				new Thread(networkTask).start();

			}
		});
		text.setVisibility(View.GONE);

	}

	private void toOnline() {
		// TODO Auto-generated method stub
		new Thread(new Runnable() {

			public void run() {
				String url =MacroCode.IP+"/ellabook-server/children/setVoiceComments";
				HttpClient httpclient = new DefaultHttpClient();
				HttpPost httppost = new HttpPost(url);
				Message msg = new Message();
				msg.what = 4;
				try {
					System.out.println("mAudioRecordFilePath:"+mAudioRecordFilePath);
					FileBody voiceStream = new FileBody(new File(mAudioRecordFilePath));

					StringBody bookid = new StringBody(Integer.toString(bookId));
					StringBody mem = new StringBody(membername);
					StringBody memberid = new StringBody(Integer.toString(memberId));
					StringBody orderid = new StringBody(orderId);
					StringBody gevaltime = new StringBody(gevalTime);
					StringBody commen = new StringBody(comment);
					StringBody title = new StringBody("");
					StringBody score = new StringBody(Integer.toString(starNum));
					StringBody resource = new StringBody(MacroCode.RESOURCE);
					StringBody type = new StringBody(Integer.toString(tyPe));

					MultipartEntity reqEntity = new MultipartEntity();
					reqEntity.addPart("file", voiceStream);
					reqEntity.addPart("bookId ", bookid );
					reqEntity.addPart("comment", commen);
					reqEntity.addPart("gevalTime", gevaltime);
					reqEntity.addPart("memberId ", memberid );
					reqEntity.addPart("memberName", mem);
					reqEntity.addPart("resource", resource);
					reqEntity.addPart("score ", score);
					reqEntity.addPart("title", title);
					reqEntity.addPart("orderId", orderid);
					reqEntity.addPart("type", type);
					httppost.setEntity(reqEntity);
					HttpResponse response = httpclient.execute(httppost);
					HttpEntity resEntity = response.getEntity();
					int statusCode = response.getStatusLine().getStatusCode();
					if (statusCode == HttpStatus.SC_OK) 
					{						
						System.out.println("服务器正常响应.....");
						if (resEntity != null) {
							// mAudioRecordView.deleteRecorderPath();
							String json = EntityUtils.toString(resEntity,"utf-8");
							boolean success = false;
							try {
								JSONObject js = new JSONObject(json);
								success = js.getBoolean("success");
							} catch (Exception e) {
								Log.e("201611141633", json);
							}							
							if (success) {
								msg.what = 3;
							}							
						}						
					}
				} catch (Exception e) {
					Log.e("201610211430", e.toString());
					Bundle data = new Bundle();
					data.putString("error", e.toString());
					msg.setData(data);
				}
				recordHandler.sendMessage(msg);
			}
		}).start();


	}

	private void starPlay() {
		// TODO Auto-generated method stub
		star1.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				star1.setBackgroundResource(R.drawable.voicecomment_star_press);
				star2.setBackgroundResource(R.drawable.voicecomment_star);
				star3.setBackgroundResource(R.drawable.voicecomment_star);
				star4.setBackgroundResource(R.drawable.voicecomment_star);
				star5.setBackgroundResource(R.drawable.voicecomment_star);
				starNum= 1;
			}
		});
		star2.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				star1.setBackgroundResource(R.drawable.voicecomment_star_press);
				star2.setBackgroundResource(R.drawable.voicecomment_star_press);
				star3.setBackgroundResource(R.drawable.voicecomment_star);
				star4.setBackgroundResource(R.drawable.voicecomment_star);
				star5.setBackgroundResource(R.drawable.voicecomment_star);
				starNum= 2;
			}
		});
		star3.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				star1.setBackgroundResource(R.drawable.voicecomment_star_press);
				star2.setBackgroundResource(R.drawable.voicecomment_star_press);
				star3.setBackgroundResource(R.drawable.voicecomment_star_press);
				star4.setBackgroundResource(R.drawable.voicecomment_star);
				star5.setBackgroundResource(R.drawable.voicecomment_star);
				starNum= 3;
			}
		});
		star4.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				star1.setBackgroundResource(R.drawable.voicecomment_star_press);
				star2.setBackgroundResource(R.drawable.voicecomment_star_press);
				star3.setBackgroundResource(R.drawable.voicecomment_star_press);
				star4.setBackgroundResource(R.drawable.voicecomment_star_press);
				star5.setBackgroundResource(R.drawable.voicecomment_star);
				starNum= 4;
			}
		});
		star5.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				star1.setBackgroundResource(R.drawable.voicecomment_star_press);
				star2.setBackgroundResource(R.drawable.voicecomment_star_press);
				star3.setBackgroundResource(R.drawable.voicecomment_star_press);
				star4.setBackgroundResource(R.drawable.voicecomment_star_press);
				star5.setBackgroundResource(R.drawable.voicecomment_star_press);
				starNum= 5;
			}
		});

	}
	// 获取文件手机路径
	private String getAmrPath() {
		File file = new File(Environment.getExternalStorageDirectory(),
				"ellabook/voiceRecord/" + RECORD_FILENAME + ".amr");
		return file.getAbsolutePath();
	}
	/** 
	 * 网络操作相关的子线程 
	 */  
	Runnable networkTask = new Runnable() {  

		@Override  
		public void run() {  
			// TODO  
			// 在这里进行 http request.网络请求相关操作  
			Message msg = new Message();  
			msg.what=2 ;
			recordHandler.sendMessage(msg);  
		}  
	};

	/**
	 * 录音完成回调监听
	 */
	class MyAudioRecordFinishListener implements AudioRecordFinishListener {
		@Override
		public void onFinish(float second, String filePath) {

            if(second>0){
                System.out.println(second+"=================================");
                mAudioRecordFilePath = filePath;
                mAudioRecordView.setVisibility(View.GONE);
                // 设置录音秒数
                show3();
                textview.setVisibility(View.VISIBLE);
                textview.setText(second/10 + "s");
                gevalTime = second/10+"";
                restartAllOtherVoice();
            }

		}
	}

	/**
	 * 返回
	 */
	private void back() {
		// TODO Auto-generated method stub
		pause();
		if (mediaPlayer!=null){
			mediaPlayer.release();
		}
		dialog.dismiss();
	}


	/**
	 * 评论成功
	 */
	protected void Success() {
		// TODO Auto-generated method stub
		Message msg = new Message();  
		msg.what=3 ;
		recordHandler.sendMessage(msg); 
		
	}
	/**
	 * 暂停播放
	 */
	public  void pause() {
		if (mediaPlayer != null ) {
			if(isonpage==3){
				if(!isPause){
					if (mediaPlayer.isPlaying()){
						mediaPlayer.pause();
					}
					isPause = true;
				}
				
			}

		}
	}

	private void stopAllOtherVoice(){

	}
	private void restartAllOtherVoice(){

	}
}
