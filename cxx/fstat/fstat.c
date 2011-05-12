#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
	char file_path[] = "./aa.txt";
    time_t rawtime;
    int retval;
    int log_fd = 
#ifdef WIN32
	    open(file_path, O_CREAT | O_APPEND | O_WRONLY, S_IREAD);
#else
	    open(file_path, O_CREAT | O_APPEND | O_WRONLY, 0444);
#endif
    if (-1 == log_fd)
    {
        printf("Open file `%s' error: %d: %s\n", file_path, errno, strerror(errno));
        retval = -1;
    }
	else
    {
        // judge whether file should be switch.
        struct stat log_stat;
        while(1)
        {
	        if ( -1 == fstat(log_fd, &log_stat) )
	        {
	            printf("Get file status error: %d: %s\n", errno, strerror(errno));
	            // we continue write logs to the old file.
	        }
	        else
	        {
	            // compare modification time.
	            time(&rawtime);
	            struct tm stm;
	            int st_sec = localtime_r(&log_stat.st_mtime, &stm)->tm_sec;
	            int sec = localtime_r(&rawtime, &stm)->tm_sec;
	            printf("%d, %d\n", st_sec, sec);
	            if (st_sec != sec)
	            	printf("------------%d, %d-------------------\n", st_sec, sec);
	        }
		    if ( -1 == ( retval = write( log_fd, &rawtime, sizeof(rawtime) ) ) )
		    {
		        printf("Write log error: %d: %s\n", errno, strerror(errno));
		        return -1;
		    }
		    usleep(100*1000L);
	    }
    }
    return retval;
}




