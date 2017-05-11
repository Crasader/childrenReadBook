#include "FKSpriteController.h"

NS_FK_BEGIN

SpriteController::SpriteController()
{
    
}
SpriteController::~SpriteController()
{
    
}

Sprite* SpriteController::getSpriteFromSpriteData(SpriteData spData,string sImageDic)
{
    string sImagePath = sImageDic + spData.getImageId();
    Sprite* spTmp = nullptr;
    //判断是否文件存在
    if (FileUtils::getInstance()->isFileExist(sImagePath)) {
        TextureCache* textureCache = Director::getInstance()->getTextureCache();
        Texture2D* texture = textureCache->getTextureForKey(sImagePath);
        if (texture) {
            spTmp = Sprite::createWithTexture(texture);
        }else
        {
            spTmp = Sprite::create(sImagePath);
        }
        spTmp->setPosition(spData.getPosition());
        spTmp->setTag(spData.getTag());
        spTmp->setAnchorPoint(spData.getAnchorPoint());
        spTmp->setLocalZOrder(spData.getZOrder());
        spTmp->setOpacity(spData.getOpacity());
        spTmp->setRotation(spData.getRotation());
        spTmp->setScale(spData.getScale());
    }
    
    
    return spTmp;
}

NS_FK_END



