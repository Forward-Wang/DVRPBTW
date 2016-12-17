#ifndef _TIMER_H
#define _TIMER_H

#include<vector>
#include "EventList.h"

class Timer{  // �¼�������
public:
	Timer(vector<Customer*>, float* timeDivision, int size=0);   // ���캯�����������Ϊ���й˿ͣ��Լ���ʱ��ο�ʼֵ
	~Timer();  // ��������
	EventList& pop();   // ���¼����һ��Ԫ�ص���
	void addEventList(vector<EventList*> newEventList);  // ���¼����������¼�
	void deleteEventList(vector<string*> eventType);     // ɾ��eventType���͵��¼�
	void run();   // �������Ľ�����������Ҫ��ʼ��Dispatcher
private:
	vector<EventList*> eventList;  // �¼���
	int size;     // �¼���ĳ���
	
};

#endif