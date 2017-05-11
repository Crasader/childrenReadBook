#ifndef __BuyBook_H__
#define __BuyBook_H__
#include "App.h"
#include "BookRoomSelectPage.h"

using namespace std;

class MyBook :public BookRoomSelectPage
{
public:
	static MyBook* BuyBookinstance;
	static MyBook* getInstance();
	MyBook();
	~MyBook();

	void initDir();//硬盘文件夹
	void loadCache();//硬盘缓存读入
	void initData();//将m_inputbooks初始化
	void addBook(int bookid, int time, bool write= true);//添加书籍
	void deleteBook(int bookid);//删除书籍
	void writeDownXml();//写入本地
	void clearBook();
	void filtrate();
	void sort();
	vector<int> getResultData();
	bool getIsSorted() const { return IsSorted; }
	void setIsSorted(bool val) { IsSorted = val; }
private:
	std::unordered_map<int, int > m_buyTimes;//下载时间记录
	std::unordered_map<int, int> m_inputbooks;
	std::unordered_map<int, int> m_tempbooks;
	vector<int> m_outputbooks;
	bool IsSorted = false;//结果是否有序
	int count = -999;
	bool needSort();
};

#endif
