/****************************************************************************
YYX框架
针对cocos2dx封装图层类
俞越星
2016/06/12
****************************************************************************/

#ifndef __YYXStruct_H__
#define __YYXStruct_H__

#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class YYXStruct : public Ref
{
public:
	YYXStruct();
	YYXStruct(long long intD, std::string str, Ref * ref);
	~YYXStruct();

	long long intData;
	std::string stringData;
	Ref* refData;

	static YYXStruct initYYXStruct(long long intD, std::string str="", Ref * ref=nullptr);
	static bool deleteMapYYXStruct(map<string, YYXStruct>& dataMap, string key);
	static long long getInt64Data(YYXStruct data, long long defaultData);
	static string getStringData(YYXStruct data, string defaultData);
	static Ref* getRefData(YYXStruct data, Ref* defaultData);
	static bool initMapYYXStruct(map<string, YYXStruct>& dataMap, string key, long long intD=0, std::string str = "", Ref * ref = nullptr);
	static long long getMapInt64(map<string, YYXStruct> dataMap, string key, long long defaultData);
	static string getMapString(map<string, YYXStruct> dataMap, string key, string defaultData);
	static Ref* getMapRef(map<string, YYXStruct> dataMap, string key, Ref* defaultData);
};
#endif