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
	int bookid = book->getBookId();
	auto it = m_data.find(bookid);
	if (it != m_data.end())
	{
		auto mybook = it->second;
		if (book != mybook)
		{
			mybook->copy(book);
			Book::del(book);
		}
	}
	else
		m_data[bookid] = book;	
	YYXLayer::writeFile(book->getJson(), getBookPath(bookid));
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
	Book* book = nullptr;
	book = new Book();
	if (book)
	{
		book->autorelease();
		book->retain();
	}
	return book;
}

void Book::del(Book* book)
{
	if (book)
		book->release();
	book = nullptr;
}

void Book::copy(Book * book)
{
	auto bookid = book->getBookId();
	auto coverurl = book->getCoverURL();
	if (bookid == m_bookId)
	{
		if (coverurl != "")
		{
			m_coverURL = coverurl;
		}
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
	data["m_bookId"] = StringUtils::format("%d", m_bookId);
	data["m_coverURL"] = m_coverURL;
	return YYXLayer::getStringFormMap(data);
}

Book * Book::getBook(string json)
{
	map<string, string> data;
	YYXLayer::TraversingJson(json, data);
	auto bookid = data.find("m_bookId");
	if (bookid != data.end())
	{
		m_bookId = Value(bookid->second).asInt();
	}
	auto coverURL = data.find("m_coverURL");
	if (coverURL != data.end())
	{
		m_coverURL = Value(coverURL->second).asString();
	}
	return this;
}
