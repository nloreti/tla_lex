%{ 
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "./TPE/structs.h"

gramatica_struct * gramatica;
char leftPart=0;
%} 
%x DEFINITION
%x TERMINALS
%x ENDTERMINALS
%x NONTERMINALS
%x ENDNONTERMINALS
%x INITIALS
%x ENDINITIALS
%x LEFTPRODUCTION
%x RIGHTPRODUCTION
%x ENDPRODUCTIONS
%x ENDDEFINITION
%x TERMSEPARATOR
%x NONTERMSEPARATOR
%x PRODSEPARATOR
%x ARROW
%x ERROR

%%

\( BEGIN(DEFINITION);
<DEFINITION>\{ ;BEGIN(TERMINALS);
<TERMINALS>[A-Z] {addTerminal(*yytext);}BEGIN(TERMSEPARATOR);
<TERMSEPARATOR>[^(,|" "|\t|\})] {reportError("terminales",yytext);}; BEGIN(ERROR);
<TERMSEPARATOR>, BEGIN(TERMINALS);
<TERMINALS,TERMSEPARATOR>\} BEGIN(ENDTERMINALS);
<ENDTERMINALS>(,|" ") ;
<ENDTERMINALS>\{ BEGIN(NONTERMINALS);
<NONTERMINALS>[a-z] {addNonTerminal(*yytext);}BEGIN(NONTERMSEPARATOR);
<NONTERMSEPARATOR>[^(,|" "|\t|\})] {reportError("no terminales", yytext);}; BEGIN(ERROR);
<NONTERMSEPARATOR>, BEGIN(NONTERMINALS);
<NONTERMINALS,NONTERMSEPARATOR>\} BEGIN(ENDNONTERMINALS);
<ENDNONTERMINALS>, ;BEGIN(INITIALS);
<INITIALS>[A-Z] {addInitialSymbol(*yytext);};
<INITIALS>, ;BEGIN(ENDINITIALS);
<ENDINITIALS>\{ BEGIN(LEFTPRODUCTION);
<LEFTPRODUCTION>[A-Z] {getProductionLeftPart(*yytext);}; BEGIN(ARROW);
<LEFTPRODUCTION>[^A-Z]|[^(" "|\t)] {reportError("una parte izquierda de producciones",yytext);};BEGIN(ERROR);
<ARROW>-> BEGIN(RIGHTPRODUCTION);
<RIGHTPRODUCTION>([A-Z][a-z]|[a-z][A-Z]|[a-z]|[A-Z]|\\) {getProduction(yytext,yyleng);}; BEGIN(PRODSEPARATOR);
<RIGHTPRODUCTION>(([A-Z]|[a-z]|\\){3,310}|[A-Z]{2,2}|[a-z]{2,2}) {reportError("una parte derecha de producciones", yytext);}; BEGIN(ERROR);	
<PRODSEPARATOR>, BEGIN(LEFTPRODUCTION);
<PRODSEPARATOR>\| BEGIN(RIGHTPRODUCTION);
<PRODSEPARATOR>\} BEGIN(ENDPRODUCTIONS);
<ENDPRODUCTIONS>\) BEGIN(ENDDEFINITION);
. ;
\n ;


%%

int
main(int argc, char* argv[]){
	FILE* fileToOpen= fopen(argv[1], "r");
	if(fileToOpen== NULL){
		printf("Error al abrir el archivo\n");
		yyin=stdin;}
	else
		yyin=fileToOpen;
	
	createGramatica();	
	yylex();
	
	showTerminalSymbols();
	 showNonTerminalSymbols();
	 showInitialSymbol();
	showProductionSymbols();
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
void reportError(char* errorMsg, char* token){

	printf("Se produjo un error tratando de leer %s cuando se leyo %s\n",errorMsg,token);
	exit(0);

}

int addTerminal(char c){
	int size = gramatica->terminals->size;
	if(size%BLOCK==0)
		gramatica->terminals->terminal=realloc(gramatica->terminals->terminal,(size+BLOCK)*sizeof(char));	
	gramatica->terminals->terminal[size] = c;
	gramatica->terminals->size = size+1;
	return 1;
}

int addNonTerminal(char c){
	int size = gramatica->nonterminals->size;
	if(size%BLOCK ==0)
		gramatica->nonterminals->nonterminals=realloc(gramatica->nonterminals->nonterminals,(size+BLOCK)*sizeof(char));
	gramatica->nonterminals->nonterminals[size] = c;
	gramatica->nonterminals->size = ++size;
	return 1;
}

int addInitialSymbol(char  c){
	gramatica->initial = c;
	return 1;
}

void getProductionLeftPart(char c){
	leftPart=c;
}

int getProduction(char* c,int lenght){
	int i=0;
	int size=gramatica->production_function->size;
	if(size%BLOCK==0)
		gramatica->production_function->productions=realloc(gramatica->production_function->productions, (size+BLOCK)*sizeof(production_struct));
	gramatica->production_function->productions[size].leftsimbol=leftPart;
	for(i=0;i< lenght;i++)
		gramatica->production_function->productions[size].rightsimbols[i]=c[i];
	
	gramatica->production_function->size=size+1;
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

void showProductionSymbols(){
	int i;
	printf("Producciones\n");
	for( i = 0; i < gramatica->production_function->size; i++){
		printf("%c -> %c%c\n", gramatica->production_function->productions[i].leftsimbol, gramatica->production_function->productions[i].rightsimbols[0],gramatica->production_function->productions[i].rightsimbols[1]);
	}
	printf("End\n\n");
}





 