#include "Simulator.h"
#include "ALNS.h"
#include "Matrix.h"
#include "PublicFunction.h"
#include "SSALNS.h"
#include<algorithm>
#include "TxtRecorder.h"

Simulator::Simulator(int samplingRate, int timeSlotLen, int timeSlotNum, int slotIndex, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
					 vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan):samplingRate(samplingRate), timeSlotLen(timeSlotLen), 
					 timeSlotNum(timeSlotNum), slotIndex(slotIndex){ // ���캯��
	vector<Customer*>::iterator custIter;
	this->promiseCustomerSet.reserve(promiseCustomerSet.end() - promiseCustomerSet.begin());
	this->promiseCustomerSet = copyCustomerSet(promiseCustomerSet);
	
	this->waitCustomerSet.reserve(waitCustomerSet.end() - waitCustomerSet.begin());
	this->waitCustomerSet = copyCustomerSet(waitCustomerSet);

	this->dynamicCustomerSet.reserve(dynamicCustomerSet.end() - dynamicCustomerSet.begin());
	this->dynamicCustomerSet = copyCustomerSet(dynamicCustomerSet);
	
	this->currentPlan.reserve(currentPlan.size());
	this->currentPlan = copyPlan(currentPlan);
}

Simulator::~Simulator(){  // ��������
	//clearCarSet();
	//clearCustomerSet();
}

void clearPlanSet(vector<vector<Car*>> planSet) {
	// ���planSet
	vector<vector<Car*>>::iterator iter;
	for(iter = planSet.begin(); iter < planSet.end(); iter++) {
		withdrawPlan((*iter));
	}
}

vector<Customer*> Simulator::generateScenario(){
	// �����龰
	// ���ݶ�̬�˿͵������Ϣ������ʱ�䴰
	// ע�⶯̬�˿�ֻ���ܳ�����slotIndex֮��
	vector<Customer*> tempCustomer = copyCustomerSet(dynamicCustomerSet);
	vector<Customer*>::iterator iter = tempCustomer.begin();
	for(iter; iter<tempCustomer.end(); iter++){
		float randFloat = random(0,1);  // ���������ѡ��˿Ϳ�����������ʱ��
		float sumation = 0;
		int count = roulette((*iter)->timeProb + slotIndex, timeSlotNum - slotIndex);  // ʱ��μ���
		float t1 = (count+slotIndex) * timeSlotLen;
		float t2 = (count+slotIndex+1) * timeSlotLen;
		float tempt = random(t1, t2);
		float maxTime = timeSlotNum * timeSlotLen;  // �������ʱ��
		(*iter)->startTime = min(tempt, maxTime - 2*(*iter)->serviceTime);  
		float timeWindowLen = random(2 * (*iter)->serviceTime, maxTime - (*iter)->startTime - (*iter)->serviceTime);  // ʱ�䴰����
		(*iter)->endTime = (*iter)->startTime + timeWindowLen;
	}
	return tempCustomer;
}

vector<int> getID(vector<Customer*> customerSet){ // �õ�customerSet������Id
	vector<int> ids(0);
	ids.reserve(customerSet.end() - customerSet.begin());
	vector<Customer*>::iterator iter = customerSet.begin();
	for(iter; iter<customerSet.end(); iter++){
		ids.push_back((*iter)->id);
	}
	return ids;
}

vector<Car*> Simulator::initialPlan(Customer depot, float capacity){     // ���ò����ƶ���ʼ�ƻ�
	ostringstream ostr;
	vector<int>::iterator intIter;
	vector<Car*>::iterator carIter;
	int i,j;
	sort(promiseCustomerSet.begin(), promiseCustomerSet.end());  // �˿ͼ�����id��С��������
	vector<int> validId;
	validId.push_back(0);   // ��һ���ڵ��ǲֿ�ڵ�
	vector<int> tempId = getID(promiseCustomerSet);    // �����ڼƻ���ʼǰ��֪�Ĺ˿�id(���ڱ������Ĺ˿�)
	for(intIter = tempId.begin(); intIter < tempId.end(); intIter++) {
		validId.push_back(*intIter);
	}
	// ��ʼ��transformMatrix
	Matrix<int> transformMatrix(validId.size(), validId.size());
	for(i=0; i<validId.size(); i++) {
		for(j=0; j<validId.size(); j++) {
			transformMatrix.setValue(i,j,0);
		}
	}
	vector<vector<Car*>> planSet(samplingRate);   // ���в����õ��ļƻ�
	vector<vector<Car*>>::iterator planIter = planSet.begin();
	// �����е��龰����ALNS�㷨�����Ұѽ����planSet��
	// �ڴ˹����н�����validId�����еĽ⣬������id��validId�еĹ˿ͽڵ�
	vector<Customer*>::iterator iter;
	ostr.str("");
	ostr << "----Sampling begins!" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	for(planIter; planIter<planSet.end(); planIter++) {
		vector<Customer*> allCustomer = copyCustomerSet(promiseCustomerSet);   // ����ִ��ALNS�㷨�����й˿ͽڵ�
		vector<Customer*> currentDynamicCust = generateScenario();  // ����
		iter = currentDynamicCust.begin();
		for(iter; iter<currentDynamicCust.end(); iter++){
			allCustomer.push_back(*iter);
		}
		int num = planIter - planSet.begin();
		ostr.str("");
		ostr << "Now is the " << num+1 << "th simulation" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		ALNS alg(allCustomer, depot, capacity);
		vector<Car*> solution(0);
		float cost = 0;
		alg.run(solution, cost);
		for(carIter = solution.begin(); carIter < solution.end(); carIter++) {
			(*carIter)->removeInvalidCustomer(validId);
			(*carIter)->updateTranformMatrix(transformMatrix);
		}
		*planIter = solution;
		deleteCustomerSet(allCustomer);   
	}

	// Ȼ��������龰�µļƻ��������֣�ȡ�÷��������Ϊ��ʼ·���ƻ�
	ostr.str("");
	ostr << "----Now assessing the performance of each scenario" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	vector<pair<int, int>> scoreForPlan(samplingRate);    // ÿ���ƻ��ĵ÷�
	for(planIter = planSet.begin(); planIter < planSet.end(); planIter++) {
		int pos = planIter - planSet.begin();             // �ڲ����õ��ļƻ��е�λ��
		int score = 0;
		for(carIter = planIter->begin(); carIter < planIter->end(); carIter++) {
			score += (*carIter)->computeScore(transformMatrix);
		}
		scoreForPlan.push_back(make_pair(score, pos));
	}
	sort(scoreForPlan.begin(), scoreForPlan.end(), descendSort<int, int>);
	vector<Car*> outputPlan = copyPlan(planSet[scoreForPlan[0].second]);
	clearPlanSet(planSet);
	deleteCustomerSet(waitCustomerSet);
	deleteCustomerSet(promiseCustomerSet);
	withdrawPlan(currentPlan);
	return outputPlan;
}



bool Simulator::checkFeasible(vector<Car*> carSet){
	// �ж�promiseCustomerSet�еĹ˿��Ƿ���carSet��
	vector<int> tempId = getID(promiseCustomerSet);  // �õ���promise�Ĺ˿�id
	sort(tempId.begin(), tempId.end());
	vector<Car*>::iterator carIter;
	for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
		vector<Customer*> tempCust = (*carIter)->getRoute().getAllCustomer();
		vector<Customer*>::iterator custIter;
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			vector<int>::iterator intIter = find(tempId.begin(), tempId.end(), (*custIter)->id);
			if(intIter < tempId.end()) {
				// ����ҵ��ˣ���ɾ��
				tempId.erase(intIter);
			}
		}
	}
	if(tempId.size() != 0) {
		return false;
	} else {
		return true;
	}
}

void validPromise(vector<Car*>Plan, vector<Customer*> hurryCustomer, vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId){
	// ��hurry customerȷ��promise
	vector<Car*>::iterator carIter;
	vector<Customer*>::iterator custIter;
	vector<int> hurryCustomerId = getID(hurryCustomer);   // hurry customer��id
	int i;
	for(carIter = Plan.begin(); carIter < Plan.end(); carIter++){
		vector<Customer*> tempCust = (*carIter)->getAllCustomer();
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			int tempId = (*custIter)->id;
			vector<int>::iterator tempIter = find(hurryCustomerId.begin(), hurryCustomerId.end(), tempId);
			if(tempIter < hurryCustomerId.end()) {
				// ���tempId��hurryCustomerId��
				newServedCustomerId.push_back(tempId);
				hurryCustomerId.erase(tempIter);  // ɾ����������Ĺ˿�ʣ��δ������Ĺ˿�
			}
		}
	}
	sort(hurryCustomerId.begin(), hurryCustomerId.end());
	// �õ������Ĺ˿�id
	vector<int>::iterator intIter;
	for(intIter = hurryCustomerId.begin(); intIter < hurryCustomerId.end(); intIter++) {
		newAbandonedCustomerId.push_back(*intIter);
	}
}

vector<Car*> Simulator::replan(vector<int> &newServedCustomerId, vector<int> &newAbandonedCustomerId, vector<int> &delayCustomerId, float capacity) {
	// ���¼ƻ�������vehicle�����Ժ�
	// ����ɸѡ��һЩ�ż��ȴ��ظ���waitCustomer�Լ�һЩ���ż��ظ���
	// newServedCustomer:  (wait customer��)ͨ��re-plan���ܵ�����Ĺ˿�
	// newAbandonedCustomer: (wait customer��)ͨ��re-planȷ���޷����ܷ���Ĺ˿�
	// delayCustomer: ����patient customer�������ǰ����ȷ�Ϸ��������δ����Ϊ�䰲��
	ostringstream ostr;
	vector<Customer*> hurryCustomer;
	vector<Customer*> patientCustomer;
	vector<Customer*>::iterator custIter;
	float nextMoment = (slotIndex+1) * timeSlotLen; // ��һ��ʱ��ε���ֹʱ�䣨���¸�ʱ��εĿ�ʼʱ�䣩
	for(custIter = waitCustomerSet.begin(); custIter < waitCustomerSet.end(); custIter) {
		if((*custIter)->tolerantTime <= nextMoment) {  // �ù˿��ż�����ʱ���ǰ�õ��ظ�
			Customer *tempCust = new Customer(**custIter);
			hurryCustomer.push_back(tempCust);     // ����hurryCustomer��
		} else {
			Customer *tempCust = new Customer(**custIter);
			patientCustomer.push_back(tempCust);
		}
	}
	// Ȼ����Щ�˿;����ܵط��뵱ǰ�ƻ���
	// �����ܣ��򷵻�'No' promise
	vector<Car*> newPlan;
	vector<Car*>::iterator carIter;
	float finalCost = 0;
	if(hurryCustomer.size() == 0) {  
		// ���û��hurryCustomer����ô����Ҫ�������replan
		ostr.str("");
		ostr << "There are no hurry Customer" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		if(patientCustomer.size() != 0) {
			ostr.str("");
			ostr << "Replan for patient customer..." << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			// ���patientCustomerΪ�գ�����Ҫ�������replan
			SSALNS SSANLS_alg2(patientCustomer, currentPlan, capacity);
			SSANLS_alg2.run(newPlan, finalCost);
			validPromise(newPlan, patientCustomer, newServedCustomerId, delayCustomerId);
		} else {
			// �������customerSet��Ϊ�գ���ֱ�Ӹ���currentPlan��newPlan
			ostr.str("");
			ostr << "There are no patient customer" << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			newPlan = copyPlan(currentPlan);
		}
	} else {
		// ��hurryCustomer����replan
		ostr.str("");
		ostr << "Replan for hurry customer..." << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		SSALNS SSALNS_alg1(hurryCustomer, currentPlan, capacity);
		SSALNS_alg1.run(newPlan, finalCost);
		validPromise(newPlan, hurryCustomer, newServedCustomerId, newAbandonedCustomerId);
		if(patientCustomer.size() != 0) {
			ostr.str("");
			ostr << "Replan for patient customer..." << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
			// ���patientCustomer��Ϊ�գ���Ӧ�ö������replan
			SSALNS SSANLS_alg2(patientCustomer, newPlan, capacity);
			SSANLS_alg2.run(newPlan, finalCost);
			validPromise(newPlan, patientCustomer, newServedCustomerId, delayCustomerId);		
		} else {
			ostr.str("");
			ostr << "There are no patient customer" << endl;
			TxtRecorder::addLine(ostr.str());
			cout << ostr.str();
		}
	}

	deleteCustomerSet(hurryCustomer);
	deleteCustomerSet(patientCustomer);
	
	// ���ȵõ�"OK promise"�Ĺ˿͵�id������������־���
	vector<int> allServedCustomerId;
	allServedCustomerId.push_back(0);   // �ֿ�ڵ������־����еĵ�һ���ڵ�
	vector<int>::iterator intIter;
	vector<int> promiseCustomerId = getID(promiseCustomerSet);
	for(intIter = promiseCustomerId.begin(); intIter < promiseCustomerId.end(); intIter++) {
		allServedCustomerId.push_back(*intIter);
	}
	for(intIter = newServedCustomerId.begin(); intIter < newServedCustomerId.end(); intIter++) {
		allServedCustomerId.push_back(*intIter);
	}
	sort(allServedCustomerId.begin(), allServedCustomerId.end());
	// Ȼ����в���������SSALNS�㷨������������龰�µļƻ�
	// ���Ҽ������־���
	// ��ʼ��transformMatrix
	Matrix<int> transformMatrix(allServedCustomerId.size(), allServedCustomerId.size());
	for(int i=0; i<allServedCustomerId.size(); i++) {
		for(int j=0; j<allServedCustomerId.size(); j++) {
			transformMatrix.setValue(i,j,0);
		}
	}
	vector<vector<Car*>> planSet(samplingRate);
	vector<vector<Car*>>::iterator planIter;
	ostr.str("");
	ostr << "----Sampling begins!" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	for(planIter = planSet.begin(); planIter < planSet.end(); planIter++) {
		vector<Car*> tempPlan;
		vector<Customer*>sampleCustomer = generateScenario(); // ������̬����˿͵��龰
		int num = planIter - planSet.begin();
		ostr.str("");
		ostr << "Now is the " << num+1 << "th simulation" << endl;
		TxtRecorder::addLine(ostr.str());
		cout << ostr.str();
		SSALNS SSALNS_alg3(sampleCustomer, newPlan, capacity);
		SSALNS_alg3.run(tempPlan, finalCost);
		for(carIter = tempPlan.begin(); carIter < tempPlan.end(); carIter++) {
			(*carIter)->removeInvalidCustomer(allServedCustomerId);
			(*carIter)->updateTranformMatrix(transformMatrix);
		}
		deleteCustomerSet(sampleCustomer);
	}

	// ȡ������ߵļƻ���Ϊ���
	ostr.str("");
	ostr << "----Now assessing the performance of each scenario" << endl;
	TxtRecorder::addLine(ostr.str());
	cout << ostr.str();
	vector<pair<int, int>> scoreForPlan(samplingRate);    // ÿ���ƻ��ĵ÷�
	for(planIter = planSet.begin(); planIter < planSet.end(); planIter++) {
		int pos = planIter - planSet.begin();             // �ڲ����õ��ļƻ��е�λ��
		int score = 0;
		for(carIter = planIter->begin(); carIter < planIter->end(); carIter++) {
			score += (*carIter)->computeScore(transformMatrix);
		}
		scoreForPlan.push_back(make_pair(score, pos));
	}
	sort(scoreForPlan.begin(), scoreForPlan.end(), descendSort<int, int>);
	vector<Car*> outputPlan = copyPlan(planSet[scoreForPlan[0].second]);
	clearPlanSet(planSet);
	deleteCustomerSet(waitCustomerSet);
	deleteCustomerSet(promiseCustomerSet);
	withdrawPlan(currentPlan);
	return outputPlan;
}