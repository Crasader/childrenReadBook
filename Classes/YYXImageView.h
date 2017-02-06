#ifndef _YYXIMAGEVIEW_H__
#define _YYXIMAGEVIEW_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "App.h"
#include "ui/UILayout.h"

USING_NS_CC;

class YYXImageView : public cocos2d::ui::ImageView
{
public:
	static YYXImageView * create(string url, int memberId, string defaultPath);
	virtual bool init(string url, int memberId, string defaultPath);
	void reloadUrl(string url, int memberId);
	void setUrl(string url);
	string getUrl();
	void setMemberId(int);
	int getMemberId();
private:
	string url = "";
	int memberId = -999;
	string defaultPath = "";
	void loadImage();
	void downloadImage();
};
#endif