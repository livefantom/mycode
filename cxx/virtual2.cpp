#include <stdio.h>

class A
{
	public:

		virtual void a() = 0;
		virtual void b() = 0;

		virtual ~A(){}
};

void A::b(){
	a();// can not call pure virtual method???
}
class B : public A
{
	public:
		virtual void a()
		{
			puts("B::a");
		}
		virtual void b()
		{
			puts("B::b");
		}
		void c()
		{
			b();
		}
};

main()
{
	B obj;
	obj.c();
}


