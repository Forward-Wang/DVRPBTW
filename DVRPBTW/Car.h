#include "Route.h"
#include<vector>
#ifndef _CAR_H
#define _CAR_H

enum State{wait, depature, arrived, startservice};

class Car{
public:
	Car(Customer &headNode, Customer &rearNode, float capacity, int index);  // ���캯��
	~Car();  // ��������
	Car(Car &item);  //���ƹ��캯��
	Car& operator= (Car &item); // ���ظ�ֵ����
	void updateState(char newState);  // ����״̬
	char getState();          // �õ���ǰ��״̬
	void updateRoute(Route &L);       // ����currentָ����·��
	void moveForword();       // ��ǰ��
	void getLeftCapacity();   // �õ�ʣ��ĳ�����
	void getServedCustomers();  // �õ��Ѿ�������Ĺ˿͵㼯
	Route& getRoute();    // �õ�����·��
	void changeCarIndex(int newIndex);  // ���ĳ������
private:
	State state;    // ����״̬�������н����ȴ�����
	Route route;   // �ƻ�Ҫ�ߵ�·��
	int carIndex;     // �������
};

#endif