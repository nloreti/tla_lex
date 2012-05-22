%{ 
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "./TPE/structs.h"

automata_struct * automata;
int final=0;
%} 
%x PARSE
%x NODE
%x STARTLABEL
%x GETLABEL
%x ENDLABEL
%x TRANSITION
%%

<PARSE,INITIAL>node\[shape=doublecircle\] {final=1;};BEGIN(NODE);
<PARSE,INITIAL>node\[shape=circle\] {final=0;};BEGIN(NODE);
<NODE>([A-Z]|[a-z]|[0-9])+ {printf("El nodo  es :%s y %s es final ",yytext,final==1?" ":"no");}; BEGIN(STARTLABEL);
<STARTLABEL>\[label=\" BEGIN(GETLABEL);
<GETLABEL>([a-z]|[A-z]|[0-9])+ {printf("con label %s \n",yytext);};BEGIN(ENDLABEL);
<ENDLABEL>\"\] ; BEGIN(PARSE);
<PARSE,TRANSITION>([A-Z]|[a-z]|[0-9])+" "?->" "?([A-Z]|[a-z]|[0-9])+" "?\[label=\"([A-Z]|[a-z]|[0-9])+\"\] {printf("la Produccion es %s",yytext);};BEGIN(TRANSITION);
. ;
\n ;
%%

int
main(int argc, char* argv[]){
	FILE* fileToOpen= fopen(argv[1], "r");
	if(fileToOpen== NULL)
		printf("Error al abrir el archivo\n");
	
	yyin=fileToOpen;

	yylex();

}



void reportError(char* errorMsg, char* token){

	printf("Se produjo un error tratando de leer %s cuando se leyo %s",errorMsg,token);
	abort();

}
 