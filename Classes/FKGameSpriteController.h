#ifndef __GAME_SPRITE_CONTROLLER_H__
#define __GAME_SPRITE_CONTROLLER_H__

#include "FKBookParser.h"
#include "cocos2d.h"
#include "FKGameSpriteData.h"
#include "FKBookData.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL GameSpriteController
{
public:
    
    GameSpriteController();
    ~GameSpriteController();
    
    Sprite* getSpriteFromGameSpriteData(GameSpriteData gsData ,string sImageDic);
    
private:
    BookData           _bookData;
};

NS_FK_END


#endif  /* GameSpriteController_h */
