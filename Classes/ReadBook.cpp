#include "ReadBook.h"

ReadBook* ReadBook::ReadBookinstance = nullptr;

ReadBook* ReadBook::getInstance()
{
	if (ReadBookinstance == nullptr)
	{
		ReadBookinstance = new ReadBook();
	}
	return ReadBookinstance;
}

ReadBook::ReadBook()
{
	App::log("ReadBook()");
}

ReadBook::~ReadBook()
{
	App::log("~ReadBook()");
}

void ReadBook::initDir()
{
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "readBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "readBook");
}

void ReadBook::loadCache()
{
	string cfilename = "readBook/readBook_" + App::getMemberID() + ".json";
	string path = FileUtils::getInstance()->getWritablePath() + cfilename;
	map<string, string> data;
	App::getMapFromFile(path, data);
	for (auto it : data)
	{
		int bookid = atoi(it.first.c_str());
		int time = atoi(it.second.c_str());
		if (bookid > 0)
			m_readTimes[bookid] = time;
	}
}

void ReadBook::initData()
{
	App::log("ReadBook::initData()");

}

void ReadBook::addBook(int bookid, int time)
{
	App::log("ReadBook::addBook()",bookid);
	m_readTimes[bookid] = time;
	IsSorted = true;
	map<string, string> data;
	for (auto it : m_readTimes)
	{
		string key = StringUtils::format("%d", it.first);
		data[key] = StringUtils::format("%d", it.second);
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = StringUtils::format("readBook/readBook_%d.json", App::GetInstance()->getMemberId());
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
}

void ReadBook::deleteBook(int bookid)
{
	App::log("ReadBook::deleteBook()",bookid);
	auto t = m_readTimes.find(bookid);
	if ( t != m_readTimes.end())
		m_readTimes.erase(t);
}

void ReadBook::clearBook()
{
	m_readTimes.clear();
	m_outputbooks.clear();
	m_tempbooks.clear();
}

void ReadBook::filtrate()
{
	App::log("ReadBook::filtrate()");
}

void ReadBook::sort()
{
	App::log("ReadBook::sort()");
	if (!needSort())
		return;
	auto data = App::sortMapToVector(m_readTimes);
	m_outputbooks.clear();
	for (auto it : data)
	{
		m_outputbooks.push_back(it.first);
	}
	//阅读记录限制在108条
	auto count = m_outputbooks.size();
	if (count > 144)
	{
		int delCount = count - 108;
		for (int i = 0; i < delCount; i++)
		{
			m_outputbooks.pop_back();
		}
		map<string, string> data;
		for (auto it : m_outputbooks)
		{
			string key = StringUtils::format("%d", it);
			data[key] = StringUtils::format("%d", m_readTimes[it]);
		}
		string json = YYXLayer::getStringFormMap(data);
		string filename = StringUtils::format("readBook/readBook_%d.json", App::GetInstance()->getMemberId());
		YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
	}
}

vector<int> ReadBook::getResultData()
{
	count = m_readTimes.size();
	IsSorted = false;
	return m_outputbooks;
}

bool ReadBook::needSort()
{
	if (IsSorted)
	{
		IsSorted = true;
	}
	else
	{
		if (count != m_readTimes.size())
			IsSorted = true;
		else
			IsSorted = false;
	}
	return IsSorted;
}
