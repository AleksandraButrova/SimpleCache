#include "learning.h"
#include <iostream>
#include <fstream>

using namespace std;

void History::push_back(long long num)
{
	if (pos != window_size) {

		item[item.size() - 1].push_back(num);
		pos++;
	}
	else
	{
		size++;
		pos = 1;
		vector<long long> w;
		w.push_back(num);
		item.push_back(w);
	}
}


void apriori(vector<vector<long long>> history, char *output_filename, map < vector<long long>, vector <long long>> &Rules)
{
	//vector<vector<long long>, long long> SetSupport;

	// generate L1
	map <vector<long long>, long long> L;
	L = makeL1(history);
	int k = 2;				// iteration number
	while (L.size())
	{
		L = scanHistory(history, generateCk(k, L, history), L);
		cout << "Hey rules!" << endl;
		saveRules(L, Rules);
		showRule(L, output_filename);
		k++;
	}
	//printRules(Rules, output_filename);
}

// Generate singelon sequences with their support from history
map <vector<long long>, long long> makeL1(vector<vector<long long>> history)
{
	map <vector<long long>, pair <long long, int>> L1;

	map <vector<long long>, long long> res;

	int window_num = 0;

	for (auto it1 = history.begin(); it1 != history.end(); ++it1)
	{
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			vector<long long> temp;
			temp.push_back(*it2);

			auto iter = L1.find(temp);

			// if this sequential is not exist add it with support 1
			if (iter == L1.end())
			{
				pair <long long, int> supp_win(1, window_num);
				L1.emplace(temp, supp_win);
			}
			// if exist then increment support
			else
				if (iter->second.second != window_num)
				{
					L1.at(temp).first = iter->second.first + 1;
					L1.at(temp).second = window_num;
				}
			}
		window_num++;
		}
		
	

	// delete sequential with support less min_supp
	for (auto it = L1.begin(); it != L1.end(); it++)
	{
		if (it->second.first >= min_supp) // !!!!
			res.emplace(it->first, it->second.first);

	}
	
	deleteUnfrequentFromHistory(history, res);
	return res;
}

void deleteUnfrequentFromHistory(vector<vector<long long>> &history, map <vector<long long>, long long> L1)
{
	for (auto it1 = history.begin(); it1 != history.end(); ++it1)
	{
		bool changed = false;
		vector <long long> changedWindow;
		changedWindow.clear();

		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			vector <long long> temp;
			temp.push_back(*it2);


			if (L1.find(temp) != L1.end())
				changedWindow.push_back(*it2);
			else
				changed = true;		
		}

		if (changed == true)
		{
			*it1 = changedWindow;
			changed = false;
		}

	}
	cout<<history.size();
	return;
}


// Scan history and find frequent sequences in that, then return this set
map <vector<long long>, long long> scanHistory(vector<vector<long long>> history, vector<vector<long long>> Ck, map <vector<long long>, long long> L)
{
	map <vector<long long>, long long> SeqSet;

	for (auto iter = Ck.begin(); iter != Ck.end(); ++iter)
	{
		// check it is a frequent seq
		long long seq_supp = calSupport(history, *iter);
		if (seq_supp >= min_supp) 
		{
			SeqSet.emplace(*iter, seq_supp);
		}
	}
	return SeqSet;
}

// for each window in history it counts sequence support
long long calSupport(vector<vector<long long>> history, vector<long long> seq)
{
	long long seq_supp = 0;

	for (auto it1 = history.begin(); it1 != history.end(); ++it1)
		if (isSupport(*it1, seq))
			seq_supp++;

	return seq_supp;
}

// Check this window is a support for this seq
/* For each item in seq run trought one input window.
If we find a match we countinue run from found window's item to window's end
and loking for next seq number*/
bool isSupport(vector<long long> hwindow, vector<long long> seq)
{
	auto it_window = hwindow.begin();
	auto it_seq = seq.begin();

	for (it_seq; it_seq != seq.end(); ++it_seq)
	{
		for (it_window; ; ++it_window)
		{
			if (it_window == hwindow.end())
				return false;

			if (*it_window == *it_seq)
			{
				it_window++;
				break;
			}
		}
	}
	return true;
}

// generate Ck (candidate) from Lk-1
vector<vector<long long>> generateCk(int k, map <vector<long long>, long long> &L, vector<vector<long long>> history)
{
	vector<vector<long long>> ret;

	for (auto it1 = L.begin(); it1 != L.end(); ++it1)								
	{
		for (auto it2 = L.begin(); it2 != L.end(); ++it2)							
		{
			vector<long long> seq1((*it1).first.begin(), --(*it1).first.end())
				, seq2((*it2).first.begin(), --(*it2).first.end());

			long long n1 = *(--(*it1).first.end())
				, n2 = *(--(*it2).first.end());

			if (seq1 == seq2 && n1 != n2)
			{
				vector<long long> temp = it1->first;
				temp.push_back(n2);

				if (k == 2)
					ret.push_back(temp);
				if (k > 2 && !hasInfrequent(temp, L))
					ret.push_back(temp);
			}
		}
	}
	return ret;
}

// check there is any subseq in this seq is not a frequet seq
bool hasInfrequent(vector<long long> seq, map <vector<long long>, long long> &L)//, map <vector<long long>, long long> &L)
{
	vector<vector<long long>> Subseq = genSubseq(seq);

	/* Scan frequent set L.
	if at least one member does not exist at L
	then our sequence seq has unfrequent subsequience
	so seq also is unfrequent*/
	for (auto it = Subseq.begin(); it != Subseq.end(); ++it)
	{
		if (*it != seq && L.find(*it) == L.end())
			return true;
	}
	return false;
}

// generate all subset
vector<vector<long long>> genSubseq(vector<long long> seq)
{
	vector<vector<long long>> Subseqs;
	Subseqs.push_back(seq);

	for (unsigned int i = 0; i < seq.size(); i++)
	{
		vector<long long> new_last;
		new_last.push_back(seq[i]);
		Subseqs.push_back(new_last);
		genRecursion(i, seq, new_last, Subseqs);
	}
	return Subseqs;
}
int genRecursion(int n, vector<long long> seq, vector<long long> last, vector<vector<long long>> &Subseqs)
{
	for (int i = n + 1;; i++)
	{
		if (i == seq.size())
			return 0;
		else
		{
			vector<long long> new_last = last;
			new_last.push_back(seq[i]);
			Subseqs.push_back(new_last);
			genRecursion(i, seq, new_last, Subseqs);
		}
	}
	return 0;
}

// show all rules
void showRule(map< vector<long long>, long long> &L, char *output_filename)
{
	fstream fout(output_filename, ios_base::app);

	// print rules by line
	for (auto iter = L.begin(); iter != L.end(); ++iter)
	{
		long long s = (iter->first).size();
		
		if (s == 1)
			continue;

		double supp = double(iter->second);

		// print one line (one rule) in correct form
		for (auto it = (iter->first).begin(); it != iter->first.end(); it++)
		{
			if (it != iter->first.end() - 1)
				fout << *it << ", ";
			else
				fout << *it << ",\t" << supp << endl;
		}
	}

	fout.close();
}

// save rules to prefetcher with their support as importance of rule
void saveRules(map< vector<long long>, long long> &L, map< vector<long long>, vector<long long>> &Rules)
{
	for (auto iter = L.begin(); iter != L.end(); ++iter)
	{
		vector <long long> rule_beg, rule_end;
		
		auto it_v = iter->first.begin();
		long long rule_e = *it_v;
		it_v++;
		
		for (it_v; it_v != iter->first.end(); it_v++)
		{
			rule_beg.push_back(rule_e);
			rule_e = *it_v;
		}
		rule_end.push_back(rule_e);
		
		auto finded = Rules.find(rule_beg);
		if (finded == Rules.end())
			Rules.emplace(rule_beg, rule_end);
		else
			finded->second.push_back(rule_e);
	}
		
}

void printRules(map< vector<long long>, vector <long long>> &Rules, char *output_filename)
{
	fstream fout(output_filename, ios_base::app);

	//fout << "\n\PRINT RULES()\n\n";

	// print rules by line
	for (auto iter = Rules.begin(); iter != Rules.end(); ++iter)
	{
		// print one line (one rule) in correct form
		for (auto it = (iter->first).begin(); it != iter->first.end(); it++)
			fout << *it << ", ";
				
		//fout << iter->second << endl;
	}

	fout.close();
}