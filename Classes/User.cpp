#include "User.h"
#include "FileNames.h"
#include "NetIntface.h"
#include "BuyBook.h"
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

void User::initData(int memberId)
{
	this->memberId = memberId;
}

void User::httpVipAndBuyBooks()
{
	string url = string(IP) + NET_USERBUYANDRENTBOOKS + "?memberId=" +StringUtils::format("%d", memberId)+ "&resource=" + resource;
	NetIntface::httpGet(url, "", [=](string json) {
		rapidjson::Document doc;
		auto result = YYXLayer::getJsonObject4Json(doc, json);
		if (result)
		{
			App::GetInstance()->myBookURLMap.clear();
			App::GetInstance()->VIPbook.clear();
			App::GetInstance()->myBuyBook.clear();
			BuyBook::getInstance()->clearBook();
			rapidjson::Value array;
			YYXLayer::getJsonArray4Json(array, doc, "data");
			YYXLayer::getDataForJsonArray(array, [](rapidjson::Value& item, int idx) {
				auto bookId = String(YYXLayer::getString4Json("0", item, "bookId")).intValue();
				auto bookName = YYXLayer::getString4Json("", item, "bookName");
				auto bookPlayUrl = YYXLayer::getString4Json("", item, "bookPlayUrl");
				auto bookCoverUrl = YYXLayer::getString4Json("", item, "bookCoverUrl");
				auto bookRelationState = String(YYXLayer::getString4Json("1", item, "bookRelationState")).intValue();//1代表买书 2代表租书
				auto isNewEllaBook = String(YYXLayer::getString4Json("0", item, "isNewEllaBook")).intValue();//是否新书 0不是 1是
				auto orderId = String(YYXLayer::getString4Json("0", item, "orderId")).intValue();
				auto time = String(YYXLayer::getString4Json("0", item, "time")).intValue();
				BuyBook::getInstance()->addBook(bookId, time, false);
				App::GetInstance()->myBookURLMap[bookId] = bookPlayUrl;
				if (bookRelationState == 1)
				{
					App::GetInstance()->myBuyBook[bookId] = time;
				}
				else
				{
					App::GetInstance()->VIPbook[bookId] = time;
				}
				BookCache::getInstance()->addBook(Book::create()->setCoverURL(bookCoverUrl)->setBookId(bookId));
			});
			thread([]() {	
				BuyBook::getInstance()->writeDownXml();
			}).detach();
			YYXLayer::sendNotify(TAG_BOOKROOMBOOKISEXIT);
		}
	}, "", [](string err) {});
}

void User::httpHeadImage(const function<void()>& callback)
{
	string url = std::string(IP).append(NET_CHILD_DETAILS);
	map<string, string>  parameter;
	parameter["memberId"] = StringUtils::format("%d", memberId);
	parameter["resource"] = resource;
	NetIntface::httpPost(url, parameter, "", [=](string json) {
		rapidjson::Document doc;
		YYXLayer::getJsonObject4Json(doc, json);
		auto code = YYXLayer::getIntForJson(-999, doc, "code");
		if (code == 0)
		{
			rapidjson::Value arrayData;
			YYXLayer::getJsonArray4Json(arrayData, doc, "data");
			for (rapidjson::SizeType i = 0; i < arrayData.Size(); i++)
			{
				rapidjson::Value & item = arrayData[i];
				auto childrenId = YYXLayer::getInt4Json(-999, item, "childrenId");
				auto childrenSex = YYXLayer::getInt4Json(-999, item, "childrenSex");
				auto childrenName = YYXLayer::getString4Json("", item, "childrenName");
				auto childrenBirth = YYXLayer::getString4Json("", item, "childrenBirth");
				auto url = YYXLayer::getString4Json("", item, "avatarUrl");
				ChildInfo* child = ChildInfo::create();
				child->setChildrenId(childrenId);
				child->setChildrenBirth(childrenBirth);
				child->setChildrenName(childrenName);
				child->setChildrenSex(childrenSex);
				child->setUrl(url);
				User::getInstance()->addChild(child);
				if (User::getInstance()->getChildId() <=0 && i==0)
				{
					User::getInstance()->setChildId(childrenId);
				}
			}
			if (callback)
				callback();
		}
	}, "", [](string error) {});
}

void User::addChild(ChildInfo* child)
{
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
}

ChildInfo * User::getChild(int childId)
{
	ChildInfo* child = nullptr;
	auto it = children.find(childId);
	if (it != children.end())
	{
		child = it->second;
	}
	return child;
}

void User::clearChildren()
{
	for (auto it : children)
	{
		auto child = it.second;
		ChildInfo::del(child);
	}
	children.clear();
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

void ChildInfo::downloadHead_CircleImage()
{
	YYXDownloadImages::GetInstance()->newDownloadImage(url, FileUtils::getInstance()->getWritablePath()+"temp"
		, StringUtils::format("%I64d.png",YYXLayer::getRandom()), high, 0, [=](string downpath) {
		string savepath = FileUtils::getInstance()->getWritablePath() + "temp/" + StringUtils::format("%I64d.png", YYXLayer::getRandom());
		NetIntface::cutTheRounded(downpath, savepath, 600, 600, "", [=](string _savepath) {
			setPath(_savepath);
			YYXLayer::setFileValue(StringUtils::format("%d_imagepath", childrenId), _savepath);
			YYXLayer::sendNotify(TAG_CHILDHEADDOWNLOADOVER, "", childrenId);
		}, "", [](string err) {});
	}, [](string error) {
	});
}
