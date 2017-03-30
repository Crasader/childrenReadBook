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
	void downloadHead_CircleImage();

	int getChildrenId() const { return childrenId; }
	void setChildrenId(int val) { childrenId = val; }
	int getChildrenSex() const { return childrenSex; }
	void setChildrenSex(int val) { childrenSex = val; }
	std::string getChildrenName() const { return childrenName; }
	void setChildrenName(std::string val) { childrenName = val; }
	std::string getChildrenBirth() const { return childrenBirth; }
	void setChildrenBirth(std::string val) { childrenBirth = val; }
	std::string getUrl() const { return url; }
	void setUrl(std::string val) { url = val; }
	std::string getPath() const { return path; }
	void setPath(std::string val) { path = val; }
private:
	int childrenId = -999;
	int childrenSex = -999;
	string childrenName = "";
	string childrenBirth = "";
	string url = "";
	string path = "";//转成圆形头像
};

class User
{
public:
	User();
	~User();
	static User* getInstance();
	void initData(int memberId);
	void httpVipAndBuyBooks();//网络获取租书和买书列表
	void httpHeadImage(const function<void()>& callback=nullptr);//网络获取当前孩子的头像

	void addChild(ChildInfo*);
	ChildInfo* getChild(int childId);
	void clearChildren();

	int getChildId() const { return childId; }
	void setChildId(int val) { childId = val; }
	int getMemberId() const { return memberId; }
	void setMemberId(int val) { memberId = val; }
private:
	static User* instance;
	int memberId = -999;
	int childId = -999;
	//int money;
	string resource="android";
	//bool vip;
	//string startTime;
	//string endTime;
	std::unordered_map<int, ChildInfo*> children;
};

#endif