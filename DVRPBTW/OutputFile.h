#ifndef _OUTPUTFILE_H
#define _OUTPUTFILE_H
#include<vector>
#include "Car.h"

class OutputFile{  // �����н�������xml�ļ���
public:
	OutputFile(vector<Car*> carSet):carSet(carSet){};   // ���캯��
	~OutputFile(){};           // ��������
	void exportData();         // ������
private:
	vector<Car*> carSet;
};

#endif