#ifndef __BOOK_CONTROLLER_H__
#define __BOOK_CONTROLLER_H__

#include "cocos2d.h"
#include "FKBookData.h"
#include "FKBookParser.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL BookController
{
public:
    
    BookController();
    ~BookController();
    
    //通过currentPage获取pageType
    static PageData           getPageTypeFromCurrentPage(int iCurrentPage);
    
private:
    
};

NS_FK_END

#endif // __BOOK_CONTROLLER_H__
