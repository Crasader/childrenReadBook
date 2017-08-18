#include "YYXTableView.h"
#include "YYXImageView.h"
#include "CrossPlatform.h"
#include "YYXSound.h"
#include "AppHttp.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define LAYOUTTAG 1024
#define ACTION 1025

bool YYXTableView::init()
{
	m_tv = TableView::create(this, Size(600, 350));
	m_tv->setAnchorPoint(Vec2(0, 0));
	m_tv->setPosition(Vec2(0, 0));
	m_tv->setDelegate(this);
	m_tv->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
	m_tv->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	addChild(m_tv);
	return true;
}

vector<map<string, YYXStruct>> YYXTableView::loadData(int bookid, int memberID)
{
	m_bookid = bookid;
	m_memberID = memberID;
	m_data.clear();
	int count = YYXStruct::getMapInt64(App::GetInstance()->myData, StringUtils::format("comment_bookid=%d", bookid), 0);
	for (int i = 0; i < count; i++)
	{
		string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, i);//bookid + 下标顺序
		int commentID = YYXStruct::getMapInt64(App::GetInstance()->myData, idKey, -999);//评论id + 类型
		string type = YYXStruct::getMapString(App::GetInstance()->myData, idKey, "");
		if (commentID == -999)
			continue;
		string memberName = "";
		int score = 0;
		string contentText = "";
		string titileText = "";
		int commentTime = 0;
		int voiceLength = 0;
		string url = "";
		int commentType = 0;
		if (type == "TEXT")
		{
			commentType = 0;
			string contentKey = StringUtils::format("comment_gevalId=%d+score+content", commentID);
			string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", commentID);
			string titleKey = StringUtils::format("comment_gevalId=%d+title", commentID);

			contentText = YYXStruct::getMapString(App::GetInstance()->myData, contentKey, "");//文字内容			
			titileText = YYXStruct::getMapString(App::GetInstance()->myData, titleKey, "");//文字标题
			memberName = YYXStruct::getMapString(App::GetInstance()->myData, timeKey, "132****1434");//用户名
			score = YYXStruct::getMapInt64(App::GetInstance()->myData, contentKey, 4);//星级
			commentTime = YYXStruct::getMapInt64(App::GetInstance()->myData, timeKey, 0);//评论时间
		}
		else if (type == "VOICE")
		{
			commentType = 1;
			string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", commentID);
			string voiceKey = StringUtils::format("comment_gevalId=%d+score+url+gevalTime", commentID);

			memberName = YYXStruct::getMapString(App::GetInstance()->myData, timeKey, "132****1434");//用户名
			score = YYXStruct::getMapInt64(App::GetInstance()->myData, voiceKey, 3);//星级
			commentTime = YYXStruct::getMapInt64(App::GetInstance()->myData, timeKey, 0);//评论时间
			voiceLength = (long)YYXStruct::getMapRef(App::GetInstance()->myData, voiceKey, 0);//语音时间
			url = YYXStruct::getMapString(App::GetInstance()->myData, voiceKey, "");//语音url
		}
		if (memberName.length() == 11)
			memberName.replace(3, 4, "****");
		string AvatarUrlKey = StringUtils::format("comment_gevalId=%d+memberId+AvatarUrl", commentID);
		int memberId = YYXStruct::getMapInt64(App::GetInstance()->myData, AvatarUrlKey, 0);//用户id
		auto AvatarUrl = YYXStruct::getMapString(App::GetInstance()->myData, AvatarUrlKey, "");//头像url
		if (memberID == -1 || memberId == memberID)
		{
			map<string, YYXStruct>val;
			val["headportait"] = YYXStruct(memberId, AvatarUrl, nullptr);
			val["userName"] = YYXStruct(bookid, memberName, nullptr);
			val["startCount"] = YYXStruct(score, "", nullptr);
			val["context"] = YYXStruct(-999, contentText, nullptr);
			val["type"] = YYXStruct(commentType, "", nullptr);
			val["voicelength"] = YYXStruct(voiceLength, "", nullptr);
			val["time"] = YYXStruct(commentTime, "", nullptr);
			string dir = FileUtils::getInstance()->getWritablePath() + "voiceComment";
			string filename = StringUtils::format("%d.mp3", commentID);
			val["voicepath"] = YYXStruct(commentID, dir + "/" + filename, nullptr);
			m_data.push_back(val);
		}
	}
	soundPlayCompletes();
	m_tv->reloadData();
	return m_data;
}

vector<map<string, YYXStruct>> YYXTableView::loadData(int bookid)
{
	m_bookid = bookid;
	m_data.clear();
	int count = YYXStruct::getMapInt64(App::GetInstance()->myData, StringUtils::format("comment_bookid=%d", bookid), 0);
	for (int i = 0; i < count; i++)
	{
		string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, i);//bookid + 下标顺序
		int commentID = YYXStruct::getMapInt64(App::GetInstance()->myData, idKey, -999);//评论id + 类型
		string type = YYXStruct::getMapString(App::GetInstance()->myData, idKey, "");
		if (commentID == -999)
			continue;
		string memberName = "";
		int score = 0;
		string contentText = "";
		string titileText = "";
		int commentTime = 0;
		int voiceLength = 0;
		string url = "";
		int commentType = 0;
		if (type == "TEXT")
		{
			commentType = 0;
			string contentKey = StringUtils::format("comment_gevalId=%d+score+content", commentID);
			string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", commentID);
			string titleKey = StringUtils::format("comment_gevalId=%d+title", commentID);

			contentText = YYXStruct::getMapString(App::GetInstance()->myData, contentKey, "");//文字内容			
			titileText = YYXStruct::getMapString(App::GetInstance()->myData, titleKey, "");//文字标题
			memberName = YYXStruct::getMapString(App::GetInstance()->myData, timeKey, "132****1434");//用户名
			score = YYXStruct::getMapInt64(App::GetInstance()->myData, contentKey, 4);//星级
			commentTime = YYXStruct::getMapInt64(App::GetInstance()->myData, timeKey, 0);//评论时间
		}
		else if (type == "VOICE")
		{
			commentType = 1;
			string timeKey = StringUtils::format("comment_gevalId=%d+commentTime+memberName+memberId", commentID);
			string voiceKey = StringUtils::format("comment_gevalId=%d+score+url+gevalTime", commentID);

			memberName = YYXStruct::getMapString(App::GetInstance()->myData, timeKey, "132****1434");//用户名
			score = YYXStruct::getMapInt64(App::GetInstance()->myData, voiceKey, 3);//星级
			commentTime = YYXStruct::getMapInt64(App::GetInstance()->myData, timeKey, 0);//评论时间
			voiceLength = (int)YYXStruct::getMapRef(App::GetInstance()->myData, voiceKey, 0);//语音时间
			url = YYXStruct::getMapString(App::GetInstance()->myData, voiceKey, "");//语音url
		}
		if(memberName.length() == 11)
			memberName.replace(3, 4, "****");
		string AvatarUrlKey = StringUtils::format("comment_gevalId=%d+memberId+AvatarUrl", commentID);
		int memberId = YYXStruct::getMapInt64(App::GetInstance()->myData, AvatarUrlKey, 0);//用户id
		auto AvatarUrl = YYXStruct::getMapString(App::GetInstance()->myData, AvatarUrlKey, "");//头像url
		map<string, YYXStruct>val;
		val["headportait"] = YYXStruct(memberId, AvatarUrl, nullptr);
		val["userName"] = YYXStruct(bookid, memberName, nullptr);
		val["startCount"] = YYXStruct(score, "", nullptr);
		val["context"] = YYXStruct(-999, contentText, nullptr);
		val["type"] = YYXStruct(commentType, "", nullptr);
		val["voicelength"] = YYXStruct(voiceLength, "", nullptr);
		val["time"] = YYXStruct(commentTime, "", nullptr);
		string dir = FileUtils::getInstance()->getWritablePath() + "voiceComment";
		string filename = StringUtils::format("%d.mp3", commentID);
		val["voicepath"] = YYXStruct(commentID, dir + "/" + filename, nullptr);
		m_data.push_back(val);
	}
	soundPlayCompletes();
	m_tv->reloadData();
	return m_data;
}

void YYXTableView::tableCellTouched(TableView* table, TableViewCell* cell)
{
	auto index = cell->getIdx();
	auto data = m_data[index];
	auto memberId = YYXStruct::getMapInt64(data, "headportait", -999);
	if (memberId > 0 && App::GetInstance()->getMemberId() == memberId)
	{
		handleYourOwnComments(table, cell);
	}
	else
	{
		handleOtherComments(table, cell);
	}
}

TableViewCell* YYXTableView::tableCellAtIndex(TableView *table, ssize_t idx)
{
	auto cell = table->dequeueCell();
	auto data = m_data[idx];
	if (!cell)
	{
		cell = TableViewCell::create();
		cell->addChild(createItem(data, idx));
	}
	else
	{
		reuseItem(cell, data, idx);
	}
	return cell;
}

Size YYXTableView::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	auto data = m_data[idx];
	auto types = YYXStruct::getMapInt64(data, "type", 1);
	if (types == 0)
	{
		auto context = YYXStruct::getMapString(data, "context", "");
		auto teext = Label::create(context, "FZHLJW.TTF", 24 * 2);
		teext->setMaxLineWidth(450 * 2);
		return Size(600, teext->getContentSize().height/2+100);
	}
	else
		return Size(600, 130);
}

ssize_t YYXTableView::numberOfCellsInTableView(TableView *table)
{
	return m_data.size();
}

void YYXTableView::loadData(map<string, YYXStruct> dat)
{
	m_data.push_back(dat);
}

Layout* YYXTableView::createItem(map<string, YYXStruct>  data, int idx)
{
	auto layout = Layout::create();
	layout->setLayoutType(cocos2d::ui::Layout::Type::RELATIVE);
	//layout->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
	//layout->setColor(Color3B::YELLOW);
	layout->setTag(LAYOUTTAG);

	//头像
	auto AvatarUrl = YYXStruct::getMapString(data, "headportait", "");
	auto memberId = YYXStruct::getMapInt64(data, "headportait", -999);
	auto headView = YYXImageView::create(AvatarUrl, memberId, "other/User_bg_ipad@2x.png");
	headView->setTag(1);
	headView->ignoreContentAdaptWithSize(false);
	headView->setSize(Size(70*2,70*2));
	//if (FileUtils::getInstance()->isFileExist(headportait))
	//	headView->loadTexture(headportait);
	//else
	//	headView->loadTexture("other/User_bg_ipad@2x.png");
	auto headViewParameter = RelativeLayoutParameter::create();
	headViewParameter->setRelativeName("headView");
	headViewParameter->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_TOP_LEFT);
	headViewParameter->setMargin(Margin(20*2, 15*2, 0, 0));
	headView->setLayoutParameter(headViewParameter);
	layout->addChild(headView);

	//分界线
	auto xian = ImageView::create("other/block.png");
	xian->ignoreContentAdaptWithSize(false);
	xian->setContentSize(Size(600 * 2, 4));
	auto xianParameter = RelativeLayoutParameter::create();
	xianParameter->setRelativeName("text");
	xianParameter->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_BOTTOM);
	xianParameter->setMargin(Margin(0, 0, 0, 0));
	xian->setLayoutParameter(xianParameter);
	layout->addChild(xian);

	//用户名
	auto userName = YYXStruct::getMapString(data, "userName", "132****3254");
	auto name = Text::create("131****1245", "wryh.TTF", 16*2);
	name->setTag(2);
	name->setColor(Color3B(90, 160, 141));
	auto nameParameter = RelativeLayoutParameter::create();
	nameParameter->setRelativeName("name");
	nameParameter->setAlign(RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_CENTER);
	nameParameter->setMargin(Margin(0, 2*2, 0, 0));
	nameParameter->setRelativeToWidgetName("headView");
	name->setLayoutParameter(nameParameter);
	name->setText(userName);
	layout->addChild(name);

	//星级
	int startCount = YYXStruct::getMapInt64(data, "startCount", 5);
	auto xingji = ImageView::create();
	xingji->setTag(3);
	xingji->ignoreContentAdaptWithSize(false);
	xingji->setSize(Size(207, 39));
	string path = "";
	switch (startCount)
	{
	case 1:
		path = "other/Backcover_star1_736h.png";
		break;
	case 2:
		path = "other/Backcover_star2_736h.png";
		break;
	case 3:
		path = "other/Backcover_star3_736h.png";
		break;
	case 4:
		path = "other/Backcover_star4_736h.png";
		break;
	default:
		path = "other/Backcover_star5_736h.png";
		break;
	}
	if(FileUtils::getInstance()->isFileExist(path))
		xingji->loadTexture(path);
	auto xingjiParameter = RelativeLayoutParameter::create();
	xingjiParameter->setRelativeName("xingji");
	xingjiParameter->setAlign(RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_TOPALIGN);
	xingjiParameter->setRelativeToWidgetName("headView");
	xingjiParameter->setMargin(Margin(40*2, 0*2, 0, 0));
	xingji->setLayoutParameter(xingjiParameter);
	layout->addChild(xingji);

	//文字
	auto context = YYXStruct::getMapString(data, "context", "");
	auto teext = Label::create("", "FZHLJW.TTF", 24*2);
	teext->setTag(4);
	teext->setColor(Color3B(69, 69, 69));
	teext->setMaxLineWidth(450*2);
	teext->setAnchorPoint(Vec2(0, 0));
	teext->setString(context);
	teext->setPosition(Vec2(130 * 2, 40 * 2));
	layout->addChild(teext);

	//语音
	auto yuying = ImageView::create("other/Backcover_voice_background_736h.png");
	yuying->setTag(5);
	auto yuyingParameter = RelativeLayoutParameter::create();
	yuyingParameter->setRelativeName("yuying");
	yuyingParameter->setAlign(RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_LEFTALIGN);
	yuyingParameter->setRelativeToWidgetName("xingji");
	yuyingParameter->setMargin(Margin(0, 15*2, 0, 0));
	yuying->setLayoutParameter(yuyingParameter);
	layout->addChild(yuying);
	yuying->setTouchEnabled(true);
	yuying->setSwallowTouches(false);
	yuying->addClickEventListener([=](Ref* sender) {
		auto memberId = YYXStruct::getMapInt64(data, "headportait", -999);
		if (memberId > 0 && App::GetInstance()->getMemberId() == memberId)
			m_myVoiceAble = true;
	});

	//语音动画
	auto voiceSp = Sprite::create();
	voiceSp->setTag(6);
	showAnimationEnd(voiceSp);
	auto yuyingbox = yuying->getSize();
	voiceSp->setPosition(Vec2(yuyingbox.width/5, yuyingbox.height /2));
	yuying->addChild(voiceSp);

	//语音长度
	int voicelen = YYXStruct::getMapInt64(data, "voicelength", 18);	
	auto voicelength = Text::create("8\"", "Impact.TTF", 22 * 2);
	voicelength->setTag(7);
	voicelength->setColor(Color3B(17, 106, 95));
	voicelength->setPosition(Vec2(yuyingbox.width*7 / 10, yuyingbox.height / 2-4));
	voicelength->setString(StringUtils::format("%d \"", voicelen));
	yuying->addChild(voicelength);
	
	//日期
	auto time = YYXStruct::getMapInt64(data, "time", 0);
	auto day = Text::create("2016-12-12", "wryh.TTF", 14 * 2);
	day->setTag(8);
	day->setColor(Color3B(151, 151, 151));
	auto dayParameter = RelativeLayoutParameter::create();
	dayParameter->setRelativeName("day");
	dayParameter->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_RIGHT_BOTTOM);
	dayParameter->setMargin(Margin(0, 0, 30*2, 10*2));
	day->setLayoutParameter(dayParameter);
	string timestr = App::getFormatTime(time);
	day->setText(timestr);
	layout->addChild(day);

	int typ = YYXStruct::getMapInt64(data, "type", 1);
	if (typ == 0)
	{//文字
		yuying->setVisible(false);
	}
	else
	{//语音
		teext->setVisible(false);
	}
	layout->setSize(tableCellSizeForIndex(m_tv, idx)*2);
	layout->setScale(0.5);
	return layout;
}

void YYXTableView::reuseItem(TableViewCell*item, map<string, YYXStruct>  data, int idx)
{
	auto layout = (Layout*)item->getChildByTag(LAYOUTTAG);
	//头像
	auto AvatarUrl = YYXStruct::getMapString(data, "headportait", "");
	auto memberId = YYXStruct::getMapInt64(data, "headportait", -999);
	auto headView = (YYXImageView*)layout->getChildByTag(1);
	headView->setTag(1);
	headView->reloadUrl(AvatarUrl, memberId);
	//if (FileUtils::getInstance()->isFileExist(headportait))
	//	headView->loadTexture(headportait);	
	//else
	//	headView->loadTexture("other/User_bg_ipad@2x.png");

	//用户名
	auto userName = YYXStruct::getMapString(data, "userName", "132****3254");
	auto name = (Text*)layout->getChildByTag(2);
	name->setText(userName);

	//星级
	int startCount = YYXStruct::getMapInt64(data, "startCount", 5);
	auto xingji =( ImageView*)layout->getChildByTag(3);
	string path = "";
	switch (startCount)
	{
	case 1:
		path = "other/Backcover_star1_736h.png";
		break;
	case 2:
		path = "other/Backcover_star2_736h.png";
		break;
	case 3:
		path = "other/Backcover_star3_736h.png";
		break;
	case 4:
		path = "other/Backcover_star4_736h.png";
		break;
	default:
		path = "other/Backcover_star5_736h.png";
		break;
	}
	if (FileUtils::getInstance()->isFileExist(path))
		xingji->loadTexture(path);

	//文字
	auto context = YYXStruct::getMapString(data, "context", "");
	auto teext = (Label*)layout->getChildByTag(4);
	teext->setVisible(true);
	teext->setString(context);

	//语音
	auto yuying = (ImageView*)layout->getChildByTag(5);
	yuying->setVisible(true);
	yuying->setTouchEnabled(true);
	yuying->setSwallowTouches(false);
	yuying->addClickEventListener([=](Ref* sender) {
		auto memberId = YYXStruct::getMapInt64(data, "headportait", -999);
		if (memberId > 0 && App::GetInstance()->getMemberId() == memberId)
			m_myVoiceAble = true;
	});

	//语音动画
	auto voiceSp = (Sprite*)yuying->getChildByTag(6);
	if (m_playingVoiceIdx == idx)
		playVoiceFrameAnimation(voiceSp);
	else
		showAnimationEnd(voiceSp);

	//语音长度
	int voicelen = YYXStruct::getMapInt64(data, "voicelength", 18);
	auto voicelength = (Text*)yuying->getChildByTag(7);
	voicelength->setString(StringUtils::format("%d \"", voicelen));

	//日期
	auto time = YYXStruct::getMapInt64(data, "time", 0);
	auto day = (Text*)layout->getChildByTag(8);
	string timestr = App::getFormatTime(time);
	day->setText(timestr);

	int typ = YYXStruct::getMapInt64(data, "type", 1);
	if (typ == 0)
	{//文字
		yuying->setVisible(false);
		//teext->setPosition(Vec2(130 * 2, 30 * 2));
	}
	else
	{//语音
		teext->setVisible(false);
	}
	layout->setSize(tableCellSizeForIndex(m_tv, idx) * 2);
	layout->setScale(0.5);
}

void YYXTableView::playVoiceFrameAnimation(Sprite* vsp)
{
	auto actin = vsp->getActionByTag(ACTION);
	if (actin == nullptr || actin->isDone())
	{
		auto action = FrameAnimation::createAnimate(3, "other/yuying.plist", "other/Backcover_voice_donghua%d_736h.png", 0.2);
		auto ac = RepeatForever::create(Sequence::create(action, action->reverse(), NULL));
		ac->setTag(ACTION);
		vsp->runAction(ac);
	}
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(vsp);
	auto listener = EventListenerCustom::create(TAG_STOPCOMMENT_SOUND_ANIMATION, [=](EventCustom* e) {
		long sp = (long)e->getUserData();
		if (sp != (long)vsp)
		{
			vsp->stopAllActions();
			showAnimationEnd(vsp);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, vsp);
}

void YYXTableView::showAnimationEnd(Sprite* sp)
{
	auto frameSP = SpriteFrameCache::getInstance()->getSpriteFrameByName("other/Backcover_voice_donghua2_736h.png");
	if (!frameSP)
	{
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("other/yuying.plist");
		frameSP = SpriteFrameCache::getInstance()->getSpriteFrameByName("other/Backcover_voice_donghua2_736h.png");
	}
	sp->setSpriteFrame(frameSP);
}

//处理自己发布的评论
void YYXTableView::handleYourOwnComments(TableView* table, TableViewCell* cell)
{
	int index = cell->getIdx();
	auto data = m_data[index];
	//点击自己发布的评论 判断是在哪个区域点击的
	if (m_myVoiceAble)
	{//播放
		handleOtherComments(table, cell);
	}
	else
	{//删除
		auto message = YYXLayer::MyMessageBox(App::getString("SHANCHUPINGLUN"), App::getString("SHANCHU"), [=]() {
			int commentID = YYXStruct::getMapInt64(data, "voicepath", -999);
			int bookid = YYXStruct::getMapInt64(data, "userName", -999);
			string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);
			YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, idKey);
			string path = FileUtils::getInstance()->getWritablePath() + "voiceComment/" + Value(commentID).asString() + ".mp3";
			if (FileUtils::getInstance()->isFileExist(path))
			{
				FileUtils::getInstance()->removeFile(path);
			}
			AppHttp::getInstance()->httpDeleteComment(commentID, [=]() {
				int bookid = YYXStruct::getMapInt64(data, "userName", -999);
				string idKey = StringUtils::format("comment_bookid=%d+index=%d", bookid, index);
				YYXStruct::deleteMapYYXStruct(App::GetInstance()->myData, idKey);
				YYXLayer::sendNotify("showCommentListView", "", m_memberID);
				AppHttp::getInstance()->httpComments(m_bookid, [=]() {
					YYXLayer::sendNotify("showCommentListView", "", m_memberID);
				});
			}, [=]() {
				AppHttp::getInstance()->httpComments(m_bookid, [=]() {
					YYXLayer::sendNotify("showCommentListView", "", m_memberID);
				});
			});
		}, "", []() {});
		Director::getInstance()->getRunningScene()->addChild(message);
		soundPlayCompletes();
	}
	m_myVoiceAble = false;
}

//声音播放完毕或者终止时 需要处理的事件
void YYXTableView::soundPlayCompletes()
{
	m_playingVoiceIdx = -999;
	YYXLayer::sendNotify(TAG_STOPCOMMENT_SOUND_ANIMATION);
	YYXSound::getInstance()->stopAll();
	YYXSound::getInstance()->resumeBackGroundMusic();
}

//处理别人发布的评论
void YYXTableView::handleOtherComments(TableView* table, TableViewCell* cell)
{
	auto index = cell->getIdx();
	auto data = m_data[index];
	//只处理语音item
	int typ = YYXStruct::getMapInt64(data, "type", 1);
	if (typ == 1)
	{		//mp3
		if (m_playingVoiceIdx == index)
		{			//暂停
			soundPlayCompletes();
		}
		else
		{//播放
			auto voicepath = YYXStruct::getMapString(data, "voicepath", "");
			if (FileUtils::getInstance()->isFileExist(voicepath))
			{
				YYXSound* controlSound = nullptr;
				controlSound = YYXSound::getInstance();
				if (controlSound)
				{
					controlSound->pauseBackGroundMusic();
					controlSound->stopAll();
					controlSound->play(voicepath, [=](int id,string path) {
						m_playingVoiceIdx = -999;
						YYXLayer::sendNotify(TAG_STOPCOMMENT_SOUND_ANIMATION);
						controlSound->resumeBackGroundMusic();
					});
					m_playingVoiceIdx = index;
				}
				//动画
				auto layout = (Layout*)cell->getChildByTag(LAYOUTTAG);
				auto yuying = (ImageView*)layout->getChildByTag(5);
				auto voiceSp = (Sprite*)yuying->getChildByTag(6);
				YYXLayer::sendNotify(TAG_STOPCOMMENT_SOUND_ANIMATION, "", (int)voiceSp);
				playVoiceFrameAnimation(voiceSp);
			}
		}
	}
}

void YYXTableView::stopAllAnimation()//提供给外部的函数
{
	YYXLayer::sendNotify(TAG_STOPCOMMENT_SOUND_ANIMATION);
}