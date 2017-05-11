#include "ControlScene.h"
#include "LoadScene.h"
#include "YYXSound.h"
#include "cocos2d.h"

using namespace std;
USING_NS_CC;


#define REPLACESCENE(X) 	callback = [=]() {Director::getInstance()->replaceScene(TransitionFade::create(0.5f, X::createScene(NextScene)));};
#define CALLBACKBOOKSTORE(X) 	callback = [=]() {Director::getInstance()->replaceScene(TransitionFade::create(0.5f, BookStore::createScene(NextScene->setData("bookStoreId", Value(X)))));};

ControlScene* ControlScene::instance = nullptr;

ControlScene::ControlScene()
{
	MySceneName name[] = {
		LoadScene //加载场景
		, BookRoomScene//书房
		, ParentScene//父母设置
		, IndexScene//首页
		, LoginScene//登陆
		, BabyCenterScene//宝贝中心
		, BookCity//书城
		, BookInfoScene//书籍详细
		, BookCityCHILD//书城中的书店	
		, PictureBook//绘本
		, Recommend//咿啦推荐
		, Free//限时免费
		, GoodReputation//五星好评
		, NewBook//咿啦新书
		, KangXuanStore//康轩书店
		, VIPBOOK//vip专属书店
		, BOOK//阅读器
		, NULLSCENE//空场景
	};
	for (auto it : name)
	{
		sceneInfoMap[it] = SceneInfo::create()->setName(it);
	}
}

ControlScene::~ControlScene()
{
	for (auto it : sceneInfoMap)
	{
		SceneInfo::del(it.second);
	}
}

ControlScene* ControlScene::getInstance()
{
	if (instance == nullptr)
	{
		instance = new ControlScene();
	}
	return instance;
}

SceneInfo* ControlScene::getSceneInfo(MySceneName name)
{
	auto info = sceneInfoMap[name];
	return info;
}

void ControlScene::backFromScene()
{
	if (m_currentScene == nullptr)
		return;
	m_nextScene = getFromScene();
	if (replace(m_currentScene, m_nextScene, false))
	{
		m_currentScene = m_nextScene;
	}
	else
	{
		pushCurrentScene(m_nextScene);
	}
}

void ControlScene::replaceScene(SceneInfo* CurrentScene, SceneInfo* NextScene, bool push)
{
	//当前场景信息保留，可以获取到
	m_currentScene = CurrentScene;
	m_nextScene = NextScene;
	if (replace(m_currentScene , m_nextScene, push))
		m_currentScene = m_nextScene;
}

void ControlScene::end()
{
	m_replace = true;
	CCLOG("ControlScene::m_replace === >>%s " , m_replace ? "true" : "false");
}



void ControlScene::pushCurrentScene(SceneInfo* sc)
{
	if (sc)
	{
		if(sc->getName() != NULLSCENE)
			sceneStack.push_back(sc);
	}
}

SceneInfo* ControlScene::getFromScene(bool pop)
{	
	SceneInfo *inf = nullptr;
	if (!sceneStack.empty())
	{
		inf = sceneStack.back();
		if(pop)
			sceneStack.pop_back();
	}
	else
	{
		inf = getSceneInfo(NULLSCENE);
	}
	return inf;
}

SceneInfo* ControlScene::getCurrentScene()
{
	if (m_currentScene == nullptr)
	{
		m_currentScene = getSceneInfo(NULLSCENE);
	}
	return m_currentScene;
}

bool ControlScene::replace(SceneInfo* CurrentScene, SceneInfo* NextScene, bool push)//核心函数
{
	show();
	if (m_replace == false)
		return false;
	if (CurrentScene->getName() == NULLSCENE)
	{
		CurrentScene = getSceneInfo(IndexScene);
	}
	if (NextScene == nullptr || NextScene->getName() == NULLSCENE)
	{
		NextScene = getSceneInfo(IndexScene);
	}
	if (push && CurrentScene)
	{
		sceneStack.push_back(CurrentScene);//堆栈规则
		if (CurrentScene->getName() == IndexScene && sceneStack.size() > 2)
		{
			sceneStack.clear();
			sceneStack.push_back(CurrentScene);
		}
	}
	m_replace = false;
	YYXSound::getInstance()->playButtonSound();
	vector<string> plist;
	function<void()> callback = nullptr;
	auto name = NextScene->getName();
	if (name == LoadScene)
	{
		plist.push_back("WaitMessageBox/csb/wait");
		REPLACESCENE(Load)
	}
	else 	if (name == BookRoomScene) {
		plist.push_back("BookRoom/csb/bookRoom");
		REPLACESCENE(BookRoom)
	}
	else 	if (name == ParentScene){
		plist.push_back("ParentScene/csb/parent");
		REPLACESCENE(Parent)
	}
	else 	if (name == IndexScene) {
		plist.push_back("IndexScene/csb/background");
		plist.push_back("IndexScene/csb/train");
		plist.push_back("IndexScene/csb/Index");
		REPLACESCENE(Index)
	}
	else 	if (name == LoginScene) {
		plist.push_back("LoginScene/csb/login");
		REPLACESCENE(Login)
	}
	else 	if (name == BabyCenterScene) {
		plist.push_back("BabyCenter/csb/babyCenter");
		REPLACESCENE(BabyCenter)
	}
	else 	if (name == BookCity) {
		plist.push_back("BookCity/csb/bookCity");
		plist.push_back("BookCity/csb/buttons");
		REPLACESCENE(BookCityScene)
	}
	else 	if (name == BookInfoScene) {
		plist.push_back("BookInfo/csb/bookInfo");
		REPLACESCENE(BookInfo)
	}
	else 	if (name == BookCityCHILD) {
		plist.push_back("BookStoreScene/csb/bookstore");
		REPLACESCENE(BookStore)
	}
	else 	if (name == PictureBook) {
		plist.push_back("BookStoreScene/csb/bookstore");
		CALLBACKBOOKSTORE(_huiben);
	}
	else 	if (name == Recommend) {
		plist.push_back("BookStoreScene/csb/bookstore");
		CALLBACKBOOKSTORE(_tuijian);
	}
	else 	if (name == Free) {
		plist.push_back("BookStoreScene/csb/bookstore");
		CALLBACKBOOKSTORE(_mianfei);
	}
	else 	if (name == GoodReputation) {
		plist.push_back("BookStoreScene/csb/bookstore");
		CALLBACKBOOKSTORE(_haoping);
	}
	else 	if (name == NewBook) {
		plist.push_back("BookStoreScene/csb/bookstore");
		CALLBACKBOOKSTORE(_xinshu);
	}
	else 	if (name == KangXuanStore) {
		plist.push_back("BookStoreScene/csb/bookstore");
		CALLBACKBOOKSTORE(_kangxuan);
	}
	else 	if (name == VIPBOOK) {
		plist.push_back("BookStoreScene/csb/bookstore");
		CALLBACKBOOKSTORE(_baonian);
	}
	preLoadResources(plist, callback);
	return true;
}

void ControlScene::preLoadResources(vector<string> plist, const function<void()> & callback)
{
	SpriteFrameCache::getInstance()->removeSpriteFrames();
	TextureCache::getInstance()->removeAllTextures();
	setLoadPlistCount(plist.size());
	for (auto it : plist)
	{
		Director::getInstance()->getTextureCache()->addImageAsync(it + ".png", [=](Texture2D* sender) {
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile(it + ".plist");
			CCLOG("SpriteFrameCache::getInstance()->addSpriteFramesWithFile (%s).plist", it.c_str());
			setLoadPlistCount(getLoadPlistCount() - 1);
			if (getLoadPlistCount() == 0)
			{
				if (App::m_debug == 0)
				{
					showMemory();
				}
				if (callback)
					callback();	
			}
		});
	}
}

void ControlScene::show()
{
	int coun = sceneStack.size();
	for (auto it : sceneStack)
	{
		auto str= it->logName();
		App::log(Value(coun ).asString()+"========================================>>>"+str + "\n");
	}
}

void ControlScene::showMemory()
{
	App::log("\n**********************************************************************************************************************************************");
	auto str = Director::getInstance()->getTextureCache()->getCachedTextureInfo();
	//YYXLayer::writeFilepp(str, FileUtils::getInstance()->getWritablePath() + "temp/OOM.txt");
	App::log(str);
	int size = NetIntface::m_functionMap.size();
	App::log(StringUtils::format("NetIntface::m_functionMap.size() =%d \n ", size));
	App::log(StringUtils::format("App::GetInstance() ->myData.size() = %d  \n", App::GetInstance()->myData.size()));
	App::log("/n**********************************************************************************************************************************************\n");
}

SceneInfo::SceneInfo()
{
}

SceneInfo::~SceneInfo()
{
}

SceneInfo* SceneInfo::create()
{
	auto it = new SceneInfo();
	if (it)
	{
		it->autorelease();
		it->retain();
	}
	return it;
}

void SceneInfo::del(SceneInfo* sc)
{
	if (sc)
	{
		if(sc->getReferenceCount()>0)
			CC_SAFE_RELEASE_NULL(sc);
	}
}

MySceneName SceneInfo::getName()
{
	if (this)
	{
		return this->name;
	}
	else
		return MySceneName::NULLSCENE;
}

cocos2d::Value SceneInfo::getData(std::string key, Value def)
{
	auto it = data.find(key);
	if (it != data.end())
	{
		return it->second;
	}
	else
		return def;
}

SceneInfo* SceneInfo::setData(std::string key, Value val)
{
	data[key] = val;
	return this;
}

std::string SceneInfo::logName()
{
	string str = "";
	switch (name)
	{
	case LoadScene:
		str = "LoadScene";
		break;
	case BookRoomScene:
		str = "BookRoomScene";
		break;
	case ParentScene:
		str = "ParentScene";
		break;
	case IndexScene:
		str = "IndexScene";
		break;
	case LoginScene:
		str = "LoginScene";
		break;
	case BabyCenterScene:
		str = "BabyCenterScene";
		break;
	case BookCity:
		str = "BookCity";
		break;
	case BookInfoScene:
		str = "BookInfoScene";
		break;
	case BookCityCHILD:
		str = "BookCityCHILD";
		break;
	case PictureBook:
		str = "PictureBook";
		break;
	case Recommend:
		str = "Recommend";
		break;
	case Free:
		str = "Free";
		break;
	case GoodReputation:
		str = "GoodReputation";
		break;
	case NewBook:
		str = "NewBook";
		break;
	case KangXuanStore:
		str = "KangXuanStore";
		break;
	case VIPBOOK:
		str = "VIPBOOK";
		break;
	case BOOK:
		str = "BOOK";
		break;
	case NULLSCENE:
		str = "NULLSCENE";
		break;
	}
	return str+"   YYX   ";
}
