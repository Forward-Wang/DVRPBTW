#ifndef _Customer_H
#define _Customer_H

#include<vector>

// �˿ͽڵ���

struct Customer{
	int id;
	char type;     // ���ͣ���D����ʾ�ֿ⣬��P����ʾȡ����
	float x;       // x����
	float y;       // y����  
	float startTime;      // ʱ�䴰��ʼʱ��
	float endTime;        // ʱ�䴰����ʱ��
	float quantity;       // ���������� 
	float serviceTime;    // ����ʱ��
	float arrivedTime;    // ��������ʱ��
	float tolerantTime;   // ����������õ��𸴵�ʱ�䣨�����ڶ�̬BHs��
	Customer *next;       // ָ����һ��node�ڵ��ָ��
	Customer *front;      // ָ��ǰһ��node�ڵ��ָ��
	vector<float> timeProb; // �ڸ���timeSlot�µķֲ�����
	bool operator< (Customer &item){
		return this->id < item.id;
	}
	//Customer operator= (Customer &item){
	//	id = item.id;
	//	type = item.type;
	//	x = item.x;
	//	y = item.y;
	//	startTime = item.startTime;
	//	endTime = item.endTime;
	//	quantity = item.quantity;
	//	serviceTime = item.serviceTime;
	//	arrivedTime = item.arrivedTime;
	//	next = item.next;
	//	front = item.front;
	//	return *this;
	//}
};

#endif