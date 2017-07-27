#include "FKFillingLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool FillingLayer::init()
{
    if ( !Layer::init() )
    {
        
        return false;
    }
    return true;
}

void FillingLayer::onExit()
{
    Layer::onExit();
}

void FillingLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void FillingLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(FillingLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(FillingLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(FillingLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(FillingLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
}


#pragma mark - touch事件

bool FillingLayer::onTouchBegan(Touch* touch, Event* event)
{
    auto parent = (GameLayer*)this->getParent();
    map<int,GameSpriteData> mTouchCollision = parent->touchCollision(touch->getLocation());
    
    if (mTouchCollision.size()>0) {
        _gsData = parent->getDataFromZOrder(mTouchCollision);
        _iTag = _gsData.getTag();
    }else
    {
        _iTag = Node::INVALID_TAG;
    }
    
    log("FillingLayer onTouchbegan _iTag = %d",_iTag);
    
    return true;
}
void FillingLayer::onTouchMoved(Touch* touch, Event* event)
{
    //检测点击的精灵是否是颜料
    if(_gsData.getColor().operator!=(Vec3(-1.0f, -1.0f, -1.0f)))
    {
        //位置拖动
        if(_iTag != Node::INVALID_TAG)
        {
            Node *parent = this->getParent();
            auto sp = parent->getChildByTag(_iTag);
            sp->setPosition(touch->getLocation());
        }
    }
    
}
void FillingLayer::onTouchEnded(Touch* touch, Event* event)
{
    log("FillingLayer onTouchEnd");
    //颜料回归原位
    auto parent = (GameLayer*)this->getParent();
    if(_gsData.getColor().operator!=(Vec3(-1.0f, -1.0f, -1.0f)))
    {
        if(_iTag != Node::INVALID_TAG)
        {
            auto sp = parent->getChildByTag(_iTag);
            sp->setPosition(_gsData.getPosition());
        }
    }
    //获取点击终点位置所对应精灵的Data值
    GameSpriteData gsData;
    
    map<int,GameSpriteData> mTouchCollision = parent->touchCollision(touch->getLocation());
    int isFillingTag;
    if (mTouchCollision.size()>0) {
        gsData = parent->getDataFromZOrder(mTouchCollision);
        isFillingTag = gsData.getTag();
    }else
    {
        isFillingTag = Node::INVALID_TAG;
    }
    log("FillingLayer isFillingTag = %d", isFillingTag);
    //根据Tag值判断是否是可被填充对象
    string isFilling = gsData.getIsFilling();
    if (isFilling == "yes" && isFillingTag != Node::INVALID_TAG)
    {
        //进行填充
        auto spFilling = parent->getChildByTag(isFillingTag);
        spFilling->setColor(Color3B(_gsData.getColor().x,_gsData.getColor().y,_gsData.getColor().z));
    }

}
void FillingLayer::onTouchCancelled(Touch* touch, Event* event)
{
    
}

NS_FK_END

