%{ 
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "./TPE/structs.h"

gramatica_struct * gramatica;
%} 


%%
\{(" "?[A-Z]" "?,)*(" "?[A-Z]" "?)\} {printf("%s\n",yytext);};
\{(" "?[a-z]" "?,)*(" "?[a-z]" "?)\} {printf("%s\n",yytext);};
[A-Z] {printf("%s\n",yytext);};
([A-Z]" "?)->(" "?((\\|([a-z]|[A-Z])+)" "?\|" "?)*(" "?\\|([a-z]|[A-Z])+)" "?) {printf("%s\n",yytext);};
.*" "?= ;
. ;
\n ;
%%

int
main(int argc, char* argv[]){
	FILE* fileToOpen= fopen(argv[1], "r");
	if(fileToOpen== NULL)
		printf("Error al abrir el archivo\n");
		
	
	yyin=fileToOpen;
	
	createGramatica();
	gramatica->production_function->size = 1;
	printf("La cantidad es: %d \n",gramatica->production_function->size);
	gramatica->terminals;
	gramatica->nonterminals;
	
	yylex();
//	printf("la cantidad de caracteres es %d \n",contador);
}


int createGramatica(){
	
	gramatica=malloc(sizeof(gramatica_struct));
	if (gramatica == NULL)
		return 0;
	return initializeFunctions() && initializeNonTerminals() && initializeTerminals();
}

int initializeFunctions(){
	gramatica->production_function =malloc(sizeof(production_function_struct));
	if(gramatica->production_function == NULL)
	return 0;
	gramatica->production_function->productions=NULL;
	gramatica->production_function->size=0;
	return 1;
}

int initializeNonTerminals(){
	gramatica->nonterminals = malloc(sizeof(nonterminals_struct));
	if(gramatica->nonterminals == NULL)
	return 0;
	gramatica->nonterminals->nonterminals =NULL;
	gramatica->nonterminals->size=0;
	return 1;
}

int initializeTerminals(){
	gramatica->terminals =malloc(sizeof(terminals_struct));
	if(gramatica->terminals == NULL)
	return 0;
	gramatica->terminals->terminal=NULL;
	gramatica->terminals->size=0;
	return 1;
}