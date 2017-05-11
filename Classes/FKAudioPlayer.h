#ifndef __AUDIO_PLAYER_H_
#define __AUDIO_PLAYER_H_
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"
#include "cocos2d.h"
#include <string>
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace CocosDenshion;
using namespace std;
using namespace experimental;

class FK_DLL AudioPlayer
{
public:
    static AudioPlayer* getInstance();

	typedef std::function<void(int, const std::string&)> AutoPlayCallBack;
    
    void end();
    
    AudioPlayer();
    ~AudioPlayer();
    
    void playBackgroundMusic(string filePath, bool loop);
    void playEffect(string filePath, string interrupt);
    void setBackgroundMusicVolume(float volume);
    void stopAllEffect();
    void resumeAllEffect();
    void pauseAllEffect();

	void uncacheAll();
    
	AudioEngine::AudioState getBackgroundPlayAudioState();
    
	void setAutoPlayCallBack(const AutoPlayCallBack& callback);
    
    //字幕声音继续播放
    void pauseOrResumeSubtitleMusic(string sPauseOrResume);
    
private:
    static AudioPlayer*                 _audioPlayer;
    unsigned int                        _iSoundId;
    unsigned int                        _iMusicId;
	AutoPlayCallBack					_autoPlayCallBack;
};


NS_FK_END

#endif /* __AUDIO_PLAYER_H_ */
