#ifndef __ORDER_PUZZLE_LAYER_H__
#define __ORDER_PUZZLE_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKGameController.h"
#include "FKAudioPlayer.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"
#include "FKGameLayer.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL OrderPuzzleLayer : public Layer
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
    
    CREATE_FUNC(OrderPuzzleLayer);
    //胜利动画
    void   setWinData();
    
private:
    EventListenerTouchOneByOne*    _oneTouch;
    int                            _iTag;
    GameController*                _gameController;
    Vec2                           _vFinishPosition;
    Vec2                           _vOriginalPosition;
    float                          _fEndScale;
    float                          _fStartScale;
    int                            _iOrderId;
    //点击的sp放置于z值最高
    int                            _iZorder;
    //判断序列号
    int                            _iOrder;
    int                            _iMaxNumber;
    AnimationGroupController       _animationGroupController;
    AnimationGroupMemberController _animationGroupMemberController;
    BookData                       _bookData;
    
};


NS_FK_END
#endif /*__ORDER_PUZZLE_LAYER_H__*/

