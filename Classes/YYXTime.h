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
	std::string getRandomFileName();
	int getIntNumber();
	int getNowDay();//获取当天的日子 3月24日 返回24
	std::string getNowTimeString();
	YYXTime();
	~YYXTime();

	static std::string getStringFormLongLong(long long t);
private:
	static YYXTime* instance;
	long long m_Random;//记录当前使用到的最后一个随机数
	int m_number = 0;
	int m_RandomL = 0;
};
#endif