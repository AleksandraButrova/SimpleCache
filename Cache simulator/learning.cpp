#include "learning.h"
#include <iostream>
#include <fstream>

using namespace std;

void History::push_back(long int num)
{
	if (pos != window_size) {

		item[item.size() - 1].push_back(num);
		pos++;
	}
	else
	{
		size++;
		pos = 1;
		vector<long int> w;
		w.push_back(num);
		item.push_back(w);
	}
}


void apriori(vector<vector<long int>> history, char *output_filename, map < vector<long int>, long int> &Rules)
{
	//vector<vector<long int>, long int > SetSupport;

	// generate L1
	map <vector<long int>, long int> L;
	L = makeL1(history);

	while (L.size())
	{
		L = scanHistory(history, generateCk(L, history), L);
		cout << "Hey rules!" << endl;
		saveRules(L, Rules);
		showRule(L, output_filename);
	}
	printRules(Rules, output_filename);
}

// Generate singelon sequences with their support from history
map <vector<long int>, long int> makeL1(vector<vector<long int>> history)
{
	map <vector<long int>, long int> L1;

	for (auto it1 = history.begin(); it1 != history.end() - 1; ++it1)
	{
		for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
		{
			vector<long int> temp;
			temp.push_back(*it2);

			long int seq_supp = calSupport(history, temp);
			if (seq_supp >= min_supp)
			{
				L1.emplace(temp, seq_supp);
			}
		}
	}
	return L1;
}

// Scan history and find frequent sequences in that, then return this set
map <vector<long int>, long int> scanHistory(vector<vector<long int>> history, vector<vector<long int>> Ck, map <vector<long int>, long int> L)
{
	map <vector<long int>, long int> SeqSet;

	for (auto iter = Ck.begin(); iter != Ck.end(); ++iter)
	{
		// check it is a frequent seq
		long int seq_supp = calSupport(history, *iter);
		if (seq_supp >= min_supp)
		{
			SeqSet.emplace(*iter, seq_supp);
		}
	}
	return SeqSet;
}

// for each window in history it counts sequence support
long int calSupport(vector<vector<long int>> history, vector<long int> seq)
{
	long int seq_supp = 0;

	for (auto it1 = history.begin(); it1 != history.end(); ++it1)
		if (isSupport(*it1, seq))
			seq_supp++;

	return seq_supp;
}

// Check this window is a support for this seq
/* For each item in seq run trought one input window.
If we find a match we countinue run from found window's item to window's end
and loking for next seq number*/
bool isSupport(vector<long int> hwindow, vector<long int> seq)
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
vector<vector<long int>> generateCk(map <vector<long int>, long int> &L, vector<vector<long int>> history)
{
	vector<vector<long int>> ret;

	for (auto it1 = L.begin(); it1 != L.end(); ++it1)								
	{
		for (auto it2 = L.begin(); it2 != L.end(); ++it2)							
		{
			vector<long int> seq1((*it1).first.begin(), --(*it1).first.end())
				, seq2((*it2).first.begin(), --(*it2).first.end());

			long int n1 = *(--(*it1).first.end())
				, n2 = *(--(*it2).first.end());

			if (seq1 == seq2 && n1 != n2)
			{
				vector<long int> temp = it1->first;
				temp.push_back(n2);

				if (!hasInfrequent(temp, history))
					ret.push_back(temp);
			}
		}
	}
	return ret;
}

// check there is any subseq in this seq is not a frequet seq
bool hasInfrequent(vector<long int> seq, vector<vector<long int>> history)//, map <vector<long int>, long int> &L)
{
	vector<vector<long int>> Subseq = genSubseq(seq);

	for (auto it = Subseq.begin(); it != Subseq.end(); ++it)
	{
		long int seq_supp = calSupport(history, *it);
		if (seq_supp < min_supp)
		{
			return true;
		}
	}
	return false;
}

// generate all subset
vector<vector<long int>> genSubseq(vector<long int> seq)
{
	vector<vector<long int>> Subseqs;
	Subseqs.push_back(seq);

	for (unsigned int i = 0; i < seq.size(); i++)
	{
		vector<long int> new_last;
		new_last.push_back(seq[i]);
		Subseqs.push_back(new_last);
		genRecursion(i, seq, new_last, Subseqs);
	}
	return Subseqs;
}
int genRecursion(int n, vector<long int> seq, vector<long int> last, vector<vector<long int>> &Subseqs)
{
	for (int i = n + 1;; i++)
	{
		if (i == seq.size())
			return 0;
		else
		{
			vector<long int> new_last = last;
			new_last.push_back(seq[i]);
			Subseqs.push_back(new_last);
			genRecursion(i, seq, new_last, Subseqs);
		}
	}
	return 0;
}

// show all rules
void showRule(map< vector<long int>, long int > &L, char *output_filename)
{
	fstream fout(output_filename, ios_base::app);

	// print rules by line
	for (auto iter = L.begin(); iter != L.end(); ++iter)
	{
		long int s = (iter->first).size();
		
		if (s == 1)
			continue;

		double supp = double((*L.find(iter->first)).second);	

		// print one line (one rule) in correct form
		for (auto it = (iter->first).begin(); it != iter->first.end(); it++)
		{
			if (it != iter->first.end() - 1)
				fout << *it << " ";
			else
				fout << " => " << *it << "\t(support = " << supp << ")\n";		
		}
	}

	fout.close();
}

// save rules to prefetcher with their support as importance of rule
void saveRules(map< vector<long int>, long int > &L, map< vector<long int>, long int > &Rules)
{
	for (auto iter = L.begin(); iter != L.end(); ++iter)
		Rules.emplace(iter->first, iter->second);
}

void printRules(map< vector<long int>, long int > &Rules, char *output_filename)
{
	fstream fout(output_filename, ios_base::app);

	fout << "\n\PRINT RULES()\n\n";

	// print rules by line
	for (auto iter = Rules.begin(); iter != Rules.end(); ++iter)
	{
		// print one line (one rule) in correct form
		for (auto it = (iter->first).begin(); it != iter->first.end(); it++)
		{
			if (it != iter->first.end() - 1)
				fout << *it << " ";
			else
				fout << " => " << *it << "\t(support = " << iter->second << ")\n";
		}
	}

	fout.close();
}