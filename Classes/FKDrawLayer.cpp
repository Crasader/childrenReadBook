#include "FKDrawLayer.h"

NS_FK_BEGIN

#define CANVES_OFFSET           89
#define COLOR_PEN_OFFSET        20
#define CANVES_MENU_OFFSET      25

#define COLOR_PEN_WIDTH         78
#define COLOR_PEN_SPACE         7
#define COLOR_PEN_NUMBER        36
#define COLOR_PENNUMBER_PAGE    6

// on "init" you need to initialize your instance
bool DrawLayer::init()
{
    if ( !Layer::init() )
    {
        
        return false;
    }
    
    _color = Color4F(Color3B(254, 199, 39));
    _fwidth = 5.0f;
    _canvesState = false;
    _earserState = false;
    Director *director = Director::getInstance();
    _winSize = director->getWinSize();
    _fWidthScale = _winSize.width/1024;
    _fHighScale = _winSize.height/768;
    
    Node *canvesNode = Node::create();
    this->addChild(canvesNode,100,1001);
    canvesNode->setScale(_fWidthScale, _fHighScale);
    canvesNode->setPosition(0,-CANVES_OFFSET*_fHighScale);
    
    //平台型号
    BookParser* bookParser = BookParser::getInstance();
    string sPlatform = bookParser->getPlatform();
    
    _sDicPath = "Canves/1024X768/";
    
    _suffix = "ipad";
    //画板背景
    string backgroundName = _sDicPath+"Drawboard_bg_"+ _suffix + ".png";
    Sprite* bg = Sprite::create(backgroundName);
    bg->setPosition(512, 45);
    canvesNode->addChild(bg,1,1);
    
    //画笔选择方向按钮
    string leftName = _sDicPath+"Drawboard_ page_down_"+ _suffix + ".png";
    string rightName = _sDicPath+"Drawboard_page_front_"+ _suffix + ".png";
    auto left = MenuItemImage::create(leftName, leftName, leftName, CC_CALLBACK_1(DrawLayer::colorPenLeftCallback, this));
    auto right = MenuItemImage::create(rightName, rightName, rightName, CC_CALLBACK_1(DrawLayer::colorPenRightCallback, this));
    auto directionMenu = Menu::create(left,right, NULL);
    directionMenu->alignItemsHorizontallyWithPadding(511);
    directionMenu->setPosition(321.5,35);
    bg->addChild(directionMenu,2);
    
    //画笔选择
    string penNormal;
    auto penView = ScrollView::create();
    penView->setContentSize(Size(510, 137+COLOR_PEN_OFFSET));
    penView->setInnerContainerSize(penView->getContentSize());
    Size pensSize = penView->getContentSize();
    for (unsigned int i = 101; i <= 136; i++) {
        penNormal = _sDicPath +"Drawboard_pen_" + toString(i-100) + "_" + _suffix + ".png";
        auto button = Button::create(penNormal,penNormal,penNormal);
        button->addClickEventListener([this](Ref* sender){
            this->colorPenSelectCallback(sender);
        });
        button->setPosition(Vec2(button->getContentSize().width/2+(button->getContentSize().width+COLOR_PEN_SPACE)*(i-101), button->getContentSize().height/2));
        penView->addChild(button,1,i);
        if (i == 101) {
            _currentPen = button;
        }
        if (i >= 107) {
            penView->setInnerContainerSize(penView->getInnerContainerSize().operator+(Size(button->getContentSize().width+COLOR_PEN_SPACE, 0)));
        }
    }
    penView->setDirection(ScrollView::Direction::HORIZONTAL);
    penView->setInertiaScrollEnabled(true);
    penView->setScrollBarEnabled(false);
    penView->setPosition(Vec2(70,-CANVES_MENU_OFFSET-21));
    penView->addEventListener([this](Ref* sender, ScrollView::EventType eventType){
        if (eventType == ScrollView::EventType::SCROLLING) {
            auto penWidthMenu = this->getChildByTag(1001)->getChildByTag(1)->getChildByTag(110);
            penWidthMenu->setOpacity(0);
            
        }else if (eventType == ScrollView::EventType::CONTAINER_MOVED)
        {
            auto penView = (ScrollView*)sender;
            int number = penView->getInnerContainerPosition().x / (COLOR_PEN_WIDTH+COLOR_PEN_SPACE);
            int remainer = (int)penView->getInnerContainerPosition().x % (COLOR_PEN_WIDTH+COLOR_PEN_SPACE);
            if (remainer != 0) {
                penView->setInnerContainerPosition(Vec2((number-1)*(COLOR_PEN_WIDTH+COLOR_PEN_SPACE),0));
            }
            
        }
    });
    bg->addChild(penView, 3, 109);
    
    //画笔粗细选择
    for (unsigned int i = 1; i < 4; i++) {
        string penWidthNormalName = _sDicPath+"Drawboard_noselect_width_"+toString(i)+"_"+ _suffix + ".png";
        string penWidthSelectName = _sDicPath+"Drawboard_select_width_"+toString(i)+"_"+ _suffix + ".png";
        
        auto penWidth = MenuItemImage::create(penWidthNormalName, penWidthSelectName, penWidthSelectName, CC_CALLBACK_1(DrawLayer::colorPenWidthSelectCallback, this));
        _vPenWidth.pushBack(penWidth);
        penWidth->setTag(i);
        if (i == 1) {
            penWidth->selected();
        }
    }
    auto penWidthMenu = Menu::createWithArray(_vPenWidth);
    penWidthMenu->alignItemsHorizontallyWithPadding(20);
    penWidthMenu->setOpacity(0);
    penWidthMenu->setPosition(182, 166);
    bg->addChild(penWidthMenu,4,110);
    
    //橡皮
    string earserName = _sDicPath +"Drawboard_rubber_" + _suffix + ".png";
    auto earserBtn = Button::create(earserName,earserName,earserName);
    earserBtn->addClickEventListener([this](Ref* sender){
        this->earserCallback(sender);
    });
    earserBtn->setPosition(Vec2(767-89,47-CANVES_MENU_OFFSET));
    bg->addChild(earserBtn,2,200);
    
    //废纸篓
    string trashName = _sDicPath +"Drawboard_trash_" + _suffix + ".png";
    auto trash = Sprite::create(trashName);
    trash->setPosition(873-89,56-CANVES_MENU_OFFSET);
    bg->addChild(trash,2,108);

    
    //画板按钮
    string canvesName = _sDicPath+"Drawboard_noselect_open_"+ _suffix + ".png";
    string canvesSelectName = _sDicPath+"Drawboard_select_open_"+ _suffix + ".png";
    auto canves = MenuItemImage::create(canvesName,
                                        canvesSelectName,
                                        canvesSelectName,
                                        CC_CALLBACK_1(DrawLayer::canvesCallback, this));
    auto canvesMenu = Menu::create(canves, NULL);
    canvesMenu->setPosition(688,116);
    canvesNode->addChild(canvesMenu, 0);
    
    //渲染render
    _renderTexture = RenderTexture::create(_winSize.width, _winSize.height,Texture2D::PixelFormat::RGBA8888);
    _renderTexture->setPosition(_winSize.width/2, _winSize.height/2);
    this->addChild(_renderTexture,0,1000);
    
    string drawFilePath = FileUtils::getInstance()->getWritablePath() + bookParser->getDrawFilePath() + toString(bookParser->getCurrentPage()) + ".png";
    
    readRenderTexture(drawFilePath);
    
    
    _selectPen.penTag = _currentPen->getTag();
    
    return true;
}

void DrawLayer::onExit()
{
    Layer::onExit();
    this->removeAllChildrenWithCleanup(true);
}

void DrawLayer::onExitTransitionDidStart()
{
    Layer::onExitTransitionDidStart();
    Director::getInstance()->getEventDispatcher()->removeEventListener(_oneTouch);
}
void DrawLayer::onEnter()
{
    Layer::onEnter();
    _oneTouch = EventListenerTouchOneByOne::create();
    _oneTouch->setSwallowTouches(true);
    _oneTouch->onTouchBegan = CC_CALLBACK_2(DrawLayer::onTouchBegan,this);
    _oneTouch->onTouchMoved = CC_CALLBACK_2(DrawLayer::onTouchMoved,this);
    _oneTouch->onTouchEnded = CC_CALLBACK_2(DrawLayer::onTouchEnded,this);
    _oneTouch->onTouchCancelled = CC_CALLBACK_2(DrawLayer::onTouchCancelled,this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_oneTouch, 2);
    
    _gameController = (GameController*)this->getUserObject();
    //获取画板区域
    PageData pagedata = _gameController->getPageData();
    _drawRect = pagedata.getRenderRect();
}

void DrawLayer::addPointInPoints(Point point, float width)
{
    LinePoint linePoint;
    linePoint.setPointAndWidth(point, width);
    _vDrawPoint.push_back(linePoint);
}

#pragma mark - Menu Callback

void DrawLayer::canvesCallback(Ref* sender)
{
    MenuItemImage* canves = (MenuItemImage*)sender;
    Node* node = this->getChildByTag(1001);
    string canvesName;
    string canvesSelectName;
    if(!_canvesState)
    {
        _canvesState = true;
        node->runAction(MoveBy::create(0.2, Vec2(0, CANVES_OFFSET*_fHighScale)));
        _currentPen->runAction(MoveBy::create(0.3, Vec2(0, COLOR_PEN_OFFSET)));
        canvesName = _sDicPath+"Drawboard_noselect_back_"+ _suffix + ".png";
        canvesSelectName = _sDicPath+"Drawboard_select_back_"+ _suffix + ".png";
        canves->setNormalImage(Sprite::create(canvesName));
        canves->setSelectedImage(Sprite::create(canvesSelectName));
        canves->setDisabledImage(Sprite::create(canvesSelectName));
        
        Node *penWidthMenu = node->getChildByTag(1)->getChildByTag(110);
        penWidthMenu->setOpacity(255);
        
        Node *earserMenu = node->getChildByTag(1)->getChildByTag(200);
        earserMenu->runAction(MoveBy::create(0.2, Vec2(0, CANVES_MENU_OFFSET)));
        Node *trashMenu = node->getChildByTag(1)->getChildByTag(108);
        trashMenu->runAction(MoveBy::create(0.2, Vec2(0, CANVES_MENU_OFFSET)));
        Node *penMenu = node->getChildByTag(1)->getChildByTag(109);
        penMenu->runAction(MoveBy::create(0.2, Vec2(0, CANVES_MENU_OFFSET)));
    }else
    {
        _canvesState = false;
        node->runAction(MoveBy::create(0.2, Vec2(0, -CANVES_OFFSET*_fHighScale)));
        _currentPen->runAction(MoveBy::create(0.3, Vec2(0, -COLOR_PEN_OFFSET)));
        
        canvesName = _sDicPath+"Drawboard_noselect_open_"+ _suffix + ".png";
        canvesSelectName = _sDicPath+"Drawboard_select_open_"+ _suffix + ".png";
        canves->setNormalImage(Sprite::create(canvesName));
        canves->setSelectedImage(Sprite::create(canvesSelectName));
        canves->setDisabledImage(Sprite::create(canvesSelectName));
        
        Node *penWidthMenu = node->getChildByTag(1)->getChildByTag(110);
        penWidthMenu->setOpacity(0);
        
        Node *earserMenu = node->getChildByTag(1)->getChildByTag(200);
        earserMenu->runAction(MoveBy::create(0.2, Vec2(0, -CANVES_MENU_OFFSET)));
        Node *trashMenu = node->getChildByTag(1)->getChildByTag(108);
        trashMenu->runAction(MoveBy::create(0.2, Vec2(0, -CANVES_MENU_OFFSET)));
        Node *penMenu = node->getChildByTag(1)->getChildByTag(109);
        penMenu->runAction(MoveBy::create(0.2, Vec2(0, -CANVES_MENU_OFFSET)));
    }
    
}

void DrawLayer::colorPenWidthSelectCallback(cocos2d::Ref *sender)
{
    for (unsigned int i = 0; i < _vPenWidth.size(); i++) {
        MenuItem* penWidth = _vPenWidth.at(i);
        penWidth->unselected();
    }
    
    MenuItemImage *penWidth = (MenuItemImage*)sender;
    penWidth->selected();
    switch (penWidth->getTag()) {
        case 1:
        {
            _fwidth = 5.0f;
            break;
        }
        case 2:
        {
            _fwidth = 8.0f;
            break;
        }
        case 3:
        {
            _fwidth = 10.0f;
            break;
        }
        default:
            break;
    }
}

void DrawLayer::colorPenSelectCallback(Ref *sender)
{
    Button *penColor = (Button*)sender;
    _currentPen->runAction(MoveBy::create(0.3, Vec2(0, -COLOR_PEN_OFFSET)));
    penColor->runAction(MoveBy::create(0.3, Vec2(0, COLOR_PEN_OFFSET)));
    _currentPen = penColor;
    //刷新选中画笔
    _selectPen.penTag = _currentPen->getTag();
    
    if (_earserState) {
        _earserState = false;
    }

    string colorPath = "colorsound/color_s_" + toString(penColor->getTag()-100) + ".mp3";
    AudioPlayer::getInstance()->playEffect(colorPath, "yes");
    
    //显示画笔粗细选择界面
    Node *bg = this->getChildByTag(1001)->getChildByTag(1);
    Node *penWidthMenu = bg->getChildByTag(110);
    penWidthMenu->setOpacity(255);
    auto pensView = (ScrollView*)bg->getChildByTag(109);
    Point innerContentPosition = pensView->getInnerContainerPosition();
    float offset = penColor->getPosition().x + innerContentPosition.x + COLOR_PEN_WIDTH/2 + COLOR_PEN_SPACE;
    if (offset == pensView->getContentSize().width) {
        penWidthMenu->setPosition(penColor->getPosition().x + innerContentPosition.x + 143 - 74*2,166);
    }else
    {
        penWidthMenu->setPosition(penColor->getPosition().x + innerContentPosition.x + 143,166);
    }
    
    colorSelect(penColor->getTag());
}

void DrawLayer::colorSelect(int iTag)
{
    switch (iTag) {
        case 101:
            _color = Color4F(Color3B(250, 246, 238));
            break;
        case 102:
            _color = Color4F(Color3B(245, 239, 73));
            break;
        case 103:
            _color = Color4F(Color3B(254, 199, 39));
            break;
        case 104:
            _color = Color4F(Color3B(254, 157, 1));
            break;
        case 105:
            _color = Color4F(Color3B(212, 140, 33));
            break;
        case 106:
            _color = Color4F(Color3B(251, 109, 20));
            break;
        case 107:
            _color = Color4F(Color3B(245, 84, 37));
            break;
        case 108:
            _color = Color4F(Color3B(253, 218, 186));
            break;
        case 109:
            _color = Color4F(Color3B(232, 63, 15));
            break;
        case 110:
            _color = Color4F(Color3B(229, 15, 18));
            break;
        case 111:
            _color = Color4F(Color3B(140, 47, 34));
            break;
        case 112:
            _color = Color4F(Color3B(255, 122, 144));
            break;
        case 113:
            _color = Color4F(Color3B(254, 67, 145));
            break;
        case 114:
            _color = Color4F(Color3B(183, 20, 34));
            break;
        case 115:
            _color = Color4F(Color3B(187, 234, 79));
            break;
        case 116:
            _color = Color4F(Color3B(83, 206, 185));
            break;
        case 117:
            _color = Color4F(Color3B(59, 200, 103));
            break;
        case 118:
            _color = Color4F(Color3B(80, 201, 31));
            break;
        case 119:
            _color = Color4F(Color3B(0, 149, 110));
            break;
        case 120:
            _color = Color4F(Color3B(0, 146, 71));
            break;
        case 121:
            _color = Color4F(Color3B(28, 67, 57));
            break;
        case 122:
            _color = Color4F(Color3B(1, 78, 91));
            break;
        case 123:
            _color = Color4F(Color3B(24, 57, 61));
            break;
        case 124:
            _color = Color4F(Color3B(0, 141, 218));
            break;
        case 125:
            _color = Color4F(Color3B(71, 206, 255));
            break;
        case 126:
            _color = Color4F(Color3B(1, 101, 179));
            break;
        case 127:
            _color = Color4F(Color3B(1, 72, 155));
            break;
        case 128:
            _color = Color4F(Color3B(4, 42, 86));
            break;
        case 129:
            _color = Color4F(Color3B(191, 137, 242));
            break;
        case 130:
            _color = Color4F(Color3B(145, 8, 132));
            break;
        case 131:
            _color = Color4F(Color3B(85, 57, 155));
            break;
        case 132:
            _color = Color4F(Color3B(33, 19, 123));
            break;
        case 133:
            _color = Color4F(Color3B(124, 124, 124));
            break;
        case 134:
            _color = Color4F(Color3B(185, 74, 28));
            break;
        case 135:
            _color = Color4F(Color3B(86, 51, 42));
            break;
        case 136:
            _color = Color4F(Color3B(0, 0, 0));
            break;
        default:
            break;
    }

}

void DrawLayer::trashOnTouch(Sprite* sprite)
{
    _renderTexture->clear(0, 0, 0, 0);
    
    //垃圾桶帧动画
    auto cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile(_sDicPath + "z1_ani_" + _suffix + ".plist");
    
    char str[50] = {0};
    Vector<SpriteFrame*> vSpriteFrame;
    for (unsigned int i = 0; i < 6; i++) {
        sprintf(str, "z1_000%d.png",i);
        auto spriteFrame = cache->getSpriteFrameByName(str);
        vSpriteFrame.pushBack(spriteFrame);
    }
    auto animation = Animation::createWithSpriteFrames(vSpriteFrame,0.2);
    sprite->runAction(Sequence::create(Animate::create(animation),CallFuncN::create([=](Node* node){
        string trashName = _sDicPath +"Drawboard_trash_" + _suffix + ".png";
        Sprite* sprite = (Sprite*)node;
        sprite->setTexture(trashName);
    }), NULL) );
}

void DrawLayer::earserCallback(Ref* sender)
{
    if (!_earserState) {
        Button* earserItem = (Button*)sender;
        earserItem->runAction(MoveBy::create(0.3, Vec2(0, COLOR_PEN_OFFSET)));
        _currentPen->runAction(MoveBy::create(0.3, Vec2(0, -COLOR_PEN_OFFSET)));
        _currentPen = earserItem;
        //刷新选中画笔
        _selectPen.penTag = earserItem->getTag();
        
        _earserState = true;
        _earserNode = DrawNode::create();
        
        float fRadius		= 10*6;						///< 圆的半径
        const int nCount	= 100;							///< 用正100边型来模拟园
        const float coef	= 2.0f * (float)M_PI/nCount;	///< 计算每两个相邻顶点与中心的夹角
        static Point circle[nCount];						///< 顶点数组
        for(unsigned int i = 0; i < nCount; i++) {
            float rads = i*coef;							///< 弧度
            circle[i].x = fRadius * cosf(rads);				///< 对应顶点的x
            circle[i].y = fRadius * sinf(rads);				///< 对应顶点的y
        }
        _earserNode->drawPolygon(circle, nCount, Color4F(0, 0, 0, 0), 0, Color4F(0, 0, 0, 0));
        _earserNode->retain();
    }
}

void DrawLayer::colorPenLeftCallback(Ref *sender)
{
    auto penWidthMenu = this->getChildByTag(1001)->getChildByTag(1)->getChildByTag(110);
    penWidthMenu->setOpacity(0);
    auto pensView = (ScrollView*)(this->getChildByTag(1001)->getChildByTag(1)->getChildByTag(109));
    int number = -pensView->getInnerContainerPosition().x/(COLOR_PEN_WIDTH+COLOR_PEN_SPACE);
    if (number >= COLOR_PENNUMBER_PAGE) {
        pensView->setInnerContainerPosition(Vec2(pensView->getInnerContainerPosition().x+(COLOR_PEN_WIDTH+COLOR_PEN_SPACE)*COLOR_PENNUMBER_PAGE, pensView->getInnerContainerPosition().y));
    }else
    {
        pensView->jumpToLeft();
    }
}

void DrawLayer::colorPenRightCallback(Ref *sender)
{
    auto penWidthMenu = this->getChildByTag(1001)->getChildByTag(1)->getChildByTag(110);
    penWidthMenu->setOpacity(0);
    auto pensView = (ScrollView*)(this->getChildByTag(1001)->getChildByTag(1)->getChildByTag(109));
    int number = -pensView->getInnerContainerPosition().x/(COLOR_PEN_WIDTH+COLOR_PEN_SPACE);
    if (number <= COLOR_PEN_NUMBER - COLOR_PENNUMBER_PAGE*2) {
        pensView->setInnerContainerPosition(Vec2(pensView->getInnerContainerPosition().x-(COLOR_PEN_WIDTH+COLOR_PEN_SPACE)*COLOR_PENNUMBER_PAGE, pensView->getInnerContainerPosition().y));
    }else
    {
        pensView->jumpToRight();
    }
}

#pragma mark - Touch Event

bool DrawLayer::onTouchBegan(Touch* touch, Event* event)
{
    Point location = touch->getLocation();
    Node* canves = this->getChildByTag(1001);
    Node* bg = canves->getChildByTag(1);
    Node* trash = bg->getChildByTag(108);
    if (trash->getBoundingBox().containsPoint(bg->convertToNodeSpace(location))) {
        trashOnTouch((Sprite*)trash);
    }
    if (_drawRect.containsPoint(location)) {
        _vDrawPoint.clear();
        addPointInPoints(location, _fwidth);
        addPointInPoints(location, _fwidth);
        addPointInPoints(location, _fwidth);
    }
    return true;
}
void DrawLayer::onTouchMoved(Touch* touch, Event* event)
{
    Point location = touch->getLocation();
    if (_drawRect.containsPoint(location)) {
        if (!_vDrawPoint.empty()) {
            vector<LinePoint>::iterator it = _vDrawPoint.end()-1;
            LinePoint linePoint = *it;
            Point dstPoint = linePoint.getPoint().operator-(location);
            float length = dstPoint.getLength();
            if (length < 1.5f) {
                return;
            }
        }
        addPointInPoints(location, _fwidth);
    }
}

void DrawLayer::onTouchEnded(Touch* touch, Event* event)
{
    Point location = touch->getLocation();
    if (_drawRect.containsPoint(location)) {
        addPointInPoints(location, _fwidth);
    }
}
void DrawLayer::onTouchCancelled(Touch* touch, Event* event)
{
    
}

#pragma mark - draw

vector<LinePoint> DrawLayer::calculateSmoothLinePoints()
{
    vector<LinePoint> vSmoothedPoints;
    if (_vDrawPoint.size() > 2) {
        //vSmoothedPoints->clear();
        for (unsigned int i = 2; i < _vDrawPoint.size(); i++) {
            LinePoint prev2 = _vDrawPoint[i-2];
            LinePoint prev1 = _vDrawPoint[i-1];
            LinePoint cur = _vDrawPoint[i];
            
            Point midPoint1 = prev1.getPoint().getMidpoint(prev2.getPoint());
            Point midPoint2 = cur.getPoint().getMidpoint(prev1.getPoint());
            
            int segmentDistance = 2;
            float distance = midPoint1.distance(midPoint2);
            int numberOfSegments = fmin(128, fmax(floor(distance/segmentDistance), 32));
            
            float t = 0.0f;
            float step = 1.0f / numberOfSegments;
            for (unsigned int j = 0; j < numberOfSegments; j++) {
                LinePoint newPoint;
                Point point = (midPoint1.operator*(pow(1 - t, 2)).operator+(prev1.getPoint().operator*(2.0f * (1 - t) * t))).operator+(midPoint2.operator*(t*t));
                float width = pow(1 - t, 2) * ((prev1.getWidth() + prev2.getWidth()) * 0.5f) + 2.0f * (1 - t) * t * prev1.getWidth() + t * t * ((cur.getWidth() + prev1.getWidth()) * 0.5f);
                newPoint.setPointAndWidth(point, width);
                vSmoothedPoints.push_back(newPoint);
                newPoint.clear();
                t += step;
            }
            LinePoint finalPoint;
            finalPoint.setPointAndWidth(midPoint2, (cur.getWidth()+prev1.getWidth())*0.5f);
            vSmoothedPoints.push_back(finalPoint);
            finalPoint.clear();
        }
        _vDrawPoint.erase(_vDrawPoint.begin(), _vDrawPoint.end()-2);
    }
    
    return vSmoothedPoints;
//    vSmoothedPoints.swap(vSmoothedPoints);
}

void DrawLayer::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
    _renderTexture->begin();
    vector<LinePoint> vSmoothedPoints = calculateSmoothLinePoints();
    if (!vSmoothedPoints.empty()) {
        //生成draw
        DrawNode* drawNode = DrawNode::create();
        for (unsigned int i = 0; i < vSmoothedPoints.size()-1; i++) {
            LinePoint linePoint1 = vSmoothedPoints[i];
            LinePoint linePoint2 = vSmoothedPoints[i+1];
            if (_earserState) {
                _earserNode->setPosition(linePoint1.getPoint());
                BlendFunc blend = {GL_ONE,GL_ZERO};
                _earserNode->setBlendFunc(blend);
                _earserNode->visit();
            }else
            {
                drawNode->drawSegment(linePoint1.getPoint(), linePoint2.getPoint(), _fwidth, _color);
            }
        }
        drawNode->visit();
    }
    _renderTexture->end();
}


void DrawLayer::saveRenderTexture(string filePath)
{
    _renderTexture->saveToFile(filePath, Image::Format::PNG);
    
}

void DrawLayer::readRenderTexture(string filePath)
{
    if (FileUtils::getInstance()->isFileExist(filePath)) {
        auto drawSprite = Sprite::create(filePath);
        drawSprite->setPosition(Vec2(_winSize.width/2, _winSize.height/2));
        _renderTexture->begin();
        drawSprite->visit();
        _renderTexture->end();
        
        Director::getInstance()->getTextureCache()->removeTextureForKey(filePath);
    }
}

#pragma mark - 点击sprite以后的功能
//得到Z值最大的sprite的tag
int DrawLayer::getTagFromZOrder(map<int, int> &mTmp)
{
    return mTmp.rbegin()->second;
}

NS_FK_END





