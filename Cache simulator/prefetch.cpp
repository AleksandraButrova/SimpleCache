#pragma once
#include "prefetch.h"
#include <iostream>
#include <fstream>

using namespace std;


void Prefetch::pustToBuff(long int addr)
{
	if (buff.size() == window_size)
		buff.erase(buff.begin());

	buff.push_back(addr);
}

bool Prefetch::read(long int addr)
{
	pustToBuff(addr);				// commit in history  for prefetcher analysis
	checkRules();					// check rules for new prefetching

	if (exist(addr))				// if addr exist in prefetcher
	{
		Storage::read(addr);		// then read it 
		update(addr);
		return 1;
	}

	else
		return 0;
}

long int Prefetch::checkRules()
{
	long int prefetched = 0;

	//cout << "\nCheck...\n";
	for (auto rules_iter = Rules.begin(); rules_iter != Rules.end(); rules_iter++)
	{
		/* If rule with chuck to prefetch exists in prefetcher
		we shouldn't check this rule as it is useless. */
		if (exist(*rules_iter->first.end()) )
			break;

		if (isRule(rules_iter->first))
			prefetched = prefetch(*(rules_iter->first).end());
	}
	//cout << "\End check.\n";

	/*=================================
	Prefetched address can be not alone
	==================================*/
	return prefetched;
}

bool Prefetch::isRule(vector <long int> rule)//map < vector<long int>, long int >::iterator)
{
	auto buff_iter = buff.begin();
	auto rule_iter = rule.begin();

	for (rule_iter; rule_iter != rule.end(); ++rule_iter)
	{
		for (buff_iter; ; ++buff_iter)
		{
			if (buff_iter == buff.end())
				return false;

			if (*buff_iter == *rule_iter)
			{
				buff_iter++;
				break;
			}
		}
	}
	return true;
}

// Return prefetched address or 0 in other case
long int Prefetch::prefetch(long int addr)
{
	if (exist(addr))				// if addr exist in prefetcher
		update(addr);				// it is for this addr remains in prefetcher longer
	else
	{
		add(addr);
		return addr;
	}

	return 0;
}

void Prefetch::addRule(vector<long int> rule, long int importance)
{
	if (Rules.size() >= rules_num)
		;// Rules.erase(Rules.begin());// evict rule
	// add rule if importance is norm
	// if (importance ???)


	

	Rules.emplace(rule, importance);
}

void Prefetch::saveStatistics(long int lba_counter, string traceName)
{
	fstream fout("statistics.txt", ios_base::app);
	fout << "================================\nPrefetcher.\nStatistics of " << traceName.c_str();
	fout << "\n================================\n\n";

	fout << "Amount of requests = " << lba_counter << endl;
	fout << "# requests  = " << requests_counter << endl;
	fout << "# miss cache = " << miss_counter << endl;

	fout << "# Read miss ratio = " << 100 * miss_counter * 1.0 / requests_counter << endl;
	fout << "# Read hit ratio = " << 100 * (1 - miss_counter * 1.0 / requests_counter) << endl;
	fout << "# Wrong prefetched = \n";
}