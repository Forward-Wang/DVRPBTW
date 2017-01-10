#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "Car.h"
#include "Customer.h"
#include<vector>

class Simulator{  // ��������
public:
	Simulator(int samplingRate, int timeSlotLen, Customer depot, float capacity); // ���캯��
	~Simulator();  // ��������
	void updateCustomerSet(vector<Customer*> mustServedCustomerSet, vector<Customer*> mayServedCustomerSet, vector<Customer*> dynamicCustomerSet);
	void clearCustomerSet();    // ��ձ��ع˿ͼ�
	void clearCarSet();         // ��ձ��ػ�������
	void updateCars(vector<Car*>newCars);         // ���³���
	vector<Customer*> generateScenario();         // �����龰
	void updateSamplingRate(int newSamplingRate); // ���²�����
	vector<Car*>& initialPlan();     // ���ò����ƶ���ʼ�ƻ�
	vector<Car*> run();   
	// ���з��������ڴ˴�����Algorithm��
private:
	int samplingRate;   // ������
	int timeSlotLen;    // ʱ��γ���
	Customer depot;     // �ֿ�ڵ�
	float capacity;     // ������
	vector<Customer*> mustServedCustomerSet;  // �������Ĺ˿ͣ���һ��Ҫ�ڼƻ����У�
	vector<Customer*> mayServedCustomerSet;   // ����Ҫ����Ĺ˿ͣ����Բ������ڼƻ����У����Ǳ�����tolerantTime֮ǰ����ظ��Ƿ��ܷ���   
	vector<Customer*> dynamicCustomerSet;     // δ֪�Ĺ˿ͼ�
	vector<Car*> carSet;   // ���г���
};

#endif