#include "FKIndexLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    _iCount = 1;
    _gameController = new GameController();
    
    //通过currentPage获取pageData
    BookParser* bookParser = BookParser::getInstance();
    _bookData = bookParser->getBookData();
    _iCurrentPage = bookParser->getCurrentPage();
    _gameController->setCurrentPage(_iCurrentPage);
    
    //解析GameSpriteData，并添加GameSprite到页面
    vector<GameSpriteData> vGsData = _gameController->getGameSpriteVector();
    initSprite(vGsData);
    
    //将animationGroupDataVector传入animationGroupController
    _animationGroupController.setAnimationGroupDataVector(_gameController->getAnimationGroupDataVector());
    
    //自动运动的动画
    autoAnimationRunAction(_animationGroupController.getAutoAnimationGroupMemberKeyData());
    
    //添加游戏页面
    gameTypeAddLayer();

    return true;
}

void GameLayer::onExit()
{
    Layer::onExit();
    this->removeAllChildrenWithCleanup(true);
    delete _gameController;
}

void GameLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("WinAnimation");

}
void GameLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(false);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(GameLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(GameLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(GameLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(GameLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 1);
    //注册胜利动画事件
    Director::getInstance()->getEventDispatcher()->addCustomEventListener("WinAnimation", CC_CALLBACK_1(GameLayer::winAnimation, this));

}
void GameLayer::initSprite(vector<GameSpriteData> vTmp)
{
    string sImageDic = _bookData.getBookPath() + "/image/";

    Sprite* gsTmp;
    GameSpriteData gsData;
    for (unsigned int i = 0; i < vTmp.size(); i++) {
        gsData = vTmp[i];
        gsTmp = _gameSpriteController.getSpriteFromGameSpriteData(gsData,sImageDic);
        if (gsTmp != nullptr) {
            this->addChild(gsTmp);
            _gameController->setGameSpriteAndDataMap(gsTmp, gsData);
            if (gsData.getIsWin() == "yes") {
                _winGsData = gsData;
            }
        }
    }
}

void GameLayer::gameTypeAddLayer()
{
    PageData pageData = BookController::getPageTypeFromCurrentPage(_iCurrentPage);
    string sGameType = pageData.getGameType();
    
    if (0 == strcmp(sGameType.c_str(), "puzzle")) {
        string sIsOrder = pageData.getIsOrder();
        if(0 == strcmp(sIsOrder.c_str(), "yes")){
            log("GameType : OrderPuzzle");
            auto layer = OrderPuzzleLayer::create();
            this->addChild(layer,-1,1000);
            layer->setUserObject(_gameController);
        }
        else{
            log("GameType : NormalPuzzle");
            auto layer = PuzzleLayer::create();
            this->addChild(layer,-1,1000);
            layer->setUserObject(_gameController);
        }
    }
    else if (0 == strcmp(sGameType.c_str(), "draw"))
    {
        log("GameType : Draw");
        auto layer = DrawLayer::create();
        this->addChild(layer,1000,1000);
        layer->setUserObject(_gameController);
    }
    else if (0 == strcmp(sGameType.c_str(), "filling"))
    {
        log("GameType : Filling");
        auto layer = FillingLayer::create();
        this->addChild(layer,-1,1000);
        layer->setUserObject(_gameController);
    }
    else if (0 == strcmp(sGameType.c_str(), "findit"))
    {
        log("GameType : FindIt");
        auto layer = FindItLayer::create();
        this->addChild(layer,-1,1000);
        layer->setUserObject(_gameController);
    }
    else if (0 == strcmp(sGameType.c_str(), "line"))
    {
        log("GameType : Line");
        auto layer = BaseLineLayer::create();
        this->addChild(layer,-1,1000);
        layer->setUserObject(_gameController);
    }
    else if (0 == strcmp(sGameType.c_str(), "orderline"))
    {
        log("GameType : OrderLine");
        auto layer = OrderLineLayer::create();
        this->addChild(layer,-1,1000);
        layer->setUserObject(_gameController);
    }
    else if (0 == strcmp(sGameType.c_str(), "complexline"))
    {
        log("GameType : ComplexLine");
        auto layer = ComplexLineLayer::create();
        this->addChild(layer,-1,1000);
        layer->setUserObject(_gameController);
    }
    else if (0 == strcmp(sGameType.c_str(), "maze"))
    {
        log("GameType : Maze");
        auto layer = MazeLayer::create();
        this->addChild(layer,1000,1000);
        layer->setUserObject(_gameController);
    }
    else if (0 == strcmp(sGameType.c_str(), "spotdifference"))
    {
        log("GameType : SpotDifference");
        auto layer = SpotDifferenceLayer::create();
        this->addChild(layer,-1,1000);
        layer->setUserObject(_gameController);
    }
    
}

#pragma mark - touch事件
bool GameLayer::onTouchBegan(Touch* touch, Event* event)
{
    map<int,GameSpriteData> mZorder = touchCollision(touch->getLocation());
    
    //对sp进行排序，返回spriteTag
    if (mZorder.size() > 0) {
        _gameSpriteData = getDataFromZOrder(mZorder);
        _iCurrentTag = _gameSpriteData.getTag();
    }else
    {
        _iCurrentTag = Node::INVALID_TAG;
    }
    
    log("_iCurrentTag = %d",_iCurrentTag);
    //点读功能
    playSpriteSoundWithTag(_iCurrentTag);
    //字幕操作
    if(_gameController->getPageData().getHasSubtitle() == "yes")
    {
        operateSubtitle();
    }
    //触发方式：点击
    if (_gameSpriteData.getTouchEnable() == "no" && _iCurrentTag != Node::INVALID_TAG) {
        //判断切片是否触发touch动画
        if (isSpriteRunTouchAction(_iCurrentTag)) {
            relateAnimationRunAction(_iCurrentTag,_gameSpriteData, "touch");
        }
    }
    return true;
}
void GameLayer::onTouchMoved(Touch* touch, Event* event)
{
    Point location = touch->getLocation();
    //触发方式：拖动
    if (_gameSpriteData.getTouchEnable() == "yes")
    {
        auto sp = this->getChildByTag(_iCurrentTag);
        sp->setLocalZOrder(9999);
        sp->setPosition(location);
    }
    else if (_gameSpriteData.getTouchEnable() == "no")
    {
        //触发方式：划动
        map<int,GameSpriteData> mZOrderTag = touchCollision(location);
        int iSwipeTag;
        GameSpriteData gsData;
        if (mZOrderTag.size() > 0) {
            gsData = getDataFromZOrder(mZOrderTag);
            iSwipeTag = gsData.getTag();
        }else
        {
            iSwipeTag = Node::INVALID_TAG;
        }
        
        if (iSwipeTag != Node::INVALID_TAG) {
            if (isSpriteRunTouchAction(iSwipeTag)) {
                relateAnimationRunAction(iSwipeTag,gsData, "swipe");
            }
        }
    }
}
void GameLayer::onTouchEnded(Touch* touch, Event* event)
{
    Point location = touch->getLocation();
    //触发方式：拖动
    if (_gameSpriteData.getTouchEnable() == "yes") {
        Rect rect = Rect(_gameSpriteData.getEndPosition().x, _gameSpriteData.getEndPosition().y, 200, 200);
        auto sp = this->getChildByTag(_iCurrentTag);
        sp->setLocalZOrder(_gameSpriteData.getZOrder());
        if (rect.containsPoint(location))
        {
            sp->setPosition(location);
            relateAnimationRunAction(_iCurrentTag,_gameSpriteData, "touch");
        }else
        {
            sp->setPosition(_gameSpriteData.getPosition());
        }
    }else
    {
        
    }
}
void GameLayer::onTouchCancelled(Touch* touch, Event* event)
{
}

map<int,GameSpriteData> GameLayer::touchCollision(cocos2d::Vec2 location)
{
    map<int,GameSpriteData> mTouchCollision;
    //获取点击精灵的tag值
    map<Sprite* ,GameSpriteData>mTmp = _gameController->getGameSpriteAndDataMap();
    for (map<Sprite* ,GameSpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it)
    {
        Sprite* sp = it->first;
        if(sp->getBoundingBox().containsPoint(location))
        {
            GameSpriteData gsData = it->second;
            if (gsData.getIsAlpha() == "yes") {
                if (PixelCollision::getInstance()->collidesWithPoint((Sprite*)sp, location))
                {
                    mTouchCollision[gsData.getZOrder()] = gsData;
                }
            }else
            {
                mTouchCollision[gsData.getZOrder()] = gsData;
            }
        }
    }
    return mTouchCollision;
}

//胜利动画事件监听
void GameLayer::winAnimation(EventCustom *eventCustom)
{
    if (_winGsData.getIsWin() == "yes") {
        relateAnimationRunAction(_winGsData.getTag(), _winGsData, "touch");
        playSpriteSoundWithTag(_winGsData.getTag());
    }
}

GameSpriteData GameLayer::getDataFromZOrder(map<int, GameSpriteData> &mTmp)
{
    return mTmp.rbegin()->second;
}

#pragma mark - 点击sprite以后的功能
//点读
void GameLayer::playSpriteSoundWithTag(int iTag)
{
    string sBookPath = _bookData.getBookPath();
    string sFileName = _gameController->getSpriteDataFromSpriteTag(iTag).getSoundId();
    log("sFileName = %s",sFileName.c_str());
    if (!sFileName.empty()) {
        string sFilePath = sBookPath + "/sound/" + sFileName;
        if (FileUtils::getInstance()->isFileExist(sFilePath))
        {
            AudioPlayer::getInstance()->playEffect(sFilePath, _gameController->getPageData().getInterrupt());
        }
    }
}

bool GameLayer::isSpriteRunTouchAction(int iTag)
{
    Node* sprite = this->getChildByTag(iTag);
    Action* touchAction = sprite->getActionByTag(ACTION_TOUCH);
    //判断切片是否触发touch动画
    if (touchAction != nullptr) {
        return false;
    }
    
    map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> mTmp = _animationGroupController.getAnimationGroupMemberDataFromSpriteTag(iTag);
    if (mTmp.size() > 0 && iTag != Node::INVALID_TAG)
    {
        map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>>::iterator it = mTmp.begin();
        vector<AnimationGroupMemberDataValue> vAnimationGroupMemberDataValueTmp = it->second;
        
        //遍历所有的receive的action
        if(vAnimationGroupMemberDataValueTmp.size() > 0)
        {
            AnimationGroupMemberDataValue animationGroupMemberDataValue;
            for(unsigned int i = 0 ;i < vAnimationGroupMemberDataValueTmp.size() ; i++)
            {
                animationGroupMemberDataValue = vAnimationGroupMemberDataValueTmp[i];
                int iReceieveTag = vAnimationGroupMemberDataValueTmp[i].getSpriteTag();
                auto rsp = this->getChildByTag(iReceieveTag);
                Action *touchAction = rsp->getActionByTag(ACTION_TOUCH);
                if (touchAction != nullptr) {
                    return false;
                }
            }
        }
    }
    return true;
}

//关联动画
void GameLayer::relateAnimationRunAction(int iTag, GameSpriteData gsData, string categroyId)
{
    //获取点击sprite的AnimationGroupMemberDataKey和AnimationGroupMemberDataValue
    map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>> mTmp = _animationGroupController.getAnimationGroupMemberDataFromSpriteTag(iTag);
    if (mTmp.size() > 0 && iTag != Node::INVALID_TAG)
    {
        map<AnimationGroupMemberDataKey, vector<AnimationGroupMemberDataValue>>::iterator it = mTmp.begin();
        vector<AnimationGroupMemberDataValue> vAnimationGroupMemberDataValueTmp = it->second;
        AnimationGroupMemberDataKey animationGroupMemberDataKey = it->first;
        
        auto sp = (Sprite*)this->getChildByTag(iTag);
        vector<AnimationData> vAnimationData = animationGroupMemberDataKey.getAnimationData();
        //点击换图
        if (replaceImageWithTouch(sp, vAnimationData,categroyId))
        {
            
        }
        //获取event为send的action
        else
        {
            Action* ac = _animationGroupMemberController.getSendActionFromAnimationGroupMemberDataKey(animationGroupMemberDataKey,categroyId);
            if (ac != nullptr)
            {
                //判断动画触发类型
                if (categroyId == "auto") {
                    ac->setTag(ACTION_AUTO);
                    sp->runAction(ac);
                }else
                {
                    ac->setTag(ACTION_TOUCH);
                    Action* autoAction = sp->getActionByTag(ACTION_AUTO);
                    //判断是否触发自动动画
                    if (autoAction != nullptr) {
                        Action* copyAutoAction = autoAction->clone();
                        copyAutoAction->retain();
                        sp->stopActionByTag(ACTION_AUTO);
                        sp->runAction(Sequence::create((FiniteTimeAction*)ac,CallFuncN::create([copyAutoAction](Node* node){
                            copyAutoAction->setTag(ACTION_AUTO);
                            node->runAction(copyAutoAction);
                            copyAutoAction->release();
                        }), NULL));
                    }else
                    {
                        sp->runAction(ac);
                    }
                }
            }
        }
        //遍历所有的receieve的action
        if(vAnimationGroupMemberDataValueTmp.size() > 0)
        {
            AnimationGroupMemberDataValue animationGroupMemberDataValue;
            for(unsigned int i = 0 ; i < vAnimationGroupMemberDataValueTmp.size() ; i++)
            {
                animationGroupMemberDataValue = vAnimationGroupMemberDataValueTmp[i];
                int iReceieveTag = vAnimationGroupMemberDataValueTmp[i].getSpriteTag();
                auto rsp = this->getChildByTag(iReceieveTag);
                //获取event为receive的action
                Action* rac = _animationGroupMemberController.getReceiveActionFromAnimationGroupMemberDataValue(animationGroupMemberDataValue,categroyId);
                if (rac != nullptr && rsp != NULL)
                {
                    //判断动画类型
                    if (categroyId == "auto") {
                        rac->setTag(ACTION_AUTO);
                        rsp->runAction(rac);
                    }else
                    {
                        rac->setTag(ACTION_TOUCH);
                        Action* autoAction = rsp->getActionByTag(ACTION_AUTO);
                        //判断是否触发自动动画
                        if (autoAction != nullptr) {
                            Action *copyAutoAction = autoAction->clone();
                            copyAutoAction->retain();
                            rsp->stopActionByTag(ACTION_AUTO);
                            rsp->runAction(Sequence::create((FiniteTimeAction*)rac,CallFuncN::create([copyAutoAction](Node* node){
                                copyAutoAction->setTag(ACTION_AUTO);
                                node->runAction(copyAutoAction);
                                copyAutoAction->release();
                            }), NULL));
                        }else
                        {
                            rsp->runAction(rac);
                        }
                    }
                }
            }
        }
        
    }
}
//自动动画
void GameLayer::autoAnimationRunAction(vector<AnimationGroupMemberData> mTmp)
{
    int iSpriteTag;
    Action* ac;
    for (size_t i = 0; i < mTmp.size(); i++) {
        iSpriteTag = mTmp[i].getSpriteTag();
        ac = _animationGroupMemberController.getActionFromAnimationGroupMemberData(mTmp[i],"auto");
        auto sp = this->getChildByTag(iSpriteTag);
        if (ac != nullptr && sp != NULL)
        {
            ac->setTag(ACTION_AUTO);
            sp->runAction(ac);
        }
    }
}
//点击切换图片
bool GameLayer::replaceImageWithTouch(Sprite* sprite , vector<AnimationData> &vAnimationData ,string categroyId)
{
    if (vAnimationData.size() == 0) {
        return false;
    }
    for (unsigned int i = 0; i < vAnimationData.size(); i++) {
        string styleId = vAnimationData[i].getStyleId();
        
        if (styleId == "imagereplace" && vAnimationData[i].getCategoryId() == categroyId) {
            int count = vAnimationData[i].getCount();
            char imageName[50];
            char soundName[50];
            string imagePath;
            string soundPath;
            string imageId = vAnimationData[i].getImageId();
            string soundId = vAnimationData[i].getSoundId();
            
            if (_iCount <= count) {
                if (_iCount == count) {
                    _iCount = 1;
                }else
                {
                    _iCount++;
                }
                
                if (imageId != "") {
                    imageId.erase(imageId.end()-4,imageId.end());
                    sprintf(imageName, "%s_%d.png", imageId.c_str(),_iCount);
                    imagePath = _bookData.getBookPath() + "/image/" + imageName;
                    
                }
                
                if (soundId != "") {
                    soundId.erase(soundId.end()-4,soundId.end());
                    sprintf(soundName, "%s_%d.mp3", soundId.c_str(),_iCount);
                    soundPath = _bookData.getBookPath() + "/sound/" + soundName;
                }
            }
            sprite->setTexture(imagePath);
            if (FileUtils::getInstance()->isFileExist(soundPath)){
                AudioPlayer::getInstance()->playEffect(soundPath, _gameController->getPageData().getInterrupt());
            }
            return true;
        }
    }
    return false;
}

//字幕控制
void GameLayer::operateSubtitle()
{
    //暂停字幕和声音
    if(_gameSpriteData.getIsSubtitle()=="")
    {
        pauseOrResumeSubtitle("pause");
        AudioPlayer::getInstance()->pauseOrResumeSubtitleMusic("pause");
    }
    //恢复字幕和声音
    else if(_gameSpriteData.getIsSubtitle()=="yes")
    {
        pauseOrResumeSubtitle("resume");
        AudioPlayer::getInstance()->pauseOrResumeSubtitleMusic("resume");
    }
    //字幕和声音重播
    if(_gameSpriteData.getIsReplay() == "yes")
    {
        string backgroundPath = _bookData.getBookPath() + "/sound/" + _gameController->getPlayBackgroundMusic();
        AudioPlayer::getInstance()->playBackgroundMusic(backgroundPath, false);
    }
    
    
}
//字幕的暂停和续播
void GameLayer::pauseOrResumeSubtitle(string sPauseOrResume)
{
    map<Sprite* ,GameSpriteData>mSGTmp = _gameController->getGameSpriteAndDataMap();
    for (map<Sprite* ,GameSpriteData>::iterator it = mSGTmp.begin(); it != mSGTmp.end(); ++it)
    {
        GameSpriteData gsData = it->second;
        if(gsData.getIsSubtitle() == "yes")
        {
            if(sPauseOrResume == "pause")
            {
                auto sp = it->first;
                Director::getInstance()->getActionManager()->pauseTarget(sp);
            }
            else if(sPauseOrResume == "resume")
            {
                auto sp = it->first;
                Director::getInstance()->getActionManager()->resumeTarget(sp);
            }
            
        }
    }    
}

NS_FK_END
