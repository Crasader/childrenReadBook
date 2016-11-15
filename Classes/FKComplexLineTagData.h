#ifndef __COMPLEX_LINE_TAG_DATA_H__
#define __COMPLEX_LINE_TAG_DATA_H__

#include "cocos2d.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL ComplexLineTagData
{
public:
    ComplexLineTagData();
    ~ComplexLineTagData();
    void                         clear();
    
    void                         setStartTag(string sStartTag);
    void                         setEndTag(string sEndTag);
    
    
    int                          getStartTag();
    int                          getEndTag();
private:
    int                          _iStartTag;
    int                          _iEndTag;
    
    
    
};

NS_FK_END

#endif /* __COMPLEX_LINE_TAG_DATA_H__ */
