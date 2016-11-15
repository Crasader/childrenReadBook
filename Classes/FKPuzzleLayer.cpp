#include "FKPuzzleLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool PuzzleLayer::init()
{
    if ( !Layer::init() )
    {
        
        return false;
    }
    return true;
}

void PuzzleLayer::onExit()
{
    Layer::onExit();
    this->removeAllChildrenWithCleanup(true);
}

void PuzzleLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void PuzzleLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(PuzzleLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(PuzzleLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(PuzzleLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(PuzzleLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
    
    //初始化胜利动画判断条件,设置胜利动画相关数据
    _iNumber = 0;
    setWinData();
    
}


#pragma mark - touch事件

bool PuzzleLayer::onTouchBegan(Touch* touch, Event* event)
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
    //
    log("PuzzleLayer touchbegan _iTag = %d",_iTag);
    //获取拼图精灵的起始点，完成点以及缩放
    _vFinishPosition = gsData.getFinishPosition();
    if(_iTag != Node::INVALID_TAG)
    {
        if(_vFinishPosition.x != 0)
        {
            auto sp = parent->getChildByTag(_iTag);
            _vTouchBeginPosition = sp->getPosition();
            if(!_vTouchBeginPosition.equals(_vFinishPosition))
            {
                _fEndScale = gsData.getEndScale();
                _fStartScale = gsData.getStartScale();
                _vOriginalPosition = gsData.getPosition();
                sp->setScale(_fEndScale);
                _iZorder = sp->getLocalZOrder();
                sp->setLocalZOrder(9999);
            }
        }
    }
    return true;
}
void PuzzleLayer::onTouchMoved(Touch* touch, Event* event)
{
    //检测精灵能否被拖动
    if(_vFinishPosition.x != 0&&!_vFinishPosition.equals(_vTouchBeginPosition))
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
void PuzzleLayer::onTouchEnded(Touch* touch, Event* event)
{
    //位置检测
    if(_vFinishPosition.x != 0&&!_vFinishPosition.equals(_vTouchBeginPosition))
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
                sp->setPosition(_vFinishPosition);
                sp->setScale(_fEndScale);
                sp->setLocalZOrder(_iZorder);
                //判断记数加1
                _iNumber = _iNumber+1;
            }
        }
    }
    //胜利动画
    if(_iNumber == _iMaxNumber)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("WinAnimation");
        _iMaxNumber++;
    }
    
}
void PuzzleLayer::onTouchCancelled(Touch* touch, Event* event)
{
}

void PuzzleLayer::setWinData()
{
    _iMaxNumber = 0;
    map<Sprite* ,GameSpriteData> mTmp = _gameController->getGameSpriteAndDataMap();
    for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
    {
        //获取可拖动切片的最大数目
        GameSpriteData gsData;
        gsData = it->second;
        Point finishPosition = gsData.getFinishPosition();
        if(finishPosition.x != 0)
        {
            _iMaxNumber = _iMaxNumber + 1;
        }
    }
}

NS_FK_END

