#ifndef _Customer_H
#define _Customer_H

// �˿ͽڵ���

struct Customer{
	int id;
	char type;     // ���ͣ���D����ʾ�ֿ⣬��L����ʾ�ͻ��㣬��B����ʾȡ����
	float x;       // x����
	float y;       // y����  
	float startTime;   // ʱ�䴰��ʼʱ��
	float endTime;     // ʱ�䴰����ʱ��
	float quantity;    // ���������� 
	float serviceTime; // ����ʱ��
	float arrivedTime; // ��������ʱ��
	Customer *next;    // ָ����һ��node�ڵ��ָ��
	Customer *front;   // ָ��ǰһ��node�ڵ��ָ��
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