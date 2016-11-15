#ifndef __SPOT_DIFFERENCE_LAYER_H__
#define __SPOT_DIFFERENCE_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKAudioPlayer.h"
#include "FKGameController.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"
#include "FKGameLayer.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL SpotDifferenceLayer : public Layer
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
    
    CREATE_FUNC(SpotDifferenceLayer);
    //draw方法
    //    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags);
    //画圈
    void                          drawCircle(Size spContentSize);
    
    //胜利动画参数
    void                           setWinData();
    
private:
    EventListenerTouchOneByOne*    _oneTouch;
    GameController*                _gameController;
    vector<int>                    _vTagQueue;
    vector<Point>                  _vPosition;
    //圆半径
    //vector<float>                  _vRadius;
    int                            _iMaxNumber;
    AnimationGroupController       _animationGroupController;
    AnimationGroupMemberController _animationGroupMemberController;
    BookData                       _bookData;
    
};

NS_FK_END


#endif /* FINDITLayer_h */
