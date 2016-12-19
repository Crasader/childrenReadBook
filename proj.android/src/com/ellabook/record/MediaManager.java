package com.ellabook.record;

import java.util.Timer;
import java.util.TimerTask;

import com.ellabook.R;
import com.ellabook.share.TileButton;

import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.view.View;

/**
 * MediaPlayer管理类
 */
public class MediaManager {

	private static MediaPlayer mediaPlayer;
	private static boolean isPause;

	/**
	 * 播放声音
	 * @param path 文件录音
	 * @param onCompletionListener 播放完成回调
	 */
	public static void playSound(String path, OnCompletionListener onCompletionListener, final TileButton button, final RoundProgressBar mCirclePercentView ) {
		if (mediaPlayer == null) {
			mediaPlayer = new MediaPlayer();

			mediaPlayer.setOnErrorListener(new OnErrorListener() {
				@Override
				public boolean onError(MediaPlayer mp, int what, int extra) {
					mediaPlayer.reset();
					return false;
				}
			});
		} else {
			mediaPlayer.reset();
		}

		try {
			mCirclePercentView.setVisibility(View.VISIBLE);
			mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
			mediaPlayer.setOnCompletionListener(onCompletionListener);
			mediaPlayer.setDataSource(path);
			mediaPlayer.prepare();
			button.setBackgroundResource(R.drawable.voicecomment_voice_button3);
			button.setEnabled(false);
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
			mediaPlayer.start();
			mCirclePercentView.setMax(mediaPlayer.getDuration());


			// 设置播放结束时监听
			mediaPlayer.setOnCompletionListener(new OnCompletionListener() {

				@Override
				public void onCompletion(MediaPlayer mp) {
					button.setBackgroundResource(R.drawable.voicecomment_voice_button2);
					mCirclePercentView.setVisibility(View.GONE);
					button.setEnabled(true);
				}
			});
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}

	/**
	 * 暂停播放
	 */
	public static void pause() {
		if (mediaPlayer != null && mediaPlayer.isPlaying()) {
			mediaPlayer.pause();
			isPause = true;
		}
	}

	/**
	 * 重新播放
	 */
	public static void resume(){
		if (mediaPlayer != null && isPause) {
			mediaPlayer.start();
			isPause = false;
		}
	}

	/**
	 * 释放资源
	 */
	public static void release(){
		if (mediaPlayer != null) {
			mediaPlayer.release();
			mediaPlayer = null;
		}
	}
}
