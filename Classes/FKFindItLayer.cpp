#include "FKFindItLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool FindItLayer::init()
{
    if ( !Layer::init() )
    {
        
        return false;
    }
    return true;
}

void FindItLayer::onExit()
{
    Layer::onExit();
}

void FindItLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void FindItLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(FindItLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(FindItLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(FindItLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(FindItLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
    //设置胜利动画相关数据
    setWinData();
}


#pragma mark - touch事件

bool FindItLayer::onTouchBegan(Touch* touch, Event* event)
{
    log("FindItLayer onTouchBegan");
    map<Sprite* ,GameSpriteData> mTmp = _gameController->getGameSpriteAndDataMap();
    GameSpriteData gsData;
    Node *parent = this->getParent();
    int tag = 0;
    //获取屏幕尺寸
    Point visibleSize = Director::getInstance()->getWinSize();
    Point touchLocation = parent->convertTouchToNodeSpace(touch);
    //如果点击的是左半屏获取sp的Data
    for ( auto sp : parent->getChildren())
    {
        if (sp->getBoundingBox().containsPoint(touch->getLocation())||sp->getBoundingBox().containsPoint(Point(touchLocation.x + 0.5 * visibleSize.x, touchLocation.y))||sp->getBoundingBox().containsPoint(Point(touchLocation.x - 0.5 * visibleSize.x, touchLocation.y)))
        {
            for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
            {
                if(sp == it->first)
                {
                    gsData = it->second;
                    tag = gsData.getTag();
                }
            }
        }
    }
    string type = gsData.getType();
    //依据Type值判断是否是不同点
    log("touchTag = %d",tag);
    if(type == "game")
    {
        _vTag.push_back(tag);
        string repeatTouch = "No";
        unsigned long iTagSize = _vTag.size();
        //重复点击判断
        for (unsigned int i = 0; i < iTagSize-1; i++)
        {
            if (_vTag[i] == _vTag[iTagSize-1])
            {
                repeatTouch = "Yes";
                _vTag.pop_back();
            }
        }
        if(repeatTouch == "No")
        {
            Point position = gsData.getPosition();
            auto circle1 = Sprite::create("circle.png");
            circle1->setAnchorPoint(Vec2(0.5, 0.5));
            circle1->setPosition(position);
            auto circle2 = Sprite::create("circle.png");
            circle2->setAnchorPoint(Vec2(0.5, 0.5));
            if(position.x<0.5 * visibleSize.x)
            {
                circle2->setPosition(0.5 * visibleSize.x + position.x, position.y);
            }
            else
            {
                circle2->setPosition(position.x-0.5 * visibleSize.x, position.y);
            }
            parent->addChild(circle1, 100);
            parent->addChild(circle2, 100);
        }
    }
    //胜利动画
    if (_vTag.size() == _iMaxNumber) {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("WinAnimation");
        _iMaxNumber++;
    }
    return true;
}
void FindItLayer::onTouchMoved(Touch* touch, Event* event)
{
}
void FindItLayer::onTouchEnded(Touch* touch, Event* event)
{
}
void FindItLayer::onTouchCancelled(Touch* touch, Event* event)
{
}

//胜利动画数据
void FindItLayer::setWinData()
{
    _iMaxNumber = 0;
    map<Sprite* ,GameSpriteData> mTmp = _gameController->getGameSpriteAndDataMap();
    for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
    {
        //获取不同切片的最大数目
        GameSpriteData gsData;
        gsData = it->second;
        string type = gsData.getType();
        if(type == "game")
        {
            _iMaxNumber = _iMaxNumber + 1;
        }
    }
}

NS_FK_END

