#include "BookStoreCache.h"
#include "YYXLayer.h"
#include "NetIntface.h"

#define CITYBUTTONFORMAT "store_%d.json"

BookStoreCache* BookStoreCache::instance = nullptr;

CityButtonInfo::CityButtonInfo()
{

}

CityButtonInfo::~CityButtonInfo()
{

}

CityButtonInfo * CityButtonInfo::create()
{
	CityButtonInfo* pRef = nullptr;
	pRef = new CityButtonInfo();
	if (pRef)
	{
		pRef->autorelease();
		pRef->retain();
	}
	return pRef;
}

void CityButtonInfo::del(CityButtonInfo * citybuttoninfo)
{
	if (citybuttoninfo)
	{
		citybuttoninfo->release();
		citybuttoninfo = nullptr;
	}
}

string CityButtonInfo::writeToJson()
{
	map<string, string> data;
	data["storeId"] = Value(storeId).asString();
	data["url"] = url;
	data["backgrondPath"] = backgrondPath;
	data["type"] = Value(type).asString();
	data["storeName"] = storeName;
	string json = YYXLayer::getStringFormMap(data);
	return json;
}

CityButtonInfo* CityButtonInfo::getFromJson(string json)
{
	map<string, string> data;
	YYXLayer::TraversingJson(json, data);
	auto it1 = data.find("storeId");
	if (it1 != data.end())
	{
		auto storeId_str = it1->second;
		setStoreId(Value(storeId_str).asInt());
	}
	auto it2 = data.find("url");
	if (it2 != data.end())
	{
		auto url = it2->second;
		setUrl(url);
	}
	auto it3 = data.find("backgrondPath");
	if (it3 != data.end())
	{
		auto backgrondPath = it3->second;
		setBackgrondPath(backgrondPath);
	}
	auto it4 = data.find("type");
	if (it4 != data.end())
	{
		auto type_str = it4->second;
		setType(type_str == "1" ? _double : _single);
	}
	auto it5 = data.find("storeName");
	if (it5 != data.end())
	{
		auto storeName = it5->second;
		setStoreName(storeName);
	}
	return this;
}

BookStoreCache::BookStoreCache()
{
	dir = FileUtils::getInstance()->getWritablePath() + "bookCity";
	cityCachePath = dir + "/bookCity.json";
}

BookStoreCache::~BookStoreCache()
{
}

void BookStoreCache::initDir()
{
	if (!FileUtils::getInstance()->isDirectoryExist(dir))
	{
		FileUtils::getInstance()->createDirectory(dir);
	}
}

void BookStoreCache::loadData()
{
	map<string, string> vec;
	string filePath = cityCachePath;
	if (!FileUtils::getInstance()->isFileExist(cityCachePath))
		filePath = FileUtils::getInstance()->fullPathForFilename("city/bookCity.json");
	string json = YYXLayer::readFile(filePath);
	YYXLayer::TraversingJson(json, vec);
	int count = vec.size();
	if (count <= 0)
		return;
	m_sequence.clear();
	for (int i = 0; i < count; i++)
	{
		auto it = vec.find(Value(i).asString());
		if (it != vec.end())
		{
			string str = it->second;
			int storeId = Value(str).asInt();
			string storeFilePath = dir + "/" + StringUtils::format(CITYBUTTONFORMAT, storeId);
			if (!FileUtils::getInstance()->isFileExist(storeFilePath))
				storeFilePath = FileUtils::getInstance()->fullPathForFilename("city/"+ StringUtils::format(CITYBUTTONFORMAT, storeId));
			string filestr = YYXLayer::readFile(storeFilePath);
			auto cibu = CityButtonInfo::create()->getFromJson(filestr);
			cibu->setStoreId(storeId);
			m_infos[storeId] = cibu;
			m_sequence.push_back(storeId);
		}
	}
}

void BookStoreCache::addStore(int castleId, CityButtonInfo* info)
{
	m_sequence.push_back(castleId);
	m_infos[castleId] = info;
}

void BookStoreCache::writeFile()
{
	for (auto info : m_infos)
	{
		auto it = info.second;
		if (it)
		{
			int storeId = it->getStoreId();
			string json = it->writeToJson();
			YYXLayer::writeFile(json, dir + "/" + StringUtils::format(CITYBUTTONFORMAT, storeId));
		}
	}

	map<string, string> vec;
	int count = m_sequence.size();
	for (int i = 0; i < count; i++)
	{
		string key = Value(i).asString();
		string value = Value(m_sequence[i]).asString();
		vec[key] = value;
	}
	string str = YYXLayer::getStringFormMap(vec);
	YYXLayer::writeFile(str, cityCachePath);
}

void BookStoreCache::allClear()
{
	for (auto it : m_infos)
	{
		CityButtonInfo::del(it.second);
	}
	m_infos.clear();
	m_sequence.clear();
}

void BookStoreCache::clearSequence()
{
	m_infos.clear();
}

CityButtonInfo* BookStoreCache::getCityButtonInfo(int bookStoreId)
{
	CityButtonInfo* data0 = nullptr;
	auto inf0 = m_infos.find(bookStoreId);
	if (inf0 != m_infos.end())
	{
		data0 = inf0->second;
	}
	else
	{
		string storeFilePath = dir + "/" + StringUtils::format(CITYBUTTONFORMAT, bookStoreId);
		if (!FileUtils::getInstance()->isFileExist(storeFilePath))
			storeFilePath = FileUtils::getInstance()->fullPathForFilename("city/" + StringUtils::format(CITYBUTTONFORMAT, bookStoreId));
		string filestr = YYXLayer::readFile(storeFilePath);
		data0 = CityButtonInfo::create()->getFromJson(filestr);
		data0->setStoreId(bookStoreId);
		m_infos[bookStoreId] = data0;
	}
	return data0;
}

BookStoreCache* BookStoreCache::BookStoreCache::getInstance()
{
	if (instance == nullptr)
	{
		instance = new BookStoreCache();
	}
	return instance;
}
