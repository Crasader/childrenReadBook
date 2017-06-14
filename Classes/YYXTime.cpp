#include "YYXTime.h"
#include <time.h>
#include "CrossPlatform.h"
#include <string>
#include <stdlib.h>
using namespace std;

YYXTime*  YYXTime::YYXTime::instance = nullptr;

YYXTime* YYXTime::getInstance()
{
	if (instance == nullptr)
	{
		instance = new YYXTime();
	}
	return instance;
}

long long YYXTime::getNowTime4MS()
{
	return CrossPlatform::getMillsTime();
}

long long YYXTime::getNowTime4S()
{
	long long times = -999;
	times = time(0);
	return times;
}

long long YYXTime::getRandom()
{
	auto times = getNowTime4S();
	if (times <= m_Random)
		++m_Random;
	else
		m_Random = times;
	return m_Random;
}

int YYXTime::getRandomL()
{
	long long times = getNowTime4S();
	auto temp = times % 100000000;
	if (temp <= m_RandomL)
		++m_RandomL;
	else
		m_RandomL = temp;
	App::log(" YYXTime::getRandomL() ", m_RandomL);
	return m_RandomL;
}

string YYXTime::getRandomFileName()
{
	int code = getRandomL();
	srand(code);
	int code2 = rand() % 1000;
	int code3 = rand() % 1000;
	int code4 = rand() % 1000;
	int code5 = rand() % 1000;
	string str = Value(code2).asString() +"_"+ Value(code3).asString() +"_"+ Value(code4).asString()+"_"+Value(code5).asString();
	return str;
}

int YYXTime::getIntNumber()
{
	++m_number;
	return m_number;
}

YYXTime::YYXTime()
{

}

YYXTime::~YYXTime()
{

}

std::string YYXTime::getNowTimeString()
{
	struct tm *lt;
	time_t tData = time(0);
	lt = localtime(&tData);
	char nowtime[24];
	memset(nowtime, 0, sizeof(nowtime));
	strftime(nowtime, 24, "%Y-%m-%d %H:%M:%S", lt);
	return string(nowtime);
}

std::string YYXTime::getStringFormLongLong(long long t)
{
	struct tm *lt;
	time_t tData = t;
	lt = localtime(&tData);
	char nowtime[24];
	memset(nowtime, 0, sizeof(nowtime));
	//strftime(nowtime, 24, "%Y-%m-%d %H:%M:%S", lt);
	strftime(nowtime, 24, "%Y-%m-%d", lt);
	return string(nowtime);
}

int YYXTime::getNowDay()
{
	struct tm *lt;
	time_t tData = time(0);
	lt = localtime(&tData);
	return lt->tm_mday;
}