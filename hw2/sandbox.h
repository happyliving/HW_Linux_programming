#define	HOOK_VAR(name)	static void* old_##name;

#define	MAKEFUNC(name, ptr, R, ...) \
	R (*name)(__VA_ARGS__) = (R (*)(__VA_ARGS__)) ptr;

#define	HOOK_V_1(name, A1, fm1)					\
	HOOK_VAR(name)						\
	void name(A1 a1) {					\
		MAKEFUNC(f, old_##name, void, A1)		\
		mlog(#name"("fm1")\n", a1);			\
		f(a1);						\
	}

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_PATH_LENGTH 30

extern void set_base(const char *);