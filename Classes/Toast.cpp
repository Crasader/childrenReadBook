#include "Toast.h"
#include "App.h"

using namespace cocostudio::timeline;

Toast* Toast::instance = nullptr;

Toast::Toast()
{
}

Toast::~Toast()
{
}

Toast* Toast::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new Toast();
	}
	return instance;
}

void Toast::create(const char* msg, bool show, float time)
{
	Toast *ret = GetInstance();
	ret->setShowTime(time);
	ret->init(msg, show);
}

void Toast::init(const char* msg, bool show)
{
	if (!show && m_dataQueue.size() > 0)
	{
		auto t = App::getCurrentTime() - m_time;
		if (t <= m_showTime)
			return;
	}
	if(!string(msg).empty())
		m_dataQueue.push(msg);
	App::log("m_dataQueue:", m_dataQueue.size());
	if (m_runing == false)
	{
		m_runing = true;
		showNext();
	}
	return;
}

void Toast::showToast()
{
	auto ly = CSLoader::createNode(TOAST_CSB);
	m_showLayer = ly;
	m_showLayer->setPosition(Vec2(-100, - 100));
	m_showLayer->retain();
}

void Toast::showNext()
{
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
		if (m_dataQueue.size() <= 0)
		{
			setRuning(false);
			return;
		}
		setRuning(true);
		auto text = (Text*)m_showLayer->getChildByName(FIND_TEXT_TOAST_CONTENT);
		string content = m_dataQueue.front();
		App::log("Toast = " + content, content.length());
		if (content.length() > 54)
			text->setFontSize(m_fontSize - 20);
		else if (content.length() > 45)
			text->setFontSize(m_fontSize - 10);
		else
			text->setFontSize(m_fontSize);
		text->setText(content);
		m_time = YYXLayer::getCurrentTime4Second();
		if (m_showLayer->getParent())
		{
			m_showLayer->removeFromParent();
		}
		Director::getInstance()->getRunningScene()->addChild(m_showLayer,10000);
		Size visiblesize = Director::getInstance()->getVisibleSize();
		m_showLayer->setPosition(Vec2(visiblesize.width/2,-200));
		m_showLayer->runAction(MoveTo::create(0.3, Vec2(visiblesize.width / 2, 100)));
	});
}

void Toast::unvisiable()
{
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
		huifuMap.clear();
		if (Toast::GetInstance()->m_dataQueue.size() > 0)
			Toast::GetInstance()->m_dataQueue.pop();
		if (m_showLayer)
			m_showLayer->removeFromParent();
		Toast::GetInstance()->setRuning(false);
		App::log("Toast show only = ", Toast::GetInstance()->m_dataQueue.size());
	});
}

void Toast::setShowTime(int second)
{
	m_showTime = second;
}
int Toast::getShowTime()
{
	return m_showTime;
}
void Toast::setRuning(bool s)
{
	m_runing = s;
}
bool Toast::getRuning()
{
	return m_runing;
}


void Toast::SceneInitToast()
{
	YYXLayer::logb("void Toast::SceneInitToast()");
	//thread([=]() {
		//App::ccsleep(100);
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
			if (getRuning() && m_showLayer)
			{
				auto text = (Text*)m_showLayer->getChildByName(FIND_TEXT_TOAST_CONTENT);
				string content = m_dataQueue.front();
				if (huifuMap.find(content) != huifuMap.end())
				{
					return;
				}
				else
				{
					huifuMap[content] = 1;
					if (content.length() > 54)
						text->setFontSize(40);
					else if (content.length() > 45)
						text->setFontSize(50);
					text->setText(content);
					Size visiblesize = Director::getInstance()->getVisibleSize();
					m_showLayer->setPosition(Vec2(visiblesize.width / 2, 100));
					m_showLayer->removeFromParent();
					m_showLayer->setVisible(true);
					Director::getInstance()->getRunningScene()->addChild(m_showLayer);
					m_time = YYXLayer::getCurrentTime4Second() + 1;
					YYXLayer::loge("void Toast::SceneInitToast()");
				}
			}
		});
	//}).detach();	
}