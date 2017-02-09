#include "SetBench.h"
#include "PublicFunction.h"

SetBench::SetBench(vector<Customer*> originCustomerSet, int timeSlotLen, int timeSlotNum, float dynamicism):
	originCustomerSet(originCustomerSet), timeSlotLen(timeSlotLen), timeSlotNum(timeSlotNum), dynamicism(dynamicism){}// ���캯��

void SetBench::constructProbInfo(){ 
	// ���ø����ڵ�ĸ�����Ϣ
	vector<int> BHsPos(0);                     // BHs��λ��
	int i;
	vector<Customer*>::iterator iter = originCustomerSet.begin();
	for(iter; iter < originCustomerSet.end(); iter++) {
		vector<float> dist = randomVec(timeSlotNum);   // �ڸ���slot�������ĸ���
		for(i=0; i<timeSlotNum; i++) {
			(*iter)->timeProb[i] = dist[i];
		}
	}
}

void SetBench::construct(vector<Customer*> &staticCustomerSet, vector<Customer*> &dynamicCustomerSet){
	// ���ݸ��������������
	constructProbInfo();
	int customerAmount = originCustomerSet.end() - originCustomerSet.begin();
	int i;
	int dynamicNum = (int)floor(customerAmount*dynamicism);  // ��̬����Ĺ˿�����
	vector<int> staticPos;           // ��̬����Ĺ˿ͽڵ���originCustomerSet�еĶ�λ
	vector<int> dynamicPos = getRandom(0, customerAmount, dynamicNum, staticPos);   // ��̬�����BHs��BHs�����µ�����
	vector<Customer*>::iterator iter = originCustomerSet.begin();
	staticCustomerSet.resize(0);
	dynamicCustomerSet.resize(0);
	for(iter; iter<originCustomerSet.end(); iter++) {
		int count = iter - originCustomerSet.begin();  // ��ǰ�˿ͽڵ���originCustomerSet�еĶ�λ
		                                               // ����Ĭ��originCustomerSet�ǰ�id��������
		vector<int>::iterator iter2 = find(dynamicPos.begin(), dynamicPos.end(), count); // Ѱ��count�Ƿ���dynamicPos�е�Ԫ��
		if(iter2 != dynamicPos.end()) {   // ��dynamicPos������
			dynamicCustomerSet.push_back(*iter);
		} else {  
			staticCustomerSet.push_back(*iter);
		}
		int selectSlot = roulette((*iter)->timeProb, timeSlotNum);   // ���������㷨�����ó��˿Ϳ�����������ʱ���
		float t1 = selectSlot * timeSlotLen;  // ʱ��εĿ�ʼ
		float t2 = (selectSlot+1) * timeSlotLen;      // ʱ��εĽ���
		float tempt = random(t1, t2);
		float maxActiveTime = timeSlotNum * timeSlotLen;  // �����ɹ���������ʱ��
		(*iter)->startTime =  min(tempt, maxActiveTime - 2 * (*iter)->serviceTime); // ���ٿ���2����serviceTime
		float timeWindowLen = random(2 * (*iter)->serviceTime, maxActiveTime - (*iter)->startTime- (*iter)->serviceTime);  // ʱ�䴰����
		(*iter)->endTime = (*iter)->startTime + timeWindowLen;
		(*iter)->tolerantTime = (*iter)->startTime + random(0.3, 0.6) * timeWindowLen;   // �����̵�����õ��ظ���ʱ�䣬Ϊ0.3-0.6����ʱ�䴰����+startTime
	}
}