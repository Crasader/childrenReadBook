#ifndef FKSubtitleController_h
#define FKSubtitleController_h

#include "cocos2d.h"
#include "FKUtil.h"
#include "FKPageData.h"
#include "FKAudioPlayer.h"
#include "FKBookParser.h"

NS_FK_BEGIN

using namespace std;
USING_NS_CC;

class SubtitleController {
public:
    SubtitleController();
    ~SubtitleController();
    
    void addSubtitle(Layer *layer, PageData pageData);
    
    void replaySubtitles(string backgroundPath);
    
    void pauseSubtitles();
    
    void resumeSubtitles();
    
private:
    map<Sprite*, SubtitleData>      _mSubtitleDataAndSprite;
};

NS_FK_END
#endif /* FKSubtitleController_h */
