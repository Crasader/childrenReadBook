#ifndef __PUZZLE_LAYER_H__
#define __PUZZLE_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKAudioPlayer.h"
#include "FKGameController.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"
#include "FKGameLayer.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL PuzzleLayer : public Layer
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
    
    CREATE_FUNC(PuzzleLayer);
    //胜利动画
    void   setWinData();
    //得到Z值最大的sprite的tag
    int     getTagFromZOrder(map<int, int> &mTmp);
    
private:
    EventListenerTouchOneByOne*    _oneTouch;
    int                            _iTag;
    GameController*                _gameController;
    Vec2                           _vFinishPosition;
    Vec2                           _vOriginalPosition;
    float                          _fEndScale;
    float                          _fStartScale;
    //点击的sp放置于z值最高
    int                            _iZorder;
    //放置到正确位置后不允许拖动（判断）
    Vec2                           _vTouchBeginPosition;
    //胜利动画参数
    int                            _iNumber;
    int                            _iMaxNumber;
    AnimationGroupController       _animationGroupController;
    AnimationGroupMemberController _animationGroupMemberController;
    BookData                       _bookData;
};

NS_FK_END


#endif /* __PUZZLE_LAYER_H__ */
