#include "SetBench.h"
#include "PublicFunction.h"
#include<ctime>
#include<cstdlib>
#include<algorithm>


SetBench::SetBench(vector<Customer*> originCustomerSet, int timeSlotLen, int timeSlotNum):
	originCustomerSet(originCustomerSet), timeSlotLen(timeSlotLen), timeSlotNum(timeSlotNum){}// ���캯��


vector<int> getRandom(int lb, int ub, int m, vector<int> &restData){
	// ����m����ͬ�ģ���ΧΪ[lb, ub)�������
	// restData, ���˷���ֵ��ʣ�����ֵ
	restData.resize(0);
	for(int i=0; i<ub-lb; i++) {
		restData.push_back(i+lb);
	}
	int total = m;
	vector<int> outputData(0);
	for(int j=0; j<m; j++) {
		vector<int>::iterator iter = restData.begin();
		int num = rand() % total; // 0-total-1
		iter += num;
		int temp = *(iter);
		outputData.push_back(num);
		restData.erase(iter);
		total--;
	}
	return outputData;
}

void SetBench::constructProbInfo(){ 
	// ���ø����ڵ�ĸ�����Ϣ
	vector<int> BHsPos(0);                     // BHs��λ��
	int i, j;
	vector<Customer*>::iterator iter = originCustomerSet.begin();
	for(iter; iter < originCustomerSet.end(); iter++) {
		vector<float> dist = randomVec(timeSlotNum);   // �ڸ���slot�������ĸ���
		(*iter)->timeProb = dist;
	}
}

void SetBench::construct(vector<Customer*> &staticCustomerSet, vector<Customer*> &dynamicCustomerSet){
	// ���ݸ��������������
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
		vector<int>::iterator iter2 = find(dynamicPos.begin(), dynamicPos.end(), count); // Ѱ��count�Ƿ���dynamicPos�е�Ԫ��
		if(iter2 != dynamicPos.end()) {   // ��dynamicPos������
			dynamicCustomerSet.push_back(*iter);
		} else {  
			staticCustomerSet.push_back(*iter);
		}
		float randFloat = random(0, 1);
		float accumulation = 0;   // �����㷨
		int timeSlot = 0;         // ʱ���
		vector<float>::iterator iter2 = (*iter)->timeProb.begin();
		i=1;  // ѡ�е�ʱ���
		while(accumulation < randFloat) {
			accumulation += *(iter2++);	
			i++;
		}
		float t1 = (i-1) * timeSlotLen;  // ʱ��εĿ�ʼ
		float t2 = i * timeSlotLen;      // ʱ��εĽ���
		float tempt = random(t1, t2);
		float maxActiveTime = timeSlotNum * timeSlotNum;  // �����ɹ���������ʱ��
		(*iter)->startTime =  min(tempt, maxActiveTime - 2 * (*iter)->serviceTime); // ���ٿ���2����serviceTime
		float timeWindowLen = random(2 * (*iter)->serviceTime, maxActiveTime - (*iter)->startTime);  // ʱ�䴰����
		(*iter)->endTime = (*iter)->startTime + timeWindowLen;
		(*iter)->tolerantTime = (*iter)->startTime + random(0.3, 0.6) * timeWindowLen;   // �����̵�����õ��ظ���ʱ�䣬Ϊ0.3-0.6����ʱ�䴰����+startTime
	}
}