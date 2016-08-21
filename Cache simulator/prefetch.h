#pragma once
#include "RAM.h"
#include "param.h"
#include <map>
#include <unordered_map>
#include <list>

using namespace std;

/* This is a part of RAM, which is used for prefetching.*/
class Prefetch : public RAM
{
private:
	int size = prefetch_entry_num;			// Number of stored chunks
	long int rules_num = rules_numer;		// Number of stored rules

public:
	
	/* Buffer for storage history. 
	Buffer size must be equel window_size
	for sunc with prefetcher learning. */
	vector<long int> buff;		

	/* Store rules.
	Vector is rule where the last element is chuck for prefect.
	Long int is support */
	map < vector<long int>, long int> Rules;

	Prefetch() : RAM() {};

	/* LRU structure.
	Append to the end, removed from the begining. */
	void pustToBuff(long int addr);


	/* If read is successly done return "1" 
	 else "0" (in case chunk does not exist in prefetcher). */
	bool read(long int addr);
	

	// With every request for read check rukes for prefetching
	long int checkRules();
	bool isRule(vector<long int> rule);

	// Prefetch chunk "addr" (works like add and update)
	long int prefetch (long int addr);


	/* Let's add rules there for limitation its numbers. */
	void addRule(vector<long int> rule, long int importance);

	void saveStatistics(long int lba_counter, string traceName);
};