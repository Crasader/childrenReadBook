#include "FKBookController.h"

NS_FK_BEGIN

BookController::BookController()
{
    
}
BookController::~BookController()
{
    
}

PageData BookController::getPageTypeFromCurrentPage(int iCurrentPage)
{
    PageData pageData;
    pageData.clear();
    BookParser* bookParser = BookParser::getInstance();
    BookData bookData = bookParser->getBookData();
    map<int, PageData> mTmp = bookData.getPageData();
    for (map<int, PageData>::iterator it = mTmp.begin(); it != mTmp.end(); ++it) {
        if (iCurrentPage == it->first) {
            pageData = it->second;
            break;
        }
    }
    return pageData;
}

NS_FK_END
