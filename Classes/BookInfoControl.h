#ifndef __BOOKINFO_CONTROL_H__
#define __BOOKINFO_CONTROL_H__

class BookInfoControl
{
public:
	BookInfoControl();
	~BookInfoControl();
	void setControl(int bookid, int memberid, int bookstauts);

	int BookId() const { return bookId; }
	void BookId(int val);
	int MemberId() const { return memberId; }
	void MemberId(int val) { memberId = val; }
	int BookStauts() const { return bookStauts; }
	void BookStauts(int val);
	bool IsBookFree() const { return isBookFree; }
	void IsBookFree(bool val) { isBookFree = val; }
	bool IsBookExit();
	void IsBookExit(bool val) { isBookExit = val; }
	bool IsBookVIP() const { return isBookVIP; }
	void IsBookVIP(bool val) { isBookVIP = val; }
	bool IsBookRent() const { return isBookRent; }
	void IsBookRent(bool val) { isBookRent = val; }
	bool IsBookBuy();
	void IsBookBuy(bool val) { isBookBuy = val; }
	bool IsMemberVIP() const { return isMemberVIP; }
	void IsMemberVIP(bool val) { isMemberVIP = val; }
private:
	int bookId;
	int memberId;
	int bookStauts;
	bool isBookFree;
	bool isBookExit;
	bool isBookVIP;
	bool isBookRent;
	bool isBookBuy;
	bool isMemberVIP;
};
#endif //  
