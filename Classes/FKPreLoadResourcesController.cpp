#include "FKPreLoadResourcesController.h"

NS_FK_BEGIN

#pragma mark - 首次进入预加载

void PreLoadResourcesController::firstEnterLayerPreloadResources()
{
    BookParser* bookParser = BookParser::getInstance();
    BookData bookData = bookParser->getBookData();
    int currentPage = bookParser->getCurrentPage();
    int pages = bookData.getPages();
    //加载两页
    if (currentPage == 1) {
        preloadOrRemoveResourcesFromStandardOrGame(currentPage,"preload");
        if (pages > 1) {
            preloadOrRemoveResourcesFromStandardOrGame(currentPage + 1, "preload");
        }
    }
}


#pragma mark - 翻页预加载
void PreLoadResourcesController::pageQuitRemoveResources()
{
    BookParser* bookParser = BookParser::getInstance();
    BookData bookData = bookParser->getBookData();
    int currentPage = bookParser->getCurrentPage();
    int pages = bookData.getPages();
    
    preloadOrRemoveResourcesFromStandardOrGame(currentPage, "remove");
    if (pages > 1) {
        if (currentPage == 1) {
            preloadOrRemoveResourcesFromStandardOrGame(currentPage + 1, "remove");
        }else if (currentPage == pages)
        {
            preloadOrRemoveResourcesFromStandardOrGame(currentPage - 1, "remove");
        }else
        {
            preloadOrRemoveResourcesFromStandardOrGame(currentPage - 1, "remove");
            preloadOrRemoveResourcesFromStandardOrGame(currentPage + 1, "remove");
        }
    }
}

void PreLoadResourcesController::pageDownPreloadResources()
{
    BookParser* bookParser = BookParser::getInstance();
    BookData bookData = bookParser->getBookData();
    int currentPage = bookParser->getCurrentPage();
    int pages = bookData.getPages();
    
    if (pages > 2) {
        if (currentPage < pages)
        {
            preloadOrRemoveResourcesFromStandardOrGame(currentPage + 1,"preload");
        }
        if (currentPage > 2 && currentPage <= pages)
        {
            preloadOrRemoveResourcesFromStandardOrGame(currentPage - 2, "remove");
        }
    }
}

void PreLoadResourcesController::pageUpPreloadResources()
{
    BookParser* bookParser = BookParser::getInstance();
    BookData bookData = bookParser->getBookData();
    int currentPage = bookParser->getCurrentPage();
    int pages = bookData.getPages();
    
    if (pages > 2) {
        if (currentPage < pages && currentPage > 1) {
            preloadOrRemoveResourcesFromStandardOrGame(currentPage - 1, "preload");
        }
        if (currentPage < pages - 1 && currentPage >= 1) {
            preloadOrRemoveResourcesFromStandardOrGame(currentPage + 2, "remove");
        }
    }
}


#pragma mark - 加载和移除
void PreLoadResourcesController::preloadOrRemoveResourcesFromStandardOrGame(int page , string state)
{
    BookParser* bookParser = BookParser::getInstance();
    string imageDirPath = bookParser->getBookPath() + "/image/";
    PageData pageData = BookController::getPageTypeFromCurrentPage(page);
    
    if (pageData.getPageType() == "standard")
    {
        SpriteData spriteData;
        map<int , SpriteData> mSpriteData = pageData.getSpriteData();
        for (map<int , SpriteData>::iterator it = mSpriteData.begin(); it != mSpriteData.end(); it++) {
            spriteData = it->second;
            if (state == "preload") {
                preloadResources(imageDirPath + spriteData.getImageId());
            }else if (state == "remove")
            {
                removeResources(imageDirPath + spriteData.getImageId());
            }
        }
    }
    else if (pageData.getPageType() == "game")
    {
        GameSpriteData gameSpriteData;
        vector<GameSpriteData> vGameSpriteData = pageData.getGameSpriteData();
        for (unsigned int i = 0 ; i < vGameSpriteData.size() ; i++) {
            gameSpriteData = vGameSpriteData[i];
            if (state == "preload") {
                preloadResources(imageDirPath + gameSpriteData.getImageId());
            }else if (state == "remove")
            {
                removeResources(imageDirPath + gameSpriteData.getImageId());
            }
        }
    }
}

void PreLoadResourcesController::preloadResources(string imageName)
{
    TextureCache* textureCache = Director::getInstance()->getTextureCache();
    textureCache->addImage(imageName);
    
    if (imageName.find("0000.png") != string::npos) {
        imageName.replace(imageName.find("0000.png"), 8, "ani.plist");
        auto cache = SpriteFrameCache::getInstance();
        cache->addSpriteFramesWithFile(imageName);
    }
}

void PreLoadResourcesController::removeResources(string imageName)
{
    TextureCache* textureCache = Director::getInstance()->getTextureCache();
    textureCache->removeTextureForKey(imageName);
    
    if (imageName.find("0000.png") != string::npos) {
        imageName.replace(imageName.find("0000.png"), 8, "ani.plist");
        auto cache = SpriteFrameCache::getInstance();
        cache->removeSpriteFramesFromFile(imageName);
        imageName.replace(imageName.find(".plist"), 6, ".png");
        textureCache->removeTextureForKey(imageName);
    }
}

NS_FK_END


