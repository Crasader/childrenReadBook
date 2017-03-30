#ifndef _CityButton_H__
#define _CityButton_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "App.h"
#include "ui/UILayout.h"

USING_NS_CC;

class CityButton : public cocos2d::ui::ImageView
{
public:
	static CityButton * create(string url, int storeId, string defaultPath);
	bool init(string url, int storeId, string defaultPath);
	void reloadUrl(string url, int storeId);
	void addClickListener(const function<void(Ref*)>& callback);

	int getStoreId() const { return storeId; }
	void setStoreId(int val) { storeId = val; }

	std::string getUrl() const { return url; }
	void setUrl(std::string val) { url = val; }
private:
	string url = "";
	int storeId = -999;
	string defaultPath = "";
	void loadImage();
	void downloadImage();

private:
	string downloadDir = "";
	function<void(Ref*)> mycallback = nullptr;
};
#endif