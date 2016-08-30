#pragma once
#include "RAM.h"
#include "prefetch.h"
#include "learning.h"

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <vector>

using namespace std;

RAM cache;
Storage storage;
Prefetch prefetcher;
History history;

long long lba_counter = 0;
long long line_counter = 0;				// the total number for big_trace is 16721775 (count by my special programm)


/* Looking for number of chunk which includes given LBA address*/
long long findNum(long long addr) {

	return addr * (unsigned long int)lba_size / (unsigned long int)chunk_size / 1024;
}

/* Delete copies from RAM if this addr exists in prefetcher */
void deleteCopy(long long addr)
{
	if (cache.exist(addr))
		cache.remove(addr);
}

void saveAllStatistics(string trace_name)
{
	storage.saveStatistics(lba_counter, trace_name);
	cache.saveStatistics(lba_counter, trace_name);
	prefetcher.saveStatistics(lba_counter, trace_name);
}
void cleanAllStatistics()
{
	storage.cleanStatistics();
	cache.cleanStatistics();
	prefetcher.cleanStatistics();
}

/* Processing one chunk "ch" with RAM and storage.*/
bool stepLearn(long long ch, string action)
{
	if (action == "r")		// if request for read and chunk 'ch' doesn't exist in cache
	{
		history.push_back(ch);
		
		if (!cache.read(ch))// then read from storage
			storage.read(ch);						// and fill read heastory
	}

	if (action == "w")							// if request for write
	{
		cache.write(ch);							// then just write in cache and in storage 
		storage.write(ch);
	}
	// Learning control
	//if (history.size >= learning_lim)
	//	return 0;

	return 1;
}

/* Processing one chunk "ch" with prefetcher, RAM and storage.*/
void stepPrefRamStor(long long ch, string action)
{
	if (action == "r" && !prefetcher.read(ch)) // if request for read and chunk 'ch' doesn't exist in prefetcher
	{

		if (!cache.read(ch))		// then read from cache
			storage.read(ch);		// and if it is impossible then read from storage
	}

	if (action == "w")
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

	long long lba;
	long long size;
	string type;
	string action;

	while (!trace.eof())
	{
		line_counter++;					// TEST LINE

		trace >> buff;					// Read LBA from trace
		lba = atoll(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoll(buff);				// Convert into the number

		trace >> buff;					// Read action with data : read/write
		action = buff;

		// new line
		trace >> buff;
		type = buff;

		trace >> buff;					// Read incoming time (haven't used)	

		if (type == "S")
			continue;
		
		lba_counter++;

		if (action == "r" && !cache.read(lba))
			storage.read(lba);									
		
		if (action == "w")
		{
			cache.write(lba);
			storage.write(lba);
		}
		
	}
	storage.saveStatistics(lba_counter, trace_name);
	cache.saveStatistics(lba_counter, trace_name);

	trace.close();
}

/* Learning prefetcher on trace */ 
void processWithLearning(string trace_name)
{
	ifstream trace(trace_name);

	char buff[20];						// buffer for reading data about request

	long long lba;
	long long size;
	string action;
	string type;

	bool endOfTrace = trace.eof();

	while (!endOfTrace)
	{
		line_counter++;					// TEST LINE

		trace >> buff;					// Read LBA from trace
		lba = atoll(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoll(buff);				// Convert into the number

		trace >> buff;					// Read action with data : read/write
		action = buff;					

		// new line
		trace >> buff;
		type = buff;

		trace >> buff;					// Read incoming time (haven't used)
		
		if (line_counter % 1000000 == 0)
			cout << line_counter <<"\t" << lba_counter << endl;
		
		if (type == "S")
			continue;

		lba_counter++;
		

		endOfTrace = trace.eof();

		// If use learning limit the uncomment
		//if ( (stepLearn(lba, action) == 0) || (endOfTrace == true))
		stepLearn(lba, action);
		if (endOfTrace == true)
		{
			trace.close();
			cout << "History is full." << endl;
			cout << "# processed lba:\n" << line_counter << "\t" << lba_counter << endl;
			apriori(history.item, "rules.txt", prefetcher.Rules);
			endOfTrace = true;			// For finish while()
		}
	}
	storage.saveStatistics(lba_counter, trace_name);
	cache.saveStatistics(lba_counter, trace_name);

}

// without learning
void processWithPrefetcher(string trace_name)
{

	ifstream trace(trace_name);

	char buff[20];						// buffer for reading data about request

	long long lba;
	long long size;
	string action;
	string type;

	while (!trace.eof())
	{
		line_counter++;					// TEST LINE
		cout << line_counter << endl;

		trace >> buff;					// Read LBA from trace
		lba = atoll(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoll(buff);				// Convert into the number

		trace >> buff;					// Read action with data : read/write
		action = buff;

		// new line
		trace >> buff;
		type = buff;

		trace >> buff;					// Read incoming time (haven't used)	

		if (type == "S")
			continue;

		lba_counter++;
		stepPrefRamStor(lba, action);
	}
}



/*One trace is processed one times by two parts:
the first is learining,
the second is process with prefetcher.*/
//void processingLearnAndPrefetch1(string trace_name)
//{
//	ifstream trace(trace_name);
//
//	char buff[20];						// buffer for input data
//
//	long long lba;
//	long long size;
//	string action;		// read or write
//	string type;		// sequentual or random
//
//	bool switcher = false;				// when learning will be finished it switches process in mode with prefetcher
//
//	while (!trace.eof())
//	{
//		line_counter++;					// TEST LINE
//
//		trace >> buff;					// Read LBA from trace
//		lba = atoll(buff);				// Convert into the number
//
//		trace >> buff;					// Read size of requested data
//		size = atoll(buff);				// Convert into the number
//
//		trace >> buff;					// Read action with data : read/write
//		action = buff;
//
//		// New line!!
//		trace >> buff;
//		type = buff;
//
//		trace >> buff;					// Read incoming time (haven't used)
//
//		// Skip sequetual 
//
//		if (type == "S")
//			continue;
//
//		cout << type << endl;
//
//		lba_counter++;
//
//
//		if (!switcher && stepLearn(lba, action) == 0)
//		{
//			/* History is fill.
//			 Go to processing obtained information.
//			 Then switch to usual processing mode with prefetcher.*/
//			cout << "History is full." << endl;
//			apriori(history.item, "rules.txt", prefetcher.Rules);
//			switcher = true;
//
//			/* Update statistics after change modes.
//			Print statistics after learning and delete.*/
//			saveAllStatistics();
//			cleanAllStatistics();
//		}
//
//		else
//		{
//			stepPrefRamStor(lba, action);
//		}
//	}
//
//	if (switcher == false)
//		cout << "Number chunks in trace is lesser then learning_lim.\nPlease, make learning_limit lesser or use method processingLearnAndPrefetch2.\n";
//	else
//		;// print statictics
//
//}

/* The first trace for learning,
the second for check prefetcher.*/
void processingLearnAndPrefetch2(string trace_name1, string trace_name2)
{
	time_t time1;
	time(&time1);
	cout << "Now: " << ctime(&time1)<< endl;
	
	processWithLearning(trace_name1);


	/* Update statistics after change modes.
	Print statistics after learning and delete.*/
	//saveAllStatistics(trace_name1);
	cleanAllStatistics();
	
	time_t time2;
	time(&time2);
	cout << "Now: " << ctime(&time2) << endl;
	//cout << "Wasted for learning: " << ctime(&time2) - ctime(&time1) << endl;
	
	cache.cleanAndResize(RAM_entry_num);
	processWithPrefetcher(trace_name2);
	

	time_t time3;
	time(&time3);
	cout << "Now: " << ctime(&time3) << endl;
	//cout << "Wasted for processing: " << ctime(&time3) - ctime(&time2) << endl;

	//printing statistic 
	saveAllStatistics(trace_name2);
}

/*Fill cache rules from file*/
void fillRules(string rules_file)
{
	fstream rules(rules_file);
	string buff;
	string::size_type sz = 0;
	vector <long long> new_rule;

	while (!rules.eof())
	{
		rules >> buff;
		while (buff != "=>" && !rules.eof())
		{
			new_rule.push_back(stoll(buff, &sz, 10));
			rules >> buff;
		}
		rules >> buff;
		new_rule.push_back(stoll(buff, &sz, 10));

		int supp = 0;
		rules >> buff;
		while (buff != ")" && !rules.eof())
		{
			if (buff == "=")
			{	
				rules >> buff;
				supp = stoll(buff, &sz, 10);
				//cout << supp << endl;
				break;
			}
			rules >> buff;
		}
		prefetcher.addRule(new_rule, supp);
		new_rule.clear();
	}
}

void proc(string trace_name)
{


	time_t time2;
	time(&time2);
	cout << "Now: " << ctime(&time2) << endl;
	//cout << "Wasted for learning: " << ctime(&time2) - ctime(&time1) << endl;

	cache.cleanAndResize(RAM_entry_num);
	fillRules("rules.txt");
	processWithPrefetcher(trace_name);


	time_t time3;
	time(&time3);
	cout << "Now: " << ctime(&time3) << endl;
	//cout << "Wasted for processing: " << ctime(&time3) - ctime(&time2) << endl;

	//printing statistic 
	saveAllStatistics(trace_name);
}

int main() 
{


	//string traceROSTELECOM = "\"C:\\Users\\Administrator\\Desktop\\Traces\\final_trace_handled_SR_5sec_NEW\"";
	string traceROSTELECOM ="Rostelecom_5(2)";
	//"final_trace_handled_SR_5sec_NEW";
	//processingLearnAndPrefetch2(traceROSTELECOM, traceROSTELECOM);
	//fillRules("rules.txt");
	proc(traceROSTELECOM);
	system("PAUSE");
	return 0;
}