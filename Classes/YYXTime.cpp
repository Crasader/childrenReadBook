#include "YYXTime.h"
#include <time.h>
#include "NetIntface.h"

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
	return NetIntface::getMillsTime();
}

long long YYXTime::getNowTime4S()
{
	long long times = -999;
	times = time(0);
	return times;
}

YYXTime::YYXTime()
{

}

YYXTime::~YYXTime()
{

}

