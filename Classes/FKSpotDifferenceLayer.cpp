#include "FKSpotDifferenceLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool SpotDifferenceLayer::init()
{
    if ( !Layer::init() )
    {
        
        return false;
    }
    
    return true;
    
}

void SpotDifferenceLayer::onExit()
{
    Layer::onExit();
    this->removeAllChildrenWithCleanup(true);
}

void SpotDifferenceLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void SpotDifferenceLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(SpotDifferenceLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(SpotDifferenceLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(SpotDifferenceLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(SpotDifferenceLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
    //胜利动画参数
    setWinData();
    
}


#pragma mark - touch事件

bool SpotDifferenceLayer::onTouchBegan(Touch* touch, Event* event)
{
    log("SpotDifferenceLayer onTouchBegan");
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    GameSpriteData gsData;
    auto parent = (GameLayer*)this->getParent();
    map<int,GameSpriteData> mTouchCollision = parent->touchCollision(touch->getLocation());
    int iTag;
    if (mTouchCollision.size()>0) {
        gsData = parent->getDataFromZOrder(mTouchCollision);
        iTag = gsData.getTag();
    }else
    {
        iTag = Node::INVALID_TAG;
    }
    int iRelationId = gsData.getRelationId();
    //取出相互关联的tag
    if(iRelationId!=0)
    {
        log("tag=%d",iTag);
        _vTagQueue.push_back(iTag);
        string sRepeatTouch = "no";
        
        //判断是否重复点击
        for(int i = 0;i<_vTagQueue.size()-1;i++)
        {
            //重复点击退出循环，不作操作
            if(_vTagQueue[i]==_vTagQueue[_vTagQueue.size()-1])
            {
                _vTagQueue.pop_back();
                sRepeatTouch = "yes";
                break;
            }
        }
        //不是重复点击把position存起来
        if(sRepeatTouch=="no")
        {
            auto sp = parent->getChildByTag(iTag);
            Point size = sp->getContentSize();
            //计算图片半径
            _vPosition.push_back(gsData.getPosition());
            for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
            {
                //取出关联精灵的Data
                GameSpriteData rgsData;
                rgsData = it->second;
                if(rgsData.getRelationId() == iRelationId && rgsData.getTag() != iTag)
                {
                    _vPosition.push_back(rgsData.getPosition());
                    _vTagQueue.push_back(rgsData.getTag());
                    auto sp = parent->getChildByTag(rgsData.getTag());
                    Size size = sp->getContentSize();
                    drawCircle(size);
                    break;
                }
            }
        }
        
    }
    //胜利动画
    if(_vTagQueue.size()==_iMaxNumber)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("WinAnimation");
        _iMaxNumber++;
    }
    return true;
}
void SpotDifferenceLayer::onTouchMoved(Touch* touch, Event* event)
{
}
void SpotDifferenceLayer::onTouchEnded(Touch* touch, Event* event)
{
}
void SpotDifferenceLayer::onTouchCancelled(Touch* touch, Event* event)
{
}

//画圈
void SpotDifferenceLayer::drawCircle(Size spContentSize)
{
    Node *parent = this->getParent();
    if (_vPosition.size()==2)
    {
        auto circle1 = Sprite::create("circle.png");
        circle1->setAnchorPoint(Vec2(0.5, 0.5));
        circle1->setPosition(_vPosition[0]);
        
        auto circle2 = Sprite::create("circle.png");
        circle2->setAnchorPoint(Vec2(0.5, 0.5));
        circle2->setPosition(_vPosition[1]);
        
        Size circleSize = circle1->getContentSize();
        if(spContentSize.height/circleSize.height>spContentSize.width/circleSize.width)
        {
            circle1->setScale(spContentSize.width/circleSize.width);
            circle2->setScale(spContentSize.width/circleSize.width);
        }
        else
        {
            circle1->setScale(spContentSize.height/circleSize.height);
            circle2->setScale(spContentSize.height/circleSize.height);
            
        }
        
        parent->addChild(circle1, 100,2016);
        parent->addChild(circle2, 100,2016);
        _vPosition.clear();
    }
    
}
//设置胜利动画
void SpotDifferenceLayer::setWinData()
{
    _iMaxNumber = 0;
    map<Sprite* ,GameSpriteData> mTmp = _gameController->getGameSpriteAndDataMap();
    for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
    {
        //获取不同游戏切片的最大数目
        GameSpriteData gsData;
        gsData = it->second;
        
        if(gsData.getRelationId()>0)
        {
            _iMaxNumber = _iMaxNumber+1;
        }
    }
}

NS_FK_END
