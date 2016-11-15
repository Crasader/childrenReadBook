#include "FKLinePoint.h"

NS_FK_BEGIN

void LinePoint::clear()
{
    _point = Vec2();
    _fWidth = 0.0f;
}

void LinePoint::setPointAndWidth(Point point, float width)
{
    _point = point;
    _fWidth = width;
}

Point LinePoint::getPoint()
{
    return _point;
}

float LinePoint::getWidth()
{
    return _fWidth;
}

NS_FK_END
