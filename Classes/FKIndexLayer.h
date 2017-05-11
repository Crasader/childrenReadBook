#ifndef __INDEX_LAYER_H__
#define __INDEX_LAYER_H__

#include "cocos2d.h"
#include "json/document.h"
#include "FKJsonParser.h"
#include "FKPageLayer.h"

NS_FK_BEGIN

USING_NS_CC;

class FK_DLL IndexLayer : public Layer
{
public:
    static Scene* createScene();
    
    virtual bool init();
    void onClickBook(Ref* pSender);
    void addBookInfo();
    
    // implement the "static create()" method manually
    CREATE_FUNC(IndexLayer);
private:
    vector<string>              _vbookInfo;
    float                       _fIconScaleX;
    float                       _fIconScaleY;
};

NS_FK_END


#endif // __INDEX_LAYER_H__
