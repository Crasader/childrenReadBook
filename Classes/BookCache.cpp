#include "BookCache.h"
#include "YYXLayer.h"
#include "YYXDownloadImages.h"
BookCache* BookCache::BookCache::instance = nullptr;

BookCache::BookCache()
{
}

BookCache::~BookCache()
{
}

BookCache * BookCache::getInstance()
{
	if (instance == nullptr)
		instance = new BookCache();
	return instance;
}

void BookCache::initDir()
{
	string dir = FileUtils::getInstance()->getWritablePath() + "book";
	if (!FileUtils::getInstance()->isDirectoryExist(dir))
		FileUtils::getInstance()->createDirectory(dir);
	m_dir = dir;

	string coverDir = FileUtils::getInstance()->getWritablePath() + "bookCover";
	if (!FileUtils::getInstance()->isDirectoryExist(dir))
		FileUtils::getInstance()->createDirectory(dir);
	m_coverDir = coverDir;
}

void BookCache::addBook(Book * book)
{
	bool del = false;
	//内存
	int bookid = book->getBookId();
	if (bookid == 637)
	{
		App::log("");
	}
	auto it = m_data.find(bookid);
	if (it != m_data.end())
	{
		auto mybook = it->second;
		if (book != mybook)
		{
			mybook->copy(book);
			del = true;
		}
	}
	else
		m_data[bookid] = book;	
	//本地硬盘
	string path = getBookPath(bookid);
	string str = book->getJson();
	if (FileUtils::getInstance()->isFileExist(path))
	{
		Book* locationbook = Book::create()->getBook(YYXLayer::readFile(path));
		locationbook->copy(book);
		str = locationbook->getJson();
		if (del)
			Book::del(book);
		Book::del(locationbook);
	}
	YYXLayer::writeFile(str, path);
}

void BookCache::delBook(int bookid)
{
	auto it = m_data.find(bookid);
	if (it != m_data.end())
	{
		m_data.erase(it);
		Book::del(it->second);
	}
}

Book * BookCache::getBook(int bookid)
{
	Book* result = nullptr;
	auto it = m_data.find(bookid);
	if (it != m_data.end())
	{
		result = it->second;
	}
	else
	{
		if (FileUtils::getInstance()->isFileExist(getBookPath(bookid)))
		{
			string json = YYXLayer::readFile(getBookPath(bookid));
			result = Book::create()->getBook(json);
			m_data[bookid] = result;
		}
	}
	return result;
}

Book * BookCache::getBookFromLocation(int bookid)
{
	Book* result = nullptr;
	if (FileUtils::getInstance()->isFileExist(getBookPath(bookid)))
	{
		string json = YYXLayer::readFile(getBookPath(bookid));
		result = Book::create()->getBook(json);
		m_data[bookid] = result;
	}
	return result;
}

std::string BookCache::getBookPath(int bookid)
{
	string path = m_dir + "/" + StringUtils::format("%d.json", bookid);
	return path;
}

void BookCache::clear()
{
	for (auto it : m_data)
	{
		auto de = it.second;
		if (de)
			Book::del(de);
	}
	m_data.clear();
}

string BookCache::getBookCoverPath(int bookid)
{
	string dir = FileUtils::getInstance()->getWritablePath() + "bookCover";
	std::string fileName = StringUtils::format("%d", bookid) + ".png";
	string bookCoverPath = dir + "/" + fileName;
	return bookCoverPath;
}


Book::Book()
{

}

Book::Book(int bookid)
{
	setBookId(bookid);
}

Book::~Book()
{

}

Book* Book::create()
{
	Book* book = new Book();
	return book;
}

void Book::del(Book* book)
{
	if (book)
		delete book;
	book = nullptr;
}

void Book::copy(Book * book)
{
	auto bookid = book->getBookId();
	auto coverurl = book->getCoverURL();
	auto bookAuthor = book->getBookAuthor();
	auto bookmarketPrice = book->getBookmarketPrice();
	auto bookPlayUrl = book->getBookPlayUrl();
	auto bookPress = book->getBookPress();
	auto bookPrice = book->getBookPrice();
	auto hashBuy = book->getHashBuy();
	auto content = book->getContent();
	auto name = book->getName();
	auto page = book->getBookPage();
	auto score = book->getScore();
	if (bookid == m_bookId)
	{
		if (!coverurl.empty())
			m_coverURL = coverurl;
		if (!bookAuthor.empty())
			m_bookAuthor = bookAuthor;
		if (bookmarketPrice >= 0)
			m_bookmarketPrice = bookmarketPrice;
		if (!bookPlayUrl.empty())
			m_bookPlayUrl = bookPlayUrl;
		if (!bookPress.empty())
			m_bookPress = bookPress;
		if (bookPrice >=0)
			m_bookPrice = bookPrice;
		if (hashBuy >=0)
			m_hashBuy = hashBuy;
		if (!content.empty())
			m_content = content;
		if (!name.empty())
			m_name = name;
		if (page >= 0)
			m_bookPage = page;
		if (score >= 0 && score <= 5)
			m_score = score;
	}
}

Book * Book::setCoverURL(std::string val)
{
	m_coverURL = val;
	return this;
}

string Book::getJson()
{
	map <string, string> data;
	data["m_bookId"] = Value(m_bookId).asString();
	data["m_coverURL"] = m_coverURL;
	data["m_bookmarketPrice"] = Value(m_bookmarketPrice).asString();
	data["m_bookPlayUrl"] = m_bookPlayUrl;
	data["m_bookPress"] = m_bookPress;
	data["m_bookPrice"] = Value(m_bookPrice).asString();
	data["m_content"] = m_content;
	data["m_hashBuy"] = Value(m_hashBuy).asString();
	data["m_name"] = m_name;
	data["m_bookPage"] = Value(m_bookPage).asString();
	data["m_score"] = Value(m_score).asString();
	return YYXLayer::getStringFormMap(data);
}

Book * Book::getBook(string json)
{
	map<string, string> data;
	YYXLayer::TraversingJson(json, data);
	auto bookid = data.find("m_bookId");
	if (bookid != data.end())
		m_bookId = Value(bookid->second).asInt();

	auto coverURL = data.find("m_coverURL");
	if (coverURL != data.end())
		m_coverURL = Value(coverURL->second).asString();

	auto bookmarketPrice = data.find("m_bookmarketPrice");
	if (bookmarketPrice != data.end())
		m_bookmarketPrice = Value(bookmarketPrice->second).asInt();

	auto bookPlayUrl = data.find("m_bookPlayUrl");
	if (bookPlayUrl != data.end())
		m_bookPlayUrl = Value(bookPlayUrl->second).asInt();

	auto bookPress = data.find("m_bookPress");
	if (bookPress != data.end())
		m_bookPress = Value(bookPress->second).asString();

	auto bookPrice = data.find("m_bookPrice");
	if (bookPrice != data.end())
		m_bookPrice = Value(bookPrice->second).asInt();

	auto content = data.find("m_content");
	if (content != data.end())
		m_content = Value(content->second).asString();

	auto hashBuy = data.find("m_hashBuy");
	if (hashBuy != data.end())
		m_hashBuy = Value(hashBuy->second).asInt();

	auto name = data.find("m_name");
	if (name != data.end())
		m_name = Value(name->second).asString();

	auto bookPage = data.find("m_bookPage");
	if (bookPage != data.end())
		m_bookPage = Value(bookPage->second).asInt();
	
	auto score = data.find("m_score");
	if (score != data.end())
		m_score = Value(score->second).asInt();
	return this;
}
