#ifndef __FILLING_LAYER_H__
#define __FILLING_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKGameController.h"
#include "FKGameLayer.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL FillingLayer : public Layer
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
    
    CREATE_FUNC(FillingLayer);
    
private:
    EventListenerTouchOneByOne*    _oneTouch;
    int                            _iTag;
    GameController*                _gameController;
    GameSpriteData                 _gsData;
    
};

NS_FK_END


#endif /* __FILLING_LAYER_H__ */
