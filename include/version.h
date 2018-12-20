#ifndef	LOADORDERDUMP_VERSION_INCLUDED
#define LOADORDERDUMP_VERSION_INCLUDED

#define MAKE_STR_HELPER(x) #x
#define MAKE_STR(x) MAKE_STR_HELPER(x)

#define LOADORDERDUMP_VERSION_MAJOR		1
#define LOADORDERDUMP_VERSION_MINOR		0
#define LOADORDERDUMP_VERSION_PATCH		0
#define LOADORDERDUMP_VERSION_BETA		0
#define LOADORDERDUMP_VERSION_VERSTRING	MAKE_STR(LOADORDERDUMP_VERSION_MAJOR) "." MAKE_STR(LOADORDERDUMP_VERSION_MINOR) "." MAKE_STR(LOADORDERDUMP_VERSION_PATCH) "." MAKE_STR(LOADORDERDUMP_VERSION_BETA)

#endif