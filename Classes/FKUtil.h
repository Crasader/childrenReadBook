#ifndef __UTIL_H__
#define __UTIL_H__

using namespace std;

// 0x00 HI ME LO
// 00   02 06 00
#define BOOK_PARSER_VERSION "2.6"

#define DATA_SUCCESS 0
#define DATA_ERROR -1

#define BUTTON_TAG_PAGE_UP 10001
#define BUTTON_TAG_PAGE_DOWN 10002
#define BUTTON_TAG_PAGE_QUIT 10003
#define BUTTON_TAG_PAGE_SHARE 10004

#define NS_FK_BEGIN                 namespace FK {
#define NS_FK_END                   }
#define USING_NS_FK                 using namespace FK

#define ACTION_AUTO                 1
#define ACTION_TOUCH                2
#define ACTION_SWIPE                3

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define FK_DLL     __declspec(dllexport)
#else
#define FK_DLL
#endif
//书籍播放模式
enum PlayModeState {
	READ,                   //正常看书模式
    AUTOPLAY,               //自动播放模式
    LISTEN,                 //听书模式
	DEFULT = READ				
};

NS_FK_BEGIN

class FK_DLL Util
{
public:
    //字符分割处理
    static vector<string> split( string str, string pattern)
    {
        vector<string> ret;
        if(pattern.empty()) {
            return ret;
        }
        size_t start = 0,index = str.find_first_of(pattern,0);
        while(index!=str.npos)
        {
            if(start!=index)
            {
                ret.push_back(str.substr(start,index-start));
            }
            start = index + 1;
            index = str.find_first_of(pattern,start);
        }
        if(!str.substr(start).empty())
        {
            ret.push_back(str.substr(start));
        }
        return ret;
    }
    
};

NS_FK_END

//string转换各个类型
template <class Type>
Type stringTo(const string& str)
{
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}
template int    stringTo(const std::string& s);
template float  stringTo(const std::string& s);
template double stringTo(const std::string& s);

//各种类型转string
template <class Type>
string toString(const Type& type) {
    stringstream ss;
    string num;
    ss << type;
    ss >> num;
    return num;
}

template string toString(const int &i);
template string toString(const float &f);
template string toString(const double &d);

#endif // __UTIL_H__
