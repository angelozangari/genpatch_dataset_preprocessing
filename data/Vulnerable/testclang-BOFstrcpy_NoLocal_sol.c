#include <string.h>
#include <stdio.h>
#define MAXINPUT 24

void manipulate_string(char* input){

	char buf[MAXINPUT];
	int sizeInput =  strlen(input) + 1; //strlen not including the terminating null character
	
	if(sizeInput <= MAXINPUT){
		strcpy(buf, input);
		printf("%s\n", buf);
	}else {
		 printf("Error\n");
	}
}

int main(int argc, char *argv[])
{
	manipulate_string(argv[1]);
	return 0;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 11,2;11,8

