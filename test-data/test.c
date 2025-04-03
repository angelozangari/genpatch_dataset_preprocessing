#include <stdio.h>

int main() {
    // Some code here
    int a = 10;
    int b = 20;
    
    
    printf("%d\n", a + b);  // Some other code
    
    return 0;
}

//                ↓↓↓VULNERABLE LINES↓↓↓

// 1,3;1,10

// 3,1;9,7
