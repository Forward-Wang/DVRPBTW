#ifndef _EVENTLIST_H
#define _EVENTLIST_H

#include<string>
#include "Customer.h"
using namespace std;

struct EventList{
	float time;   // �¼�������ʱ��
	string eventType;  
	// �¼�  
	// 1.�¹˿͵���
	// 2.��������Ŀ�ĵ�
	// 3.������ɷ���
	// 4.��������
	// 5.��ʱ��ε�����2-4����¼���
	int carIndex;  // �¼������ĳ������
	Customer customer; // �¼������Ĺ˿ͽڵ�
}

#endif