#include "FKDiyRotateBy.h"

NS_CC_BEGIN

DiyRotateBy* DiyRotateBy::create(float duration, float deltaAngle,float radius,Point center)
{
    
    DiyRotateBy *DiyRotateBy = new (std::nothrow) class DiyRotateBy();
    DiyRotateBy->initWithDuration(duration, deltaAngle,radius,center);
    DiyRotateBy->autorelease();
    
    return DiyRotateBy;
}

DiyRotateBy::DiyRotateBy()
: _is3D(false)
{
}

bool DiyRotateBy::initWithDuration(float duration, float deltaAngle,float radius,Point center)
{
    _fDegrees = deltaAngle;
    _fRadius = radius;
    _center = center;
    if (ActionInterval::initWithDuration(duration))
    {
        _deltaAngle.x = _deltaAngle.y = deltaAngle;
        
        return true;
    }
    
    return false;
}

void DiyRotateBy::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    //获得偏角
    _startPosition = target->getPosition();
    getDriftAngle();
    if(_is3D)
    {
        _startAngle = target->getRotation3D();
    }
    else
    {
        _startAngle.x = target->getRotationSkewX();
        _startAngle.y = target->getRotationSkewY();
    }
}

void DiyRotateBy::update(float time)
{
    // FIXME: shall I add % 360
    if (_target)
    {
        if(_is3D)
        {
            Vec3 v;
            v.x = _startAngle.x + _deltaAngle.x * time;
            v.y = _startAngle.y + _deltaAngle.y * time;
            v.z = _startAngle.z + _deltaAngle.z * time;
            _target->setRotation3D(v);
        }
        else
        {
#if CC_USE_PHYSICS
            if (_startAngle.x == _startAngle.y && _deltaAngle.x == _deltaAngle.y)
            {
                _target->setRotation(_startAngle.x + _deltaAngle.x * time);
                float degrees = _deltaAngle.x * time;
                float fRadian = degrees * M_PI / 180.0f;
                Point pos = Point(_center.x+_fRadius*sinf(fRadian+_fDriftAngle),_center.y+_fRadius*cosf(fRadian+_fDriftAngle));;
                _target->setPosition(pos);
            }
            else
            {
                _target->setRotationSkewX(_startAngle.x + _deltaAngle.x * time);
                _target->setRotationSkewY(_startAngle.y + _deltaAngle.y * time);
            }
#else
            
            _target->setRotationSkewX(_startAngle.x + _deltaAngle.x * time);
            _target->setRotationSkewY(_startAngle.y + _deltaAngle.y * time);
#endif // CC_USE_PHYSICS
        }
    }
}
//得到偏角
void DiyRotateBy::getDriftAngle()
{
    //起始点位于第一象限
    if(_startPosition.x >= _center.x && _startPosition.y > _center.y)
    {
        _fDriftAngle = asinf((_startPosition.x-_center.x)/_fRadius);
    }
    //起始点位于第二象限
    else if(_startPosition.x > _center.x && _startPosition.y <= _center.y)
    {
        _fDriftAngle = acosf((_startPosition.x-_center.x)/_fRadius)+M_PI_2;
        
    }
    //起始点位于第三象限
    else if(_startPosition.x <= _center.x && _startPosition.y < _center.y)
    {
        _fDriftAngle = asinf((_center.x-_startPosition.x)/_fRadius)+ M_PI;
    }
    //起始点位于第四象限
    else if(_startPosition.x < _center.x && _startPosition.y >= _center.y)
    {
        _fDriftAngle = acosf((_center.x-_startPosition.x)/_fRadius)+ 1.5* M_PI;
        
    }
    
}

NS_CC_END

