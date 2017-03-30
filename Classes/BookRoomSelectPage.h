/*
这是一个书房选项卡的基类
1.获取数据
2.排序规则
3.展示数据
*/
#ifndef __BOOKROOMSELECTPAGE_H__
#define __BOOKROOMSELECTPAGE_H__
#include "App.h"
USING_NS_CC;
using namespace std;

class BookRoomSelectPage
{
public:
	BookRoomSelectPage();
	~BookRoomSelectPage();
	virtual void initData()=0;//初始化数据
	virtual void filtrate() = 0;//筛选，剔除
	virtual void sort() = 0;//排序
	virtual vector<int> getResultData() = 0;//提供筛选和排序后结果的数组
};
#endif
