#ifndef __Parent_SCENE_H__
#define __Parent_SCENE_H__

#include "cocos2d.h"
#include "IndexScene.h"
#include "FileNames.h"
#include "App.h"
//#include "SqliteManager.h"
#include "DropBox.h"
//#include "DownloadRes.h"
#include "RechargeLayer.h"

class Parent : public cocos2d::Layer
{
private:
	Node* m_show;//��¼��ǰ��ʾ��ͼ��
	Node* m_parentNode;//��������
	int m_times;//��ȡ��֤���ʱ
	map<string,void*> m_voids;//��ָ��
public:
	~Parent();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

	void onEnterTransitionDidFinish();
	// implement the "static create()" method manually
    CREATE_FUNC(Parent);	

	//���ý���ĳ�ʼ��
	Layer* initNode_Set();

	//��ϵ���ǽ���ĳ�ʼ��
	Layer* initNode_Contact();

	//�˺���Ϣ�����ʼ��
	Layer* initNode_Account();

	//��½�����ʼ��
	Layer* initNode_Login();

	//Ԥ����
	//static void prestrain();

	//��ȡ���ػ����С
	float getLocalCacheSize();

	//��ȡ�ļ��д�С
	long getDirectorySize(string);

	//�������
	void deleteCache();

	//ɾ�����طǿ�Ŀ¼
	static void DeleteDirectory(string );

	//�������
	void FeedBack(string,string);

	//���������޸��û���Ϣ
	//void updateInfo(int, string, string);
	//void updateInfoCallback(string, int, string, string);

	//֪ͨ�޸��û���Ϣ�ɹ�
	//void notifyModifySuccess();

	//֪ͨ�޸��û���Ϣʧ��
	//void notifyModifyFailed();

	//��ȡ��֤��
	void getCode(string);

	//���������޸�����
	void modifyPassword(string, string);

	//���������˻����
	void getUserMoney();

	//չʾ���
	void showRedPacket();

	//��ȡ��Ч����Ļص�
	//void httpGetRedpacketCallback(string json);

	void listViewShowRedPacket(ListView * listview);

	void loadRedPacketData(Node * node, int index);
	//��ȡ��Ч���
	void httpGetUserRedPacket();

	//void stringHttpRequest(HttpRequest::Type type, string url, map<string, string> paramter, int outTime, std::function<void(string json)> runable, std::function<void()> errorRunable);
};

#endif // __Parent_SCENE_H__
