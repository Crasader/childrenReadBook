#pragma once
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;

using namespace std;
//using namespace cocos2d::ui;

class MyEditBox : public Node, public cocos2d::ui::EditBoxDelegate
{
public:
	MyEditBox();
	~MyEditBox();
	virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)override;
	virtual void editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox)override;
	virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)override;
	virtual void editBoxReturn(cocos2d::ui::EditBox* editBox)override;

	static MyEditBox* create(Size, string);
	void initEditBox(Size, string);
	void setMyInputMode(cocos2d::ui::EditBox::InputMode);
	void setMyInputFlag(cocos2d::ui::EditBox::InputFlag);
	void setBoxMaxLength(int);
	void setMyFontSize(int);
	string getString();
	void setString(string);
private:
	cocos2d::ui::EditBox* m_editbox;
};


