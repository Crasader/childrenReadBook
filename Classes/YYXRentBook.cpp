#include "YYXRentBook.h"

YYXRentBook* YYXRentBook::instance = nullptr;

void YYXRentBook::httpRentBook()
{
	string url = string(IP).append(NET_RENT);
	map<string, string> par;
	par["memberId"] =StringUtils::format("%d",m_memberId);
	par["bookId"] = StringUtils::format("%d", m_bookId);
	par["resource"] = App::m_resource;
	par["payType"] = "2";
	NetIntface::httpPost(url, par, "rentSuccess", [=](string json) {
		rapidjson::Document doc;
		bool rent = false;
		if (YYXLayer::getJsonObject4Json(doc, json))
		{
			auto code = YYXLayer::getIntForJson(-1, doc, "code");
			if (code == 0 || code == 4)
				rent = true;
		}
		if (rent)
		{
			App::addvipBook(m_bookId);//上传租书记录成功后, 本地也需要记录租书
			App::log("============httpRentBook>>>>addvipBook");
			if (m_callback)
				m_callback();
		}
		else
			Toast::create(App::getString("ZUSHUSHIBAI"),false);
	}, "rentFail", [](string str) {
		Toast::create(App::getString("NETEXCEPTION"),false);
	});
}

void YYXRentBook::httpBookInfo()
{
	NetIntface::httpGetBookInfo(m_bookId, "", [=](string json) {
		NetIntface::httpGetBookInfoCallBack(json, [=](float avgScore, bool isvip, int bookId, int bookPage, int bookPrice100, int bookmarketPrice100, int remainTime, string bookAuthor,
			string bookSize, string bookPress, string bookIntroduction, string bookName, string bookPlayUrl, string bookCoverUrl, string bookViewUrl) {
			//解析正常
			m_bookName = bookName;
			//书籍信息 原价+书籍名称+ 书页数
			string BookNameKey = StringUtils::format("bookName+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookNameKey, bookmarketPrice100, bookName, (Ref*)bookPage);
			//现价+封面url+新书标记
			string BookPriceKey = StringUtils::format("bookPrice+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPriceKey, bookPrice100, bookCoverUrl);
			//购书标记+下载URL + 	是否是vip书籍
			string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", bookId);
			if (isvip)
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, -999, bookPlayUrl, (Ref*)1);
			else
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPlayUrlKey, -999, bookPlayUrl, (Ref*)0);
			//活动倒计时 + 书籍作者
			string BookAuthorKey = StringUtils::format("bookAuthor+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookAuthorKey, remainTime, bookAuthor);
			//出版社 + 书籍星级
			string BookPressKey = StringUtils::format("bookPress+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookPressKey, avgScore, bookPress);
			//书籍资源大小
			string BookSizeKey = StringUtils::format("bookSize+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, BookSizeKey, -999, bookSize);
			//书籍介绍
			string bookIntroductionKey = StringUtils::format("bookIntroduction+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookIntroductionKey, -999, bookIntroduction);
			//试读url
			string bookViewUrlKey = StringUtils::format("bookViewUrl+bookID=%d", bookId);
			YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookViewUrlKey, -999, bookViewUrl);
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
				RentBookMessageBox();
			});
		}, []() {
			//解析错误
			Toast::create(App::getString("HUOQUSHUJIXIANGQINGYICHANG"),false);
		});
	}, "", [](string str) {
		Toast::create(App::getString("NETEXCEPTION"),false);
	});
}

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
	httpBookInfo();
}

void YYXRentBook::backgroundThreadRentBook(int bookid, int memberId, const function<void()>& val)
{
	m_bookId = bookid;
	m_memberId = memberId;
	m_callback = val;
	httpRentBook();
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
		int tday = App::GetInstance()->m_me->vipTime / 86400;
		time->setText(StringUtils::format("%d%s", tday, App::getString("STR_BOOINFODAY")));
	}
	if (yes)
	{
		yes->addClickEventListener([=](Ref* send) {
			layer->removeFromParent();
			httpRentBook();
		});
	}
	if (close)
	{
		close->addClickEventListener([=](Ref* sender) {
			layer->removeFromParent();
		});
	}
	layer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	layer->getParentNode()->setPosition(Director::getInstance()->getWinSize() / 2);
	Director::getInstance()->getRunningScene()->addChild(layer);
}

