#pragma once
#include "storage.h"
#include <iostream>
#include <fstream>

using namespace std;

Storage::Storage()
{
	read_counter = 0;
	requests_counter = 0;
}

void Storage::read(long int addr)
{
	read_counter++;
	requests_counter++;
}

void Storage::write(long int addr)
{
	requests_counter++;
}

void Storage::printStatistics(long int lba_counter)
{	
	cout << "================================\nStorage statistics.";
	cout << "\n================================\n\n";
	
	// Total number lba in trace
	cout << "Total request's number = " << lba_counter << endl;
	
	// Requests to storage
	cout << "# requests = " << requests_counter << endl;
	cout << "# read requests = " << read_counter << endl;

	cout << "\nRead ratio = " << 100 * read_counter * 1.0 / requests_counter << endl;
	cout << "Write ratio = " << 100 * (1 - read_counter * 1.0 / requests_counter) << endl;
}

void Storage::cleanStatistics()
{
	requests_counter = 0;
	read_counter = 0;
}

void Storage::saveStatistics(long int lba_counter, string traceName)
{
	fstream fout("statistics.txt", ios_base::app);
	fout << "================================\nStorage.\nStatistics of "<<traceName.c_str();
	fout << "\n================================\n\n";
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    fout << traceName.c_str();
	// Total number lba in trace
	fout << "Total request's number = " << lba_counter << endl;
	
	// Rerquests to storage
	fout << "# requests  = " << requests_counter << endl;
	fout << "# miss cache = " << read_counter << endl;

	fout << "\nRead ratio = " << 100 * read_counter * 1.0 / requests_counter << " %\n";
	fout << "Write ratio = " << 100 * (1 - read_counter * 1.0 / requests_counter) << " %\n";	
	
}