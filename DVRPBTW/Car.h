#include "Route.h"
#include<vector>
#ifndef _CAR_H
#define _CAR_H

enum State{wait, depature, arrived, serving, offwork};

class Car{
public:
	Car(Customer &headNode, Customer &rearNode, float capacity, int index, bool artificial = false);  // ���캯��
	~Car();          // ��������
	Car(Car &item);  //���ƹ��캯��
	Car& operator= (Car &item);       // ���ظ�ֵ����
	void updateState(char newState);  // ����״̬
	char getState();                  // �õ���ǰ��״̬
	void updateRoute(Route &L);       // ����currentָ����·��
	bool moveForward(){return route.moveForward();}               // ��ǰ��
	float getLeftQuantity() {return route.getLeftQuantity();}     // �õ�ʣ��ĳ�����
	void decreaseLeftQuantity(float amount) {route.decreaseLeftQuantity(amount);}  // ���ٻ�����ʣ������
	void getServedCustomers();  // �õ��Ѿ�������Ĺ˿͵㼯
	Route& getRoute();    // �õ�����·��
	void changeCarIndex(int newIndex);    // ���ĳ������
	int getCarIndex() {return carIndex;}  // �õ��������
	bool judgeArtificial() {return artificial;} // ���س�������
	float getDepatureTime() {return nearestDepatureTime;}  // ���ػ������µĳ���ʱ��
	void updateDepatureTime(float time) {nearestDepatureTime = time;}  // �������µĳ���ʱ��
	float getCapacity() {return route.getCapacity();}
private:
	State state;    // ����״̬�������н����ȴ�����
	Route route;    // �ƻ�Ҫ�ߵ�·��
	float nearestDepatureTime;   // ���µĳ���ʱ��
	bool artificial;  // Ϊtrue��ʾ���鹹�ĳ�����false��ʾ��ʵ�ĳ���
	int carIndex;     // �������
};

#endif