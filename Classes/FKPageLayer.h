#ifndef __PAGE_LAYER_H__
#define __PAGE_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKStandardLayer.h"
#include "FKGameLayer.h"
#include "FKPreLoadResourcesController.h"
#include "FKUtil.h"
//视屏
#include "ui/UIVideoPlayer.h"
#include "ui/CocosGUI.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;


class FK_DLL PageLayer : public Layer
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
    CREATE_FUNC(PageLayer);
    
    //视屏相关
    void videoPlayOverCallback();
    void showVideo();
    
    void initResource();
    void pageUp(Ref* pSender);
    void pageDown(Ref* pSender);
    void pageQuit(Ref* pSender);
    void pageShare(Ref* pSender);
    
    void setMenuOpacity(float opacity);
    
    //粒子系统
    void showParticleEffect(string sAutoOrTouch);
    
    //Andriod和ios平台有效
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    void videoEventCallback(Ref* sender, experimental::ui::VideoPlayer::EventType eventType);
#endif
        
private:
    bool                            _isButtonEnable;
    int                             _iCurrentPage;
    PageData                        _pageData;
    BookData                        _bookData;
    BookParser*                     _bookParser;
    EventListenerTouchOneByOne*     _oneTouch;
    
    Point                           _touchLocation;
};

NS_FK_END
#endif // __PAGE_LAYER_H__
