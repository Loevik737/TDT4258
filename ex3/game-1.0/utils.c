#include "utils.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <error.h>
#include <errno.h>

void print_info(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

void print_error(const char *tag, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "[%s] ", tag);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

void panic(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "[panic] ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(-1);
}

void nsleep(uint64_t nsec)
{
	struct timespec ts;

	ts.tv_sec = nsec / NSEC;
	ts.tv_nsec = nsec % NSEC;

	while (nanosleep(&ts, &ts) == -1 && errno == EINTR) ;
}
