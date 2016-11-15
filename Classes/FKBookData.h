#ifndef __BOOK_DATA_H__
#define __BOOK_DATA_H__

#include "cocos2d.h"
#include "FKPageData.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL BookData
{
public:
    
    BookData();
    ~BookData();
    
    
    int                         getPages();
    void                        setPages(int iPages);
    string                      getBookPath();
    void                        setBookPath(string sBookPath);
    
    map<int, PageData>          getPageData();
    void                        setPageData(int iCurrentPage, PageData &pageData);
    //素材系数比
    void                        setResourcesScale(float resourcesScale);
    float                       getResourcesScale();
    //坐标系数比
    void                        setCoordinateScale(float coordinateScale);
    float                       getCoordinateScale();
    //窗口偏移量
    void                        setWinSizeOffset(Vec2 winSizeOffset);
    Vec2                        getWinSizeOffset();
    
    void                        clear();
private:
    //页数
    int                         _iPages;
    //当前页码
    int                         _iCurrentPage;
    //书名
    string                      _sBookPath;
    
    PageData                    _pageData;
    //书页Map
    map<int, PageData>          _mPageData;
    
    float                       _fResourcesScale;
    float                       _fCoordinateScale;
    Vec2                        _winSizeOffset;
};

NS_FK_END


#endif // __BOOK_DATA_H__
