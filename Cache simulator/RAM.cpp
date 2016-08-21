#pragma once
#include "RAM.h"
#include <iostream>
#include <fstream>

using namespace std;

RAM::RAM(){
	Storage();
	filled = 0;
	wrongAdd = 0;
}

void RAM::evict(){
	long int num_e = LRU.back()->num;

	auto it = LRU.back();

	// Check using this lba for statistics (wrong prefetched)
	if (HashTable.find(num_e)->second.second == false)
		wrongAdd++;

	// Now delete
	HashTable.erase(num_e);
	LRU.pop_back();

	delete it;

	filled--;
}

void RAM::add(long int num){
	Chunk *temp = new Chunk(num);
	
	if (filled >= size ) 	
	{
		evict();		
		//cout<<"ADD"<<endl;
	}

	LRU.push_front(temp);
	
	pair< list<Chunk*>::iterator, bool > p(LRU.begin(), false);
	HashTable.emplace(num, p);
	
	filled++;
}

void RAM::update(long int num){
	Chunk *temp = new Chunk(num);
	
	auto it = *HashTable.find(num)->second.first;

	LRU.erase(HashTable.find(num)->second.first);		// remove from LRU
	HashTable.erase(num);								// remove from Table
	
	delete it;

	filled--;

	add(num);	
	
	//cout<<"UPDATE\n";	
}

// If read is successly done return "1" 
// else "0" (in case chunk does not exist in RAM).
bool RAM::read(long int addr)
{
	if (exist(addr))			
	{
		Storage::read(addr);								// then read and update
		update(addr);

		HashTable.find(addr)->second.second = true;

		return 1;
	}
	else
	{
		add(addr);									// else add and write
		Storage::write(addr);
	}
	return 0;
}

void RAM::write(long int addr)
{
	if (exist(addr))			// if addr exist at RAM 
		update(addr);			// then update
}

bool RAM::exist(long int addr)
{
	if (HashTable.find(addr) != HashTable.end())			// if addr exist at RAM
		return true;
	else
		return false;
}

void RAM::remove(long int addr)
{
	LRU.remove(*HashTable.find(addr)->second.first);
	HashTable.erase(addr);

	filled--;
}

void RAM::saveStatistics(long int lba_counter, string traceName)
{
	fstream fout("statistics.txt", ios_base::app);
	fout << "================================\nRAM.\nStatistics of " << traceName.c_str();
	fout << "\n================================\n\n";

	fout << "Amount of requests = " << lba_counter << endl;
	fout << "# requests  = " << requests_counter << endl;
	fout << "# miss cache = " << miss_counter << endl;

	fout << "# Read miss ratio = " << 100 * miss_counter * 1.0 / requests_counter << endl;
	fout << "# Read hit ratio = " << 100 * (1 - miss_counter * 1.0 / requests_counter) << endl;
	fout << "# Wrong prefetched = \n";
}