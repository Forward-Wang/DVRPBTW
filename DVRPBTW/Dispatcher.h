#ifndef _DISPATCHER_H
#define _DISPATHCER_H

#include "EventList.h"
#include "Car.h"
#include "ProbInfo.h"
#include<vector>

class Dispatcher{   // ����������
public:
	Dispatcher(vector<Customer*> initKnownCustomer, ProbInfo);  // ���캯��
	~Dispatcher(); // ��������
	EventList& handleNewCustomer(float time, Customer& newCustomer);  // �����¹˿͵���
	EventList& handleCarArrived(float time, int carIndex);   // ������������¼�
	EventList& handleFinishedService(float time, int carIndex);  // ���������ɷ����¼�
	EventList& handleDepature(float time, int carIndex);  // ������������¼�
	EventList& handleNewTimeSlot(float time, vector<Customer*> newKnownCustomers); // ��ʱ��ο�ʼ 
	Car& searchCar(int carIndex);   // ���ݻ��������������
	void addKnownCustomer(vector<Customer*> newKnownCustomers); 
	// ������֪����Ĺ˿ͼ����ڴ˴�����Simulator��
	void setSamplingRate(int samplingRate);   // ���ò�����
private:
	vector<Customer*> knownCustomer;  // ��ǰ������֪�Ĺ˿�
	ProbInfo probInfo;                // ���й˿͵���ʷ��Ϣ 
	int samplingRate;    // ������
};

#endif