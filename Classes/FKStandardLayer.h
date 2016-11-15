#ifndef __STANDARD_LAYER_H__
#define __STANDARD_LAYER_H__

#include "cocos2d.h"
#include "FKStandardController.h"
#include "FKSpriteController.h"
#include "FKIndexLayer.h"
#include "FKBookController.h"
#include "SimpleAudioEngine.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"
#include "FKAudioPlayer.h"
#include "FKPixelCollision.h"
//视屏
#include "ui/UIVideoPlayer.h"
#include "ui/CocosGUI.h"

NS_FK_BEGIN

USING_NS_CC;
class FK_DLL StandardLayer : public Layer
{
public:
    static Scene* createScene();
    
    virtual bool init();
    
    virtual void onEnter();
    virtual void onExit();
    virtual void onExitTransitionDidStart();
    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchMoved(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
    virtual void onTouchCancelled(Touch* touch, Event* event);
    
    // implement the "static create()" method manually
    CREATE_FUNC(StandardLayer);
    
    void initSprite(map<int, SpriteData> mTmp);
    
    //得到Z值最大的sprite的tag
    int getTagFromZOrder(map<int, int> &mTmp);
    
    //点读功能
    void playSpriteSoundWithTag(int iTag);
    
    //自动运动动画
    void autoAnimationRunAction(vector<AnimationGroupMemberData> mTmp);
    
    //触发关联动画，根据categroyId的触发类型
    void relateAnimationRunAction(int iTag, string categroyId);
    
    bool isSpriteRunTouchAction(int iTag);
    
    //换图功能
    bool   replaceImageWithTouch(Sprite* sprite , vector<AnimationData> &vAnimationData ,string categroyId);
    
    map<int,int> touchCollision(Vec2 location);
    
    //暂停字幕动画
    void     operateSubtitle();
    void     pauseOrResumeSubtitle(string sPauseOrResume);
    
private:
    bool                            _isRunAutoAction;
    int                             _iCurrentTag;
    SpriteData                      _spriteData;
    BookData                        _bookData;
    
    int                             _iCurrentPage;
    StandardController              _standardController;
    SpriteController                _spriteController;
    AnimationGroupController        _animationGroupController;
    AnimationGroupMemberController  _animationGroupMemberController;
    EventListenerTouchOneByOne*     _oneTouch;
    int                             _iCount;
};


NS_FK_END

#endif // __STANDARD_LAYER_H__
