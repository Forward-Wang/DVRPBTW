#ifndef _EVENTELEMENT_H
#define _EVENTELEMENT_H

#include<string>
#include<iostream>
#include "Customer.h"
using namespace std;

enum EventType{newCustomer, carArrived, finishedService, carDepature, newTimeSlot, carOffWork};
// �¼�  
// 1.�¹˿͵���
// 2.��������Ŀ�ĵ�
// 3.������ɷ���
// 4.��������
// 5.��ʱ��ε�����2-4����¼���

struct EventElement{
	float time;   // �¼�������ʱ��
	EventType eventType;  
	int carIndex;      // �¼������ĳ������
	int customerId;    // �¼������Ĺ˿ͽڵ�id
	EventElement(float time, EventType eventType, int carIndex, int customerId){ // ���캯��
		this->time = time;
		this->eventType = eventType;
		this->carIndex = carIndex;
		this->customerId = customerId;
	}  
	EventElement(EventElement &item){  // ���ƹ��캯��
		this->time = item.time;
		this->eventType = item.eventType;
		this->carIndex = item.carIndex;
		this->customerId = item.customerId;
	}
	EventElement& operator= (EventElement& item) {
		this->time = item.time;
		this->carIndex = item.carIndex;
		this->eventType = item.eventType;
		this->customerId = item.customerId;
		return *this;
	}
};

#endif