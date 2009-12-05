#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#define SHM_KEY 0x123456


typedef struct {
	char name[4];
	int  age;
} people;


class Man
{
	char _name[4];
	int	 _age;
public:
	const char* getName()const;
	int getAge()const;
	void setName(const char*);
	void setAge(int);
	virtual ~Man(){};
	virtual void print();
};

inline const char* Man::getName()const
{
	return _name;
}

inline int Man::getAge()const
{
	return _age;
}

inline void Man::setName(const char* name)
{
	strncpy(_name, name, 4);
}

inline void Man::setAge(int age)
{
	_age = age;
}

inline void Man::print()
{
	printf("%s is %d years old.", _name, _age);
}




