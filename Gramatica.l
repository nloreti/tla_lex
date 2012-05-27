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
int err,pid;
FILE * file;
	FILE* fileToOpen= fopen(argv[1], "r");
	if(fileToOpen== NULL){
		printf("Error al abrir el archivo\n");
		return 1;
	}
	else
		yyin=fileToOpen;
	
	createGramatica();	
	yylex();
	
	showTerminalSymbols();
	showNonTerminalSymbols();
	showInitialSymbol();
	showProductionSymbols();
	
	//Aca empiezo
	eliminateInproductive();
	
	eliminateUnreach();

	left_eliminateUnitaries();
	eliminateUnreach();
	
	showTerminalSymbols();
	showNonTerminalSymbols();
	showInitialSymbol();
	showProductionSymbols();
	
	
	if ( isLeft() == 1 ){
		left_normalize();
	}else if ( isLeft() == 0){
		right_normalize();
	}else{
		printf("Error en la gramatica\n");
	}
	printf("La gramatica es Correcta\n\n");
			
	showProductionSymbols();
	
	//Armo el Archivo;
	makeDotFile(file);
	
	//Grafico
	if ( (pid = fork()) < 0 ){
			perror("fork");
			exit(1);
	}
	if (pid == 0){
			system("/usr/local/bin/dot -Tpng file.dot -o graph.png");
			exit(1);
	}
}

int addProductions(char leftside, char rightside1, char rightside2){
	int size = gramatica->production_function->size;
	gramatica->production_function->productions[size].leftsimbol = leftside;
	gramatica->production_function->productions[size].rightsimbols[0] = rightside1;
	gramatica->production_function->productions[size].rightsimbols[1] = rightside2;
	gramatica->production_function->size = size + 1;
	return 1;
}
int isLeft(){
	int resp = -1;
	int k;
	
	for (k = 0; k < gramatica->production_function->size; k++){
			if ( gramatica->production_function->productions[k].leftsimbol != 0 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				char sright = gramatica->production_function->productions[k].rightsimbols[1];
				char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
				if ( (fright >= 'a' && fright <= 'z') && sright != -1 && resp == -1){
					resp = 1;
				}else if( (fright >= 'A' && fright <= 'Z') && sright != -1 && resp == -1){
					resp = 0;
				}else if( (fright >= 'A' && fright <= 'Z') && sright != -1 && resp == 1){
						resp = -1;
						return resp;
				}else if( (fright >= 'a' && fright <= 'z') && sright != -1 && resp == 0){
							resp = -1;
							return resp;
				}
			}
	}
	
	return resp;
}

void right_normalize(){
	int j,k,i;
	
	for (k = 0; k < gramatica->production_function->size; k++){
			if ( gramatica->production_function->productions[k].leftsimbol != 0 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				char sright = gramatica->production_function->productions[k].rightsimbols[1];
				char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
				if ( ( (fright >= 'a' && fright <= 'z') && sright == 0)){
					gramatica->production_function->productions[k].rightsimbols[1] = gramatica->production_function->productions[k].rightsimbols[0];
					gramatica->production_function->productions[k].rightsimbols[0] = 'M';
				}
			}
	}
	rotate_productions();
//	addProductions('M','/',-1);
	

}

void rotate_productions(){
	int k;
	
	for (k = 0; k < gramatica->production_function->size; k++){
			if ( gramatica->production_function->productions[k].leftsimbol != 0 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				char sright = gramatica->production_function->productions[k].rightsimbols[1];
				char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
				gramatica->production_function->productions[k].leftsimbol = fright;
				gramatica->production_function->productions[k].rightsimbols[0] = sright;
				gramatica->production_function->productions[k].rightsimbols[1] = left;
			}
	}
	gramatica->initial = 'M';
	addProductions('A','\\',0);
}
void left_normalize(){
	int j,k,i;
	
	for (k = 0; k < gramatica->production_function->size; k++){
			if ( gramatica->production_function->productions[k].leftsimbol != 0 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				char sright = gramatica->production_function->productions[k].rightsimbols[1];
				char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
				if ( ( (fright >= 'a' && fright <= 'z') && sright == 0)){
					gramatica->production_function->productions[k].rightsimbols[1] = 'M';
				}
			}
	}
	addProductions('M','\\',0);
	

}

void left_eliminateUnitaries(){
	int i,j,k;
	
	for (k = 0; k < gramatica->production_function->size; k++){
		if ( gramatica->production_function->productions[k].leftsimbol != 0 ){
			char left = gramatica->production_function->productions[k].leftsimbol;
			char sright = gramatica->production_function->productions[k].rightsimbols[1];
			char fright = gramatica->production_function->productions[k].rightsimbols[0];
		
			if( fright >= 'A' && fright <= 'Z' && sright == 0){
				for (j = 0; j < gramatica->production_function->size; j++){
					if ( gramatica->production_function->productions[j].leftsimbol == fright ){
						char first_right = gramatica->production_function->productions[j].rightsimbols[0];
						char scnd_right = gramatica->production_function->productions[j].rightsimbols[1];
						addProductions(left,first_right,scnd_right);
					}
				}
					//Elimino la unitaria que está demas
					gramatica->production_function->productions[k].leftsimbol = 0;
			}
		
		}
	}
}
void eliminateUnreach(){
	int k,j,h;
	char * old = malloc(sizeof(char)*15);
	char * new = malloc(sizeof(char)*15);
	char newSymbol;
	//i=0;
	j=0;
	
	//old[i++] = gramatica->initial;
	new[j++] = gramatica->initial;
	
		for( h = 0; h < j; h++){
			newSymbol = new[h];
			for (k = 0; k < gramatica->production_function->size; k++){
				if ( gramatica->production_function->productions[k].leftsimbol != 0 ){
					char left = gramatica->production_function->productions[k].leftsimbol;
					char sright = gramatica->production_function->productions[k].rightsimbols[1];
					char fright = gramatica->production_function->productions[k].rightsimbols[0];

					if ( left == newSymbol ){
						if( sright >= 'A' && sright <= 'Z'){
							if( !isContained(sright,new,j) ){
								new[j] = sright;
								j++;
							}
						}
						if( fright >= 'A' && fright <= 'Z'){
							if( !isContained(fright,new,j) ){
								new[j] = fright; 
								j++;
							}
						}
					}
				}
			}
		}
		
		for (k = 0; k < gramatica->production_function->size; k++){
			if ( gramatica->production_function->productions[k].leftsimbol != 0 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				if ( !isContained(left,new,j) ){
					gramatica->production_function->productions[k].leftsimbol = 0;
				}
			}
		}
}

void eliminateInproductive(){
	int k,j,i;
	
	char * new = malloc(sizeof(char)*15);
	j=0;
	
	//Cargo los primeros;
		for (k = 0; k < gramatica->production_function->size; k++){
			if ( gramatica->production_function->productions[k].leftsimbol != 0 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				char sright = gramatica->production_function->productions[k].rightsimbols[1];
				char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
				if ( ( (fright >= 'a' && fright <= 'z') && sright == 0) || fright == '\\'){
					if ( !isContained(left,new,j) ){
						new[j] = left;
						j++;
					}
				}
			}
		}
	//Recorro a partir de los primeros para llegar a los que son productivos.	
	for( i = 0; i<j; i++){
		char newSymbol = new[i];
		for (k = 0; k < gramatica->production_function->size; k++){
				if ( gramatica->production_function->productions[k].leftsimbol != 0 ){
					char left = gramatica->production_function->productions[k].leftsimbol;
					char sright = gramatica->production_function->productions[k].rightsimbols[1];
					char fright = gramatica->production_function->productions[k].rightsimbols[0];
				
					if ( fright == newSymbol){
						if ( !isContained(left,new,j)){
							new[j] = left;
							j++;
						}
					}
					
					if ( sright == newSymbol){
						if ( !isContained(left,new,j)){
							new[j] = left;
							j++;
						}
					}
					
				}
		}			
	}
	
	//Elimino los que no estan contenidos en new o estan contenidos en inproductivos;
	char * inproductivos = malloc ( sizeof(char) * 15);
	int size = 0;
	for ( k = 0; k < gramatica->terminals->size; k++){
		if ( !isContained(gramatica->terminals->terminal[k], new, j) ){
			inproductivos[size] = gramatica->terminals->terminal[k];
			size++;
		}
	}
	
	for (k = 0; k < gramatica->production_function->size; k++){
		if ( gramatica->production_function->productions[k].leftsimbol != 0){
			char left = gramatica->production_function->productions[k].leftsimbol;
			char sright = gramatica->production_function->productions[k].rightsimbols[1];
			char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
			if ( (isContained(fright,inproductivos,size) || isContained(sright,inproductivos, size) || isContained(left,inproductivos,size)) ){
				gramatica->production_function->productions[k].leftsimbol = 0;
			}
		}
	}	
	
}

int isContained(char left, char * new, int size){
	int i;
	
	for ( i = 0; i < size; i++){
		if ( left == new[i]){
			return 1;
		}
	}
	
	return 0;
	
}


void makeDotFile(FILE * file){
	int i,j,k;
	//Crear el archivo.
	file=fopen("file.dot","w+");
	//Insertar lineas.
	fputs("digraph{\n",file);
	//Inserta no terminales
	i=0;
	for (j = 0; j < gramatica->production_function->size; j++){
		if ( gramatica->production_function->productions[j].leftsimbol != 0 &&  gramatica->production_function->productions[j].rightsimbols[0] != '\\'){
		
		fputs("node[shape=circle] ",file);
		fputc( gramatica->production_function->productions[j].leftsimbol,file);
		fputs(" [label=\"",file);
		fputc((char)i+'0',file);
		fputs("\"];\n",file);
		i++;
		}	
	}
	//Pongo los finales 
	for (j = 0; j < gramatica->production_function->size; j++){
		if ( gramatica->production_function->productions[j].leftsimbol != 0 &&  gramatica->production_function->productions[j].rightsimbols[0] == '\\'){
		
		fputs("node[shape=doublecircle] ",file);
		fputc( gramatica->production_function->productions[j].leftsimbol,file);
		fputs(" [label=\"",file);
		fputc((char)i+'0',file);
		fputs("\"];\n",file);
		i++;
		}	
	}
//	fputs("node[shape=doublecircle] ",file);
//	fputc('M',file);
//	fputs(" [label=\"",file);
//	fputc((char)i+'0',file);
//	fputs("\"];\n",file);
	
	
	//Transiciones
	for (k = 0; k < gramatica->production_function->size; k++){
		if ( gramatica->production_function->productions[k].leftsimbol != 0 &&  gramatica->production_function->productions[k].rightsimbols[0] != '\\'){
			char left = gramatica->production_function->productions[k].leftsimbol;
			char right = gramatica->production_function->productions[k].rightsimbols[1];
			char label = gramatica->production_function->productions[k].rightsimbols[0];
			fputc(left,file);
			fputs("->",file);
			fputc(right,file);
			fputs(" [label=\"",file);
			fputc(label,file);
			fputs("\"];\n",file);	
			i++;	
		}
	}
	fputs("}\n",file);
	
	return;
	
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





 