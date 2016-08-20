
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

	char buff[20];						// буфер дл€ считывани€ данных из файла
	int lba;
	int size;
	

	while (!trace.eof())
	{
		trace >> buff;					// считали LBA из трейса
		lba = atoi(buff);				// конвертировали в число

		trace >> buff;					// считали размер запрашиваемых данных
		size = atoi(buff);				// конвертировали в число
	
		// »щем номера чанков, которые содержат адреса с lba до (lba + size)
		int num1 = storage.findChunkNum(lba);
		int num2 = storage.findChunkNum(lba + size);

		// ƒл€ всех номеров в промежутке провер€ем сторедж на наличие ссылки в кэш
		//if (num1 == num2)
		for (int i = num1; (num1 == num2)? (i < num2):(i <= num2) ; i++)
		{
			if (storage.chunks[i].ram_param == NULL){			// если нет ссылки из сторедж на список
				//cout<<"ADD"<<endl;
				cache.add(&(storage.chunks)[i]);				// то добавили
			}
			else{												// если есть
				//cout<<"UPDATE\t"<<storage.chunks[i].ram_param<<endl;
				cache.update(storage.chunks[i].ram_param);		// то обновили сегмент в списке
			}
		}
		
		// —читывание остальных данных со строки трейса (пока не нужны)
		trace >> buff;					// read/write
		trace >> buff;					// incoming time

		//cout<<lba<<"\t\t"<<size<<endl;
	}

	trace.close();
	return 0;
}