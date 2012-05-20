#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "./TPE/structs.h"

#define MAXLENGHT 15

gramatica_struct * gramatica;


int
main(){	
	int err;
	err = createGramatica();
	if (err == 0){
		printf("Hubo un error creando la gramatica");
		return 0;
	}
		
	gramatica->terminals;
	gramatica->nonterminals;

	addTerminalSymbols("{A,B,C}");
	addNonTerminalSymbols("{a,b,c}");
	addInitialSymbol("A");
	
	showTerminalSymbols();
	showNonTerminalSymbols();
	showInitialSymbol();
	
}

void addInitialSymbol(char * c){
	gramatica->initial = *c;
}


//{A, B, C}
int addTerminalSymbols(char * string){
	
	int err,i;
	
	for ( i = 0; string[i] != '\n' && string[i] != '\0'; i++){
		if ( string [i] >= 'A' && string[i] <= 'Z'){
			err = addTerminal(string[i]);
			if ( err != 1 ){
				return 0;
			}
		}
	}
	
	return 1;

}

int addTerminal(char c){
	int size = gramatica->terminals->size;
	gramatica->terminals->terminal[size] = c;
	gramatica->terminals->size = size+1;
	return 1;
}

int addNonTerminalSymbols(char * string){
	
	int err,i;
	
	for ( i = 0; string[i] != '\n' && string[i] != '\0'; i++){
		if ( string [i] >= 'a' && string[i] <= 'z'){
			err = addNonTerminal(string[i]);
			if ( err != 1 ){
				return 0;
			}
		}
	}
	
	return 1;
}

int addNonTerminal(char c){
	int size = gramatica->nonterminals->size;
	gramatica->nonterminals->nonterminals[size] = c;
	gramatica->nonterminals->size = ++size;
	return 1;
}

//{A -> bC|C|a, B->aB}
int addProductionSymbols(char * string){
	
	int i;
	int flag;
	char left = -1;
	char fright = -1;
	char sright = -1;
	
	flag = 0;
	for(i=0; string[i] != '\0' && string[i] != '\n'; i++){
		if(flag){
			if( (string[i] >= 'a' && string[i] <= 'z') || (string [i] >= 'A' && string[i] <= 'Z') ){
				if ( fright == -1 ){
					fright = string[i];
				}else{
					sright = string[i];
				}
			}else if ( string[i] == '|' || string[i] == '}' ){
				addProduction(left,fright,sright);
				fright = -1;
				sright = -1;
			}else if ( string[i] == ','){
				flag = 0;
				fright = -1;
				sright = -1;
			}
		}
		if( (string [i] >= 'A' && string[i] <= 'Z') && !flag ){
			left = string[i];
			flag = 1;
		}
	}
	
	
}


int addProduction(char leftside, char rightside1, char rightside2){
	int size = gramatica->production_function->size;
	gramatica->production_function->productions[size].leftsimbol = leftside;
	gramatica->production_function->productions[size].rightsimbols[0] = rightside1;
	gramatica->production_function->productions[size].rightsimbols[0] = rightside2;
	gramatica->production_function->size = size++;
	return 1;
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
	gramatica->nonterminals->nonterminals =malloc(sizeof(char*)*MAXLENGHT);
	gramatica->nonterminals->size=0;
	return 1;
}

int initializeTerminals(){
	gramatica->terminals =malloc(sizeof(terminals_struct));
	if(gramatica->terminals == NULL)
	return 0;
	gramatica->terminals->terminal=malloc(sizeof(char*)*MAXLENGHT);
	gramatica->terminals->size=0;
	return 1;
}

void showTerminalSymbols(){
	int i;
	printf("Simbolos Terminales\n");
	for( i = 0; i < gramatica->terminals->size; i++){
		printf("%c\n", gramatica->terminals->terminal[i]);
		
	}
	printf("End\n\n");
}

void showNonTerminalSymbols(){
	int i;
	printf("Simbolos No Terminales\n");
	for( i = 0; i < gramatica->nonterminals->size; i++){
		printf("%c\n", gramatica->nonterminals->nonterminals[i]);
		
	}
	printf("End\n\n");
}


void showInitialSymbol(){
	
	printf("Simbolo Inicial\n");
		printf("%c\n", gramatica->initial);
	printf("End\n\n");
}