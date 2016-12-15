#include "YYXTableView.h"

USING_NS_CC;
USING_NS_CC_EXT;

YYXTableView::YYXTableView()
{
}

void YYXTableView::tableCellTouched(TableView* table, TableViewCell* cell)
{
	auto index = cell->getIdx();
}

TableViewCell* YYXTableView::tableCellAtIndex(TableView *table, ssize_t idx)
{
	/* auto cell = table->dequeueCell();
	 if (!cell)
	 {
		 cell = TableViewCell::create();
		 auto label = Label::createWithTTF(_childTestNames[idx], "fonts/arial.ttf", 20.0f);
		 label->setTag(TABEL_LABEL_TAG);
		 label->setPosition(200, 15);
		 cell->addChild(label);
	 }
	 else
	 {
		 auto label = (Label*)cell->getChildByTag(TABEL_LABEL_TAG);
		 label->setString(_childTestNames[idx]);
	 }*/

    //return cell;
	return nullptr;
}

Size YYXTableView::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
    return Size(400, 30);
}

ssize_t YYXTableView::numberOfCellsInTableView(TableView *table)
{
    //return _childTestNames.size();
	return 0;
}