%{ 
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "./TPE/structs.h"

gramatica_struct * gramatica;
%} 
%x DEFINITION
%x TERMINALS
%x ENDTERMINALS
%x NONTERMINALS
%x ENDNONTERMINALS
%x INITIALS
%x ENDINITIALS
%x PRODUCTIONS
%x ENDPRODUCTIONS
%x ENDDEFINITION
%x TERMSEPARATOR
%x NONTERMSEPARATOR
%x PRODSEPARATOR

%%

\( BEGIN(DEFINITION);
<DEFINITION>\{ ;BEGIN(TERMINALS);
<TERMINALS>[^(,|}|" ")] {printf("el terminal es %s\n",yytext);}BEGIN(TERMSEPARATOR);
<TERMSEPARATOR>[^(,|" "|\t|\})] {reportError("terminales",yytext);};
<TERMSEPARATOR>, BEGIN(TERMINALS);
<TERMINALS,TERMSEPARATOR>\} BEGIN(ENDTERMINALS);
<ENDTERMINALS>\{ BEGIN(NONTERMINALS);
<NONTERMINALS>[^(,|}|" ")] {printf("el no terminal es %s\n",yytext);}BEGIN(NONTERMSEPARATOR);
<NONTERMSEPARATOR>[^(,|" "|\t|\})] {reportError("no terminales", yytext);};
<NONTERMSEPARATOR>, BEGIN(NONTERMINALS);
<NONTERMINALS,NONTERMSEPARATOR>\} BEGIN(ENDNONTERMINALS);
<ENDNONTERMINALS>, ;BEGIN(INITIALS);
<INITIALS>[^(" "|,|\n|\t)] {printf("El inicial es %s\n",yytext);};
<INITIALS>, ;BEGIN(ENDINITIALS);
<ENDINITIALS>\{ BEGIN(PRODUCTIONS);
<PRODUCTIONS>([A-Z]" "?)->(" "?((\\|([a-z]|[A-Z])+)" "?\|" "?)*(" "?\\|([a-z]|[A-Z])+)" "?) {printf("la produccion es %s\n",yytext);} BEGIN(PRODSEPARATOR);
<PRODSEPARATOR>, BEGIN(PRODUCTIONS);
<PRODSEPARATOR>[^(,|" "|\t|\})] {reportError("producciones", yytext);};
<PRODUCTIONS,PRODSEPARATOR>\} BEGIN(ENDPRODUCTIONS);
<ENDPRODUCTIONS>\) BEGIN(ENDDEFINITION);
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
	yylex();

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

	printf("Se produjo un error tratando de leer %s cuando se leyo %s",errorMsg,token);
	abort();

}
 