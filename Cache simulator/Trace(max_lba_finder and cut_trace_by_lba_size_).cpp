
#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <ctime>
#include <sstream>
#include <algorithm>
#include "ram.h"
#include "chunk.h"
#include "param.h"

using namespace std;


int main ()
{
	RAM cache(cache_lines);
	Storage storage;//s;

	//Storage *storage = &s;
	
	ifstream trace("my_trace");

	char buff[20];						// ����� ��� ���������� ������ �� �����
	int lba;
	int size;
	

	while (!trace.eof())
	{
		trace >> buff;					// ������� LBA �� ������
		lba = atoi(buff);				// �������������� � �����

		trace >> buff;					// ������� ������ ������������� ������
		size = atoi(buff);				// �������������� � �����
	
		// ���� ������ ������, ������� �������� ������ � lba �� (lba + size)
		int num1 = storage.findChunkNum(lba);
		int num2 = storage.findChunkNum(lba + size);

		// ��� ���� ������� � ���������� ��������� ������� �� ������� ������ � ���
		//if (num1 == num2)
		for (int i = num1; (num1 == num2)? (i < num2):(i <= num2) ; i++)
		{
			if (storage.chunks[i].ram_param == NULL){			// ���� ��� ������ �� ������� �� ������
				//cout<<"ADD"<<endl;
				cache.add(&(storage.chunks)[i]);				// �� ��������
			}
			else{												// ���� ����
				//cout<<"UPDATE\t"<<storage.chunks[i].ram_param<<endl;
				cache.update(storage.chunks[i].ram_param);		// �� �������� ������� � ������
			}
		}
		
		// ���������� ��������� ������ �� ������ ������ (���� �� �����)
		trace >> buff;					// read/write
		trace >> buff;					// incoming time

		//cout<<lba<<"\t\t"<<size<<endl;
	}

	trace.close();
	return 0;
}