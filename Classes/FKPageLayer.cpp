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
    scene->addChild(layer,2);

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
    showParticleEffect("auto");
    
    //播放背景音
    string backgroundPath = _bookData.getBookPath() + "/sound/" + _pageData.getSoundId();
    AudioPlayer::getInstance()->setBackgroundMusicVolume(1.0f);
    AudioPlayer::getInstance()->playBackgroundMusic(backgroundPath, false);
    
    return true;
}

void PageLayer::onExit()
{
    Layer::onExit();
    this->removeAllChildrenWithCleanup(true);
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
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 1);

    if (_bookParser->getBookPlayModeState() == PlayModeState::AUTOPLAY) {
        EventDispatcher *eventDispatcher = Director::getInstance()->getEventDispatcher();
        eventDispatcher->disEnabledEventListeners(false,EventListenerTouchOneByOne::LISTENER_ID);
    }

	BookParser::getInstance()->setPlayModeChangeCallBack([](PlayModeState playMode){
		EventDispatcher *eventDispatcher = Director::getInstance()->getEventDispatcher();
		switch (playMode)
		{
		case READ:
			eventDispatcher->disEnabledEventListeners(true, EventListenerTouchOneByOne::LISTENER_ID);
			break;
		case AUTOPLAY:
		case LISTEN:
			eventDispatcher->disEnabledEventListeners(false, EventListenerTouchOneByOne::LISTENER_ID);
			break;
		default:
			break;
		}
	});
}

void PageLayer::initResource()
{
    Size winSize = Director::getInstance()->getWinSize();
    CCLOG("屏幕分辨率 ＝ %.0f*%.0f",winSize.width,winSize.height);
    float highScale = winSize.height/1536;
    auto pageUp = MenuItemImage::create("ReadBook_PageUp_ipad.png",
                                        "ReadBook_PageUp_Selected_ipad.png",
                                        CC_CALLBACK_1(PageLayer::pageUp, this));
    pageUp->setPosition(-winSize.width/2, -winSize.height/2);
    pageUp->setScale(highScale*0.63);
    pageUp->setAnchorPoint(Vec2(0, 0));
    pageUp->setTag(BUTTON_TAG_PAGE_UP);
    if (_iCurrentPage == 1) {
        pageUp->unselected();
        pageUp->setVisible(false);
    }
    
    auto pageDown = MenuItemImage::create("ReadBook_PageDown_ipad.png",
                                          "ReadBook_PageDown_Selected_ipad.png",
                                          CC_CALLBACK_1(PageLayer::pageDown, this));
    pageDown->setPosition(winSize.width/2, 2-winSize.height/2);
    pageDown->setScale(highScale*0.63);
    pageDown->setAnchorPoint(Vec2(1.0f, 0));
    pageDown->setTag(BUTTON_TAG_PAGE_DOWN);
    //if (_iCurrentPage == _bookData.getPages()) {
        //pageDown->unselected();
        //pageDown->setVisible(false);
    //}
    
    auto pageQuit = MenuItemImage::create("Common_Return_ipad@2x.png",
                                          "Common_Return_Selected_ipad@2x.png",
                                          CC_CALLBACK_1(PageLayer::pageQuit, this));
    pageQuit->setPosition(-winSize.width/2, winSize.height/2);
    pageQuit->setScale(highScale*0.8);
    pageQuit->setAnchorPoint(Vec2(0, 1.0f));
    pageQuit->setTag(BUTTON_TAG_PAGE_QUIT);
    
    auto pageShare = MenuItemImage::create("ReadBook_Share.png",
                                          "ReadBook_Share_Selected.png",
                                          CC_CALLBACK_1(PageLayer::pageShare, this));
    pageShare->setPosition(winSize.width/2, winSize.height/2);
    pageShare->setScale(highScale*0.8);
    pageShare->setAnchorPoint(Vec2(1.0f, 1.0f));
    pageShare->setTag(BUTTON_TAG_PAGE_SHARE);
    
    Menu *menuPage = Menu::create(pageUp,pageDown,pageQuit,pageShare,NULL);
	//Menu *menuPage = Menu::create(pageUp, pageDown, pageQuit,  NULL);
    this->addChild(menuPage,100,10000);
    BookParser::getInstance()->setPageMenu(menuPage);
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
			_bookParser->bookQuit();
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
			_isButtonEnable = true;
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
			_isButtonEnable = true;
		}
		else
		{
			_bookParser->pageDown();
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

bool PageLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _touchLocation = touch->getLocation();
    showParticleEffect("touch");
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

//粒子系统
void PageLayer::showParticleEffect(string sAutoOrTouch)
{
    vector<ParticleSystemData> vParticleSystemData = _pageData.getParticleSystemData();
    ParticleSystem *particle = nullptr;
    if(vParticleSystemData.size()>0)
    {
        for(unsigned long i = 0; i<vParticleSystemData.size();i++)
        {
            string particleImagePath = _bookData.getBookPath() + "/image/" +vParticleSystemData[i].getImageId();
            string sParticleStyle = vParticleSystemData[i].getParticleStyle();
            string sCategory = vParticleSystemData[i].getCategory();
            int iZorder = vParticleSystemData[i].getZorder();
            if(sCategory == "auto"&&sAutoOrTouch == "auto")
            {
                if(sParticleStyle == "snow")
                {
                    particle = ParticleSnow::create();
                }
                else if(sParticleStyle == "fire")
                {
                    particle = ParticleFire::create();
                }
                else if(sParticleStyle == "firework")
                {
                    particle = ParticleFireworks::create();
                }
                else if(sParticleStyle == "sun")
                {
                    particle = ParticleSun::create();
                }
                else if(sParticleStyle == "galaxy")
                {
                    particle = ParticleGalaxy::create();
                }
                else if(sParticleStyle == "flower")
                {
                    particle = ParticleFlower::create();
                }
                else if(sParticleStyle == "meteor")
                {
                    particle = ParticleMeteor::create();
                }
                else if(sParticleStyle == "spiral")
                {
                    particle = ParticleSpiral::create();
                }
                else if(sParticleStyle == "explosion")
                {
                    particle = ParticleExplosion::create();
                }
                else if(sParticleStyle == "smoke")
                {
                    particle = ParticleSmoke::create();
                }
                else if(sParticleStyle == "rain")
                {
                    particle = ParticleRain::create();
                    particle->setLife(10);
                    particle->setStartSize(10.0f);
                    particle->setStartSizeVar(5.0f);
                }
                particle->setTexture(Director::getInstance()->getTextureCache()->addImage(particleImagePath));
                particle->setPosition(vParticleSystemData[i].getPosition());
                this->addChild(particle,iZorder);
            }
            else if(sCategory == "touch"&&sAutoOrTouch == "touch")
            {
                if(sParticleStyle == "snow")
                {
                    particle = ParticleSnow::create();
                }
                else if(sParticleStyle == "fire")
                {
                    particle = ParticleFire::create();
                }
                else if(sParticleStyle == "firework")
                {
                    particle = ParticleFireworks::create();
                }
                else if(sParticleStyle == "sun")
                {
                    particle = ParticleSun::create();
                }
                else if(sParticleStyle == "galaxy")
                {
                    particle = ParticleGalaxy::create();
                }
                else if(sParticleStyle == "flower")
                {
                    particle = ParticleFlower::create();
                }
                else if(sParticleStyle == "meteor")
                {
                    particle = ParticleMeteor::create();
                }
                else if(sParticleStyle == "spiral")
                {
                    particle = ParticleSpiral::create();
                }
                else if(sParticleStyle == "explosion")
                {
                    particle = ParticleExplosion::create();
                }
                else if(sParticleStyle == "smoke")
                {
                    particle = ParticleSmoke::create();
                }
                else if(sParticleStyle == "rain")
                {
                    particle = ParticleRain::create();
                }
                particle->setPosition(_touchLocation);
                particle->setTexture(Director::getInstance()->getTextureCache()->addImage(particleImagePath));
                particle->setDuration(2);
                this->addChild(particle,iZorder);
            }
        }
        
    }
}

NS_FK_END

