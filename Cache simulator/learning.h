#pragma once
#include "param.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <map>

using namespace std;

 
/*History is class who commits requests in history order having specific structure.
All history devided into fixed size windows.
Each entry lba is in a window.*/
class History {
public:
	vector<vector<long long>> item;
	long long pos;						// Number position in window. It needs for correct fulling windows.
	int size;							// Number of windows in history. It is learning limit.

	History() : pos(window_size), size(0) {};

	void push_back(long long num);
};


// Methods for learning
void apriori(vector<vector<long long>> history, string output_filename, map< vector<long long>, vector <long long>> &Rules);

map <vector<long long>, long long> makeL1(vector<vector<long long>> history);


void deleteUnfrequentFromHistory(vector<vector<long long>> &history, map <vector<long long>, long long> L1);

map <vector<long long>, long long> scanHistory(vector<vector<long long>> history, map<vector<long long>, pair<long long, int>> Ck,
	map <vector<long long>, long long> SeqSet);

long long calSupport(vector<vector<long long>> history, vector<long long> seq);

bool isSupport(vector<long long> hwindow, vector<long long> seq);

vector<vector<long long>> generateCk(int k, map <vector<long long>, long long> &L, vector<vector<long long>> history);
map<vector<long long>, pair <long long, int>> genCk(int k, map <vector<long long>, long long> &L, vector<vector<long long>> history);
bool hasInfrequent(vector<long long> seq, map <vector<long long>, long long> &L);
//,map <vector<long long>, long long> &L);

vector<vector<long long>> genSubseq(vector<long long> seq);
int genRecursion(int n, vector<long long> seq, vector<long long> last, vector<vector<long long>> &Subseqs);


void showRule(map< vector<long long>, long long> &SetSupport, string output_filename);

// save rules to prefetcher
void saveRules(map< vector<long long>, long long> &L, map< vector<long long>, vector <long long>> &Rules);

void printRules(map< vector<long long>, vector <long long>> &Rules, string output_filename);
