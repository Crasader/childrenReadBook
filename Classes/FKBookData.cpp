#include "FKBookData.h"

NS_FK_BEGIN

BookData::BookData()
{
    
}
BookData::~BookData()
{
    
}

void BookData::clear()
{
    _pageData.clear();
    _mPageData.clear();
    _sBookPath.clear();
    _iPages = 0;
    _iCurrentPage = 0;
    _fCoordinateScale = 0.0f;
    _fResourcesScale = 0.0f;
}

//Page页数相关操作
int BookData::getPages()
{
    return _iPages;
}
void BookData::setPages(int iPages)
{
    _iPages = iPages;
}
//BookName相关操作
string BookData::getBookPath()
{
    return _sBookPath;
}
void BookData::setBookPath(string sBookPath)
{
    _sBookPath = sBookPath;
}

void BookData::setResourcesScale(float resourcesScale)
{
    _fResourcesScale = resourcesScale;
}

float BookData::getResourcesScale()
{
    return _fResourcesScale;
}

void BookData::setCoordinateScale(float coordinateScale)
{
    _fCoordinateScale = coordinateScale;
}

float BookData::getCoordinateScale()
{
    return _fCoordinateScale;
}

void BookData::setWinSizeOffset(cocos2d::Vec2 winSizeOffset)
{
    _winSizeOffset = winSizeOffset;
}

Vec2 BookData::getWinSizeOffset()
{
    return _winSizeOffset;
}

//PageData相关操作
std::map<int, PageData> BookData::getPageData()
{
    return _mPageData;
}
void BookData::setPageData(int iCurrentPage, PageData &pageData)
{
    _iCurrentPage = iCurrentPage;
    _pageData = pageData;
    _mPageData[iCurrentPage] = pageData;
}

NS_FK_END
