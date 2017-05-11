#include <map>
#ifndef _BOOKSTORECACHE_H__
#define _BOOKSTORECACHE_H__

using namespace std;

class StorePage
{
public:
	StorePage();
	~StorePage();
	int getBook(int index);

	void addBook(int num, int bookId);
	string getJson();
	void readJson(string json);
	void copy(StorePage*);

	int getStoreId() const { return m_storeId; }
	StorePage* setStoreId(int val) {
		m_storeId = val;
		return this;
	}

	int getPageNumber() const { return m_pageNumber; }
	StorePage* setPageNumber(int val) {
		m_pageNumber = val;
		return this;
	}
	std::map<int, int> getBooks() const { return m_books; }
private:
	int m_storeId = -999;
	int m_pageNumber = -999;
	std::map<int, int > m_books;
};

class BookStoreCache
{
public:
	BookStoreCache();
	~BookStoreCache();
	static BookStoreCache* getInstance();

	int getBookId(int page, int index);

	void initDir();
	void clear();

	int getStoreId() const { return m_storeId; }
	void setStoreId(int val);

	int getTotal();
	void setTotal(int val);

	//网络请求 与当前商店同id 写入内存和本地  不同则写本地
	void netCallback(int storeId, int total);
	void netCallBack(int storeId, int page, StorePage* pageData);
private:
	static BookStoreCache* instance;
	int m_storeId = -999;//当亲商店的id
	int m_total = 0;//当前书店的书籍总数
	string m_dir = "";//父文件夹
	string m_bookStoreDir = "";//当前书店文件夹
	string getBookStorePagePath(int page);//获取当前商店第page页的json文件路径
	std::unordered_map<int, StorePage*> m_data;
};

#endif