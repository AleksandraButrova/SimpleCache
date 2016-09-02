#pragma once
#include "Chunk.h"
#include "param.h"
#include "storage.h"

#include <unordered_map>
#include <list>

using namespace std;

/* Class RAM describe simple model RAM memory.
It includes hash table and doubly linked list.
Doubly linked list is LRU structure, 
which consists of chunks. */
class RAM : public Storage
{
private:
	
public:	

	// for statistics
	long long readReq;
	long long writeReq;
	long long missCounter;
	long long wrongAdd;

	vector <long long> reads, misses, wrongs;

	int size;
	long long filled;												// It is number of enmtry. It mustn't be exceed RAM_size from param.h	
			
	list <Chunk*> LRU;												// LRU structure for eviction
	
																	/*Param long long: address of chunk.	
	Param bool: 
	false = this lba haven't been read after adding
	true = this lba have been read after adding*/
	unordered_map <long long, pair< list<Chunk*>::iterator, bool>> HashTable;		// Structure for searches
	
	// 100% size
	RAM();
	
	void evict();
	void add(long long num, bool used);
	void update(long long num);

	bool read(long long addr);

	void write(long long addr);

	bool exist(long long addr);
	
	void remove(long long addr);

	void saveStatistics(long long lba_counter, string traceName);
	
	void cleanAndResize(int new_size);
};
	
