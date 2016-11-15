#ifndef __FIND_IT_LAYER_H__
#define __FIND_IT_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKAudioPlayer.h"
#include "FKGameController.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL FindItLayer : public Layer
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
    
    CREATE_FUNC(FindItLayer);
    //胜利动画
    void   setWinData();
    
private:
    EventListenerTouchOneByOne*    _oneTouch;
    GameController*                _gameController;
    //胜利动画参数
    vector<int>                    _vTag;
    int                            _iMaxNumber;
    AnimationGroupController       _animationGroupController;
    AnimationGroupMemberController _animationGroupMemberController;
    BookData                       _bookData;
    
};

NS_FK_END


#endif /* FINDITLayer_h */
