#ifndef __PAGE_DATA_H__
#define __PAGE_DATA_H__

#include "cocos2d.h"
#include "FKSpriteData.h"
#include "FKGameSpriteData.h"
#include "FKKeyPathData.h"
#include "FKComplexLineData.h"
#include "FKMovieData.h"
#include "FKParticleSystemData.h"
#include "FKAnimationGroupSetData.h"
#include "FKSubtitleData.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL PageData
{
public:
    
    PageData();
    ~PageData();
    
    string                      getSoundId();
    string                      getInterrupt();
    string                      getPageType();
    
    void                        setSoundId(string sSoundId);
    void                        setInterrupt(string sInterrupt);
    void                        setPageType(string sPageType);
    
    void                        setSubtitleSoundId(string sSubtitleSoundId);
    string                      getSubtitleSoundId();
    //字幕模块
    vector<SubtitleData>        getSubtitleSet();
    void                        setSubtitleSet(SubtitleData &subtitleData);
    
    vector<AnimationGroupData>  getAnimationGroupSetData();
    void                        setAnimationGroupSet(AnimationGroupSetData &animationGroupSetData);
    void                        setAnimationGroupSetClear();
    
    /*--------------------standard------------------------*/
    map<int, SpriteData>        getSpriteData();
    void                        setSpriteData(SpriteData &spData);
    
    /*--------------------game------------------------*/
    string                      getGameType();
    void                        setGameType(string sGameType);
    void                        setGameSpriteData(GameSpriteData &gsData);
    vector<GameSpriteData>      getGameSpriteData();
    
    //拼图/顺序连线，判断是否有序
    string                      getIsOrder();
    void                        setIsOrder(string sIsOrder);
    //复杂连线游戏
    void                        setComplexLineData(ComplexLineData &clData);
    vector<ComplexLineData>     getComplexLineData();
    
    //画板
    void                        setRenderRect(string sRenderRectX,string sRenderRectY,string
                                              sRenderRectW,string sRenderRectH,float coordinateScale,Vec2 winSizeOffset);
    cocos2d::Rect               getRenderRect();
    
    //迷宫游戏
    void                        setKeyPathData(KeyPathData &kpData);
    vector<KeyPathData>         getKeyPathData();
    
    //视频
    void                        setMovieData(MovieData &mvData);
    vector<MovieData>           getMovieData();
    //粒子系统
    void                        setParticleSystemData(ParticleSystemData &psData);
    vector<ParticleSystemData>  getParticleSystemData();
    
    void                        clear();
private:
    //字幕音频
    string                          _sSubtitleSoundId;
    //背景音频
    string                          _sSoundId;
    //背景音是否能够被打断
    string                          _sInterrupt;
    //页面类型，standard or game
    string                          _sPageType;
    //游戏类型，目前puzzle 、findit、filling、line、draw、orderline
    string                          _sGameType;
    //拼图游戏扩展，判断是否是顺序
    string                          _sIsOrder;
    //存放GameSpriteData
    vector<GameSpriteData>          _vGameSpriteData;
    //将所有sprite通过tag进行标记
    map<int, SpriteData>            _mSpriteData;
    //动画组模块，包含一个或者多个动画组
    vector<AnimationGroupSetData>   _vAnimationGroupSet;
    //字幕模块
    vector<SubtitleData>             _vSubtitleSet;
    //视频模块
    vector<MovieData>               _vMovieData;
    //粒子系统
    vector<ParticleSystemData>      _vParticleSystemData;
    //画板游戏：
    //画板区域
    float                           _fRenderRectX;
    float                           _fRenderRectY;
    float                           _fRenderRectW;
    float                           _fRenderRectH;
    //迷宫游戏存放路径信息
    vector<KeyPathData>             _vKeyPathData;
    //复杂连线游戏连线信息
    vector<ComplexLineData>         _vComplexLineData;
    
};

NS_FK_END


#endif // __PAGE_DATA_H__
