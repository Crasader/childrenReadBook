#include "BookInfoControl.h"
#include "cocos2d.h"
#include "App.h"
USING_NS_CC;

BookInfoControl::BookInfoControl()
{	
	App::log("BookInfoControl()");
	bookId = -999;
	memberId = -999;
	bookStauts = 0;
	isBookFree = false;
	isBookExit = false;
	isBookVIP = false;
	isBookRent = false;
	isBookBuy = false;
	isMemberVIP = false;
}

BookInfoControl::~BookInfoControl()
{
	App::log("~BookInfoControl()");
}

void BookInfoControl::setControl(int bookid, int memberid, int bookstauts)
{
	BookId(bookid);
	MemberId(memberid);
	BookStauts(bookstauts);
}

void BookInfoControl::BookId(int val)
{
	bookId = val;
	if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(val)))
		isBookExit = true;
	else
		isBookExit = false;
}

void BookInfoControl::BookStauts(int val)
{/*
	1.判断是否是VIP用户
		A.VIP用户	 1 << 4
		B.非VIP	 0 << 4
		2.判断是否购买过书籍
		A.购买	 1 << 3
		B.未购买	 0 << 3
		3.判断是否超过限制
		A.超	 1 << 2
		B.未超	 0 << 2
		4.判断是否已经租过
		A.租过	 1 << 1
		B.未租	 0 << 1
		5.判断书籍是否属于VIP类型书籍
		A.属于     1
		B.不属于  0
		6.判断书籍是否免费
		A.免费	 1 << 5
		B.不免费  0 << 5
		*/
	bookStauts = val;
	auto code = val & (1 << 0);
	if (code > 0)
		IsBookVIP(true);
	else
		IsBookVIP(false);

	code = val & (1 << 1);
	if (code > 0)
		IsBookRent(true);
	else
		IsBookRent(false);

	code = val & (1 << 3);
	if (code > 0)
		IsBookBuy(true);
	else
		IsBookBuy(false);

	code = val & (1 << 4);
	if (code > 0)
		IsMemberVIP(true);
	else
		IsMemberVIP(false);

	code = val & (1 << 5);
	if (code > 0)
		IsBookFree(true);
	else
		IsBookFree(false);
}

bool BookInfoControl::IsBookExit()
{
	if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookId)))
		isBookExit = true;
	else
		isBookExit = false;
	return isBookExit;
}

bool BookInfoControl::IsBookBuy()
{
	if (!isBookBuy)
	{
		if (App::GetInstance()->m_me)
		{
			if (App::GetInstance()->myBuyBook.find(bookId) != App::GetInstance()->myBuyBook.end())
				isBookBuy = true;
		}
	}
	return isBookBuy;
}
