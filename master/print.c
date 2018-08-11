#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include "print.h"

void print_time(void)
{
	time_t t;
	struct tm *p; 
	struct timeval tus;
	t = 0; 
	time(&t);
	p = localtime(&t);
	gettimeofday(&tus, NULL);

	printf("[%02d-%02d-%02d %02d:%02d:%02d:%03d]", 1900+p->tm_year, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tus.tv_usec/1000);
}
