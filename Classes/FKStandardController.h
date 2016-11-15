#ifndef __STANDARD_CONTROLLER_H__
#define __STANDARD_CONTROLLER_H__

#include "cocos2d.h"
#include "FKPageData.h"
#include "FKBookParser.h"
NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL StandardController
{
public:
    
    StandardController();
    ~StandardController();
    
    void                        setCurrentPage(int iCurrentPage);
    
    int                         getBookPages();
    
    PageData                    getPageData();
    
    //获取粒子信息
    vector<ParticleSystemData>  getParticleSystemData();
    
    string                      getPageType();
    
    map<int, SpriteData>        getSpriteMap();
    
    /**
     *  通过Tag获取对应sprite的data
     *
     *  @param iTag spriteTag
     *
     *  @return spriteData
     */
    SpriteData                  getSpriteDataFromSpriteTag(int iTag);
    
    //获取背景音
    string                      getPlayBackgroundMusic();
    
    //获取animationGroupDataVector
    vector<AnimationGroupData>  getAnimationGroupDataVector();
    
    //获取视频
    vector<MovieData>           getMovieData();
    
private:
    BookData                _bookData;
    PageData                _pageData;
};

NS_FK_END

#endif // __STANDARD_CONTROLLER_H__
