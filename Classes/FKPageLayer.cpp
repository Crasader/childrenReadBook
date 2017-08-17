#include "FKPageLayer.h"
NS_FK_BEGIN

Scene* PageLayer::createScene()
{
    auto scene = Scene::create();
    
    //通过currentPage获取pageType
    BookParser* bookParser = BookParser::getInstance();
    int iCurrentPage = bookParser->getCurrentPage();
    PageData pageData = BookController::getPageTypeFromCurrentPage(iCurrentPage);
    string sPageType = pageData.getPageType();
    
    //判断当前页面是standard页面还是game页面
    if (0 == strcmp(sPageType.c_str(), "standard"))
    {
        auto layer = StandardLayer::create();
        scene->addChild(layer,1,1);
    }
    else if (0 == strcmp(sPageType.c_str(), "game"))
    {
        auto layer = GameLayer::create();
        scene->addChild(layer,1,1);
    }
    
    auto layer = PageLayer::create();
    scene->addChild(layer,2,2);

    return scene;
}

// on "init" you need to initialize your instance
bool PageLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    _isButtonEnable = true;
    _bookParser = BookParser::getInstance();
    _iCurrentPage = _bookParser->getCurrentPage();
    _bookData = _bookParser->getBookData();
    _pageData = BookController::getPageTypeFromCurrentPage(_iCurrentPage);
    
    if (!_bookParser->getIsReading()) {
        PreLoadResourcesController::firstEnterLayerPreloadResources();
        _bookParser->setIsReading(true);
    }
    log("------------------------------------------第%s页--------------------------------------------",toString(_iCurrentPage).c_str());
    
    initResource();
    showVideo();
    _particleSystemController.addParticleSystemForLayer(_pageData.getParticleSystemData(), "auto", this, _bookData.getBookPath(), Vec2::ZERO);
    
    string buf = Director::getInstance()->getTextureCache()->getCachedTextureInfo();
    //解决android下logcat输出不全问题
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    if(buf.size() > 1000) {
        for(int i = 0;i<buf.size();i+=1000){
            if(i+1000<buf.size()){
                log("%s",buf.substr(i,i+1000).c_str());
            }
            else{
                log("%s",buf.substr(i,buf.size()).c_str());
            }
        }
    }else{
        log("%s", buf.c_str());
    }
#else
    log("%s", buf.c_str());
#endif
    return true;
}

void PageLayer::doAutoPlay(float time)
{
	BookParser::getInstance()->pageDown(false);
}

void PageLayer::onExit()
{
    Layer::onExit();
}

void PageLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
	BookParser::getInstance()->setPlayModeChangeCallBack(nullptr);
}

void PageLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(false);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(PageLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(PageLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(PageLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(PageLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, -1);

    playBackgroundMusic();
    
    if (_bookParser->getBookPlayModeState() == PlayModeState::AUTOPLAY) {
        EventDispatcher *eventDispatcher = Director::getInstance()->getEventDispatcher();
        eventDispatcher->disEnabledEventListeners(false,EventListenerTouchOneByOne::LISTENER_ID);
    }
    
	BookParser::getInstance()->setPlayModeChangeCallBack([this](PlayModeState playMode){
		EventDispatcher *eventDispatcher = Director::getInstance()->getEventDispatcher();
		switch (playMode)
		{
		case READ:
			eventDispatcher->disEnabledEventListeners(true, EventListenerTouchOneByOne::LISTENER_ID);
			unschedule(schedule_selector(PageLayer::doAutoPlay));
			break;
		case AUTOPLAY:
		case LISTEN:
			eventDispatcher->disEnabledEventListeners(false, EventListenerTouchOneByOne::LISTENER_ID);
            if (AudioPlayer::getInstance()->getBackgroundPlayAudioState() == experimental::AudioEngine::AudioState::ERROR)
			{
				scheduleOnce(schedule_selector(PageLayer::doAutoPlay), 2.0f);
			}
			break;
		default:
			break;
		}
	});
}

void PageLayer::playBackgroundMusic()
{
    string backgroundPath;
    if (_pageData.getSubtitleSoundId() == "") {
        backgroundPath = _bookData.getBookPath() + "/sound/" + _pageData.getSoundId();
    }else
    {
        backgroundPath = _bookData.getBookPath() + "/sound/" + _pageData.getSubtitleSoundId();
        string bgPath = _bookData.getBookPath() + "/sound/" + _pageData.getSoundId();
        AudioEngine::play2d(bgPath);
    }
    
    _subtitleController.addSubtitle(this, _pageData);
    
    //播放背景音
    if (FileUtils::getInstance()->isFileExist(backgroundPath) && backgroundPath.find(".mp3") != backgroundPath.npos)
    {
        AudioPlayer::getInstance()->playBackgroundMusic(backgroundPath, false);
        AudioPlayer::getInstance()->setBackgroundMusicVolume(1.0f);
    }
    //如果当前页没有背景音且是自动播放模式，设置5s后翻页
    else if (_bookParser->getBookPlayModeState() == PlayModeState::AUTOPLAY)
    {
        scheduleOnce(schedule_selector(PageLayer::doAutoPlay), 5.0f);
    }
}

void PageLayer::initResource()
{
    Size winSize = Director::getInstance()->getWinSize();
    CCLOG("屏幕分辨率 ＝ %.0f*%.0f",winSize.width,winSize.height);
    Size resSize = _bookParser->getResourceSize();
    bool isScale = false;
    float sizeOffset = resSize.width - winSize.width;
    if (50 < sizeOffset) {
        isScale = true;
    }
    auto pageUp = MenuItemImage::create("ReadBook_PageUp_667h.png",
                                        "ReadBook_PageUp_Selected_667h.png",
                                        CC_CALLBACK_1(PageLayer::pageUp, this));
    pageUp->setPosition(-winSize.width/2, -winSize.height/2);
    pageUp->setAnchorPoint(Vec2(0, 0));
    pageUp->setTag(BUTTON_TAG_PAGE_UP);
    if (_iCurrentPage == 1) {
        pageUp->unselected();
        pageUp->setVisible(false);
    }
    
    auto pageDown = MenuItemImage::create("ReadBook_PageDown_667h.png",
                                          "ReadBook_PageDown_Selected_667h.png",
                                          CC_CALLBACK_1(PageLayer::pageDown, this));
    pageDown->setPosition(winSize.width/2, 2-winSize.height/2);
    pageDown->setAnchorPoint(Vec2(1.0f, 0));
    pageDown->setTag(BUTTON_TAG_PAGE_DOWN);
	if (_iCurrentPage == _bookData.getPages()) {
		pageDown->unselected();
		pageDown->setVisible(false);
	}
    
    auto pageQuit = MenuItemImage::create("Common_Return_667h.png",
                                          "Common_Return_Selected_667h.png",
                                          CC_CALLBACK_1(PageLayer::pageQuit, this));
    pageQuit->setPosition(-winSize.width/2, winSize.height/2);
    pageQuit->setAnchorPoint(Vec2(0, 1.0f));
    pageQuit->setTag(BUTTON_TAG_PAGE_QUIT);
    
    auto pageShare = MenuItemImage::create("ReadBook_Share.png",
                                          "ReadBook_Share_Selected.png",
                                          CC_CALLBACK_1(PageLayer::pageShare, this));
    pageShare->setPosition(winSize.width/2, winSize.height/2);
    pageShare->setScale(0.5);
    pageShare->setAnchorPoint(Vec2(1.0f, 1.0f));
    pageShare->setTag(BUTTON_TAG_PAGE_SHARE);
    
    
    if (isScale) {
        pageDown->setScale(0.65);
        pageUp->setScale(0.65);
    }
    Menu *menuPage = Menu::create(pageUp,pageDown,pageQuit,pageShare,NULL);
    this->addChild(menuPage,100,10000);
    BookParser::getInstance()->setPageMenu(menuPage);
    
    BookParser::getInstance()->runPageMenuChangeCallBack();
}

void PageLayer::setMenuOpacity(float opacity)
{
    Menu *menuPage = BookParser::getInstance()->getPageMenu();
    menuPage->setOpacity(opacity);
}

void PageLayer::pageShare(cocos2d::Ref *pSender)
{
    if (_isButtonEnable) {
        _isButtonEnable = false;
        AudioPlayer::getInstance()->stopAllEffect();
        BookParser::getInstance()->runBookShareCallBack();
        _isButtonEnable = true;
    }
}

void PageLayer::pageQuit(Ref* pSender)
{
    if (_isButtonEnable) {
        _isButtonEnable = false;
		BookParser::PageCallBack callback = _bookParser->getPageQuitCallBack();
		if (callback != nullptr)
		{
			_bookParser->runPageQuitCallBack();
		}
		else
		{
            _bookParser->bookQuit();
		}
		_isButtonEnable = true;
    }
}

void PageLayer::pageUp(Ref* pSender)
{
    if (_isButtonEnable) {
        _isButtonEnable = false;
		if (_bookParser->getPageUpCallBack() != nullptr)
		{
			_bookParser->runPageUpCallBack();
			//_isButtonEnable = true;
		}
		else
		{
			_bookParser->pageUp();
		}
    }
    
}

void PageLayer::pageDown(Ref* pSender)
{
    if (_isButtonEnable) {
        _isButtonEnable = false;
       
		if (_bookParser->getPageDownCallBack() != nullptr)
		{
			_bookParser->runPageDownCallBack();
			//_isButtonEnable = true;
		}
		else
		{
			_bookParser->pageDown(true);
		}
    }
}

//视屏相关
void PageLayer::showVideo()
{
    vector<MovieData> vMovieData = _pageData.getMovieData();
    if(vMovieData.size() > 0)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        for(unsigned int i = 0;i<vMovieData.size();i++)
        {
            string sMovieId = vMovieData[i].getMovieId();
            string sMoviePath = _bookData.getBookPath() + "/movie/" + sMovieId;
            Vec4   frame = vMovieData[i].getFrame();
            auto videoPlayer = experimental::ui::VideoPlayer::create();
            videoPlayer->setPosition(Point(frame.x, frame.y));
            videoPlayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            videoPlayer->setContentSize(Size(frame.z , frame.w));
            this->addChild(videoPlayer);
            if (videoPlayer)
            {
                videoPlayer->setFileName(sMoviePath);
                videoPlayer->play();
            }
            videoPlayer->addEventListener(CC_CALLBACK_2(PageLayer::videoEventCallback, this));
            
        }
#endif
        
    }
}
void PageLayer::videoPlayOverCallback()
{
    
}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void PageLayer::videoEventCallback(Ref* sender, experimental::ui::VideoPlayer::EventType eventType){
    switch (eventType) {
        case experimental::ui::VideoPlayer::EventType::PLAYING:
            break;
        case experimental::ui::VideoPlayer::EventType::PAUSED:
            break;
        case experimental::ui::VideoPlayer::EventType::STOPPED:
            break;
        case experimental::ui::VideoPlayer::EventType::COMPLETED:
            videoPlayOverCallback();
            break;
        default:
            break;
    }
}
#endif

void PageLayer::pauseSubtitles()
{
    _subtitleController.pauseSubtitles();
}

bool PageLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    Point touchLocation = touch->getLocation();
    for (auto sp : this->getChildren()) {
        if (sp->getBoundingBox().containsPoint(touchLocation)) {
            if (sp->getName() == "subtitle") {
                if (AudioPlayer::getInstance()->getBackgroundPlayAudioState() == AudioEngine::AudioState::PLAYING) {
                    
                }else if (AudioPlayer::getInstance()->getBackgroundPlayAudioState() == AudioEngine::AudioState::PAUSED)
                {
                    _subtitleController.resumeSubtitles();
                }else
                {
                    string backgroundPath;
                    if (_pageData.getSubtitleSoundId() == "") {
                        backgroundPath = _bookData.getBookPath() + "/sound/" + _pageData.getSoundId();
                    }else
                    {
                        backgroundPath = _bookData.getBookPath() + "/sound/" + _pageData.getSubtitleSoundId();
                    }
                    _subtitleController.replaySubtitles(backgroundPath);
                }
                break;
            }
        }
    }
    
    _particleSystemController.addParticleSystemForLayer(_pageData.getParticleSystemData(), "touch", this, _bookData.getBookPath(), touchLocation);
    return true;
}

void PageLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{

}

void PageLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{

}

void PageLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{

}

NS_FK_END

