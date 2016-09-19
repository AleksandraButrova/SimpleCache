#pragma once
#include "prefetch.h"
#include <iostream>
#include <fstream>

using namespace std;

Prefetch::Prefetch()
{
	RAM::RAM();/*
	size = prefetch_entry_num;
	filled = 0;
	missCounter = 0;
	readReq = 0;
	writeReq = 0;
	wrongAdd = 0;
	prefetched = 0;*/
}

void Prefetch::pustToBuff(long long addr)
{
	if (buff.size() == window_size)
		buff.erase(buff.begin());

	buff.push_back(addr);
}

bool Prefetch::read(long long addr)
{
	Storage::read(addr);

	pustToBuff(addr);				// commit in history  for prefetcher analysis
	checkRules();					// check rules for new prefetching

	if (exist(addr))				// if addr exist in prefetcher
	{
		// confirn the usefulness of cached addr
		this->HashTable.find(addr)->second.second = true;

		update(addr);
		return 1;
	}

	else
	{
		missCounter++;
		return 0;
	}
}


void Prefetch::checkRules()
{
	vector <long long> temp;
	temp.push_back(buff.back());
	isRule(temp);
}

bool Prefetch::isRule(vector <long long> rule)
{
	/*auto buff_iter = buff.begin();
	auto rule_iter = rule.begin();

	bool find = false;

	for (rule_iter; rule_iter != rule.end() - 1; ++rule_iter)
	{
		for (buff_iter; ; ++buff_iter)
		{
			if (buff_iter == buff.end())
				return false;

			if (*buff_iter == *rule_iter)
			{
				buff_iter++;
				find = true;
				break;
			}
		}
	}
	return true*/;
	auto it = Rules.find(rule);
	
	if (it != Rules.end())
	{
		prefetch(it->second);
		return true;
	}
	return false;

}

// Return prefetched address or 0 in other case
long long Prefetch::prefetch(vector <long long> addresses)
{
	for (int i = 0; i < addresses.size(); i++)
	{
		long long addr = addresses[i];

		if (exist(addr))				// if addr exist in prefetcher
			update(addr);				// it is for this addr remains in prefetcher longer
		else
		{
			prefetched++;

			add(addr, false);
		}
	}

	return 0;
}

void Prefetch::addRule(vector<long long> rule_beg, vector<long long> rule_end)
{
	auto finded = Rules.find(rule_beg);
	if (finded == Rules.end())
		Rules.emplace(rule_beg, rule_end);
	else
		finded->second.push_back(rule_end[0]);
}

void Prefetch::saveStatistics(long long lba_counter, char* traceName, char* stat_name)
{
	fstream fout(stat_name, ios_base::app);
	fout << "================================\n";
	fout << "================================\n";
	fout<<"PREFETCHER\n";
	fout << "\n================================\n\n";

	//fout << "Amount of requests = " << lba_counter << endl;
	fout << "# requests  = " << requests << endl;
	//fout << "# read requests  = " << readReq << "  (" << 100 * readReq * 1.0 / (readReq + writeReq) << " %)\n";

	fout << "# prefetcher miss  = " << missCounter << "  (" << 100 * missCounter * 1.0 / requests << " %)\n";
	
	fout << "# Wrong prefetched = "<< wrongAdd <<"  (" << 100 * wrongAdd / prefetched << " %)\n";
	fout << "Accuracy = " << 100 - 100 * wrongAdd / prefetched << " %\n";

}
void Prefetch::cleanStatistics()
{
	RAM::cleanStatistics();
	prefetched = 0;
}

void Prefetch::cleanAndResize(int new_size)
{
	size = new_size;

	cleanStatistics();


	LRU.clear();
	HashTable.clear();
	buff.clear();
}