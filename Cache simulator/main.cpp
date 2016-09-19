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


/*Save storage, cache and prefetcher statistics of trace "trace_name" (the second argument)
in file "statistic.txt".
Warning: do not use this function when prefetcher has not been used because devesion by zero will occure.*/
void saveAllStatistics(long long lba_counter, char* trace_name, char* stat_name)
{
	storage.saveStatistics(lba_counter, trace_name, stat_name);
	cache.saveStatistics(lba_counter, trace_name, stat_name);
	prefetcher.saveStatistics(lba_counter, trace_name, stat_name);
}

/*Save current storage, cache and prefetcher statistics in file "stat_name"(the third argument) in csv format.
Warning: do not use this function when prefetcher has not been used because devesion by zero will occure.*/
void saveStatisticsCSV(float percent, long long lba_counter, string stat_name)
{
	fstream fout(stat_name, ios_base::app);
	
	/*fout << lba_counter << ", ";
	fout << percent << ", ";
	fout << storage.read_counter << ", ";
	fout << cache.readReq << ", ";
	fout << cache.missCounter << ", ";
	fout << prefetcher.readReq << ", ";
	fout << prefetcher.missCounter << ", ";
	fout << prefetcher.wrongAdd << ", ";
	fout << prefetcher.wrongAdd * 1.0 / prefetcher.prefetched << ", ";
	fout << prefetcher.prefetched << "\n";*/
	
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
	history.push_back(ch);							// fill history	
		
		if (!cache.read(ch))						// read from cache
			storage.read(ch);						// if there is no chunk ch then read from storage

	return 1;
}

/* Processing one chunk "ch" with prefetcher, RAM and storage.*/
void stepPrefRamStor(long long ch, string action)
{
	if (prefetcher.ret_size() != 0)			// if prefetcher is anable (if we have allocated memory for him)
		if (!prefetcher.read(ch))		// if request for read and chunk 'ch' doesn't exist in prefetcher
			if (!cache.read(ch))		// then read from cache 
				storage.read(ch);		// and if it is impossible then read from storage
	
	if (prefetcher.ret_size() == 0)			// prefetcher is disable (if we have not allocated memory for him)
		if (!cache.read(ch))			// then read from cache
			storage.read(ch);			// and if it is impossible then read from storage
}

/* One trace is processed one time without learning and prefetcher.*/
void processing(char* trace_name, char* stat_name)
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
	storage.saveStatistics(lba_counter, trace_name, stat_name);
	cache.saveStatistics(lba_counter, trace_name, stat_name);

	trace.close();
}

/* Learning prefetcher on trace */ 
int processWithLearning(char* trace_name, char* rulesName, char* stat_name)
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

		
		//if (line_counter % 1000000 == 0)
		//	cout << line_counter <<"\t" << lba_counter << endl;
		
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
			//cout << "History is full." << endl;
			//cout << "# processed lba:\n" << line_counter << "\t" << lba_counter << endl << endl;
			apriori(history.item, rulesName, prefetcher.Rules);
			endOfTrace = true;			// For finish while()

			storage.saveStatistics(lba_counter, trace_name, stat_name);
			cache.saveStatistics(lba_counter, trace_name, stat_name);

			return lba_counter;
		}
	}

}

/* The proccess works with prefetcher after learning. */
long long processWithPrefetcher(char* trace_name)
{
	time_t time1;
	time(&time1);
	cout << "\nStart working on trace " << trace_name << " at " << ctime(&time1)<< endl;
	
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


		if (line_counter % 100000 == 0)
			cout << line_counter/100000 << "\t" << lba_counter << endl;

		if (type == "S")
			continue;
		
		lba_counter++;
		stepPrefRamStor(lba, action);
	}
	return lba_counter;
}

/* Point of entry in the learning process of prefetcher.
The first arg is trace for learning,
the second if for obtained rules.*/
int processingLearn(char* trace_name, char* rulesName, char* stat_name)
{
	time_t time1;
	time(&time1);
	cout << "Start learning on trace "<<trace_name<<" at "<< ctime(&time1)<< endl;
	
	return processWithLearning(trace_name, rulesName, stat_name);
}

/* Fill prefetcher rules from file.*/
void fillRules(char* rules_file)
{
	prefetcher.Rules.clear();

	fstream rules(rules_file);
	char buff[32];
	vector <long long> rule_beg, rule_end;
	bool  end = false;
	string::size_type sz = 0;
	
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

/* Fill header of statists for understanding csv in RStudio.*/
void fillHeaderStat(char* stat_name)
{
	fstream fout(stat_name, ios_base::app);

	fout << "all, ";
	fout << "percent, ";
	fout << "storage.reads, ";
	fout << "cache.reads, ";
	fout << "cache.miss, ";
	fout << "prefetcher.reas, ";
	fout << "prefetcher.misses, ";
	fout << "prefetcher.wrongAdd, ";
	fout << "prefetcher.wrongAddPer, ";
	fout << "prefetcher.prefetched\n";

	fout.close();

}

/* Processing trace_name (5 arg) with prefetcher 
which have from a% to b% member in increment of step = stp.
Save this statistic in stat_name in csv format. */
void step(float a, float b, float stp, int size, char* trace_name, char* stat_name)
{
	for (float percent = a; percent <= b; percent += stp)
	{
		time_t time2;
		time(&time2);
		cout << "Now: " << ctime(&time2) << "\tpercent = " << percent << endl;

		storage.cleanStatistics();
		cache.cleanAndResize(size * (100 - percent) * 0.01);
		prefetcher.cleanAndResize(size * percent * 0.01);

		long long lba_counter = processWithPrefetcher(trace_name);
		saveStatisticsCSV(percent, lba_counter, stat_name);

	}
}

/*This function allows setup parameters as total size and 
percent of prefetching size from interval. Then trace trace_name is proccessed 
with this parameters ans save statistics in stat_name in csv format. 
It is considered that the rules have already done.*/
void processingAndSaveCSV(char* trace_name, char* stat_name)
{
	fillHeaderStat(stat_name);

	float size = 1e5,			// total  cache size
		fromPercent = 0,		// percent to begin 
		toPercent = 95,			// percent to finish (including this number if stepPercent allows)
		stepPercent = 5;		// percent for increment 


	step(fromPercent, toPercent, stepPercent, size, trace_name, stat_name);
}

/* The same of processingAndSaveCSV(...) but fill prefetch rules from file.*/
void processingAndSaveCSVwithFilling(char* trace_name, char* rules_name, char* stat_name)
{
	fillRules(rules_name);
	//fillHeaderStat(stat_name);

	float size = 1e3,			// total  cache size
		fromPercent = 75,		// percent to begin 
		toPercent = 95,			// percent to finish (including this number if stepPercent allows)
		stepPercent = 5;		// percent for increment 


	step(fromPercent, toPercent, stepPercent, size, trace_name, stat_name);
}

void learnALL(vector <char*> traces)
{
	fstream stat("stat_all_per0");
	stat << "trace_num, cache_miss\n";
	

	for (int i = 0; i < traces.size(); i++)
	{
		prefetcher.Rules.clear();

		if (i % 2 == 0)
		{
			char rules[50] = "rule_sup10_";
			strcat(rules, traces[i]);
			cout << rules << endl;

			int lba_counter = processingLearn(traces[i], rules, "statALL_sup10_per0.csv");
			cout << "Find " << prefetcher.Rules.size() + 1 << " rules.\n";
			stat << i << ", " << cache.missCounter * 100.0 / lba_counter << endl;
		} 
		else
		{
			cache.cleanStatistics();

			cout << "Number of processed lba is " << processWithPrefetcher(traces[i + 1]) << endl;
			
			prefetcher.Rules.clear();
		}


	}

}



int main() 
{
/*
	char buffer[10];
	char str[20] = "sorting.exe ";
	int i = 1;

	_itoa(i, buffer, 10);

	strcat(str, buffer);

	cout << str << endl;*/

	char* rules_name = "test_rules.txt",
		*testTrace = "test_trace.txt";
	char* stat_name = "stat1_size1e5_my.csv";
	char* trace_ROSGOS = "log_2016-05-19_15%3A22%3A08.977654.txt_SR_5(1)";
	char* trace_ROSGOS_all = "log_2016-05-19_15%3A22%3A08.977654.txt_SR",
		*stat2 = "stat_1.csv";

	vector <char*> traces = {"test_trace.txt"};
	
	learnALL(traces);
	//processingLearn(testTrace, rules_name);
	
	//processingAndSaveCSVwithFilling(trace_ROSGOS, rules_name, stat_name);
	//processingAndSaveCSV(trace_ROSGOS_all, stat2);
	system("PAUSE");
	return 0;
}