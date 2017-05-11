#pragma once
#ifndef __YYXTIME_H__
#define __YYXTIME_H__
#include <string>

class YYXTime
{
public:
	static YYXTime* getInstance();
	long long getNowTime4MS();
	long long getNowTime4S();
	long long getRandom();
	int getRandomL();
	YYXTime();
	~YYXTime();

	static std::string getStringFormLongLong(long long t);
private:
	static YYXTime* instance;
	long long m_Random;//记录当前使用到的最后一个随机数
};
#endif