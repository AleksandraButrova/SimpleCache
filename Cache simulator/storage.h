#include <string>
#include <vector>

using namespace std;
/* Class Storage discribe data on disks.
In the model there isn't necessary to store data,
but some methods are needed to be.*/ 
class Storage{
	 // here may be data 
public:
	long long requests_counter;
	long long read_counter;

	vector <long long> reqs;

	Storage();

	void write(long long addr);							//	It writes data on disks.
	void read(long long addr);							//	It reads data from desks. 
	void printStatistics(long long lba_counter);		//	It prints miss counter and request counter on console.
	void cleanStatistics();
	void saveStatistics(long long lba_counter, string traceName);
 };
