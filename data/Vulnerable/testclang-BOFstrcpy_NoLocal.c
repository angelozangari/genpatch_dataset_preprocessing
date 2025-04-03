#include <string.h>
#include <stdio.h>
#define MAXINPUT 24

void manipulate_string(char* input){

	char buf[MAXINPUT];
	strcpy(buf, input);

	printf("%s\n", buf);
}


int main(int argc, char *argv[])
{
	manipulate_string(argv[1]);

	return 0;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 8,1;8,7

