#include "FKPageData.h"

NS_FK_BEGIN

PageData::PageData()
{
    
}
PageData::~PageData()
{
    
}

void PageData::clear()
{
    _sSubtitleSoundId.clear();
    _sSoundId.clear();
    _sInterrupt.clear();
    _sPageType.clear();
    _sGameType.clear();
    _sIsOrder.clear();
    _vGameSpriteData.clear();
    _mSpriteData.clear();
    _vMovieData.clear();
    _vSubtitleSet.clear();
    _vAnimationGroupSet.clear();
    _vParticleSystemData.clear();
    _fRenderRectX = 0.0f;
    _fRenderRectY = 0.0f;
    _fRenderRectW = 0.0f;
    _fRenderRectH = 0.0f;
}

map<int, SpriteData> PageData::getSpriteData()
{
    return _mSpriteData;
}
vector<GameSpriteData> PageData::getGameSpriteData()
{
    return _vGameSpriteData;
}

string PageData::getSoundId()
{
    return _sSoundId;
}
string PageData::getInterrupt()
{
    return _sInterrupt;
}
string PageData::getPageType()
{
    return _sPageType;
}
string PageData::getGameType()
{
    return _sGameType;
}

vector<AnimationGroupData> PageData::getAnimationGroupSetData()
{
    vector<AnimationGroupData> vTmp;
    if (_vAnimationGroupSet.size() > 0) {
        vTmp = _vAnimationGroupSet[0].getAnimationGroupData();
    }
    else
    {
        log("animationGroupSet.size() error");
    }
    return vTmp;
}


void PageData::setSpriteData(SpriteData &spData)
{
    _mSpriteData[spData.getTag()] = spData;
}
void PageData::setGameSpriteData(GameSpriteData &gsData)
{
    _vGameSpriteData.push_back(gsData);
}
void PageData::setSoundId(string sSoundId)
{
    _sSoundId = sSoundId;
}
void PageData::setInterrupt(string sInterrupt)
{
    _sInterrupt = sInterrupt;
}

void PageData::setPageType(string sPageType)
{
    _sPageType = sPageType;
}
void PageData::setGameType(string sGameType)
{
    _sGameType = sGameType;
}

void PageData::setSubtitleSoundId(string sSubtitleSoundId)
{
    _sSubtitleSoundId = sSubtitleSoundId;
}
string PageData::getSubtitleSoundId()
{
    return _sSubtitleSoundId;
}

void PageData::setSubtitleSet(SubtitleData &subtitleData)
{
    _vSubtitleSet.push_back(subtitleData);
}

vector<SubtitleData> PageData::getSubtitleSet()
{
    return _vSubtitleSet;
}

void PageData::setAnimationGroupSet(AnimationGroupSetData &animationGroupSetData)
{
    _vAnimationGroupSet.push_back(animationGroupSetData);
}
void PageData::setAnimationGroupSetClear()
{
    _vAnimationGroupSet.clear();
}

//拼图/顺序连线，判断是否有序
string PageData::getIsOrder()
{
    return _sIsOrder;
}
void PageData::setIsOrder(string sIsOrder)
{
    _sIsOrder = sIsOrder;
}

//画板游戏
void PageData::setRenderRect(string sRenderRectX, string sRenderRectY, string sRenderRectW, string sRenderRectH, float coordinateScale,Vec2 winSizeOffset)
{
    _fRenderRectX = stringTo<float>(sRenderRectX)*coordinateScale - winSizeOffset.x;
    _fRenderRectY = stringTo<float>(sRenderRectY)*coordinateScale - winSizeOffset.y;
    _fRenderRectW = stringTo<float>(sRenderRectW)*coordinateScale;
    _fRenderRectH = stringTo<float>(sRenderRectH)*coordinateScale;
}
cocos2d::Rect PageData::getRenderRect()
{
    return Rect(_fRenderRectX-_fRenderRectW/2,_fRenderRectY-_fRenderRectH/2,_fRenderRectW,_fRenderRectH);
}
//迷宫游戏
void PageData::setKeyPathData(KeyPathData &kpData)
{
    _vKeyPathData.push_back(kpData);
}
vector<KeyPathData> PageData::getKeyPathData()
{
    return _vKeyPathData;
}
//复杂连线游戏
void PageData::setComplexLineData(ComplexLineData &clData)
{
    _vComplexLineData.push_back(clData);
}
vector<ComplexLineData> PageData::getComplexLineData()
{
    return _vComplexLineData;
}
//视频
void PageData::setMovieData(MovieData &mvData)
{
    _vMovieData.push_back(mvData);
}
vector<MovieData> PageData::getMovieData()
{
    return _vMovieData;
}
//粒子系统
void PageData::setParticleSystemData(ParticleSystemData &psData)
{
    _vParticleSystemData.push_back(psData);
}
vector<ParticleSystemData> PageData::getParticleSystemData()
{
    return _vParticleSystemData;
}

NS_FK_END






