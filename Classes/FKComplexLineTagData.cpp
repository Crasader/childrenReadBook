#include "FKComplexLineTagData.h"

NS_FK_BEGIN

ComplexLineTagData::ComplexLineTagData()
{
    
}
ComplexLineTagData::~ComplexLineTagData()
{
    
}
void ComplexLineTagData::clear()
{
    _iStartTag = 0;
    _iEndTag =0;
}

void ComplexLineTagData::setStartTag(string sStartTag)
{
    _iStartTag = stringTo<int>(sStartTag);
}
void ComplexLineTagData::setEndTag(string sEndTag)
{
    _iEndTag = stringTo<int>(sEndTag);
}



int ComplexLineTagData::getStartTag()
{
    return _iStartTag;
}
int ComplexLineTagData::getEndTag()
{
    return _iEndTag;
}

NS_FK_END
