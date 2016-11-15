#ifndef __COMPLEX_LINE_DATA_H__
#define __COMPLEX_LINE_DATA_H__

#include "cocos2d.h"
#include "FKComplexLineTagData.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL ComplexLineData
{
public:
    ComplexLineData();
    ~ComplexLineData();
    void                         clear();
    void                         setLineGroupId(string sLineGroupId);
    void                         setLineMode(string sLineMode);
    void                         setComplexLineTagData(ComplexLineTagData &cltData);
    
    
    
    vector<ComplexLineTagData>   getComplexLineTagData();
    int                          getLineGroupId();
    string                       getLineMode();
    
private:
    int                          _iLineGrouopId;
    string                       _sLineMode;
    vector<ComplexLineTagData>   _vComplexLineTagData;
    
    
    
};

NS_FK_END

#endif /* __COMPLEX_LINE_DATA_H__ */
