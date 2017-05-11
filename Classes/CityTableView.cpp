#include "CityTableView.h"
#include "CityButton.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define LAYOUTTAG 1024
#define ACTION 1025

bool CityTableView::init()
{
	m_tv = TableView::create(this, Size(Director::getInstance()->getWinSize().width - 40, 600));
	m_tv->setAnchorPoint(Vec2(0, 0));
	m_tv->setPosition(Vec2(0, 0));
	m_tv->setDelegate(this);
	m_tv->setDirection(cocos2d::extension::ScrollView::Direction::HORIZONTAL);
	m_tv->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	addChild(m_tv);
	return true;
}

void CityTableView::tableCellTouched(TableView* table, TableViewCell* cell)
{
	auto index = cell->getIdx();
}

TableViewCell* CityTableView::tableCellAtIndex(TableView *table, ssize_t idx)
{
	auto cell = table->dequeueCell();
	auto data = m_data[idx];
	auto type = m_typeList[idx];
	int tag = 0;
	if (type == _double)
		tag = 2;
	else
		tag = 1;
	if (cell && cell->getTag() == tag)
	{
		reuseItem(cell, data, idx);
	}
	else
	{
		cell = TableViewCell::create();
		cell->addChild(createItem(data, idx));
	}
	if (type == _double)
		cell->setTag(2);
	else
		cell->setTag(1);
	return cell;
}

Size CityTableView::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	auto data = m_typeList[idx];
	if(data == _double)
		return Size(248,600);
	else
		return Size(320, 600);
}

ssize_t CityTableView::numberOfCellsInTableView(TableView *table)
{
	return m_data.size();
}

void CityTableView::loadData()
{
	if (BookStoreCache::getInstance()->getSequence().size() < 2);
		BookStoreCache::getInstance()->loadData();
	m_data.clear();
	m_typeList.clear();
	auto _data = BookStoreCache::getInstance()->getInfos();
	auto _storeids = BookStoreCache::getInstance()->getSequence();
	for (int i = 0; i < _storeids.size(); i++)
	{
		int id = _storeids[i];
		auto info = _data.find(id);
		if (info != _data.end())
		{
			auto _info = info->second;
			auto _type = _info->getType();
			Size item = Size::ZERO;
			if (_type == CityButtonType::_double)
			{
				item.width = _info->getStoreId();
				int j = i + 1;
				if (j < _storeids.size())
				{
					int id_next = _storeids[j];
					auto info_next = _data.find(id_next);
					if (info_next != _data.end())
					{
						auto _info_next = info_next->second;
						auto _type_next = _info_next->getType();
						if (_type_next == CityButtonType::_double)
						{
							++i;
							item.height = _info_next->getStoreId();
						}
					}
				}
			}
			else
			{
				item.width = _info->getStoreId();
			}
			m_typeList.push_back(_type);
			m_data.push_back(item);
		}
	}
	m_tv->reloadData();
}

Layout* CityTableView::createItem(Size  mapdata, int idx)
{
	auto layout = Layout::create();
	layout->setLayoutType(cocos2d::ui::Layout::Type::RELATIVE);
	layout->setTag(LAYOUTTAG);
	//layout->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
	//layout->setColor(Color3B::YELLOW);
	if (m_typeList[idx] == _double)
	{
		int id0 = mapdata.width;
		int id1 = mapdata.height;
		CityButtonInfo* data0 = BookStoreCache::getInstance()->getCityButtonInfo(id0);
		CityButtonInfo* data1 = BookStoreCache::getInstance()->getCityButtonInfo(id1);
		if (data0)//上面的按钮
		{
			//书店封面
			auto cover = CityButton::create(data0->getUrl(), data0->getStoreId(), Size(168,176));
			cover->setTag(0);
			auto coverViewParameter = RelativeLayoutParameter::create();
			coverViewParameter->setRelativeName("cover0");
			coverViewParameter->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_TOP_LEFT);
			coverViewParameter->setMargin(Margin(40, 40, 40, 0));
			cover->setLayoutParameter(coverViewParameter);
			layout->addChild(cover);
		}
		if (data1)//下面的按钮
		{
			//书店封面
			auto cover = CityButton::create(data1->getUrl(), data1->getStoreId(), Size(168, 176),_double);
			cover->setTag(1);
			auto coverViewParameter = RelativeLayoutParameter::create();
			coverViewParameter->setRelativeName("cover1");
			coverViewParameter->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_TOP_LEFT);
			coverViewParameter->setMargin(Margin(40, 250, 40, 0));
			coverViewParameter->setRelativeToWidgetName("cover0");
			cover->setLayoutParameter(coverViewParameter);
			layout->addChild(cover);
		}
	}
	else
	{
		int id0 = mapdata.width;
		CityButtonInfo* data0 = BookStoreCache::getInstance()->getCityButtonInfo(id0);
		if (data0)//大按钮
		{
			auto cover = CityButton::create(data0->getUrl(), data0->getStoreId(), Size(280, 294), _single);
			cover->setTag(3);
			auto coverViewParameter = RelativeLayoutParameter::create();
			coverViewParameter->setRelativeName("cover0");
			coverViewParameter->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_TOP_LEFT);
			coverViewParameter->setMargin(Margin(20, 100, 20, 0));
			cover->setLayoutParameter(coverViewParameter);
			layout->addChild(cover);
		}
	}
	layout->setSize(tableCellSizeForIndex(m_tv, idx));	
	return layout;
}

void CityTableView::reuseItem(TableViewCell*item, Size mapdata, int idx)
{
	auto layout = (Layout*)item->getChildByTag(LAYOUTTAG);

	if (m_typeList[idx] == _double)
	{
		int id0 = mapdata.width;
		int id1 = mapdata.height;
		CityButtonInfo* data0 = BookStoreCache::getInstance()->getCityButtonInfo(id0);
		CityButtonInfo* data1 = BookStoreCache::getInstance()->getCityButtonInfo(id1);
		if (data0 && id0 != 0)//上面的按钮
		{
			//书店封面
			auto cover = (CityButton*)layout->getChildByTag(0);
			cover->setVisible(true);
			cover->reloadUrl(data0->getUrl(), data0->getStoreId());
		}
		if (data1 && id1 != 0)//下面的按钮
		{
			//书店封面
			auto cover = (CityButton*)layout->getChildByTag(1);
			cover->setVisible(true);
			cover->reloadUrl(data1->getUrl(), data1->getStoreId());
		}
		else
		{
			auto cover = (CityButton*)layout->getChildByTag(1);
			cover->setVisible(false);
		}
	}
	else
	{
		int id0 = mapdata.width;
		CityButtonInfo* data0 = BookStoreCache::getInstance()->getCityButtonInfo(id0);
		if (data0)//大按钮
		{
			auto cover = (CityButton*)layout->getChildByTag(3);
			cover->reloadUrl(data0->getUrl(), data0->getStoreId());
		}
	}
	layout->setSize(tableCellSizeForIndex(m_tv, idx));
}