%{ 
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "./TPE/structs.h"
#define MAXLENGHT 30

automata_struct * automata;
gramatica_struct * gr;
int final=0;

char lastLeftLabel=0;
char lastRightLabel=0;

%} 
%x PARSE
%x NODE
%x STARTLABEL
%x GETLABEL
%x ENDLABEL
%x LEFTTRANSITION
%x RIGHTTRANSITION
%x ARROW
%x STARTLABELTRANSITION
%x GETLABELTRANSITION
%x LABELSEPARATOR
%x GETANOTHERLABELTRANSITION
%x ENDDEFINITION

%%
\n ;
. ;
<PARSE,INITIAL,LEFTTRANSITION>\/\/.+\n? ;
<PARSE,INITIAL,LEFTTRANSITION>\n ;
<PARSE,INITIAL>node\[shape=doublecircle\] {final=1;};BEGIN(NODE);
<PARSE,INITIAL>node\[shape=circle\] {final=0;};BEGIN(NODE);
<NODE>([A-Z]|[a-z]|[0-9])+ {insertNode(yytext,yyleng);}; BEGIN(STARTLABEL);
<STARTLABEL>\[label=\" BEGIN(GETLABEL);
<GETLABEL>([a-z]|[A-z]|[0-9]) {getLabel(*yytext);};BEGIN(ENDLABEL);
<ENDLABEL>\"\]; BEGIN(PARSE);

<PARSE,LEFTTRANSITION>([A-Za-z0-9])+ {getTransitionLeftPart(yytext);}BEGIN(ARROW);
<ARROW>-> BEGIN(RIGHTTRANSITION);
<RIGHTTRANSITION>([A-Z]|[a-z]|[0-9])+ {getTransitionRightPart(yytext);}BEGIN(STARTLABELTRANSITION);
<STARTLABELTRANSITION>\[label=\" BEGIN(GETLABELTRANSITION);
<STARTLABELTRANSITION>[^((\[label=\")|" "|\t|\n)] {printf("BAD FORMAT\n"); exit(1);}
<GETLABELTRANSITION>([a-z]|\\\\) {getLabelTransition(*yytext);}BEGIN(LABELSEPARATOR);
<LABELSEPARATOR>\/ BEGIN(GETANOTHERLABELTRANSITION);
<GETANOTHERLABELTRANSITION>([a-z]|\\\\) {createNewTransition(lastLeftLabel,lastRightLabel,*yytext);}BEGIN(LABELSEPARATOR);
<LABELSEPARATOR>\"\]\; BEGIN(LEFTTRANSITION);

<LEFTTRANSITION>\}\n? BEGIN(ENDDEFINITION);

%%



int createNewTransition(char leftLabel,char rightLabel, char transition){
	int size =automata->delta->size;
	if(size%BLOCK==0)
		automata->delta->delta=realloc(automata->delta->delta,(size+BLOCK)*sizeof(delta_struct*));
	automata->delta->delta[size]=malloc(sizeof(delta_struct));
	automata->delta->delta[size]->initialnode=leftLabel;
	automata->delta->delta[size]->finalnode=rightLabel;
	automata->delta->delta[size]->transition=transition;
	automata->delta->size=size+1;
	
}


int
main(int argc, char* argv[]){
	FILE* fileToOpen= fopen(argv[1], "r");
	if(fileToOpen== NULL){
		printf("Error al abrir el archivo\n");
		return 1;
	}else
		yyin=fileToOpen;

	createAutomata();
	yylex();
	showNodes();
	showFinals();
	showTransitions();
	showLenguage();
	gr_init();
	afdToGr();
	gr_print();
	gr_toFile();
}



char getLabelForTrans(char* name){

	int i=0;
	for(i ; i<automata->nodes->size; i++){
		
		if(strcmp(name,automata->nodes->nodes[i]->name)==0){
			
			return automata->nodes->nodes[i]->label;
		}	
	}
	return 0;

}

int getTransitionLeftPart(char* c){
	
	int label=getLabelForTrans(c);
	if(label==0){
		printf("No existe tal Nodo\n");
		//exit(1);
	}
	
	int size =automata->delta->size;
	if(size%BLOCK==0)
		automata->delta->delta=realloc(automata->delta->delta,(size+BLOCK)*sizeof(delta_struct*));
	
	automata->delta->delta[size]=malloc(sizeof(delta_struct));
	automata->delta->delta[size]->initialnode=label;
	lastLeftLabel=label;

	return 1;
}

int getTransitionRightPart(char* c){
	
	int size =automata->delta->size;
	int label=getLabelForTrans(c);
	if(label==0){
		printf("No existe tal Nodo\n");
		exit(1);
	}
	
	automata->delta->delta[size]->finalnode=label;
	lastRightLabel=label;
}

int getLabelTransition(char c){
	
	int size =automata->delta->size;
	
	automata->delta->delta[size]->transition=c;
	
	automata->delta->size=size+1;
	
	addLabelToLenguage(c);
	return 1;
}

int addLabelToLenguage(char c){
	if(c == '\\')
		return 1;
	if(!labelExist(c))
		addLabel(c);	
	return 1;
}

int addLabel(char c){
	int size=automata->language->size;
	if(size%BLOCK ==0)
		automata->language->language=realloc(automata->language->language,(size+BLOCK)*sizeof(char));
	automata->language->language[size]=c;
	
	automata->language->size=size+1;

}

int labelExist(char c){
	int i=0;
	for(i;i<automata->language->size;i++){
		if(automata->language->language[i] == c){
		return 1;
		}
	}
	return 0;
}

int insertNode(char* name, int lenght){
	
	int size=automata->nodes->size;
	if(size%BLOCK==0)
		automata->nodes->nodes=realloc(automata->nodes->nodes,(size+BLOCK)*sizeof(node_struct *));
	automata->nodes->nodes[size]=malloc(sizeof(node_struct));
	automata->nodes->nodes[size]->name=malloc(lenght+1);
	strcpy(automata->nodes->nodes[size]->name,name);
	automata->nodes->size=size+1;
	
	return 1;
}

int getLabel(char c){
	
	int pos =automata->nodes->size-1;
	automata->nodes->nodes[pos]->label=c;
	if(final)
		insertFinal(c);
	if(c == '0')
		automata->initial= c;
	return 1;
}	

int insertFinal(char c){
	int size=automata->finals->size;
	if(size%BLOCK==0)
		automata->finals->nodes=realloc(automata->finals->nodes,(size+BLOCK)*sizeof(char));
	automata->finals->nodes[size]=c;
	automata->finals->size=size+1;
	return 1;

}
	

void reportError(char* errorMsg, char* token){

	printf("Se produjo un error tratando de leer %s cuando se leyo %s",errorMsg,token);
	abort();

}

int createAutomata(){

	automata=malloc(sizeof(automata_struct));
	initializeNodes();
	initilizeFinals();
	initializeLanguage();
	initializeDelta();
	return 1;	

 }

int initializeNodes(){

	automata->nodes=malloc(sizeof(nodes_struct));
	automata->nodes->nodes=NULL;
	automata->nodes->size=0;
	return 1;
}

int initilizeFinals(){

	automata->finals=malloc(sizeof(finals_struct));
	automata->finals->size=0;
	automata->finals->nodes=NULL;
	return 1;
}
int initializeLanguage(){
	
	automata->language=malloc(sizeof(language_struct));
	automata->language->language=NULL;
	automata->language->size=0;
	return 1;
}

int initializeDelta(){

	automata->delta=malloc(sizeof(delta_struct));
	automata->delta->size=0;
	automata->delta->delta=NULL;
	return 1;
}

void showNodes(){
	int i =0;
	printf("---------NODES-----------\n");
	printf("hay %d Nodos \n",automata->nodes->size);
	for(i=0; i<automata->nodes->size; i++){
		printf("%s con label %c \n",automata->nodes->nodes[i]->name,automata->nodes->nodes[i]->label);
		}
}

void showFinals(){
	int i=0;
	printf("--------FINALES-----------\n");
	printf("Hay %d Finales\n",automata->finals->size);
	for(i=0; i<automata->finals->size; i++){
		printf("label %c\n",automata->finals->nodes[i]);
	}
}

void showTransitions(){
	int i=0;
	printf("---------------TRANSITIONS----------\n");
	printf("Hay %d Transisiones\n",automata->delta->size);
	for(i=0; i<automata->delta->size; i++){
		printf("%c -> %c (%c) \n",automata->delta->delta[i]->initialnode,automata->delta->delta[i]->finalnode,automata->delta->delta[i]->transition);
	}

}

void showLenguage(){
	int i=0;
	printf("----------------LENGUAGE----------------\n");
	printf("Hay %d elementos en el Lenguaje\n",automata->language->size);

	for(i=0; i<automata->language->size; i++){
		printf("%c  ",automata->language->language[i]);
	}
	printf("\n");

}


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
	return automata->language->language;
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
	char * string = malloc((automata->nodes->size + 1) * sizeof(char));
	
	for(i = 0; i<automata->nodes->size; i++ ){
		string[i] = automata->nodes->nodes[i]->label;
	}
	return string;
}

int au_getAlphabetLength(){
	return automata->language->size;
}

char au_move(char state, char symbol){
	int i = 0;
	
	for( i=0; i<automata->delta->size; i++ ){
		//printf("Checkin if %c == %c and %c == %c\n", state, automata->delta->delta[i]->initialnode, symbol, automata->delta->delta[i]->transition);
		if( automata->delta->delta[i]->initialnode == state && automata->delta->delta[i]->transition == symbol )
			return automata->delta->delta[i]->finalnode;
	}
	return 0;
}

char au_getState(int index){
	return automata->nodes->nodes[index]->label;
}

char au_getSymbol(int index){
	return automata->language->language[index];
}

int au_isFinal(char state){
	int i = 0;
	for( i=0; i<automata->finals->size; i++ ){
		if( automata->finals->nodes[i] == state ){
			return 1;
		}
	}
	return 0;
}

char au_getInitial(){
	return automata->initial;
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
	return automata->nodes->size;
}


void gr_addProduction(char leftside, char rightside1, char rightside2){
	int size = gr->production_function->size;
	gr->production_function->productions[size].leftsimbol = leftside;
	gr->production_function->productions[size].rightsimbols[0] = rightside1;
	gr->production_function->productions[size].rightsimbols[1] = rightside2;
	//printf("%c->%c%c\n",leftside, rightside1, rightside2);
	gr->production_function->size++;
}
	