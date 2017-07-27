#include "BorrowBook.h"

BorrowBook* BorrowBook::BorrowBookinstance = nullptr;

BorrowBook* BorrowBook::getInstance()
{
	if (BorrowBookinstance == nullptr)
	{
		BorrowBookinstance = new BorrowBook();
	}
	return BorrowBookinstance;
}


void BorrowBook::destoryInstance()
{
	if (BorrowBookinstance)
	{
		delete BorrowBookinstance;
	}
	BorrowBookinstance = nullptr;
}

BorrowBook::BorrowBook()
{
	App::log("BorrowBookinstance()");
}

BorrowBook::~BorrowBook()
{
	App::log("~BorrowBookinstance()");
}

void BorrowBook::initDir()
{
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "BorrowBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "BorrowBook");
}

void BorrowBook::loadCache()
{
	string cfilename = "BorrowBook/BorrowBook_" + App::getMemberID() + ".json";
	string path = FileUtils::getInstance()->getWritablePath() + cfilename;
	map<string, string> data;
	App::getMapFromFile(path, data);
	for (auto it : data)
	{
		int bookid = atoi(it.first.c_str());
		int lock = atoi(it.second.c_str());
		if (bookid > 0)
			m_borrowbooks[bookid] = lock;
	}
}

void BorrowBook::initData()
{
	App::log("BorrowBook::initData()");
	if(m_borrowbooks.empty())
		loadCache();
}

void BorrowBook::addBook(int bookid, int lock, bool write)
{
	m_borrowbooks[bookid] = lock;
	auto its = m_borrowbooks.find(bookid);
	if (its != m_borrowbooks.end())
	{
		if (lock > its->second)
		{
			m_borrowbooks[bookid] = lock;
		}
	}
	else
		m_borrowbooks[bookid] = lock;
	if (write)
	{
		writeDownXml();
	}
}

void BorrowBook::deleteBook(int bookid)
{
	App::log("BorrowBook::deleteBook()",bookid);
	auto it = m_borrowbooks.find(bookid);
	if (it != m_borrowbooks.end())
	{
		m_borrowbooks.erase(it);
	}
}

void BorrowBook::writeDownXml()
{
	map<string, string> data;
	for (auto it : m_borrowbooks)
	{
		string key = StringUtils::format("%d", it.first);
		data[key] = StringUtils::format("%d", it.second);
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = StringUtils::format("BorrowBook/BorrowBook_%d.json", App::GetInstance()->getMemberId());
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
}

void BorrowBook::clearBook()
{
	m_borrowbooks.clear();
}