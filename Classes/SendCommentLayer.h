#ifndef __SendCommentLayer_HH__
#define __SendCommentLayer_HH__

#include "cocos2d.h"
#include "FileNames.h"
#include "App.h"
#include "MyEditBox.h"
class SendComment : public cocos2d::Layer
{
public:
	SendComment();
	~SendComment();
	bool init(int);
	static SendComment* create(int);
	void sendComment(string, string);
	void sendOver(int);
	void showXingJi(int data, ImageView* xing2, ImageView* xing3, ImageView* xing4, ImageView* xing5);
private:
	int m_bookId;
	int m_orderId;
	int star_score;
};

#endif //__SendCommentLayer_HH__