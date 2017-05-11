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
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    GameSpriteData gsData;
    auto parent = (GameLayer*)this->getParent();
    map<int,GameSpriteData> mTouchCollision = parent->touchCollision(touch->getLocation());
    
    if (mTouchCollision.size()>0) {
        gsData = parent->getDataFromZOrder(mTouchCollision);
        _iTag = gsData.getTag();
    }else
    {
        _iTag = Node::INVALID_TAG;
    }
    
    log("FillingLayer onTouchbegan _iTag = %d",_iTag);
    //获取填充游戏的颜料的RGB和初始位置
    _vColor = gsData.getColor();
    _vOriginalPosition = gsData.getPosition();
    return true;
}
void FillingLayer::onTouchMoved(Touch* touch, Event* event)
{
    //检测点击的精灵是否是颜料
    if(_vColor.x != 0 || _vColor.y != 0 || _vColor.z != 0 )
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
    if(_vColor.x != 0 || _vColor.y != 0 || _vColor.z != 0 )
    {
        auto sp = parent->getChildByTag(_iTag);
        sp->setPosition(_vOriginalPosition);
    }
    //获取点击终点位置所对应精灵的Data值
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    GameSpriteData gsData;
    //填色
    if(_vColor.x != 0 || _vColor.y != 0 || _vColor.z != 0 )
    {
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
        if (isFilling == "yes")
        {
            //进行填充
            auto spFilling = parent->getChildByTag(isFillingTag);
            spFilling->setColor(Color3B(_vColor.x,_vColor.y,_vColor.z));
        }
    }
}
void FillingLayer::onTouchCancelled(Touch* touch, Event* event)
{
    
}

NS_FK_END

