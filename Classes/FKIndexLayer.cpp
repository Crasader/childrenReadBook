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
    
    addBookInfo();
    
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
            auto label = Label::createWithTTF(_vbookInfo[i], "fonts/FZHLJW.TTF", 20);
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
            auto label = Label::createWithTTF(_vbookInfo[i], "fonts/FZHLJW.TTF", 20);
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
            auto label = Label::createWithTTF(_vbookInfo[i], "fonts/FZHLJW.TTF", 20);
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

void IndexLayer::addBookInfo()
{
    _vbookInfo.push_back("test");
    _vbookInfo.push_back("ramtest");
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
	bookParser->setBookEndCallBack([](){
		log("book end.");
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
    bookParser->setPageUpCallBack([]() {
        BookParser::getInstance()->pageUp();
        log("page up");
    });
    bookParser->setPageDownCallBack([]() {
        BookParser::getInstance()->pageDown(true);
        log("page Down");
    });
    //返回按钮接口调用
    bookParser->setPageQuitCallBack([]() {
        BookParser::getInstance()->bookQuit();
        log("page quit");
    });
    //修改书籍按钮接口
    bookParser->setPageMenuChangeCallBack([](Menu* menu){
    #ifdef CC_PLATFORM_WIN32_DOT_NET
        Size winSize = Director::getInstance()->getWinSize();
        float highScale = winSize.height/1536;
        
        auto pageUp = (MenuItemImage *)menu->getChildByTag(BUTTON_TAG_PAGE_UP);
        auto pageDown = (MenuItemImage *)menu->getChildByTag(BUTTON_TAG_PAGE_DOWN);
        auto pageQuit = (MenuItemImage *)menu->getChildByTag(BUTTON_TAG_PAGE_QUIT);
        auto pageShare = (MenuItemImage *)menu->getChildByTag(BUTTON_TAG_PAGE_SHARE);
        
        pageUp->setNormalImage(Sprite::create("page_up_normal.png"));
        pageUp->setSelectedImage(Sprite::create("page_up_selected.png"));
        pageUp->setPosition(-winSize.width / 2 + pageUp->getContentSize().width / 6, 0);
        pageUp->setScale(highScale*1.5);
        pageUp->setAnchorPoint(Vec2(0.0f, 0.5f));
        
        pageDown->setNormalImage(Sprite::create("page_down_normal.png"));
        pageDown->setSelectedImage(Sprite::create("page_down_selected.png"));
        pageDown->setPosition(winSize.width / 2 - pageDown->getContentSize().width / 6, 0);
        pageDown->setScale(highScale*1.5);
        pageDown->setAnchorPoint(Vec2(1.0f, 0.5f));
        
        pageQuit->setVisible(false);
        pageShare->setVisible(false);
    #endif
    });
    
	//设置资源尺寸
//	bookParser->setResourceSize(Size(2048, 1536));
    //书籍解析主函数
	//坐标比例基准： iphone：640；ipad：768
	int code = bookParser->bookJsonParser(bookPath, 1.0, bookParser->getResourceSize().height/ 640, "ios");
    log("error code ==== %d",code);
    
}


NS_FK_END

