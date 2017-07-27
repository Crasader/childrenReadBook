#pragma once
#ifndef __SETBOOK_H__
#define __SETBOOK_H__

#include <string>
#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class ReadBookRecord : public Ref
{
public:
	ReadBookRecord();
	~ReadBookRecord();
	static ReadBookRecord* create();
	static void del(ReadBookRecord*);
	void WriteFile();
	void httpUploadReadingRecord(function<void(ReadBookRecord*)> callback = nullptr);

	int getReadBookId() const { return this->readBookId; }
	void setReadBookId(int val) {
		this->readBookId = val;
		CCLOG("readBookId = %d", this->readBookId);
	}
	int getReadMemberId() const { return this->readMemberId; }
	void setReadMemberId(int val) {
		this->readMemberId = val;
		CCLOG("setReadMemberId = %d", this->readMemberId);
	}
	int getReadChildren() const { return this->readChildren; }
	void setReadChildren(int val) {
		this->readChildren = val;
		CCLOG("readChildren = %d", this->readChildren);
	}
	std::string getReadStartTime() const { return this->readStartTime; }
	void setReadStartTime(std::string val) {
		this->readStartTime = val; 
		CCLOG("readStartTime = %s", this->readStartTime.c_str());
	}
	std::string getReadEndTime() const { return this->readEndTime; }
	void setReadEndTime(std::string val) {
		this->readEndTime = val;
		CCLOG("============================================>>>>>>>>readEndTime = %s", this->readEndTime.c_str());
	}
	bool getIsHttp() const { return isHttp; }
	void setIsHttp(bool val) { isHttp = val; }

private:
	int readBookId;
	int readMemberId;
	int readChildren;
	string readStartTime;
	string readEndTime;
	bool isHttp;
};

class SetBook
{
public:
	SetBook();
	~SetBook();
	static SetBook* getInstance();
	void readBook(int bookId, bool isview);

	int getBookId() const { return bookId; }
	SetBook* setBookId(int val) { 
		bookId = val; 
		return this;
	}
	bool getIsView() const { return isView; }
	SetBook* setIsView(bool val) {
		isView = val; 
		return this;
	}
	int getMemberId() const { return memberId; }
	SetBook* setMemberId(int val) {
		memberId = val; 
		return this;
	}
	int getChildrenId() const { return childrenId; }
	SetBook* setChildrenId(int val) {
		childrenId = val;
		return this;
	}

	long long getOpacityTime() const { return opacityTime; }
	void setOpacityTime(int val) { opacityTime = val; }

private:
	static SetBook* instance;
	int bookId = -999;
	int memberId = -999;
	int childrenId = -999;
	bool isView = false;//是否试读
	ReadBookRecord* m_CurrentReadBook=nullptr;//当前正在阅读的书籍记录
	long long opacityTime = 0;

	void zushunet(int bookid);//租书流程
	void readBook();//设置书内按钮
	void readButton();
	//定时 点击常亮 10秒后恢复半透明
	void setButtonOpacity(int time = 11);
	//阅读记录
	void saveReadRecordStart();
	void saveReadRecording( );
	void saveReadRecordEnd( );
	void ShareBook(cocos2d::RenderTexture* img, int bookId);
	void initResource(cocos2d::Layer* layer);
};

#endif