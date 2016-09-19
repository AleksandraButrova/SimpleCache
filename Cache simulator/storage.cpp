#pragma once
#include "storage.h"
#include <iostream>
#include <fstream>

using namespace std;

Storage::Storage()
{
	readReq = 0;
	requests = 0;
}
void Storage::read(long long addr)
{
	readReq++;
	requests++;
}

void Storage::write(long long addr)
{
	requests++;
}

void Storage::cleanStatistics()
{
	requests = 0;
	readReq = 0;
}

void Storage::saveStatistics(long long lba_counter, char* traceName, char* statName)
{
	fstream fout(statName, ios_base::app);
	fout << "================================================================\n";
	fout << "STORAGE";
	fout << "\n================================\n";

	fout << "Total request's number = " << lba_counter << endl;

	fout << "# requests = " << requests << endl;
	fout << "# reads = " << readReq << endl;
	}
