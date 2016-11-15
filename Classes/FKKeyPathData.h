#ifndef __KEY_PATH_DATA_H__
#define __KEY_PATH_DATA_H__

#include "cocos2d.h"
#include "FKNormalPointData.h"
#include "FKAnimationData.h"
#include "FKKeyPointData.h"
#include "FKUtil.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace std;

class FK_DLL KeyPathData
{
public:
    KeyPathData();
    ~KeyPathData();
    void                     clear();
    
    void                     setStartPointData(KeyPointData &startPointData);
    void                     setEndPointData(KeyPointData &endPointData);
    void                     setNormalePointData(NormalPointData &normalPointData);
    
    KeyPointData             getStartPointData();
    KeyPointData             getEndPointData();
    vector<NormalPointData>  getNormalPointData();
    
private:
    KeyPointData             _startPointData;
    KeyPointData             _endPointData;
    vector<NormalPointData>  _vNormalPointData;
    
};

NS_FK_END

#endif /* KeyPathData_h */

