#ifndef DIY_ROTATE_By_h
#define DIY_ROTATE_By_h

#include "cocos2d.h"
#include "FKUtil.h"

NS_CC_BEGIN

class FK_DLL DiyRotateBy: public ActionInterval
{
public:
    
    static DiyRotateBy* create(float duration, float deltaAngle,float radius,Point center);
    
    virtual void update(float time) override;
    
    virtual void startWithTarget(Node *target) override;
CC_CONSTRUCTOR_ACCESS:
    DiyRotateBy();
    virtual ~DiyRotateBy() {}
    
    bool initWithDuration(float duration, float deltaAngle,float radius,Point center);
    //获得偏角
    void  getDriftAngle();
    
protected:
    bool              _is3D;
    Vec3              _deltaAngle;
    Vec3              _startAngle;
    Point             _center;
    Point             _startPosition;
    float             _fDegrees;
    float             _fRadius;
    float             _fDriftAngle;
private:
    CC_DISALLOW_COPY_AND_ASSIGN(DiyRotateBy);
};

NS_CC_END

#endif /* DIY_ROTATE_By_h */
