#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include <stdarg.h>

void print_info(const char *fmt, ...);
void print_error(const char *tag, const char *fmt, ...);
void panic(const char *fmt, ...);

#define assert(expr) \
	if(!(expr)){ \
		panic("Assertion '" #expr "' failed! (" __FILE__ ":%i)",__LINE__); \
	}

#endif
