﻿#ifndef __YYXBookOver_HH__
#define __YYXBookOver_HH__
#include "cocos2d.h"
#include "App.h"

USING_NS_CC;
using namespace std;

class YYXBookOver
{
private:
	int m_bookId = 0;
	int m_memberId = 0;
	bool m_isUserVip = false;
	bool m_isUserBuy = false;

	void buySuccessMessageBox(ImageView* goumaichenggongLayer);
	void yaoqingzhuce();

public:
	YYXBookOver();
	~YYXBookOver();
	static YYXBookOver* instance;
	static YYXBookOver* getInstance();
	void init(int bookId, int memberId, bool isTry);
	Layer* tryReadBookOverLayer();
	Layer* readBookOverLayer();
	bool UserVip() const { return m_isUserVip; }
	void UserVip(bool val) { m_isUserVip = val; }
	bool UserBuy() const { return m_isUserBuy; }
	void UserBuy(bool val) { m_isUserBuy = val; }
};
#endif 