#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "json/document.h"
#include "FKBookData.h"
#include "FKPageData.h"
#include "FKKeyPathData.h"
#include "FKMovieData.h"
#include "FKParticleSystemData.h"
#include "FKComplexLineData.h"
#include "FKAnimationData.h"
#include "FKAnimationGroupData.h"
#include "FKAnimationGroupMemberData.h"
#include "FKAnimationGroupSetData.h"
#include "FKAnimationComboData.h"
#include "FKSubtitleData.h"

NS_FK_BEGIN

USING_NS_CC;


class FK_DLL JsonParser
{
public:
    
    JsonParser();
    ~JsonParser();
    
    int parserBook(string sBookPath,float resourcesScale,float coordinateScale,Vec2 winSizeOffset);
protected:
    
    void                        dealloc();

    int                         setPageDataToBookData(const rapidjson::Value &book);

/*------------------------------------ 页面 ------------------------------------*/
    /*------------------ subtitle ---------------------*/
    int                         setSubtitleDataToPageData(const rapidjson::Value &subtitleSet);
    int                         setJsonToSubtitleData(const rapidjson::Value &subtitle);
    
    /*------------------ standard ---------------------*/
    int                         setStandardPageDataToBookData(int i, const rapidjson::Value &pageDic);
    int                         setSpriteDataToStandard(const rapidjson::Value &spriteSet);
    int                         setJsonToSpriteData(const rapidjson::Value &sprite);
    
    /*------------------ game -------------------------*/
    int                         setGamePageDataToBookData(int i, const rapidjson::Value &pageDic);
    int                         setGameDataToGame(const rapidjson::Value &gameSet);
    int                         setJsonToGameSpriteData(const rapidjson::Value &sprite);
    //迷宫
    int                         setKeyPathDataToGame(const rapidjson::Value &keyPath);
    int                         setKeyPathAnimationGroupData(const rapidjson::Value &animationGroup);
    int                         setNormalPointToKeyPathData(const rapidjson::Value &normalPoint);
    //复杂连线
    int                         setLineToComplexLineData(const rapidjson::Value &line);
    int                         setLineTagToComplexLineData(const rapidjson::Value &lineTag);
    
/*------------------------------------ 动画组 ------------------------------------*/
    int                         setAnimationGroupDataToPageData(const rapidjson::Value &animationGroupSet);
    
    int                         setAnimationGroupMemberDataToAnimationGroupData(const rapidjson::Value &animationGroup);
    
    int                         setJsonToAnimationGroupMemberData(const rapidjson::Value &animationGroupMember);

    int                         setAnimationComboSetDataToAnimationGroupMemberData(const rapidjson::Value &animationComboSet, const rapidjson::Value &event);
    
    int                         setAnimationSetDataToAnimationGroupMemberData(const rapidjson::Value &animationSet, const rapidjson::Value &event);
    
    int                         setJsonToAnimationData(const rapidjson::Value &animationData, string event);

    
/*------------------------------------ 视频 ------------------------------------*/
    int                         setMovieDataToPageData(const rapidjson::Value &movieSet);
    int                         setJsonToMovieData(const rapidjson::Value &movie);
    
/*------------------------------------ 粒子 ------------------------------------*/
    int                         setParticleSystemDataToPageData(const rapidjson::Value &particleSet);
    int                         setJsonToParticleSystemData(const rapidjson::Value &particle);
    
private:
    string                                  _Event;
    BookData                                _bookData;
    PageData                                _pageData;
    SubtitleData                            _subtitleData;
    MovieData                               _movieData;
    SpriteData                              _spriteData;
    KeyPathData                             _keyPathData;
    GameSpriteData                          _gameSpriteData;
    ParticleSystemData                      _particleSystemData;
    
    
    AnimationComboData                      _animationComboData;
    AnimationData                           _animationData;
    AnimationGroupData                      _animationGroupData;
    AnimationGroupMemberDataKey             _animationGroupMemberDataKey;
    AnimationGroupMemberDataValue           _animationGroupMemberDataValue;
    vector<AnimationGroupMemberDataValue>   _vAnimationGroupMemberDataValue;
    AnimationGroupSetData                   _animationGroupSetData;
    NormalPointData                         _normalPointData;
    KeyPointData                            _startPointData;
    KeyPointData                            _endPointData;
    ComplexLineData                         _complexLineData;
    ComplexLineTagData                      _complexLineTagData;
};


NS_FK_END

#endif // __JSON_PARSER_H__
