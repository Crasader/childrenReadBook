#include "FKSubtitleController.h"

NS_FK_BEGIN

SubtitleController::SubtitleController()
{
    _mSubtitleDataAndSprite.clear();
}

SubtitleController::~SubtitleController()
{
    
}

void SubtitleController::addSubtitle(cocos2d::Layer *layer, FK::PageData pageData)
{
    if (pageData.getSubtitleSet().size() > 0) {
        for (int i = 0; i < pageData.getSubtitleSet().size(); i++) {
            SubtitleData subtitleData = pageData.getSubtitleSet()[i];
            
            string sFilePath = BookParser::getInstance()->getBookData().getBookPath() + "/image/" + subtitleData.getImageId();
            Sprite *subtitle = Sprite::create(sFilePath);
            subtitle->setPosition(subtitleData.getPosition());
            subtitle->setName("subtitle");
            subtitle->setOpacity(0);
            layer->addChild(subtitle, subtitleData.getZOrder());
            
            if (subtitleData.getAnimationEffect() == "fade") {
                Action *action = Sequence::create(DelayTime::create(subtitleData.getStartTime()),FadeTo::create(0.3, 255),DelayTime::create(subtitleData.getDuration()),FadeTo::create(0.3, 0), NULL);
                action->setTag(1);
                subtitle->runAction(action);
            }
            
            _mSubtitleDataAndSprite[subtitle] = subtitleData;
        }
    }
}

void SubtitleController::pauseSubtitles()
{
    AudioPlayer::getInstance()->pauseOrResumeSubtitleMusic("pause");
    for (map<Sprite*, SubtitleData>::iterator it = _mSubtitleDataAndSprite.begin();it != _mSubtitleDataAndSprite.end(); it++)
    {
        it->first->getActionManager()->pauseTarget(it->first);
    }
}

void SubtitleController::resumeSubtitles()
{
    AudioPlayer::getInstance()->pauseOrResumeSubtitleMusic("resume");
    for (map<Sprite*, SubtitleData>::iterator it = _mSubtitleDataAndSprite.begin();it != _mSubtitleDataAndSprite.end(); it++)
    {
        it->first->getActionManager()->resumeTarget(it->first);
    }
}

void SubtitleController::replaySubtitles(string backgroundPath)
{
    AudioPlayer::getInstance()->playBackgroundMusic(backgroundPath, false);
    for (map<Sprite*, SubtitleData>::iterator it = _mSubtitleDataAndSprite.begin();it != _mSubtitleDataAndSprite.end(); it++) {
        Sprite *subtitle = it->first;
        SubtitleData subtitleData = it->second;
        if (subtitleData.getAnimationEffect() == "fade") {
            Action *action = Sequence::create(DelayTime::create(subtitleData.getStartTime()),FadeTo::create(0.3, 255),DelayTime::create(subtitleData.getDuration()),FadeTo::create(0.3, 0), NULL);
            action->setTag(1);
            subtitle->runAction(action);
        }
    }
}

NS_FK_END
