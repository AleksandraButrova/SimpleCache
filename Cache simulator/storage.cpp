#pragma once
#include "storage.h"
#include <iostream>
#include <fstream>

using namespace std;

Storage::Storage()
{
	miss_counter = 0;
	requests_counter = 0;
}

void Storage::read(long int addr)
{
	miss_counter++;
	requests_counter++;
	//cout<<"Read address:\t"<<addr<<endl;
}

void Storage::write(long int addr)
{
	requests_counter++;
	//cout<<"Write address:\t"<<addr<<endl;
}

void Storage::printStatistics(long int lba_counter)
{
	cout << "Amount of requests = " << lba_counter << endl;
	cout << "Requests = " << requests_counter << endl;
	cout << "Miss cache = " << miss_counter << endl;

	cout << "Read miss ratio = " << 100 * miss_counter * 1.0 / requests_counter << endl;
	cout << "Read hit ratio = " << 100 * (1 - miss_counter * 1.0 / requests_counter) << endl;
}

void Storage::cleanStatistics()
{
	requests_counter = 0;
	miss_counter = 0;
}

// добавить вывод параметров статистики 
// min_sup, etc
void Storage::saveStatistics(long int lba_counter, string traceName)
{
	fstream fout("statistics.txt", ios_base::app);
	fout << "================================\nStorage.\nStatistics of "<<traceName.c_str();
	fout << "\n================================\n\n";
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    fout << traceName.c_str();
	fout << "Amount of requests = " << lba_counter << endl;
	fout << "# requests  = " << requests_counter << endl;
	fout << "# miss cache = " << miss_counter << endl;

	fout << "# Read miss ratio = " << 100 * miss_counter * 1.0 / requests_counter << endl;
	fout << "# Read hit ratio = " << 100 * (1 - miss_counter * 1.0 / requests_counter) << endl;	
	
}