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

			// the total number for big_trace is 16721775 (count by my special programm)


/* Looking for number of chunk which includes given LBA address*/
long long findNum(long long addr) {

	return addr * (unsigned long int)lba_size / (unsigned long int)chunk_size / 1024;
}

void saveAllStatistics(long long lba_counter, string trace_name)
{
	storage.saveStatistics(lba_counter, trace_name);
	cache.saveStatistics(lba_counter, trace_name);
	prefetcher.saveStatistics(lba_counter, trace_name);
}

void saveStatisticsSet(float percent, long long lba_counter, string stat_name)
{
	fstream fout(stat_name, ios_base::app);

	
	fout << lba_counter << ", ";
	fout << percent << ", ";
	fout << storage.read_counter << ", ";
	fout << cache.readReq << ", ";
	fout << cache.missCounter << ", ";
	fout << cache.missCounter * 1.0 / cache.readReq << ", ";
	fout << prefetcher.readReq << ", ";
	fout << prefetcher.missCounter << ", ";
	fout << prefetcher.missCounter * 1.0 / prefetcher.readReq << ", ";
	fout << prefetcher.wrongAdd << ", ";
	fout << prefetcher.wrongAdd * 1.0 / prefetcher.prefetched << ", ";
	fout << prefetcher.prefetched << "\n";
	
	
	fout.close();
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
	history.push_back(ch);
		
		if (!cache.read(ch))						// then read from storage
			storage.read(ch);						// and fill read heastory
	

	return 1;
}

/* Processing one chunk "ch" with prefetcher, RAM and storage.*/
void stepPrefRamStor(long long ch, string action)
{
	if (prefetcher.size != 0)
		if (!prefetcher.read(ch))	// if request for read and chunk 'ch' doesn't exist in prefetcher
			if (!cache.read(ch))		// then read from cache
				storage.read(ch);		// and if it is impossible then read from storage
	
	if (prefetcher.size == 0)
		if (!cache.read(ch))		// then read from cache
			storage.read(ch);		// and if it is impossible then read from storage
}

/* One trace is processed one time
without learning and prefetcher.*/
void processing(string trace_name)
{
	ifstream trace(trace_name);

	char buff[20];						// buffer for reading data about request

	long long lba_counter = 0;
	long long line_counter = 0;

	long long lba;
	long long size;
	string type;
	string action = "";

	while (!trace.eof())
	{
		line_counter++;					// TEST LINE

		trace >> buff;					// volum name, useless

		trace >> buff;					// Read LBA from trace
		lba = atoll(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoll(buff);				// Convert into the number

		trace >> buff;					// meta, useless
		trace >> buff;					// stast time, useless
		trace >> buff;					// finish time, useless

		trace >> buff;
		type = buff;


		if (line_counter % 1000000 == 0)
			cout << line_counter << "\t" << lba_counter << endl;

		if (type == "S")
			continue;;
		
		lba_counter++;

		if (!cache.read(lba))		// then read from cache
			storage.read(lba);		// and if it is impossible then read from storage

		
		
	}
	storage.saveStatistics(lba_counter, trace_name);
	cache.saveStatistics(lba_counter, trace_name);

	trace.close();
}

/* Learning prefetcher on trace */ 
void processWithLearning(string trace_name, string rulesName)
{
	ifstream trace(trace_name);
	
	char buff[100];						// buffer for reading data about request

	long long lba_counter = 0;
	long long line_counter = 0;

	long long lba;
	long long size;
	string action = "";
	string type;

	bool endOfTrace = trace.eof();

	trace.getline(buff, 100);			///useless string
	/*volume_name	lba	len	meta	start_time	end_time	R
LUN04	2225205317	16	1	1463660529.065354	1463660529.066004	S*/
	while (!endOfTrace)
	{
		line_counter++;					// TEST LINE

		trace >> buff;					// volum name, useless

		trace >> buff;					// Read LBA from trace
		lba = atoll(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoll(buff);				// Convert into the number

		trace >> buff;					// meta, useless
		trace >> buff;					// stast time, useless
		trace >> buff;					// finish time, useless

		trace >> buff;
		type = buff;

		
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
			cout << "# processed lba:\n" << line_counter << "\t" << lba_counter << endl << endl;
			apriori(history.item, rulesName, prefetcher.Rules);
			endOfTrace = true;			// For finish while()

			storage.saveStatistics(lba_counter, trace_name);
			cache.saveStatistics(lba_counter, trace_name);
		}
	}

}

// without learning
long long processWithPrefetcher(string trace_name)
{

	ifstream trace(trace_name);

	char buff[100];						// buffer for reading data about request

	long long lba_counter = 0;
	long long line_counter = 0;

	long long lba;
	long long size;
	string action = "";
	string type;

	trace.getline(buff, 100);			///useless string

	while (!trace.eof())
	{
		line_counter++;					// TEST LINE

		trace >> buff;					// volum name, useless

		trace >> buff;					// Read LBA from trace
		lba = atoll(buff);				// Convert into the number

		trace >> buff;					// Read size of requested data
		size = atoll(buff);				// Convert into the number

		trace >> buff;					// meta, useless
		trace >> buff;					// stast time, useless
		trace >> buff;					// finish time, useless

		trace >> buff;
		type = buff;


		if (line_counter % 10000 == 0)
			cout << line_counter << "\t" << lba_counter << endl;

		if (type == "S")
			continue;

		lba_counter++;
		stepPrefRamStor(lba, action);
	}
	return lba_counter;
}

/* The first trace for learning,
the second for check prefetcher.*/
void processingLearnAndPrefetch2(string trace_name1, string trace_name2)
{
	string rulesName = "rules_sup_5_w1e3";

	time_t time1;
	time(&time1);
	cout << "Now: " << ctime(&time1)<< endl;
	
	processWithLearning(trace_name1, rulesName);


	/* Update statistics after change modes.
	Print statistics after learning and delete.*/
	//saveAllStatistics(trace_name1);
	cleanAllStatistics();
	
	//time_t time2;
	//time(&time2);
	//cout << "Now: " << ctime(&time2) << endl;
	////cout << "Wasted for learning: " << ctime(&time2) - ctime(&time1) << endl;
	//
	//cache.cleanAndResize(RAM_entry_num);
	//long long lba_counter = processWithPrefetcher(trace_name2);
	//

	//time_t time3;
	//time(&time3);
	//cout << "Now: " << ctime(&time3) << endl;
	////cout << "Wasted for processing: " << ctime(&time3) - ctime(&time2) << endl;

	////printing statistic 
	//saveAllStatistics(lba_counter, trace_name2);
}

/*Fill cache rules from file*/
void fillRules(string rules_file)
{
	fstream rules(rules_file);
	char buff[32];
	vector <long long> rule_beg, rule_end;
	long long num;
	bool  end = false;
	string::size_type sz = 0;
	//stoll(buff, &sz, 10)
	
	while (!rules.eof())
	{
		// begin
		rules >> buff;
		for (int i = 0; i < 32; i++)
			if (buff[i] == ',')
			{
				buff[i] = '\0';
				break;
			}
		rule_beg.push_back(stoll(buff, &sz, 10));

		// end
		rules >> buff;
		for (int i = 0; i < 32; i++)
			if (buff[i] == ',')
			{
				buff[i] = '\0';
				break;
			}
		rule_end.push_back(stoll(buff, &sz, 10));

		// support
		rules >> buff;
		prefetcher.addRule(rule_beg, rule_end);
		rule_beg.clear();
		rule_end.clear();

	}
}

void proc(string trace_name )
{

	cache.cleanAndResize(RAM_entry_num);
	time_t time2;
	time(&time2);
	cout << "Now: " << ctime(&time2) << endl;
	//cout << "Wasted for learning: " << ctime(&time2) - ctime(&time1) << endl;

	cache.cleanAndResize(RAM_entry_num);
	fillRules("rules.txt");
	long long lba_counter = processWithPrefetcher(trace_name);


	time_t time3;
	time(&time3);
	cout << "Now: " << ctime(&time3) << endl;
	//cout << "Wasted for processing: " << ctime(&time3) - ctime(&time2) << endl;

	//printing statistic 
	saveAllStatistics(lba_counter, trace_name);
}
// fixed rules

void step(float a, float b, float t, int size, string tr1, string stat_name)
{
	for (float percent = a; percent <= b; percent += t)
	{
		time_t time2;
		time(&time2);
		cout << "Now: " << ctime(&time2) << "\tpercent = " << percent << endl;

		storage.cleanStatistics();
		cache.cleanAndResize(size * (100 - percent) * 0.01);
		prefetcher.cleanAndResize(size * percent * 0.01);

		long long lba_counter = processWithPrefetcher(tr1);
		saveStatisticsSet(percent, lba_counter, stat_name);

	}
}
void proc1(string tr1)
{
	fillRules("rules_sup_5_w1e3");

	string stat_name = "stat1_size1e5.csv";//"stat_(3)_s40_10per.csv";

	fstream fout(stat_name, ios_base::app);
	
	fout << "all, ";
	fout << "percent, ";
	fout << "storage.reads, ";
	fout << "cache.reads, ";
	fout << "cache.miss, ";
	fout << "cache.missesPer, ";
	fout << "prefetcher.reas, ";
	fout << "prefetcher.misses, ";
	fout << "prefetcher.missesPer, ";
	fout << "prefetcher.wrongAdd, ";
	fout << "prefetcher.wrongAddPer, ";
	fout << "prefetcher.prefetched\n";

	fout.close();
	int size = 1e5;

	step(0, 50, 10, size, "log_2016-05-19_15%3A22%3A08.977654.txt_SR_5", stat_name);
	//step(0, 3, 0.010, size, "log_2016-05-19_15%3A22%3A08.977654.txt_SR_5(2)", stat_name);
	//step(0, 3, 0.010, size, "log_2016-05-19_15%3A22%3A08.977654.txt_SR_5(3)", stat_name);
}
int main() 
{

	string traceROSTELECOM ="Rostelecom_5";
	string traceROSGOS = "log_2016-05-19_15%3A22%3A08.977654.txt_SR_5(1)";
	string testTrace = "test_trace.txt";
	//processingLearnAndPrefetch2(testTrace, testTrace);
	
	proc1(traceROSGOS);
	system("PAUSE");
	return 0;
}