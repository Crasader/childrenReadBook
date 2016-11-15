#include "FKComplexLineData.h"

NS_FK_BEGIN

ComplexLineData::ComplexLineData()
{
    
}
ComplexLineData::~ComplexLineData()
{
    
}
void ComplexLineData::clear()
{
    _vComplexLineTagData.clear();
    _sLineMode.clear();
    _iLineGrouopId = 0;
}

/*..............set..............*/
void ComplexLineData::setLineGroupId(string sLineGroupId)
{
    _iLineGrouopId = stringTo<int>(sLineGroupId);
}
void ComplexLineData::setLineMode(string sLineMode)
{
    _sLineMode =sLineMode;
}
void ComplexLineData::setComplexLineTagData(ComplexLineTagData &cltData)
{
    _vComplexLineTagData.push_back(cltData);
}

/*..............get.........*/
int ComplexLineData::getLineGroupId()
{
    return _iLineGrouopId ;
}
string ComplexLineData::getLineMode()
{
    return _sLineMode;
}

vector<ComplexLineTagData> ComplexLineData::getComplexLineTagData()
{
    return _vComplexLineTagData;
}

NS_FK_END
