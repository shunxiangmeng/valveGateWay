#ifndef _DEBUG_H
#define _DEBUG_H


#define INFO(fmt, args...) {printf("\e[0;32m"); \
						print_time(); \
						printf("[%s %d]", __FILE__, __LINE__); \
						printf(fmt, ##args); \
						printf("\e[0m");}

#define WARN(fmt, args...) {printf("\e[0;33m"); \
						print_time(); \
						printf("[%s %d]", __FILE__, __LINE__); \
						printf(fmt, ##args); \
						printf("\e[0m");}


#define ERROR(fmt, args...) {printf("\e[0;31m"); \
						print_time(); \
						printf("[%s %d]", __FILE__, __LINE__); \
						printf(fmt, ##args); \
						printf("\e[0m");}

#endif

