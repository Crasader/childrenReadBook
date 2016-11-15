#ifndef __MOVIE_DATA_H__
#define __MOVIE_DATA_H__

#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL MovieData
{
public:
    MovieData();
    ~MovieData();
    void          setMovieId(string sMovieId);
    void          setFrame(string sPositionX,string sPositionY,string sWidth,string sHeight,float coordinateScale,float resourcesScale);
    void          setScalingMode(string sScalingMode);
    void          setControlStyle(string sControlStyle);
    void          setMovieSourceType(string sMovieSourceType);
    
    string        getMovieId();
    Vec4          getFrame();
    string        getScalingMode();
    string        getControlStyle();
    string        getMovieSourceType();
    void          clear();
private:
    string        _sMovieId;
    float         _fPositionX;
    float         _fPositionY;
    float         _fWidth;
    float         _fHeight;
    string        _sScalingMode;
    string        _sControlStyle;
    string        _sMovieSourceType;
    
};

NS_FK_END

#endif
