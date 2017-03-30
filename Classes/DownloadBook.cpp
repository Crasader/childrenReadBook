#include "DownloadBook.h"

DownloadBook* DownloadBook::DownloadBookinstance = nullptr;

DownloadBook* DownloadBook::getInstance()
{
	if (DownloadBookinstance == nullptr)
	{
		DownloadBookinstance = new DownloadBook();
	}
	return DownloadBookinstance;
}

DownloadBook::DownloadBook()
{
	App::log("DownloadBook()");
}

DownloadBook::~DownloadBook()
{
	App::log("~DownloadBook()");
}

void DownloadBook::initDir()
{
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "downloadBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "downloadBook");
}

void DownloadBook::loadCache()
{
	string cfilename = "downloadBook/downloadbook_" + App::getMemberID() + ".json";
	string path = FileUtils::getInstance()->getWritablePath() + cfilename;
	map<string, string> data;
	App::getMapFromFile(path, data);
	for (auto it : data)
	{
		int bookid = atoi(it.first.c_str());
		int time = atoi(it.second.c_str());
		if (bookid > 0)
			m_downloadTimes[bookid] = time;
	}
}

void DownloadBook::initData()
{
	App::log("DownloadBook::initData()");
	if (!needSort())
		return;
	//购买的书 + 年卡的书
	m_inputbooks.insert(App::GetInstance()->myBuyBook.begin(), App::GetInstance()->myBuyBook.end());
	m_inputbooks.insert(App::GetInstance()->VIPbook.begin(), App::GetInstance()->VIPbook.end());
}

void DownloadBook::addBook(int bookid, int time)
{
	App::log("DownloadBook::addBook()",bookid);
	m_downloadTimes[bookid] = time;
	IsSorted = true;
	map<string, string> data;
	for (auto it : m_downloadTimes)
	{
		string key = StringUtils::format("%d", it.first);
		data[key] = StringUtils::format("%d", it.second);
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = StringUtils::format("downloadBook/downloadbook_%d.json", App::GetInstance()->getMemberId());
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
}

void DownloadBook::deleteBook(int bookid)
{
	App::log("DownloadBook::deleteBook()",bookid);
	auto it = m_inputbooks.find(bookid);
	if (it != m_inputbooks.end())
	{
		m_inputbooks.erase(it);
	}
	auto t = m_downloadTimes.find(bookid);
	if ( t != m_downloadTimes.end())
		m_downloadTimes.erase(t);
}

void DownloadBook::clearBook()
{
	m_inputbooks.clear();
	m_downloadTimes.clear();
	m_outputbooks.clear();
	m_tempbooks.clear();
}

void DownloadBook::filtrate()
{
	App::log("DownloadBook::filtrate()");
	if (!needSort())
		return;
	m_tempbooks.clear();
	for (auto it : m_inputbooks)
	{
		auto bookid = it.first;
		auto path = App::getBookRead4Json_txtPath(bookid);
		if (FileUtils::getInstance()->isFileExist(path))
		{
			int time = 0;
			auto she = m_downloadTimes.find(bookid);
			if (she != m_downloadTimes.end())
			{
				time = she->second;
			}
			m_tempbooks[bookid] = time;
		}
	}
}

void DownloadBook::sort()
{
	App::log("DownloadBook::sort()");
	if (!needSort())
		return;
	auto data = App::sortMapToVector(m_tempbooks);
	m_outputbooks.clear();
	for (auto it : data)
	{
		m_outputbooks.push_back(it.first);
	}
}

vector<int> DownloadBook::getResultData()
{
	count = m_downloadTimes.size();
	IsSorted = false;
	return m_outputbooks;
}

bool DownloadBook::needSort()
{
	if (IsSorted)
	{
		IsSorted = true;
	}
	else
	{
		if (count != m_downloadTimes.size())
			IsSorted = true;
		else
			IsSorted = false;
	}
	return IsSorted;
}
