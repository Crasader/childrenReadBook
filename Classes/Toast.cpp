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
			text->setFontSize(40);
		else if(content.length() > 45)
			text->setFontSize(50);
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

//----------------------------------------------------------

	/*auto point = Vec2(visibleSize.width/2, 100);

	auto label = Label::createWithSystemFont(msg.c_str(), "Arial", 40);

	label->setColor(Color3B::WHITE);

	label->ignoreAnchorPointForPosition(false);

	auto layer = LayerColor::create(Color4B(100, 100, 100, 255));

	layer->ignoreAnchorPointForPosition(false);

	layer->setContentSize(label->getContentSize() + Size(20, 15));

	node->addChild(layer,100);

	node->addChild(label,100);

	layer->setPosition(point);

	label->setPosition(layer->getPosition());

	auto mov = App::getCurrentTime() - m_time;
	if (mov >0)		mov = 0;
	else		mov = 1;
	auto seq1 = Sequence::create(DelayTime::create(mov),FadeIn::create(time / 5), DelayTime::create(time / 5 * 1.5), FadeOut::create(time / 5 * 2.5), [=](Ref* sender) {
		this->removeFromParentAndCleanup(true);
	}, NULL);

	auto seq2 = Sequence::create(EaseSineIn::create(MoveBy::create(time / 5, Vec2(0, 50))), DelayTime::create(time / 5 * 2),
		EaseSineOut::create(MoveBy::create(time / 3, Vec2(0, -50))), NULL);

	auto spawn = Spawn::create(seq1, seq2, NULL);

	auto action = Repeat::create(spawn, 1);

	auto seq3 = Sequence::create(DelayTime::create(mov),FadeIn::create(time / 5), DelayTime::create(time / 5 * 1.5), FadeOut::create(time / 5 * 2.5), [=](Ref* sender) {
		this->removeFromParentAndCleanup(true);
	}, NULL);

	auto seq4 = Sequence::create(EaseSineIn::create(MoveBy::create(time / 5, Vec2(0, 50))), DelayTime::create(time / 5 * 2),
		EaseSineOut::create(MoveBy::create(time / 3, Vec2(0, -50))), NULL);

	auto spawn2 = Spawn::create(seq3, seq4, NULL);

	auto action2 = Repeat::create(spawn2, 1);

	layer->setOpacity(0);

	layer->runAction(action);

	label->setOpacity(0);

	label->runAction(action2);
    return true;
}

Toast* Toast::create(cocos2d::Node *node, const std::string &msg, const float &time)
{
	Toast *ret = new (std::nothrow) Toast();
	if (ret)
	{
		if (ret->init(node, msg, time))
		{
			ret->autorelease();
		}
		else
		{
			delete ret;
			ret = nullptr;
		}
	}
	return ret;
}

void Toast::create(const char * msg,bool show)
{
	auto mv = App::getCurrentTime() - m_time;
	if (show)
		mv = 3;
	if (mv > 2)
	{

		MessageBox(msg, "系统提示");
		m_time = App::getCurrentTime();
	}
}
*/