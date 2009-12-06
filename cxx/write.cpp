#include <fstream>
#include <iostream>
using namespace std;

class CTest
{
	CTest(int a){};
};

int main()
{
//	CTest t;
	ofstream file;
	file.open("./log.txt");
	file<<"aaaa\n";
	file.flush();
	file<<"bbbb\n";
	file.close();
	return 0;
}
