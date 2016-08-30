#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	string traceForCut
		, newTrace;

	traceForCut = "final_trace_handled_SR_5sec_NEW";
	newTrace = "Rostelecom_5(2)";

	ifstream fout(traceForCut);
	ofstream fin(newTrace);

	long int line_counter = 0;

	/*while (!fout.eof())
	{
		line_counter++;
		cout << line_counter << endl;
	}*/
	//cout << "Total number of lba: " << line_counter << endl;
	//cout << "How much to cut from this trace? ";
	
	//long int num;
	//cin >> num;

	char buff[100];
	for (int i = 0; i < 1e5; i++)
	{
		//cout << i << endl;
		fout.getline(buff, 100);
		//fin << buff << "\n";
	}
	for (int i = 0; i < 1e5; i++)
	{
		cout << i << endl;
		fout.getline(buff, 100);
		fin << buff << "\n";
	}

	fout.close();
	fin.close();

	return 0;
}