#ifndef __BASELINE_LAYER_H__
#define __BASELINE_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKAudioPlayer.h"
#include "FKGameController.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL BaseLineLayer : public Layer
{
public:
    virtual bool init();
    
    virtual void onEnter();
    virtual void onExit();
    virtual void onExitTransitionDidStart();
    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchMoved(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
    virtual void onTouchCancelled(Touch* touch, Event* event);
    CREATE_FUNC(BaseLineLayer);
    
    //draw方法
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags);
    
    //胜利动画
    void   setWinData();
    
private:
    EventListenerTouchOneByOne*    _oneTouch;
    GameController*                _gameController;
    Point                          _positionMain;
    Point                          _positionMinor;
    int                            _iTagMain;
    int                            _iTagMinor;
    DrawNode*                      _drawNode;
    vector<Point>                  _vMainLocation;
    vector<Point>                  _vMinorLocation;
    vector<string>                 _vMainOrMinor;
    map<Sprite* ,GameSpriteData>   _mTmpCopy;
    //胜利动画参数
    int                            _iMaxNumber;
    AnimationGroupController       _animationGroupController;
    AnimationGroupMemberController _animationGroupMemberController;
};

NS_FK_END


#endif  /* BaseLineLayer_h */
