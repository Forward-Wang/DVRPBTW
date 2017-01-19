#include "Simulator.h"
#include "ALNS.h"
#include "Matrix.h"
#include "PublicFunction.h"
#include "SSALNS.h"
#include<algorithm>

Simulator::Simulator(int samplingRate, int timeSlotLen, int slotIndex, vector<Customer*> promiseCustomerSet, vector<Customer*> waitCustomerSet,
					 vector<Customer*> dynamicCustomerSet, vector<Car*> currentPlan):samplingRate(samplingRate), timeSlotLen(timeSlotLen), slotIndex(slotIndex){ // ���캯��
	vector<Customer*>::iterator custIter;
	this->promiseCustomerSet.reserve(promiseCustomerSet.end() - promiseCustomerSet.begin());
	for(custIter = promiseCustomerSet.begin(); custIter < promiseCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		this->promiseCustomerSet.push_back(newCust);
	}
	this->waitCustomerSet.reserve(waitCustomerSet.end() - waitCustomerSet.begin());
	for(custIter = waitCustomerSet.begin(); custIter < waitCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		this->waitCustomerSet.push_back(newCust);
	}
	this->dynamicCustomerSet.reserve(dynamicCustomerSet.end() - dynamicCustomerSet.begin());
	for(custIter = dynamicCustomerSet.begin(); custIter < dynamicCustomerSet.end(); custIter++) {
		Customer* newCust = new Customer(**custIter);
		this->dynamicCustomerSet.push_back(newCust);
	}
	vector<Car*>::iterator carIter;
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		Car* newCar = new Car(**carIter);
		this->currentPlan.push_back(newCar);
	}
}

Simulator::~Simulator(){  // ��������
	//clearCarSet();
	//clearCustomerSet();
}

void Simulator::clearCustomerSet(){    // ��ձ��ع˿ͼ�
	vector<Customer*>::iterator iter = promiseCustomerSet.begin();
	for(iter; iter < promiseCustomerSet.end(); iter++){  
		delete *iter;
	}
	iter = waitCustomerSet.begin();
	for(iter; iter < waitCustomerSet.end(); iter++){
		delete *iter;
	}
	iter = dynamicCustomerSet.begin();
	for(iter; iter < dynamicCustomerSet.end(); iter++){
		delete *iter;
	}
	promiseCustomerSet.resize(0);
	waitCustomerSet.resize(0);
	dynamicCustomerSet.resize(0);
}

void Simulator::clearCarSet(){  // ��ջ�������
	vector<Car*>::iterator iter = currentPlan.begin();
	for(iter; iter<currentPlan.end(); iter++) {
		delete *iter;
	}
	currentPlan.resize(0);
}

vector<Customer*> Simulator::generateScenario(){
	// �����龰
	// ���ݶ�̬�˿͵������Ϣ������ʱ�䴰
	vector<Customer*>::iterator iter = dynamicCustomerSet.begin();
	for(iter; iter<dynamicCustomerSet.end(); iter++){
		float randFloat = random(0,1);  // ���������ѡ��˿Ϳ�����������ʱ��
		float sumation = 0;
		vector<float>::iterator iter2 = (*iter)->timeProb.begin();
		int count = roulette((*iter)->timeProb);  // ʱ��μ���
		float t1 = count * timeSlotLen;
		float t2 = (count+1) * timeSlotLen;
		float tempt = random(t1, t2);
		float maxTime = (int)(*iter)->timeProb.size() * timeSlotLen;  // �������ʱ��
		(*iter)->startTime = floor(min(tempt, maxTime - 2*(*iter)->tolerantTime));  
		float timeWindowLen = floor(random(2 * (*iter)->serviceTime, maxTime - (*iter)->startTime));  // ʱ�䴰����
		(*iter)->endTime = (*iter)->startTime + timeWindowLen;
	}
}

vector<int> getID(vector<Customer*> customerSet){ // �õ�customerSet������Id
	vector<int> ids(0);
	ids.reserve(customerSet.end() - customerSet.begin());
	vector<Customer*>::iterator iter = customerSet.begin();
	for(iter; iter<customerSet.end(); iter++){
		ids.push_back((*iter)->id);
	}
}

Matrix<int> computeTransform(vector<vector<pair<int,int>>> ids, int customernum){
	// ���е�scenario�м���ڵ�֮���ת��Ƶ��
	// customernum: ids�й˿ͽڵ����Ŀ
	Matrix<int> transformRate(customernum+1, customernum+1); 
	for(int i=0; i<customernum+1; i++){  // transfromRate������Ԫ������
		for(int j=0; j<customernum+1; j++){
			transformRate.setValue(i, j, 0);
		}
	}
	// ��һ��/��һ�ж�ӦidΪ0
	// ��Matrix�У�id��С�����������
	vector<vector<pair<int, int>>>::iterator iter = ids.begin();
	for(iter; iter<ids.end(); iter++){
		vector<pair<int, int>>::iterator iter2 = iter->begin();
		for(iter2; iter2<iter->end()-1;){
			int frontpos = iter2->second;   // ǰһ���ڵ���staticIds�е�λ��
			int backpos = (++iter2)->second; // ��һ���ڵ���staticIds�е�λ��
			int originValue = transformRate.getElement(frontpos+1, backpos+1);  // ��һ��Ԫ����depot��Ҫ˳��1
			transformRate.setValue(frontpos+1, backpos+1, ++originValue);
		}
	}
}

vector<pair<int, int>> getIdSetInPlan(vector<Car*> planSet, vector<int> validId){
	// ��planSet��id��ValidId�еĽڵ�ת��ΪIdSet���洢����˳����Ϣ��
	// ���ص������ĵ�һ��Ԫ�أ���0��ʾ�ֿ�ڵ㣬ÿ������·����0����
	// �ڶ���Ԫ�أ���ʾ�ڵ�id��ValidId��λ�ã����ڲֿ�ڵ�0����λ�ü�Ϊ-1
	vector<pair<int, int>> output(0);
	vector<Car*>::iterator carIter = planSet.begin();
	for(carIter; carIter<planSet.end(); carIter++){
		vector<Customer*> customerThisRoute = (*carIter)->getRoute().getAllCustomer(); // �ó���������й˿ͽڵ�
		vector<Customer*>::iterator custIter = customerThisRoute.begin();
		output.push_back(make_pair(0,-1));  // �Ӳֿ⿪ʼ��λ��Ϊ-1����Ϊ�˿͵���staticIdλ����allPlan�д�0��ʼ
		for(custIter; custIter<customerThisRoute.end(); custIter++){
			int currentId = (*custIter)->id;
			vector<int>::iterator idIter = find(validId.begin(), validId.end(), currentId); // ������ǰ�˿͵�id�Ƿ��ڶ�̬�˿ͼ�����
			if(idIter != validId.end()){  // ����Ҫ����Ĺ˿�
				output.push_back(make_pair(currentId, (int)(idIter - validId.begin())));
			}
		}
		output.push_back(make_pair(0,-1));  // �Ӳֿ����
	}
	return output;
}

int computeScore(vector<pair<int, int>> idInPlan, Matrix<int> transformMatrix){
	// ����idInPlan�ĵ÷�
	vector<pair<int, int>>::iterator iter = idInPlan.begin();
	int score = 0;
	for(iter; iter<idInPlan.end()-1;){
		int frontpos = iter->second;
		int backpos = (++iter)->second;
		score += transformMatrix.getElement(frontpos, backpos);
	}
	return score;
}

vector<Car*> assessment(Matrix<int> transformMat, vector<vector<pair<int, int>>> allScenarioId, vector<Customer*> allServedCustomer, 
	vector<Car*> originPlan, bool mark){
	// ����transfromMat��allScenarioId�������ۣ�ѡ�����·���ƻ�
	// allScenarioId��allServedCustomer��Ӧ
	// originPlan: ���в���֮ǰ��·���ƻ���������ȡ������㣬�յ��Լ��ػ�������Ϣ
	// mark: false��ʾû��originPlan, true��ʾ��originPlan
	vector<vector<pair<int, int>>>::iterator planIter = allScenarioId.begin();
	vector<int, int> scoreForPlan(0);  // ����plan�ĵ÷֣��ڶ���Ԫ����plan��λ�ã���ΪҪ����
	int count = 0;
	for(planIter; planIter<allScenarioId.end(); planIter++){
		scoreForPlan.push_back(make_pair(computeScore(*planIter, transformMat), count++));
	}
	sort(scoreForPlan.begin(), scoreForPlan.end(), descendSort<int, int>);   // ��������
	int planIndex = scoreForPlan[0].second;   // ѡ�е�plan��λ��
	vector<pair<int, int>> idInPlan = allScenarioId[planIndex];
	vector<pair<int, int>>::iterator iter2 = idInPlan.begin()+1; // ��һ��Ԫ���ǲֿ⣬����
	vector<Car*> outputPlan(0);   // �����ʼ�ƻ�
	count = 0;  // ������ż���

	// ���÷���ߵļƻ���id˳������Ӧ��customerһһ��Ӧ��·������Ϊ���
	for(iter2; iter2<idInPlan.end(); ){
		Car currentCar  = *originPlan[0];
		if(mark == true) {
			currentCar = *originPlan[count];
		}
		Customer headNode = currentCar.getRoute().getHeadNode();
		Customer rearNode = currentCar.getRoute().getRearNode();
		float capacity = currentCar.getRoute().getCapacity();
		float quantity = currentCar.getRoute().getQuantity();
		Car *tempCar = new Car(headNode, rearNode, capacity, count++, false, quantity);
		while((*iter2).first != 0){ // �Բֿ�Ϊ�ϵ�
			Customer item = *allServedCustomer[(*iter2).second];
			tempCar->getRoute().insertAtRear(item);
			iter2++;
		}
		outputPlan.push_back(tempCar);
		iter2++;
	}
	return outputPlan;
}

vector<Car*>& Simulator::initialPlan(){     // ���ò����ƶ���ʼ�ƻ�
	sort(promiseCustomerSet.begin(), promiseCustomerSet.end());  // �˿ͼ�����id��С��������
	vector<int> staticId = getID(promiseCustomerSet);       // �����ڼƻ���ʼǰ��֪�Ĺ˿�id(���ڱ������Ĺ˿�)
	vector<vector<pair<int, int>>> allScenarioId(samplingRate);  // ���в����õ��ļƻ�
	                                                        // ��һ������id���ڶ�������id��staticid�е�λ��
															// ·��֮����0����
	vector<vector<pair<int, int>>>::iterator planIter = allScenarioId.begin();
	Customer depot = currentPlan[0]->getRoute().getRearNode();
	float capacity = currentPlan[0]->getRoute().getCapacity();
	// �����е��龰����ALNS�㷨�����Ұѽ��Ӧ��id˳�����allIdSet��
	for(planIter; planIter<allScenarioId.end(); planIter++) {
		vector<Customer*> allCustomer(0);   // ����ִ��ALNS�㷨�����й˿ͽڵ�
		vector<Customer*>::iterator iter = promiseCustomerSet.begin();
		for(iter; iter<promiseCustomerSet.end(); iter++){
			allCustomer.push_back(*iter);
		}
		vector<Customer*> currentDynamicCust = generateScenario();  // ����
		iter = currentDynamicCust.begin();
		for(iter; iter<currentDynamicCust.end(); iter++){
			allCustomer.push_back(*iter);
		}
		ALNS alg(allCustomer, depot, capacity);
		vector<Car*> solution(0);
		float cost = 0;
		alg.run(solution, cost);
		*planIter = getIdSetInPlan(solution, staticId);  // ֻ����promise customer��id
	}

	// �����нڵ�֮���ת�ƹ�ϵ���м�¼���õ����־���
	// Ȼ��������龰�µļƻ��������֣�ȡ�÷��������Ϊ��ʼ·���ƻ�
	Matrix<int> transformMat = computeTransform(allScenarioId, (int)staticId.size());  // ת�ƾ���
	Car *tempCar = new Car(depot, depot, capacity, 0);
	vector<Car*> originPlan;
	originPlan.push_back(tempCar);
	vector<Car*> outputPlan = assessment(transformMat, allScenarioId, promiseCustomerSet, originPlan, false);
	return outputPlan;
}

void validPromise(vector<Car*>Plan, vector<Customer*> hurryCustomer, vector<Customer*> &newServedCustomer, vector<Customer*> &newAbandonedCustomer){
	// ��hurry customerȷ��promise
	vector<Car*>::iterator carIter;
	vector<Customer*>::iterator custIter;
	vector<int> hurryCustomerId = getID(hurryCustomer);   // hurry customer��id
	vector<bool> validMark;  // Ԫ��Ϊtrue��ʾȷ�Ϸ���Ϊfalse��ʾ��������
	int i;
	for(i=0; i<(int)hurryCustomer.size(); i++) {
		validMark.push_back(false);
	}
	for(carIter = Plan.begin(); carIter < Plan.end(); carIter++){
		vector<Customer*> tempCust = (*carIter)->getRoute().getAllCustomer();
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			int tempId = (*custIter)->id;
			vector<int>::iterator tempIter = find(hurryCustomerId.begin(), hurryCustomerId.end(), tempId);
			if(tempIter < hurryCustomerId.end()) {
				// ���tempId����hurryCustomerId��
				validMark[tempIter - hurryCustomerId.begin()] = true;
			}
		}
	}
	for(i=0; i<(int)hurryCustomer.size(); i++) {
		if(validMark[i] == true) {
			newServedCustomer.push_back(hurryCustomer[i]);
		} else {
			newAbandonedCustomer.push_back(hurryCustomer[i]);
		}	
	}
}

vector<Car*> Simulator::replan(vector<Customer*> newServedCustomer, vector<Customer*> newAbandonedCustomer, vector<Customer*> delayCustomer) {
	// ���¼ƻ�������vehicle�����Ժ�
	// ����ɸѡ��һЩ�ż��ȴ��ظ���waitCustomer�Լ�һЩ���ż��ظ���
	// newServedCustomer:  (wait customer��)ͨ��re-plan���ܵ�����Ĺ˿�
	// newAbandonedCustomer: (wait customer��)ͨ��re-planȷ���޷����ܷ���Ĺ˿�
	// delayCustomer: ����patient customer�������ǰ����ȷ�Ϸ��������δ����Ϊ�䰲��
	vector<Customer*> hurryCustomer;
	vector<Customer*> patientCustomer;
	vector<Customer*>::iterator custIter;
	float nextMoment = (slotIndex+1) * timeSlotLen; // ��һ��ʱ��ε���ֹʱ�䣨���¸�ʱ��εĿ�ʼʱ�䣩
	for(custIter = waitCustomerSet.begin(); custIter < waitCustomerSet.end(); ) {
		if((*custIter)->tolerantTime <= nextMoment) {  // �ù˿��ż�����ʱ���ǰ�õ��ظ�
			hurryCustomer.push_back(*custIter);    // ����hurryCustomer��
			waitCustomerSet.erase(custIter);       // �����waitCustomerSet��ɾ��
		} else {
			patientCustomer.push_back(*custIter);
			waitCustomerSet.erase(custIter);
		}
	}
	// Ȼ����Щ�˿;����ܵط��뵱ǰ�ƻ���
	// �����ܣ��򷵻�'No' promise
	SSALNS SSALNS_alg1(hurryCustomer, currentPlan);
	vector<Car*> newPlan;
	float finalCost = 0;
	SSALNS_alg1.run(newPlan, finalCost);
	validPromise(newPlan, hurryCustomer, newServedCustomer, newAbandonedCustomer);
	SSALNS SSANLS_alg2(patientCustomer, newPlan);
	SSANLS_alg2.run(newPlan, finalCost);
	validPromise(newPlan, patientCustomer, newServedCustomer, delayCustomer);
	
	// ����
	vector<Customer*> allServedCustomer;    // ���н��ܷ���Ĺ˿�
	for(custIter = promiseCustomerSet.begin(); custIter < promiseCustomerSet.end(); custIter++) {
		allServedCustomer.push_back(*custIter);
	}
	for(custIter = newServedCustomer.begin(); custIter < promiseCustomerSet.end(); custIter++) {
		allServedCustomer.push_back(*custIter);
	}
	sort(allServedCustomer.begin(), allServedCustomer.end());    // �����й˿Ͱ�id��������
	vector<int> allServedCustomerId = getID(allServedCustomer);  // ���й˿͵�id
	vector<vector<pair<int, int>>> allScenarioId(samplingRate);  // ���в����õ��ļƻ���id
	                                                             // ��һ��int��ʾ�˿͵�id���ڶ���int��ʾ��id��allServedCustomerId�е�λ��
	                                                             // ÿ��·����0��ʼ��0������0��ʾ�ֿ⣬��λ��Ĭ��Ϊ-1
	vector<vector<pair<int, int>>>::iterator planIter;
	for(planIter = allScenarioId.begin(); planIter < allScenarioId.end(); planIter++) {
		vector<Car*> tempPlan;
		vector<Customer*>sampleCustomer = generateScenario(); // ������̬����˿͵��龰
		SSALNS SSALNS_alg3(sampleCustomer, newPlan);
		SSALNS_alg3.run(tempPlan, finalCost);
		*planIter = getIdSetInPlan(tempPlan, allServedCustomerId);
	}

	// ����
	Matrix<int> transfromMat = computeTransform(allScenarioId, (int)allServedCustomerId.size());  // �õ����۾���
	vector<Car*> outputPlan = assessment(transfromMat, allScenarioId, allServedCustomer, newPlan, true);
	return outputPlan;
}