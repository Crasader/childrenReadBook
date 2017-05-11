#include "User.h"
#include "FileNames.h"
#include "CrossPlatform.h"
#include "MyBook.h"
#include "YYXDownloadImages.h"
#include "DownloadBook.h"
#include "ReadBook.h"
#include "BookCache.h"
User* User::instance = nullptr;

User::User()
{	
}

User::~User()
{

}

User* User::getInstance()
{
	if (instance == nullptr)
	{
		instance = new User();
	}
	return instance;
}

void User::deleteInstance()
{
	if (instance)
	{
		instance->clearChildren();
		delete instance;
		instance = nullptr;
	}
}

void User::addChild(ChildInfo* child)
{
	m_Mchildren.lock();
	if (child)
	{
		auto childId = child->getChildrenId();
		auto it = children.find(childId);
		if (it != children.end())
		{
			auto _child = it->second;
			if (child != _child)
			{
				auto childrenBirth = child->getChildrenBirth();
				if(!childrenBirth.empty())
					_child->setChildrenBirth(childrenBirth);

				auto childrenName = child->getChildrenName();
				if (!childrenName.empty())
					_child->setChildrenName(childrenName);

				auto childrenSex = child->getChildrenSex();
				if (childrenSex != -999)
					_child->setChildrenSex(childrenSex);

				auto url = child->getUrl();
				if (!url.empty())
					_child->setUrl(url);
				ChildInfo::del(child);
			}
		}
		else
			children[childId] = child;
	}
	m_Mchildren.unlock();
}

void User::delChild(int childId)
{
	m_Mchildren.lock();
	auto it = children.find(childId);
	if ( it != children.end())
	{
		auto child = it->second;
		children.erase(it);
		ChildInfo::del(child);
	}
	m_Mchildren.unlock();
}

ChildInfo * User::getChild(int childId)
{
	m_Mchildren.lock();
	ChildInfo* child = nullptr;
	auto it = children.find(childId);
	if (it != children.end())
	{
		child = it->second;
	}
	else
	{
		child = ChildInfo::create();
		child->setChildrenId(childId);		
	}
	m_Mchildren.unlock();
	return child;
}

void User::clearChildren()
{
	m_Mchildren.lock();
	for (auto it : children)
	{
		auto child = it.second;
		ChildInfo::del(child);
	}
	children.clear();
	m_Mchildren.unlock();
}

int User::getChildId()
{
	if (childId <= 0)
	{
		childId =Value( YYXLayer::getFileValue("childId", "-999", dir)).asInt();
	}
	return childId;
}

void User::setChildId(int val)
{
	childId = val;
	assert(childId != 0);
	YYXLayer::setFileValue("childId", Value(childId).asString(), dir);
}

int User::getMemberId()
{
	if (memberId < 0)
	{
		memberId = Value(YYXLayer::getFileValue("userId", "-999", dir)).asInt();
	}
	return memberId;
}

void User::setMemberId(int val)
{
	memberId = val;
	YYXLayer::setFileValue("userId", Value(memberId).asString(), dir);
}

std::string User::getUserAccount()
{
	if (userAccount.length()  != 11)
	{
		userAccount = YYXLayer::getFileValue("userAccount", "***********", dir);
	}
	return userAccount;
}

User * User::setUserAccount(std::string val)
{
	userAccount = val;
	YYXLayer::setFileValue("userAccount", userAccount,dir);
	return this;
}

int User::getSex()
{
	if (sex < 0)
	{
		sex = Value(YYXLayer::getFileValue("userSex", "0", dir)).asInt();
	}
	return sex;
}

User * User::setSex(int val)
{
	sex = val;
	YYXLayer::setFileValue("userSex", Value(sex).asString(),dir);
	return this;
}

std::string User::getUserCity()
{
	if (userCity.empty())
	{
		userCity = YYXLayer::getFileValue("userCity", App::getString("BEIJING"), dir);
	}
	return userCity;
}

User* User::setUserCity(std::string val)
{
	userCity = val;
	YYXLayer::setFileValue("userCity", userCity, dir);
	return this;
}

std::string User::getUserProvince()
{
	if (userProvince.empty())
	{
		userProvince = YYXLayer::getFileValue("userProvince", App::getString("BEIJING"), dir);
	}
	return userProvince;
}

User* User::setUserProvince(std::string val)
{
	userProvince = val;
	YYXLayer::setFileValue("userProvince", userProvince, dir);
	return this;
}

int User::getMoney()
{
	if (money < 0)
	{
		money = Value(YYXLayer::getFileValue("userBalance", "0", dir)).asInt();
	}
	return money;
}

User* User::setMoney(int val)
{
	money = val;
	YYXLayer::setFileValue("userBalance", Value(money).asString(), dir);
	return this;
}

bool User::getVip()
{
	if(vip < 0)
	{
		vip = Value(YYXLayer::getFileValue("userVip", "0", dir)).asInt();
	}
	if (vip == 1)
		return true;
	else
		return false;
}

User * User::setVip(bool val)
{
	if (val)
		vip = 1;
	else
		vip = 0;
	YYXLayer::setFileValue("userVip", Value(vip).asString(), dir);
	return this;
}

std::string User::getStartTime()
{
	if (startTime.empty())
	{
		startTime = YYXLayer::getFileValue("startTime", "----", dir);
	}
	return startTime;
}

User * User::setStartTime(std::string val)
{
	startTime = val;
	YYXLayer::setFileValue("startTime", startTime, dir);
	return this;
}

std::string User::getEndTime()
{
	if (endTime.empty())
	{
		endTime = YYXLayer::getFileValue("endTime", "----", dir);
	}
	return endTime;
}

User* User::setEndTime(std::string val)
{
	endTime = val;
	YYXLayer::setFileValue("endTime", endTime, dir);
	return this;
}

int User::getVipTime()
{
	if (vipTime < 0)
	{
		vipTime = Value(YYXLayer::getFileValue("vipTime", "0", dir)).asInt();
	}
	return vipTime;
}

User* User::setVipTime(int val)
{
	vipTime = val;
	YYXLayer::setFileValue("vipTime", Value(vipTime).asString(), dir);
	return this;
}

void User::initDir()
{
	if (!FileUtils::getInstance()->isFileExist(dir))
		FileUtils::getInstance()->createDirectory(dir);
}

std::vector<int> User::getAllChildId()
{
	m_Mchildren.lock();
	vector<int > data;
	for (auto it : children)
	{
		data.push_back(it.first);
	}
	m_Mchildren.unlock();
	return data;
}

ChildInfo::ChildInfo()
{

}

ChildInfo::~ChildInfo()
{
}

ChildInfo * ChildInfo::create()
{
	ChildInfo* child = nullptr;
	child = new ChildInfo();
	child->autorelease();
	child->retain();
	return child;
}

void ChildInfo::del(ChildInfo * child)
{
	if (child)
	{
		child->release();
		child = nullptr;
	}
}

std::string ChildInfo::getChildImageFileName(int childid)
{
	string fileName = StringUtils::format("%d.png", childid);
	return fileName;
}

void ChildInfo::writeFile()
{
	setChildrenId(childrenId);
	setChildrenName(childrenName);
	setChildrenBirth(childrenBirth);
	setChildrenSex(childrenSex);
	setUrl(url);
	setPath(path);
}

ChildInfo* ChildInfo::setChildrenId(int val)
{
	childrenId = val;
	if (val < 0)
		return this;
	dir = User::getInstance()->getDir() + "/" + Value(childrenId).asString();
	YYXLayer::setFileValue("childrenId", Value(childrenId).asString(), dir);
	return this;
}

int ChildInfo::getChildrenSex()
{
	if (childrenSex < 0)
	{
		childrenSex = Value(YYXLayer::getFileValue("childrenSex", "0", dir)).asInt();
	}
	return childrenSex;
}

ChildInfo* ChildInfo::setChildrenSex(int val)
{
	childrenSex = val;
	if(!dir.empty())
		YYXLayer::setFileValue("childrenSex", Value(childrenSex).asString(), dir);
	return this;
}

std::string ChildInfo::getChildrenName()
{
	if (childrenName.empty())
	{
		childrenName = YYXLayer::getFileValue("childrenName", "*****", dir);
	}
	return childrenName;
}

ChildInfo* ChildInfo::setChildrenName(std::string val)
{
	childrenName = val;
	if (!dir.empty())
		YYXLayer::setFileValue("childrenName", childrenName, dir);
	return this;
}

std::string ChildInfo::getChildrenBirth()
{
	if (childrenBirth.empty())
	{
		childrenBirth = YYXLayer::getFileValue("childrenBirth", "------", dir);
	}
	return childrenBirth;
}

ChildInfo* ChildInfo::setChildrenBirth(std::string val)
{
	childrenBirth = val;
	if (!dir.empty())
		YYXLayer::setFileValue("childrenBirth", childrenBirth, dir);
	return this;
}

std::string ChildInfo::getUrl()
{
	if (url.empty())
	{
		url = YYXLayer::getFileValue("url", "", dir);
	}
	return url;
}

ChildInfo* ChildInfo::setUrl(std::string val)
{
	url = val;
	if (!dir.empty())
		YYXLayer::setFileValue("url", url, dir);
	return this;
}

std::string ChildInfo::getPath()
{
	if (path.empty())
	{
		path = YYXLayer::getFileValue("path", "", dir);
	}
	return path;
}

ChildInfo* ChildInfo::setPath(std::string val)
{
	path = val;
	if (!dir.empty())
		YYXLayer::setFileValue("path", path, dir);
	return this;
}