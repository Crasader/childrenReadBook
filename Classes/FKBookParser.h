#ifndef __BOOK_PARSER_H__
#define __BOOK_PARSER_H__

#include "FKBookData.h"
#include "FKJsonParser.h"

NS_FK_BEGIN

class FK_DLL BookParser
{
public:
    static BookParser* getInstance();

	typedef std::function<void(PlayModeState playMode)> PlayModeChangeCallBack;

	typedef std::function<void()>						PageCallBack;
	typedef std::function<void(RenderTexture*)>			ShareCallBack;
    typedef std::function<void(Menu*)>                  PageMenuChangeCallBack;
	
	enum { NO_PAGE_TO_TURN = 0};

	void init();

	Scene* getRootScene();

    void end();
    
    BookParser();
    ~BookParser();
    
    /**
     *  将书籍路径、素材比例和坐标比例传入，进行书籍解析
     *
     *  @param sBookPath            书籍路径，书籍绝对路径
     *  @param resourcesScale       素材比例，决定素材显示的缩放比例
     *  @param coordinateScale      坐标比例，决定素材坐标的缩放比例
     *  @param platform             平台类型，ios、android、win32
     *
     *  @return error code           0:parser success
                                    -1:default
                                    -2:book path not exist
                                    -3:json path not exist
                                    -4:json is empty
     */
    int bookJsonParser(string sBookPath,float resourcesScale,float coordinateScale,string platform);
    
    //书籍播放模式
    void setBookPlayModeState(PlayModeState playMode);
    PlayModeState getBookPlayModeState();

	//暂停/恢复播放
	void pausePlay();
	void resumePlay();

	bool getPaused() { return _isPaused; }
    
    //书籍数据
    BookData getBookData();
    void setBookData(BookData bookData);
    
    //当前书页
    int getCurrentPage();
    void setCurrentPage(int iCurrentPage);

	/**
	*  向上翻页
	*  @return int          ：[1,MAX_INT) 当前页数
	*						：NO_PAGE_TO_TURN 当前书本已经在第一页无法上翻
	*/
	int pageUp();

	/**
	*  向下翻页
	*  @return int          ：[1,MAX_INT) 当前页数
	*						：NO_PAGE_TO_TURN 当前书本已经在最后一页无法下翻
	*/
	int pageDown(bool isByHand);

	/**
	*  退出当前书籍
	*/
	void bookQuit();

	/**
	*  页分享
	*/
	void pageShare();
    
    //get书籍路径
    string getBookPath();
    //get平台类型
    string getPlatform();
    
    //获取书籍按钮接口
    Menu *getPageMenu();
    void setPageMenu(Menu *pageMenu);
    void setPageMenuChangeCallBack(const PageMenuChangeCallBack &pageMenuChangeCallBack);
    PageMenuChangeCallBack getPageMenuChangeCallBack(){ return _pageMenuChangeCallBack; };
    void runPageMenuChangeCallBack();
    
    //设置按钮调用接口
	void setPageUpCallBack(const PageCallBack &pageUpCallBack);
	void setPageDownCallBack(const PageCallBack &pageDownCallBack);
	void setPageQuitCallBack(const PageCallBack &pageQuitCallBack);
	PageCallBack getPageUpCallBack(){ return _pageUpCallBack; };
	PageCallBack getPageDownCallBack(){ return _pageDownCallBack; };
	PageCallBack getPageQuitCallBack(){ return _pageQuitCallBack; };
    void runPageUpCallBack();
    void runPageDownCallBack();
    void runPageQuitCallBack();
    
    //设置分享接口
	void setBookShareCallBack(const ShareCallBack& pageShareCallBack);
	ShareCallBack getBookShareCallBack(){ return _pageShareCallBack; };
    void runBookShareCallBack();

	//当前书本播放结束回调接口
	void setBookEndCallBack(const PageCallBack &bookEndCallBack);
	PageCallBack getBookEndCallBack(){ return _bookEndCallBack; };

	//当前书本播放模式改变回调接口
	void setPlayModeChangeCallBack(const PlayModeChangeCallBack &playModeChangeCallBack);

    //画板保存路径
    void setDrawFilePath(string filePath);
    string getDrawFilePath();
    
    //是否是reading状态，默认为false
    void setIsReading(bool isReading);
    bool getIsReading();
    
    //获取素材和坐标比例系数
    float getResourcesScale();
    float getCoordinateScale();
    
    //获取书籍截图,仅能够在书籍中调用,其它场景返回空,请勿直接调用
    RenderTexture*  pageScreenShot();
    
    //获取解析器版本号
    string getVersion();
    
    //比较屏幕宽高比和设计宽高比，计算坐标偏移量
    Vec2 computeWinSizeOffset();

	//设置/获取资源文件的大小
    void setResourceSize(const cocos2d::Size& resourceSize);
	cocos2d::Size getResourceSize() const;

private:
	/**
	*  保存绘画页
	*/
	void drawLayerSavePNGFile();
	/**
	*  显示当前页
	*/
	void doPageTurn(bool backwards, bool isByHand);
    
private:
    float                       _fResourcesScale;
    float                       _fCoordinateScale;
    bool                        _isReading;
    string                      _sBookPath;
    string                      _sPlatform;
    string                      _sDrawFilePath;
    BookData                    _bookData;
    int                         _iCurrentPage;
    static BookParser*          _bookParser;
    PlayModeState               _playMode;
    Menu*                       _pageMenu;
 
	PageCallBack				_pageUpCallBack;
	PageCallBack				_pageDownCallBack;
	PageCallBack				_pageQuitCallBack;
	ShareCallBack				_pageShareCallBack;

	PageCallBack				_bookEndCallBack;
	PlayModeChangeCallBack		_playModeChangeCallBack;

    PageMenuChangeCallBack      _pageMenuChangeCallBack;
	bool						_isPaused;//当前书本是否处于暂停状态
	cocos2d::Size				_resourceSize;
};

NS_FK_END


#endif /* __BOOK_PARSER_H__ */
