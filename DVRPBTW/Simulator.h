#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "Car.h"
#include "Customer.h"
#include<vector>

class Simulator{  // ��������
public:
	Simulator(int samplingRate, int timeSlotLen, int slotIndex, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
		vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan); // ���캯��
	~Simulator();  // �������� 
	void clearCustomerSet();  // ���Simulator�еĹ˿ͼ�
	void clearCarSet();       // ���Simulator�еĳ�������
	vector<Car*>& initialPlan();     // ���ò����ƶ���ʼ�ƻ�
	vector<Car*> replan(vector<Customer*> newServedCustomer, vector<Customer*> newAbandonedCustomer, vector<Customer*> delayCustomer);
	vector<Customer*> generateScenario();  // ������̬�˿͵��龰
private:
	int samplingRate;   // ������
	int timeSlotLen;    // ʱ��γ���
	int slotIndex;      // ��ǰ�ǵڼ���slot
	vector<Customer*> promiseCustomerSet;     // �������Ĺ˿ͣ���һ��Ҫ�ڼƻ����У�
	vector<Customer*> waitCustomerSet;        // ����Ҫ����Ĺ˿ͣ����Բ������ڼƻ����У����Ǳ�����tolerantTime֮ǰ����ظ��Ƿ��ܷ���   
	vector<Customer*> dynamicCustomerSet;     // δ֪�Ĺ˿ͼ�
	vector<Car*> currentPlan;   
	// ��ǰ�ƻ�
	// �ڳ�ʼ����ʱ�򣬲�û�е�ǰ�ƻ�����ʱcurrentPlan������һ���ճ���������������ȡcapacity, depot��Ϣ
};

#endif