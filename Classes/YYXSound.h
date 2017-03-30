#pragma once
#include <iostream>
#include <unordered_map>
#include <functional>
#ifndef __YYXSOUND_H__
#define __YYXSOUND_H__

using namespace std;
class YYXSound
{
public:
	static YYXSound* getInstance();
	YYXSound();
	~YYXSound();
	void init();
	int play(string path, std::function<void (int id, string path)> callback = nullptr, bool loop=false, float volume=1);//播放其他声音
	void stop(int id);
	void playButtonSound();//按钮音效
	void playIndex_DoorSound();//开门音效
	void playIndex_FlowerSound();//开花音效
	void playIndex_TrainSound();//汽笛音效
	void playIndex_BirdSound();//鸟叫音效
	void playStore_ScrollSound();//书店滚动音效
	void stopBackGroundMusic();//关闭背景音乐
	void pauseBackGroundMusic();//暂停背景音乐
	void resumeBackGroundMusic();//恢复背景音乐
	void playBackGroundMusic();//播放背景音乐
	void stopAll();//关闭所有声音


	bool getSound();
	void setSound(bool val);
	bool getMusic();
	void setMusic(bool val);
private:
	static YYXSound* instance;

	bool sound = true;
	bool music = true;
	int backGroundMusicId = -999;
	std::unordered_map<int,long long> audioEngineIds;//<soundId, time>
};

#endif