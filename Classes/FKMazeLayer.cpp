#include "FKMazeLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool MazeLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    return true;
}

void MazeLayer::onExit()
{
    Layer::onExit();
}

void MazeLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void MazeLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(MazeLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(MazeLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(MazeLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(MazeLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
    initPrompt();
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
    {
        GameSpriteData gsData = it->second;
        //获取主角切片tag值
        string role = gsData.getRole();
        if (role == "protagonist") {
            _roleSprite = it->first;
        }
    }
}


#pragma mark - touch事件

bool MazeLayer::onTouchBegan(Touch* touch, Event* event)
{
    log("MazeLayer onTouchBegan");
    //initPrompt();
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    GameSpriteData gsData;
    Action* ac;
    //获取点击切片的keyPointTag
    int keyPointTag;
    auto parent = (GameLayer*)this->getParent();
    map<int,GameSpriteData> mTouchCollision = parent->touchCollision(touch->getLocation());
    if (mTouchCollision.size()>0) {
        gsData = parent->getDataFromZOrder(mTouchCollision);
        keyPointTag = gsData.getTag();
    }else
    {
        keyPointTag = Node::INVALID_TAG;
    }
    
    string sAnimationType;
    vector<AnimationData> _vAnimationData;
    //根据点击位置判断怎样移动
    //获取路径信息
    string isWin = "No";
    Vector<FiniteTimeAction*> vAction;
    vector<KeyPathData> vKeyPathData = _gameController->getKeyPathDataVector();
    Point location = parent->convertTouchToNodeSpace(touch);
    for (int i =0; i< vKeyPathData.size(); i++)
    {
        //获取普通点数据
        vector<NormalPointData> vNormalPointData = vKeyPathData[i].getNormalPointData();
        //获取起始点和终止点
        Point endPosition = vKeyPathData[i].getEndPointData().getPosition();
        Point startPosition = vKeyPathData[i].getStartPointData().getPosition();
        Rect endRect = Rect(endPosition.x-50,endPosition.y-50, 100,100);
        Rect startRect = Rect(startPosition.x-50,startPosition.y-50,100,100);
        
        //判断是否向前走
        if (endRect.containsPoint(location))
        {
            int normalPointTag;
            
            if (startPosition.equals(_roleSprite->getPosition()))
            {
                string pointType = vKeyPathData[i].getEndPointData().getPointType();
                if(pointType == "end")
                {
                    isWin = "Yes";
                }
                // 如果存在普通点
                if(vNormalPointData.size()>0)
                {
                    //获取普通点坐标
                    for(int j = 0;j< vNormalPointData.size();j++)
                    {
                        Point normalPoint = vNormalPointData[j].getPosition();
                        auto move = MoveTo::create(0.5, normalPoint);
                        vAction.pushBack(move);
                        //如果普通点存在动画
                        normalPointTag = vNormalPointData[j].getSpriteTag();
                        if (normalPointTag > -2)
                        {
                            //前进普通点动画动作
                            forwardNormalPointAnimationRunAction(vNormalPointData,j,normalPointTag);
                        }
                    }
                }
                //主角切片移动
                auto move = MoveTo::create(0.5, endPosition);
                vAction.pushBack(move);
                ac = Sequence::create(vAction);
                _roleSprite->runAction(ac);
                vAction.clear();
                
                //终点动画触发
                //设置延迟时间,以便roleSP到达后触发动画
                float delayTime = vNormalPointData.size()*0.5+0.5;
                endPointAnimationRunAction(vKeyPathData,keyPointTag,delayTime);
                //添加提示切片
                promptPath(vKeyPathData,vKeyPathData[i].getEndPointData().getCategory(),delayTime,isWin);
                break;
            }
            
        }
        //判断是否向后退
        else if(startRect.containsPoint(location))
        {
            int normalPointTag;
            if (endPosition.equals(_roleSprite->getPosition()))
            {
                //string pointType = vKeyPathData[i].getPointType();
                if(vNormalPointData.size() > 0)
                {
                    //获取普通点坐标
                    for(unsigned long j = vNormalPointData.size();j> 0;j--)
                    {
                        Point normalPoint = vNormalPointData[j-1].getPosition();
                        auto move = MoveTo::create(0.5, normalPoint);
                        vAction.pushBack(move);
                        //如果普通点存在动画
                        normalPointTag = vNormalPointData[j-1].getSpriteTag();
                        if (normalPointTag > -2)
                        {
                            backwardNormalPointAnimationRunAction(vNormalPointData,j,normalPointTag);
                        }
                        
                    }
                }
                auto move = MoveTo::create(0.5, startPosition);
                vAction.pushBack(move);
                ac = Sequence::create(vAction);
                _roleSprite->runAction(ac);
                vAction.clear();
                
                //终点动画触发
                //设置延迟时间,以便roleSP到达后触发动画
                float delayTime = vNormalPointData.size()*0.5+0.5;
                endPointAnimationRunAction(vKeyPathData,keyPointTag,delayTime);
                //添加提示切片
                promptPath(vKeyPathData,vKeyPathData[i].getStartPointData().getCategory(),delayTime,isWin);
                break;
            }
        }
    }
    //胜利动画
    if (isWin == "Yes")
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("WinAnimation");
    }
    return true;
}
void MazeLayer::onTouchMoved(Touch* touch, Event* event)
{
}
void MazeLayer::onTouchEnded(Touch* touch, Event* event)
{
}
void MazeLayer::onTouchCancelled(Touch* touch, Event* event)
{
}
//前进普通点动画动作
void MazeLayer::forwardNormalPointAnimationRunAction(vector<NormalPointData> vNormalPointData, int j,int normalPointTag)
{
    vector<AnimationData> _vAnimationData;
    Node* parent = this->getParent();
    _vAnimationData = vNormalPointData[j].getAnimationData();
    string sAnimationType  = vNormalPointData[j].getAnimationType();
    int times = vNormalPointData[j].getTimes();
    float delayTime = (j+1)*0.5;
    //普通点绑定精灵运动
    auto  normalPointSP = parent->getChildByTag(normalPointTag);
    auto  delay = DelayTime::create(delayTime);
    Action* normalPointAC;
    normalPointAC = getMazeAction(_vAnimationData, sAnimationType,times);
    auto normalCombineAC = Sequence::create(delay,normalPointAC, NULL);
    normalPointSP->runAction(normalCombineAC);
}
//后退普通点动画动作
void MazeLayer::backwardNormalPointAnimationRunAction(vector<NormalPointData> vNormalPointData, unsigned long j, int normalPointTag)
{
    vector<AnimationData> _vAnimationData;
    Node* parent = this->getParent();
    _vAnimationData = vNormalPointData[j-1].getAnimationData();
    string sAnimationType  = vNormalPointData[j-1].getAnimationType();
    int times = vNormalPointData[j].getTimes();
    float delayTime = j*0.5;
    //普通点绑定精灵运动
    auto  normalPointSP = parent->getChildByTag(normalPointTag);
    auto  delay = DelayTime::create(delayTime);
    Action* normalPointAC;
    normalPointAC = getMazeAction(_vAnimationData, sAnimationType,times);
    auto normalCombineAC = Sequence::create(delay,normalPointAC, NULL);
    normalPointSP->runAction(normalCombineAC);
}
//终点动画
void MazeLayer::endPointAnimationRunAction(vector<KeyPathData> vKeyPathData, int keyPointTag,float delayTime)
{
    vector<AnimationData>_vAnimationData;
    Node* parent = this->getParent();
    Action* ac;
    for (int i =0; i<vKeyPathData.size(); i++)
    {
        if(keyPointTag == vKeyPathData[i].getEndPointData().getSpriteTag())
        {
            string sAnimationType = vKeyPathData[i].getEndPointData().getAnimationType();
            _vAnimationData = vKeyPathData[i].getEndPointData().getAnimationData();
            int times = vKeyPathData[i].getEndPointData().getTimes();
            //切片运动
            auto  sp = parent->getChildByTag(keyPointTag);
            auto  delay = DelayTime::create(delayTime);
            ac = getMazeAction(_vAnimationData, sAnimationType,times);
            auto combineAC = Sequence::create(delay,ac, NULL);
            sp->runAction(combineAC);
            break;
        }
    }
    
}
//获取迷宫动画的动作
Action* MazeLayer::getMazeAction(vector<AnimationData> vAnimationDataTmp,string sAnimationType,int times)
{
    Vector<FiniteTimeAction*> vAction;
    for (size_t i = 0; i < vAnimationDataTmp.size(); i++) {
        vAction.pushBack(AnimationController::getActionFromAnimationData(vAnimationDataTmp[i]));
    }
    
    Action* ac = nullptr;
    if (sAnimationType == "spawn" && vAction.size() > 0) {
        ac = Spawn::create(vAction);
    }
    else if (sAnimationType == "sequence" && vAction.size() > 0)
    {
        ac = Sequence::create(vAction);
    }
    else if (sAnimationType == "repeat" && vAction.size() > 0)
    {
        ac = Repeat::create(Sequence::create(vAction), times);
    }
    else if (sAnimationType == "repeatforever" && vAction.size() > 0)
    {
        ac = RepeatForever::create(Sequence::create(vAction));
    }
    else if (sAnimationType == "null" && vAction.size() > 0)
    {
        ac = vAction.at(0);
    }
    
    return ac;
    
}

//初始化提示路径
void MazeLayer::initPrompt()
{
    vector<KeyPathData> vKeyPathData = _gameController->getKeyPathDataVector();
    for (int i = 0; i<vKeyPathData.size(); i++)
    {
        int startCategory = vKeyPathData[i].getStartPointData().getCategory();
        string pointType = vKeyPathData[i].getStartPointData().getPointType();
        auto spStart = this->getChildByTag(2016+startCategory);
        
        if(spStart == NULL)
        {
            auto  spPromt = Sprite::create("quan.png");
            spPromt->setPosition(vKeyPathData[i].getStartPointData().getPosition());
            spPromt->setOpacity(0);
            auto scale1 = ScaleTo::create(1.0, 0.7);
            auto scale2 = ScaleTo::create(1.0, 0.5);
            auto action = RepeatForever::create(Sequence::create(scale1,scale2,NULL));
            
            spPromt->runAction(action);
            this->addChild(spPromt,100,2016+startCategory);
        }
        int endCategory = vKeyPathData[i].getEndPointData().getCategory();
        auto spEnd = this->getChildByTag(2016+endCategory);
        if(spEnd == NULL)
        {
            auto  spPromt = Sprite::create("quan.png");
            spPromt->setPosition(vKeyPathData[i].getEndPointData().getPosition());
            if(pointType == "start")
            {
                spPromt->setOpacity(255);
            }
            else
            {
                spPromt->setOpacity(0);
            }
            auto scale1 = ScaleTo::create(1.0, 0.7);
            auto scale2 = ScaleTo::create(1.0, 0.5);
            auto action = RepeatForever::create(Sequence::create(scale1,scale2,NULL));
            
            spPromt->runAction(action);
            this->addChild(spPromt,100,2016+endCategory);
        }
    }
    
}
//提示可走路径
void MazeLayer::promptPath(vector<KeyPathData> vKeyPathData,int iCategory,float fDelayTime,string isWin)
{
    //start点的提示切片
    if(_vCategory.size() == 0)
    {
        
        auto sp=this->getChildByTag(2016+iCategory);
        if(sp!= NULL)
        {
            auto fadeTo = FadeTo::create(0.0,0);
            auto delay = DelayTime::create(fDelayTime);
            auto sequence = Sequence::create(delay, fadeTo, NULL);
            sp->runAction(sequence);
        }
    }
    //其他提示切片
    else if(_vCategory.size() > 0)
    {
        for(int i = 0;i<_vCategory.size();i++)
        {
            auto sp=this->getChildByTag(2016+_vCategory[i]);
            if(sp!= NULL)
            {
                auto fadeTo = FadeTo::create(0.0,0);
                auto delay = DelayTime::create(fDelayTime);
                auto sequence = Sequence::create(delay, fadeTo, NULL);
                sp->runAction(sequence);
            }
        }
        _vCategory.clear();
    }
    //如果角色所在位置不在终点
    if(isWin == "No")
    {
        for (int i = 0; i<vKeyPathData.size(); i++)
        {
            int startCategory = vKeyPathData[i].getStartPointData().getCategory();
            int endCategory = vKeyPathData[i].getEndPointData().getCategory();
            if(startCategory == iCategory)
            {
                _vCategory.push_back(vKeyPathData[i].getEndPointData().getCategory());
            }
            else if(endCategory == iCategory)
            {
                _vCategory.push_back(vKeyPathData[i].getStartPointData().getCategory());
            }
        }
        if(_vCategory.size()>0)
        {
            for (int i = 0; i<_vCategory.size(); i++)
            {
                auto spPromt = this->getChildByTag(2016+_vCategory[i]);
                if(spPromt!= NULL)
                {
                    auto  fadeTo = FadeTo::create(0.0,255);
                    auto  delay = DelayTime::create(fDelayTime);
                    auto sequence = Sequence::create(delay, fadeTo, NULL);
                    spPromt->runAction(sequence);
                }
            }
        }
    }
}

NS_FK_END
