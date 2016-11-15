#ifndef LinePoint_h
#define LinePoint_h

#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL LinePoint
{
public:
    void setPointAndWidth(Point point, float width);
    Point getPoint();
    float getWidth();
    
    void clear();
    
private:
    Point                          _point;
    float                          _fWidth;
    
};

NS_FK_END

#endif /* LinePoint_h */
