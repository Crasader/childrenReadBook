#include "FKGameSpriteController.h"

NS_FK_BEGIN

GameSpriteController::GameSpriteController()
{
    
}
GameSpriteController::~GameSpriteController()
{
    
}

Sprite* GameSpriteController::getSpriteFromGameSpriteData(GameSpriteData gsData ,string sImageDic)
{
    string sImagePath = sImageDic + gsData.getImageId();
    Sprite* spTmp = nullptr;
    //判断文件是否存在
    if (FileUtils::getInstance()->isFileExist(sImagePath)) {
        TextureCache* textureCache = Director::getInstance()->getTextureCache();
        Texture2D* texture = textureCache->getTextureForKey(sImagePath);
        if (texture) {
            spTmp = Sprite::createWithTexture(texture);
        }else
        {
            spTmp = Sprite::create(FileUtils::getInstance()->fullPathForFilename(sImagePath));
        }
        spTmp->setPosition(gsData.getPosition());
        spTmp->setTag(gsData.getTag());
        spTmp->setAnchorPoint(gsData.getAnchorPoint());
        spTmp->setLocalZOrder(gsData.getZOrder());
        spTmp->setOpacity(gsData.getOpacity());
        spTmp->setRotation(gsData.getRotation());
        float startScale = gsData.getStartScale();
        //拼图游戏额外初始化
        if (startScale!=0)
        {
            spTmp->setScale(startScale);
        }
        else
        {
            spTmp->setScale(gsData.getScale());
        }
    }
    return spTmp;
}

NS_FK_END

