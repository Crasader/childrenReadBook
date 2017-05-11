#include "BookRoomScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "YYXVisitor.h"
#include "DownloadBook.h"
#include "ReadBook.h"
#include "MyBook.h"
#include "SetBook.h"
#include "YYXDownloadImages.h"
#include <assert.h>
#include "User.h"
#include "BookCache.h"
#include "AppHttp.h"

USING_NS_CC;
using namespace cocostudio::timeline;
using namespace std;

#define zz(d) App::log(string("BookRoom::")+#d);
#define zzz(d) App::log(string("BookRoom::")+#d+"----END");

BookRoom::BookRoom() {
	YYXLayer::logb("BookRoom::BookRoom()");
	YYXLayer::loge("BookRoom::BookRoom()");
}

BookRoom::~BookRoom() {
	YYXLayer::logb("BookRoom::~BookRoom()");
	ControlScene::getInstance()->end();
	BookCache::getInstance()->clear();
	DownloadBook::getInstance()->clearBook();
	MyBook::getInstance()->clearBook();
	YYXLayer::loge("BookRoom::~BookRoom()");
}

Scene* BookRoom::createScene(SceneInfo* sceneInfo)
{
	//zz(createScene)
    auto scene = Scene::create();
    auto layer = BookRoom::create(sceneInfo);
    scene->addChild(layer);
	//zzz(createScene)
    return scene;
}

BookRoom* BookRoom::create(SceneInfo* data)
{
	BookRoom *pRet = new(std::nothrow) BookRoom();
	if (pRet && pRet->init(data))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool BookRoom::init(SceneInfo* sceneInfo)
{
	//zz(init)
    if ( !Layer::init() )
    {
		//zzz(init)
        return false;
    }
	YYXVisitor::getInstance()->bookRoomSceneInit();
	ReadBook::getInstance()->setIsSorted(true);
	DownloadBook::getInstance()->setIsSorted(true);
	MyBook::getInstance()->setIsSorted(true);
	m_offline = !NetIntface::IsNetConnect();
	if (App::m_debug == 0)
	{
		m_offline = true;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//App::m_RunningScene = MySceneName::BookRoomScene;
	Data data;
	if (App::GetInstance()->getData(BOOKROOM_CSB, data))
		layer = (Layer*)CSLoader::createNode(data);
	else
		layer = (Layer*)CSLoader::createNode(BOOKROOM_CSB);
	layer->setAnchorPoint(Vec2(0.5f, 0.5f));
	layer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(layer);
	//setCurrentPageNumber(0);
	//setCurrentPageNumber(App::GetInstance()->m_showSceneData.intData);
	//string bookmode = App::GetInstance()->m_showSceneData.stringData;
	//bookMode = atoi(bookmode.c_str());
	//--------------初始化场景信息----------------
	bookMode = sceneInfo->getData("bookMode", Value(0)).asInt();
	setCurrentPageNumber(sceneInfo->getData("currentPageNumber", Value(0)).asInt());

	if (bookMode == 0 || bookMode == 3 || bookMode == 5)
	{
		App::log("                             bookmode= " , bookMode);
	}
	else
	{
		bookMode = 0;
	}
	if (getCurrentPageNumber() < 0 || getCurrentPageNumber() >1000)
		setCurrentPageNumber(0);
	//1.展示白天或者黑夜
	if (!App::isNight()) {
		//auto sprNight = (Sprite*)layer->getChildByName(FIND_SPRITE_BY_NAME_NIGHT_PIC);
		//sprNight->setVisible(false);
	}
	auto name = Label::create();
	name->setPosition(Vec2(550,550));
	name->setTTFConfig(App::m_ttf);
	addChild(name, 1);
	

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
		book0->setVisible(false);
		book1->setVisible(false);
		book2->setVisible(false);
		books.push_back(book0);
		books.push_back(book1);
		books.push_back(book2);
	}
	if (xia3ben)
	{
		Node* book3 = xia3ben->getChildByName("book3");
		Node* book4 = xia3ben->getChildByName("book4");
		Node* book5 = xia3ben->getChildByName("book5");
		book3->setVisible(false);
		book4->setVisible(false);
		book5->setVisible(false);
		books.push_back(book3);
		books.push_back(book4);
		books.push_back(book5);
	}
	
	//滑动控制
	touchmove = EventListenerTouchOneByOne::create();
	touchmove->onTouchBegan = [=](Touch* t, Event* e) {
		//zz(EventListenerTouchOneByOne  onTouchBegan  touchmove )
		m_click = true;
		//zzz(EventListenerTouchOneByOne  onTouchBegan  touchmove)
		return true;
	};
	touchmove->onTouchMoved = [=](Touch* t, Event* e) {
		//zz(EventListenerTouchOneByOne  onTouchMoved  touchmove)
		auto moveLocation = t->getStartLocation() - t->getLocation();
		if (moveLocation.x > 30 || moveLocation.x < -30 || moveLocation.y > 30 || moveLocation.y < -30)
		{//滑动
			m_click = false;
			unschedule("longAnEditMode");
			unschedule("UNEditModeTime");
		}
		//zzz(EventListenerTouchOneByOne  onTouchMoved  touchmove)
	};
	touchmove->onTouchEnded = [=](Touch* t, Event* e) {
		//zz(EventListenerTouchOneByOne  onTouchEnded  touchmove)
			//zzz(EventListenerTouchOneByOne  onTouchEnded  touchmove)
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchmove, -100);

	touchlistener = EventListenerTouchOneByOne::create();
	touchlistener->onTouchBegan = [=](Touch* t, Event* e) {
		//zz(EventListenerTouchOneByOne -- onTouchBegan)
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
		//zzz(EventListenerTouchOneByOne-- onTouchBegan)
		return true;
	};
	touchlistener->onTouchMoved = [=](Touch* t, Event* e) {
		//zz(EventListenerTouchOneByOne --onTouchMoved)
		auto moveLocation = t->getStartLocation() - t->getLocation();		
		auto key = "bookRoomMoveTime";
		if (moveLocation.x > 160 || moveLocation.y < -80)
		{				//向右
			YYXLayer::controlTouchTime(2, key, [=]() {
				setCurrentPageNumber(getCurrentPageNumber() + 1);
				RollingAnimation(shang3ben, xia3ben, [=]() {
					refershPage(bookMode);
				});
			});
		}
		else if (moveLocation.x < -160 || moveLocation.y > 80)
		{				//向左
			YYXLayer::controlTouchTime(2, key, [=]() {
				setCurrentPageNumber(getCurrentPageNumber() - 1);
				RollingAnimation(shang3ben, xia3ben, [=]() {
					refershPage(bookMode);
				});
			});
		}
		//zzz(EventListenerTouchOneByOne --onTouchMoved)
	};
	touchlistener->onTouchEnded = [=](Touch* t, Event* e) {
		//zz(EventListenerTouchOneByOne --onTouchEnded)
		//zzz(EventListenerTouchOneByOne --onTouchEnded)
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchlistener, 1);
	//帮助
	auto helper = (Button*)layer->getChildByName("Button_1");
	if (helper)
	{
		helper->addClickEventListener([=](Ref* sender) {
			auto orgpor = Director::getInstance()->getVisibleOrigin();
			auto sizew = Director::getInstance()->getVisibleSize();
			map<string, int64String> parameter;
			YYXLayer::insertMap4ParaType(parameter, "className", -999, "helper");
			YYXLayer::insertMap4ParaType(parameter, "csb", -999, "BookRoom/csb/help.csb");
			auto helplayer = YYXLayer::create(parameter);
			auto closebox = (Button*)helplayer->findControl("Button_1");
			auto bgbox = (ImageView*)helplayer->findControl("Image_1");
			closebox->addClickEventListener([=](Ref* sender) {
				helplayer->removeFromParent();
			});
			bgbox->setTouchEnabled(true);
			bgbox->setSwallowTouches(true);
			bgbox->addClickEventListener([=](Ref* sender) {
			});
			helplayer->getParentNode()->setAnchorPoint(Vec2(0.5, 0.5));
			helplayer->setPosition(orgpor + sizew / 2);
			addChild(helplayer);
		});
	}
	//已下载
	auto Button_first = (ImageView*)layer->getChildByName("Image_14_2"); 
	//最近阅读
	auto Button_second = (ImageView*)layer->getChildByName("Image_14_1"); 
	//已购买
	auto Button_fourth = (ImageView*)layer->getChildByName("Image_14");
	//展示模式   已下载  = 0 已收藏 = 2 已购买 = 3 包年图书 = 4	 最近阅读 = 5
	auto runFc = [=](int modeNumber) {
		//YYXLayer::controlTouchTime(1, "bookroomClickTime", [=]() {
			//zz(select button);
			if (bookMode == modeNumber)
				return;
			bookMode = modeNumber;
			showImageView(bookMode, Button_first, Button_second, nullptr, Button_fourth);
			setCurrentPageNumber(0);
			refershPage(bookMode);
			showBookPageNumber();
			//zzz(select button)
		//});
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
			runFc(5);
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
		//zz(doing EventListener <httpCallBackRefersh> )
		if (books.size() > 0)
		{
			refershPage(bookMode);
			showBookPageNumber();
		}
		//zzz(doing EventListener <httpCallBackRefersh>)
	});

	showImageView(bookMode, Button_first, Button_second, nullptr, Button_fourth);
	//最上层返回
	auto homeButton = (Button*)layer->getChildByName(FIND_BUTTON_BY_NAME_BOOKROOM_BACK);
	homeButton->removeFromParent();
	addChild(homeButton);
	homeButton->setTouchEnabled(true);
	homeButton->addClickEventListener([=](Ref* sender) {
		//zz(homeButton--- ClickEventListener)
		if (!m_click)
		{
			//zzz(homeButton-- - ClickEventListener---m_click = false)
			return;
		}
		ControlScene::getInstance()->backFromScene();
		//zzz(homeButton-- - ClickEventListener)
	});
	homeButton->setPosition(origin + Vec2(14, visibleSize.height - 14));
	initEvent();
		//zzz(init)
	return true;
}

void BookRoom::initEvent()
{
	//书籍删除 或者下载完成 自动刷新添加到列表中
	auto listener1 = EventListenerCustom::create(TAG_BOOKROOMBOOKISEXIT, [=](EventCustom* e) {
		ReadBook::getInstance()->setIsSorted(true);
		DownloadBook::getInstance()->setIsSorted(true);
		MyBook::getInstance()->setIsSorted(true);
		refershPage(bookMode);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener1, layer);
	//蒙板 长按书籍常亮效果
	auto backgro = (ImageView*)layer->getChildByName("Image_3");
	auto booknode = layer->getChildByName("book0_0");
	booknode->setVisible(false);
	backgro->setVisible(false);
	auto listeners = EventListenerCustom::create("bookRoomScenemengban", [=](EventCustom* e) {
		//zz(bookRoomScenemengban)
		long posi = (long)e->getUserData();
		changLongClickBookPosition(backgro, booknode, posi);
		//zzz(bookRoomScenemengban)
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listeners, layer);
}

void BookRoom::initHttp()
{
	//zz(initHttp)
	AppHttp::getInstance()->httpVipAndBuyBooks();
	//zzz(initHttp)
}

//刷新当前页书籍
void BookRoom::refershPage(int status)
{	//展示模式   已下载  = 0 已收藏 = 2 //购书列表 = 3 包年图书 = 4	 最近阅读 = 5
	//zz(refershPage)
	vector<int> bookidData;
	BookRoomSelectPage *bookRoomSelectPage=nullptr;
	if (status == 3)//购书列表
	{
		bookRoomSelectPage = MyBook::getInstance();
	}
	else if (status == 0)//下载
	{
		bookRoomSelectPage = DownloadBook::getInstance();
	}
	else if (status == 5)//最近阅读
	{
		bookRoomSelectPage = ReadBook::getInstance();		
	}

	if (bookRoomSelectPage)
	{
		bookRoomSelectPage->initData();
		bookRoomSelectPage->filtrate();
		bookRoomSelectPage->sort();
		bookidData = getPageData(bookRoomSelectPage->getResultData());
	}
	else
		return;
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
	//zzz(refershPage)
}

//刷新book节点信息
void BookRoom::refershBookNode(Node* book, int bookid)
{
	//App::log("--------------------------refershBookNode", bookid);
	assert(book != nullptr);
	assert(book->getReferenceCount()>0);
	if (!book || book->getReferenceCount()==0)
	{
		App::log("--------------------------refershBookNode   getReferenceCount", bookid);
		return;
	}
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(book);
	if (bookid < 0)
	{
		book->setVisible(false);
		return;
	}
	book->setVisible(true);
	//zz(refershBookNode1);
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
	//已经下载
	auto downloaded = (Sprite*)book->getChildByName("BookHome_yixiazaibiaoshi_736h_1");
	//封面
	auto cover = (ImageView*)book->getChildByName("Image");
	//zz(refershBookNode2);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//*****************************************************<<      显示逻辑     >>***********************************************************
	//-----------------------------------------------------------------------------------------------------------------------------------------

	//标记vip书籍 = 用户是vip 书籍是vip方式获得的 并且不是购买的
	if (vip)
	{
		bool visible_vip = false;
		auto uservip = userIsVip();
		auto buy = bookIsMyBuyBooks(bookid);
		auto rent = bookIsMyVipBooks(bookid);
		if (uservip && rent && !buy)
			visible_vip = true;
		vip->setVisible(visible_vip);
	}
	//zz(refershBookNode3);
	//封面
	cover->setTag(bookid);
	auto coverpath = App::getBookCoverPngPath(bookid);
	if (FileUtils::getInstance()->isFileExist(coverpath) && cover)
	{
		cover->loadTexture(coverpath);
	}
	else
	{
		cover->loadTexture("other/Book_cover.png");
		auto listener = EventListenerCustom::create(TAG_BOOKCOVERDOWNLOAD, [=](EventCustom* e) {
			long _bookid = (long)e->getUserData();
			if (_bookid == cover->getTag())
			{
				auto copath = App::getBookCoverPngPath(_bookid);
				if (FileUtils::getInstance()->isFileExist(copath))
				{
					cover->loadTexture(copath);
				}
			}
		});
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, cover);
		auto mybook = BookCache::getInstance()->getBook(bookid);
		if (mybook)
		{
			string url2 = mybook->getCoverURL();
			YYXDownloadImages::GetInstance()->newDownloadImage(url2, BookCache::getInstance()->getCoverDir(), 
				StringUtils::format("%d.png", bookid), high, 0, [bookid](string str) {
				YYXLayer::sendNotify(TAG_BOOKCOVERDOWNLOAD, "", bookid);
			});
		}
	}
	//zz(refershBookNode4);
	//下载中， 已经下载，未下载
	if (downloaded)
		downloaded->setVisible(false);
	if (download)
		download->setVisible(false);
	if (textbg)
		textbg->setVisible(false);
	if (text)
		text->setVisible(false);
	string taskTag = YYXDownload::GetInstance()->getTaskTag(App::getReadZipDir(), StringUtils::format("%d.zip", bookid));
	auto status = YYXDownload::GetInstance()->getTaskStatus(taskTag);
	//zz(refershBookNode5);
	if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
	{
		if (downloaded)
			downloaded->setVisible(true);
	}
	else
	{
		string BookNameKey = StringUtils::format("bookName+bookID=%d", bookid);
		//书籍信息 原价+书籍名称+ 书页数
		string taskBookName = YYXStruct::getMapString(App::GetInstance()->myData, BookNameKey, "");
		YYXStruct::initMapYYXStruct(App::GetInstance()->myData, taskTag, bookid, "<<" + taskBookName + ">>");
		//加监听
		auto listenerProgressing = EventListenerCustom::create(taskTag + "_BookProgressing", [=](EventCustom* e) {
			long p = (long)e->getUserData();
			showStauts(p, download, textbg, text);
			if (p == 100)
			{
				refershBookNode(book, bookid);
			}
		});
		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerProgressing, book);
		int progressing = 9000;
		//判断是否在下载队列 准备队列 暂停  下完删除了
		if (status == YYXDownloadStatus::_ready || status == YYXDownloadStatus::_download)
		{
			progressing = 0;
		}
		else if (status == YYXDownloadStatus::_pause)
		{
			progressing = 8000;
		}
		showStauts(progressing, download, textbg, text);
	}
	//zz(refershBookNode6);
	//锁
	if (_suo && bookIsSuo(bookid))
	{//vip过期包年图书上锁
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
		_suo->setVisible(false);
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//*****************************************************<<      监听     >>***********************************************************
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//书籍的节点上的监听：作用是刷新book，参数表（bookid）
	auto listenerCompileChangeBook = EventListenerCustom::create("bookRoomSceneCompileChange", [=](EventCustom* e) {
		long sendbookid = (long)e->getUserData();
		if (sendbookid == bookid)
			refershBookNode(book, bookid);
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerCompileChangeBook, book);

	//书籍的节点上的监听：作用是修改封面
	auto listenerCoverBook = EventListenerCustom::create(StringUtils::format("bookRoomCoverDownloadSuccess"), [=](EventCustom* e) {
		if (FileUtils::getInstance()->isFileExist(coverpath) && cover)
		{
			cover->loadTexture(coverpath);
		}
	});
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerCoverBook, book);
	//zz(refershBookNode7);
	//点击事件
	bookClick(book, bookid);
	//长按事件
	bookLongClick(book, bookid);
	App::log("--------------------------refershBookNode---END", bookid);
}

//点击事件
void BookRoom::bookClick(Node* book, int bookid)
{
	//zz(bookClick)
	App::log("bookid = ", bookid);
	auto cover = (ImageView*)book->getChildByName("Image");
	cover->setSwallowTouches(true);
	cover->setTouchEnabled(true);
	//提示下载
	auto download = (Sprite*)book->getChildByName("Download");
	//文字背景
	auto textbg = (Sprite*)book->getChildByName("DownloadBackground");
	//文字
	auto text = (Text*)book->getChildByName("Text");
	cover->addClickEventListener([=](Ref* sender) {
		YYXLayer::controlTouchTime(0.5, "BookRoomSceneClickCoverTime", [=]() {
			if (bookIsSuo(bookid))
			{
				//Toast::create(App::getString("QINGNINKAITONGNIANKAFUWU"), false);
				addChild(YYXLayer::MyMessageBox(App::getString("NINDEVIPGUOQISHIFOUXUGEFI"), App::getString("QUXUFEI"), [=]() {
					YYXVisitor::getInstance()->hintLogin([=](){
						//youke
						ControlScene::getInstance()->replaceScene(ControlScene::getInstance()->getSceneInfo(BookRoomScene)->setData("bookMode", Value(bookMode))->
							setData("currentPageNumber", Value(m_currentPageNumber)), ControlScene::getInstance()->getSceneInfo(LoginScene));
					}, [=]() {
						//qudenglu
						ControlScene::getInstance()->replaceScene(ControlScene::getInstance()->getSceneInfo(BookRoomScene)->setData("bookMode", Value(bookMode))->
							setData("currentPageNumber", Value(m_currentPageNumber)), ControlScene::getInstance()->getSceneInfo(LoginScene));
					}, [=]() {
						addChild(XZLayer::payBuyVip());
					},"  ");
				}, App::getString("QUXIANGXIANG"), nullptr));
				return;
			}
			if (!m_click || m_compile)
				return;
			if (bookIsMyBuyBooks(bookid) || bookIsMyVipBooks(bookid))
			{
				if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))
				{
					//打开书本
					ControlScene::getInstance()->pushCurrentScene(
						ControlScene::getInstance()->getSceneInfo(BookRoomScene)->setData("bookMode"
							, Value(bookMode))->setData("currentPageNumber"
								, Value(m_currentPageNumber)));
					App::log("=======================m_currentPageNumber>>>", m_currentPageNumber);
					SetBook::getInstance()->readBook(bookid, false);
				}
				else
				{
					//zz(down111111)
					down(book, bookid);
				}
			}
			else
			{
				string bookmode = StringUtils::format("%d", bookMode);
				ControlScene::getInstance()->replaceScene(ControlScene::getInstance()->getSceneInfo(BookRoomScene)->setData("bookMode", Value(bookMode))->
					setData("currentPageNumber", Value(m_currentPageNumber)), ControlScene::getInstance()->getSceneInfo(BookInfoScene)->setData("bookId",Value(bookid)));
			}
		});
	});
	//zzz(bookClick)
	App::log("bookid = ", bookid);
}

//长按
void BookRoom::bookLongClick(Node* book, int bookid)
{
	//zz(bookLongClick)
		App::log("bookid = ", bookid);
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
					App::log("longClickNode" , (long)longClickNode);
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
	//zzz(bookLongClick)
		App::log("bookid = ", bookid);
}

void BookRoom::down(Node* book, int bookid) {
	//zz(down)
		App::log("bookid = ", bookid);
	//提示下载
	auto download = (Sprite*)book->getChildByName("Download");
	//文字背景
	auto textbg = (Sprite*)book->getChildByName("DownloadBackground");
	//文字
	auto text = (Text*)book->getChildByName("Text");
	string dir = App::getReadZipDir();
	string filename = StringUtils::format("%d.zip", bookid);
	string taskTag = YYXDownload::GetInstance()->getTaskTag(dir, filename);
	auto taskstatus = YYXDownload::GetInstance()->getTaskStatus(taskTag);
	if (taskstatus == YYXDownloadStatus::_null || taskstatus == YYXDownloadStatus::_pause)
	{
		//未下载的情况, 点击下载
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
						AppHttp::getInstance()->httpUploadDownloadRecord(bookid);
						//下载成功,解压
						std::thread mythread(&YYXDownload::uncompress, App::getReadZipDir(), StringUtils::format("%d", bookid), App::getReadDir(), [=](string zipPath) {
							YYXLayer::sendNotify(taskTag2 + "_BookProgressing", "", 100);
							//App::addDownloadBookRecord(bookid);
							DownloadBook::getInstance()->addBook(bookid, (int)YYXLayer::getCurrentTime4Second());
							YYXLayer::sendNotify(TAG_BOOKROOMBOOKISEXIT);
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
						});
						mythread.detach();
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
				refershBookNode(book, bookid);
				////任务字符串绑定书名
				//string BookNameKey = StringUtils::format("bookName+bookID=%d", bookid);
				////书籍信息 原价+书籍名称+ 书页数
				//string taskBookName = YYXStruct::getMapString(App::GetInstance()->myData, BookNameKey, "");
				//YYXStruct::initMapYYXStruct(App::GetInstance()->myData, bookTag, bookid, "<<" + taskBookName + ">>");
				////加监听
				//showStauts(0, download, textbg, text);
				//auto listenerProgressing = EventListenerCustom::create(bookTag + "_BookProgressing", [=](EventCustom* e) {
				//	long p = (long)e->getUserData();
				//	showStauts(p, download, textbg, text);
				//	if (p == 100)
				//	{
				//		refershBookNode(book, bookid);
				//	}
				//});
				//Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerProgressing, book);
			}
		}
		else
		{
			if (NetIntface::IsNetConnect())
			{
				//记录场景
				ControlScene::getInstance()->replaceScene(ControlScene::getInstance()->getSceneInfo(BookRoomScene)->setData("bookMode", Value(bookMode))->
					setData("currentPageNumber", Value(m_currentPageNumber)), ControlScene::getInstance()->getSceneInfo(BookInfoScene)->setData("bookId", Value(bookid)));
			}
			else
				Toast::create(App::getString("WEIZHAODAOXIAZAIDIZHI"));
		}
	}
	else if (taskstatus == YYXDownloadStatus::_download || taskstatus == YYXDownloadStatus::_ready) {
		Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(taskTag + "_BookProgressing");
		YYXDownload::GetInstance()->pause(taskTag);
		showStauts(8000, download, textbg, text);
	}
	else
	{
		if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(bookid)))//可能性不大
		{
			ControlScene::getInstance()->pushCurrentScene(
				ControlScene::getInstance()->getSceneInfo(BookRoomScene)->setData("bookMode"
					, Value(bookMode))->setData("currentPageNumber"
						, Value(m_currentPageNumber)));
			SetBook::getInstance()->readBook(bookid, false);
		}
		else//下载了又删除 记录存在书却不在了
		{
			YYXDownload::GetInstance()->setTaskStatus(taskTag, YYXDownloadStatus::_null);
			down(book, bookid);
		}
	}
	//zzz(down)
		App::log("bookid = ", bookid);
}



int BookRoom::getCurrentPageNumber()
{
	//zz(getCurrentPageNumber)
	int pagesum = ceil(bookSum / 6.0);
	if (m_currentPageNumber < 0)
	{
		if (pagesum <= 0)
			setCurrentPageNumber(0);
		else
			setCurrentPageNumber(pagesum - 1);
	}
	else
	{
		if (m_currentPageNumber > pagesum - 1)
			setCurrentPageNumber(0);
	}
	//zzz(getCurrentPageNumber)
		App::log("m_currentPageNumber = ", m_currentPageNumber);
	return m_currentPageNumber;
}

void BookRoom::cleanup()
{		
	//zz(cleanup)
	_eventDispatcher->removeEventListener(touchmove);
	_eventDispatcher->removeEventListener(touchlistener);
	layer->removeAllChildrenWithCleanup(true);
	layer->removeFromParentAndCleanup(true);
	//zzz(cleanup)
}

//下载状态展示
void BookRoom::showStauts(int stauts, Sprite* download, Sprite* textbg, Text* text)
{
	////zz(showStauts)
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
		//zzz(showStauts  8000)
		if (download)
			download->setVisible(false);
		if (text)
			text->setText(App::getString("ZANTING"));
	}
	////zzz(showStauts)
}

//滚动动画
void BookRoom::RollingAnimation(Node* node1, Node* node2,  function<void()> callback)
{
	//zz(RollingAnimation)
	auto ac = Sequence::create(MoveBy::create(0.5, Vec2(0, -170)), CallFuncN::create([=](Ref* sender) {
		if (callback)
			callback();
		showBookPageNumber();
	}), MoveBy::create(0.5, Vec2(0, 170)), NULL);
	node1->runAction(ac->clone());
	node2->runAction(ac);
	//zzz(RollingAnimation)
}

//展示第1/5页
void BookRoom::showBookPageNumber()
{//购书列表 = 3 已收藏 = 2 已下载  = 0 包年图书 = 4	
	//zz(showBookPageNumber);
	if (pageCircle == nullptr || moveCircle == nullptr || bookMode<0 || bookSum<0 || bookMode> 10 || bookSum > 10000)
		return;
	App::log("bookMode = ", bookMode);
	App::log("bookSum = ", bookSum);
	int pagesum = ceil(bookSum / 6.0);
	//创建页数圆圈
	if (pageCircle->getChildrenCount() > 0)
		pageCircle->removeAllChildrenWithCleanup(true);
	if (pagesum <= 0)
	{
		moveCircle->setPosition(Vec2(-1000, -1000));
		return;
	}
	auto size = Director::getInstance()->getVisibleSize();
	int startPosX = size.width / 2 - pagesum * 30 / 2;
	for (int i = 0; i < pagesum; i++) {
		auto pLabel = Label::create(App::getString("CIRCLE"), "wryh.TTF", 65);
		pLabel->setPosition(Vec2(startPosX + i * 30, 20));
		pLabel->setScale(0.5f);
		pLabel->setColor(ccc3(120, 120, 120));
		pageCircle->addChild(pLabel, 0, i);
	}
	auto p = pageCircle->getChildByTag(getCurrentPageNumber());
	if(p)
		moveCircle->setPosition(p->getPosition());
	//zzz(showBookPageNumber)
		App::log("bookMode = ", bookMode);
	App::log("bookSum = ", bookSum);
}

void BookRoom::onEnterTransitionDidFinish()
{
	//zz(onEnterTransitionDidFinish)
	Layer::onEnterTransitionDidFinish();
	Toast::GetInstance()->SceneInitToast();
	initHttp();
	//书籍
	refershPage(bookMode); //购书列表 = 3 已收藏 = 2 已下载  = 0 包年图书 = 4	
	//页码
	showBookPageNumber();
	//zzz(onEnterTransitionDidFinish)
}

void BookRoom::showImageView(int status, ImageView* view1, ImageView* view2, ImageView* view3, ImageView* view4 )
{
	//zz(showImageView)
	//已下载  已收藏   年卡图书    购书记录
	//购书列表 = 3 已收藏 = 2 已下载  = 0 包年图书 = 4	
	switch (status)
	{
	case 0:
		view1->loadTexture("BookRoom/res/BookHome_button_yixiazai_pre_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/BookHome_button_zuijinyuedu_nor_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/BookHome_button_yigoumai_nor_736h.png", TextureResType::PLIST);
		break;
	case 5:
		view1->loadTexture("BookRoom/res/BookHome_button_yixiazai_nor_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/BookHome_button_zuijinyuedu_pre_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/BookHome_button_yigoumai_nor_736h.png", TextureResType::PLIST);
		break;
	case 3:
		view1->loadTexture("BookRoom/res/BookHome_button_yixiazai_nor_736h.png", TextureResType::PLIST);
		view2->loadTexture("BookRoom/res/BookHome_button_zuijinyuedu_nor_736h.png", TextureResType::PLIST);
		view4->loadTexture("BookRoom/res/BookHome_button_yigoumai_pre_736h.png", TextureResType::PLIST);
		break;
	}
	//zzz(showImageView)
}

//长按的效果是 一个背景图片盖住 上面一个书籍节点移动不同的位置 覆盖下面的书籍
void BookRoom::changLongClickBookPosition(ImageView* backgro ,Node* booknode, int  position)
{
	//zz(changLongClickBookPosition)
	if (position != -1)
	{
		backgro->setVisible(true);
		backgro->setTouchEnabled(true);
		backgro->addClickEventListener([=](Ref* sender) {
			changLongClickBookPosition(backgro, booknode, -1);
		});
		if (booknode) {
			//编辑模式界面
			booknode->setVisible(true);
			booknode->setPosition(reorder);
			auto node1 = booknode->getChildByName("FileNode_1");
			refershBookNode(node1, longClickId);
			auto del = (Button*)booknode->getChildByName("Button_1");
			auto download = (Button*)booknode->getChildByName("Button_2");
			if (FileUtils::getInstance()->isFileExist(App::getBookRead4Json_txtPath(longClickId)))
			{
				del->setVisible(true);
				download->setVisible(false);
				del->addClickEventListener([=](Ref* sender) {
					YYXLayer::controlTouchTime(1, "bookRoomSceneDeleteBookTime.png", [=]() {
						auto message = YYXLayer::MyMessageBox(App::getString("QUEDINGSHANCHU"), "", [=]() {
							YYXLayer::DeleteDirectory(App::getBookReadPath(longClickId));
							changLongClickBookPosition(backgro, booknode, -1);
							YYXLayer::sendNotify("bookRoomSceneCompileChange","", longClickId);
							YYXLayer::sendNotify(TAG_BOOKROOMBOOKISEXIT);
						}, "", [=]() {
							changLongClickBookPosition(backgro, booknode, -1);
						});
						if (message)
							addChild(message, 1);
					});
				});
			}
			else
			{
				del->setVisible(false);
				download->setVisible(true);
				//未下载的情况, 点击下载
				download->addClickEventListener([=](Ref* sender) {
					changLongClickBookPosition(backgro, booknode, -1);
					string dir = App::getReadZipDir();
					string filename = StringUtils::format("%d.zip", longClickId);
					string taskTag = YYXDownload::GetInstance()->getTaskTag(dir, filename);
					auto taskstatus = YYXDownload::GetInstance()->getTaskStatus(taskTag);
					if (taskstatus == _null || taskstatus == _pause)
					{
						//zz(down22222222)
						down(longClickNode, longClickId);
					}
				});
			}
		}
	}
	else {
		m_compile = false;
		backgro->setVisible(false);
		if (booknode) {
			booknode->setPosition(Vec2(-1550, 550));
			booknode->setVisible(false);
		}
	}
	//zzz(changLongClickBookPosition)
}

bool BookRoom::bookIsMyVipBooks(int bookid)
{
	////zz(bookIsMyVipBooks)
	bool result = false;
	if (App::GetInstance()->VIPbook.find(bookid) != App::GetInstance()->VIPbook.end())
		result= true;
	else
		result= false;
	////zzz(bookIsMyVipBooks)
	return result;
}

bool BookRoom::userIsVip()
{
	if (YYXVisitor::getInstance()->getVisitorMode())
		return false;
	else if (User::getInstance()->getVip())
		return true;
	else
		return false;
}

bool BookRoom::bookIsMyBuyBooks(int bookid)
{
	////zz(bookIsMyBuyBooks)
	bool result = false;
	if (App::GetInstance()->myBuyBook.find(bookid) != App::GetInstance()->myBuyBook.end())
		result = true;
	else
		result = false;
	////zzz(bookIsMyBuyBooks)
	return result;
}

//是否要上锁
bool BookRoom::bookIsSuo(int bookid)
{
	////zz(bookIsSuo)
	bool result = false;
	auto uservip = userIsVip();
	auto buy = bookIsMyBuyBooks(bookid);
	auto rent = bookIsMyVipBooks(bookid);
	if (!uservip)
	{
		if (rent)
		{
			if (!buy)
				result = true;
		}
	}
	////zzz(bookIsSuo)
	return result;
}

vector<int> BookRoom::getPageData(vector<int> vecdata)
{
	//zz(getPageData)
	bookSum = vecdata.size();
	vector<int> data;	
	int begin;//第一本书的sort
	int end;//最后一本书的sort
	begin = getCurrentPageNumber() * 6;
	end = getCurrentPageNumber() * 6 + 6;
	if (end >= bookSum)
		end = bookSum;
	for (int i = begin; i < end; i++)
	{
		if (i < 0 || i >= vecdata.size())
			continue;
		auto bookItem = vecdata[i];
		data.push_back(bookItem);
	}
	//zzz(getPageData)
	return data;
}

