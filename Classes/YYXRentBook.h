#ifndef __YYXRentBook_HH__
#define __YYXRentBook_HH__
#include "cocos2d.h"
#include "App.h"
#include "NetIntface.h"

USING_NS_CC;
using namespace std;

class YYXRentBook
{
private:
	function<void()> m_callback;//租书成功后的回调
	int m_bookId = 0;
	int m_memberId = 0;
	string m_bookName = "";
	void httpRentBook();
	void httpBookInfo();

public:
	YYXRentBook();
	~YYXRentBook();
	static YYXRentBook* instance;
	static YYXRentBook* getInstance();

	void newRentBook(int bookid, int memberId, const function<void()>& callback);//带界面确认
	void backgroundThreadRentBook(int bookid, int memberId, const function<void()>& val);//后台直接租书
	void setCallback(const function<void()>& val) { m_callback = val; }
	void RentBookMessageBox();
};
#endif 