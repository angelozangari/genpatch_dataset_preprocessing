#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "myfunc.h"
#include "roots.h"

int main()
{
    char a[3][MAX];

    char t = '\0';
    int i = 0;

    for (; t != '\n'; i++) {
        if (scanf("%s%c", a[i], &t) != 2)
            return 0;
    }

    if (input_check(a, i))
        print(solve(atof(a[0]), atof(a[1]), atof(a[2])));

    return myfunc(2);
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 15,18;15,24

// 18,18;18,24

// 16,18;16,24

