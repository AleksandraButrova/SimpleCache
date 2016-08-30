#pragma once
#include "prefetch.h"
#include <iostream>
#include <fstream>

using namespace std;

Prefetch::Prefetch()
{
	filled = 0;
	missCounter = 0;
	readReq = 0;
	writeReq = 0;
	wrongAdd = 0;
}

void Prefetch::pustToBuff(long long addr)
{
	if (buff.size() == window_size)
		buff.erase(buff.begin());

	buff.push_back(addr);
}

bool Prefetch::read(long long addr)
{
	readReq++;
	pustToBuff(addr);				// commit in history  for prefetcher analysis
	checkRules();					// check rules for new prefetching

	if (exist(addr))				// if addr exist in prefetcher
	{
		Storage::read(addr);		// then read it 
		update(addr);
		return 1;
	}

	else
	{
		missCounter++;
		return 0;
	}
}

long long Prefetch::checkRules()
{
	long long prefetched = 0;

	//cout << "\nCheck...\n";
	for (auto rules_iter = Rules.begin(); rules_iter != Rules.end(); rules_iter++)
	{
		/* If rule with chuck to prefetch exists in prefetcher
		we shouldn't check this rule as it is useless. */
		if (exist(rules_iter->first[(rules_iter->first).size() - 1]))
			break;

		if (isRule(rules_iter->first))
			prefetched = prefetch((rules_iter->first)[rules_iter->first.size() - 1]);
	}
	//cout << "\End check.\n";

	/*=================================
	Prefetched address can be not alone
	==================================*/
	return prefetched;
}

bool Prefetch::isRule(vector <long long> rule)//map < vector<long long>, long long>::iterator)
{
	auto buff_iter = buff.begin();
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
	return true;
}

// Return prefetched address or 0 in other case
long long Prefetch::prefetch(long long addr)
{
	if (exist(addr))				// if addr exist in prefetcher
		update(addr);				// it is for this addr remains in prefetcher longer
	else
	{
		prefetched++;

		add(addr);
		return addr;
	}

	return 0;
}

void Prefetch::addRule(vector<long long> rule, long long importance)
{
	//if (Rules.size() >= rules_num)
		;// Rules.erase(Rules.begin());// evict rule
	// add rule if importance is norm
	// if (importance ???)

	Rules.emplace(rule, importance);
}

void Prefetch::saveStatistics(long long lba_counter, string traceName)
{
	fstream fout("statistics.txt", ios_base::app);
	fout << "================================\nPrefetcher.\nStatistics of " << traceName.c_str();
	fout << "\n================================\n\n";

	fout << "Amount of requests = " << lba_counter << endl;
	fout << "# requests  = " << readReq + writeReq << endl;
	fout << "# read requests  = " << readReq << "  (" << 100 * readReq * 1.0 / (readReq + writeReq) << " %)\n";

	fout << "# prefetcher miss  = " << missCounter << "  (" << 100 * missCounter * 1.0 / readReq << " %)\n";
	fout << "# prefetcher hit  = " << readReq - missCounter << "  (" << 100 * (1 - missCounter * 1.0 / readReq) << " %)\n";

	fout << "# Wrong prefetched = "<< wrongAdd <<"  (" << 100 * wrongAdd / prefetched << " %)\n";
	fout << "Accuracy = " << 100 - 100 * wrongAdd / prefetched << " %\n";
}