#ifndef _PUBLICFUNCTION_H
#define _PUBLICFUNCTION_H
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<vector>
#include<numeric>

using namespace std;

float random(float start, float end){
	// ����start��end֮��������
	return start+(end-start)*rand()/(RAND_MAX+1.0);
}

template<class T1, class T2>
bool ascendSort(pair<T1, T2> x, pair<T1, T2> y){   
	// ��������
	// �ڶ���Ԫ�ذ�����Ԫ����ԭ�����е�λ��
	return x.first < y.first;
}

template<class T1, class T2>
bool descendSort(pair<T1, T2> x, pair<T1, T2> y){
	// �ݼ�����
	// �ڶ���Ԫ�ذ�����Ԫ����ԭ�����е�λ��
	return x.first > y.second;
}

vector<float> randomVec(int num){  // ����num������������ǵĺ�Ϊ1
	float rest = 1;  // ��ʼ����Ϊ1
	vector<float> output(0);
	for(int i=0; i<num; i++) {
		float temp = random(0, rest); // ���������
		output.push_back(temp);
		rest -= temp;
	}
	return output;
}

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

int roulette(vector<float> probability) {
	// �����㷨
	// ���ѡ��һ������k (from 0 to |probability|)��
	vector<float>::iterator floatIter;
	float sumation1 = accumulate(probability.begin(), probability.end(), 0); // ���
	for(floatIter = probability.begin(); floatIter < probability.end(); floatIter++) {
		*floatIter /= sumation1;  // ��һ��
	}
	int totalNum = probability.end() - probability.begin();  // ����Ŀ
	int k = 0;
	float sumation = 0;
	float randFloat = rand()/(RAND_MAX + 1.0f);
	floatIter = probability.begin();
	while((sumation < randFloat) && (floatIter < probability.end())) {
		k++;
		sumation += *(floatIter++);
	}
	k = max(k-1, 0); // randFloat = 0 ʱ k-1 < 0
	return k;
}
#endif