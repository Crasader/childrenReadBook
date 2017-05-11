#include "FKParticleSystemController.h"

NS_FK_BEGIN

void ParticleSystemController::addParticleSystemForLayer(vector<ParticleSystemData> vParticleSystemData, string triggerMode, cocos2d::Layer *layer, string sImageDic, Vec2 touchPosition)
{
    ParticleSystem *particle = NULL;
    if(!vParticleSystemData.empty())
    {
        for(unsigned long i = 0; i < vParticleSystemData.size(); i++)
        {
            string sParticleStyle = vParticleSystemData[i].getParticleStyle();
            string sCategory = vParticleSystemData[i].getCategory();
            int iZorder = vParticleSystemData[i].getZorder();
            if(sCategory == "auto" && triggerMode == "auto")
            {
                particle = getPaticleSystem(vParticleSystemData[i], sImageDic);
                if (sParticleStyle == "rain") {
                    particle->setLife(10);
                    particle->setStartSize(10.0f);
                    particle->setStartSizeVar(5.0f);
                }
                layer->addChild(particle,iZorder);
            }
            else if(sCategory == "touch" && triggerMode == "touch")
            {
                particle = getPaticleSystem(vParticleSystemData[i], sImageDic);
                particle->setPosition(touchPosition);
                particle->setDuration(2);
                layer->addChild(particle,iZorder);
            }
        }
        
    }
}

ParticleSystem* ParticleSystemController::getPaticleSystem(ParticleSystemData particleData, string sImageDic)
{
    string particleImagePath = sImageDic + "/image/" + particleData.getImageId();
    ParticleSystem* particle = NULL;
    if(particleData.getParticleStyle() == "snow")
    {
        particle = ParticleSnow::create();
    }
    else if(particleData.getParticleStyle() == "fire")
    {
        particle = ParticleFire::create();
    }
    else if(particleData.getParticleStyle() == "firework")
    {
        particle = ParticleFireworks::create();
    }
    else if(particleData.getParticleStyle() == "sun")
    {
        particle = ParticleSun::create();
    }
    else if(particleData.getParticleStyle() == "galaxy")
    {
        particle = ParticleGalaxy::create();
    }
    else if(particleData.getParticleStyle() == "flower")
    {
        particle = ParticleFlower::create();
    }
    else if(particleData.getParticleStyle() == "meteor")
    {
        particle = ParticleMeteor::create();
    }
    else if(particleData.getParticleStyle() == "spiral")
    {
        particle = ParticleSpiral::create();
    }
    else if(particleData.getParticleStyle() == "explosion")
    {
        particle = ParticleExplosion::create();
    }
    else if(particleData.getParticleStyle() == "smoke")
    {
        particle = ParticleSmoke::create();
    }
    else if(particleData.getParticleStyle() == "rain")
    {
        particle = ParticleRain::create();
    }
    particle->setPosition(particleData.getPosition());
    particle->setTexture(Director::getInstance()->getTextureCache()->addImage(particleImagePath));
    return particle;
}

NS_FK_END
