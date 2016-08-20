#pragma once
#include "storage.h"
#include <iostream>

using namespace std;

Storage::Storage(){
	miss_counter = 0;
	requests_counter = 0;
}

void Storage::read(long int addr){
	miss_counter++;
	requests_counter++;
	//cout<<"Read address:\t"<<addr<<endl;
}

void Storage::write(long int addr){
	requests_counter++;
	//cout<<"Write address:\t"<<addr<<endl;
}

void Storage::print_statistics(long int lba_counter)
{
	cout << "Amount of requests = " << lba_counter << endl;
	cout << "Requests = " << requests_counter << endl;
	cout << "Miss cache = " << miss_counter << endl;

	cout << "Read miss ratio = " << 100 * miss_counter * 1.0 / requests_counter << endl;
	cout << "Read hit ratio = " << 100 * (1 - miss_counter * 1.0 / requests_counter) << endl;
}