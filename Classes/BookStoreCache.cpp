#include "YYXLayer.h"
#include "BookStoreCache.h"

BookStoreCache* BookStoreCache::instance = nullptr;

StorePage::StorePage()
{
}

StorePage::~StorePage()
{
}

int StorePage::getBook(int index)
{
	auto it = m_books.find(index);
	if (it != m_books.end())
	{
		int bookid = it->second;
		return bookid;
	}
	return -999;
}

void StorePage::addBook(int num, int bookId)
{
	m_books[num] = bookId;
}

std::string StorePage::getJson()
{
	map<string, string> data;
	for (auto it : m_books)
	{
		data[Value(it.first).asString()] = Value(it.second).asString();
	}
	return YYXLayer::getStringFormMap(data);
}

void StorePage::readJson(string json)
{
	map<string, string> vec;
	YYXLayer::TraversingJson(json, vec);
	int count = vec.size();
	for (int i = 0; i < count; i++)
	{
		auto it = vec.find(Value(i).asString());
		if (it != vec.end())
		{
			int bookid = Value(it->second).asInt();
			m_books[i] = bookid;
		}
	}
}

void StorePage::copy(StorePage * data)
{
	if (data)
	{
		m_books.clear();
		auto newData = data->getBooks();
		m_books.insert(newData.begin(), newData.end());
		delete data;
		data = nullptr;
	}
}

BookStoreCache::BookStoreCache()
{
	m_dir = FileUtils::getInstance()->getWritablePath() + "bookStore";
}

BookStoreCache::~BookStoreCache()
{
}

BookStoreCache * BookStoreCache::getInstance()
{
	if (instance == nullptr)
		instance = new BookStoreCache();
	return instance;
}

int BookStoreCache::getBookId(int page, int index)
{
	int bookid = -999;
	auto it = m_data.find(page);
	if (it != m_data.end())
	{
		auto pageData = it->second;
		if (pageData)
		{
			bookid = pageData->getBook(index);
		}
	}
	else
	{
		string path = getBookStorePagePath(page);
		auto pageData = new StorePage();
		pageData->setStoreId(m_storeId);
		pageData->setPageNumber(page);
		string json = YYXLayer::readFile(path);
		pageData->readJson(json);
		m_data[page] = pageData;
		bookid = pageData->getBook(index);
	}
	return bookid;
}

void BookStoreCache::initDir()
{
	if (!FileUtils::getInstance()->isDirectoryExist(m_dir))
		FileUtils::getInstance()->createDirectory(m_dir);
}

void BookStoreCache::clear()
{
	m_storeId = -999;
	m_total = 0;
	m_bookStoreDir = "";
	for (auto it : m_data)
	{
		auto sp = it.second;
		if (sp)
			delete sp;
		sp = nullptr;
	}
	m_data.clear();
}

void BookStoreCache::setStoreId(int val)
{
	m_storeId = val;
	m_bookStoreDir = m_dir + "/" + Value(val).asString();
	if (!FileUtils::getInstance()->isDirectoryExist(m_bookStoreDir))
		FileUtils::getInstance()->createDirectory(m_bookStoreDir);
	auto total = YYXLayer::getFileValue("m_total", "0", m_bookStoreDir);
	m_total = Value(total).asInt();
}

int BookStoreCache::getTotal()
{
	if (m_total <= 0) {
		auto total = YYXLayer::getFileValue("m_total", "0", m_bookStoreDir);
		m_total = Value(total).asInt();
	}
	return m_total;
}

void BookStoreCache::setTotal(int val)
{
	m_total = val;
}

void BookStoreCache::netCallback(int storeId ,int total)
{
	if (m_storeId == storeId)
	{
		m_total = total;
	}
	string dir = m_dir + "/" + Value(storeId).asString();
	if (!FileUtils::getInstance()->isDirectoryExist(dir))
		FileUtils::getInstance()->createDirectory(dir);
	YYXLayer::setFileValue("m_total", Value(total).asString(), dir);
}

void BookStoreCache::netCallBack(int storeId, int page, StorePage * pageData)
{
	string dir = m_dir + "/" + Value(storeId).asString();
	if (!FileUtils::getInstance()->isDirectoryExist(dir))
		FileUtils::getInstance()->createDirectory(dir);
	string json = pageData->getJson();
	string path = dir + "/" + Value(page).asString() + ".json";
	YYXLayer::writeFile(json, path);
	if (m_storeId == storeId)
	{
		auto it = m_data.find(page);
		if (it != m_data.end())
		{
			it->second->copy(pageData);
		}
		else
			m_data[page] = pageData;
	}
}

std::string BookStoreCache::getBookStorePagePath(int page)
{
	string path = m_bookStoreDir + "/" + Value(page).asString() + ".json";
	return path;
}