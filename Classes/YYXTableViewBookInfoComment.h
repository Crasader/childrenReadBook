/****************************************************************************
 Copyright (c) 2013-2015 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef _YYXTALBLEVIEWBookInfoComment_H__
#define _YYXTALBLEVIEWBookInfoComment_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "App.h"
#include "ui/UILayout.h"
#include "FrameAnimation.h"

class YYXTableViewBookInfoComment : public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate , public Layer
{
private:
	TableView* m_tv;
	std::vector<map<string, YYXStruct>> m_data;
	int m_playingVoiceIdx = -999;//正在播放声音的item下标
	bool m_myVoiceAble = false;//点中的是自己的评论播音区域
	int m_bookid = -999;
	int m_memberID = -1;
public:
	CREATE_FUNC(YYXTableViewBookInfoComment);
	bool init();
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx) override;
    virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table) override;

	vector<map<string, YYXStruct>> loadData(int bookid, int memberId);
	vector<map<string, YYXStruct>> loadData(int bookid);
	void loadData(map<string, YYXStruct> dat);

	Layout* createItem(map<string, YYXStruct>  data, int idx);
	void reuseItem(TableViewCell * item, map<string, YYXStruct> data, int idx);
	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) override {}
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) override{}

public:
	static void stopAllAnimation();

private:
	void playVoiceFrameAnimation(Sprite* vsp);

	void showAnimationEnd(Sprite* sp);
	void handleYourOwnComments(TableView* table, TableViewCell* cell);
	void soundPlayCompletes();
	void handleOtherComments(TableView* table, TableViewCell* cell);
};
#endif