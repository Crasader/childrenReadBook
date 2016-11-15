#ifndef __WAITING_SCENE_H__
#define __WAITING_SCENE_H__

#include "cocos2d.h"
#include "IndexScene.h"
#include "App.h"
#include "FKBookParser.h"
class Waiting : public cocos2d::Layer
{
public:
	static Scene * createScene(std::string bookId, bool isView);

	static Waiting * create(std::string bookId, bool isView);

	virtual bool init(std::string bookId, bool isView);
	//解析出bookName 进行分享
	void Share(string json, string path);
	//void saveReadRecord();
	void saveReadRecordStart(string bookid, bool isView);
	void saveReadRecording(string bookid, bool isView);
	void saveReadRecordEnd(string bookid, bool isView);
	Layer* tryReadBackCover(int bookid);
	Layer* BackCover(int bookid);
	void httpGetReadTime(int bookId);
	void initResource(Layer* layer);
};

#endif // __HELLOWORLD_SCENE_H__
