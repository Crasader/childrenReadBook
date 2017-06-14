#include "PushMessage.h"
#include <string>
#include "App.h"
#include "YYXVisitor.h"
using namespace std;

PushMessage*  PushMessage::PushMessage::instance = nullptr;

PushMessage* PushMessage::getInstance()
{
	if (instance == nullptr)
	{
		instance = new PushMessage();
	}
	return instance;
}

PushMessage::PushMessage()
{
	m_run = false;
}

PushMessage::~PushMessage()
{
}

void PushMessage::pushTask(string json)
{
	m_Mdata.lock();
	m_data.push(json);
	m_Mdata.unlock();
}

std::string PushMessage::popTask()
{
	string json = "";
	m_Mdata.lock();
	json = m_data.front();
	m_data.pop();
	m_Mdata.unlock();
	return json;
}

int PushMessage::getTaskCount()
{
	int count = 0;
	m_Mdata.lock();
	count = m_data.size();
	m_Mdata.unlock();
	return count;
}

void PushMessage::clear()
{
	m_Mdata.lock();
	if (!m_data.empty())
	{
		int count = m_data.size();
		for (int i = 0; i < count;i++)
		{
			m_data.pop();
		}
	}
	m_Mdata.unlock();
}

void PushMessage::newTask(string json)
{
	thread([json]() {
		PushMessage::getInstance()->pushTask(json);
		PushMessage::getInstance()->startHander();
	}).detach();
}

void PushMessage::startHander()
{
	m_Mhander.lock();
	if (!m_run)
	{
		if (getTaskCount() > 0)
		{
			string taskjson = popTask();
			handerTask(taskjson);
		}
	}
	m_Mhander.unlock();
}

void PushMessage::handerTask(string json)
{
	m_run = true;
	bool result = true;
	rapidjson::Document doc;
	result = YYXLayer::getJsonObject4Json(doc, json);
	if (result)
	{
		string mytype = YYXLayer::getStringForJson("-1", doc, "MessageType");
		auto t = Value(mytype).asInt();
		switch (t)
		{
		case MessageType::logout:
			App::cancelData();
			YYXVisitor::getInstance()->loginVisitor();
			break;
		default:
			break;
		}
	}
	m_run = false;
	startHander();
}
