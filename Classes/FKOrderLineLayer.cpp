#include "FKOrderLineLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool OrderLineLayer::init()
{
    if ( !Layer::init() )
    {
        
        return false;
    }
    //生成draw
    _drawNode = DrawNode::create();
    _drawNode->retain();
    return true;
}

void OrderLineLayer::onExit()
{
    Layer::onExit();
}

void OrderLineLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void OrderLineLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(OrderLineLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(OrderLineLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(OrderLineLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(OrderLineLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
    
    //初始化判断序列号，赋值为1，最大序列号为0
    _iOrder = 2;
    _iMaxOrder = 0;
    _isWin = false;
    initFirstOrderSprite();
    
}


#pragma mark - touch事件

bool OrderLineLayer::onTouchBegan(Touch* touch, Event* event)
{
    log("OrderLineLayer onTouchEnd");
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    GameSpriteData gsData;
    auto parent = (GameLayer*)this->getParent();
    map<int,GameSpriteData> mTouchCollision = parent->touchCollision(touch->getLocation());
    int Tag;
    if (mTouchCollision.size()>0) {
        gsData = parent->getDataFromZOrder(mTouchCollision);
        Tag = gsData.getTag();
    }else
    {
        Tag = Node::INVALID_TAG;
    }
    log("maxorder = %d", _iMaxOrder);
    _iOrderId = gsData.getOrderId();
    auto sp = parent->getChildByTag(Tag);
    //判断点击精灵的orderId是否与序列号相等
    if (_iOrderId == _iOrder)
    {
        //点击精灵恢复原状态
        sp->setScale(gsData.getScale());
        sp->setOpacity(gsData.getOpacity());
        _vPosition.push_back(gsData.getPosition());
        _vLineTag.push_back(gsData.getTag());
        //touchImaged出现
        _bookData = _gameController->getBookData();
        string sBookPath =_bookData.getBookPath();
        string sFile = sBookPath + "/image/" + gsData.getImageTouched();
        Sprite* spTmp = Sprite::create(FileUtils::getInstance()->fullPathForFilename(sFile));
        spTmp->setPosition(gsData.getPosition());
        spTmp->setTag(gsData.getTag()+2016);
        spTmp->setScale(gsData.getScale());
        spTmp->setLocalZOrder(gsData.getZOrder());
        spTmp->setOpacity(255);
        spTmp->setRotation(gsData.getRotation());
        this->addChild(spTmp);
        _iOrder= _iOrder+1;
        
        if(_iOrder == _iMaxOrder+1)
        {
            promptAnimation(1);
            for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
            {
                gsData =it->second;
                if(1 == gsData.getOrderId())
                {
                    this->removeChildByTag(2016+gsData.getTag());
                    break;
                }
            }
        }
        else
        {
            promptAnimation(_iOrder);
        }
    }
    //判断是不是要首末段相连
    else if(_vPosition.size()>1 && _iOrderId ==1&&_iOrder == _iMaxOrder+1)
    {
        _vPosition.push_back(gsData.getPosition());
        
    }
    //胜利动画
    if(_iOrder == _iMaxOrder+1&&_vPosition.size() == _iMaxOrder+1&&_isWin == false)
    {
        _isWin = true;
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("WinAnimation");

    }
    return true;
}
void OrderLineLayer::onTouchMoved(Touch* touch, Event* event)
{
}
void OrderLineLayer::onTouchEnded(Touch* touch, Event* event)
{
}
void OrderLineLayer::onTouchCancelled(Touch* touch, Event* event)
{
}
//初始化第一个连线精灵
void OrderLineLayer::initFirstOrderSprite()
{
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    GameSpriteData gsData;
    for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
    {
        gsData =it->second;
        int orderId = gsData.getOrderId();
        if(orderId == 1)
        {
            _bookData = _gameController->getBookData();
            string sBookPath =_bookData.getBookPath();
            string sFile = sBookPath + "/image/" + gsData.getImageTouched();
            Sprite* spTmp = Sprite::create(FileUtils::getInstance()->fullPathForFilename(sFile));
            spTmp->setPosition(gsData.getPosition());
            spTmp->setTag(gsData.getTag()+2016);
            spTmp->setAnchorPoint(gsData.getAnchorPoint());
            spTmp->setLocalZOrder(gsData.getZOrder());
            spTmp->setOpacity(gsData.getOpacity());
            spTmp->setRotation(gsData.getRotation());
            spTmp->setScale(gsData.getScale());
            this->addChild(spTmp);
            _vPosition.push_back(gsData.getPosition());
            _vLineTag.push_back(gsData.getTag());
            promptAnimation(2);
        }
        if (orderId > 0 && orderId > _iMaxOrder)
        {
            _iMaxOrder = orderId;
        }

    }
}
//提示动画
void OrderLineLayer::promptAnimation(int orderId)
{
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    GameSpriteData gsData;
    for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
    {
        gsData =it->second;
        if(orderId == gsData.getOrderId())
        {
            auto sp = it->first;
            auto scaleToBig = ScaleTo::create(1.0, 1.1*BookParser::getInstance()->getResourcesScale());
            auto scaleToSmall = ScaleTo::create(1.0, 1*BookParser::getInstance()->getResourcesScale());
            auto fadeToSmall = FadeTo::create(1.0, 100);
            auto fadeToBig = FadeTo::create(1.0, 255);
            sp->runAction(RepeatForever::create(Sequence::create(Spawn::create(scaleToBig,fadeToSmall, nullptr),Spawn::create(scaleToSmall,fadeToBig, nullptr), NULL)));
            
        }
        if(orderId-1 == gsData.getOrderId())
        {
            auto sp = it->first;
            sp->stopAllActions();
        }
        //点击连线末端sp时
        if(orderId == 1)
        {
            if(_iMaxOrder == gsData.getOrderId())
            {
                auto sp = it->first;
                sp->stopAllActions();
            }
        }
    }
}
//绘制连线
void OrderLineLayer::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    unsigned long i = _vPosition.size();
    if (i > 1)
    {
        for (int j=1; j<_vPosition.size(); j++)
        {
            _drawNode->drawSegment(_vPosition[j-1], _vPosition[j], 8, Color4F::RED);
            _drawNode->visit();
        }
    }
    //当最后一根线连好后移除所有的线和精灵
    if (_vPosition.size()>1 && _iOrderId ==1&&_iOrder == _iMaxOrder+1)
    {
        Node *parent = this->getParent();
        _vPosition.clear();
        for(int i = 0;i < _vLineTag.size();i++)
        {
            parent->removeChildByTag(_vLineTag[i]);
            this->removeChildByTag(2016+_vLineTag[i]);
            
        }
    }
}

NS_FK_END



