#include "FKComplexLineLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool ComplexLineLayer::init()
{
    if ( !Layer::init() )
    {
        
        return false;
    }
    //渲染render
    Director *director = Director::getInstance();
    Size _winSize = director->getWinSize();
    _renderTexture = RenderTexture::create(_winSize.width, _winSize.height,Texture2D::PixelFormat::RGBA8888);
    _renderTexture->setPosition(Vec2(_winSize.width/2, _winSize.height/2));
    this->addChild(_renderTexture);

    //生成draw
    _drawNode = DrawNode::create();
    _drawNode->retain();
    return true;
}

void ComplexLineLayer::onExit()
{
    Layer::onExit();
    this->removeAllChildrenWithCleanup(true);
}

void ComplexLineLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void ComplexLineLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(ComplexLineLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(ComplexLineLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(ComplexLineLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(ComplexLineLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
    
    //设置胜利动画参数
    _isLineFinish=true;
    _iLineFinishNumber=0;
    setWinData();
}


#pragma mark - touch事件

bool ComplexLineLayer::onTouchBegan(Touch* touch, Event* event)
{
    log("ComplexLineLayer onTouchEnd");
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
    log("_iTag = %d",_iTag);
    int groupId = gsData.getGroupId();
    log("groupId = %d",groupId);
    //找出对应的lineGroupId
    //如果点击的是顺序或无序的切片
    if(groupId > 0)
    {
        vector<ComplexLineData> vComplexLineData = _gameController->getComplexLineDataVector();
        string lineMode;
        for(unsigned int i = 0; i < vComplexLineData.size();i++)
        {
            if(vComplexLineData[i].getLineGroupId() == groupId)
            {
                lineMode = vComplexLineData[i].getLineMode();
                if(_isLineFinish == true&&lineMode=="order")
                {
                    _vOrderComplexLineTagData = vComplexLineData[i].getComplexLineTagData();
                    _iCurrentLine = i;
                    
                }
                else
                {
                    _vDisorderComplexLineTagData = vComplexLineData[i].getComplexLineTagData();
                }
                break;
            }
        }
        //如果是无序连线
        if(lineMode == "disorder")
        {
            //将点击获取的tag值存入_vTagQueue队列中，队列中最多只存两个数据
            _vDisorderCurrentTagQueue.push_back(_iTag);
            //选中设置动画
            auto sp = parent->getChildByTag(_iTag);
            auto scale1 = ScaleTo::create(0.6, 1.1*BookParser::getInstance()->getResourcesScale());
            auto scale2 = ScaleTo::create(0.6, 1.0*BookParser::getInstance()->getResourcesScale());
            sp->runAction(RepeatForever::create(Sequence::create(scale1,scale2, NULL)));
            //判断上次点击是否是有序连线
            if(_vOrderTagQueue.size()==1)
            {
                auto spOrder=parent->getChildByTag(_vOrderTagQueue[0]);
                spOrder->stopAllActions();
                sp->stopAllActions();
                _vOrderTagQueue.clear();
                _vDisorderCurrentTagQueue.clear();
            }
            //两次点击都为无序
            if (_vDisorderCurrentTagQueue.size() >1)
            {
                //判断是否重复连线
                bool isRepeatLine = false;
                for(int i = 0;i<_vDisorderTagQueue.size();i++)
                {
                    if((_vDisorderTagQueue[i].x == _vDisorderCurrentTagQueue[0]&&_vDisorderTagQueue[i].y == _vDisorderCurrentTagQueue[1])||(_vDisorderTagQueue[i].x == _vDisorderCurrentTagQueue[1]&&_vDisorderTagQueue[i].y == _vDisorderCurrentTagQueue[0]))
                    {
                        isRepeatLine = true;
                        break;
                    }
                }
                if(isRepeatLine == false)
                {
                    //判断是否可联线
                    for (unsigned i = 0; i<_vDisorderComplexLineTagData.size();i++)
                    {
                        if ((_vDisorderCurrentTagQueue[0]== _vDisorderComplexLineTagData[i].getStartTag()&&_vDisorderCurrentTagQueue[1]== _vDisorderComplexLineTagData[i].getEndTag())||(_vDisorderCurrentTagQueue[1]== _vDisorderComplexLineTagData[i].getStartTag()&&_vDisorderCurrentTagQueue[0]== _vDisorderComplexLineTagData[i].getEndTag()))
                        {
                            int startTag = _vDisorderComplexLineTagData[i].getStartTag();
                            int endTag = _vDisorderComplexLineTagData[i].getEndTag();
                            auto startSP = parent->getChildByTag(startTag);
                            auto endSP = parent->getChildByTag(endTag);
                            //获取连线精灵的位置信息
                            _vStartPosition.push_back(startSP->getPosition());
                            _vEndPosition.push_back(endSP->getPosition());
                            _vDisorderTagQueue.push_back(Point(_vDisorderCurrentTagQueue[0],_vDisorderCurrentTagQueue[1]));
                            break;
                        }
                    }
                }
                //停止选中动画恢复原状态
                for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
                {
                    auto gsData = it->second;
                    for (unsigned int i = 0; i < _vDisorderCurrentTagQueue.size(); i++) {
                        auto sp = parent->getChildByTag(_vDisorderCurrentTagQueue[i]);
                        if (sp == it->first)
                        {
                            sp->stopAllActions();
                            sp->setScale(gsData.getScale());
                        }
                    }
                }
                _vDisorderCurrentTagQueue.clear();
            }
        }
        //如果是顺序连线
        else if(lineMode == "order")
        {
            _vOrderTagQueue.push_back(_iTag);
            //选中设置动画
            auto sp=parent->getChildByTag(_iTag);
            auto scale1 = ScaleTo::create(0.6, 1.1*BookParser::getInstance()->getResourcesScale());
            auto scale2 = ScaleTo::create(0.6, 1.0*BookParser::getInstance()->getResourcesScale());
            sp->runAction(RepeatForever::create(Sequence::create(scale1,scale2, NULL)));
            //判断上次点击是否是无序连线
            if(_vDisorderCurrentTagQueue.size()==1)
            {
                auto spDisorder=parent->getChildByTag(_vDisorderCurrentTagQueue[0]);spDisorder->stopAllActions();
                sp->stopAllActions();
                _vOrderTagQueue.clear();
                _vDisorderCurrentTagQueue.clear();
            }
            //两次点击都为有序
            if (_vOrderTagQueue.size()>1)
            {
                //判断是否重复连线
                bool isRepeatLine = false;
                if(_vLineQueue.size()>0)
                {
                    for(int i = 0;i<_vLineQueue.size();i++)
                    {
                        if(_vLineQueue[i]==_iCurrentLine)
                        {
                            isRepeatLine = true;
                            break;
                        }
                    }
                }
                if(isRepeatLine == false)
                {
                    unsigned long lineNumber = _vOrderPosition.size()/2;
                    unsigned long tagSize = _vOrderTagQueue.size();
                    if((_vOrderComplexLineTagData[lineNumber].getStartTag() == _vOrderTagQueue[tagSize-1]&&_vOrderComplexLineTagData[lineNumber].getEndTag() == _vOrderTagQueue[tagSize-2])||
                       (_vOrderComplexLineTagData[lineNumber].getStartTag() == _vOrderTagQueue[tagSize-2]&&_vOrderComplexLineTagData[lineNumber].getEndTag() == _vOrderTagQueue[tagSize-1]))
                    {
                        auto startSP = parent->getChildByTag(_vOrderComplexLineTagData[lineNumber].getStartTag());
                        auto endSP = parent->getChildByTag(_vOrderComplexLineTagData[lineNumber].getEndTag());
                        //获取连线精灵的位置信息
                        _vOrderPosition.push_back(startSP->getPosition());
                        _vOrderPosition.push_back(endSP->getPosition());
                        _iLineFinishNumber++;
                        _isLineFinish = false;
                        if(_iLineFinishNumber % _vOrderComplexLineTagData.size() == 0)
                        {
                            _vLineQueue.push_back(_iCurrentLine);
                        }
                    }
                }
                //停止选中动画恢复原状态
                for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
                {
                    auto gsData = it->second;
                    for (unsigned int i = 0; i < _vOrderTagQueue.size(); i++) {
                        auto sp = parent->getChildByTag(_vOrderTagQueue[i]);
                        if (sp == it->first)
                        {
                            sp->stopAllActions();
                            sp->setScale(gsData.getScale());
                        }
                    }
                }
                _vOrderTagQueue.clear();
            }
        }
        if(_iMaxLine == _vStartPosition.size()+_iLineFinishNumber)
            
        {
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("WinAnimation");
            _iMaxLine++;
        }
    }
    return true;
}
void ComplexLineLayer::onTouchMoved(Touch* touch, Event* event)
{
}
void ComplexLineLayer::onTouchEnded(Touch* touch, Event* event)
{
}
void ComplexLineLayer::onTouchCancelled(Touch* touch, Event* event)
{
}
//绘制连线
void ComplexLineLayer::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    //无序连线
    if(_vEndPosition.size()!=0)
    {
        _drawNode->drawSegment(_vStartPosition[_vStartPosition.size()-1], _vEndPosition[_vEndPosition.size()-1], 8, Color4F::RED);
        _drawNode->visit();
    }
    _renderTexture->begin();
    //顺序连线
    unsigned long i = _vOrderPosition.size();
    if(_vOrderPosition.size()%2==0&&i>0)
    {
        _drawNode->drawSegment(_vOrderPosition[i-2], _vOrderPosition[i-1], 8, Color4F::RED);
        _drawNode->visit();
    }
     _renderTexture->end();
    if(_vOrderPosition.size()>0&&_vOrderPosition.size()/2 == _vOrderComplexLineTagData.size())
    {
        _vOrderPosition.clear();
        _isLineFinish = true;
        _vOrderComplexLineTagData.clear();
        
    }
}
void ComplexLineLayer::setWinData()
{
    //取出顺序连线和无序连线的连线数目
    vector<ComplexLineData> vComplexLineData = _gameController->getComplexLineDataVector();
    vector<ComplexLineTagData> _vComplexLineTagData;
    for (unsigned int i = 0; i < vComplexLineData.size(); i++)
    {
        string lineMode = vComplexLineData[i].getLineMode();
        if (lineMode == "disorder") {
            _vComplexLineTagData =vComplexLineData[i].getComplexLineTagData();
            _iMaxLine = _vComplexLineTagData.size()+_iMaxLine;
        }
        else if (lineMode == "order") {
            _vComplexLineTagData =vComplexLineData[i].getComplexLineTagData();
            _iMaxLine = _vComplexLineTagData.size()+_iMaxLine;
        }
    }
}

NS_FK_END

