#ifndef __YYXBuyBook_HH__
/*
购买书籍
1.请求账户余额 send("showyue")
2.请求账户红包 send("referRedPackets")
3.请求书籍的价格和书籍名称
4.请求购书  成功后回调函数
4.弹出第一个对话框  可选红包


用法:
init();初始化

*/
#define __YYXBuyBook_HH__
#include "cocos2d.h"
#include "App.h"

USING_NS_CC;
using namespace std;

class YYXBuyBook
{
private:
	static YYXBuyBook* instance;
	int m_bookId = 0;
	int m_memberId = 0;
	int m_bookPrice = 0;
	string m_bookName = "";
	int m_myMoney = 0;
	int m_hongbaoId = 0;
	int m_hongbao = 0;
	bool m_manualSelectHongbao = false;
	bool m_click = true;//点击的标示
	bool m_show = true;//网络请求后只允许一个弹窗

	function<void(int)> m_buyBookSuccessCallBack = nullptr;
	function<void()> m_outSceneCallBack = nullptr;//切换场景的时候 要先push当前场景 不然回不来

	void httpMoney();
	void httpRedPacket();
	void httpBookInfo();
	void httpBuyBook();
	Layer* selectRedPacke();//选红包界面
	Layer* buyBookFirst();//第一个买书界面
	Layer* buyBookSecond();//第二个买书界面
	int getOptimalRedPacket();//最优算法获取红包
	void listViewShowRedPacket(ListView* listview);//选红包界面的 红包列表刷新
	void feeCharging();//充值
	void gotoLogin();//去登录
public:
	YYXBuyBook();
	~YYXBuyBook();
	static YYXBuyBook* GetInstance();
	//购书入口
	void newBuyBook(int bookid, int memberid, const function<void(int)>& buyBookCallBack = nullptr, const function<void()>& goToLoginCallBack = nullptr);

	void init(int bookid, int memberid);
	void newBuyFreeBook(int bookid, int memberid, const function<void(int)>& buyBookCallBack = nullptr);
	void buyBook(const function<void(int)>& buyBookCallBack = nullptr, const function<void()>& goToLoginCallBack = nullptr);
	void setOutScene(const function<void()>& goToLoginCallBack = nullptr);
	int BookId() const { return m_bookId; }
	void BookId(int val) { m_bookId = val; }
	int MemberId() const { return m_memberId; }
	void MemberId(int val) { m_memberId = val; }
	std::string BookName() const { return m_bookName; }
	void BookName(std::string val) { m_bookName = val; }
	int Money() const { return m_myMoney; }
	void Money(int val) { m_myMoney = val; }
	int HongbaoId() const { return m_hongbaoId; }
	void HongbaoId(int val) { m_hongbaoId = val; }
	int Hongbao() const { return m_hongbao; }
	void Hongbao(int val) { m_hongbao = val; }
	void loadRedPacketData(Node* leftRedPacket, int i);

	int getBookPrice() const { return m_bookPrice; }
	void setBookPrice(int val) { m_bookPrice = val; }
};
#endif 