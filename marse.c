#include "TPE/structs.h"
#include <stdio.h>
#include <stdlib.h>
#define MAXLENGHT 15


void afdToGr();
statechange_struct* createStateChange(char, char, char );
char au_getSymbol(int);
char au_getInitial();
char au_getState(int);
char au_move(char, char);
char * toUpperCase(char*);
void auToGr();
void au_init();
char* gr_printNonTerminalSymbols();
char* gr_printTerminalSymbols();
char* gr_printProductions();
void gr_addProduction(char leftside, char rightside1, char rightside2);
void gr_toFile();
char* au_getStates();
char* au_getAlphabet();
int au_isFinal(char);
void gr_print();

automata_struct * au;
gramatica_struct * gr;


void auToGr(){

	//anfToAfd();
	afdToGr();
	
	
	
	gr_toFile();
	gr_print();
}


void anfToAfd(){
	;
}

void clausuraLambda(){
	;
}

void move(){
	;	
}

void afdToGr(){
	//VT <- E
	gr_addTerminalSymbols(au_getAlphabet());
	//printf("%s\n", au_getAlphabet());
	//VN <- K 
	gr_addNonTerminalSymbols(toUpperCase(au_getStates()));
	//printf("%s\n", au_getStates());
	int i, j = 0;
	//printf("Estados: %d\n",au_getStateAmount());
	//printf("Alfabeto: %d\n",au_getAlphabetLength());
	//Para cada estado q perteneciente a K
	for( i=0; i<au_getStateAmount(); i++){
		//Para cada letra dentro del alfabeto
		for( j=0; j<au_getAlphabetLength(); j++){
			char aux;
			//printf("GetState%d: %c\tGetSymbol%d: %c -> %c\n", i, au_getState(i), j, au_getSymbol(j), au_move(au_getState(i), au_getSymbol(j)));
			if( (aux = au_move(au_getState(i), au_getSymbol(j))) != 0 ){
				//printf("Adding %c->%c%c\n",au_getState(i), au_getSymbol(j), toupper(aux));
				gr_addProduction(au_getState(i), au_getSymbol(j), toupper(aux));
				if( au_isFinal(aux) ){
					gr_addProduction(au_getState(i), au_getSymbol(j), '\\');
				}
			}
		}
	}
	if( au_isFinal(au_getInitial()) ){
		gr_addProduction(au_getInitial(), '\\', '\\');
	}
}


void gr_toFile(){
	FILE* f = fopen( "gramaticaOUT.gr", "w+");
	fputs("GramaticaOUT=({", f);
	fputs(gr_printNonTerminalSymbols(), f);
	fputs("},{", f);
	fputs(gr_printTerminalSymbols(), f);
	fputs("},",f);
	fputc(au_getInitial(), f);
	fputs(",{",f);
	fputs(gr_printProductions(),f);
	fputs("})\n",f);
	fclose(f);
}


void gr_print(){
	printf("%d\n", gr->production_function->size);
	printf("GRAMATICA = ({%s},", gr_printNonTerminalSymbols());
	printf("{%s},", gr_printTerminalSymbols());
	printf("%c,",au_getInitial());
	printf("{%s})\n", gr_printProductions());
}

void au_init(){
	au = malloc(sizeof(automata_struct));
	au->nodes = malloc(sizeof(nodes_struct));
	{
		au->nodes->nodes = malloc(10 * sizeof(int *));
		int i = 0;
		for( i=0; i<10; i++ ){
			au->nodes->nodes[i] = malloc(sizeof(node_struct));
			au->nodes->nodes[i]->label = i + '0';
		}
		au->nodes->size = 10;
	}
	au->finals = malloc(sizeof(finals_struct));
	{
		au->finals->nodes = "89";
		au->finals->size = 2;
	}
	au->language = malloc(sizeof(language_struct));
	{
		au->language->language = "abcde";
		au->language->size = 5;
	}
	au->delta = malloc(sizeof(delta_struct));
	{
		au->delta->delta = malloc(16 * sizeof(int *));
		au->delta->delta[0] = createStateChange('0', 'c', '8');
		au->delta->delta[1] = createStateChange('0', 'a', '1');
		au->delta->delta[2] = createStateChange('0', 'b', '3');
		au->delta->delta[3] = createStateChange('1', 'a', '2');
		au->delta->delta[4] = createStateChange('1', 'b', '2');
		au->delta->delta[5] = createStateChange('2', 'd', '8');
		au->delta->delta[6] = createStateChange('2', 'a', '5');
		au->delta->delta[7] = createStateChange('2', 'c', '3');
		au->delta->delta[8] = createStateChange('3', 'e', '7');
		au->delta->delta[9] = createStateChange('3', 'b', '4');
		au->delta->delta[10] = createStateChange('4', 'b', '0');
		au->delta->delta[11] = createStateChange('5', 'd', '9');
		au->delta->delta[12] = createStateChange('5', 'e', '6');
		au->delta->delta[13] = createStateChange('6', 'e', '7');
		au->delta->delta[14] = createStateChange('7', 'a', '9');
		au->delta->delta[15] = createStateChange('9', 'a', '9');
		au->delta->size = 16;
	}
	au->initial = '0';
	
}


void gr_init(){
	gr = malloc(sizeof(gramatica_struct));
	gr->production_function = malloc(sizeof(production_function_struct));
	gr->production_function->productions = malloc(MAXLENGHT * 15 * sizeof(production_struct));
	gr->production_function->size = 0;
	gr->nonterminals = malloc(sizeof(nonterminals_struct));
	gr->nonterminals->nonterminals = malloc(MAXLENGHT * sizeof(char));
	gr->nonterminals->size = 0;
	gr->terminals = malloc(sizeof(terminals_struct));
	gr->terminals->terminal = malloc(MAXLENGHT * sizeof(char));
	gr->terminals->size = 0;
	gr->initial;
}

statechange_struct* createStateChange(char initialNode, char transition, char finalNode ){
	statechange_struct* sc = malloc(sizeof(statechange_struct));
	sc->initialnode = initialNode;
	sc->finalnode = finalNode;
	sc->transition = transition;
	
	return sc;
}

char * au_getAlphabet(){
	return au->language->language;
}



int gr_addTerminalSymbols(char* symbols){
	
	int i = 0;
	while( symbols[i] != 0){
		gr->terminals->terminal[gr->terminals->size++] = symbols[i++];
	}
	gr->terminals->terminal[gr->terminals->size] = 0;
}

int gr_addNonTerminalSymbols(char* symbols){
	int i = 0;
	while( symbols[i] != 0){
		gr->nonterminals->nonterminals[gr->nonterminals->size++] = symbols[i++];
	}
	gr->nonterminals->nonterminals[gr->nonterminals->size] = 0;
}

char* toUpperCase(char* string){
	int i = 0;
	char * resp = malloc(strlen(string)+1);
	while( string[i] != 0 ){
		resp[i] = toupper(string[i++]);
	}
	resp[i] = 0;
	return resp;
}

char* au_getStates(){
	int i = 0;
	char * string = malloc((au->nodes->size + 1) * sizeof(char));
	
	for(i = 0; i<au->nodes->size; i++ ){
		string[i] = au->nodes->nodes[i]->label;
	}
	return string;
}

int au_getAlphabetLength(){
	return au->language->size;
}

char au_move(char state, char symbol){
	int i = 0;
	
	for( i=0; i<au->delta->size; i++ ){
		//printf("Checkin if %c == %c and %c == %c\n", state, au->delta->delta[i]->initialnode, symbol, au->delta->delta[i]->transition);
		if( au->delta->delta[i]->initialnode == state && au->delta->delta[i]->transition == symbol )
			return au->delta->delta[i]->finalnode;
	}
	return 0;
}

char au_getState(int index){
	return au->nodes->nodes[index]->label;
}

char au_getSymbol(int index){
	return au->language->language[index];
}

int au_isFinal(char state){
	int i = 0;
	for( i=0; i<au->finals->size; i++ ){
		if( au->finals->nodes[i] == state ){
			return 1;
		}
	}
	return 0;
}

char au_getInitial(){
	return au->initial;
}


char* gr_printNonTerminalSymbols(){
	return gr->nonterminals->nonterminals;
}

char* gr_printTerminalSymbols(){
	return gr->terminals->terminal;
}

char* gr_printProductions(){
	int i,j = 0;
	char * string = malloc((gr->production_function->size + 1 ) * sizeof(char) * 6);
	//printf("%d",gr->production_function->size);
	for( i=0; i<gr->production_function->size; i++ ){
		string[j++] = gr->production_function->productions[i].leftsimbol;
		string[j++] = '-';
		string[j++] = '>';
		string[j++] = gr->production_function->productions[i].rightsimbols[0];
		string[j++] = gr->production_function->productions[i].rightsimbols[1];
		string[j++] = ',';
	}
	string[j-1] = 0;
	//printf("%s\n",string);
	return string;
}

int au_getStateAmount(){
	return au->nodes->size;
}


void gr_addProduction(char leftside, char rightside1, char rightside2){
	int size = gr->production_function->size;
	gr->production_function->productions[size].leftsimbol = leftside;
	gr->production_function->productions[size].rightsimbols[0] = rightside1;
	gr->production_function->productions[size].rightsimbols[1] = rightside2;
	//printf("%c->%c%c\n",leftside, rightside1, rightside2);
	gr->production_function->size++;
}