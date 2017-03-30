#ifndef __ReadBook_H__
#define __ReadBook_H__
#include "App.h"
#include "BookRoomSelectPage.h"

using namespace std;

class ReadBook :public BookRoomSelectPage
{
public:
	static ReadBook* ReadBookinstance;
	static ReadBook* getInstance();
	ReadBook();
	~ReadBook();

	void initDir();//硬盘文件夹
	void loadCache();//硬盘缓存读入
	void initData();//将m_inputbooks初始化
	void addBook(int bookid, int time);//添加书籍
	void deleteBook(int bookid);//删除书籍
	void clearBook();
	void filtrate();
	void sort();
	vector<int> getResultData();
	bool getIsSorted() const { return IsSorted; }
	void setIsSorted(bool val) { IsSorted = val; }
private:
	std::unordered_map<int, int > m_readTimes;//阅读时间记录
	std::unordered_map<int, int> m_tempbooks;
	vector<int> m_outputbooks;
	bool IsSorted = false;//结果是否有序
	int count = -999;
	bool needSort();
};

#endif
