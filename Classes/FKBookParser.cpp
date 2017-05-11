#include "FKPageLayer.h"
#include "FKBookParser.h"

NS_FK_BEGIN

BookParser* BookParser::_bookParser = nullptr;

BookParser* BookParser::getInstance()
{
    if (_bookParser == nullptr) {
        _bookParser = new BookParser();
    }
    return _bookParser;
}

void BookParser::end()
{
	AudioPlayer::getInstance()->end();
    if (_bookParser != nullptr) {
        delete _bookParser;
        _bookParser = nullptr;	
    }
}

BookParser::BookParser()
:_bookEndCallBack(nullptr)
, _pageDownCallBack(nullptr)
,_pageUpCallBack(nullptr)
,_pageShareCallBack(nullptr)
,_pageQuitCallBack(nullptr)
,_playModeChangeCallBack(nullptr)
, _playMode(PlayModeState::DEFULT)
,_pageMenuChangeCallBack(nullptr)
{
	init();
}

void BookParser::init()
{
	_isReading = false;
	_bookData.clear();
	_iCurrentPage = 0;
	_fCoordinateScale = 0.0f;
	_fResourcesScale = 0.0f;
	_sBookPath.clear();
	_sPlatform.clear();
	_sDrawFilePath.clear();
	_isPaused = false;
	AudioPlayer::getInstance()->stopAllEffect();
	AudioPlayer::getInstance()->uncacheAll();
	SpriteFrameCache::getInstance()->removeSpriteFrames();
	Director::getInstance()->getTextureCache()->removeAllTextures();
}

Scene* BookParser::getRootScene()
{
#ifdef CC_PLATFORM_WIN32_DOT_NET
	auto blackLayer = LayerColor::create(Color4B(255, 255, 255, 255));
	auto rootScene = Scene::create();
	rootScene->addChild(blackLayer);
#else
	auto rootScene = IndexLayer::createScene();
#endif

	return rootScene;
}

BookParser::~BookParser()
{
    
}

int BookParser::bookJsonParser(string sBookPath, float resourcesScale, float coordinateScale,string platform)
{
	init();
    _sBookPath = sBookPath;
    _sPlatform = platform;
    _fResourcesScale = resourcesScale;
    _fCoordinateScale = coordinateScale;
    Vec2 winSizeOffset = computeWinSizeOffset();
    JsonParser jsonParser;
    int errCode = jsonParser.parserBook(sBookPath,resourcesScale,coordinateScale,winSizeOffset);
    if (errCode == 0) {
        log("parser success!");
        setCurrentPage(1);
        Director::getInstance()->replaceScene(PageLayer::createScene());
    }else
    {
        log("parser fail!");
    }
    return errCode;
}

cocos2d::Size BookParser::getResourceSize() const
{
	if (_resourceSize.equals(Size(0, 0)))
	{
		return Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
	}
	return _resourceSize;
}

void BookParser::setResourceSize(const cocos2d::Size& resourceSize)
{
	_resourceSize = resourceSize;
}

Vec2 BookParser::computeWinSizeOffset()
{
	Vec2 offset;
	//目前ResolutionPolicy::FIXED_HEIGHT情况下，getWinSize()获取的是进行过高度缩放后的Size。
	//偏移的目的是使整个画面裁剪后仍居中
	Size winSize = Director::getInstance()->getWinSize();
	Size resSize = getResourceSize();
	float offsetX = (resSize.width - winSize.width) / 2;
	float offsetY = (resSize.height - winSize.height) / 2;
	offset = Vec2(offsetX, offsetY);
	return offset;
}

void BookParser::setBookPlayModeState(PlayModeState playMode)
{
    _playMode = playMode;

	switch (_playMode)
	{
	case READ:
		AudioPlayer::getInstance()->setAutoPlayCallBack(nullptr);
		break;
	case AUTOPLAY:
		AudioPlayer::getInstance()->setAutoPlayCallBack([this](int musicId, std::string soundPath){
            pageDown(false);
		});
		break;
	case LISTEN:
		break;
	default:
		break;
	}

	if (_playModeChangeCallBack != nullptr)
	{
		_playModeChangeCallBack(_playMode);
	}
}

PlayModeState BookParser::getBookPlayModeState()
{
    return _playMode;
}

void BookParser::pausePlay()
{
	_isPaused = true;
	AudioPlayer::getInstance()->pauseAllEffect();
	//字幕的暂停(待添加)
}

void BookParser::resumePlay()
{
	_isPaused = false;
	AudioPlayer::getInstance()->resumeAllEffect();
	//字幕的恢复(待添加)
}

void BookParser::setBookData(BookData bookData)
{
    _bookData = bookData;
}

BookData BookParser::getBookData()
{
    return _bookData;
}

void BookParser::setCurrentPage(int iCurrentPage)
{
    _iCurrentPage = iCurrentPage;
}

int BookParser::getCurrentPage()
{
    return _iCurrentPage;
}

string BookParser::getBookPath()
{
    return _sBookPath;
}

string BookParser::getPlatform()
{
    return _sPlatform;
}

void BookParser::setPageMenu(cocos2d::Menu *pageMenu)
{
    _pageMenu = pageMenu;
}

Menu* BookParser::getPageMenu()
{
    return _pageMenu;
}

void BookParser::setPageMenuChangeCallBack(const PageMenuChangeCallBack &pageMenuChangeCallBack)
{
    _pageMenuChangeCallBack = pageMenuChangeCallBack;
}

void BookParser::runPageMenuChangeCallBack()
{
    if (_pageMenuChangeCallBack != nullptr) {
        _pageMenuChangeCallBack(_pageMenu);
    }
}

void BookParser::setIsReading(bool isReading)
{
    _isReading = isReading;
}

bool BookParser::getIsReading()
{
    return _isReading;
}

void BookParser::setDrawFilePath(string filePath)
{
    _sDrawFilePath = filePath;
}

string BookParser::getDrawFilePath()
{
    return _sDrawFilePath;
}

float BookParser::getCoordinateScale()
{
    return _fCoordinateScale;
}

float BookParser::getResourcesScale()
{
    return _fResourcesScale;
}

void BookParser::setPageUpCallBack(const PageCallBack &pageUpCallBack)
{
	_pageUpCallBack = pageUpCallBack;
}

void BookParser::setPageDownCallBack(const PageCallBack &pageDownCallBack)
{
	_pageDownCallBack = pageDownCallBack;
}

void BookParser::setPageQuitCallBack(const PageCallBack &pageQuitCallBack)
{
	_pageQuitCallBack = pageQuitCallBack;
}

void BookParser::runPageUpCallBack()
{
	if (_pageUpCallBack != nullptr) {
		_pageUpCallBack();
    }
}

void BookParser::runPageDownCallBack()
{
	if (_pageDownCallBack != nullptr) {
		_pageDownCallBack();
	}
}

void BookParser::runPageQuitCallBack()
{
	if (_pageQuitCallBack != nullptr) {
		_pageQuitCallBack();
	}
}

void BookParser::setBookShareCallBack(const ShareCallBack& pageShareCallBack)
{
	_pageShareCallBack = pageShareCallBack;
}

void BookParser::runBookShareCallBack()
{
	if (_pageShareCallBack != nullptr)
	{
		RenderTexture* renderTexture = pageScreenShot();
		_pageShareCallBack(renderTexture);
	}
}

void BookParser::setBookEndCallBack(const PageCallBack &bookEndCallBack)
{
	_bookEndCallBack = bookEndCallBack;
}

void BookParser::setPlayModeChangeCallBack(const PlayModeChangeCallBack &playModeChangeCallBack)
{
	_playModeChangeCallBack = playModeChangeCallBack;
}

RenderTexture* BookParser::pageScreenShot()
{
    Size winSize = Director::getInstance()->getWinSize();
    Scene* scene = Director::getInstance()->getRunningScene();
    Node* pageLayer = scene->getChildByTag(1);
    if (pageLayer != nullptr) {
        RenderTexture* renderTexture = RenderTexture::create(winSize.width, winSize.height, Texture2D::PixelFormat::RGBA8888);
        PageData pageData = BookController::getPageTypeFromCurrentPage(_iCurrentPage);
        renderTexture->begin();
        if (pageData.getGameType() == "draw") {
            for (int i = 0; i < pageLayer->getChildren().size();i++) {
                auto node = pageLayer->getChildren().at(i);
                if (node->getTag() != 1000) {
                    node->visit();
                }
            }
            auto drawLayer = pageLayer->getChildByTag(1000);
            Node* render = drawLayer->getChildByTag(1000);
            Node* canvesNode = drawLayer->getChildByTag(1001);
            Renderer *renderer = Director::getInstance()->getRenderer();
            Mat4 transformMatrix = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
            
            render->visit(renderer, transformMatrix, true);
            canvesNode->visit(renderer, transformMatrix, true);
            
        }else
        {
            pageLayer->visit();
        }
        renderTexture->end();
        return renderTexture;
    }
    return nullptr;
}

string BookParser::getVersion()
{
    return BOOK_PARSER_VERSION;
}

int BookParser::pageUp()
{
	AudioPlayer::getInstance()->stopAllEffect();

	if (_iCurrentPage > 1)
	{
		drawLayerSavePNGFile();
		_bookParser->setCurrentPage(--_iCurrentPage);
		doPageTurn(true, true);
		return _iCurrentPage;
	}
	
	return NO_PAGE_TO_TURN;
}

int BookParser::pageDown(bool isByHand)
{
	//cocos2d-x3.14.1修复了音频结束回调中stop语音崩溃的问题。
	AudioPlayer::getInstance()->stopAllEffect();
    if (_iCurrentPage < _bookData.getPages())
    {
        drawLayerSavePNGFile();
        _bookParser->setCurrentPage(++_iCurrentPage);
        doPageTurn(false, isByHand);
        return _iCurrentPage;
    }
    else if (_bookEndCallBack != nullptr)
    {
        _bookEndCallBack();
    }
    return NO_PAGE_TO_TURN;
}

void BookParser::bookQuit()
{
	_bookParser->setIsReading(false);
	AudioPlayer::getInstance()->stopAllEffect();
	AudioPlayer::getInstance()->uncacheAll();
	PreLoadResourcesController::pageQuitRemoveResources();
    drawLayerSavePNGFile();
	log("%s", Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
//#ifdef CC_PLATFORM_WIN32_DOT_NET
//	SpriteFrameCache::getInstance()->removeSpriteFrames();
//	Director::getInstance()->getTextureCache()->removeAllTextures();
//	Director::getInstance()->replaceScene(BookParser::getInstance()->getRootScene());
//#else
//	Director::getInstance()->replaceScene(TransitionPageTurn::create(1.0f, BookParser::getInstance()->getRootScene(), false));
//#endif
}

void BookParser::doPageTurn(bool backwards, bool isByHand)
{
	if (backwards)
	{
		PreLoadResourcesController::pageUpPreloadResources();
	}
	else
	{
		PreLoadResourcesController::pageDownPreloadResources();
	}
	if (isByHand)
	{
		AudioPlayer::getInstance()->playEffect("booknextpage.mp3", "yes");
	}
	Director::getInstance()->replaceScene(TransitionPageTurn::create(1.0f, PageLayer::createScene(), backwards));
}

void BookParser::drawLayerSavePNGFile()
{
	PageData pageData = BookController::getPageTypeFromCurrentPage(_iCurrentPage);
	if (pageData.getGameType() == "draw") {
		DrawLayer* drawLayer = (DrawLayer*)Director::getInstance()->getRunningScene()->getChildByTag(1)->getChildByTag(1000);

		string drawFilePath = _bookParser->getDrawFilePath() + toString(_iCurrentPage) + ".png";
		drawLayer->saveRenderTexture(drawFilePath);
	}
}


NS_FK_END

