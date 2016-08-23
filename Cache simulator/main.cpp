#pragma once
#include "RAM.h"
#include "prefetch.h"
#include "learning.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

RAM cache;
Storage storage;
Prefetch prefetcher;
History history;

long int lba_counter = 0;
long int line_counter = 0;				// the total number for big_trace is 16721775 (count by my special programm)


/* Looking for number of chunk which includes given LBA address*/
long int findNum(long int addr) {

	return addr * (unsigned long int)lba_size / (unsigned long int)chunk_size / 1024;
}

/* Delete copies from RAM if this addr exists in prefetcher */
void deleteCopy(long int addr)
{
	if (cache.exist(addr))
		cache.remove(addr);
}

void saveAllStatistics()
{
	storage.saveStatistics(lba_counter, "statistics.txt");
	cache.saveStatistics(lba_counter, "statistics.txt");
	prefetcher.saveStatistics(lba_counter, "statistics.txt");
}
void cleanAllStatistics()
{
	storage.cleanStatistics();
	cache.cleanStatistics();
	prefetcher.cleanStatistics();
}

/* Processing one chunk "ch" with RAM and storage.*/
bool stepLearn(long int ch, string action)
{
	if (action == "read" && !cache.read(ch))		// if request for read and chunk 'ch' doesn't exist in cache
	{
		storage.read(ch);							// then read from storage
		history.push_back(ch);						// and fill read heastory
	}

	if (action == "write")							// if request for write
	{
		cache.write(ch);							// then just write in cache and in storage 
		storage.write(ch);
	}

	// Learning control
	if (history.size >= learning_lim)
	{
		cout << "History is full." << endl;
		apriori(history.item, "rules.txt", prefetcher.Rules);
		return 0;
	}
	return 1;
}

/* Processing one chunk "ch" with prefetcher, RAM and storage.*/
void stepPrefRamStor(long int ch, string action)
{
	if (action == "read" && !prefetcher.read(ch)) // if request for read and chunk 'ch' doesn't exist in prefetcher
	{

		if (!cache.read(ch))		// then read from cache
			storage.read(ch);		// and if it is impossible then read from storage
	}

	if (action == "write")
	{
		storage.write(ch);
		cache.write(ch);
		prefetcher.write(ch);
	}

}

/* One trace is processed one time
without learning and prefetcher.*/
void processing(string trace_name)
{
	ifstream trace(trace_name);

	char buff[20];						// buffer for reading data about request

	long int lba;
	long int size;
	string action;

	while (!trace.eof())
	{
		line_counter++;					// TEST LINE

		trace >> buff;					// Read LBA from trace
		lba = atoi(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoi(buff);				// Convert into the number

		trace >> buff;					// Read action with data : read/write
		action = buff;

		trace >> buff;					// Read incoming time (haven't used)

										// Separete all data into LBA
										// And looking for chunks' number including LBA from lba to (lba + size/lba_size)
		long int chunk_beg = findNum(lba);
		long int chunk_end = findNum(lba + size);

		// Processing each chunk in the gap found above
		for (long int ch = chunk_beg; ch <= chunk_end; ch++)
		{
			lba_counter++;

			if (action == "read" && !cache.read(ch))
				storage.read(ch);									

			if (action == "write")
			{
				cache.write(ch);
				storage.write(ch);
			}
		}
	}
	trace.close();
}

/* Learning prefetcher on trace */ 
void processWithLearning(string trace_name)
{

	ifstream trace(trace_name);

	char buff[20];						// buffer for reading data about request

	long int lba;
	long int size;
	string action;

	while (!trace.eof())
	{
		line_counter++;					// TEST LINE

		trace >> buff;					// Read LBA from trace
		lba = atoi(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoi(buff);				// Convert into the number

		trace >> buff;					// Read action with data : read/write
		action = buff;					

		trace >> buff;					// Read incoming time (haven't used)

										// Separete all data into LBA
										// And looking for chunks' number including LBA from lba to (lba + size/lba_size)
		long int chunk_beg = findNum(lba);
		long int chunk_end = findNum(lba + size);

		// Processing each chunk in the gap found above
		for (long int ch = chunk_beg; ch <= chunk_end; ch++)
		{
			lba_counter++;
			
			if (stepLearn(ch, action) == 0)
			{
				trace.close();
				return;
			}
		}
	}
	// In case if number chunks in trace is lesser then learning_lim
	apriori(history.item, "rules.txt", prefetcher.Rules);
}

// without learning
void processWithPrefetcher(string trace_name)
{

	ifstream trace(trace_name);

	char buff[20];						// buffer for reading data about request

	long int lba;
	long int size;
	string action;

	while (!trace.eof())
	{
		line_counter++;					// TEST LINE

		trace >> buff;					// Read LBA from trace
		lba = atoi(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoi(buff);				// Convert into the number

		trace >> buff;					// Read action with data : read/write
		action = buff;

		trace >> buff;					// Read incoming time (haven't used)

										// Separete all data into LBA
										// And looking for chunks' number including LBA from lba to (lba + size/lba_size)
		long int chunk_beg = findNum(lba);
		long int chunk_end = findNum(lba + size);

		// Processing each chunk in the gap found above
		for (long int ch = chunk_beg; ch <= chunk_end; ch++)
		{
			lba_counter++;

			stepPrefRamStor(ch, action);
		}
	}
}

/*One trace is processed one times by two parts:
the first is learining,
the second is process with prefetcher.*/
void processingLearnAndPrefetch1(string trace_name)
{
	ifstream trace(trace_name);

	char buff[20];						// buffer for input data

	long int lba;
	long int size;
	string action;

	bool switcher = false;				// when learning will be finished it switches process in mode with prefetcher

	while (!trace.eof())
	{
		line_counter++;					// TEST LINE

		trace >> buff;					// Read LBA from trace
		lba = atoi(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoi(buff);				// Convert into the number

		trace >> buff;					// Read action with data : read/write
		action = buff;

		trace >> buff;					// Read incoming time (haven't used)

										// Separete all data into LBA
										// And looking for chunks' number including LBA from lba to (lba + size/lba_size)
		long int chunk_beg = findNum(lba);
		long int chunk_end = findNum(lba + size);

		// Processing each chunk in the gap found above
		for (long int ch = chunk_beg; ch <= chunk_end; ch++)
		{
			lba_counter++;

			if (!switcher && stepLearn(ch, action) == 0)
			{
				/* History is fill.
				Go to processing obtained information.
				Then switch to usual processing mode with prefetcher.*/
				cout << "History is full." << endl;
				apriori(history.item, "rules.txt", prefetcher.Rules);
				switcher = true;

				/* Update statistics after change modes.
				Print statistics after learning and delete.*/
				saveAllStatistics();
				cleanAllStatistics();
			}

			else
			{
				stepPrefRamStor(ch, action);
			}
		}

		
	}

	if (switcher == false)
		cout << "Number chunks in trace is lesser then learning_lim.\nPlease, make learning_limit lesser or use method processingLearnAndPrefetch2.\n";
	else
		;// print statictics

}

/* The first trace for learning,
the second for check prefetcher.*/
void processingLearnAndPrefetch2(string trace_name1, string trace_name2)
{
	processWithLearning(trace_name1);

	/* Update statistics after change modes.
	Print statistics after learning and delete.*/
	saveAllStatistics();
	cleanAllStatistics();

	processWithPrefetcher(trace_name2);
	// add here printing statistic 

	saveAllStatistics();
}


long int main() 
{

	string traceROSTELECOM = "C:\\Users\\Administrator\\Desktop\\Traces\\final_trace_handled_SR_5sec_NEW";
	processingLearnAndPrefetch2(traceROSTELECOM, traceROSTELECOM);

	system("PAUSE");
	return 0;
}