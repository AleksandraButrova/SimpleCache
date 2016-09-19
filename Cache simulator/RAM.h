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
protected:
	int size;
	long long filled;		// It is number of entry. It mustn't be exceed RAM_size from param.h	
	
							/*Structure for searches
							Param long long: address of chunk.
							Param bool:
							false = this lba haven't been read after adding
							true = this lba have been read after adding*/
	unordered_map <long long, pair< list<Chunk*>::iterator, bool>> HashTable;		
	list <Chunk*> LRU;		// LRU structure for eviction
	

	// for statistics
	
	long long wrongAdd;

public:	
	long long missCounter;

	/*Init with 100% size (that is no prefetcher)*/
	RAM();
	int ret_size();
	void evict();
	void add(long long num, bool used);
	void update(long long num);

	bool read(long long addr);

	void write(long long addr);

	bool exist(long long addr);
	
	void remove(long long addr);

	void saveStatistics(long long lba_counter, char* traceName, char* statName);
	
	void cleanStatistics();

	void cleanAndResize(int new_size);
};
	
