#include "BookRoomScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "YYXFunctionQueue.h"
#include "YYXVisitor.h"

USING_NS_CC;
using namespace cocostudio::timeline;
using namespace std;


BookRoom::BookRoom() {
	YYXLayer::logb("BookRoom::BookRoom()");
	YYXLayer::loge("BookRoom::BookRoom()");
}

BookRoom::~BookRoom() {
	YYXLayer::logb("BookRoom::~BookRoom()");
	YYXLayer::loge("BookRoom::~BookRoom()");
}

Scene* BookRoom::createScene()
{
    auto scene = Scene::create();
    auto layer = BookRoom::create();
    scene->addChild(layer);
    return scene;
}

bool BookRoom::init()
{
	YYXLayer::logb("BookRoom::init()");
    if ( !Layer::init() )
    {
        return false;
    }
	YYXVisitor::getInstance()->bookRoomSceneInit();
	m_offline = !NetIntface::IsNetConnect();
	YYXFunctionQueue::GetInstance()->clear();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	App::m_RunningScene = MySceneName::BookRoomScene;
	Data data;
	if (App::GetInstance()->getData(BOOKROOM_CSB, data))
		layer = (Layer*)CSLoader::createNode(data);
	else
		layer = (Layer*)CSLoader::createNode(BOOKROOM_CSB);
	layer->setAnchorPoint(Vec2(0.5f, 0.5f));
	layer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(layer);
	m_currentPageNumber = 0;
	m_currentPageNumber = App::GetInstance()->m_showSceneData.intData;
	string bookmode = App::GetInstance()->m_showSceneData.stringData;
	bookMode = atoi(bookmode.c_str());
	if (bookMode == 0 || bookMode == 2 || bookMode == 3 || bookMode == 4)
		App::log("                             bookmode= " + bookmode);
	else
		bookMode = 0;
	if (m_currentPageNumber < 0 || m_currentPageNumber >1000)
		m_currentPageNumber = 0;
	//1.展示白天或者黑夜
	if (!App::isNight()) {
		//auto sprNight = (Sprite*)layer->getChildByName(FIND_SPRITE_BY_NAME_NIGHT_PIC);
		//sprNight->setVisible(false);
	}
	auto name = Label::create();
	name->setPosition(Vec2(550,550));
	name->setTTFConfig(App::m_ttf);
	addChild(name, 1);
	//Director::getInstance()->getEventDispatcher()->addCustomEventListener("showName", [=](EventCustom* e) {
	//	int st = (int)e->getUserData();
	//	switch (st)
	//	{
	//	case 1:
	//		name->setString(App::getString("STR_BUY"));
	//		break;
	//	case 2:
	//		name->setString(App::getString("STR_DOWNLOAD"));
	//		break;
	//	case 3:
	//		name->setString(App::getString("STR_READ"));
	//		break;
	//	case 4:
	//		name->setString(App::getString("VIPXIAZAI"));
	//		break;
	//	default:
	//		break;
	//	}		
	//});
	//2.无登录
	//if (!App::GetInstance()->m_me) 
		//return true;
	
	//蒙板 长按书籍常亮效果
	auto backgro = (ImageView*)layer->getChildByName("Image_3");
	auto booknode = layer->getChildByName("book0_0");
	booknode->setVisible(false);
	backgro->setVisible(false);
	auto cover = (ImageView*)booknode->getChildByName("Image");
	cover->setTouchEnabled(true);
	cover->addClickEventListener([=](Ref* sender) {});
	cover->setSwallowTouches(true);
	auto listeners = EventListenerCustom::create("bookRoomScenemengban", [=](EventCustom* e) {
		long posi = (long)e->getUserData();
		changLongClickBookPosition(backgro, booknode, posi);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listeners, layer);
	//左边树枝
	auto lefttree = (Sprite*)layer->getChildByName(FIND_SPRITE_BY_NAME_LEFT_TREE);
	lefttree->setPositionX((1094 - visibleSize.width) / 2);
	//右边树枝
	auto righttree = (Sprite*)layer->getChildByName(FIND_SPRITE_BY_NAME_RIGHT_TREE);
	righttree->setPositionX((1094 + visibleSize.width) / 2);
	//树杈
	auto tree = (Sprite*)layer->getChildByName(FIND_SPRITE_BY_NAME_BRANCH);
	tree->setPositionX((1094 + visibleSize.width) / 2);
	//咿啦
	auto ellaBird = ( ImageView*)tree->getChildByTag(34);
	ellaBird->setTouchEnabled(true);
	ellaBird->addClickEventListener([=](Ref* sender) {
		if (!m_click)
			return;	
		if (App::m_debug == 0)
		{
			App::runTestFunction();
		}
	});
	//页数圆圈节点
	pageCircle = Node::create();
	pageCircle->setPosition(Vec2(0, 0));
	addChild(pageCircle, 1);
	//翻页圆圈
	moveCircle = Label::create(App::getString("CIRCLE"), "wryh.TTF", 65);
	moveCircle->setScale(0.5f);
	moveCircle->setPosition(Vec2(-100, -100));
	addChild(moveCircle, 1);
	//6本书
	auto shang3ben = layer->getChildByName("Node_1");
	auto xia3ben = layer->getChildByName("Node_2");
	if (shang3ben)
	{
		Node* book0 = shang3ben->getChildByName("book0");
		Node* book1 = shang3ben->getChildByName("book1");
		Node* book2 = shang3ben->getChildByName("book2");
		books.push_back(book0);
		books.push_back(book1);
		books.push_back(book2);
	}
	if (xia3ben)
	{
		Node* book3 = xia3ben->getChildByName("book3");
		Node* book4 = xia3ben->getChildByName("book4");
		Node* book5 = xia3ben->getChildByName("book5");
		books.push_back(book3);
		books.push_back(book4);
		books.push_back(book5);
	}	
	//书籍
	refershPage(bookMode); //购书列表 = 3 已收藏 = 2 已下载  = 0 包年图书 = 4	
	//滑动控制
	auto touchmove = EventListenerTouchOneByOne::create();
	touchmove->onTouchBegan = [=](Touch* t, Event* e) {
		//App::log("BookRoom touchmove");
		m_click = true;
		return true;
	};
	touchmove->onTouchMoved = [=](Touch* t, Event* e) {
		//App::log("BookRoom touchmove onTouchMoved");
		auto moveLocation = t->getStartLocation() - t->getLocation();
		if (moveLocation.x > 30 || moveLocation.x < -30 || moveLocation.y > 30 || moveLocation.y < -30)
		{//滑动
			m_click = false;
			unschedule("longAnEditMode");
			unschedule("UNEditModeTime");
		}
	};
	touchmove->onTouchEnded = [=](Touch* t, Event* e) {
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchmove, -100);

	auto touchlistener = EventListenerTouchOneByOne::create();
	touchlistener->onTouchBegan = [=](Touch* t, Event* e) {
		//App::log("BookRoom touchlistener");
		App::GetInstance()->addTime("longAnEditModeTime", NetIntface::getMillsTime());
		if (m_compile)
		{
			schedule([=](float f) {
				auto t1 = App::GetInstance()->getTime("longAnEditModeTime", NetIntface::getMillsTime());
				auto nowt = NetIntface::getMillsTime();
				if (nowt > (t1 + 500))
				{
					m_compile = false;
					YYXLayer::sendNotify("bookRoomScenemengban", "", -1);
					unschedule("UNEditModeTime");
				}
			}, 0.5f, "UNEditModeTime");
		}
		return true;
	};
	touchlistener->onTouchMoved = [=](Touch* t, Event* e) {
		//App::log("BookStore touchlistener onTouchMoved");
		auto moveLocation = t->getStartLocation() - t->getLocation();		
		auto key = "bookRoomMoveTime";
		if (moveLocation.x > 160 || moveLocation.y < -80)
		{				//向右
			YYXLayer::controlTouchTime(2, key, [=]() {
				++m_currentPageNumber;
				RollingAnimation(shang3ben, xia3ben, [=]() {
					refershPage(bookMode);
				});
			});
		}
		else if (moveLocation.x < -160 || moveLocation.y > 80)
		{				//向左
			YYXLayer::controlTouchTime(2, key, [=]() {
				--m_currentPageNumber;
				RollingAnimation(shang3ben, xia3ben, [=]() {
					refershPage(bookMode);
				});
			});
		}
	};
	touchlistener->onTouchEnded = [=](Touch* t, Event* e) {
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchlistener, 1);
	//页码
	showBookPageNumber();
	//购书列表
	auto Button_first = (ImageView*)layer->getChildByName("Image_14_2"); 
	//包年图书
	auto Button_second = (ImageView*)layer->getChildByName("Image_14_1"); 
	//下载
	auto Button_three = (ImageView*)layer->getChildByName("Image_14_0");
	//最近阅读
	auto Button_fourth = (ImageView*)layer->getChildByName("Image_14");
	//购书列表 = 3 已收藏 = 2 已下载  = 0 包年图书 = 4	
	auto runFc = [=](int modeNumber) {
		bookMode = modeNumber;
		showImageView(bookMode, Button_first, Button_second, Button_three, Button_fourth);
		m_currentPageNumber = 0;
		refershPage(bookMode);
		showBookPageNumber();
	};

	if (Button_first)
	{
		Button_first->setTouchEnabled(true);
		Button_first->addClickEventListener([=](Ref* sbeder) {
			runFc(0);
		});
	}

	if (Button_second)
	{
		Button_second->setTouchEnabled(true);
		Button_second->addClickEventListener([=](Ref* sbeder) {
			runFc(2);
		});
	}
	
	if (Button_three)
	{
		Button_three->setTouchEnabled(true);
		Button_three->addClickEventListener([=](Ref* sbeder) {
			runFc(4);
		});
	}
	
	if (Button_fourth)
	{
		Button_fourth->setTouchEnabled(true);
		Button_fourth->addClickEventListener([=](Ref* sbeder) {
			runFc(3);
		});
	}

	_eventDispatcher->addCustomEventListener("httpCallBackRefersh", [=](EventCustom* e) {
		refershPage(bookMode);
		showBookPageNumber();
	});

	showImageView(bookMode, Button_first, Button_second, Button_three, Button_fourth);
	//最上层返回
	auto homeButton = (Button*)layer->getChildByName(FIND_BUTTON_BY_NAME_BOOKROOM_BACK);
	homeButton->removeFromParent();
	addChild(homeButton);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		if (!m_click)
			return;
		if (YYXLayer::getBoolFromXML(SOUND_KEY))
			YYXLayer::PLAYBUTTON;
		App::GetInstance()->popBack();
		Index::BackPreviousScene();
	});
	homeButton->setPosition(origin + Vec2(14, visibleSize.height - 14));
	initHttp();
	YYXLayer::loge("BookRoom::init()");
    return true;
}

void BookRoom::initHttp()
{
	if (App::GetInstance()->m_me != nullptr)
	{
		auto memberid = App::GetInstance()->m_me->id;
		if (memberid > 0)
		{
			App::httpGetVipBook();
			App::httpGetBuyBook();
			App::httpGetCollectBook();
		}
	}
}

//刷新当前页书籍
void BookRoom::refershPage(int status)
{	//购书列表 = 3 已收藏 = 2 已下载  = 0 包年图书 = 4	
	vector<int> bookidData;
	auto suo = (ImageView*)layer->getChildByName("Image_4");
	if (App::GetInstance()->m_me && App::GetInstance()->m_me->vip == true)
	{
		suo->setVisible(false);
	}
	else
	{
		suo->setVisible(true);
		if (bookMode == 4)
			suo->loadTexture("other/wodeshufang-niankasuoding_736h.png");
		else
			suo->loadTexture("other/wodeshufang-niankasuoding-mr_736h.png");
	}
	if (status == 3)//购书列表
	{
		bookidData = getCurrentPageBookID4BuyBook();
	}
	else if (status == 2)//收藏
	{
		bookidData = getCurrentPageBookID4Collect();
	}
	else if (status == 0)//下载
	{
		bookidData =  getCurrentPageBookID4BuyBookDownload();
	}
	else if (status == 4)//包年图书
	{
		bookidData = getCurrentPageBookID4VIP();
	}

	//YYXLayer::sendNotify("showName", "", status);
	for (int i = 0; i < 6; i++)
	{
		if (i < bookidData.size())
		{
			refershBookNode(books[i], bookidData[i]);
		}
		else
		{
			refershBookNode(books[i], -999);
		}
	}
}

//刷新book节点信息
void BookRoom::refershBookNode(Node* book, int bookid)
{
	App::log("--------------------------refershBookNode", bookid);
	//scheduleUpdate();
	book->setVisible(true);
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(book);
	if (bookid < 0)
	{
		book->setVisible(false);
		return;
	}	
	//离线模式
	if (m_offline && !FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
	{
		book->setVisible(false);
		return;
	}
	//vip包年标签
	auto vip = (Sprite*)book->getChildByName("Sprite_1");
	//锁
	auto _suo = (ImageView*)book->getChildByName("suo1");
	//提示下载
	auto download = (Sprite*)book->getChildByName("Download");
	//文字背景
	auto textbg = (Sprite*)book->getChildByName("DownloadBackground");
	//文字
	auto text = (Text*)book->getChildByName("Text");
	if (vip)
	{
		vip->setVisible(false);
		string BookPlayUrlKey = StringUtils::format("bookPlayUrl+bookID=%d", bookid);
		auto isvip = (int)YYXStruct::getMapRef(App::GetInstance()->myData, BookPlayUrlKey, nullptr);
		if (BookPlayUrlKey == "bookPlayUrl+bookID=455")
		{
			auto isvip = (int)YYXStruct::getMapRef(App::GetInstance()->myData, BookPlayUrlKey, nullptr);
			//App::log(" usvip=", isvip);
		}
		if (App::GetInstance()->m_me)
		{
			if (App::GetInstance()->m_me->vip)
			{				
					if (App::GetInstance()->VIPbook.count(bookid)>0)
					{
						vip->setVisible(true);
					}				
			}			
		}
	}
	//监听 刷新状态 传bookid
	auto listenerCompileChangeBook = EventListenerCustom::create("bookRoomSceneCompileChange", [=](EventCustom* e) {
		long sendbookid = (long)e->getUserData();
		if (sendbookid == bookid)
			refershBookNode(book, bookid);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerCompileChangeBook, book);
	auto cover = (ImageView*)book->getChildByName("Image");
	cover->setTag(bookid);
	auto coverpath =App::getBookCoverPngPath(bookid);
	if (FileUtils::getInstance()->isFileExist(coverpath) && cover)
	{
		cover->loadTexture(coverpath);
	}
	else
	{
		cover->loadTexture("other/Book_cover.png");
	}
	auto listenerCoverBook = EventListenerCustom::create(StringUtils::format("bookRoomCoverDownloadSuccess"), [=](EventCustom* e) {
		if (FileUtils::getInstance()->isFileExist(coverpath) && cover)
		{
			cover->loadTexture(coverpath);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerCoverBook, book);

	string zippath = App::getBookReadZipPath(bookid);
	string taskTag = YYXDownload::getTag(zippath);
	if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
	{
		if (download)
			download->setVisible(false);
		if (textbg)
			textbg->setVisible(false);
		if (text)
			text->setVisible(false);
	}
	else {
		if (taskTag != "")
		{
			//判断是否在下载队列 准备队列 暂停  下完删除了
			if (YYXDownload::GetInstance()->isExitDownloadQueue(taskTag) || YYXDownload::GetInstance()->isExitReadyQueue(taskTag))
			{
				showStauts(0, download, textbg, text);
				auto listenerProgressing = EventListenerCustom::create(taskTag + "_BookProgressing", [=](EventCustom* e) {
					long p = (long)e->getUserData();
					showStauts(p, download, textbg, text);
					if (p == 100)
					{
						refershBookNode(book, bookid);
					}
				});
				Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerProgressing, book);
			}
			else if (YYXDownload::GetInstance()->isPause(taskTag))
				showStauts(8000, download, textbg, text);
			else
			{
				showStauts(9000, download, textbg, text);
			}
		}
		else
			showStauts(9000, download, textbg, text);
	}
	if (bookIsSuo(bookid))
	{//vip过期包年图书上锁
		if (_suo)
			_suo->setVisible(true);
		if (download)
			download->setVisible(false);
		if (textbg)
			textbg->setVisible(false);
		if (text)
			text->setVisible(false);
	}
	else
	{
		if (_suo)
			_suo->setVisible(false);
	}
	bookClick(book, bookid);
	bookLongClick(book, bookid);
}

//点击事件
void BookRoom::bookClick(Node* book, int bookid)
{
	auto cover = (ImageView*)book->getChildByName("Image");
	//提示下载
	auto download = (Sprite*)book->getChildByName("Download");
	//文字背景
	auto textbg = (Sprite*)book->getChildByName("DownloadBackground");
	//文字
	auto text = (Text*)book->getChildByName("Text");
	cover->addClickEventListener([=](Ref* sender) {
		if (bookIsSuo(bookid))
		{
			Toast::create(App::getString("QINGNINKAITONGNIANKAFUWU"), false);
			return;
		}
		if (!m_click || m_compile)
			return;
		YYXLayer::controlTouchTime(1, "BookRoomSceneClickCoverTime", [=]() {
			if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
			{
				if (!m_click || m_compile)
					return;
				//记录场景
				string bookmode = StringUtils::format("%d", bookMode);
				App::GetInstance()->pushScene(BookRoomScene, m_currentPageNumber, bookmode);
				//打开试读书本
				Director::getInstance()->replaceScene(Waiting::createScene(StringUtils::format("%d", bookid), false));
				//试读不需要上传阅读记录
			}
			else
				down(book, bookid);
		});
	});
}

//长按
void BookRoom::bookLongClick(Node* book, int bookid)
{
	auto cover = (ImageView*)book->getChildByName("Image");
	cover->setTouchEnabled(true);
	cover->setSwallowTouches(false);	
	cover->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		if (bookIsSuo(bookid))
			return;
		auto t1 = App::GetInstance()->getTime("longAnEditModeTime", NetIntface::getMillsTime());
		switch (type)
		{
		case Widget::TouchEventType::BEGAN:
			schedule([=](float f) {
				auto nowt = NetIntface::getMillsTime();
				if (nowt > (t1 + 1000))
				{
					m_compile = true;
					longClickNode = book;
					longClickId = bookid;
					reorder = book->getPosition();
					App::log("longClickNode" , (int)longClickNode);
					App::log("longClickId", bookid);
					App::log("reorder.x = ", reorder.x);
					App::log("reorder.y = ", reorder.y);
					YYXLayer::sendNotify("bookRoomScenemengban", "", 1);
					unschedule("longAnEditMode");
				}
			}, 0.2f, "longAnEditMode");
			break;
		case Widget::TouchEventType::ENDED:
			App::log("longAnEditMode");			
			unschedule("longAnEditMode");
			break;
		}		
	});
}

void BookRoom::down(Node* book, int bookid) {
	//提示下载
	auto download = (Sprite*)book->getChildByName("Download");
	//文字背景
	auto textbg = (Sprite*)book->getChildByName("DownloadBackground");
	//文字
	auto text = (Text*)book->getChildByName("Text");
	//不存在
	//下载的时候, 点击暂停下载
	string zippath = App::getBookReadZipPath(bookid);
	string taskTag = YYXDownload::getTag(zippath);
	if (taskTag != "" && !YYXDownload::GetInstance()->isEnd(taskTag) && !YYXDownload::GetInstance()->isPause(taskTag))
	{
		Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(taskTag + "_BookProgressing");
		YYXDownload::GetInstance()->pause(taskTag);
		showStauts(8000, download, textbg, text);
	}
	else
	{//不存在 
	 //下载的时候, 点击暂停下载
		string zippath = App::getBookReadZipPath(bookid);
		string taskTag = YYXDownload::getTag(zippath);
		if (taskTag != "" && !YYXDownload::GetInstance()->isEnd(taskTag) && !YYXDownload::GetInstance()->isPause(taskTag))
		{
			Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(taskTag + "_BookProgressing");
			YYXDownload::GetInstance()->pause(taskTag);
			showStauts(8000, download, textbg, text);
		}
		else
		{			//未下载的情况, 点击下载
			auto pairData = App::GetInstance()->myBookURLMap.find(bookid);
			if (pairData != App::GetInstance()->myBookURLMap.end())
			{
				string url = pairData->second;
				//App::log("             App::GetInstance()->myBuyBookMap.find(bookid);"+ url, bookid);
				if (url == "")
				{
					Toast::create(App::getString("WEIZHAODAOXIAZAIDIZHI"), false);
				}
				else if (url != ""  && App::getNetSetAndHintSet())
				{
					string beginKey = "bookRoomSceneDownLoadBookBegin";
					string progressKey = "bookRoomSceneDownLoadBookProgress";
					string endKey = "bookRoomSceneDownLoadBookEnd";
					string bookTag = YYXDownload::GetInstance()->download(url, App::getReadZipDir(), StringUtils::format("%d.zip", bookid), beginKey, [=](YYXStruct data)
					{}, progressKey, [=](YYXStruct data)
					{
						auto taskTag = YYXStruct::getStringData(data, "");
						if (YYXDownload::GetInstance()->isPause(taskTag))
							return;
						if (data.intData <= 0)
							return;
						if (data.intData <= 99)
						{
								YYXLayer::sendNotify(taskTag + "_BookProgressing", "", data.intData);
						}
						else
							YYXLayer::sendNotify(taskTag + "_BookProgressing", "", 99);
					}, endKey, [=](YYXStruct data) {
						auto taskTag2 = YYXStruct::getStringData(data, "");
						int code = YYXStruct::getInt64Data(data, -2);
						string bookName = YYXStruct::getMapString(App::GetInstance()->myData, taskTag2, "");
						int bookid = YYXStruct::getMapInt64(App::GetInstance()->myData, taskTag2, -999);
						if (code == 0)
						{
							NetIntface::AddDownLoadRecord(App::GetInstance()->m_me->id, bookid, "", [](string str) {}, "", [](string str) {});
							//下载成功,解压
							std::thread(&YYXDownload::uncompress, App::getReadZipDir(), StringUtils::format("%d", bookid), App::getReadDir(), [=](string zipPath) {
								YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 100);
								App::addDownloadBookRecord(bookid);
							}, [=](string zipPath) {
								//解压失败
								if (!YYXDownload::GetInstance()->isPause(taskTag2))
								{
									YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 999);
									string bookNameError = bookName + App::getString("JIEYACUOWUCHANGXINXIAZAI") + StringUtils::format("%d", bookid);
									Toast::create(bookNameError.c_str());
									App::downloadFailDeleteFiles(App::getBookReadZipPath(bookid), App::getBookReadPath(bookid));
								}
								else
								{
									YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 8000);
								}
							}).detach();
						}
						else
						{
							//下载错误
							if (!YYXDownload::GetInstance()->isPause(taskTag2))
							{
								YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 999);
								string bookNameError = bookName + App::getString("XIAZAICUOWUCHANGXINXIAZAI");
								Toast::create(bookNameError.c_str());
								App::downloadFailDeleteFiles(App::getBookReadZipPath(bookid), App::getBookReadPath(bookid));
							}
						}
					});					
					//任务字符串绑定书名
					string BookNameKey = StringUtils::format("bookName+bookID=%d", bookid);
					//书籍信息 原价+书籍名称+ 书页数
					string taskBookName = YYXStruct::getMapString(App::GetInstance()->myData, BookNameKey, "");
					YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookTag, bookid, "<<" + taskBookName + ">>");
					//加监听
					showStauts(0, download, textbg, text);
					auto listenerProgressing = EventListenerCustom::create(bookTag + "_BookProgressing", [=](EventCustom* e) {
						long p = (long)e->getUserData();
						showStauts(p, download, textbg, text);
						if (p == 100)
						{
							refershBookNode(book, bookid);
						}
					});
					Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerProgressing, book);
				}
			}
			else
			{
				if (NetIntface::IsNetConnect())
				{
					//记录场景
					string bookmode = StringUtils::format("%d", bookMode);
					App::GetInstance()->pushScene(BookRoomScene, m_currentPageNumber, bookmode);
					Index::GoToBookInfo(bookid);
				}
				else
					Toast::create(App::getString("WEIZHAODAOXIAZAIDIZHI"));
			}
		}
	}
}

//下载状态展示
void BookRoom::showStauts(int stauts, Sprite* download, Sprite* textbg, Text* text)
{
	if (textbg)
		textbg->setVisible(true);
	if (text)
		text->setVisible(true);
	if (stauts < 0)//准备下载
	{
		if (download)
			download->setVisible(false);
		if (text)
			text->setText("0%");
	}
	else if (stauts <= 99)//下载中
	{
		if (download)
			download->setVisible(false);
		text->setText(StringUtils::format("%d%%", stauts));
	}
	else if (stauts == 100)//下载完成
	{
		if (download)
			download->setVisible(false);
		if (textbg)
			textbg->setVisible(false);
		if (text)
			text->setVisible(false);
	}
	else if (stauts == 9000 || stauts == 999)//未下载
	{
		if (download)
			download->setVisible(true);
		if (textbg)
			textbg->setVisible(false);
		if (text)
			text->setVisible(false);
	}
	else if (stauts == 8000)//暂停
	{
		if (download)
			download->setVisible(false);
		if (text)
			text->setText(App::getString("ZANTING"));
	}
}

//全部图书中已经下载在本地的部分
//计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
vector<int> BookRoom::getCurrentPageBookID4BuyBookDownload()
{
	vector<int> v_boosSort;
	map<int, int >data;
	for (auto it : App::GetInstance()->myBuyBook)
	{
		int bookid = it.first;
		int time = it.second;
		if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
		{
			auto ite = App::GetInstance()->bookDownload.find(bookid);
			if (ite != App::GetInstance()->bookDownload.end())
				time = ite->second;	
			data[bookid] = time;
		}
	}
	auto sortDat = App::sortMapToVector(data);
	bookSum = sortDat.size();
	int pagesum = ceil(bookSum / 6.0);
	if (m_currentPageNumber < 0)
	{
		m_currentPageNumber = pagesum - 1;
	}
	else if (m_currentPageNumber > pagesum - 1)
	{
		m_currentPageNumber = 0;
	}
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = m_currentPageNumber * 6;
	end = m_currentPageNumber * 6 + 5;
	if (end >= bookSum)
		end = bookSum - 1;
	for (int i = begin; i <= end; i++)
	{
		auto bookid = sortDat[i].first;
		v_boosSort.push_back(bookid);
	}
	return v_boosSort;
}

//计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果 (购书列表)
vector<int> BookRoom::getCurrentPageBookID4BuyBook()
{	
	vector<int> v_boosSort;
	auto sortDat = App::sortMapToVector(App::GetInstance()->myBuyBook);
	bookSum = sortDat.size();
	int pagesum = ceil(bookSum / 6.0);
	if (m_currentPageNumber < 0)
	{
		m_currentPageNumber = pagesum - 1;
	}
	else if (m_currentPageNumber > pagesum - 1)
	{
		m_currentPageNumber = 0;
	}
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = m_currentPageNumber * 6;
	end = m_currentPageNumber * 6 + 5;
	if (end >= bookSum)
		end = bookSum - 1;
	for (int i = begin; i <= end; i++)
	{
		auto bookid = sortDat[i].first;
		v_boosSort.push_back(bookid);
	}
	return v_boosSort;
}

//滚动动画
void BookRoom::RollingAnimation(Node* node1, Node* node2,  function<void()> callback)
{
	auto ac = Sequence::create(MoveBy::create(0.5, Vec2(0, -170)), CallFuncN::create([=](Ref* sender) {
		if (callback)
			callback();
		showBookPageNumber();
	}), MoveBy::create(0.5, Vec2(0, 170)), NULL);
	node1->runAction(ac->clone());
	node2->runAction(ac);
}

//展示第1/5页
void BookRoom::showBookPageNumber()
{//购书列表 = 3 已收藏 = 2 已下载  = 0 包年图书 = 4	
	App::log("BookRoom::showBookPageNumber", bookMode);
	if (bookMode == 3)
	{
		bookSum = App::GetInstance()->myBuyBook.size();
	}
	else if (bookMode == 2)
	{
		bookSum = App::GetInstance()->bookCollect.size();
	}
	else if (bookMode == 0)
	{
		
	}
	else if (bookMode == 4)
	{
		bookSum = App::GetInstance()->VIPbook.size();
	}
	int pagesum = ceil(bookSum / 6.0);
	//创建页数圆圈
	if (pageCircle->getChildrenCount() > 0)
		pageCircle->removeAllChildrenWithCleanup(true);
	if (pagesum <= 0)
	{
		moveCircle->setPosition(Vec2(-1000, -1000));
		return;
	}
	//notify(REMOVEWAITLayer);
	auto size = Director::getInstance()->getVisibleSize();
	int startPosX = size.width / 2 - pagesum * 30 / 2;
	for (int i = 0; i < pagesum; i++) {
		auto pLabel = Label::create(App::getString("CIRCLE"), "wryh.TTF", 65);
		pLabel->setPosition(Vec2(startPosX + i * 30, 20));
		pLabel->setScale(0.5f);
		pLabel->setColor(ccc3(120, 120, 120));
		pageCircle->addChild(pLabel, 0, i);
	}
	auto p = pageCircle->getChildByTag(m_currentPageNumber);
	if(p)
		moveCircle->setPosition(p->getPosition());
	App::log("BookRoom::showBookPageNumber--- END", bookSum);
}

void BookRoom::onEnterTransitionDidFinish()
{
	App::log("BookRoom::onEnterTransitionDidFinish");
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	App::log("BookRoom::onEnterTransitionDidFinish---END");
}

//展示最近下载
//void BookRoom::showDownloadRecord()
//{
//	m_currentPageNumber = 0;
//	refershPage(2);
//}
//
////展示最近阅读
//void BookRoom::showReadRecord()
//{
//	m_currentPageNumber = 0;
//	refershPage(3);
//}



//收藏界面, 计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
vector<int> BookRoom::getCurrentPageBookID4Collect()
{
	vector<int> v_boosSort;
	auto booksum = App::GetInstance()->bookCollect.size();
	int pagesum = ceil(booksum / 6.0);
	if (m_currentPageNumber < 0)
	{
		m_currentPageNumber = pagesum - 1;
	}
	else if (m_currentPageNumber > pagesum - 1)
	{
		m_currentPageNumber = 0;
	}
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = m_currentPageNumber * 6;
	end = m_currentPageNumber * 6 + 5;
	if (end >= booksum)
		end = booksum - 1;
	auto vcdata = App::sortMapToVector(App::GetInstance()->bookCollect);
	for (int i = begin; i <= end; i++)
	{
		auto bookid = vcdata[i];
		v_boosSort.push_back(bookid.first);
	}
	return v_boosSort;
}
//最近租书界面, 计算得到当前页的所有书籍bookid, 如果越界, 进行滚动效果
vector<int> BookRoom::getCurrentPageBookID4VIP()
{
	vector<int> v_boosSort;
	auto booksum = App::GetInstance()->VIPbook.size();
	int pagesum = ceil(booksum / 6.0);
	if (m_currentPageNumber < 0)
	{
		m_currentPageNumber = pagesum - 1;
		if (m_currentPageNumber < 0)
			m_currentPageNumber = 0;
	}
	else if (m_currentPageNumber > pagesum - 1)
	{
		m_currentPageNumber = 0;
	}
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = m_currentPageNumber * 6;
	end = m_currentPageNumber * 6 + 5;
	if (end >= booksum)
		end = booksum - 1;
	auto vcdata = App::sortMapToVector(App::GetInstance()->VIPbook);
	for (int i = begin; i <= end; i++)
	{
		auto bookid = vcdata[i];
		v_boosSort.push_back(bookid.first);
		App::log("   -------------------------------------------       rent bookid= ", bookid.first);
	}
	return v_boosSort;
}

void BookRoom::showImageView(int status, ImageView* view1, ImageView* view2, ImageView* view3, ImageView* view4 )
{
	//已下载  已收藏   年卡图书    购书记录
	//购书列表 = 3 已收藏 = 2 已下载  = 0 包年图书 = 4	
	switch (status)
	{
	case 0:
		view1->loadTexture("BookRoom/res/ysc_nav_sel_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/sc_nav_nor_736h.png", TextureResType::PLIST);
		view3->loadTexture("BookRoom/res/nkts_nav_nor_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/gm_nav_nor_736h.png", TextureResType::PLIST);
		break;
	case 2:
		view1->loadTexture("BookRoom/res/ysc_nav_nor_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/sc_nav_sel_736h.png", TextureResType::PLIST);
		view3->loadTexture("BookRoom/res/nkts_nav_nor_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/gm_nav_nor_736h.png", TextureResType::PLIST);
		break;
	case 3:
		view1->loadTexture("BookRoom/res/ysc_nav_nor_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/sc_nav_nor_736h.png", TextureResType::PLIST);
		view3->loadTexture("BookRoom/res/nkts_nav_nor_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/gm_nav_sel_736h.png", TextureResType::PLIST);
		break;
	case 4:
		view1->loadTexture("BookRoom/res/ysc_nav_nor_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/sc_nav_nor_736h.png", TextureResType::PLIST);
		view3->loadTexture("BookRoom/res/nkts_nav_sel_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/gm_nav_nor_736h.png", TextureResType::PLIST);
		break;
	}
}

//长按的效果是 一个背景图片盖住 上面一个书籍节点移动不同的位置 覆盖下面的书籍
void BookRoom::changLongClickBookPosition(ImageView* backgro ,Node* booknode, int  position)
{
	if (position != -1)
	{
		backgro->setVisible(true);
		backgro->setTouchEnabled(true);
		backgro->addClickEventListener([=](Ref* sender) {
			changLongClickBookPosition(backgro, booknode, -1);
		});
		if (booknode) {
			////编辑模式界面
			//提示下载
			auto download = (Sprite*)booknode->getChildByName("Download");
			//文字背景
			auto textbg = (Sprite*)booknode->getChildByName("DownloadBackground");
			//文字
			auto text = (Text*)booknode->getChildByName("Text");
			booknode->setVisible(true);
			booknode->setPosition(reorder);
			refershBookNode(booknode, longClickId);
			auto close = (ImageView*)booknode->getChildByName("close");
			close->setSwallowTouches(true);
			close->setTouchEnabled(true);
			auto deleteBtn = (ImageView*)close->getChildByName("Image_19");
			deleteBtn->setSwallowTouches(true);
			deleteBtn->setTouchEnabled(true);
			if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(longClickId)))
			{
				deleteBtn->loadTexture("BookRoom/res/BookHome_button_shanchu_736h.png", TextureResType::PLIST);
				deleteBtn->addClickEventListener([=](Ref* sender) {
					YYXLayer::controlTouchTime(1, "bookRoomSceneDeleteBookTime.png", [=]() {
						auto message = YYXLayer::MyMessageBox(App::getString("QUEDINGSHANCHU"), "", [=]() {
							YYXLayer::DeleteDirectory(App::getBookReadPath(longClickId));
							changLongClickBookPosition(backgro, booknode, -1);
							YYXLayer::sendNotify("bookRoomSceneCompileChange","", longClickId);
						}, "", []() {});
						if (message)
							addChild(message, 1);
					});
				});
			}
			else
			{
				deleteBtn->loadTexture("BookRoom/res/BookHome_button_xiazai_736h.png", TextureResType::PLIST);
				//未下载的情况, 点击下载
				deleteBtn->addClickEventListener([=](Ref* sender) {
					changLongClickBookPosition(backgro, booknode, -1);
					down(longClickNode, longClickId);
				});
			}
			auto collectBtn = (ImageView*)close->getChildByName("Image_18");
			collectBtn->setSwallowTouches(true);
			collectBtn->setTouchEnabled(true);
			map<int, int >::iterator it = App::GetInstance()->bookCollect.find(longClickId);
			if (it == App::GetInstance()->bookCollect.end()) {
				collectBtn->loadTexture("BookRoom/res/BookHome_button_shoucang_736h.png", TextureResType::PLIST);
				collectBtn->addClickEventListener([=](Ref* sender) {
					collectBtn->loadTexture("BookRoom/res/BookHome_button_yishoucang_736h.png", TextureResType::PLIST);
					NetIntface::httpBookCollect(longClickId, 1, [=](string json) {
						rapidjson::Document doc;
						auto result = YYXLayer::getJsonObject4Json(doc, json);
						if (result)
							result = YYXLayer::getBool4Json(false, doc, "result");
						if (result)
						{
							App::addRecordBookCollect(longClickId);
							Toast::create(App::getString("COLLECTSUCCESS"));							
							collectBtn->loadTexture("BookRoom/res/BookHome_button_yishoucang_736h.png", TextureResType::PLIST);
						}
						else
						{
							collectBtn->loadTexture("BookRoom/res/BookHome_button_shoucang_736h.png", TextureResType::PLIST);
							Toast::create(App::getString("COLLECTFAIL"));
						}
						YYXLayer::sendNotify("bookRoomScenemengban", "", -1);
					}, [=](string json) {
						collectBtn->loadTexture("BookRoom/res/BookHome_button_shoucang_736h.png", TextureResType::PLIST);
						Toast::create(App::getString("COLLECTFAIL"));
						YYXLayer::sendNotify("bookRoomScenemengban", "", -1);
					});					
				});
			}
			else {
				collectBtn->loadTexture("BookRoom/res/BookHome_button_yishoucang_736h.png", TextureResType::PLIST);
				collectBtn->addClickEventListener([=](Ref* sender) {
					collectBtn->loadTexture("BookRoom/res/BookHome_button_shoucang_736h.png", TextureResType::PLIST);
					NetIntface::httpBookCollect( longClickId, 2, [=](string json) {
						rapidjson::Document doc;
						auto result = YYXLayer::getJsonObject4Json(doc, json);
						if (result)
							result = YYXLayer::getBool4Json(false, doc, "result");
						if (result)
						{
							App::deleteRecordBookCollect(longClickId);
							Toast::create(App::getString("DECOLLECTSUCCESS"));
							collectBtn->loadTexture("BookRoom/res/BookHome_button_shoucang_736h.png", TextureResType::PLIST);
						}
						else 
						{
							collectBtn->loadTexture("BookRoom/res/BookHome_button_yishoucang_736h.png", TextureResType::PLIST);
							Toast::create(App::getString("DECOLLECTFAIL"));
						}
						YYXLayer::sendNotify("bookRoomScenemengban", "", -1);
					}, [=](string json) {
						collectBtn->loadTexture("BookRoom/res/BookHome_button_yishoucang_736h.png", TextureResType::PLIST);
						Toast::create(App::getString("DECOLLECTFAIL"));
						YYXLayer::sendNotify("bookRoomScenemengban", "", -1);
					});					
				});
			}
		}
	}
	else {
		m_compile = false;
		backgro->setVisible(false);
		if (booknode) {
			booknode->setPosition(Vec2(-550, 550));
			booknode->setVisible(false);
		}
	}
}

bool BookRoom::bookIsMyVipBooks(int bookid)
{
	if (App::GetInstance()->VIPbook.count(bookid) > 0)
		return true;
	else
		return false;
}

bool BookRoom::userIsVip()
{
	if (App::GetInstance()->m_me && App::GetInstance()->m_me->vip)
		return true;
	else
		return false;
}

bool BookRoom::bookIsMyBuyBooks(int bookid)
{
	if (App::GetInstance()->myBuyBook.count(bookid) > 0)
		return true;
	else
		return false;
}

//是否要上锁
bool BookRoom::bookIsSuo(int bookid)
{
	auto uservip = userIsVip();
	auto buy = bookIsMyBuyBooks(bookid);
	auto rent = bookIsMyVipBooks(bookid);
	if (!uservip)
	{
		if (rent)
		{
			if (!buy)
				return true;
		}
	}
	return false;
}