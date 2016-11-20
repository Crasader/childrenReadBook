package org.cocos2dx.cpp;

import java.io.File;

public class MacroCode {
	
	//handle消息类型	
	public final static int LoadScene_AccountLogin = 10000;
	public final static int LoadScene_getChildDetails = 10001;
	public final static int LoadScene_GetBuyBook = 10002;
	public final static int LoadScene_GetBookCityButtonsInfo = 10003;
	public final static int BookCityScene_GetBookCityButtonsInfo = 10004;
	public final static int BookCityScene_DownLoadPicture = 10005;
	public final static int BookStoreScene_GetBookStorePageInfo = 10006;
	public final static int BookStoreScene_DownLoadCover = 10007;
	public final static int BookInfoScene_GetBookInfo = 10008;
	public final static int BookInfoScene_DownLoadCover = 10009;
	public final static int BOOKInfoActivity = 10010;
	public final static int BookInfoScene_GetComment = 10011;
	public final static int BookInfoScene_Back_ReturnCode = 10012;
	public final static int HttpException = 10013;
	public final static int LoadScene_AccountLoginException = 10014;
	public final static int BookInfoScene_DidYouBuyThisBookException = 10015;
	public final static int BabyCenterScene_UpImage = 10016;
	public final static int BabyCenterScene_REFERCHILDPORTRAIT = 10017;
	public final static int WeixinPay = 10018;
	public final static int WeixinPayResult = 10019;
	public final static int RechargeGetUserBalance = 10020;
	public final static int RechargeBuyBookSuccess = 10021;
	public final static int RechargeBuyBookMyBalanceIsNotEnough = 10022;
	public final static int RechargeBuyBookFail = 10023;
	public final static int LoadScene_AccountRegiste = 10024;
	public final static int MessageBox = 10025;
	public final static int BookInfoScene_SendComment = 10026;
	public final static int SUCCESS = 10027;
	public final static int FAIL = 10028;
	public final static int BookShare = 10029;
	public final static int openPhotoAlbumSelectImage = 10030;
	public final static int photographAlbumSelectImage = 10031;
	public final static int commentTheRecording = 10032;
	public final static int InviteFriendsToRegister = 10033;
	
	public final static String IP = "http://cloud.ellabook.cn";
//	public final static String IP = "http://192.168.10.10:8089";
//	public final static String IP = "http://114.55.175.152";
	public final static String RESOURCE = "android";//"iphone6";
	public final static String NET_LOGIN = "/ellabook-server/member/login.do";
	//登录post (username String 用户名& password String 密码) ----0男1女2保密
	public final static String NET_CHILD_DETAILS = "/ellabook-server/children/childrenList.do";
	//获取宝贝列表详情POST
	public final static String NET_BOOKROOMBOOKLIST = "/ellabook-server/mybook";
	//保存阅读记录
	public final static String NET_SAVEREADHISTORY = "/ellabook-server/member/saveReadHistory.do";
	//获取书房已购书本	
	public final static String NET_BOOKCITY = "/ellabook-server/castle";
	//获取书城的商店列表
	public final static String NET_BOOKSID = "/ellabook-server/castle/";
	//获取本店的书籍ID GET
	public final static String NET_BOOKINFO = "/ellabook-server/store/book/";
	//获取书籍详情
	public final static String NET_BOOKINFOSENDCOMMENT = "/ellabook-server/evaluate/evaluate_android";
	//发表评论
	public final static String NET_BOOKINFOSADDEVALUATE = "/ellabook-server/evaluate/addEvaluate";
	//发表文字评论
	public final static String NET_BOOKINFOSDELECTGOODSEVALUATE = "/ellabook-server/evaluate/delectGoodsEvaluate";
	//删除评论
	public final static String NET_BOOKINFOSGOODSEVALUATELIST = "/ellabook-server/evaluate/goodsEvaluateList.do";
	//获取评论列表
	public final static String NET_UPLOADAVATAR = "/ellabook-server/children/setAvatar.do";
	//上传头像	
	public final static String NET_GETAVATAR = "/ellabook-server/children/getAvatar.do";
	//获取邀请好友和红包列表
	public final static String NET_ISBUY="/ellabook-server/order/QueryOrderStatus.do";
	//查询书籍是否购买
	public final static String NET_GETRECHARGE = "/ellabook-server/order/queryMemberAmount.do";
	//获取用户余额
	public final static String NET_GETRECHARGEORDERID = "/ellabook-server/order/GetChargeid_aliypay";
	//充值订单
	public final static String NET_RECHARG_BUYBOOK_ORDERID = "/ellabook-server/order";
	//余额购书下订单
	public final static String NET_RECHARG_BUYBOOK_RECHARGECHARGE = "/ellabook-server/order/successCharge";
	//余额扣款
	public final static String NET_RECHARG_BUYBOOK ="/ellabook-server/balancePay/pay";
	//余额购书一次性完成购书
	public final static String NET_REGISTER = "/ellabook-server/coupon_samuel/createCouponBusiness.do";//"/ellabook-server/member";
	//注册
	public final static String NET_ADDDOWNLOADRECODE ="/ellabook-server/book/adddownloadrecord.do";
	//上传下载记录
	public final static String NET_USERREDPACKET ="/ellabook-server/coupon_samuel/getActiveCouponList.do";
	//获取用户有效红包列表
	public final static String NET_BUYBOOKWITHREDPACKET = "/ellabook-server/coupon_samuel/pay";
	//使用红包购书
	public final static String NET_GETFIRSTCHILDHEADPORTRAIT = "/ellabook-server/children/getChildrenAvatar";
	//获取第一个孩子的iD和头像
	public static final String NET_ADDCHILD = "/ellabook-server/children/addChildren_android.do";
	//添加宝贝信息
	public static final String NET_DELETECHILD = "/ellabook-server/children/deleteChildren.do";
	//删除宝贝信息
	public static final String NET_MODIFY_CHILD_INFO = "/ellabook-server/children/updateChildren_android.do";
	 //修改宝贝信息POST
	public static final String NET_MODIFY_CITY = "/ellabook-server/member/updateMemberInfo_android.do";
	//修改城市资料 POST
	
	public final static String DB_USERINFO = "userinfo";//用户信息 表名 
	public final static String DB_BOOKINFO = "bookInfo";//书籍详细信息 表名 
	public final static String DB_CHILDREN = "children";//小孩信息 表名
	public final static String DB_RES = "resCache";//资源文件表 表名
	public final static String DB_USERBOOKLIST = "userBookList";//用户已购书籍列表 表名
	public final static String DB_STOREINFO = "storeList";//书店信息表 表名
	public final static String DB_BOOKLIST = "storeForBookList";//书店包含的所有书籍 表名
//	//-----------------------------------------数据库-------------------------------------------------------------------------
//	#define 
//
//	#define DB_BOOSSTORELIST "bookStoreList"//书店列表 表名//
	

//	#define DB_DOWNLOAD "download"//下载信息 表名



//
//	#define NET_REGISTER "/ellabook-server/member"//注册post 
//
//	#define NET_GETCODE "/ellabook-server/note/send.do"//获取注册验证码
//
//	#define NET_ADDCHILD "/ellabook-server/children/childrenList.do"//添加宝贝信息
//
//	#define NET_CHANGECHILDINFO "/ellabook-server/children/updateChildren.do"//修改孩子信息
//
//	#define NET_CHANGEPASSWORD "/ellabook-server/member/modifyPassword.do"//修改密码
//
//	#define NET_BOOKSTORE "/ellabook-server/oss/queryOssStoreDressInfo.do"//获取书店信息 废弃
//
//	#define NET_FEEDBACKSAVE "/ellabook-server/feedback/save.do"//用户反馈信息 POST
//
//	#define NET_MODIFY_CITY "/ellabook-server/member/updateMemberInfo.do"//修改城市资料 POST
//
//	#define NET_USERDETAILS "/ellabook-server/member/"//获取用户信息 GET
//
//	#define NET_MODIFY_PASSWORD "/ellabook-server/member/modifyPassword.do"//用户修改密码 POST
//
//	#define NET_QUERYREADHISTORY "/ellabook-server/member/queryReadHistory.do"//阅读记录
//
//	#define NET_SAVE_READHISTORY "/ellabook-server/member/saveReadHistory.do"//保存阅读记录
//
//	#define  NET_GET_ORDER "/ellabook-server/order" //POST获取订单号,GET更新后台已购书籍列表
//
//	#define  NET_CHILD_DETAILS "/ellabook-server/children/childrenList.do" //获取宝贝详情POST
//
//	#define  NET_MODIFY_CHILD_INFO "/ellabook-server/children/updateChildren.do" //修改宝贝信息POST
}
