#include<stdio.h>
class Base1
{
public:
	Base1(){printf("Base1::Base1()\n");}
	~Base1(){printf("Base1::~Base1()\n");}
};


class Base2
{
public:
        Base2(){printf("Base2::Base2()\n");}
        virtual ~Base2(){printf("Base2::~Base2()\n");}
};

//
class Son1 : public Base1
{
public:
	Son1(){printf("Son1::Son1()\n");}
	~Son1(){printf("Son1::~Son1()\n");}
};

class Son2 : public Base2
{
public:
        Son2(){printf("Son2::Son2()\n");}
        ~Son2(){printf("Son2::~Son2()\n");}
};

class Son1 : public Base1
{
public:
        Son1():Base1()
	{
		printf("Son1::Son1()\n");
	}
        ~Son1()
	{
		printf("Son1::~Son1()\n");
	}
};

int main()
{
	Son1 o1;
	Son2 o2;
	return 0;
}
