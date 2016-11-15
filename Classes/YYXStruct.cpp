#include "YYXStruct.h"

YYXStruct::YYXStruct()
	:intData(0),stringData(""), refData(nullptr)
{
}

YYXStruct::YYXStruct(long long intD,std::string str, Ref* ref)
	: intData(intD), stringData(str),refData(ref)
{
}

YYXStruct::~YYXStruct()
{
}

YYXStruct YYXStruct::initYYXStruct(long long intD, std::string str, Ref * refs)
{
	YYXStruct data = YYXStruct(intD,str,refs);
	return YYXStruct();
}

long long YYXStruct::getInt64Data(YYXStruct data, long long defaultData)
{
	if (&data)
		return data.intData;
	else
		return defaultData;
}

string YYXStruct::getStringData(YYXStruct data, string defaultData)
{
	if (&data)
		return data.stringData;
	else
	{
		if (&defaultData)
			return defaultData;
		else
			return "";
	}
}

Ref * YYXStruct::getRefData(YYXStruct data, Ref * defaultData)
{
	if (&data)
		return data.refData;
	else
		return defaultData;
}

long long YYXStruct::getMapInt64(map<string, YYXStruct> dataMap, string key, long long defaultData)
{
	if (&dataMap && &key)
	{
		if (dataMap.find(key) == dataMap.end())
			return defaultData;
		else
		{
			return dataMap[key].intData;
		}
	}
	else
		return defaultData;
}

string YYXStruct::getMapString(map<string, YYXStruct> dataMap, string key, string defaultData)
{
	if (&dataMap && &key)
	{
		if (dataMap.find(key) == dataMap.end())
			return defaultData;
		else
		{
			return dataMap[key].stringData;
		}
	}
	else
		return defaultData;
}

Ref * YYXStruct::getMapRef(map<string, YYXStruct> dataMap, string key, Ref * defaultData)
{
	if (&dataMap && &key)
	{
		if (dataMap.find(key) == dataMap.end())
			return defaultData;
		else
		{
			return dataMap[key].refData;
		}
	}
	else
		return defaultData;
}

bool YYXStruct::initMapYYXStruct(map<string, YYXStruct>& dataMap, string key, long long intD , std::string str , Ref * refs )
{
	if (&dataMap && &key && &str)
	{
		if (dataMap.find(key) == dataMap.end())
		{
			dataMap[key] = YYXStruct(intD, str, refs);
		}
		else
		{
			if(intD != -999)
				dataMap[key].intData = intD;
			if(str != "")
				dataMap[key].stringData = str;
			if (refs != nullptr)
				dataMap[key].refData = refs;
		}
		return true;
	}
	return false;
}

bool YYXStruct::deleteMapYYXStruct(map<string, YYXStruct>& dataMap, string key)
{
	if (&dataMap && &key)
	{
		if (dataMap.find(key) != dataMap.end())
		{
			dataMap.erase(key);
			return true;
		}
	}
	return false;
}