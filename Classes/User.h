#ifndef __USER_H__
#define __USER_H__

#include <string>
#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class ChildInfo : public Ref
{
public:
	ChildInfo();
	~ChildInfo();
	static ChildInfo* create();
	static void del(ChildInfo*);
	static string getChildImageFileName(int childid);
	void writeFile();

	int getChildrenId() const { return childrenId; }
	ChildInfo* setChildrenId(int val);

	int getChildrenSex();
	ChildInfo* setChildrenSex(int val);

	std::string getChildrenName();
	ChildInfo* setChildrenName(std::string val);

	std::string getChildrenBirth();
	ChildInfo* setChildrenBirth(std::string val);

	std::string getUrl();
	ChildInfo* setUrl(std::string val);

	std::string getPath();
	ChildInfo* setPath(std::string val);

	std::string getDir() const { return dir; }
private:
	int childrenId = -999;
	int childrenSex = -999;
	string childrenName = "";
	string childrenBirth = "";
	string url = "";
	string path = "";//转成圆形头像
	string dir = "";
};

class User
{
public:
	User();
	~User();
	static User* getInstance();
	static void deleteInstance();

	void addChild(ChildInfo*);
	void delChild(int childId);
	ChildInfo* getChild(int childId);
	void clearChildren();

	int getChildId();
	void setChildId(int val);

	int getMemberId();
	void setMemberId(int val);

	std::string getUserAccount();
	User* setUserAccount(std::string val);

	int getSex();
	User* setSex(int val);

	std::string getUserCity();
	User* setUserCity(std::string val);

	std::string getUserProvince();
	User* setUserProvince(std::string val);

	int getMoney();
	User* setMoney(int val);

	bool getVip();
	User* setVip(bool val);

	std::string getStartTime();
	User* setStartTime(std::string val);

	std::string getEndTime();
	User* setEndTime(std::string val);

	int getVipTime();
	User* setVipTime(int val);

	void initDir();
	std::string getDir() const { return dir; }

	std::unordered_map<int, ChildInfo *> getChildren() const { return children; }
private:
	static User* instance;
	int memberId = -999;
	int childId = -999;
	string userAccount = "*******";
	int sex = -999;
	string userCity = "";
	string userProvince = "";
	int money = -999;//分
	int vip = -999;//0=false 1= true
	string startTime = "";
	string endTime = "";
	int vipTime = -999;
	std::unordered_map<int, ChildInfo*> children;
	string dir = "";//本地存储的文件夹路径
};

#endif