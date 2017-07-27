#ifndef __BorrowBook_H__
#define __BorrowBook_H__
#include "App.h"

using namespace std;

class BorrowBook 
{
public:
	static BorrowBook* getInstance();
	static void destoryInstance();
	BorrowBook();
	~BorrowBook();

	void initDir();//硬盘文件夹
	void loadCache();//硬盘缓存读入
	void initData();//将m_inputbooks初始化
	void addBook(int bookid, int lock, bool write= true);//添加书籍
	void deleteBook(int bookid);//删除书籍
	void writeDownXml();//写入本地
	void clearBook();
	std::unordered_map<int, int> m_borrowbooks;

private:
	static BorrowBook* BorrowBookinstance;

};

#endif
