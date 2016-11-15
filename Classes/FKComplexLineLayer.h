#ifndef __COMPLEX_LINE_LAYER_H__
#define __COMPLEX_LINE_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKAudioPlayer.h"
#include "FKGameController.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"
#include "FKGameLayer.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL ComplexLineLayer : public Layer
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
    CREATE_FUNC(ComplexLineLayer);
    
    //draw方法
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags);
    
    //胜利动画
    void   setWinData();
    
private:
    DrawNode*                      _drawNode;
    GameController*                _gameController;
    EventListenerTouchOneByOne*    _oneTouch;
    RenderTexture*                 _renderTexture;
    //无序连线
    vector<Point>                  _vDisorderTagQueue;
    vector<int>                    _vDisorderCurrentTagQueue;
    vector<Point>                  _vStartPosition;
    vector<Point>                  _vEndPosition;
    vector<ComplexLineTagData>     _vDisorderComplexLineTagData;
    
    //顺序连线
    bool                           _isLineFinish;
    int                            _iLineFinishNumber;
    int                            _iTag;
    vector<int>                    _vOrderTagQueue;
    vector<Point>                  _vOrderPosition;
    vector<ComplexLineTagData>     _vOrderComplexLineTagData;
    int                            _iCurrentLine;
    vector<int>                    _vLineQueue;
    //胜利动画参数
    unsigned long                  _iMaxLine;
    BookData                       _bookData;
    AnimationGroupController       _animationGroupController;
    AnimationGroupMemberController _animationGroupMemberController;
};


NS_FK_END

#endif  /* __COMPLEX_LINE_LAYER_H__ */
