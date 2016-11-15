#include "FKIndexLayer.h"

NS_FK_BEGIN

#define BOOK_LABLE_WIDTH_OFFSET       50
#define BOOK_LABLE_HIGH_OFFSET        40
#define BOOK_ITEM_HIGH_OFFSET         80

Scene* IndexLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = IndexLayer::create();
    scene->addChild(layer);
    return scene;
}
// on "init" you need to initialize your instance
bool IndexLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    //获取屏幕尺寸
    Point screenSize = Director::getInstance()->getWinSize();
    _fIconScaleX = 0.8*screenSize.x/1024;
    _fIconScaleY= screenSize.y/768;
    //背景封面
    auto backGroundSp = Sprite::create("Default-Landscape~ipad.png");
    backGroundSp->setPosition(Vec2(screenSize.x/2, screenSize.y/2));
    backGroundSp->setScale(screenSize.x/1024,screenSize.y/768);
    this->addChild(backGroundSp,1);
    
    auto shelf1 = Sprite::create("shelf_ipad@2x.png");
    shelf1->setPosition(Vec2(screenSize.x/2,screenSize.y*0.7));
    this->addChild(shelf1,2);
    
    auto shelf2 = Sprite::create("shelf_ipad@2x.png");
    shelf2->setPosition(Vec2(screenSize.x/2,screenSize.y*0.4));
    this->addChild(shelf2,2);
    
    auto shelf3 = Sprite::create("shelf_ipad@2x.png");
    shelf3->setPosition(Vec2(screenSize.x/2,screenSize.y*0.1));
    this->addChild(shelf3,2);
    
    //添加书籍
    _vbookInfo.push_back("test");
    _vbookInfo.push_back("cyj");
	_vbookInfo.push_back("bld");
	_vbookInfo.push_back("cf");
    
    int lable_x1, lable_x2, lable_x3;
    lable_x3 = lable_x2 = lable_x1 = screenSize.x/5 - 80;
    int item_x1, item_x2, item_x3;
    item_x3 = item_x2 = item_x1 = lable_x1 - screenSize.x/2;
    for (unsigned int i = 0; i < _vbookInfo.size(); i++)
    {
        if (i<=3)
        {
            int lable_y1 = screenSize.y * 0.7 + BOOK_LABLE_HIGH_OFFSET * _fIconScaleY;
            int item_y1 = lable_y1 - screenSize.y/2 + BOOK_ITEM_HIGH_OFFSET * _fIconScaleY;
            auto label = Label::createWithTTF(_vbookInfo[i], "fonts/Marker Felt.ttf", 20);
            label->setPosition(Vec2(lable_x1,lable_y1));
            label->setScale(_fIconScaleX,_fIconScaleY);
            this->addChild(label, 4);
            auto bookItem = MenuItemImage::create("book_ipad_128.png",
                                                  "book_ipad_128_selected.png",
                                                  CC_CALLBACK_1(IndexLayer::onClickBook,this));
            bookItem->setTag(i+1);
            bookItem->setPosition(Vec2(item_x1,item_y1));
            bookItem->setScale(_fIconScaleX,_fIconScaleY);
            auto menu = Menu::create(bookItem,NULL);
            this->addChild(menu,5);
            lable_x1 = lable_x1 + screenSize.x/5 + BOOK_LABLE_WIDTH_OFFSET;
            item_x1 = lable_x1-screenSize.x/2;
        }
        //第二行
        else if (i >= 4 && i < 8)
        {
            int lable_y2 = screenSize.y * 0.4 + BOOK_LABLE_HIGH_OFFSET * _fIconScaleY;
            int item_y2 = lable_y2 - screenSize.y/2 + BOOK_ITEM_HIGH_OFFSET * _fIconScaleY;
            auto label = Label::createWithTTF(_vbookInfo[i], "fonts/Marker Felt.ttf", 20);
            label->setPosition(Vec2(lable_x2,lable_y2));
            label->setScale(_fIconScaleX,_fIconScaleY);
            this->addChild(label, 4);
            
            auto bookItem = MenuItemImage::create("book_ipad_128.png",
                                                  "book_ipad_128_selected.png",
                                                  CC_CALLBACK_1(IndexLayer::onClickBook, this));
            bookItem->setTag(i+1);
            bookItem->setPosition(Vec2(item_x2,item_y2));
            bookItem->setScale(_fIconScaleX,_fIconScaleY);
            auto menu = Menu::create(bookItem,NULL);
            this->addChild(menu,5);
            lable_x2 = lable_x2 + screenSize.x/5 + BOOK_LABLE_WIDTH_OFFSET;
            item_x2 = lable_x2 - screenSize.x/2;
            
        }
        //第三行
        else if (i >= 8 && i < 12)
        {
            int lable_y3 = screenSize.y * 0.1 + BOOK_LABLE_HIGH_OFFSET * _fIconScaleY;
            int item_y3 = lable_y3 - screenSize.y/2 + BOOK_ITEM_HIGH_OFFSET * _fIconScaleY;
            auto label = Label::createWithTTF(_vbookInfo[i], "fonts/Marker Felt.ttf", 20);
            label->setPosition(Vec2(lable_x3,lable_y3));
            label->setScale(_fIconScaleX,_fIconScaleY);
            this->addChild(label, 4);
            
            auto bookItem = MenuItemImage::create("book_ipad_128.png",
                                                  "book_ipad_128_selected.png",
                                                  CC_CALLBACK_1(IndexLayer::onClickBook, this));
            bookItem->setTag(i+1);
            bookItem->setPosition(Vec2(item_x3,item_y3));
            bookItem->setScale(_fIconScaleX,_fIconScaleY);
            auto menu = Menu::create(bookItem,NULL);
            this->addChild(menu,5);
            lable_x3 = lable_x3 + screenSize.x/5 + BOOK_LABLE_WIDTH_OFFSET;
            item_x3 = lable_x3 - screenSize.x/2;
        }
    }
    return true;
}

void IndexLayer::onClickBook(Ref* pSender)
{
    int iTag;
    auto menuItem = (MenuItemImage*)pSender;
    iTag = menuItem->getTag();
    
    std::string bookPath = "book_res/" + _vbookInfo[iTag-1];
    BookParser* bookParser = BookParser::getInstance();
    //获取版本号
    log("Version : %s",bookParser->getVersion().c_str());
    //设置画板保存路径
	bookParser->setDrawFilePath("");
    //设置阅读模式，默认为read
	bookParser->setBookPlayModeState(PlayModeState::READ);

	//书本播放结束回调
	bookParser->setBookEndCallBack([](const std::string &str){
		log("book end: %s",str.c_str());
	});
    //分享按钮接口调用
    bookParser->setBookShareCallBack([](RenderTexture *renderTexture){
        Size winSize = Director::getInstance()->getWinSize();
        Scene* scene = Director::getInstance()->getRunningScene();
        Node* node = scene->getChildByTag(1);
        renderTexture->setPosition(winSize.width/2,winSize.height/2);
        node->addChild(renderTexture,10000);
    });
    //翻页按钮接口调用
    //bookParser->setPageUpCallBack([]() {
    //    log("page up");
    //});
    //bookParser->setPageDownCallBack([]() {
    //    log("page Down");
    //});
    ////返回按钮接口调用
    //bookParser->setPageQuitCallBack([]() {
    //    log("page quit");
    //});
    //书籍解析主函数
    int code = bookParser->bookJsonParser(bookPath, 640.0/750.0, 640.0/640.0, "ios");
    log("error code ==== %d",code);
    //获取书籍中按钮
    Menu *menu = bookParser->getPageMenu();
    auto pageUp = menu->getChildByTag(BUTTON_TAG_PAGE_UP);
    auto pageDown = menu->getChildByTag(BUTTON_TAG_PAGE_DOWN);
    auto pageQuit = menu->getChildByTag(BUTTON_TAG_PAGE_QUIT);
    auto pageShare = menu->getChildByTag(BUTTON_TAG_PAGE_SHARE);
    pageUp->setOpacity(255);
    pageDown->setOpacity(255);
    pageQuit->setOpacity(255);
    pageShare->setOpacity(255);
}


NS_FK_END

