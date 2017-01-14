#include "Dispatcher.h"
#include<algorithm>

bool ascendSortForCustId(Customer* item1, Customer* item2) {
	return item1->id < item2->id;
}

Dispatcher::Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, int samplingRate){
	int custNum = staticCustomerSet.end() - staticCustomerSet.begin() + dynamicCustomerSet.end() - dynamicCustomerSet.begin(); // �ܹ˿���
	ServedCustomerId.reserve(custNum);     // �Ѿ�������Ĺ˿�id
	promisedCustomerId.reserve(custNum);
	waitCustomerId.reserve(custNum);
	rejectCustomerId.reserve(custNum);
	vector<Customer*>::iterator custIter = staticCustomerSet.begin();
	for(custIter; custIter < staticCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		allCustomer.push_back(newCust);
		promisedCustomerId.push_back(newCust->id);
	}
	for(custIter = dynamicCustomerSet.begin(); custIter < dynamicCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		allCustomer.push_back(newCust);		
	}
	sort(allCustomer.begin(), allCustomer.end(), ascendSortForCustId);  // ��id���е�������
}

void Dispatcher::setSamplingRate(int samplingRate){   // ���ò�����
	this->samplingRate = samplingRate;
}


