#ifndef __WAITING_SCENE_H__
#define __WAITING_SCENE_H__

#include "cocos2d.h"
#include "IndexScene.h"
#include "App.h"
#include "FKBookParser.h"
#include "YYXTableView.h"
class Waiting : public cocos2d::Layer
{
public:
	static Scene * createScene(std::string bookId, bool isView);

	static Waiting * create(std::string bookId, bool isView);

	virtual bool init(std::string bookId, bool isView);
	//解析出bookName 进行分享
	void Share(string json, string path);
	//void saveReadRecord();
	void saveReadRecordStart(string bookid, bool isView=false);
	void saveReadRecording(string bookid, bool isView=false);
	void saveReadRecordEnd(string bookid, bool isView=false);
	Layer* tryReadBackCover(int bookid);
	Layer* BackCover(int bookid);
	void httpGetReadTime(int bookId);
	void initResource(Layer* layer);

private:
	void ShareBook(RenderTexture* img, int bookId);//分享
	Layer* newTryCover(int bookid);
	static bool isBuyThisBook(int bookid);
	void yaoqingzhuce();
	void buySuccessMessageBox();
	void httpRentBook(int bookId, const function<void()> runable);
};

#endif // __HELLOWORLD_SCENE_H__
