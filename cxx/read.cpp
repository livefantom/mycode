#include <fstream>
#include <iostream>
using namespace std;

int main()
{
	ifstream file("./log.txt");
	char buf[1024] = {0};
	file.getline(buf, 1024);
	file.close();
	cout<<buf;
}
