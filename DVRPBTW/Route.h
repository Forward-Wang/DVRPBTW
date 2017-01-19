#ifndef _ROUTE_H
#define _ROUTE_H

#include<iostream>
#include<vector>
#include "Customer.h"

using namespace std;


class Route{
public:
	Route(Customer &headNode, Customer &rearNode, float capacity);      // ���캯��
	~Route();   // ��������
	Route(const Route &L);  // ���ƹ��캯��
	Route& operator= (const Route &L);  // ���ظ�ֵ����� 
	Customer& operator[] (int k);     // �õ������еĵ�k��Ԫ��
	const Customer& operator[] (int k) const; // ��̬����
	bool isEmpty();   // �ж������Ƿ�Ϊ��
	bool insertAfter(Customer &item1, Customer &item2); // ����������item1��ͬ�Ľڵ�������ڵ�item2
	void insertAtHead(Customer &item);   // �ڱ�ͷ����item
	void insertAtRear(Customer &item);    // �ڱ�β����item
	bool deleteNode(Customer &item);   // ��������ɾ����item��ͬ�Ľڵ�
	bool next();  // currentָ����ǰ�ߣ�������false����ʾ�Ѿ��ߵ�ͷ
	void printRoute();   // ��ӡ·��
	void clear();     // �������
	void assignCar(int carIndex); // Ϊroute���䳵��
	void replaceRoute(const Route &route);  // ��route�滻��currentָ����·��
	Route& capture();  // ץȡcurrentָ����·��
	Customer currentPos();   // ���ص�ǰλ��
	Customer nextPos() {return *(current->next);}      // ��һ���ڵ��λ��
	bool moveForward();      // ��ǰ��
	int getSize();     // �õ���ǰ����Ĵ�С
	vector<Customer*> getAllCustomer();  // �õ�·�������еĹ˿ͽڵ�
	vector<float> computeReducedCost(float DTpara[], bool artificial = false);  // �������нڵ���Ƴ�����
	void computeInsertCost(Customer item, float &minValue, Customer &customer1, float &secondValue, Customer &customer2, 
		float noiseAmount = 0.0f, bool noiseAdd = false, float penaltyPara = 0.0f, bool adativeNoise = false);  
	// ����item�ڵ���·���е���С������ۺʹ�С�������
	// ���������/�μѲ����ǰ��Ĺ˿ͽڵ�
	// penaltyParaΪ�ͷ�ϵ����������Ҫ�ͷ���penaltyPara = 0
	bool timeWindowJudge(Customer *pre, int pos, Customer item);
	// �����item���뵽pre�����Ƿ��Υ��ʱ�䴰Լ��
	void refreshArrivedTime();   // ����һ�¸����ڵ�ĵ���ʱ��
	void changeCarIndex(int newIndex);  // ���³������
	float getLen(float DTpara[], bool artificial = false);   // �õ�·������
	Customer getHeadNode();    // �õ�ͷ���
	Customer getRearNode();    // �õ�β�ڵ�
	vector<float> getArrivedTime();     // �õ��������нڵ��arrivedTime��ע���һ��Ԫ����0
	float getQuantity() {return this->quantity;}       // �õ�������ʹ�õ�����(װ�ع˿�)
	float getLeftQuantity() {return this->leftQuantity;}    // �õ�����ʣ���������복����ʻ�����أ�
	float getCapacity() {return this->capacity;}       // ���ر����ĳ�����
	void decreaseLeftQuantity(float amount) {leftQuantity -= amount;}   // ����ʣ��ĳ��������������µĹ˿ͣ�
private:
	Customer *head, *current, *rear;  // ��ͷ����β�͵�ǰָ�룬��ǰָ��ָ�������ǰ��פ��
	int size;         // ����ĳ���
	int carIndex;     // ��������
	float quantity;   // ��ǰ��ʹ�õ�����(��װ�صĹ˿������й�)
	float leftQuantity;   // ʣ���������복����ʻ����йأ�
	float capacity;       // ��������������Ҳ��һ��
	void copy(const Route& L);  // �������������ƹ��캯�������ء�=������ʹ��
	vector<float> arrivedTime;  // �ڸ����ڵ�ĵ���ʱ��
};


#endif