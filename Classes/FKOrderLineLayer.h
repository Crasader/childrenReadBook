#ifndef __ORDER_LINE_LAYER_H__
#define __ORDER_LINE_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKAudioPlayer.h"
#include "FKGameController.h"
#include "FKAnimationController.h"
#include "FKGameSpriteController.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"
#include "FKGameLayer.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL OrderLineLayer : public Layer
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
    CREATE_FUNC(OrderLineLayer);
    
    //draw方法
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags);
    //初始化第一个连线精灵
    void     initFirstOrderSprite();
    //提示动画
    void     promptAnimation(int OrderId);
private:
    int                            _iOrderId;
    BookData                       _bookData;
    DrawNode*                      _drawNode;
    vector<Point>                  _vPosition;
    vector<int>                    _vLineTag;
    GameController*                _gameController;
    GameSpriteController           _gameSpriteController;
    EventListenerTouchOneByOne*    _oneTouch;
    //胜利动画
    bool                           _isWin;
    int                            _iOrder;
    int                            _iMaxOrder;
    AnimationGroupController       _animationGroupController;
    AnimationGroupMemberController _animationGroupMemberController;
    
};


NS_FK_END

#endif  /* __ORDER_LINE_LAYER_H__ */
