/* Class Storage discribe data on disks.
In the model there isn't necessary to store data,
but some methods are needed to be.*/ 
class Storage{
	 // here may be data 
public:
	long int requests_counter;
	long int miss_counter;

	Storage();

	void write(long int addr);							//	It writes data on disks.
	void read(long int addr);							//	It reads data from desks. 
	void printStatistics(long int lba_counter);		//	It prints miss counter and request counter on console.
	void cleanStatistics();
	void saveStatistics(long int lba_counter, string traceName);
 };
