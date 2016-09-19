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
protected:
	long long prefetched;

	/* Buffer for storage history.
	Buffer size must be equel window_size
	for sunc with prefetcher learning. */
	vector<long long> buff;

	
public:
	/* Store rules.
	Vector is rule where the last element is chuck for prefect.
	long long is support */
	map <vector<long long>, vector <long long>> Rules;

	Prefetch();

	/* LRU structure.
	Append to the end, removed from the begining. */
	void pustToBuff(long long addr);


	/* If read is successfuly done return "1" 
	 else "0" (in case chunk does not exist in prefetcher). */
	bool read(long long addr);
	
	//void add(long long num, bool used);

	// With every request for read check rukes for prefetching
	void checkRules();
	bool isRule(vector<long long> rule);

	// Prefetch chunk "addr" (works like add and update)
	long long prefetch (vector <long long> addr);


	/* Let's add rules there for limitation its numbers. */
	void addRule(vector<long long> rule_beg, vector<long long> rule_end);

	void saveStatistics(long long lba_counter, char* traceName, char* stat_name);

	void cleanStatistics();

	void cleanAndResize(int new_size);
};