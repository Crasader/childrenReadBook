#include "MyEditBox.h"

MyEditBox::MyEditBox() {
	m_editbox = nullptr;
}

MyEditBox::~MyEditBox() {

}

MyEditBox* MyEditBox::create(Size size, string defaultText) {
	MyEditBox* pRect = new MyEditBox();
	if (pRect) {
		pRect->autorelease();
		pRect->initEditBox(size, defaultText);
	}
	else {
		pRect->release();
		pRect = nullptr;
	}
	return pRect;
}

void MyEditBox::initEditBox(Size size, string defaultText) {
	m_editbox = cocos2d::ui::EditBox::create(size, cocos2d::ui::Scale9Sprite::create("editBox.png"));// "editBox.png", cocos2d::ui::Widget::TextureResType::LOCAL);
	m_editbox->setFontColor(Color3B::BLACK);
	m_editbox->setPlaceHolder(defaultText.c_str());
	m_editbox->setReturnType(cocos2d::ui::EditBox::KeyboardReturnType::DONE);
	m_editbox->setDelegate(this); 
	addChild(m_editbox);
}

void MyEditBox::setMyInputFlag(cocos2d::ui::EditBox::InputFlag inputFlag) {
	m_editbox->setInputFlag(inputFlag);
}

void MyEditBox::setMyInputMode(cocos2d::ui::EditBox::InputMode inputMode) {
	m_editbox->setInputMode(inputMode);
}

void MyEditBox::setBoxMaxLength(int maxLength) {
	m_editbox->setMaxLength(maxLength);
}

void MyEditBox::setMyFontSize(int fontSize) {
	m_editbox->setFontSize(fontSize);
	m_editbox->setPlaceholderFontSize(fontSize);
}

string MyEditBox::getString() {
	return string(m_editbox->getText());
}

void MyEditBox::setString(string text) {
	m_editbox->setText(text.c_str());
}

void MyEditBox::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox)
{
}

void MyEditBox::editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox)
{
}

void MyEditBox::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)
{
}

void MyEditBox::editBoxReturn(cocos2d::ui::EditBox* editBox)
{
}