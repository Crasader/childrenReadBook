#include "YYXSound.h"
#include "YYXLayer.h"
#include "YYXTime.h"
#include "FKBookParser.h"
using namespace std;
USING_NS_FK;

YYXSound*  YYXSound::YYXSound::instance = nullptr;

YYXSound* YYXSound::getInstance()
{
	if (instance == nullptr)
	{
		instance = new YYXSound();
	}
	return instance;
}

YYXSound::YYXSound()
{	
}

YYXSound::~YYXSound()
{
}

void YYXSound::init()
{
	string path = FileUtils::getInstance()->getWritablePath() + "sound";
	if (!FileUtils::getInstance()->isDirectoryExist(path))
		FileUtils::getInstance()->createDirectory(path);
	path = FileUtils::getInstance()->getWritablePath() + "music";
	if (!FileUtils::getInstance()->isDirectoryExist(path))
		FileUtils::getInstance()->createDirectory(path);
	auto _sound = YYXLayer::getFileValue("sound", "true", FileUtils::getInstance()->getWritablePath() + "sound/" + App::GetInstance()->getMemberID());
	auto _music = YYXLayer::getFileValue("music", "true", FileUtils::getInstance()->getWritablePath() + "music/" + App::GetInstance()->getMemberID());
	if (_sound == "true")
		sound = true;
	else
		sound = false;
	if (_music == "true")
		music = true;
	else
		music = false;
}

int YYXSound::play(string path, function<void(int id, string path)> callback, bool loop, float volume)
{
	int voiceId = 0;
	voiceId = AudioEngine::play2d(path, loop, volume);
	audioEngineIds[voiceId] = YYXTime::getInstance()->getNowTime4S();
	AudioEngine::setFinishCallback(voiceId, [=](int id, string path) {
		auto it = audioEngineIds.find(id);
		if (it != audioEngineIds.end())
		{
			audioEngineIds.erase(it);
		}
		if (callback)
			callback(id, path);
	});
	return voiceId;
}

void YYXSound::stop(int id)
{
	AudioEngine::stop(id);
	auto it = audioEngineIds.find(id);
	if (it != audioEngineIds.end())
	{
		audioEngineIds.erase(it);
	}
}

void YYXSound::playButtonSound()
{
	if (sound)
	{
		play(ELLA_SOUND_BUTTON);
	}
}

void YYXSound::playIndex_DoorSound()
{
	if (sound)
	{
		play(ELLA_SOUND_OPENDOOR);
	}
}

void YYXSound::playIndex_FlowerSound()
{
	if (sound)
	{
		play(ELLA_SOUND_FLOWERS);
	}
}

void YYXSound::playIndex_TrainSound()
{
	if (sound)
	{
		play(ELLA_SOUND_TRAINWHISTLE);
	}
}

void YYXSound::playIndex_BirdSound()
{
	if (sound)
	{
		play(ELLA_SOUND_BIRDS, nullptr, false, 0.3f);
	}
}

void YYXSound::playStore_ScrollSound()
{
	if (sound)
	{
		play(ELLA_SOUND_CASTLENEXTPAGE);
	}
}

void YYXSound::stopBackGroundMusic()
{
	AudioEngine::stop(backGroundMusicId);
}

void YYXSound::pauseBackGroundMusic()
{
	auto state = AudioEngine::getState(backGroundMusicId);
	if (state == AudioEngine::AudioState::PLAYING)
		AudioEngine::pause(backGroundMusicId);
}

void YYXSound::resumeBackGroundMusic()
{
	if (!music || BookParser::getInstance()->getIsReading())
		return;
	auto state = AudioEngine::getState(backGroundMusicId);
	if (state == AudioEngine::AudioState::PAUSED)
		AudioEngine::resume(backGroundMusicId);
	else
	{
		playBackGroundMusic();
	}
}

void YYXSound::playBackGroundMusic()
{
	if (!music || BookParser::getInstance()->getIsReading())
		return;
	auto starts = AudioEngine::getState(backGroundMusicId);
	if (starts == AudioEngine::AudioState::PLAYING)
		return;
	if (starts == AudioEngine::AudioState::PAUSED)
	{
		AudioEngine::resume(backGroundMusicId);
		return;
	}
	if (App::isNight())
		backGroundMusicId = AudioEngine::play2d(ELLA_SOUND_BACKMUSIC_DAY_NIGHT, true);
	else
		backGroundMusicId = AudioEngine::play2d(ELLA_SOUND_BACKMUSIC_DAY, true);
}

void YYXSound::stopAll()
{
	for (auto it : audioEngineIds)
	{
		AudioEngine::stop(it.first);
	}
	audioEngineIds.clear();
}

bool YYXSound::getSound()
{
	return sound;
}

void YYXSound::setSound(bool val)
{
	sound = val;
	YYXLayer::setFileValue("sound", sound ? "true" : "false", FileUtils::getInstance()->getWritablePath() + "sound/" + App::GetInstance()->getMemberID());
}

bool YYXSound::getMusic()
{
	return music;
}

void YYXSound::setMusic(bool val)
{
	music = val;
	YYXLayer::setFileValue("music", music ? "true" : "false", FileUtils::getInstance()->getWritablePath() + "music/" + App::GetInstance()->getMemberID());
}


