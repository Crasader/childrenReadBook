#include "FKKeyPathData.h"

NS_FK_BEGIN

KeyPathData::KeyPathData()
{
    
}
KeyPathData::~KeyPathData()
{
    
}
void KeyPathData::clear()
{    
    _vNormalPointData.clear();
    _endPointData.clear();
    _startPointData.clear();
}
void KeyPathData::setNormalePointData(NormalPointData &normalPointData)
{
    _vNormalPointData.push_back(normalPointData);
}
void KeyPathData::setStartPointData(KeyPointData &startPointData)
{
    _startPointData = startPointData;
}
void KeyPathData::setEndPointData(KeyPointData &endPointData)
{
    _endPointData = endPointData;
}
vector<NormalPointData> KeyPathData::getNormalPointData()
{
    return  _vNormalPointData;
}
KeyPointData KeyPathData::getStartPointData()
{
    return  _startPointData;
}
KeyPointData KeyPathData::getEndPointData()
{
    return  _endPointData;
}

NS_FK_END

