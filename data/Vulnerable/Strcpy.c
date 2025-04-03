#include <stdio.h>
#include <string.h>
int main() {
char src[20] = "C programming";
char dest[20];
// copying src to dest
strcpy(dest, src);
puts(dest); // C programming
return 0;
}