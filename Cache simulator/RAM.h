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
	int size = RAM_entry_num;
public:		
	long int filled;												// It is number of enmtry. It mustn't be exceed RAM_size from param.h	
	list <Chunk*> LRU;												// LRU structure for eviction
	unordered_map <long int, list<Chunk*>::iterator> HashTable;		// Structure for searches
	
	RAM();
	
	void evict();
	void add(long int num);
	void update(long int num);

	bool read(long int addr);

	void write(long int addr);

	bool exist(long int addr);
	
	void remove(long int addr);

	// methonds interaction with prefetcher		

};
	
