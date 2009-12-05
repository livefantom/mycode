#include <map>

template<typename _Ty>
class A
{
	typedef std::map<_Ty, _Ty> T_MAP;
	typedef typename T_MAP::iterator T_ITER;

};

int main(int argc, char** argv)
{
	return 0;
}
