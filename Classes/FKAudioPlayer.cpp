#include "FKAudioPlayer.h"
#include "FKBookParser.h"

NS_FK_BEGIN

AudioPlayer* AudioPlayer::_audioPlayer = nullptr;

AudioPlayer* AudioPlayer::getInstance()
{
    if (_audioPlayer == nullptr) {
        _audioPlayer = new AudioPlayer();
    }
    return _audioPlayer;
}

AudioPlayer::AudioPlayer()
{
    _iMusicId = 0;
    _iSoundId = 0;
    _audioEngine = new AudioEngine();
    
}

AudioPlayer::~AudioPlayer()
{
    
}

void AudioPlayer::end()
{
    _audioEngine->end();
    delete _audioEngine;
    if (_audioPlayer == nullptr) {
        delete _audioPlayer;
        _audioPlayer = nullptr;
    }
}

void AudioPlayer::playBackgroundMusic(string filePath, bool loop)
{
    if (FileUtils::getInstance()->isFileExist(filePath) && filePath.find(".mp3") != filePath.npos)
    {
        if (_audioEngine->getState(_iMusicId) == AudioEngine::AudioState::PLAYING) {
            _audioEngine->stop(_iMusicId);
        }
        _iMusicId = _audioEngine->play2d(filePath,loop);
		if (BookParser::getInstance()->getBookPlayModeState() == AUTOPLAY)
		{
			_audioEngine->setFinishCallback(_iMusicId, _autoPlayCallBack);
		}
    }
}

void AudioPlayer::setAutoPlayCallBack(const AutoPlayCallBack& callback)
{
	_autoPlayCallBack = callback;
	_audioEngine->setFinishCallback(_iMusicId, _autoPlayCallBack);
}

void AudioPlayer::playEffect(string filePath, string interrupt)
{
    if (filePath.find(".mp3") != filePath.npos) {
        if (_iSoundId != 0) {
            _audioEngine->stop(_iSoundId);
        }
        
        if (_audioEngine->getState(_iMusicId) == AudioEngine::AudioState::PLAYING) {
            if (interrupt == "no") {
                _audioEngine->setVolume(_iMusicId, 0.1);
            }
            else
            {
                _audioEngine->pause(_iMusicId);
            }
        }
        _iSoundId = _audioEngine->play2d(filePath);
    }
}

void AudioPlayer::setBackgroundMusicVolume(float volume)
{
    _audioEngine->setVolume(_iMusicId, volume);
}

void AudioPlayer::stopAllEffect()
{
    _audioEngine->stopAll();
}

void AudioPlayer::resumeAllEffect()
{
    _audioEngine->resumeAll();
}

void AudioPlayer::pauseAllEffect()
{
    _audioEngine->pauseAll();
}

//字幕声音
void AudioPlayer::pauseOrResumeSubtitleMusic(string sPauseOrResume)
{
    if(sPauseOrResume == "pause")
    {
        _audioEngine->pause(_iMusicId);
    }
    else if(sPauseOrResume == "resume")
    {
        _audioEngine->resume(_iMusicId);
        _audioEngine->stop(_iSoundId);
    }
}

NS_FK_END
