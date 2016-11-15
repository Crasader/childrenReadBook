#include "FKParticleSystemData.h"

NS_FK_BEGIN

ParticleSystemData::ParticleSystemData()
{
}
ParticleSystemData::~ParticleSystemData()
{
}

void ParticleSystemData::clear()
{
    _sImageId.clear();
    _sParticleStyle.clear();
    _sCategory.clear();
    _iZorder = 0;
    _fPositionX = 0.0;
    _fPositionY = 0.0;
}
/****************SET************/
void ParticleSystemData::setCategory(string sCategory)
{
    _sCategory = sCategory;
}
void ParticleSystemData::setParticleStyle(string sParticleStyle)
{
    _sParticleStyle = sParticleStyle;
}
void ParticleSystemData::setImageId(string sImageId)
{
    _sImageId = sImageId;
}
void ParticleSystemData::setPosition(string sPositionX, string sPositionY,float coordinateScale)
{
    _fPositionX = stringTo<float>(sPositionX)*coordinateScale;
    _fPositionY = stringTo<float>(sPositionY)*coordinateScale;
}
void ParticleSystemData::setZorder(string sZorder)
{
    _iZorder = stringTo<float>(sZorder);
}


/****************GET************/
string ParticleSystemData::getCategory()
{
    return _sCategory;
}
string ParticleSystemData::getParticleStyle()
{
    return _sParticleStyle;
}
string ParticleSystemData::getImageId()
{
    return _sImageId;
}
Vec2 ParticleSystemData::getPosition()
{
    return Vec2(_fPositionX, _fPositionY);
}
int ParticleSystemData::getZorder()
{
    return _iZorder;
}

NS_FK_END

