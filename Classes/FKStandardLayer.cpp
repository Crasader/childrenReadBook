#include "FKStandardLayer.h"

NS_FK_BEGIN

// on "init" you need to initialize your instance
bool StandardLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    _iCount = 1;
    //通过currentPage获取pageData
    BookParser* bookParser = BookParser::getInstance();
    _bookData = bookParser->getBookData();
    _iCurrentPage = bookParser->getCurrentPage();
    _standardController.setCurrentPage(_iCurrentPage);
    
    //解析spriteData，并添加sprite到页面
    map<int, SpriteData> mSpData = _standardController.getSpriteMap();
    initSprite(mSpData);
    
    //将animationGroupDataVector传入animationGroupController
    _animationGroupController.setAnimationGroupDataVector(_standardController.getAnimationGroupDataVector());
    //自动运动的动画
    autoAnimationRunAction(_animationGroupController.getAutoAnimationGroupMemberKeyData());
    
    return true;
}

void StandardLayer::onExit()
{
    Layer::onExit();
    this->removeAllChildrenWithCleanup(true);
}

void StandardLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}

void StandardLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(StandardLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(StandardLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(StandardLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(StandardLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 1);
    
}


void StandardLayer::initSprite(map<int, SpriteData> mTmp)
{
    string sImageDic = _bookData.getBookPath() + "/image/";

    Sprite* spTmp;
    for (map<int, SpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it) {
        spTmp = _spriteController.getSpriteFromSpriteData(it->second,sImageDic);
        if (spTmp != nullptr) {
            this->addChild(spTmp);
        }
    }
}

#pragma mark - touch事件

bool StandardLayer::onTouchBegan(Touch* touch, Event* event)
{
    map<int, int> mZOrderTag = touchCollision(touch->getLocation());
    
    //对sp进行排序，返回spriteTag
    if (mZOrderTag.size() > 0) {
        _iCurrentTag = getTagFromZOrder(mZOrderTag);
    }else
    {
        _iCurrentTag = Node::INVALID_TAG;
    }
    
    log("_iCurrentTag = %d",_iCurrentTag);
    //点读
    playSpriteSoundWithTag(_iCurrentTag);
    
    _spriteData = _standardController.getSpriteDataFromSpriteTag(_iCurrentTag);
    
    log("ImageId = %s",_spriteData.getImageId().c_str());
    //字幕操作
    if(_standardController.getPageData().getHasSubtitle() == "yes")
    {
        operateSubtitle();
    }
    //触发方式：点击
    if (_spriteData.getTouchEnable() == "no" && _iCurrentTag != Node::INVALID_TAG)
    {
        //判断切片是否触发touch动画
        if (isSpriteRunTouchAction(_iCurrentTag)) {
            relateAnimationRunAction(_iCurrentTag, "touch");
        }
    }
    
    return true;
}
void StandardLayer::onTouchMoved(Touch* touch, Event* event)
{
    Point location = touch->getLocation();
    //触发方式：拖动
    if (_spriteData.getTouchEnable() == "yes")
    {
        auto sp = this->getChildByTag(_iCurrentTag);
        sp->setLocalZOrder(9999);
        sp->setPosition(location);
    }
    else if (_spriteData.getTouchEnable() == "no")
    {
        //触发方式：划动
        map<int,int> mZOrderTag = touchCollision(location);
        int iSwipeTag;
        if (mZOrderTag.size() > 0) {
            iSwipeTag = getTagFromZOrder(mZOrderTag);
        }else
        {
            iSwipeTag = Node::INVALID_TAG;
        }
        
        if (iSwipeTag != Node::INVALID_TAG) {
            if (isSpriteRunTouchAction(iSwipeTag)) {
                relateAnimationRunAction(iSwipeTag, "swipe");
            }
        }
    }
    
}
void StandardLayer::onTouchEnded(Touch* touch, Event* event)
{
    Point location = touch->getLocation();
    //触发方式：拖动
    if (_spriteData.getTouchEnable() == "yes") {
        Rect rect = Rect(_spriteData.getEndPosition().x, _spriteData.getEndPosition().y, 200, 200);
        auto sp = this->getChildByTag(_iCurrentTag);
        sp->setLocalZOrder(_spriteData.getZOrder());
        if (rect.containsPoint(location))
        {
            sp->setPosition(_spriteData.getEndPosition());
            relateAnimationRunAction(_iCurrentTag, "touch");
        }else
        {
            sp->setPosition(_spriteData.getPosition());
        }
    }else
    {
        
    }
}
void StandardLayer::onTouchCancelled(Touch* touch, Event* event)
{
}

map<int,int> StandardLayer::touchCollision(Vec2 location)
{
    map<int,int> mTouchCollision;
    for (auto sp : this->getChildren()) {
        if (sp->getBoundingBox().containsPoint(location)) {
            SpriteData spriteData = _standardController.getSpriteDataFromSpriteTag(sp->getTag());
            if (spriteData.getIsAlpha() == "yes") {
                if (PixelCollision::getInstance()->collidesWithPoint((Sprite*)sp, location))
                {
                    mTouchCollision[sp->getLocalZOrder()] = sp->getTag();
                }
            }else
            {
                mTouchCollision[sp->getLocalZOrder()] = sp->getTag();
            }
        }
    }
    return mTouchCollision;
}

#pragma mark - 点击sprite以后的功能
//得到Z值最大的sprite的tag
int StandardLayer::getTagFromZOrder(map<int, int> &mTmp)
{
    return mTmp.rbegin()->second;
}
void StandardLayer::playSpriteSoundWithTag(int iTag)
{
    string sBookPath = _bookData.getBookPath();
    string sFileName = _standardController.getSpriteDataFromSpriteTag(iTag).getSoundId();
    if (!sFileName.empty()) {
        string sFilePath = sBookPath + "/sound/" + sFileName;
        if (FileUtils::getInstance()->isFileExist(sFilePath))
        {
            AudioPlayer::getInstance()->playEffect(sFilePath, _standardController.getPageData().getInterrupt());
            
        }
    }
}

bool StandardLayer::isSpriteRunTouchAction(int iTag)
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
void StandardLayer::relateAnimationRunAction(int iTag, string categroyId)
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
        
        //判断是否是可切换图片
        if (replaceImageWithTouch(sp, vAnimationData , categroyId))
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
        //遍历所有的receive的action
        if(vAnimationGroupMemberDataValueTmp.size() > 0)
        {
            AnimationGroupMemberDataValue animationGroupMemberDataValue;
            for(unsigned int i = 0 ;i < vAnimationGroupMemberDataValueTmp.size() ; i++)
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
void StandardLayer::autoAnimationRunAction(vector<AnimationGroupMemberData> mTmp)
{
    int iSpriteTag;
    Action* ac;
    Sprite* sp;
    for (size_t i = 0; i < mTmp.size(); i++) {
        iSpriteTag = mTmp[i].getSpriteTag();
        ac = _animationGroupMemberController.getActionFromAnimationGroupMemberData(mTmp[i],"auto");
        sp = (Sprite*)this->getChildByTag(iSpriteTag);
        if (ac != nullptr && sp != nullptr)
        {
            ac->setTag(ACTION_AUTO);
            sp->runAction(ac);
        }
    }
}
//点击切换图片
bool StandardLayer::replaceImageWithTouch(Sprite* sprite , vector<AnimationData> &vAnimationData,string categroyId)
{
    if (vAnimationData.size() == 0) {
        return false;
    }
    for (unsigned int i = 0; i < vAnimationData.size(); i++) {
        string styleId = vAnimationData[i].getStyleId();
        
        if (styleId == "imagereplace" && categroyId == vAnimationData[i].getCategoryId()) {
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
                AudioPlayer::getInstance()->playEffect(soundPath, _standardController.getPageData().getInterrupt());
            }
            return true;
        }
    }
    return false;
}

//字幕控制
void StandardLayer::operateSubtitle()
{
    //暂停字幕和声音
    if(_spriteData.getIsSubtitle()=="")
    {
        pauseOrResumeSubtitle("pause");
        AudioPlayer::getInstance()->pauseOrResumeSubtitleMusic("pause");
    }
    //恢复字幕和声音
    else if(_spriteData.getIsSubtitle()=="yes")
    {
        pauseOrResumeSubtitle("resume");
        AudioPlayer::getInstance()->pauseOrResumeSubtitleMusic("resume");
    }
    //字幕和声音重播
    if(_spriteData.getIsReplay() == "yes")
    {
        string backgroundPath = _bookData.getBookPath() + "/sound/" + _standardController.getPlayBackgroundMusic();
        AudioPlayer::getInstance()->playBackgroundMusic(backgroundPath, false);
    }
    
    
}
//字幕的暂停和续播
void StandardLayer::pauseOrResumeSubtitle(string sPauseOrResume)
{
    map<int, SpriteData> mSpData = _standardController.getSpriteMap();
    for (map<int, SpriteData>::iterator it = mSpData.begin(); it != mSpData.end(); ++it)
    {
        SpriteData spData = it->second;
        if(spData.getIsSubtitle() == "yes")
        {
            if(sPauseOrResume == "pause")
            {
                auto sp = this->getChildByTag(it->first);
                Director::getInstance()->getActionManager()->pauseTarget(sp);
            }
            else if(sPauseOrResume == "resume")
            {
                auto sp = this->getChildByTag(it->first);
                Director::getInstance()->getActionManager()->resumeTarget(sp);
            }
            
        }
    }
    
}

NS_FK_END

