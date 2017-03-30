﻿#include "BuyBook.h"

BuyBook* BuyBook::BuyBookinstance = nullptr;

BuyBook* BuyBook::getInstance()
{
	if (BuyBookinstance == nullptr)
	{
		BuyBookinstance = new BuyBook();
	}
	return BuyBookinstance;
}

BuyBook::BuyBook()
{
	App::log("BuyBook()");
}

BuyBook::~BuyBook()
{
	App::log("~BuyBook()");
}

void BuyBook::initDir()
{
	if (!FileUtils::getInstance()->isDirectoryExist(FileUtils::getInstance()->getWritablePath() + "buyBook"))
		FileUtils::getInstance()->createDirectory(FileUtils::getInstance()->getWritablePath() + "buyBook");
}

void BuyBook::loadCache()
{
	string cfilename = "buyBook/buyBook_" + App::getMemberID() + ".json";
	string path = FileUtils::getInstance()->getWritablePath() + cfilename;
	map<string, string> data;
	App::getMapFromFile(path, data);
	for (auto it : data)
	{
		int bookid = atoi(it.first.c_str());
		int time = atoi(it.second.c_str());
		if (bookid > 0)
			m_buyTimes[bookid] = time;
	}
}

void BuyBook::initData()
{
	App::log("BuyBook::initData()");
	if (!needSort())
		return;
	//购买的书 + 年卡的书
	m_inputbooks.insert(App::GetInstance()->myBuyBook.begin(), App::GetInstance()->myBuyBook.end());
	m_inputbooks.insert(App::GetInstance()->VIPbook.begin(), App::GetInstance()->VIPbook.end());
}

void BuyBook::addBook(int bookid, int time, bool write)
{
	//App::log("BuyBook::addBook()",bookid);
	//App::log("BuyBook::addBook() time=", time);
	m_inputbooks[bookid] = time;
	auto its = m_buyTimes.find(bookid);
	if (its != m_buyTimes.end())
	{
		if (time > its->second)
		{
			m_buyTimes[bookid] = time;
		}
	}
	else
		m_buyTimes[bookid] = time;
	if (write)
	{
		writeDownXml();
	}
}

void BuyBook::deleteBook(int bookid)
{
	App::log("BuyBook::deleteBook()",bookid);
	auto it = m_inputbooks.find(bookid);
	if (it != m_inputbooks.end())
	{
		m_inputbooks.erase(it);
	}
	auto t = m_buyTimes.find(bookid);
	if ( t != m_buyTimes.end())
		m_buyTimes.erase(t);
}

void BuyBook::writeDownXml()
{
	map<string, string> data;
	for (auto it : m_buyTimes)
	{
		string key = StringUtils::format("%d", it.first);
		data[key] = StringUtils::format("%d", it.second);
	}
	string json = YYXLayer::getStringFormMap(data);
	string filename = StringUtils::format("buyBook/buyBook_%d.json", App::GetInstance()->getMemberId());
	YYXLayer::writeFile(json, FileUtils::getInstance()->getWritablePath() + filename);
}

void BuyBook::clearBook()
{
	m_inputbooks.clear();
	m_buyTimes.clear();
	m_outputbooks.clear();
	m_tempbooks.clear();
}

void BuyBook::filtrate()
{
	if (!needSort())
		return;
	App::log("BuyBook::filtrate()");
	m_tempbooks.clear();
	for (auto it : m_inputbooks)
	{
		int time = 0;
		auto bookid = it.first;
		auto it2 = m_buyTimes.find(bookid);
		if (it2 != m_buyTimes.end())
		{
			time = it2->second;
		}
		m_tempbooks[bookid] = time;
	}
}

void BuyBook::sort()
{
	if (!needSort())
		return;
	App::log("BuyBook::sort()");
	auto data = App::sortMapToVector(m_tempbooks);
	m_outputbooks.clear();
	for (auto it : data)
	{
		m_outputbooks.push_back(it.first);
	}
}

vector<int> BuyBook::getResultData()
{
	count = m_buyTimes.size();
	IsSorted = false;
	return m_outputbooks;
}

bool BuyBook::needSort()
{
	if (IsSorted)
	{
		IsSorted = true;
	}
	else
	{
		if (count != m_buyTimes.size())
			IsSorted = true;
		else
			IsSorted = false;
	}
	return IsSorted;
}
