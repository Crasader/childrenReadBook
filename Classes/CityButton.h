#ifndef _CityButton_H__
#define _CityButton_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "App.h"
#include "ui/UILayout.h"
#include "BookCityCache.h"

USING_NS_CC;

class CityButton : public cocos2d::ui::ImageView
{
public:
	static CityButton * create(string url, int storeId, Size size = Size(150, 150), CityButtonType type2 = _double);
	bool init(string url, int storeId, Size size = Size(150, 150), CityButtonType type2 = _double);
	void addBorder();
	void reloadUrl(string url, int storeId);
	void addClickListener(int storeId);

	int getStoreId() const { return storeId; }
	void setStoreId(int val) { storeId = val; }

	std::string getUrl() const { return url; }
	void setUrl(std::string val) { url = val; }
private:
	string url = "";
	int storeId = -999;
	string defaultPath = "";
	Size m_size ;
	CityButtonType m_type = _double;
	TextureResType m_textureType = Widget::TextureResType::PLIST;
	void loadImage();
	void downloadImage();

private:
	string downloadDir = "";
	function<void(Ref*)> mycallback = nullptr;
};
#endif