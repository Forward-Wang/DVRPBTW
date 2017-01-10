#include "Dispatcher.h"

Dispatcher::Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, int samplingRate):
	dynamicCustomerSet(dynamicCustomerSet), samplingRate(samplingRate){
	// ���캯��
	// �ڴ˴�Ӧ�����ɳ�ʼ�ƻ���
	mustServedCustomerSet.resize(staticCustomerSet.size());
	copy(staticCustomerSet.begin(), staticCustomerSet.end(), mustServedCustomerSet.begin());  // ��ʼ��֪�Ĺ˿Ͷ���must served����
	mayServedCustomerSet.resize(0);    // ��ʼ��may served���͵ļ���Ϊ��
	currentPlan.resize(0);
}

void Dispatcher::setSamplingRate(int samplingRate){   // ���ò�����
	this->samplingRate = samplingRate;
}


