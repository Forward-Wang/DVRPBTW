#ifndef _SIMULATOR_H
#define _SIMULATOR_H
#include "Car.h"
#include "Customer.h"
#include "ProbInfo.h"
#include<vector>

class Simulator{  // ��������
public:
	Simulator(int samplingRate, vector<Customer*> unservedCustomers, vector<Car*> cars, ProbInfo probinfo); // ���캯��
	~Simulator();  // ��������
	void updateUnservedCustomer(vector<Customer*> newUnservedCustomers); // ����δ����Ĺ˿�  
	void updateSamplingRate(int newSamplingRate);     // ���²�����
	void updateCars(vector<Car*>newCars);  // ���³���
	vector<Route*> run();   
	// ���з��������ڴ˴�����Algorithm��
private:
	int samplingRate;   // ������
	vector<Customer*> unservedCustomers;  // δ����Ĺ˿�
	vector<Car*> Cars;   // ���г���
	ProbInfo probInfo;   // �˿���ʷ��Ϣ
};

#endif