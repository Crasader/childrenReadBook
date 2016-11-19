#include "IndexScene.h"
#include "NetIntface.h"

USING_NS_CC;
using namespace cocostudio::timeline;


Index::~Index()
{
	YYXLayer::logb("Index::~Index()");
	YYXLayer::loge("Index::~Index()");
}

Scene* Index::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Index::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}
void Index::onEnterTransitionDidFinish() {
	Layer::onEnterTransitionDidFinish();
	//消息推送
	showNotification();
	//版本比较
	dealEllaVersion();
	//注册送红包
	//auto memberId = UserDefault::getInstance()->getIntegerForKey("accountRegisteUserId", -999);//注册成功标记, 首页提示红包
	string memberIdstr = YYXLayer::getFileValue("accountRegisteUserId", "");
	if (memberIdstr != "")
	{
		//网络请求有效红包
		string url = string(IP).append(NET_USERREDPACKET).append("?memberId=").append(memberIdstr);
		string runKey = "IndexSceneHttpGetUserRedPacketSuccess";
		NetIntface::httpGet(url, runKey, [](string json) {
			NetIntface::httpGetUserRedPacketsCallBack(json, []() {
				App::GetInstance()->m_redPacket.clear();
			}, [](int coupon_id, int coupon_amount100, string coupon_expire_time) {
				if (coupon_id != -999 || coupon_amount100 != -99900) {
					map<string, YYXStruct> mapresult;
					YYXStruct::initMapYYXStruct(mapresult, "coupon_id", coupon_id);
					YYXStruct::initMapYYXStruct(mapresult, "coupon_amount", coupon_amount100);
					YYXStruct::initMapYYXStruct(mapresult, "coupon_expire_time", 0, coupon_expire_time);
					App::GetInstance()->m_redPacket.push_back(mapresult);
				}
			}, [](int expiring_coupon_count) {
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "expiring_coupon_count", expiring_coupon_count);
				YYXLayer::sendNotify("IndexSceneShowRedPacket");
			}, []() {});
		}, "", [](string str) {});
	}
	Toast::GetInstance()->SceneInitToast();
	auto vipRenew = showVIPRenew();
	if (vipRenew)
	{
		addChild(vipRenew);
	}
}

// on "init" you need to initialize your instance
bool Index::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
	App::m_RunningScene = MySceneName::IndexScene;
	//安卓返回键
	auto androidListener = EventListenerKeyboard::create();
	androidListener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event) {
		if (!App::GetInstance()->isBack)
			return;
		switch (keyCode)
		{
		case EventKeyboard::KeyCode::KEY_ESCAPE:
			CocosAndroidJni::AppExit();
			break;
		default:
			break;
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(androidListener, this);
	
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	CCLOG("w=%f,h=%f,x=%f,y=%f", visibleSize.width, visibleSize.height, origin.x, origin.y);
	/////////////////////////////
	auto cache = SpriteFrameCache::getInstance();
	auto plist = PLIST_INDEX;
	

	//根据系统时间判断显示白天场景或夜晚场景
	if (App::isNight()) {
		//背景音效播放
		if (!App::GetInstance()->isMusicPlay) {
			if (YYXLayer::getBoolFromXML(MUSIC_KEY)) {
				SimpleAudioEngine::getInstance()->playBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY_NIGHT, true);
			}
			else
			{
				SimpleAudioEngine::getInstance()->playBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY_NIGHT, true);
				SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
			}
			App::GetInstance()->isMusicPlay = true;
		}
		//天空
		auto sky = Sprite::create();
		sky->initWithSpriteFrameName(PICTURE_INDEX_BACKGROUND02);
		sky->setAnchorPoint(Vec2(0, 1));
		addChild(sky);
		sky->setPosition(Vec2(0, origin.y + visibleSize.height));

		//星星
		auto sprShan1 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_SHUANGUANG2);
		sprShan1->setPosition(Vec2(524, 534));
		sprShan1->setScale(0.7f);
		addChild(sprShan1);
		auto sprStar1 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_STAR1);
		sprStar1->setPosition(Vec2(524, 534));
		addChild(sprStar1);

		auto sprShan2 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_SHUANGUANG2);
		sprShan2->setPosition(Vec2(356, 434));
		addChild(sprShan2);
		auto sprStar2 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_STAR2);
		sprStar2->setPosition(Vec2(356, 434));
		addChild(sprStar2);

		auto sprShan3 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_SHUANGUANG2);
		sprShan3->setPosition(Vec2(760, 475));
		addChild(sprShan3);
		auto sprStar3 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_STAR2);
		sprStar3->setPosition(Vec2(760, 475));
		addChild(sprStar3);

		sprShan1->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1.0f), FadeIn::create(1.0f), NULL)));
		sprShan2->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1.2f), FadeIn::create(1.2f), NULL)));
		sprShan3->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1.1f), FadeIn::create(1.1f), NULL)));

		//流星
		auto sprGuang1 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_SHUANGUANG1);
		sprGuang1->setPosition(Vec2(680, 650));
		addChild(sprGuang1);
		auto sprLiuxing1 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_LIUXING);
		sprLiuxing1->setPosition(Vec2(680, 650));
		sprLiuxing1->setAnchorPoint(Vec2(0.18f, 0.12f));
		addChild(sprLiuxing1);
		auto action1 = RepeatForever::create(Spawn::create(EaseSineOut::create(MoveTo::create(3, Vec2(440, 330))), 
			Sequence::create(DelayTime::create(2.7f), FadeOut::create(0.3f), Place::create(Vec2(680, 650)), FadeIn::create(0.1f), DelayTime::create(5.0f), NULL), NULL));
		sprGuang1->runAction(action1);
		sprLiuxing1->runAction(action1);

		auto sprGuang2 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_SHUANGUANG1);
		sprGuang2->setPosition(Vec2(377, 595));
		sprGuang2->setOpacity(0);
		addChild(sprGuang2);
		auto sprLiuxing2 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_LIUXING);
		sprLiuxing2->setPosition(Vec2(377, 595));
		sprLiuxing2->setAnchorPoint(Vec2(0.18f, 0.12f));
		sprLiuxing2->setOpacity(0);
		addChild(sprLiuxing2);
		auto action2 = RepeatForever::create(Spawn::create(EaseSineOut::create(MoveTo::create(2.1f, Vec2(226, 400))), 
			Sequence::create(FadeIn::create(0.3f), DelayTime::create(1.5f), FadeOut::create(0.3f), Place::create(Vec2(377, 595)), DelayTime::create(4.0f), NULL), NULL));
		sprGuang2->runAction(action2);
		sprLiuxing2->runAction(action2);

		auto sprGuang3 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_SHUANGUANG1);
		sprGuang3->setPosition(Vec2(844, 595));
		sprGuang3->setOpacity(0);
		addChild(sprGuang3);
		auto sprLiuxing3 = Sprite::createWithSpriteFrameName(PICTURE_INDEX_LIUXING);
		sprLiuxing3->setPosition(Vec2(844, 595));
		sprLiuxing3->setAnchorPoint(Vec2(0.18f, 0.12f));
		sprLiuxing3->setOpacity(0);
		addChild(sprLiuxing3);
		auto action3 = RepeatForever::create(Spawn::create(EaseSineOut::create(MoveTo::create(2.5f, Vec2(650, 370))),
			Sequence::create(FadeIn::create(0.3f), DelayTime::create(1.9f), FadeOut::create(0.3f), Place::create(Vec2(844, 595)), DelayTime::create(4.5f), NULL), NULL));
		sprGuang3->runAction(action3);
		sprLiuxing3->runAction(action3);

		//远山
		addChild(ContinuousMove::create(PICTURE_INDEX_MOUNTAIN2, 1, Vec2(0, 670 * 0.4), Vec2(2736 * 0.4, 0), 200, Vec2(0, 0), Vec2(1, 0), Vec2(0, -1 * visibleSize.height / 2), 1.0f, 0.5f, -20, 2));
		//近山
		addChild(ContinuousMove::create(PICTURE_INDEX_HILL2, 1, Vec2(0, 286 * 0.4), Vec2(2736 * 0.4, 0), 120, Vec2(0, 0), Vec2(1, 0), Vec2(0, -1 * visibleSize.height / 2), 0.8f, 0.5f, -20, 2));
		//树林
		addChild(ContinuousMove::create(PICTURE_INDEX_WOODS2, 1, Vec2(0, 158 * 0.4), Vec2(2736 * 0.4, 0), 60, Vec2(0, 0), Vec2(1, 0), Vec2(0, -1 * visibleSize.height / 2), 0.6f, 0.3f, 5, 1));
		//铁轨
		addChild(ContinuousMove::create(PICTURE_INDEX_TRACK2, 1, Vec2(0, 0), Vec2(2736 * 0.4, 0), 30, Vec2(0, 0), Vec2(1, 0), Vec2(0, -1 * visibleSize.height / 2), 0.5f, 0, 0, 0));
	}
	else
	{
		//背景音效播放
		if (!App::GetInstance()->isMusicPlay) {
			if (YYXLayer::getBoolFromXML(MUSIC_KEY)) {
				SimpleAudioEngine::getInstance()->playBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY, true);
			}
			else
			{
				SimpleAudioEngine::getInstance()->playBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY, true);
				SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
			}
			App::GetInstance()->isMusicPlay = true;
		}
		//天空
		auto sky = Sprite::create();
		sky->initWithSpriteFrameName(PICTURE_INDEX_BACKGROUND0);
		sky->setAnchorPoint(Vec2(0, 1));
		addChild(sky);
		sky->setPosition(Vec2(0, origin.y + visibleSize.height));

		this->runAction(Sequence::create(DelayTime::create(3), CallFuncN::create([=](Ref* sender) {
			this->removeChild(sky);
			cache->removeSpriteFrameByName(PICTURE_INDEX_BACKGROUND0);
		}), NULL));
		auto sky1 = Sprite::create();
		sky1->initWithSpriteFrameName(PICTURE_INDEX_SKY);
		sky1->setAnchorPoint(Vec2(0, 1));
		addChild(sky1);
		sky1->setPosition(Vec2(0, origin.y + visibleSize.height));

		//远山
		addChild(ContinuousMove::create(PICTURE_INDEX_MOUNTAIN, 1, Vec2(0, 670 * 0.4), Vec2(2736 * 0.4, 0), 200, Vec2(0, 0), Vec2(1, 0), Vec2(0, -1 * visibleSize.height / 2), 1.0f, 0.5f, -20, 2));
		//近山
		addChild(ContinuousMove::create(PICTURE_INDEX_HILL, 1, Vec2(0, 286 * 0.4), Vec2(2736 * 0.4, 0), 120, Vec2(0, 0), Vec2(1, 0), Vec2(0, -1 * visibleSize.height / 2), 0.8f, 0.5f, -20, 2));
		//树林
		addChild(ContinuousMove::create(PICTURE_INDEX_WOODS, 1, Vec2(0, 158 * 0.4), Vec2(2736 * 0.4, 0), 60, Vec2(0, 0), Vec2(1, 0), Vec2(0, -1 * visibleSize.height / 2), 0.6f, 0.3f, 5, 1));
		//铁轨
		addChild(ContinuousMove::create(PICTURE_INDEX_TRACK, 1, Vec2(0, 0), Vec2(2736 * 0.4, 0), 30, Vec2(0, 0), Vec2(1, 0), Vec2(0, -1 * visibleSize.height / 2), 0.5f, 0, 0, 0));

		//飞鸟
		auto bird1 = Bird::create(PICTURE_INDEX_YELLOWBIRD_FIRSTFRAME, plist, FORMAT_PICTURE_NAME_INDEX_YELLOWBIRD, Vec2(2800 * 0.4, 1530 * 0.4), Vec2(1350 * 0.4, 1100 * 0.4), Vec2(1350 * 0.4, 1100 * 0.4), Vec2(-200 * 0.4, 1530 * 0.4), 3, 8);
		addChild(bird1);
		auto bird2 = Bird::create(PICTURE_INDEX_BLUEBIRD_FIRSTFRAME, plist, FORMAT_PICTURE_NAME_INDEX_BLUEBIRD, Vec2(2900 * 0.4, 1530 * 0.4), Vec2(visibleSize.width / 2, visibleSize.height * 0.75), Vec2(0, visibleSize.height * 0.75), Vec2(-300, visibleSize.height), 3, 9);
		addChild(bird2);
		auto bird3 = Bird::create(PICTURE_INDEX_CYANBIRD_FIRSTFRAME, plist, FORMAT_PICTURE_NAME_INDEX_CYANBIRD, Vec2(2900 * 0.4, 1530 * 0.4), Vec2(visibleSize.width, visibleSize.height * 0.75), Vec2(visibleSize.width / 2, visibleSize.height* 0.75), Vec2(0, visibleSize.height), 3, 7);
		addChild(bird3);
		auto bird4 = Bird::create(PICTURE_INDEX_PINKBIRD_FIRSTFRAME, plist, FORMAT_PICTURE_NAME_INDEX_PINKBIRD, Vec2(2900 * 0.4, 1530 * 0.4), Vec2(900 * 0.4, 1200 * 0.4), Vec2(visibleSize.width / 3, visibleSize.height* 0.75), Vec2(-300 * 0.4, 1530 * 0.4), 3, 6);
		addChild(bird4);
		auto bird5 = Bird::create(PICTURE_INDEX_REDBIRD_FIRSTFRAME, plist, FORMAT_PICTURE_NAME_INDEX_REDBIRD, Vec2(2900 * 0.4, 1530 * 0.4), Vec2(visibleSize.width * 0.8, visibleSize.height * 0.8), Vec2(visibleSize.width * 0.5, visibleSize.height * 0.9), Vec2(0, visibleSize.height), 3, 5);
		addChild(bird5);
	}
	
	
	// 音乐
	if (YYXLayer::getBoolFromXML(MUSIC_KEY))
	{
		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	}
	else
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

	//火车
	auto listview = ListView::create();
	addChild(listview);
	listview->setDirection(cocos2d::ui::ScrollView::Direction::HORIZONTAL);
	listview->setSize(Size(1094, 860 * 0.4));
	listview->setPosition(Vec2(origin.x + 1094, origin.y + 250 * 0.4));
	listview->setGravity(ListView::Gravity::BOTTOM);
	listview->setTouchEnabled(false);
	listview->setBounceEnabled(true);
	listview->setScrollBarEnabled(false);
	//第一项item 空白
	auto layout0 = Layout::create();
	layout0->setSize(Size(500 * 0.4, 1000 * 0.4));
	listview->pushBackCustomItem(layout0);
	//第二项火车头
	listview->pushBackCustomItem(createItem(LOCOMOTIVE,LOCOMOTIVE_CSB, 0, 90, true, Size(680 * 0.4, 1000 * 0.4)));
	//书城
	auto itemSize = Size(530 * 0.4, 1000 * 0.4);
	listview->pushBackCustomItem(createItem(BOOKCITY,BOOKCITY_CSB, 0, 90, true, Size(580 * 0.4, 1000 * 0.4)));
	//VIP专属书店
	listview->pushBackCustomItem(createItem(VIPBOOKSTORE, VIPBOOK_CSB, 0, 90, true, itemSize));
	//绘本
	listview->pushBackCustomItem(createItem(PICTUREBOOK,PICTUREBOOK_CSB, 0, 90, true, itemSize));
	//咿啦推荐	
	listview->pushBackCustomItem(createItem(RECOMMEND,RECOMMEND_CSB, 0, 90, true, itemSize));
	//限时免费
	listview->pushBackCustomItem(createItem(LIMITFREE,LIMITFREE_CSB, 0, 90, true, itemSize));
	//五星好评
	listview->pushBackCustomItem(createItem(EXCELLENT_FIVE_STAR,EXCELLENT_FIVE_STAR_CSB, 0, 90, true, itemSize));
	//咿啦新书
	listview->pushBackCustomItem(createItem(NEWBOOK,NEWBOOK_CSB, 0, 90, true, itemSize));
	//康轩书店
	listview->pushBackCustomItem(createItem(KANGXUANBOOKSTORE, KANGXUANBOOKSTORE_CSB, 0, 90, true, itemSize));
	//最后一项空白
	auto layoutend = Layout::create();
	layoutend->setSize(Size(1000 * 0.4, 1000 * 0.4));
	listview->pushBackCustomItem(layoutend);	
	//listview点击事件
	listview->addEventListenerListView(this, listvieweventselector(Index::selectedItemEvent));
	//花
	creatFlows(Vec2(-100 * 0.4, 70 * 0.4), PICTURE_INDEX_FLOWER3_ENDFRAME, 30, 21, plist, FORMAT_PICTURE_NAME_FLOWER3, 0.2f);
	creatFlows(Vec2(500 * 0.4, 90 * 0.4), PICTURE_INDEX_FLOWER2_ENDFRAME, 30, 17, plist, FORMAT_PICTURE_NAME_FLOWER2, 0.2f);
	creatFlows(Vec2(1000 * 0.4, 30 * 0.4), PICTURE_INDEX_FLOWER_ENDFRAME, 30, 17, plist, FORMAT_PICTURE_NAME_FLOWER, 0.2f);
	//左侧节点
	Node* leftNode = Node::create();
	addChild(leftNode);
	leftNode->setPosition(Vec2(origin.x, origin.y + 20 * 0.4));
	//树
	auto leftTree = Sprite::create();
	leftTree->initWithSpriteFrameName(PICTURE_INDEX_LEFT_TREE);
	CCLOG("tree= %d,%d", leftTree->getTexture()->getPixelsWide(), leftTree->getTexture()->getPixelsHigh());
	leftTree->setAnchorPoint(Vec2(333.0f / 1176.0f, 0));
	leftTree->setScale(4.0 / 6.0);
	leftNode->addChild(leftTree);
	//门
	auto door_img = ImageView::create(PICTURE_INDEX_LEFT_DOOR, TextureResType::PLIST);
	door_img->setScale9Enabled(true);
	door_img->setScale(4.0 / 6.0);
	auto door = Sprite::create();
	auto door2 = Sprite::create();
	door2->setScale(4.0 / 6.0);
	leftNode->addChild(door2);
	door2->setPosition(Vec2(151 * 0.4, 310 * 0.4));
	door->initWithSpriteFrameName(PICTURE_INDEX_LEFT_DOOR);
	door->setScale(4.0 / 6.0);
	leftNode->addChild(door_img);
	leftNode->addChild(door);
	door->setPosition(Vec2(151 * 0.4, 310 * 0.4));
	door_img->setPosition(Vec2(151 * 0.4, 310 * 0.4));
	door->getTexture()->setAliasTexParameters();
	door2->getTexture()->setAliasTexParameters();
	//灯
	auto light = Sprite::create();
	light->initWithSpriteFrameName(PICTURE_INDEX_LEFT_LAMP);
	light->setAnchorPoint(Vec2(0.5, 1));
	light->setPosition(Vec2(182 * 0.4, 471 * 0.4));
	light->setRotation(-15);
	light->setScale(4.0 / 6.0);
	leftNode->addChild(light);
	//音乐按钮
	auto music = ImageView::create();
	if (YYXLayer::getBoolFromXML(MUSIC_KEY))
	{
		music->loadTexture(PICTURE_INDEX_OPEN_MUSIC, TextureResType::PLIST);
		music->setTag(OPEN);
	}
	else
	{
		music->loadTexture(PICTURE_INDEX_CLOSE_MUSIC, TextureResType::PLIST);
		music->setTag(CLOSE);
	}
	music->setScale(4.0 / 6.0);
	leftNode->addChild(music);
	music->setPosition(Vec2(91 * 0.4, 750 * 0.4));
	music->setTouchEnabled(true);
	music->addClickEventListener([=](Ref* pSender) {
		CCLOG("音乐按钮回调函数");
		YYXLayer::controlTouchTime(1, "setmusicTime", [=]() {
			if (music->getTag() == OPEN)
			{
				music->loadTexture(PICTURE_INDEX_CLOSE_MUSIC, TextureResType::PLIST);
				music->setTag(CLOSE);
				YYXLayer::setFileValue(MUSIC_KEY, "false");
				YYXLayer::setFileValue(SOUND_KEY, "false");
				SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
				SimpleAudioEngine::getInstance()->stopAllEffects();
				SimpleAudioEngine::getInstance()->end();
			}
			else
			{
				music->loadTexture(PICTURE_INDEX_OPEN_MUSIC, TextureResType::PLIST);
				music->setTag(OPEN);
				YYXLayer::setFileValue(MUSIC_KEY, "true");
				YYXLayer::setFileValue(SOUND_KEY, "true");
				if (App::isNight())
					SimpleAudioEngine::getInstance()->playBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY_NIGHT, true);
				else
					SimpleAudioEngine::getInstance()->playBackgroundMusic(ELLA_SOUND_BACKMUSIC_DAY, true);
			}
		}, []() {
			Toast::create(App::getString("CAOZUOGUOYUPINGFAN"),false);
		});
	});
	////猫头鹰
	auto owl = Sprite::create();
	owl->initWithSpriteFrameName(PICTURE_INDEX_OWL);
	leftNode->addChild(owl);
	owl->setPosition(Vec2(351 * 0.4, 1600 * 0.4));
	owl->setRotation(-10);
	owl->setScale(4.0 / 6.0);
	//整体旋转45度
	leftNode->setRotation(-45);
	auto seq = Sequence::create(RotateTo::create(1, 0), CallFuncN::create([=](Ref* pSender) {
		//整体旋转后灯晃动
		auto anmi = RotateBy::create(4, 30);
		auto seq = Sequence::create(anmi, anmi->reverse(), NULL);
		light->runAction(RepeatForever::create(seq));

		////门的帧动画	
		auto cache = SpriteFrameCache::getInstance();
		auto door_animate = FrameAnimation::createAnimate(4,plist,FORMAT_PICTURE_NAME_INDEX_LEFT_DOOR,0.1f);
		auto seqDoor = Sequence::create(door_animate, door_animate->reverse(), NULL);
		door->runAction(RepeatForever::create(seqDoor));
		//猫头鹰移动
		owl->runAction(MoveTo::create(0.5f, Vec2(351*0.4, 1207*0.4)));
	}), NULL);
	leftNode->runAction(seq);
	auto grass = Sprite::create();
	grass->initWithSpriteFrameName(PICTURE_INDEX_LEFT_GRASS);
	grass->setAnchorPoint(Vec2(0, 0));
	grass->setScale(4.0 / 6.0);
	addChild(grass);
	grass->setPosition(origin);
	
//右节点
	//树
	Node* rightNode = Node::create();
	addChild(rightNode);
	rightNode->setPosition(Vec2(origin.x + visibleSize.width, origin.y - 40));
	auto rightTree = Sprite::create();
	rightTree->initWithSpriteFrameName(PICTURE_INDEX_RIGHT_TREE);
	rightTree->setAnchorPoint(Vec2(527.0f / 946.0f, 0));
	rightTree->setScale(4.0 / 6.0);
	rightNode->addChild(rightTree);	

	////头像边框
	auto ring = Sprite::create();
	ring->getTexture()->setAliasTexParameters();
	ring->initWithSpriteFrameName(PICTURE_INDEX_PORTRAITRIM);
	ring->setScale(4.0 / 6.0);
	rightNode->addChild(ring, 1);
	ring->setPosition(Vec2(-203 * 0.4, 1471 * 0.4));

	//头像
	auto portrait = ImageView::create(PICTURE_INDEX_PORTRAIT,TextureResType::PLIST);
	portrait->setSize(portrait->getContentSize());
	portrait->ignoreContentAdaptWithSize(false);
	portrait->setScale(4.0 / 6.0);
	rightNode->addChild(portrait);
	portrait->setPosition(Vec2(-203 * 0.4, 1471 * 0.4));

	bool findDataSour = false;
	auto childid = YYXStruct::getMapInt64(App::GetInstance()->myData, "ShowChildID", -999);
	if (App::GetInstance()->m_me == nullptr || childid == -999) {
		portrait->loadTexture(PICTURE_INDEX_PORTRAIT_NO_LOGIN, TextureResType::PLIST);
	}
	else
	{
		auto path = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildHeadPortrait", "");
		if (path != "" && FileUtils::getInstance()->isFileExist(path))
		{
			portrait->loadTexture(path);
			findDataSour = true;
		}
		else
		{
			string savePath = FileUtils::getInstance()->getWritablePath() + StringUtils::format("temp/childHead_%d.png", childid);
			if (FileUtils::getInstance()->isFileExist(savePath))
			{
				YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "ShowChildHeadPortrait", -999, savePath);
				YYXLayer::setFileValue("ShowChildHeadPortrait", savePath);
				portrait->loadTexture(savePath);
				findDataSour = true;
			}
		}
		if (!findDataSour)
		{
			portrait->loadTexture(PICTURE_INDEX_PORTRAIT, TextureResType::PLIST);
		}
	}
	//下载第一个孩子的头像回调
	auto listenerHeadPortrait = EventListenerCustom::create("IndexSceneReferShowHeadPortrait", [portrait](EventCustom* e) {
		auto path = YYXStruct::getMapString(App::GetInstance()->myData, "ShowChildHeadPortrait", "");
		if (portrait && path != "" && FileUtils::getInstance()->isFileExist(path))
			portrait->loadTexture(path);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerHeadPortrait, portrait);
	////皇冠
	auto crown = Sprite::create();
	crown->initWithSpriteFrameName(PICTURE_INDEX_CROWN);
	crown->setScale(4.0 / 6.0);
	rightNode->addChild(crown, 1);
	crown->setPosition(Vec2(-203 * 0.4, 1494 * 0.4));
	//家长选项
	auto parent = ImageView::create(PICTURE_INDEX_PARENT, TextureResType::PLIST);
	parent->setScale9Enabled(true);
	parent->setAnchorPoint(Vec2(0.5, 1));
	rightNode->addChild(parent);
	parent->setRotation(15);
	parent->setScale(4.0 / 6.0);
	parent->setPosition(Vec2(-176 * 0.4, 1221 * 0.4));
	parent->setTouchEnabled(false);
	parent->addClickEventListener([=](Ref *sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
		 YYXLayer::PLAYBUTTON;
		addChild(SelectLayer([]() {
			App::GetInstance()->pushScene(IndexScene);
			GoToParentScene();
		}));
	});
	////整体旋转45度
	rightNode->setRotation(45);
	auto rseq = Sequence::create(RotateTo::create(1, 0), CallFuncN::create([=](Ref* pSender) {
		//整体旋转后家长选项晃动
		auto anmi = RotateBy::create(4, -30);
		auto seq = Sequence::create(anmi, anmi->reverse(), NULL);
		parent->runAction(RepeatForever::create(seq));
	}), NULL);
	rightNode->runAction(rseq);
	auto rightGrass = Sprite::create();
	rightGrass->initWithSpriteFrameName(PICTURE_INDEX_RIGHT_GRASS);
	rightGrass->setAnchorPoint(Vec2(1, 0));
	rightGrass->setScale(4.0 / 6.0);
	addChild(rightGrass);
	rightGrass->setPosition(origin.x + visibleSize.width, origin.y);

	//火车移动
	listview->runAction(Sequence::create(DelayTime::create(1), CallFuncN::create([=](Ref* pSender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
		SimpleAudioEngine::getInstance()->playEffect(ELLA_SOUND_TRAINWHISTLE);
	}), MoveTo::create(2, Vec2(origin.x + 100 * 0.4, origin.y + 235 * 0.4)), CallFuncN::create([=](Ref* sender) {		
		listview->setTouchEnabled(true);
		parent->setTouchEnabled(true);
		portrait->setTouchEnabled(true);
		//宝贝中心跳转
		portrait->addClickEventListener([=](Ref* sender) {
			if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
			App::GetInstance()->pushScene(MySceneName::IndexScene);
			GoToBabyCenterScene();
		});
		door_img->setTouchEnabled(true);
		//点击门跳转书房
		door_img->addClickEventListener([=](Ref* sender) {
			door_img->setVisible(false);
			door->setVisible(false);
			Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
			door2->runAction(Sequence::create(CallFuncN::create([=](Ref* pSender) {
				if (YYXLayer::getBoolFromXML(SOUND_KEY))
				SimpleAudioEngine::getInstance()->playEffect(ELLA_SOUND_OPENDOOR);
			}),
				FrameAnimation::createAnimate(7, plist, FORMAT_PICTURE_NAME_DOOR, 0.2f), 
				CallFuncN::create([=](Ref *sender) {
				//跳转书房
				App::GetInstance()->pushScene(MySceneName::IndexScene);
				GoToBookRoomScene();
			}), NULL));
		});	
	}), NULL));
	Director::getInstance()->getEventDispatcher()->addCustomEventListener("IndexSceneShowRedPacket", [=](EventCustom* e) {
		showRedPacket();
		//UserDefault::getInstance()->deleteValueForKey("accountRegisteUserId");
		YYXLayer::deleteFileValue("accountRegisteUserId");
	});	
    return true;
}

void Index::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

//创建火车车厢
Layout* Index::createItem(int tag,const std::string& csbfilename,int startIndex,int endIndex, bool loop,Size layoutsize)
{
	auto layout3 = Layout::create();
	Data data;
	Node* huiben;
	ActionTimeline* huibenAnimation;
	if (App::GetInstance()->getData(csbfilename, data))
	{
		App::log("find csb File cache!");
		huiben = CSLoader::createNode(data);
		huibenAnimation = CSLoader::createTimeline(data, csbfilename);
	}
	else
	{
		huiben = CSLoader::createNode(csbfilename);
		huibenAnimation = CSLoader::createTimeline(csbfilename);
	}	
	huiben->runAction(huibenAnimation);
	huibenAnimation->gotoFrameAndPlay(startIndex, endIndex, loop);
	layout3->addChild(huiben);
	layout3->setSize(layoutsize);
	layout3->setTag(tag);
	layout3->setTouchEnabled(true);
	return layout3;
}

//创建花
void Index::creatFlows(Vec2 position, const std::string &imageFileName,float time, int nFrameNumber, const std::string& c_plist, const char* c_FileNameFormat, float delay)
{
	auto flow = Sprite::create();
	auto imageview = ImageView::create(imageFileName, TextureResType::PLIST);
	this->addChild(imageview);
	this->addChild(flow);
	imageview->setTouchEnabled(true);
	flow->setVisible(false);
	imageview->setVisible(false);
	flow->setAnchorPoint(Vec2(0.5,0));
	imageview->setAnchorPoint(Vec2(0.5, 0));
	imageview->setPosition(position);
	flow->setPosition(position);
	flow->runAction(Sequence::create(DelayTime::create(3), CallFuncN::create([=](cocos2d::Ref * sender) {
		imageview->setVisible(true);
		imageview->setScale(0);
		imageview->runAction(ScaleTo::create(2, 1));
	}), NULL));
	flow->runAction(RepeatForever::create(Sequence::create(MoveBy::create(time, Vec2(2736 * 0.4, 0)), CallFuncN::create([=](cocos2d::Ref * sender) {
		flow->setPosition(position);
	}), NULL)));
	imageview->runAction(RepeatForever::create(Sequence::create(MoveBy::create(time, Vec2(2736 * 0.4, 0)), CallFuncN::create([=](cocos2d::Ref * sender) {
		imageview->setPosition(position);
		imageview->setScale(0);
		imageview->runAction(ScaleTo::create(2, 1));
	}), NULL)));
	imageview->addClickEventListener([=](cocos2d::Ref * sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			SimpleAudioEngine::getInstance()->playEffect(ELLA_SOUND_FLOWERS);

		imageview->setVisible(false);
		flow->setVisible(true);
		flow->runAction(Sequence::create(FrameAnimation::createAnimate(nFrameNumber, c_plist, c_FileNameFormat, delay), CallFuncN::create([=](cocos2d::Ref * sender) {
			imageview->setVisible(true);
			flow->setVisible(false);
		}), NULL));
	});
}

//---------------------------转场函数-----------------------------------------------------
//void Index::replaceSceneAnimation(std::function<void ()> runfunction)
//{
//	Director::getInstance()->replaceScene(Waiting::createScene(runfunction));
//}

//跳转书房
void Index::GoToBookRoomScene()
{
	 
	if (YYXLayer::getBoolFromXML(SOUND_KEY))
		YYXLayer::PLAYBUTTON;
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST4);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, BookRoom::createScene()));
	//Director::getInstance()->replaceScene( BookRoom::createScene());
}

//跳转父母设置
void Index::GoToParentScene()
{
	 
	if (YYXLayer::getBoolFromXML(SOUND_KEY))
		YYXLayer::PLAYBUTTON;
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST10);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, Parent::createScene()));
}

//跳转首页
void Index::GoToIndexScene()
{
	 
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST6);
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST7);
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST8);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, Index::createScene()));
}

//跳转登录
void Index::GoToLoginScene()
{
	 
	if (YYXLayer::getBoolFromXML(SOUND_KEY))
		YYXLayer::PLAYBUTTON;
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST9);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, Login::createScene()));
}

//跳转宝贝中心
void Index::GoToBabyCenterScene()
{
	 
	if (YYXLayer::getBoolFromXML(SOUND_KEY))
		YYXLayer::PLAYBUTTON;
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST1);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, BabyCenter::createScene()));
	App::log(" Index::GoToBabyCenterScene()---END");
}

//跳转书城
void Index::GoToBookCity()
{
	CocosAndroidJni::stopRequestByTag(-1);
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST2);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, BookCityScene::createScene()));
	//Director::getInstance()->replaceScene( BookCityScene::createScene());
}

//跳转书城里的书店
void Index::GoToBookCityChildStore(int bookStoreId)
{
	 
	CocosAndroidJni::stopRequestByTag(-1);
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST5);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f,BookStore::createScene(bookStoreId)));
	//Director::getInstance()->replaceScene(BookStore::createScene(bookStoreId));
}

//跳转书籍详情
void Index::GoToBookInfo(int bookId)
{
	 
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST3);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f,BookInfo::createScene(bookId)));
	//Director::getInstance()->replaceScene(BookInfo::createScene(bookId));
}

//跳转绘本
void Index::GoToPictureBook()
{
	 
	CocosAndroidJni::stopRequestByTag(-1);
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST5);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f,BookStore::createScene(P_TRAIN_2)));
}

//跳转咿啦推荐
void Index::GoToRecommend()
{
	 
	CocosAndroidJni::stopRequestByTag(-1);
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST5);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f,BookStore::createScene(P_TRAIN_3)));
	//Director::getInstance()->replaceScene( BookStore::createScene(P_TRAIN_3));
}

//跳转限时免费
void Index::GoToFree()
{
	 
	CocosAndroidJni::stopRequestByTag(-1);
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST5);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f,BookStore::createScene(P_TRAIN_4)));
	//Director::getInstance()->replaceScene( BookStore::createScene(P_TRAIN_4));
}

//跳转五星好评
void Index::GoToGoodReputation()
{
	 
	CocosAndroidJni::stopRequestByTag(-1);
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST5);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f,BookStore::createScene(P_TRAIN_5)));
	//Director::getInstance()->replaceScene(BookStore::createScene(P_TRAIN_5));
}

//跳转咿啦新书
void Index::GoToNewBook()
{
	 
	CocosAndroidJni::stopRequestByTag(-1);
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST5);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f,BookStore::createScene(P_TRAIN_6)));
	//Director::getInstance()->replaceScene( BookStore::createScene(P_TRAIN_6));
}

//跳转康轩书店
void Index::GoToKangXuanStore()
{
	 
	CocosAndroidJni::stopRequestByTag(-1);
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST5);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f,BookStore::createScene(P_TRAIN_7)));
}

//跳转VIP书店
void Index::GoToVIPBook()
{
	CocosAndroidJni::stopRequestByTag(-1);
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(LOADSCENE_FIND_PLIST5);
	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, BookStore::createScene(BOOKSTOREID_TRAIN_8)));
}

void Index::BackPreviousScene()
{
	CCLOG("%s","<=Back scene ");
	int id = 21;
	switch (App::GetInstance()->m_showScene)
	{
	case BookRoomScene:
		GoToBookRoomScene();
		break;
	case ParentScene:
		GoToParentScene();
		break;
	case IndexScene:
		GoToIndexScene();
		break;
	case LoginScene:
		GoToLoginScene();
		break;
	case BabyCenterScene:
		GoToBabyCenterScene();
		break;
	case BookCity:
		GoToBookCity();
		break;
	case BookInfoScene:
		GoToBookInfo(App::GetInstance()->m_showSceneData.intData);
		break;
	case PictureBook:
		GoToPictureBook();
		break;
	case Recommend:
		GoToRecommend();
		break;
	case MySceneName::Free:
		GoToFree();
		break;
	case GoodReputation:
		GoToGoodReputation();
		break;
	case NewBook:
		GoToNewBook();
		break;
	case KangXuanStore:
		GoToKangXuanStore();
		break;
	case BookCityCHILD:
		id = App::GetInstance()->m_showSceneData.intData;
		if (id < 0 || id >100000)
		{
			id = 21;
		}
		GoToBookCityChildStore(id);
		break;
	case VIPBOOK:
		GoToVIPBook();
		break;
	default:
		GoToIndexScene();
		break;
	}
}

void Index::selectedItemEvent(Ref* pSender, ListViewEventType type) 
{
	auto listview = (ListView*)pSender;
	auto index = listview->getCurSelectedIndex();
	Point listLocation = listview->getInnerContainerPosition();
	CCLOG("listLocation= (%f,%f)", listLocation.x, listLocation.y);
	auto item = (Layout*)listview->getItem(index);
	if (item->getTag() == LOCOMOTIVE)
		return;
	CCLOG("item= (%f,%f)", item->getPosition().x, item->getPosition().y);
	App::GetInstance()->m_showScene = IndexScene;
	App::GetInstance()->m_showSceneData.intData = 0;
	switch (type)
	{
	case ListViewEventType::LISTVIEW_ONSELECTEDITEM_START:
		break;
	case ListViewEventType::LISTVIEW_ONSELECTEDITEM_END:
		Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
		App::GetInstance()->pushScene(MySceneName::IndexScene);
		maskAnimation(item, listLocation);
		break;
	default:
		break;
	}
}

Layer* Index::WaitLayer()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(PLIST_WAITMESSAGEBOX);
	Data data;
	Layer* wait;
	ActionTimeline* timeline;
	if (App::GetInstance()->getData(WAITMESSAGEBOX_CSB, data))
	{
		wait = (Layer*)CSLoader::createNode(data);
		timeline = CSLoader::createTimeline(data, WAITMESSAGEBOX_CSB);
	}
	else
	{
		wait = (Layer*)CSLoader::createNode(WAITMESSAGEBOX_CSB);
		timeline = CSLoader::createTimeline(WAITMESSAGEBOX_CSB);
	}
	wait->setAnchorPoint(Vec2(0.5f, 0.5f));
	wait->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));	
	wait->runAction(timeline);
	timeline->gotoFrameAndPlay(0, 28, true);
	auto imageview = (ImageView*)wait->getChildByName(FIND_IMAGEVIEW_BY_NAME_WAITMESSAGEBOX); 
	imageview->setTouchEnabled(true);
	imageview->setSwallowTouches(true);
	imageview->addClickEventListener([](Ref* sender) {
		CCLOG("dianji");
	});
	return wait;
}

Layer* Index::SelectLayer(std::function<void()> fun)
{
	Data data;
	Node* parentMessageBox;
	if (App::GetInstance()->getData(PARENTMESSAGEBOX_CSB,data))
	{
		parentMessageBox = CSLoader::createNode(data);
	}
	else
	{
		parentMessageBox = CSLoader::createNode(PARENTMESSAGEBOX_CSB);
	}
	parentMessageBox->setAnchorPoint(Vec2(0.5, 0.5));
	Size visibleSize = Director::getInstance()->getVisibleSize();
	parentMessageBox->setPosition(visibleSize / 2);
	//关闭按钮
	auto closeButton = ( Button*)parentMessageBox->getChildByName(FIND_BUTTON_BY_NAME_PARENT_CLOSE);
	//左1
	auto first = ( ImageView*)parentMessageBox->getChildByName(PARENTMESSAGEBOX_FIND_IMAGVIEW_BYNAME_FIRST);
	//左2
	auto second = ( ImageView*)parentMessageBox->getChildByName(PARENTMESSAGEBOX_FIND_IMAGVIEW_BYNAME_SECOND);
	//左3
	auto three = ( ImageView*)parentMessageBox->getChildByName(PARENTMESSAGEBOX_FIND_IMAGVIEW_BYNAME_THREE);
	//背景
	auto bg = ( ImageView*)parentMessageBox->getChildByName(PARENTMESSAGEBOX_FIND_IMAGVIEW_BYNAME_BG);
	//文本
	auto text = (Text*)parentMessageBox->getChildByName(PARENTMESSAGEBOX_FIND_TEXT_BYNAME);
	closeButton->setTouchEnabled(true);
	first->setTouchEnabled(true);
	second->setTouchEnabled(true);
	three->setTouchEnabled(true);
	bg->setTouchEnabled(true);
	//关闭按钮点击事件
	closeButton->addClickEventListener([=](Ref* sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
		YYXLayer::PLAYBUTTON;
		closeButton->setTouchEnabled(false);
		parentMessageBox->removeFromParent();
	});
	//背景屏蔽层
	bg->setSwallowTouches(true);
	bg->addClickEventListener([](Ref* sender) {});
	//定义红色 = 0  黄色 = 1 蓝色 = 2    摆放顺序 从大到小
	srand((int)App::getCurrentTime());
	auto red = rand() % 100;
	auto blue = rand() % 100;
	auto yellow = rand() % 100;
	CCLOG("r = %d , b = %d ,  y = %d", red, blue, yellow);
	//----------------------------------------复用函数------------------------------------
	auto loadUnselect = [](int tag, ImageView* imageview) {
		switch (tag)
		{
		case 0:
			imageview->loadTexture(PICTURE_RED_UNSELECT, TextureResType::PLIST);
			break;
		case 1:
			imageview->loadTexture(PICTURE_YELLOW_UNSELECT, TextureResType::PLIST);
			break;
		case 2:
			imageview->loadTexture(PICTURE_BLUE_UNSELECT, TextureResType::PLIST);
			break;
		}
	};
	auto loadSelect = [](int tag, ImageView* imageview) {
		switch (tag)
		{
		case 0:
			imageview->loadTexture(PICTURE_RED_SELECT, TextureResType::PLIST);
			break;
		case 1:
			imageview->loadTexture(PICTURE_YELLOW_SELECT, TextureResType::PLIST);
			break;
		case 2:
			imageview->loadTexture(PICTURE_BLUE_SELECT, TextureResType::PLIST);
			break;
		}
	};
	if (yellow > (red > blue ? red : blue))
	{
		//黄色最大
		loadUnselect(1,first);
		first->setTag(1);
		if (red > blue)
		{
			//红色第2
			loadUnselect(0,second);
			second->setTag(0);
			loadUnselect(2, three);
			three->setTag(2);
		}
		else
		{
			//蓝色第2
			loadUnselect(2, second);
			second->setTag(2);
			loadUnselect(0, three);
			three->setTag(0);
		}
	}
	else
	{
		if (red > blue)
		{
			//红色最大
			loadUnselect(0, first);
			first->setTag(0);
			if (blue > yellow)
			{
				//蓝色第二
				loadUnselect(2, second);
				second->setTag(2);
				loadUnselect(1, three);
				three->setTag(1);
			}
			else
			{
				//黄色第二
				loadUnselect(1, second);
				second->setTag(1);
				loadUnselect(2, three);
				three->setTag(2);
			}
		}
		else
		{
			//蓝色最大
			loadUnselect(2, first);
			first->setTag(2);
			if (red > yellow)
			{
				//红色第二
				loadUnselect(0, second);
				second->setTag(0);
				loadUnselect(1, three);
				three->setTag(1);
			}
			else
			{
				//黄色第二
				loadUnselect(1, second);
				second->setTag(1);
				loadUnselect(0, three);
				three->setTag(0);
			}
		}
	}
	auto selectColor = rand() % 3;
	switch (selectColor)
	{
	case 0:
		text->setString(App::getString("STR_RED"));
		text->setTag(0);
		break;
	case 1:
		text->setString(App::getString("STR_YELLOW"));
		text->setTag(1);
		break;
	case 2:
		text->setString(App::getString("STR_BLUE"));
		text->setTag(2);
		break;
	}
	auto lister = [=](Ref* sender) {
		auto imageview = (ImageView*)sender;
		auto tag =  imageview->getTag();
		auto answer = text->getTag();
		loadUnselect(first->getTag(),first);
		loadUnselect(second->getTag(), second);
		loadUnselect(three->getTag(), three);
		loadSelect(tag,imageview);
		if (tag == answer)
		{
			parentMessageBox->removeFromParent();
			fun();
		}
		else
		{
			parentMessageBox->removeFromParent();
		}
	};
	first->addClickEventListener(lister);
	second->addClickEventListener(lister);
	three->addClickEventListener(lister);
	return (Layer*)parentMessageBox;
}

//蒙版动画
void Index::maskAnimation(Layout* layout, Point point)
{
	//auto touchSp = ImageView::create();

	auto touchSp = (ImageView*)layout;
	if (layout->getTag() == LOCOMOTIVE)
		return;
	LayerColor* maskColor = LayerColor::create(ccc4(0, 0, 0, 255));

	auto clipingNode = ClippingNode::create();
	clipingNode->setInverted(true);
	clipingNode->setAlphaThreshold(0.0f);
	clipingNode->addChild(maskColor);

	Sprite* clipSprite = Sprite::create();
	clipSprite->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(PICTURE_INDEXSCENE_CLIPPING));
	clipSprite->getTexture()->setAliasTexParameters();
	clipSprite->setPosition(ccpAdd(touchSp->getPosition(), ccp(point.x + 370 * 0.4, point.y + 530 * 0.4)));
	//CCLOG("touchSp= (%f,%f)", touchSp->getPosition().x, touchSp->getPosition().y);
	//CCLOG("point= (%f,%f)", point.x, point.y);
	clipingNode->setStencil(clipSprite);
	auto scaleTo01 = ScaleTo::create(0.5f, 0.7f);
	auto scaleTo02 = ScaleTo::create(0.4f, 0);
	clipSprite->runAction(Sequence::create(CallFuncN::create([=](cocos2d::Ref * sender) {
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
		YYXLayer::PLAYBUTTON;
	}), EaseSineIn::create(scaleTo01), DelayTime::create(0.2f), EaseSineOut::create(scaleTo02), DelayTime::create(0.1f), CallFuncN::create([=](cocos2d::Ref * sender) {
		switch (layout->getTag())
		{
		case LOCOMOTIVE:				
			break;
		case BOOKCITY:			
			GoToBookCity();
			break;
		case PICTUREBOOK:
			GoToPictureBook();
			break;
		case RECOMMEND:
			GoToRecommend();
			break;
		case LIMITFREE:
			GoToFree();
			break;
		case EXCELLENT_FIVE_STAR:
			GoToGoodReputation();
			break;
		case NEWBOOK:
			GoToNewBook();
			break;
		case KANGXUANBOOKSTORE:
			GoToKangXuanStore();
			break;
		case VIPBOOKSTORE:
			GoToVIPBook();
			break;
		default:
			break;
		}
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, "showYouAreVip", 1);
	}), NULL));
	this->addChild(clipingNode);
}

void Index::showNotification() {
	YYXLayer::logb("Index::showNotification()");
	if(App::GetInstance()->getFromScene() != MySceneName::LoadScene)
		return;
	int pushid = YYXStruct::getMapInt64(App::GetInstance()->myData, "pushId", -999);
	//if (App::GetInstance()->m_notification == nullptr) 
	if(pushid == -999)
		return;
	int oldPushId = 0;
	string oldPushIdstr = YYXLayer::getFileValue("notification_push_id", "0");
	oldPushId = atoi(oldPushIdstr.c_str());
	App::log("           oldPushId = ", oldPushId);
	if (pushid <= oldPushId) {
		return;
	}
	//展示推送消息
	auto show = ShowNotification::create(false);
	if(show)
		addChild(show);
	YYXLayer::loge("Index::showNotification()");
}

void Index::dealEllaVersion() {
	YYXLayer::logb(" Index::dealEllaVersion()");	
	if (App::GetInstance()->getFromScene() != MySceneName::LoadScene)
		return;
	string netVersionStr = YYXStruct::getMapString(App::GetInstance()->myData, "netVersion", "");
	int netVersionInt = YYXStruct::getMapInt64(App::GetInstance()->myData, "netVersion", 0);
	if (netVersionInt == 0 || netVersionStr == "")
		return;
	auto numstr = App::GetInstance()->version;
	auto index1 = numstr.find_first_of(".");
	auto index2 = numstr.find_last_of(".");
	int A = atoi(numstr.substr(0, 1).c_str()) * 10000;
	int B = atoi(numstr.substr(index1 + 1, 1).c_str()) * 1000;
	int C = atoi(numstr.substr(index2 + 1).c_str());
	int appVersionNumber = A + B  + C;
	App::log("server version= "+ netVersionStr, netVersionInt);
	App::log("location version= "+ numstr , appVersionNumber);
	if (strcmp(numstr.c_str(), netVersionStr.c_str()) == 0)
		return;
	else
	{
		if (netVersionInt > appVersionNumber)
		{
			//展示版本更新
			auto show = ShowNotification::create(true);
			if(show)
				addChild(show);
		}
	}	
	YYXLayer::loge(" Index::dealEllaVersion()");
}

void Index::showRedPacket()
{
	//界面展示红包
	auto redcount = App::GetInstance()->m_redPacket.size();
	if (redcount <= 0)
		return;
	map<string, int64String> paramter;
	YYXLayer::insertMap4ParaType(paramter, "className", -999, "gongxihuodehongbao");
	YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_GONGXIHUODEYILAHONGBAO_CSB);
	auto redPacketLayer = YYXLayer::create(paramter);
	if (!redPacketLayer)
		return;
	auto listview = (ListView*)redPacketLayer->findControl(FIND_LISTVIEW_BY_NAME_SELECTREDPACKET_REDPACKET);
	listview->setScrollBarAutoHideEnabled(false);
	listview->setScrollBarEnabled(true);
	auto closeButton = (Button*)redPacketLayer->findControl(FIND_BUTTON_BY_NAME_GONGXIHUODEYILAHONGBAO_CLOSE);
	closeButton->setTouchEnabled(true);
	closeButton->addClickEventListener([=](Ref* sender) {
		redPacketLayer->removeFromParentAndCleanup(true);
	});
	redPacketLayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
	redPacketLayer->setPosition(Director::getInstance()->getVisibleSize() / 2);
	listViewShowRedPacket(listview);
	//auto eventlist = EventListenerCustom::create("referRedPackets", [=](EventCustom* e) {
		//listViewShowRedPacket(listview);
	//});
	//Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventlist, redPacketLayer);
	addChild(redPacketLayer);
}

void Index::listViewShowRedPacket(ListView* listview)
{
	if (!listview)
	{
		App::log("listview is null");
		return;
	}
	listview->removeAllItems();
	auto data = App::GetInstance()->m_redPacket;
	for (int i = 0; i < data.size(); i++)
	{
		Layout* layout = Layout::create();
		map<string, int64String> paramter;
		YYXLayer::insertMap4ParaType(paramter, "className", -999, "BookInfo");
		YYXLayer::insertMap4ParaType(paramter, "csb", -999, PAY_SELECTREDPACKET_ITEM_CSB);
		auto item = YYXLayer::create(paramter);
		auto leftRedPacket = (Node*)item->findControl(PAY_SELECTREDPACKET_ITEM_LEFT);
		auto rightRedPacket = (Node*)item->findControl(PAY_SELECTREDPACKET_ITEM_RIGHT);
		loadRedPacketData(leftRedPacket, i);
		loadRedPacketData(rightRedPacket, i + 1);
		item->getParentNode()->setAnchorPoint(Vec2(0, 0));
		if (i + 1 >= data.size())
			rightRedPacket->setVisible(false);
		layout->addChild(item);
		layout->setContentSize(Size(500, 200));
		listview->pushBackCustomItem(layout);
		i++;
	}
	listview->jumpToTop();
}

//加载红包数据到节点上
void Index::loadRedPacketData(Node* node, int index)
{
	bool show = true;
	auto data = App::GetInstance()->m_redPacket;
	if (index >= data.size())
		return;
	int coupon_id = YYXStruct::getMapInt64(data[index], "coupon_id", -999);
	int coupon_amount = YYXStruct::getMapInt64(data[index], "coupon_amount", -999);
	auto coupon_expire_time = YYXStruct::getMapString(data[index], "coupon_expire_time", "");

	if (coupon_id == -999 || coupon_amount == -999 || coupon_expire_time == "")
		show = false;
	if (node)
		node->setVisible(show);
	else
		return;
	if (show)
	{
		//有效期
		auto text_coupon_expire_time = (Text*)node->getChildByName(FIND_TEXT_BY_NAME_EVERYREDPACKET_TIME);
		if (text_coupon_expire_time)
		{
			coupon_expire_time = coupon_expire_time.substr(0, 10);
			text_coupon_expire_time->setText(App::getString("YOUXIAOQIZHI") + coupon_expire_time);
		}
		//红包价值
		auto text_coupon_amount = (Text*)node->getChildByName(FIND_TEXT_BY_NAME_EVERYREDPACKET_PRICE);
		if (text_coupon_amount)
			text_coupon_amount->setText(StringUtils::format("%d", (int)(coupon_amount / 100)) + App::getString("YUAN"));
		//打钩
		auto img_couponSelect = (ImageView*)node->getChildByName(FIND_IMAGEVIE_BY_NAME_EVERYREDPACKET_SELECT);
		img_couponSelect->setVisible(false);
		//红包封面
		auto redPacket = (ImageView*)node->getChildByName(FIND_IMAGEVIE_BY_NAME_EVERYREDPACKET_COVER);
	}
}

//续费vip
YYXLayer* Index::showVIPRenew()
{
	return nullptr;
	if (App::GetInstance()->getFromScene() != MySceneName::LoadScene)
		return nullptr;
	if (App::GetInstance()->m_me)
	{
		App::log("           you are vip Time= ", App::GetInstance()->m_me->vipTime);
		if (App::GetInstance()->m_me->vip /*&& App::GetInstance()->m_me->vipTime < 2592000*/)
		{			
			map<string, int64String> paramter;
			YYXLayer::insertMap4ParaType(paramter, "className", -999, "IndexSceneShowVIPRenew");
			YYXLayer::insertMap4ParaType(paramter, "csb", -999, "VIPBook/csb/renew.csb");
			auto viphint = YYXLayer::create(paramter);
			auto pNode = viphint->getParentNode();
			pNode->setAnchorPoint(Vec2(0.5, 0.5));
			pNode->setPosition(Director::getInstance()->getWinSize() / 2);
			auto bclose = (Button*)viphint->findControl("Button_1");
			if (bclose)
			{
				bclose->addClickEventListener([=](Ref* sender) {
					viphint->removeFromParent();
				});
			}
			auto time = (Text*)viphint->findControl("Text_6_0_0");
			if (time)
			{
				time->setText(StringUtils::format("come %s to %s,vipTime = %d", App::GetInstance()->m_me->startvip.c_str(), App::GetInstance()->m_me->endvip.c_str(), App::GetInstance()->m_me->vipTime));
			}
			auto acc = (Text*)viphint->findControl("Text_6_0");
			if (acc)
			{
				acc->setText(StringUtils::format("memberID = %d", App::GetInstance()->m_me->id));
			}
			return viphint;
		}
	}	
	return nullptr;
}