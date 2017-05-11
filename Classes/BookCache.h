#ifndef __BOOKCACHE_H__
#define __BOOKCACHE_H__

#include <string>
#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class Book : public Ref
{
public:
	Book();
	Book(int bookid);
	~Book();
	static Book* create();
	static void del(Book*);
	void copy(Book* book);
	std::string getCoverURL() const { return m_coverURL; }
	Book* setCoverURL(std::string val);
	string getJson();
	Book* getBook(string json);
	
	int getBookId() const { return m_bookId; }
	Book* setBookId(int val) {
		m_bookId = val;
		return this;
	}
	std::string getName() const { return m_name; }
	void setName(std::string val) { m_name = val; }
	std::string getContent() const { return m_content; }
	void setContent(std::string val) { m_content = val; }
	int getBookPrice() const { return m_bookPrice; }
	void setBookPrice(int val) { m_bookPrice = val; }
	int getBookmarketPrice() const { return m_bookmarketPrice; }
	void setBookmarketPrice(int val) { m_bookmarketPrice = val; }
	int getHashBuy() const { return m_hashBuy; }
	void setHashBuy(int val) { m_hashBuy = val; }
	std::string getBookPlayUrl() const { return m_bookPlayUrl; }
	void setBookPlayUrl(std::string val) { m_bookPlayUrl = val; }
	std::string getBookAuthor() const { return m_bookAuthor; }
	void setBookAuthor(std::string val) { m_bookAuthor = val; }
	std::string getBookPress() const { return m_bookPress; }
	void setBookPress(std::string val) { m_bookPress = val; }
	int getBookPage() const { return m_bookPage; }
	void setBookPage(int val) { m_bookPage = val; }
	int getScore() const { return m_score; }
	void setScore(int val) { m_score = val; }
	int getIsNewEllaBook() const { return isNewEllaBook; }
	void setIsNewEllaBook(int val) { isNewEllaBook = val; }
private:
	int m_bookId = -999;
	string m_coverURL = "";//封面下载地址
	string m_name = "";//书名
	string m_content = "";//介绍
	int m_bookPrice = -999;//价格
	int m_bookmarketPrice = -999;//纸质书价格
	int m_hashBuy = -999;//是否已购买
	string m_bookPlayUrl = "";//下载地址
	string m_bookAuthor = "";//作者
	string m_bookPress = "";//出版社
	int m_bookPage = -999;//页数
	int m_score = -999;//评分
	int isNewEllaBook = -999;//新书
};

class BookCache
{
public:
	BookCache();
	~BookCache();
	static BookCache* getInstance();
	void initDir();
	void addBook(Book* book);
	void delBook(int bookid);
	Book* getBook(int bookid);
	Book * getBookFromLocation(int bookid);
	string getBookPath(int bookid);
	void clear();

	string getBookCoverPath(int bookid);

	std::string getCoverDir() const { return m_coverDir; }
	void setCoverDir(std::string val) { m_coverDir = val; }

private:
	static BookCache* instance;
	std::unordered_map<int, Book*> m_data;
	string m_dir = "";//书缓存文件夹
	string m_coverDir = "";//书封面缓存文件夹
};


#endif