//#ifndef _CITYTALBLEVIEW_H__
//#define _CITYTALBLEVIEW_H__
//
//#include "cocos2d.h"
//#include "extensions/cocos-ext.h"
//#include "App.h"
//#include "ui/UILayout.h"
//#include "FrameAnimation.h"
//#include <vector>
//using namespace std;
//
//class CityButtonInfo : public Ref
//{
//public:
//	CityButtonInfo();
//	~CityButtonInfo();
//	static CityButtonInfo* create();
//	static void del(CityButtonInfo* citybuttoninfo);
//private:
//	int storeId = -999;
//	string url = "";
//	string nameImagePath = "";
//	string backgrondPath = "";
//};
//
//class CityTableView : public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate , public Layer
//{
//private:
//	TableView* m_tv;
//	vector<CityButtonInfo*> m_data;
//
//public:
//	CREATE_FUNC(CityTableView);
//	bool init();
//    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
//    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx) override;
//    virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx) override;
//    virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table) override;
//
//	vector<map<string, YYXStruct>> loadData(int bookid, int memberId);
//	vector<map<string, YYXStruct>> loadData(int bookid);
//	void loadData(map<string, YYXStruct> dat);
//
//	Layout* createItem(map<string, YYXStruct>  data, int idx);
//	void reuseItem(TableViewCell * item, map<string, YYXStruct> data, int idx);
//	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) override {}
//    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) override{}
//
//public:
//	static void stopAllAnimation();
//
//private:
//	void playVoiceFrameAnimation(Sprite* vsp);
//
//	void showAnimationEnd(Sprite* sp);
//	void handleYourOwnComments(TableView* table, TableViewCell* cell);
//	void soundPlayCompletes();
//	void handleOtherComments(TableView* table, TableViewCell* cell);
//};
//#endif