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
private:
	int m_bookId = -999;
	string m_coverURL = "";//封面下载地址
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