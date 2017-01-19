#include "Dispatcher.h"
#include<algorithm>
#include "Simulator.h"
#include "PublicFunction.h"

const float MAX_FLOAT = numeric_limits<float>::max();

bool ascendSortForCustId(Customer* item1, Customer* item2) {
	return item1->id < item2->id;
}

Dispatcher::Dispatcher(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, Customer depot, float capacity, int timeSlotLen, 
					   int samplingRate = 30): depot(depot), capacity(capacity), timeSlotLen(timeSlotLen){
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
		dynamicCustomerId.push_back(newCust->id);
	}
	sort(allCustomer.begin(), allCustomer.end(), ascendSortForCustId);  // ��id���е�������
}

void clearPlan(vector<Car*> &plan) {  // ���plan
	vector<Car*>::iterator carIter;
	for(carIter = plan.begin(); carIter < plan.end(); carIter++) {
		delete(*carIter);
	}
	plan.resize(0);
}

void Dispatcher::carFinishedTask(int carIndex){       // �ճ� 
	vector<Car*>::iterator carIter;
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		if((*carIter)->getCarIndex == carIndex) {
			Car *newCar = new Car(**carIter);
			currentPlan.erase(carIter);
			finishedPlan.push_back(newCar);
			break;
		}
	}
}

// wait, depature, arrived, serving
vector<Car*> captureFuturePlan(vector<Car*> currentPlan, int currentTime) {
	// ��currentPlan��ץȡ��û���߹��ļƻ�
	// ������Ҫ�жϻ�����״̬����������ѳ�����δ��Ŀ�ĵأ���futurePlan�ĳ�������һ������ĵ�
	// �����������ĳ���˿ʹ�����futurePlan�ĳ�������һ����ʵ�ĵ�
	vector<Car*>::iterator carIter;
	Customer depot = currentPlan[0]->getRoute().getRearNode();
	vector<Car*> outputPlan;
	for(carIter = currentPlan.begin(); carIter < currentPlan.end(); carIter++) {
		Customer pastNode = (*carIter)->getRoute().currentPos();  // �Ӹõ����
		Customer nextNode = (*carIter)->getRoute().nextPos();  // Ŀ�ĵ��Ǹõ�
		Customer* startNode = new Customer;
		startNode->id = 0;
		startNode->type = 'D';
		switch((*carIter)->getState()){
		case depature: {
			// ��������·��
			float nearestDepartTime = (*carIter)->getDepatureTime();
			float dist = sqrt(pow(pastNode.x - nextNode.x, 2) + pow(pastNode.y - nextNode.y, 2));
			startNode->x = (currentTime - nearestDepartTime) / dist * (nextNode.x - pastNode.x) + pastNode.x;
			startNode->y = (currentTime - nearestDepartTime) / dist * (nextNode.y - pastNode.y) + pastNode.y;
			startNode->arrivedTime = currentTime;
			startNode->serviceTime = 0;
			startNode->startTime = currentTime;
			break;
					   }
		case wait: {
			startNode->x = pastNode.x;
			startNode->y = pastNode.y;
			startNode->arrivedTime = currentTime;
			startNode->serviceTime = 0;
			startNode->startTime = currentTime;
			break;
				   }
		case arrived: {  // �������Ӧ���ǲ��ᷢ����
			startNode->x = nextNode.x;
			startNode->y = nextNode.y;
			startNode->arrivedTime = currentTime;
			startNode->serviceTime = nextNode.serviceTime;
			startNode->startTime = currentTime;
			break;
					  }
		case serving: {
			startNode->x = pastNode.x;
			startNode->y = pastNode.y;
			startNode->arrivedTime = currentTime;
			startNode->serviceTime = pastNode.serviceTime - (currentTime - pastNode.arrivedTime);  // ����ʱ���Ѿ���ȥ��һ���֣�ע��˿͵����Ӧ��ȷ��arrivedTime
			startNode->startTime = currentTime;
			break;
					  }
		}
		float leftQuantity = (*carIter)->getRoute().getLeftQuantity();  // ����ʣ������
		float capacity = (*carIter)->getRoute().getCapacity();
		int carIndex = (*carIter)->getCarIndex();
		Car* newCar = new Car(*startNode, depot, leftQuantity, carIndex, false);
		Route tempRoute = (*carIter)->getRoute().capture();        // ץȡcurrentָ����·��
		vector<Customer*> tempCust = tempRoute.getAllCustomer();   // ���currentָ�������й˿�
		vector<Customer*>::iterator custIter;
		for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
			newCar->getRoute().insertAtRear(**custIter);
		}
		outputPlan.push_back(newCar);
	}
	return outputPlan;
}

vector<EventElement> Dispatcher::handleNewTimeSlot(int slotIndex){ // ��ʱ��ο�ʼ
	vector<Customer*> promiseCustomerSet;
	vector<Customer*> waitCustomerSet;
	vector<Customer*> dynamicCustomerSet;
	vector<int>::iterator custIdIter;
	vector<Car*>::iterator carIter;
	vector<Car*> updatedPlan;
	for(custIdIter = promisedCustomerId.begin(); custIdIter< promisedCustomerId.end(); custIdIter++) {
		promiseCustomerSet.push_back(allCustomer[*custIdIter - 1]);  // id��1��ʼ���
	}
	for(custIdIter = dynamicCustomerId.begin(); custIdIter < dynamicCustomerId.end(); custIdIter++) {
		dynamicCustomerSet.push_back(allCustomer[*custIdIter - 1]);
	}

	vector<EventElement> newEventList;
	if(slotIndex == 0) {  // ·���ƻ���Ҫ��ʼ��
		Car* newCar = new Car(depot, depot, capacity, 0);
		vector<Car*> currentPlan;
		currentPlan.push_back(newCar);
		Simulator smu(samplingRate, timeSlotLen, slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, currentPlan);
		updatedPlan = smu.initialPlan();
		vector<Car*>::iterator carIter;
		for(carIter = updatedPlan.begin(); carIter < updatedPlan.end(); carIter++) {
			currentPlan.push_back(*carIter);
			Customer nextCustomer = (*carIter)->getRoute().nextPos();
			float time = sqrt(pow(depot.x - nextCustomer.x, 2) + pow(depot.y - nextCustomer.y, 2));
			EventElement newEvent(time, carArrived, (*carIter)->getCarIndex(), nextCustomer.id);
			newEventList.push_back(newEvent);
		}
	} else {
		for(custIdIter = waitCustomerId.begin(); custIdIter < waitCustomerId.end(); custIdIter++) {
			waitCustomerSet.push_back(allCustomer[*custIdIter - 1]);
		}
		vector<Car*> futurePlan = captureFuturePlan(currentPlan, slotIndex*timeSlotLen);
		Simulator smu(samplingRate, timeSlotLen, slotIndex, promiseCustomerSet, waitCustomerSet, dynamicCustomerSet, futurePlan);
		// ���ﴫ�ݵ�Ӧ����һ�����˸�桱��·���ƻ�
		vector<Customer*> newServedCustomer;
		vector<Customer*> newAbandonedCustomer;
		vector<Customer*> delayCustomer;
		updatedPlan = smu.replan(newServedCustomer, newAbandonedCustomer, delayCustomer);

		// ����promiseCustomer, rejectCustomer�Լ�waitCustomer
		vector<Customer*>::iterator custIter;
		vector<int>::iterator intIter;
		vector<int> tempVec;
		for(custIter = newServedCustomer.begin(); custIter < newServedCustomer.end(); custIter++) {
			promisedCustomerId.push_back((*custIter)->id);
			tempVec.push_back((*custIter)->id);
		}
		for(custIter = newAbandonedCustomer.begin(); custIter < newAbandonedCustomer.end(); custIter++) {
			rejectCustomerId.push_back((*custIter)->id);
		}
		vector<int> tempVec2;
		sort(waitCustomerId.begin(), waitCustomerId.end());
		sort(tempVec.begin(), tempVec.end());
		set_difference(waitCustomerId.begin(), waitCustomerId.end(), tempVec.begin(), tempVec.end(), tempVec2.begin());
		waitCustomerId = tempVec2;

		// ����·���ƻ������ݱ������һվĿ�ĵز���newEventList
		int count = 0;
		for(carIter = updatedPlan.begin(); carIter < updatedPlan.end(); carIter++) {
			// ���õ����¼ƻ����嵽ԭ�ƻ���currentָ�����
			Car* currentCar = currentPlan[count++];
			currentCar->getRoute().replaceRoute((*carIter)->getRoute());  // �滻currentCar��currenָ���·��
			switch(currentCar->getState()) {
			case wait: {
				// do nothing
				break;
					   }
			case depature:{
				Customer nextCustomer = currentCar->getRoute().nextPos(); // ��һ���˿͵�id
				Customer startNode = (*carIter)->getRoute().getHeadNode();  // ����ĳ�����
				float time = slotIndex*timeSlotLen + sqrt(pow(startNode.x - nextCustomer.x, 2) + pow(startNode.y - nextCustomer.y, 2));
				int carIndex = currentCar->getCarIndex();
				EventElement newEvent(time, carArrived, carIndex, nextCustomer.id);
				newEventList.push_back(newEvent);
				break;
						  }
			case serving: {
				// do nothing
				break;
						  }
			case arrived: {
				// do nothing
				break;
						  }
			}
		}
	}
	return newEventList;
} 

EventElement Dispatcher::handleNewCustomer(int slotIndex, Customer& newCustomer){  // �����¹˿͵���
	vector<int>::iterator intIter;
	for(intIter = dynamicCustomerId.begin(); intIter < dynamicCustomerId.end(); intIter++) { // ��δ֪�˿���ɾ��
		if(*intIter == newCustomer.id) {
			dynamicCustomerId.erase(intIter);
			break;
		}
	}
	float minInsertCost = MAX_FLOAT;
	pair<int, Customer> insertPos;   // ��һ��int�ǻ�����ţ���currentPlan�е�λ�ã����ڶ���Customer�ǲ����ǰ��Ĺ˿�
	vector<Car*>::iterator carIter;
	vector<Car*> futurePlan = captureFuturePlan(currentPlan, newCustomer.startTime);  // ֻ���ܲ��뵽currentָ�����
	EventElement tempEvent(-1, carArrived, -1, -1);  // һ����Ч���¼�
	float time;
	for(carIter = futurePlan.begin(); carIter < futurePlan.end(); carIter++) {
		// ��newCustomer��ÿ��route����С�������
		Customer customer1, customer2;
		float minValue, secondValue;
		(*carIter)->getRoute().computeInsertCost(newCustomer, minValue, customer1, secondValue, customer2);
		if(minValue < minInsertCost) {
			insertPos = make_pair(carIter - currentPlan.begin(), customer1);
			if((*carIter)->getState() == depature && customer1.id == 0) {
				// ���������·;�����½ڵ��Ϊ��һվĿ�ĵ�
				time = customer1.startTime + sqrt(pow(customer1.x - newCustomer.x, 2) + pow(customer1.y - newCustomer.y, 2));
				tempEvent.carIndex = (*carIter)->getCarIndex();
				tempEvent.time = time;
				tempEvent.eventType = carArrived;
				tempEvent.customerId = newCustomer.id;
			}
		}
	}
	EventElement newEvent(-1, carArrived, -1, -1);  // һ����Ч���¼�
	if(minInsertCost == MAX_FLOAT) {
		// û�п��в����
		if(newCustomer.tolerantTime < (slotIndex+1)*timeSlotLen) {  // �Ȳ���replan����reject
			rejectCustomerId.push_back(newCustomer.id);
		} else {  // ���򣬽���ȴ��Ĺ˿�����
			waitCustomerId.push_back(newCustomer.id);  
		}
	} else {
		int selectedCarPos = insertPos.first;
		if(insertPos.second.id == 0 && currentPlan[selectedCarPos]->getState() == depature) { // ������ڵ�������
			Customer frontCustomer = currentPlan[selectedCarPos]->getRoute().currentPos();    // currentָ���Ӧ�ڵ�
			currentPlan[selectedCarPos]->getRoute().insertAfter(frontCustomer, newCustomer);
			newEvent = tempEvent;  // �϶������ֵ��tempEvent
		} else {
			currentPlan[selectedCarPos]->getRoute().insertAfter(insertPos.second, newCustomer);
		}
	}
	return newEvent;
}

EventElement Dispatcher::handleCarArrived(float time, int carIndex){                 // ������������¼�
	currentPlan[carIndex]->updateState(serving);    // ���Ļ���״̬
	Customer servedCustomer = currentPlan[carIndex]->getRoute().currentPos();        // ������Ĺ˿�
	float finishedTime = time + servedCustomer.serviceTime;
	currentPlan[carIndex]->decreaseLeftQuantity(servedCustomer.quantity); // ���»�����ʣ������
	EventElement newEvent(finishedTime, finishedService, carIndex, servedCustomer.id);
	return newEvent;
}

EventElement Dispatcher::handleFinishedService(float time, int carIndex){       // ���������ɷ����¼�
	bool mark = currentPlan[carIndex]->moveForward();
	EventElement newEvent(-1, carArrived, -1, -1);     // ��Ч�¼�
	if(mark == false) { 
		currentPlan[carIndex]->updateState(offwork);   // �ճ�
		newEvent.time = time;
		newEvent.carIndex = carIndex;
		newEvent.eventType = carOffWork;
		carFinishedTask(carIndex);
	} else {
		Customer currentCustomer = currentPlan[carIndex]->getRoute().currentPos();
		Customer nextCustomer = currentPlan[carIndex]->getRoute().nextPos();
		ServedCustomerId.push_back(currentCustomer.id);    // �����ѷ�����˿�
		vector<int>::iterator intIter;
		for(intIter = promisedCustomerId.begin(); intIter < promisedCustomerId.end(); intIter++) {
			// ���յ�'OK'promise�Ĺ˿���ɾ��
			if(*intIter == currentCustomer.id) {
				promisedCustomerId.erase(intIter);
				break;
			}
		}
		currentPlan[carIndex]->updateDepatureTime(time);  // ���³���ʱ��
		currentPlan[carIndex]->updateState(depature);     // ����
		time += sqrt(pow(currentCustomer.x - nextCustomer.x, 2) + pow(currentCustomer.y - nextCustomer.y, 2));
		newEvent.time = time;
		newEvent.carIndex = carIndex;
		newEvent.eventType = carDepature;
		newEvent.customerId = nextCustomer.id;
	}
	return newEvent;
}


