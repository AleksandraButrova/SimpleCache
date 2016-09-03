#pragma once
#include "RAM.h"
#include <iostream>
#include <fstream>

using namespace std;

RAM::RAM(){
	Storage();
	filled = 0;
	missCounter = 0;
	readReq = 0;
	writeReq = 0;
	wrongAdd = 0;
	size = total_RAM_entry_number;
}

void RAM::evict(){
	long long num_e = LRU.back()->num;

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

void RAM::add(long long num, bool used)
{
	Chunk *temp = new Chunk(num);
	
	if (filled >= size ) 	
	{
		evict();		
		//cout<<"ADD"<<endl;
	}

	LRU.push_front(temp);
	
	pair< list<Chunk*>::iterator, bool > p(LRU.begin(), used);
	HashTable.emplace(num, p);
	
	filled++;
}

void RAM::update(long long num){
	//Chunk *temp = new Chunk(num);
	
	auto it = *HashTable.find(num)->second.first;

	LRU.erase(HashTable.find(num)->second.first);		// remove from LRU
	HashTable.erase(num);								// remove from Table
	
	delete it;

	filled--;

	add(num, true);	
	
	//cout<<"UPDATE\n";	
}

// If read is successly done return "1" 
// else "0" (in case chunk does not exist in RAM).
bool RAM::read(long long addr)
{
	readReq++;

	if (exist(addr))			
	{
		//Storage::read(addr);								// then read and update
		update(addr);

		HashTable.find(addr)->second.second = true;

		return 1;
	}
	else
	{
		missCounter++;

		add(addr, false);									// else add and write
		Storage::write(addr);
	}
	return 0;
}

void RAM::write(long long addr)
{
	if (exist(addr))			// if addr exist at RAM 
	{
		update(addr);			// then update
		writeReq++;
	}
}

bool RAM::exist(long long addr)
{
	if (HashTable.find(addr) != HashTable.end())			// if addr exist at RAM
		return true;
	else
		return false;
}

void RAM::remove(long long addr)
{
	LRU.remove(*HashTable.find(addr)->second.first);
	HashTable.erase(addr);

	filled--;
}

void RAM::saveStatistics(long long lba_counter, string traceName)
{
	fstream fout("statistics.txt", ios_base::app);
	fout << "================================\n";
	fout << "================================\n";
	fout << "RAM\n";
	fout << "================================\n";

	//fout << "Amount of requests = " << lba_counter << endl;
	fout << "# requests  = " << readReq + writeReq << endl;
	//fout << "# read requests  = " << readReq << "  (" << 100 * readReq * 1.0 / (readReq + writeReq) << " %)\n";

	fout << "# cache miss  = " << missCounter << "  (" << 100 * missCounter * 1.0 / readReq << " %)\n";
	fout << "# cache hit  = " << readReq - missCounter << "  (" << 100 * (1 - missCounter * 1.0 / readReq) << " %)\n";

}

void RAM::cleanAndResize(int new_size)
{
	size = new_size;

	filled =01;
	missCounter = 0;
	readReq = 0;
	writeReq = 0;
	wrongAdd = 0;

	LRU.clear();
	HashTable.clear();

}
