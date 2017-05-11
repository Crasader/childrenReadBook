#include "YYXRentBook.h"
#include "MyBook.h"
#include "YYXSound.h"
#include "User.h"
#include "AppHttp.h"
#include "YYXTime.h"
#include "HttpWaiting.h"

YYXRentBook* YYXRentBook::instance = nullptr;

YYXRentBook::YYXRentBook()
{

}

YYXRentBook::~YYXRentBook()
{

}


YYXRentBook* YYXRentBook::getInstance()
{
	if (instance == nullptr)
		instance = new YYXRentBook();
	return instance;
}

void YYXRentBook::newRentBook(int bookid, int memberId, const function<void()>& val)
{
	m_bookId = bookid;
	m_memberId = memberId;
	m_callback = val;
	int key = YYXTime::getInstance()->getIntNumber();
	auto waitinglayer = HttpWaiting::getInstance()->newWaitingLayer(key, [=]() {
		//Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
			RentBookMessageBox();
		//});
	});
	if (waitinglayer)
		Director::getInstance()->getRunningScene()->addChild(waitinglayer);
	AppHttp::getInstance()->httpBookInfo(bookid,[key](string name) {
		YYXRentBook::getInstance()->m_bookName = name;
		YYXLayer::sendNotify(TAG_REMOVEWAITING, "", key);
	});
}

void YYXRentBook::backgroundThreadRentBook(int bookid, int memberId, const function<void()>& val)
{
	m_bookId = bookid;
	m_memberId = memberId;
	m_callback = val;
	AppHttp::getInstance()->httpRentBook(m_bookId, [=]() {
		App::addvipBook(m_bookId);//上传租书记录成功后, 本地也需要记录租书
		MyBook::getInstance()->addBook(m_bookId, YYXLayer::getCurrentTime4Second());
		if (m_callback)
			m_callback();
	});
}

void YYXRentBook::RentBookMessageBox()
{
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "rentbook");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, "BookInfo/csb/rentbook.csb");
	auto layer = YYXLayer::create(paramter);
	auto yes = (Button*)layer->findControl("Button_2");
	auto close = (Button*)layer->findControl("Button_1");
	auto bookname = (Text*)layer->findControl("Text_6");
	auto time = (Text*)layer->findControl("Text_6_0");
	if (bookname)
	{
		bookname->setText(m_bookName);
	}
	if (time)
	{
		int tday = User::getInstance()->getVipTime() / 86400;
		time->setText(StringUtils::format("%d%s", tday, App::getString("STR_BOOINFODAY")));
	}
	if (yes)
	{
		yes->addClickEventListener([=](Ref* send) {
			YYXSound::getInstance()->playButtonSound();
			layer->removeFromParent();
			int key = YYXTime::getInstance()->getIntNumber();
			auto waitlayer = HttpWaiting::getInstance()->newWaitingLayer(key, [=]() {
				if (m_callback)
					m_callback();
			});
			if(waitlayer)
				Director::getInstance()->getRunningScene()->addChild(waitlayer);
			AppHttp::getInstance()->httpRentBook(m_bookId, [key]() {
				auto bookid = YYXRentBook::getInstance()->m_bookId;
				App::addvipBook(bookid);//上传租书记录成功后, 本地也需要记录租书
				MyBook::getInstance()->addBook(bookid, YYXTime::getInstance()->getNowTime4S());
				YYXLayer::sendNotify(TAG_REMOVEWAITING, "", key);
			});
		});
	}
	if (close)
	{
		close->addClickEventListener([=](Ref* sender) {
			YYXSound::getInstance()->playButtonSound();
			layer->removeFromParent();
		});
	}
	layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	layer->getParentNode()->setPosition(Director::getInstance()->getWinSize() / 2);
	Director::getInstance()->getRunningScene()->addChild(layer);
}

