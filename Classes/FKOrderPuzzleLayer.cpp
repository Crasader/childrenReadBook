#include "FKOrderPuzzleLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool OrderPuzzleLayer::init()
{
    if ( !Layer::init() )
    {
        
        return false;
    }
    return true;
    
}

void OrderPuzzleLayer::onExit()
{
    Layer::onExit();
}

void OrderPuzzleLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void OrderPuzzleLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(OrderPuzzleLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(OrderPuzzleLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(OrderPuzzleLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(OrderPuzzleLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
    
    //初始化判断序列号，赋值为1；设置胜利动画相关数据
    _iOrder = 1;
    setWinData();
    
}


#pragma mark - touch事件

bool OrderPuzzleLayer::onTouchBegan(Touch* touch, Event* event)
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
    log("OrderPuzzleLayer touchbegan _iTag = %d",_iTag);
    //获取拼图精灵的起始点，完成点以及缩放
    _vFinishPosition = gsData.getFinishPosition();
    _iOrderId = gsData.getOrderId();
    log("iOrder = %d",_iOrder);
    log("iOrdeId = %d",_iOrderId);
    if(_vFinishPosition.x != 0 && _iOrderId == _iOrder)
    {
        if(_iTag != Node::INVALID_TAG)
        {
            _fEndScale = gsData.getEndScale();
            _fStartScale = gsData.getStartScale();
            _vOriginalPosition = gsData.getPosition();
            Node *parent = this->getParent();
            auto sp = parent->getChildByTag(_iTag);
            sp->setScale(_fEndScale);
            _iZorder = sp->getLocalZOrder();
            sp->setLocalZOrder(1200);
        }
    }
    return true;
    
}
void OrderPuzzleLayer::onTouchMoved(Touch* touch, Event* event)
{
    log("OrderPuzzleLayer onTouchMoved_iTag = %d",_iTag);
    //检测精灵能否被拖动
    if(_vFinishPosition.x != 0&& _iOrderId == _iOrder)
    {
        //位置拖动
        if(_iTag != Node::INVALID_TAG)
        {
            Node *parent = this->getParent();
            Point touchLocation = parent->convertTouchToNodeSpace(touch);
            auto sp = parent->getChildByTag(_iTag);
            sp->setPosition(touchLocation);
        }
    }
    
}
void OrderPuzzleLayer::onTouchEnded(Touch* touch, Event* event)
{
    log("OrderPuzzleLayer onTouchEnd");
    //位置检测
    if(_vFinishPosition.x != 0&& _iOrderId == _iOrder)
    {
        if(_iTag != Node::INVALID_TAG)
        {
            Node *parent = this->getParent();
            auto sp = parent->getChildByTag(_iTag);
            sp->setScale(_fEndScale);
            Point spLocation = sp->getPosition();
            //如果移动到的位置不在正确位置附近则将其放置在原始位置上
            if (!_vFinishPosition.equals(spLocation))
            {
                sp->setPosition(_vOriginalPosition);
                sp->setScale(_fStartScale);
                sp->setLocalZOrder(_iZorder);
            }
            //如果移动到的位置在正确位置附近则将其放置在正确位置上
            Rect finishRect = Rect(_vFinishPosition.x-50, _vFinishPosition.y-50, 100, 100);
            if (finishRect.containsPoint(spLocation))
            {
                //顺序序列加1，查找下一序列拼图
                _iOrder = _iOrder+1;
                sp->setPosition(_vFinishPosition);
                sp->setScale(_fEndScale);
                sp->setLocalZOrder(_iZorder);
            }
        }
    }
    //胜利动画
    if(_iOrder -1 == _iMaxNumber)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("WinAnimation");
        _iMaxNumber++;
    }
    
}
void OrderPuzzleLayer::onTouchCancelled(Touch* touch, Event* event)
{
}
void OrderPuzzleLayer::setWinData()
{
    _iMaxNumber = 0;
    map<Sprite* ,GameSpriteData> mTmp = _gameController->getGameSpriteAndDataMap();
    for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
    {
        //获取可拼切片的最大数目
        GameSpriteData gsData;
        gsData = it->second;
        
        if(gsData.getOrderId()>_iMaxNumber)
        {
            _iMaxNumber = gsData.getOrderId();
        }
    }
}

NS_FK_END


