#include <unistd.h>

int main()
{
	umask(023);// permission = mode & ~umask = 0666 & ~022 = 0644 = rw-r--r--
	mkdir("aaa", 0777);// permission = mode & ~umask & 0777

	return 0;
}
