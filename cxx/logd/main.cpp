#include "LogSvr.h"
#include <unistd.h>
//#include "types.h"

int main(int argc, char** argv)
{
	daemon(0, 1);
    LogSvr svr;
    svr.initialize();
    svr.work();
    svr.release();
    return 0;
}

