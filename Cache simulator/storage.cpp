#pragma once
#include "storage.h"
#include <iostream>
#include <fstream>

using namespace std;

Storage::Storage()
{
	read_counter = 1;
	requests_counter = 1;
}
void Storage::read(long long addr)
{
	read_counter++;
	requests_counter++;
}

void Storage::write(long long addr)
{
	requests_counter++;
}

void Storage::printStatistics(long long lba_counter)
{	
	cout << "================================================================\n";
	cout<<"STORAGE";
	cout << "\n================================\n";
	
	// Total number lba in trace
	cout << "Total request's number = " << lba_counter << endl;
	
	// Requests to storage
	cout << "# requests = " << requests_counter << endl;
	/*cout << "# read requests = " << read_counter << endl;

	cout << "\nRead ratio = " << 100 * read_counter * 1.0 / requests_counter << endl;
	cout << "Write ratio = " << 100 * (1 - read_counter * 1.0 / requests_counter) << endl;*/
}

void Storage::cleanStatistics()
{
	requests_counter = 1;
	read_counter = 1;
}

void Storage::saveStatistics(long long lba_counter, string traceName)
{
	fstream fout("statistics.txt", ios_base::app);
	fout << "================================================================\n";
	fout << "STORAGE";
	fout << "\n================================\n";

	// Total number lba in trace
	fout << /*"Total request's number = " <<*/ lba_counter << endl;

	// Requests to storage
	fout << /*"# requests = " <<*/ requests_counter << endl;
	/*fout << "# read requests = " << read_counter << endl;

	fout << "\nRead ratio = " << 100 * read_counter * 1.0 / requests_counter << endl;
	fout << "Write ratio = " << 100 * (1 - read_counter * 1.0 / requests_counter) << endl;*/

	reqs.push_back(requests_counter);
}
