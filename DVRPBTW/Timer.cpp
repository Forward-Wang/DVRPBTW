#include "Timer.h"
#include<cassert>
#include<algorithm>

bool ascendSort(EventElement &a, EventElement &b){  // ��������
	return a.time < b.time;
}

EventElement& Timer::pop(){
	assert(eventList.size() > 0);
	vector<EventElement>::iterator iter = eventList.begin(); 
	EventElement *newElement = new EventElement(*iter);
	eventList.erase(iter);
	return *newElement;
}

Timer::Timer(vector<Customer*> staticCustomerSet, vector<Customer*> dynamicCustomerSet, int timeSlotLen): staticCustomerSet(staticCustomerSet), 
	dynamicCustomerSet(dynamicCustomerSet), timeSlotLen(timeSlotLen){   // ���캯�����������Ϊ���й˿ͣ��Լ���ʱ��ο�ʼֵ

	EventElement *newEvent;
	int timeSlotNum = (int)staticCustomerSet[0]->timeProb.size();   // ʱ�����Ŀ
	int i;
	for(i=0; i<timeSlotNum; i++) {  // ���ӡ�ʱ��ε���¼�
		newEvent = new EventElement(i*timeSlotLen, newTimeSlot, -1, -1);
		eventList.push_back(*newEvent);
	}
	vector<Customer*>::iterator iter = dynamicCustomerSet.begin();
	for(iter; iter< dynamicCustomerSet.end(); iter++) {  // ���ӡ��¹˿͵����¼���
		newEvent = new EventElement((*iter)->startTime, newCustomer, -1, (*iter)->id);
		eventList.push_back(*newEvent);
	}
	sort(eventList.begin(), eventList.end(), ascendSort);
}

void Timer::addEventElement(EventElement &newEvent){  // ���¼����������¼�){  // ���¼����������¼�
	eventList.push_back(newEvent);
	sort(eventList.begin(), eventList.end(), ascendSort);
}

void Timer::deleteEventList(EventType eventType){     // ɾ��eventType���͵��¼�
	vector<EventElement>::iterator iter = eventList.begin();
	for(iter; iter<eventList.end(); ){
		if((*iter).eventType == eventType) {
			iter = eventList.erase(iter);
		}
		iter++;
	}
}

void Timer::updateEventElement(EventElement &newEvent){  // �����¼�
	vector<EventElement>::iterator iter = eventList.begin();
	for(iter; iter<eventList.end(); iter++){
		if((*iter).carIndex == newEvent.carIndex && (*iter).customerId == newEvent.customerId) {
			// �жϸ����¼���λ�ã�������֮
			// ע�����һ���˿ͽڵ㣬���¼����н����һ���¼�
			(*iter).time = newEvent.time;
			(*iter).eventType = newEvent.eventType;
		}
	}
}