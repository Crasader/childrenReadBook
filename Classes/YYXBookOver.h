#ifndef __YYXBookOver_HH__
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
	bool m_isBookCover = false;//app界面和后台切换的时候使用 获取当前是否在封底界面 在封底就不恢复 在书籍就恢复

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
	bool BookCover() const { return m_isBookCover; }
	void BookCover(bool val) { m_isBookCover = val; }
};
#endif 