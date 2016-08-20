#pragma once
#include "param.h"
#include <vector>
#include <set>
#include <map>

using namespace std;

 
/*History is class who commits requests in history order having specific structure.
All history devided into fixed size windows.
Each entry lba is in a window.*/
class History {
public:
	vector<vector<long int>> item;
	long int pos;						// Number position in window. It needs for correct fulling windows.
	int size;							// Number of windows in history. It is learning limit.

	History() : pos(window_size), size(0) {};

	void push_back(long int num);
};


// Methods for learning
void apriori(vector<vector<long int>> history, char *output_filename, map< vector<long int>, long int > &Rules);

map <vector<long int>, long int> makeL1(vector<vector<long int>> history);

map <vector<long int>, long int> scanHistory(vector<vector<long int>> history, vector<vector<long int>> Ck,
	map <vector<long int>, long int> SeqSet);

long int calSupport(vector<vector<long int>> history, vector<long int> seq);

bool isSupport(vector<long int> hwindow, vector<long int> seq);

vector<vector<long int>> generateCk(map <vector<long int>, long int> &L, vector<vector<long int>> history);

bool hasInfrequent(vector<long int> seq, vector<vector<long int>> history);
//,map <vector<long int>, long int> &L);

vector<vector<long int>> genSubseq(vector<long int> seq);
int genRecursion(int n, vector<long int> seq, vector<long int> last, vector<vector<long int>> &Subseqs);


void showRule(map< vector<long int>, long int > &SetSupport, char *output_filename);

// save rules to prefetcher
void saveRules(map< vector<long int>, long int > &L, map< vector<long int>, long int > &Rules);

void printRules(map< vector<long int>, long int> &Rules, char *output_filename);
