#ifndef __SPRITE_CONTROLLER_H__
#define __SPRITE_CONTROLLER_H__

#include "FKBookParser.h"
#include "cocos2d.h"
#include "FKSpriteData.h"
#include "FKBookData.h"

NS_FK_BEGIN

using namespace std;

class FK_DLL SpriteController
{
public:
    
    SpriteController();
    ~SpriteController();
    
    /**
     *  通过spData生成sprite对象
     *
     *  @param spData spriteData
     *
     *  @return Sprite*
     */
    Sprite* getSpriteFromSpriteData(SpriteData spData ,string sImageDic);
    
    
private:
    BookData           _bookData;
    Sprite             _sprite;
};

NS_FK_END
USING_NS_CC;

#endif // __SPRITE_CONTROLLER_H__
