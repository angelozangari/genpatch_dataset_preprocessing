#include <string.h>

char *strcat(char *restrict dest, const char *restrict src)
{
	strcpy(dest + strlen(dest), src);
	return dest;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 5,1;5,7

