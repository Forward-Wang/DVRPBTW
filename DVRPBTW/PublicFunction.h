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
#endif