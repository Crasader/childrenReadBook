#include "FKBaseLineLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool BaseLineLayer::init()
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

void BaseLineLayer::onExit()
{
    Layer::onExit();
}

void BaseLineLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void BaseLineLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(BaseLineLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(BaseLineLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(BaseLineLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(BaseLineLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
    //设置胜利动画相关数据
    setWinData();
}


#pragma mark - touch事件

bool BaseLineLayer::onTouchBegan(Touch* touch, Event* event)
{
    log("BaseLineLayer onTouchBegan");
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    GameSpriteData gsData;
    Node *parent = this->getParent();
    //获取点击sp的Data
    for ( auto sp : parent->getChildren())
    {
        if (sp->getBoundingBox().containsPoint(touch->getLocation())){
            for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it){
                if(sp == it->first)
                {
                    gsData = it->second;
                    //获取属性为main的Tag值和position
                    if (gsData.getRelation() == "main")
                    {
                        _iTagMain = gsData.getTag();
                        _positionMain = gsData.getPosition();
                        _vMainOrMinor.push_back("main");
                        
                        
                        auto sp=it->first;
                        auto scale1 = ScaleTo::create(0.6, 1.1*BookParser::getInstance()->getResourcesScale());
                        auto scale2 = ScaleTo::create(0.6, 1.0*BookParser::getInstance()->getResourcesScale());
                        
                        sp->runAction(RepeatForever::create(Sequence::create(scale1,scale2, NULL)));
                        _mTmpCopy[it->first] =gsData;
                        break;
                    }
                    //获取属性为minor的Tag值和position
                    else if (gsData.getRelation() == "minor")
                    {
                        _iTagMinor = gsData.getTag();
                        _positionMinor = gsData.getPosition();
                        _vMainOrMinor.push_back("minor");
                        
                        auto sp=it->first;
                        auto scale1 = ScaleTo::create(0.6, 1.1*BookParser::getInstance()->getResourcesScale());
                        auto scale2 = ScaleTo::create(0.6, 1.0*BookParser::getInstance()->getResourcesScale());
                        
                        sp->runAction(RepeatForever::create(Sequence::create(scale1,scale2, NULL)));
                        _mTmpCopy[it->first] =gsData;
                        break;
                    }
                    
                    
                }
            }
        }
    }
    log("iTagMain = %d,iTagMinor = %d",_iTagMain,_iTagMinor);
    //进行关联判断，Tag值相同的进行连线
    unsigned long iMainOrMinorSize = _vMainOrMinor.size();
    if (_vMainOrMinor.size() > 1)
    {
        //判断相邻两次点击是否为main和minor
        if(_iTagMinor == _iTagMain&&_vMainOrMinor[iMainOrMinorSize - 1] != _vMainOrMinor[iMainOrMinorSize - 2])
        {
            _vMainLocation.push_back(_positionMain);
            _vMinorLocation.push_back(_positionMinor);
            unsigned long minorLocationSize = _vMinorLocation.size();
            //重复点击无效
            for (unsigned int i = 0; i< _vMinorLocation.size() - 1; i++)
            {
                if (_vMinorLocation[i].x == _vMinorLocation[minorLocationSize-1].x&&_vMinorLocation[i].y == _vMinorLocation[minorLocationSize-1].y)
                {
                    _vMainLocation.pop_back();
                    _vMinorLocation.pop_back();
                    break;
                }
            }
            
        }
        //停止选中动画恢复原状态
        for (map<Sprite* ,GameSpriteData>::iterator it = _mTmpCopy.begin(); it != _mTmpCopy.end(); ++it)
        {
            auto sp = it->first;
            auto gameSpriteData = it->second;
            sp->stopAllActions();
            sp->setScale(gameSpriteData.getScale());
        }
        _mTmpCopy.clear();
        _vMainOrMinor.clear();
    }
    //胜利动画
    if(_vMinorLocation.size() == _iMaxNumber)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("WinAnimation");
        _iMaxNumber++;
    }
    return true;
}
void BaseLineLayer::onTouchMoved(Touch* touch, Event* event)
{
}
void BaseLineLayer::onTouchEnded(Touch* touch, Event* event)
{
}
void BaseLineLayer::onTouchCancelled(Touch* touch, Event* event)
{
}
void BaseLineLayer::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    if (!_vMainLocation.empty()) {
        unsigned long i = _vMainLocation.size();
        //绘制连线
        _drawNode->drawSegment(_vMainLocation[i-1], _vMinorLocation[i-1], 8, Color4F::RED);
        _drawNode->visit();
    }
}
void BaseLineLayer::setWinData()
{
    _iMaxNumber = 0;
    map<Sprite* ,GameSpriteData> mTmp = _gameController->getGameSpriteAndDataMap();
    for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
    {
        //获取可连线切片的最大数目
        GameSpriteData gsData;
        gsData = it->second;
        string getMain = gsData.getRelation();
        if(getMain == "minor")
        {
            _iMaxNumber = _iMaxNumber + 1;
        }
    }
}

NS_FK_END


