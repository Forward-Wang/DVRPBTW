#ifndef _DISPATCHER_H
#define _DISPATHCER_H

#include "EventElement.h"
#include "Car.h"
#include "ProbInfo.h"
#include<vector>

class Dispatcher{   // ����������
public:
	Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, int samplingRate = 30);  // ���캯��
	~Dispatcher(){}; // ��������
	EventElement& handleNewCustomer(float time, Customer& newCustomer);  // �����¹˿͵���
	EventElement& handleCarArrived(float time, int carIndex);            // ������������¼�
	EventElement& handleFinishedService(float time, int carIndex);       // ���������ɷ����¼�
	EventElement& handleDepature(float time, int carIndex);              // ������������¼�
	EventElement& handleNewTimeSlot(float time, vector<Customer*> newKnownCustomers); // ��ʱ��ο�ʼ 
	Car& searchCar(int carIndex);   // ���ݻ��������������
	void addKnownCustomer(vector<Customer*> newKnownCustomers); 
	// ������֪����Ĺ˿ͼ����ڴ˴�����Simulator��
	void setSamplingRate(int samplingRate);   // ���ò�����
private:
	vector<Customer*> allCustomer;    // ���еĹ˿�
	vector<int> ServedCustomerId;     // �Ѿ�������Ĺ˿�id
	vector<int> promisedCustomerId;   // (δ�������)�Ѿ��õ�'OK' promise�Ĺ˿�id
	vector<int> waitCustomerId;       // (δ�����������֪��)���ڵȴ�'OK' promise�Ĺ˿�id
	vector<int> rejectCustomerId;     // (δ�����������֪��)�õ���'NO' promise�Ĺ˿�id
	vector<Car*> currentPlan;                 // ��ǰ�ƻ�
	int samplingRate;    // ������
};

#endif