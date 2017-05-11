#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "cocos2d.h"
#include "FKBookData.h"
#include "FKBookParser.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL GameController:public Ref
{
public:
    GameController();
    ~GameController();
    
    void                            setCurrentPage(int iCurrentPage);
    
    int                             getBookPages();
    
    BookData                        getBookData();
    
    PageData                        getPageData();
    
    string                          getPageType();
    
    string                          getGameType();
    
    vector<GameSpriteData>          getGameSpriteVector();
    
    //迷宫游戏路径信息
    vector<KeyPathData>             getKeyPathDataVector();
    //复杂连线游戏连线信息
    vector<ComplexLineData>         getComplexLineDataVector();
    //生成Sprtie*作为key，GameSpriteData作为value的map
    void                            setGameSpriteAndDataMap(Sprite *sprite, GameSpriteData &gsData);
    
    map<Sprite* ,GameSpriteData>    getGameSpriteAndDataMap();
    
    //粒子系统
    vector<ParticleSystemData>      getParticleSystemData();
    
    //获取背景音
    string                          getPlayBackgroundMusic();
    
    //获取animationGroupDataVector
    vector<AnimationGroupData>      getAnimationGroupDataVector();
private:
    BookData                         _bookData;
    PageData                         _pageData;
    Sprite                           _sprite;
    map<Sprite* ,GameSpriteData>     _mGameSpriteAndData;
};

NS_FK_END



#endif /* GameController_h */
