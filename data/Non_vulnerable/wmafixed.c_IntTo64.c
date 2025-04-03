#include "wmafixed.h"
int64_t IntTo64(int x){
    int64_t res = 0;
    unsigned char *p = (unsigned char *)&res;
#ifdef ROCKBOX_BIG_ENDIAN
    p[5] = x & 0xff;
    p[4] = (x & 0xff00)>>8;
    p[3] = (x & 0xff0000)>>16;
    p[2] = (x & 0xff000000)>>24;
#else
    p[2] = x & 0xff;
    p[3] = (x & 0xff00)>>8;
    p[4] = (x & 0xff0000)>>16;
    p[5] = (x & 0xff000000)>>24;
#endif
    return res;
}
