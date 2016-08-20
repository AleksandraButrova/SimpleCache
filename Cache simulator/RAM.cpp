#pragma once
#include "RAM.h"
#include <iostream>

using namespace std;

RAM::RAM(){
	Storage();
	filled = 0;
}

void RAM::evict(){
	long int num_e = LRU.back()->num;

	auto it = LRU.back();

	HashTable.erase(num_e);
	LRU.pop_back();

	delete it;

	filled--;
	//cout<<"EVICT\t"<<num_e<<endl;
}

void RAM::add(long int num){
	Chunk *temp = new Chunk(num);
	
	if (filled >= size ) 	
	{
		evict();		
		//cout<<"ADD"<<endl;
	}

	LRU.push_front(temp);
	HashTable.emplace(num, LRU.begin());
	filled++;
}

void RAM::update(long int num){
	Chunk *temp = new Chunk(num);
	
	auto it = *HashTable.find(num)->second;

	LRU.erase(HashTable.find(num)->second);		// remove from LRU
	HashTable.erase(num);						// remove from Table
	
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
	LRU.remove(*HashTable.find(addr)->second);
	HashTable.erase(addr);

	filled--;
}