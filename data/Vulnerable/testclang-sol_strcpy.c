#include <string.h>
#include <stdio.h>
 
int main() {

	char password[] = "gramsec2018";
	char user[] = "riasc";

	int sizePass =  sizeof(password) / sizeof(password[0]); //sizePass -> 12
	int sizeUser =  sizeof(user) / sizeof(user[0]); //sizeUser -> 6

	if(sizePass <= sizeUser){
   		strcpy(user, password);
	}

   	printf("User: %s\n", user);
   	printf("Password: %s\n", password);

	return 0;
}
//						↓↓↓VULNERABLE LINES↓↓↓

// 13,5;13,11

