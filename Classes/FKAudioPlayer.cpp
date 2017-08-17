#include "FKAudioPlayer.h"
#include "FKBookParser.h"

NS_FK_BEGIN

AudioPlayer* AudioPlayer::_audioPlayer = nullptr;

#ifdef ERROR
#undef ERROR
#endif // ERROR

AudioPlayer* AudioPlayer::getInstance()
{
    if (_audioPlayer == nullptr) {
		_audioPlayer = new AudioPlayer();
		AudioEngine::lazyInit();
    }
    return _audioPlayer;
}

AudioPlayer::AudioPlayer()
{
    _iMusicId = 0;
    _iSoundId = 0;
}

AudioPlayer::~AudioPlayer()
{
    
}

void AudioPlayer::end()
{
    AudioEngine::end();
    
    if (_audioPlayer != nullptr) {
        delete _audioPlayer;
        _audioPlayer = nullptr;
    }
}

void AudioPlayer::playBackgroundMusic(string filePath, bool loop)
{
	AudioEngine::AudioState audioState = AudioEngine::getState(_iMusicId);
    if (audioState == AudioEngine::AudioState::PLAYING) {
		AudioEngine::stop(_iMusicId);
    }
	_iMusicId = AudioEngine::play2d(filePath, loop);
    if (BookParser::getInstance()->getBookPlayModeState() == AUTOPLAY)
    {
		AudioEngine::setFinishCallback(_iMusicId, _autoPlayCallBack);
    }
    if (BookParser::getInstance()->getPaused()) {
        BookParser::getInstance()->pausePlay();
    }
}

void AudioPlayer::setAutoPlayCallBack(const AutoPlayCallBack& callback)
{
	_autoPlayCallBack = callback;
	AudioEngine::setFinishCallback(_iMusicId, _autoPlayCallBack);
}

void AudioPlayer::playEffect(string filePath, string interrupt)
{
    if (filePath.find(".mp3") != filePath.npos) {
        if (_iSoundId != 0) {
			AudioEngine::stop(_iSoundId);
        }
        
		if (AudioEngine::getState(_iMusicId) == AudioEngine::AudioState::PLAYING) {
            if (interrupt == "no") {
				AudioEngine::setVolume(_iMusicId, 0.1);
            }
            else
            {
				AudioEngine::stop(_iMusicId);
            }
        }
		_iSoundId = AudioEngine::play2d(filePath);
    }
}

void AudioPlayer::setBackgroundMusicVolume(float volume)
{
	AudioEngine::setVolume(_iMusicId, volume);
}

void AudioPlayer::stopAllEffect()
{
	AudioEngine::stopAll();
}

void AudioPlayer::uncacheAll()
{
	AudioEngine::uncacheAll();
}

void AudioPlayer::resumeAllEffect()
{
	AudioEngine::resumeAll();
}

void AudioPlayer::pauseAllEffect()
{
	AudioEngine::pauseAll();
}

AudioEngine::AudioState AudioPlayer::getBackgroundPlayAudioState()
{
	return AudioEngine::getState(_iMusicId);
}

//字幕声音
void AudioPlayer::pauseOrResumeSubtitleMusic(string sPauseOrResume)
{
    if(sPauseOrResume == "pause")
    {
		AudioEngine::pause(_iMusicId);
    }
    else if(sPauseOrResume == "resume")
    {
		AudioEngine::resume(_iMusicId);
        AudioEngine::stop(_iSoundId);
    }
}

NS_FK_END
