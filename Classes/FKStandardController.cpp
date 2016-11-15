#include "FKStandardController.h"

NS_FK_BEGIN

StandardController::StandardController()
{
    
}
StandardController::~StandardController()
{
    
}

void StandardController::setCurrentPage(int iCurrentPage)
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

vector<AnimationGroupData> StandardController::getAnimationGroupDataVector()
{
    return _pageData.getAnimationGroupSetData();
}

int StandardController::getBookPages()
{
    return _bookData.getPages();
}

PageData StandardController::getPageData()
{
    return _pageData;
}


string StandardController::getPageType()
{
    return _pageData.getPageType();
}

map<int, SpriteData> StandardController::getSpriteMap()
{
    return _pageData.getSpriteData();
}

string StandardController::getPlayBackgroundMusic()
{
    return _pageData.getSoundId();
}


SpriteData StandardController::getSpriteDataFromSpriteTag(int iTag)
{
    SpriteData spData;
    map<int, SpriteData> mTmp = _pageData.getSpriteData();
    for (map<int, SpriteData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it) {
        if (iTag == it->first) {
            spData = it->second;
        }
    }
    return spData;
}
//获取视频信息
vector<MovieData> StandardController::getMovieData()
{
    return _pageData.getMovieData();
}
//获取粒子信息
vector<ParticleSystemData> StandardController::getParticleSystemData()
{
    return _pageData.getParticleSystemData();
}

NS_FK_END

