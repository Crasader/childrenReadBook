#include "DropBox.h"

DropBox::DropBox() {
	lastIndex1 = 0;
	lastIndex2 = 0;
	lastIndex3 = 0;
	myListView3 = NULL;
	isEnable = true;
	selectedStr1 = "";
	selectedStr2 = "";
	selectedStr3 = "";
	strProvince = "";
	strCity = "";
}

DropBox::~DropBox() {

}

DropBox* DropBox::create(Vec2 pos, Size size, string font, TextHAlignment hAligment) {
	DropBox* pRect = new DropBox();
	if (pRect) {
		pRect->autorelease();
		pRect->tag = true;
		pRect->initDateBox(pos, size, font, hAligment);
	}
	else
	{
		delete pRect;
		pRect = nullptr;
	}
	return pRect;
}

DropBox* DropBox::create(Vec2 pos, Size size, string plistPath, string font, TextHAlignment hAligment) {
	DropBox* pRect = new DropBox();
	if (pRect) {
		pRect->autorelease();
		pRect->tag = false;
		pRect->plitFileName = "DropBox/" + plistPath;
		pRect->initSiteBox(pos, size, font, hAligment);
	}
	else
	{
		delete pRect;
		pRect = nullptr;
	}
	return pRect;
}

//初始化日期选择框
void DropBox::initDateBox(Vec2 pos, Size size, string font, TextHAlignment hAligment) {
	for (int i = DROPBOX_STARTYEAR; i <= DROPBOX_STARTYEAR + DROPBOX_WIDTHYEAR; i++) {
		char str[10];
		sprintf(str, "%d", i);
		myParam1.push_back(string(str));
	}
	for (int i = 1; i < 13; i++)
	{
		char str[5];
		sprintf(str, "%02d", i);
		myParam2.push_back(string(str));
	}
	for (int i = 1; i < 32; i++)
	{
		char str[5];
		sprintf(str, "%02d", i);
		myParam3.push_back(string(str));
	}

	selectedStr1 = myParam1[0];
	selectedStr2 = myParam2[0];
	selectedStr3 = myParam3[0];

	//显示文本控件
	myLabel = Label::create(myParam1[0] + ".01.01", font, size.height);
	myLabel->setAnchorPoint(Vec2(0.0f, 0.0f));
	myLabel->setPosition(pos);
	myLabel->setColor(Color3B::BLACK);
	myLabel->setHorizontalAlignment(hAligment);
	addChild(myLabel);

	//触发选择列表的按钮
	auto myMenu = MenuItemImage::create("DropBox/1000X100.png", "DropBox/1000X100.png", CC_CALLBACK_1(DropBox::menuCallBack1, this));
	myMenu->setScaleX(size.width / 400.0f);
	myMenu->setScaleY(size.height / 40.0f);
	myMenu->setAnchorPoint(Vec2(0.0f, 0.0f));
	myMenu->setPosition(pos);

	//屏蔽触摸
	imgView = ImageView::create("DropBox/1000X100.png");
	imgView->setScaleX(size.width / 400.0f);
	imgView->setScaleY(size.height / 40.0f);
	imgView->setTouchEnabled(false);
	imgView->setAnchorPoint(Vec2(0.5f, 0.5f));
	imgView->setPosition(Vec2(547, 307));
	imgView->addClickEventListener([=](Ref* sender) {
		log("this is ImgView");
	});
	addChild(imgView);

	auto menu = Menu::create(myMenu, NULL);
	menu->setAnchorPoint(Vec2(0, 0));
	menu->setPosition(Vec2(0, 0));
	addChild(menu);

	//半透明黑底
	bgLayer = LayerColor::create(Color4B::BLACK, 1094, 614);
	bgLayer->setAnchorPoint(Vec2(0, 0));
	bgLayer->setOpacity(100);
	bgLayer->setVisible(false);
	addChild(bgLayer);

	//提交按钮
	//auto commit = MenuItemImage::create("DropBox/commit2.png", "DropBox/commit1.png", CC_CALLBACK_1(DropBox::menuCallBack2, this));
	//auto commit = Button::create("other/bianji.png", "DropBox/bianji2.png");
	//commit->setTitleText(App::getString("TIJIAO"));
	//commit->setAnchorPoint(Vec2(0.5f, 0.5f));
	//commit->setPosition(Vec2(547, 40));
	//commit->addClickEventListener(CC_CALLBACK_1(DropBox::menuCallBack2, this));
	//auto menu2 = Menu::create(commit, NULL);
	//menu2->setAnchorPoint(Vec2(0, 0));
	//menu2->setPosition(Vec2(0, 0));
	//bgLayer->addChild(commit);
	auto commit = Button::create("other/bianji.png", "other/bianji2.png");
	commit->setTitleText(App::getString("TIJIAO"));
	commit->setTitleColor(Color3B::WHITE);
	commit->setTitleFontSize(50);
	commit->setTitleFontName("FZHLJW.TTF");
	commit->setScale(0.5);
	commit->setAnchorPoint(Vec2(0.5f, 0.5f));
	commit->setPosition(Vec2(547, 40));
	commit->addClickEventListener(CC_CALLBACK_1(DropBox::menuCallBack2, this));
	bgLayer->addChild(commit);

	//圆角白底图
	auto bgImg = Sprite::create("DropBox/2000X1200X3.png");
	bgImg->setAnchorPoint(Vec2(0, 0));
	bgImg->setPosition(Vec2(147, 80));
	bgLayer->addChild(bgImg);

	//选择列表1
	myListView1 = ListView::create();
	myListView1->setDirection(SCROLLVIEW_DIR_VERTICAL);
	myListView1->setBounceEnabled(true);
	myListView1->setTouchEnabled(false);
	myListView1->setScrollBarEnabled(false);
	myListView1->setVisible(false);
	myListView1->setAnchorPoint(Vec2(0, 1));
	myListView1->setPosition(Vec2(0, 456));
	myListView1->setSize(Size(DROPBOX_WIDTH_DATE, DROPBOX_HEIGHT*DROPBOX_PARAMCOUNT));
	myListView1->addEventListenerListView(this, listvieweventselector(DropBox::selectedItemEventDate1));
	bgImg->addChild(myListView1);

	for (int i = 0; i < myParam1.size(); i++) {
		auto widget = Layout::create();
		widget->setTouchEnabled(true);

		auto layerC = LayerColor::create(ccc4(0, 0, 0, 100), DROPBOX_WIDTH_DATE, DROPBOX_HEIGHT);
		layerC->setAnchorPoint(Vec2(0, 0));
		if (i != 0) layerC->setVisible(false);
		widget->addChild(layerC, 0, 10);
		auto item = LabelTTF::create(myParam1[i], "hkhb", DROPBOX_HEIGHT*0.8f);
		item->setAnchorPoint(Vec2(0.5, 0.5));
		item->setPosition(Vec2(DROPBOX_WIDTH_DATE / 2.0f, DROPBOX_HEIGHT / 2));
		item->setColor(Color3B::BLACK);
		widget->addChild(item, 0, 11);

		widget->setContentSize(layerC->getContentSize());
		myListView1->pushBackCustomItem(widget);
	}

	//选择列表2
	myListView2 = ListView::create();
	myListView2->setDirection(SCROLLVIEW_DIR_VERTICAL);
	myListView2->setBounceEnabled(true);
	myListView2->setTouchEnabled(false);
	myListView2->setScrollBarEnabled(false);
	myListView2->setVisible(false);
	myListView2->setAnchorPoint(Vec2(0, 1));
	myListView2->setPosition(Vec2(DROPBOX_WIDTH_DATE + 1, 456));
	myListView2->setSize(Size(DROPBOX_WIDTH_DATE, DROPBOX_HEIGHT*DROPBOX_PARAMCOUNT));
	myListView2->addEventListenerListView(this, listvieweventselector(DropBox::selectedItemEventDate2));
	bgImg->addChild(myListView2);

	for (int i = 0; i < myParam2.size(); i++) {
		auto widget = Layout::create();
		widget->setTouchEnabled(true);

		auto layerC = LayerColor::create(ccc4(0, 0, 0, 100), DROPBOX_WIDTH_DATE, DROPBOX_HEIGHT);
		layerC->setAnchorPoint(Vec2(0, 0));
		if (i != 0) layerC->setVisible(false);
		widget->addChild(layerC, 0, 10);
		auto item = LabelTTF::create(myParam2[i], "hkhb", DROPBOX_HEIGHT*0.8f);
		item->setAnchorPoint(Vec2(0.5, 0.5));
		item->setPosition(Vec2(DROPBOX_WIDTH_DATE / 2.0f, DROPBOX_HEIGHT / 2));
		item->setColor(Color3B::BLACK);
		widget->addChild(item, 0, 11);

		widget->setContentSize(layerC->getContentSize());
		myListView2->pushBackCustomItem(widget);
	}

	//选择列表3
	myListView3 = ListView::create();
	myListView3->setDirection(SCROLLVIEW_DIR_VERTICAL);
	myListView3->setBounceEnabled(true);
	myListView3->setTouchEnabled(false);
	myListView3->setScrollBarEnabled(false);
	myListView3->setVisible(false);
	myListView3->setAnchorPoint(Vec2(0, 1));
	myListView3->setPosition(Vec2(DROPBOX_WIDTH_DATE * 2 + 2, 456));
	myListView3->setSize(Size(DROPBOX_WIDTH_DATE, DROPBOX_HEIGHT*DROPBOX_PARAMCOUNT));
	myListView3->addEventListenerListView(this, listvieweventselector(DropBox::selectedItemEventDate3));
	bgImg->addChild(myListView3);

	for (int i = 0; i < myParam3.size(); i++) {
		auto widget = Layout::create();
		widget->setTouchEnabled(true);

		auto layerC = LayerColor::create(ccc4(0, 0, 0, 100), DROPBOX_WIDTH_DATE, DROPBOX_HEIGHT);
		layerC->setAnchorPoint(Vec2(0, 0));
		if (i != 0) layerC->setVisible(false);
		widget->addChild(layerC, 0, 10);
		auto item = LabelTTF::create(myParam3[i], "hkhb", DROPBOX_HEIGHT*0.8f);
		item->setAnchorPoint(Vec2(0.5, 0.5));
		item->setPosition(Vec2(DROPBOX_WIDTH_DATE / 2.0f, DROPBOX_HEIGHT / 2));
		item->setColor(Color3B::BLACK);
		widget->addChild(item, 0, 11);

		widget->setContentSize(layerC->getContentSize());
		myListView3->pushBackCustomItem(widget);
	}
}

//初始化地点选择框
void DropBox::initSiteBox(Vec2 pos, Size size, string font, TextHAlignment hAligment) {
	auto plistDic = Dictionary::createWithContentsOfFile(plitFileName.c_str());

	DictElement *pElement;
	CCDICT_FOREACH(plistDic, pElement)
	{
		myParam1.push_back(string(pElement->getStrKey()));
	}

	auto paramArr = (Array*)plistDic->objectForKey(myParam1[0]);
	for (int i = 0; i < paramArr->count(); i++) {
		auto str = (__String*)paramArr->objectAtIndex(i);
		myParam2.push_back(string(str->getCString()));
	}

	selectedStr1 = myParam1[0];
	selectedStr2 = myParam2[0];
	strProvince = selectedStr1;
	strCity = selectedStr2;

	//显示文本控件
	myLabel = Label::create(strProvince + strCity, font, size.height);
	myLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
	myLabel->setPosition(pos);
	myLabel->setColor(ccColor3B::BLACK);
	myLabel->setHorizontalAlignment(hAligment);
	addChild(myLabel);

	//触发选择列表的按钮
	auto myMenu = MenuItemImage::create("DropBox/1000X100.png", "DropBox/1000X100.png", CC_CALLBACK_1(DropBox::menuCallBack1, this));
	myMenu->setScaleX(size.width / 400.0f);
	myMenu->setScaleY(size.height / 40.0f);
	myMenu->setAnchorPoint(Vec2(0.5f, 0.5f));
	myMenu->setPosition(pos);

	auto menu = Menu::create(myMenu, NULL);
	menu->setAnchorPoint(Vec2(0, 0));
	menu->setPosition(Vec2(0, 0));
	addChild(menu);

	//屏蔽触摸
	imgView = ImageView::create("DropBox/1000X100.png");
	imgView->setScaleX(size.width / 400.0f);
	imgView->setScaleY(size.height / 40.0f);
	imgView->setTouchEnabled(false);
	imgView->setAnchorPoint(Vec2(0, 0));
	imgView->addClickEventListener([=](Ref* sender) {
		log("this is ImgView");
	});
	addChild(imgView);

	//半透明黑底
	bgLayer = LayerColor::create(Color4B::BLACK, 1094, 614);
	bgLayer->setAnchorPoint(Vec2(0, 0));
	bgLayer->setOpacity(100);
	bgLayer->setVisible(false);
	addChild(bgLayer);

	//提交按钮
	//auto commit = MenuItemImage::create("DropBox/commit2.png", "DropBox/commit1.png", CC_CALLBACK_1(DropBox::menuCallBack2, this));
	//commit->setAnchorPoint(Vec2(0.5f, 0.5f));
	//commit->setPosition(547, 40);
	//auto menu2 = Menu::create(commit, NULL);
	//menu2->setAnchorPoint(Vec2(0, 0));
	//menu2->setPosition(Vec2(0, 0));
	//bgLayer->addChild(menu2);

	//auto commit = MenuItemImage::create("DropBox/commit2.png", "DropBox/commit1.png", CC_CALLBACK_1(DropBox::menuCallBack2, this));
	auto commit = Button::create("other/bianji.png", "other/bianji2.png");
	commit->setTitleText(App::getString("TIJIAO"));
	commit->setTitleColor(Color3B::WHITE);
	commit->setTitleFontSize(50);
	commit->setTitleFontName("FZHLJW.TTF");
	commit->setScale(0.5);
	commit->setAnchorPoint(Vec2(0.5f, 0.5f));
	commit->setPosition(Vec2(547, 40));
	commit->addClickEventListener(CC_CALLBACK_1(DropBox::menuCallBack2, this));
	//auto menu2 = Menu::create(commit, NULL);
	//menu2->setAnchorPoint(Vec2(0, 0));
	//menu2->setPosition(Vec2(0, 0));
	bgLayer->addChild(commit);

	//圆角白底图
	auto bgImg = Sprite::create("DropBox/2000X1200X2.png");
	bgImg->setAnchorPoint(Vec2(0, 0));
	bgImg->setPosition(Vec2(147, 80));
	bgLayer->addChild(bgImg);

	//选择列表1
	myListView1 = ListView::create();
	myListView1->setDirection(SCROLLVIEW_DIR_VERTICAL);
	myListView1->setBounceEnabled(true);
	myListView1->setTouchEnabled(false);
	myListView1->setScrollBarEnabled(false);
	myListView1->setVisible(false);
	myListView1->setAnchorPoint(Vec2(0, 1));
	myListView1->setPosition(Vec2(0, 456));
	myListView1->setSize(Size(DROPBOX_WIDTH_SITE, DROPBOX_HEIGHT*DROPBOX_PARAMCOUNT));
	myListView1->addEventListenerListView(this, listvieweventselector(DropBox::selectedItemEvent1));
	bgImg->addChild(myListView1);

	for (int i = 0; i < myParam1.size(); i++) {
		auto widget = Layout::create();
		widget->setTouchEnabled(true);

		auto layerC = LayerColor::create(ccc4(0, 0, 0, 100), DROPBOX_WIDTH_SITE, DROPBOX_HEIGHT);
		layerC->setAnchorPoint(Vec2(0, 0));
		if (i != 0) layerC->setVisible(false);
		widget->addChild(layerC, 0, 10);
		auto item = LabelTTF::create(myParam1[i], "hkhb", DROPBOX_HEIGHT*0.8f);
		item->setAnchorPoint(Vec2(0.5, 0.5));
		item->setPosition(Vec2(DROPBOX_WIDTH_SITE / 2.0f, DROPBOX_HEIGHT / 2));
		item->setColor(Color3B::BLACK);
		widget->addChild(item, 0, 11);

		widget->setContentSize(layerC->getContentSize());
		myListView1->pushBackCustomItem(widget);
	}

	//选择列表2
	myListView2 = ListView::create();
	myListView2->setDirection(SCROLLVIEW_DIR_VERTICAL);
	myListView2->setBounceEnabled(true);
	myListView2->setTouchEnabled(false);
	myListView2->setScrollBarEnabled(false);
	myListView2->setVisible(false);
	myListView2->setAnchorPoint(Vec2(0, 1));
	myListView2->setPosition(Vec2(DROPBOX_WIDTH_SITE, 456));
	myListView2->setSize(Size(DROPBOX_WIDTH_SITE, DROPBOX_HEIGHT*DROPBOX_PARAMCOUNT));
	myListView2->addEventListenerListView(this, listvieweventselector(DropBox::selectedItemEvent2));
	bgImg->addChild(myListView2);

	changeItemSite();
}

void DropBox::changeItemSite() {
	lastIndex2 = 0;
	selectedStr2 = myParam2[lastIndex2];
	myListView2->removeAllItems();
	for (int i = 0; i < myParam2.size(); i++) {
		auto widget = Layout::create();
		widget->setTouchEnabled(true);

		auto layerC = LayerColor::create(ccc4(0, 0, 0, 100), DROPBOX_WIDTH_SITE, DROPBOX_HEIGHT);
		layerC->setAnchorPoint(Vec2(0, 0));
		if (i != 0) layerC->setVisible(false);
		widget->addChild(layerC, 0, 10);
		auto item = LabelTTF::create(myParam2[i], "hkhb", DROPBOX_HEIGHT * 0.7f);
		item->setAnchorPoint(Vec2(0.5, 0.5));
		item->setPosition(Vec2(DROPBOX_WIDTH_SITE / 2.0f, DROPBOX_HEIGHT/2));
		item->setColor(Color3B::BLACK);
		widget->addChild(item, 0, 11);

		widget->setContentSize(layerC->getContentSize());
		myListView2->pushBackCustomItem(widget);
	}
}

void DropBox::setDateText(string text) {
	myLabel->setString(text);
}

void  DropBox::setSiteText(string site1, string site2) {
	strProvince = site1;
	strCity = site2;
	myLabel->setString(site1 + site2);
}

void  DropBox::setSiteText(string text) {
	myLabel->setString(text);
}

string DropBox::getDateText() {
	return myLabel->getString();
}

vector<string> DropBox::getSiteText() {
	vector<string> myText;
	myText.push_back(strProvince);
	myText.push_back(strCity);
	return myText;
}

void DropBox::menuCallBack1(Ref* sender) {
	if (!isEnable)
		return;
	bgLayer->setVisible(true);
	imgView->setTouchEnabled(true);
	myListView1->setTouchEnabled(true);
	myListView2->setTouchEnabled(true);
	myListView1->setVisible(true);
	myListView2->setVisible(true);
	if (myListView3) {
		myListView3->setTouchEnabled(true);
		myListView3->setVisible(true);
	}
}

void DropBox::menuCallBack2(Ref* sender) {
	bgLayer->setVisible(false);
	imgView->setTouchEnabled(false);
	myListView1->setTouchEnabled(false);
	myListView2->setTouchEnabled(false);
	myListView1->setVisible(false);
	myListView2->setVisible(false);

	if (tag) {
		myLabel->setString(selectedStr1 + "." + selectedStr2 + "." + selectedStr3);
		myListView3->setTouchEnabled(false);
		myListView3->setVisible(false);
	}
	else
	{
		strProvince = selectedStr1;
		strCity = selectedStr2;
		myLabel->setString(selectedStr1 + selectedStr2);
	}
}

void DropBox::selectedItemEvent1(Ref *pSender, ListViewEventType type) {
	
	switch (type) {
	case cocos2d::ui::LISTVIEW_ONSELECTEDITEM_END:
	{
		ListView* listView = static_cast<ListView*>(pSender);

		if(lastIndex1 == listView->getCurSelectedIndex())
			break;

		auto selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex1));
		auto layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(false);

		lastIndex1 = listView->getCurSelectedIndex();
		selectedStr1 = myParam1[lastIndex1];

		selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex1));
		layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(true);

		auto plistDic = Dictionary::createWithContentsOfFile(plitFileName.c_str());
		auto paramArr = (Array*)plistDic->objectForKey(selectedStr1);
		myParam2.clear();
		for (int i = 0; i < paramArr->count(); i++) {
			auto str = (__String*)paramArr->objectAtIndex(i);
			myParam2.push_back(string(str->getCString()));
		}
		changeItemSite();
	}
	break;
	default:
		break;
	}
}
void DropBox::selectedItemEvent2(Ref *pSender, ListViewEventType type) {

	switch (type) {
	case cocos2d::ui::LISTVIEW_ONSELECTEDITEM_END:
	{
		ListView* listView = static_cast<ListView*>(pSender);

		if (lastIndex2 == listView->getCurSelectedIndex())
			break;

		auto selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex2));
		auto layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(false);

		lastIndex2 = listView->getCurSelectedIndex();
		selectedStr2 = myParam2[lastIndex2];

		selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex2));
		layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(true);
	}
	break;
	default:
		break;
	}
}

void DropBox::selectedItemEventDate1(Ref *pSender, ListViewEventType type) {

	switch (type) {
	case cocos2d::ui::LISTVIEW_ONSELECTEDITEM_END:
	{
		ListView* listView = static_cast<ListView*>(pSender);

		if (lastIndex1 == listView->getCurSelectedIndex())
			break;

		auto selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex1));
		auto layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(false);

		lastIndex1 = listView->getCurSelectedIndex();
		selectedStr1 = myParam1[lastIndex1];

		selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex1));
		layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(true);
	}
	break;
	default:
		break;
	}
}
void DropBox::selectedItemEventDate2(Ref *pSender, ListViewEventType type) {

	switch (type) {
	case cocos2d::ui::LISTVIEW_ONSELECTEDITEM_END:
	{
		ListView* listView = static_cast<ListView*>(pSender);

		if (lastIndex2 == listView->getCurSelectedIndex())
			break;

		auto selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex2));
		auto layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(false);

		lastIndex2 = listView->getCurSelectedIndex();
		selectedStr2 = myParam2[lastIndex2];

		selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex2));
		layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(true);
	}
	break;
	default:
		break;
	}
}
void DropBox::selectedItemEventDate3(Ref *pSender, ListViewEventType type) {

	switch (type) {
	case cocos2d::ui::LISTVIEW_ONSELECTEDITEM_END:
	{
		ListView* listView = static_cast<ListView*>(pSender);

		if (lastIndex3 == listView->getCurSelectedIndex())
			break;

		auto selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex3));
		auto layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(false);

		lastIndex3 = listView->getCurSelectedIndex();
		selectedStr3 = myParam3[lastIndex3];

		selectedLayout = static_cast<Layout*>(listView->getItem(lastIndex3));
		layColor = dynamic_cast<LayerColor*>(selectedLayout->getChildByTag(10));
		layColor->setVisible(true);
	}
	break;
	default:
		break;
	}
}

void DropBox::setDropEnable(bool enable) {
	isEnable = enable;
}
