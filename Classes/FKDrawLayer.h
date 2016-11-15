#ifndef __DRAW_LAYER_H__
#define __DRAW_LAYER_H__

#include "cocos2d.h"
#include "FKBookParser.h"
#include "FKGameController.h"
#include "FKLinePoint.h"
#include "ui/CocosGUI.h"
#include "FKAudioPlayer.h"

NS_FK_BEGIN

USING_NS_CC;
using namespace cocos2d::ui;


struct SelectPen {
    int penTag;
    int pageNum;
};

class FK_DLL DrawLayer : public Layer
{
public:
    
    virtual bool init();
    
    virtual void onEnter();
    virtual void onExit();
    virtual void onExitTransitionDidStart();
    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchMoved(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
    virtual void onTouchCancelled(Touch* touch, Event* event);
    
    CREATE_FUNC(DrawLayer);
    //得到Z值最大的sprite的tag
    int getTagFromZOrder(map<int, int> &mTmp);
    //重载draw方法
    virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags);
    //add point to vector
    void addPointInPoints(Point point, float width);
    //计算平稳点集
    vector<LinePoint> calculateSmoothLinePoints();
    
    //画板弹出控制
    void canvesCallback(Ref* sender);
    //画笔选择
    void colorPenSelectCallback(Ref* sender);
    //画笔粗细选择
    void colorPenWidthSelectCallback(Ref* sender);
    //画笔翻页
    void colorPenLeftCallback(Ref* sender);
    void colorPenRightCallback(Ref* sender);
    //橡皮擦
    void earserCallback(Ref* sender);
    //废纸篓
    void trashOnTouch(Sprite* sprite);
    void colorSelect(int iTag);

    
    //画板全屏保存功能
    void saveRenderTexture(string filePath);
    void readRenderTexture(string fliePath);
private:
    EventListenerTouchOneByOne*    _oneTouch;
    GameController*                _gameController;
    RenderTexture*                 _renderTexture;
    Color4F                        _color;
    float                          _fwidth;
    vector<LinePoint>              _vDrawPoint;
    Rect                           _drawRect;
    bool                           _canvesState;
    Vector<MenuItem*>              _vPenColor;
    Vector<MenuItem*>              _vPenWidth;
    Button*                        _currentPen;
    DrawNode*                      _earserNode;
    bool                           _earserState;
    Size                           _winSize;
    string                         _suffix;
    string                         _sDicPath;
    float                          _fWidthScale;
    float                          _fHighScale;
    struct SelectPen               _selectPen;
};

NS_FK_END


#endif /* __DRAW_LAYER_H__ */
