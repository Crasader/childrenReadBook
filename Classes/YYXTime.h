#pragma once
#ifndef __YYXTIME_H__
#define __YYXTIME_H__

class YYXTime
{
public:
	static YYXTime* getInstance();
	long long getNowTime4MS();
	long long getNowTime4S();
	
	YYXTime();
	~YYXTime();

private:
	static YYXTime* instance;

};
#endif