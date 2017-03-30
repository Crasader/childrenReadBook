#include "BookRoomSelectPage.h"

BookRoomSelectPage::BookRoomSelectPage()
{
	App::log("BookRoomSelectPage::BookRoomSelectPage()");
}

BookRoomSelectPage::~BookRoomSelectPage()
{
	App::log("BookRoomSelectPage::~BookRoomSelectPage()");
}

//std::vector<int> BookRoomSelectPage::getPageData(int pageNum, int pageCount)
//{
//	App::log("BookRoomSelectPage::getPageData()");
//	//vector<int> data;
//	//auto vecdata = getResultData();
//	//int booksum = vecdata.size();
//	//int begin;//第一本书的sort
//	//int end;//最后一本书的sort
//	//begin = pageNum * pageCount;
//	//end = pageNum * pageCount + pageCount;
//	//if (end >= booksum)
//	//	end = booksum;
//	//for (int i = begin; i < end; i++)
//	//{
//	//	if (i < 0 || i >= vecdata.size())
//	//		continue;
//	//	pair<int,int> bookItem = vecdata[i];
//	//	data.push_back(bookItem.first);
//	//}
//	//return data;
//	vector<int> data;
//	return data;
//}