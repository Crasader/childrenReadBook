#include "FKGameController.h"

NS_FK_BEGIN

GameController::GameController()
{
    
}
GameController::~GameController()
{
    
}
GameController* GameController::create()
{
	GameController* gameControl = new GameController();
	if (gameControl)
	{
		gameControl->autorelease();
		return gameControl;
	}
	CC_SAFE_DELETE(gameControl);
	return nullptr;
}

void GameController::setCurrentPage(int iCurrentPage)
{
    BookParser* bookParser = BookParser::getInstance();
    _bookData = bookParser->getBookData();
    map<int, PageData> mTmp = _bookData.getPageData();
    
    for (map<int, PageData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it) {
        if (iCurrentPage == it->first) {
            _pageData = it->second;
            
            break;
        }
    }
}
vector<AnimationGroupData> GameController::getAnimationGroupDataVector()
{
    return _pageData.getAnimationGroupSetData();
}

int GameController::getBookPages()
{
    return _bookData.getPages();
}

BookData GameController::getBookData()
{
    return _bookData;
}
PageData GameController::getPageData()
{
    return _pageData;
}


string GameController::getPageType()
{
    return _pageData.getPageType();
}

string GameController::getGameType()
{
    return _pageData.getGameType();
}

vector<GameSpriteData> GameController::getGameSpriteVector()
{
    return _pageData.getGameSpriteData();
}

void GameController::setGameSpriteAndDataMap(Sprite* sprite, GameSpriteData &gsData)
{
    _mGameSpriteAndData[sprite] = gsData;
}

map<Sprite* ,GameSpriteData> GameController::getGameSpriteAndDataMap()
{
    return _mGameSpriteAndData;
}

string GameController::getPlayBackgroundMusic()
{
    return _pageData.getSoundId();
}

//迷宫游戏
vector<KeyPathData> GameController::getKeyPathDataVector()
{
    return _pageData.getKeyPathData();
}
//复杂连线游戏
vector<ComplexLineData> GameController::getComplexLineDataVector()
{
    return _pageData.getComplexLineData();
}
//获取粒子信息
vector<ParticleSystemData> GameController::getParticleSystemData()
{
    return _pageData.getParticleSystemData();
}

NS_FK_END

