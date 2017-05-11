#ifndef _BOOKCITYCACHE_H__
#define _BOOKCITYCACHE_H__

#include "cocos2d.h"
#include <vector>
using namespace std;
USING_NS_CC;

enum CityButtonType
{
	_single,//单一按钮
	_double//双按钮
};

class CityButtonInfo : public Ref
{
public:
	CityButtonInfo();
	~CityButtonInfo();

	static CityButtonInfo* create();
	static void del(CityButtonInfo* citybuttoninfo);

	string writeToJson();
	CityButtonInfo* getFromJson(string json);

	int getStoreId() const { return storeId; }
	CityButtonInfo* setStoreId(int val) {
		storeId = val;
		return this;
	}

	std::string getUrl() const { return url; }
	CityButtonInfo* setUrl(std::string val) {
		url = val;
		return this;
	}

	std::string getBackgrondPath() const { return backgrondPath; }
	CityButtonInfo* setBackgrondPath(std::string val) {
		backgrondPath = val;
		return this;
	}

	CityButtonType getType() const { return type; }
	CityButtonInfo* setType(CityButtonType val) {
		type = val;
		return this;
	}

	std::string getStoreName() const { return storeName; }
	CityButtonInfo* setStoreName(std::string val) {
		storeName = val;
		return this;
	}

private:
	int storeId = -999;
	string url = "";
	string backgrondPath = "";
	CityButtonType type = _double;
	string storeName = "";
};

class BookCityCache
{
public:
	BookCityCache();
	~BookCityCache();
	static BookCityCache* getInstance();
	void initDir();
	void loadData();
	void addStore(int ,CityButtonInfo*);
	void writeFile();
	void allClear();
	void clearSequence();//清除顺序
	CityButtonInfo* getCityButtonInfo(int bookStoreId);

	std::vector<int> getSequence() const { return m_sequence; }
	void setSequence(std::vector<int> val) { m_sequence = val; }
	std::map<int, CityButtonInfo *> getInfos() const { return m_infos; }
	void setInfos(std::map<int, CityButtonInfo *> val) { m_infos = val; }
private:
	static BookCityCache* instance;
	string dir = "";
	string cityCachePath = "";
	vector<int> m_sequence;
	map<int, CityButtonInfo*> m_infos;
};
#endif