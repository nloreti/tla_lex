#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int
main(int argc, char* argv[]){

	int pid=0;
	char* file= argv[1];
	char* occur;
	if((occur=strstr(file,".gr"))!=NULL){
		pid=fork();
		if(pid==0){
	printf("HOLAAAAAAAAAAAAAAAAAAAAAA");
			execv("gramatica",argv);}
	}
	else
		printf("El archivo .gr\n");
		return 0;
}
