#include <vector>

using namespace std;
/* Class Storage discribe data on disks.
In the model there isn't necessary to store data,
but some methods are needed to be.*/ 
class Storage{
	 // here may be data 
public:
	long long requests;
	long long readReq;
	
	Storage();

	void write(long long addr);							//	It writes data on disks.
	void read(long long addr);							//	It reads data from desks. 
	void cleanStatistics();
	void saveStatistics(long long lba_counter, char* traceName, char* statName);
 };
