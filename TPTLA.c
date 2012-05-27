#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int
main(int argc, char* argv[]){

	int pid=0;
	char* file= argv[1];
	char* occur;
	if((occur=strstr(file,".dot"))!=NULL){

		pid=fork();
		if(pid==0)
			execv("automata", argv);
		//else
		//	wait();
	}
	else if((occur=strstr(file,".gr"))!=NULL){
		pid=fork();
		if(pid==0)
			execv("gramatica",argv);
	//	else
	//		wait();
	}
	else
		printf("El archivo no es ni .gr ni .dot\n");
		return 0;
}
