#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include <stdarg.h>
#define NSEC (1000000000)

void print_info(const char *fmt, ...);
void print_error(const char *tag, const char *fmt, ...);
void panic(const char *fmt, ...);
void nsleep(uint64_t nsec);

#define assert(expr) \
	if(!(expr)){ \
		panic("Assertion '" #expr "' failed! (" __FILE__ ":%i)",__LINE__); \
	}

#endif
