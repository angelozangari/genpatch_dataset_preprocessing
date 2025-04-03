#include <stdio.h>
#include <string.h>
int main() {
char s1[100] = "This is an ", s2[] = "insecure implementation";
// strcat concatenates str1 and str2
// the resultant string is stored in str1.
strcat(s1, s2);
return 0;
}