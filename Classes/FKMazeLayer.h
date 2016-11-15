#ifndef __MAZE_LAYER_H__
#define __MAZE_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKGameController.h"
#include "FKAudioPlayer.h"
#include "FKAnimationController.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"
#include "FKGameLayer.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL MazeLayer : public Layer
{
public:
    virtual bool init();
    
    
    virtual void  onEnter();
    virtual void  onExit();
    virtual void  onExitTransitionDidStart();
    virtual bool  onTouchBegan(Touch* touch, Event* event);
    virtual void  onTouchMoved(Touch* touch, Event* event);
    virtual void  onTouchEnded(Touch* touch, Event* event);
    virtual void  onTouchCancelled(Touch* touch, Event* event);
    CREATE_FUNC(MazeLayer);
    
    Action* getMazeAction(vector<AnimationData> vAnimationDataTmp,string sAnimationType,int times);
        
    //终点动画动作
    void          endPointAnimationRunAction(vector<KeyPathData> vKeyPathData,int keyPointTag,float delayTime);
    
    //普通点前进动画动作
    void          forwardNormalPointAnimationRunAction(vector<NormalPointData> vNormalPointData,int j,int normalPointTag);
    
    //普通点后退动画动作
    void          backwardNormalPointAnimationRunAction(vector<NormalPointData> vNormalPointData,unsigned long j,int normalPointTag);
    //添加提示
    void          initPrompt();
    void          promptPath(vector<KeyPathData> vKeyPathData,int iCategory,float fDelayTime,string isWin);
private:
    GameController*                    _gameController;
    EventListenerTouchOneByOne*        _oneTouch;
    
    //胜利动画参数
    Sprite                            *_roleSprite;
    vector<int>                        _vCategory;
    BookData                           _bookData;
    AnimationGroupController           _animationGroupController;
    AnimationGroupMemberController     _animationGroupMemberController;    
    
};
NS_FK_END

#endif /* __MAZE_LAYER_H__*/
