#ifndef _CITYTALBLEVIEW_H__
#define _CITYTALBLEVIEW_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "App.h"
#include "ui/UILayout.h"
#include "FrameAnimation.h"
#include <vector>
#include "BookStoreCache.h"
using namespace std;

class CityTableView : public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate , public Layer
{
private:
	TableView* m_tv;
	vector<CityButtonType> m_typeList;
	vector<Size> m_data;

public:
	CREATE_FUNC(CityTableView);
	bool init();
	void loadData();
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx) override;
    virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table) override;

	Layout* createItem(Size mapdata, int idx);
	void reuseItem(TableViewCell * item, Size mapdata, int idx);
	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) override {}
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) override{}
};
#endif