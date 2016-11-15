#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKGameController.h"
#include "FKGameSpriteController.h"
#include "FKIndexLayer.h"
#include "FKBookController.h"
#include "SimpleAudioEngine.h"
#include "FKAnimationGroupController.h"
#include "FKAnimationGroupMemberController.h"

//GameLayer
#include "FKPuzzleLayer.h"
#include "FKOrderPuzzleLayer.h"
#include "FKDrawLayer.h"
#include "FKFillingLayer.h"
#include "FKFindItLayer.h"
#include "FKBaseLineLayer.h"
#include "FKOrderLineLayer.h"
#include "FKComplexLineLayer.h"
#include "FKMazeLayer.h"
#include "FKSpotDifferenceLayer.h"


NS_FK_BEGIN

USING_NS_CC;

class FK_DLL GameLayer : public Layer
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
    CREATE_FUNC(GameLayer);
    
    //得到Z值最大的sprite的tag
    GameSpriteData getDataFromZOrder(map<int, GameSpriteData> &mTmp);

    void    winAnimation(EventCustom *eventCustom);
    
    //添加游戏layer
    void    gameTypeAddLayer();
    
    void    initSprite(vector<GameSpriteData> vTmp);
    
    //点读功能
    void    playSpriteSoundWithTag(int iTag);
    
    //自动运动动画
    void    autoAnimationRunAction(vector<AnimationGroupMemberData> mTmp);
    
    bool    isSpriteRunTouchAction(int iTag);
    
    //换图功能
    bool    replaceImageWithTouch(Sprite* sprite , vector<AnimationData> &vAnimationData ,string categroyId);
    
    //触发关联动画，根据categroyId的触发类型
    void    relateAnimationRunAction(int iTag, GameSpriteData gsData, string categroyId);
    
    map<int,GameSpriteData> touchCollision(Vec2 location);
    
    //暂停字幕动画
    void    operateSubtitle();
    void    pauseOrResumeSubtitle(string sPauseOrResume);
    
private:
    GameSpriteData                 _winGsData;
    bool                           _isRunAutoAction;
    int                            _iCurrentTag;
    GameSpriteData                 _gameSpriteData;
    BookData                       _bookData;
    int                            _iCurrentPage;
    GameController*                _gameController;
    GameSpriteController           _gameSpriteController;
    AnimationGroupController       _animationGroupController;
    AnimationGroupMemberController _animationGroupMemberController;
    EventListenerTouchOneByOne*    _oneTouch;
    int                            _iCount;
};

NS_FK_END

#endif // __GAME_LAYER_H__
