#include "FKMovieData.h"

NS_FK_BEGIN

MovieData::MovieData()
{
}
MovieData::~MovieData()
{
}
void MovieData::clear()
{
    _sScalingMode.clear();
    _sMovieSourceType.clear();
    _sControlStyle.clear();
    _fPositionX = 0.0;
    _fPositionY = 0.0;
    _fWidth = 0.0;
    _fHeight = 0.0;
}
//*********set**********
void MovieData::setMovieId(string sMovieId)
{
    _sMovieId = sMovieId;
}
void MovieData::setFrame(string sPositionX, string sPositionY, string sWidth, string sHeight,float coordinateScale,float resourcesScale)
{
    _fPositionX = stringTo<float>(sPositionX)*coordinateScale;
    _fPositionY = stringTo<float>(sPositionY)*coordinateScale;
    _fWidth = stringTo<float>(sWidth)*resourcesScale;
    _fHeight = stringTo<float>(sHeight)*resourcesScale;
}
void MovieData::setScalingMode(string sScalingMode)
{
    _sScalingMode = sScalingMode;
}
void MovieData::setControlStyle(string sControlStyle)
{
    _sControlStyle = sControlStyle;
}
void MovieData::setMovieSourceType(string sMovieSourceType)
{
    _sMovieSourceType = sMovieSourceType;
}

//*********get**********
string MovieData::getMovieId()
{
    return _sMovieId;
}
Vec4 MovieData::getFrame()
{
    return Vec4(_fPositionX, _fPositionY, _fWidth, _fHeight);
}
string MovieData::getScalingMode()
{
    return _sScalingMode;
}
string MovieData::getControlStyle()
{
    return _sControlStyle;
}
string MovieData::getMovieSourceType()
{
    return _sMovieSourceType ;
}

NS_FK_END


