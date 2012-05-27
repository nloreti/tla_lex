#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "./TPE/structs.h"

#define MAXLENGHT 15

gramatica_struct * gramatica;


int
main(){	
	int err,pid;
	FILE * file;
	
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
	addProductionSymbols("{ A -> bB|C, B -> bC|bB, C->c, D->B}");
//	addProductionSymbols("{ A -> Bb|C, B -> Bb|Cb, C->c}");
	
	//Si A->C (Entonces elimino unitarias)
	
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

int isLeft(){
	int resp = -1;
	int k;
	
	for (k = 0; k < gramatica->production_function->size; k++){
			if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
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
			if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				char sright = gramatica->production_function->productions[k].rightsimbols[1];
				char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
				if ( ( (fright >= 'a' && fright <= 'z') && sright == -1)){
					gramatica->production_function->productions[k].rightsimbols[1] = gramatica->production_function->productions[k].rightsimbols[0];
					gramatica->production_function->productions[k].rightsimbols[0] = 'M';
				}
			}
	}
	rotate_productions();
//	addProduction('M','/',-1);
	

}

void rotate_productions(){
	int k;
	
	for (k = 0; k < gramatica->production_function->size; k++){
			if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				char sright = gramatica->production_function->productions[k].rightsimbols[1];
				char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
				gramatica->production_function->productions[k].leftsimbol = fright;
				gramatica->production_function->productions[k].rightsimbols[0] = sright;
				gramatica->production_function->productions[k].rightsimbols[1] = left;
			}
	}
	gramatica->initial = 'M';
	addProduction('A','\\',-1);
}
void left_normalize(){
	int j,k,i;
	
	for (k = 0; k < gramatica->production_function->size; k++){
			if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				char sright = gramatica->production_function->productions[k].rightsimbols[1];
				char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
				if ( ( (fright >= 'a' && fright <= 'z') && sright == -1)){
					gramatica->production_function->productions[k].rightsimbols[1] = 'M';
				}
			}
	}
	addProduction('M','\\',-1);
	

}

void left_eliminateUnitaries(){
	int i,j,k;
	
	for (k = 0; k < gramatica->production_function->size; k++){
		if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
			char left = gramatica->production_function->productions[k].leftsimbol;
			char sright = gramatica->production_function->productions[k].rightsimbols[1];
			char fright = gramatica->production_function->productions[k].rightsimbols[0];
		
			if( fright >= 'A' && fright <= 'Z' && sright == -1){
				for (j = 0; j < gramatica->production_function->size; j++){
					if ( gramatica->production_function->productions[j].leftsimbol == fright ){
						char first_right = gramatica->production_function->productions[j].rightsimbols[0];
						char scnd_right = gramatica->production_function->productions[j].rightsimbols[1];
						addProduction(left,first_right,scnd_right);
					}
				}
					//Elimino la unitaria que está demas
					gramatica->production_function->productions[k].leftsimbol = -1;
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
				if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
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
			if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				if ( !isContained(left,new,j) ){
					gramatica->production_function->productions[k].leftsimbol = -1;
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
			if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
				char left = gramatica->production_function->productions[k].leftsimbol;
				char sright = gramatica->production_function->productions[k].rightsimbols[1];
				char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
				if ( ( (fright >= 'a' && fright <= 'z') && sright == -1) || fright == '\\'){
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
				if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
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
		if ( gramatica->production_function->productions[k].leftsimbol != -1 ){
			char left = gramatica->production_function->productions[k].leftsimbol;
			char sright = gramatica->production_function->productions[k].rightsimbols[1];
			char fright = gramatica->production_function->productions[k].rightsimbols[0];
			
			if ( (isContained(fright,inproductivos,size) || isContained(sright,inproductivos, size) || isContained(left,inproductivos,size)) ){
				gramatica->production_function->productions[k].leftsimbol = -1;
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
		if ( gramatica->production_function->productions[j].leftsimbol != -1 &&  gramatica->production_function->productions[j].rightsimbols[0] != '\\'){
		
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
		if ( gramatica->production_function->productions[j].leftsimbol != -1 &&  gramatica->production_function->productions[j].rightsimbols[0] == '\\'){
		
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
		if ( gramatica->production_function->productions[k].leftsimbol != -1 &&  gramatica->production_function->productions[k].rightsimbols[0] != '\\'){
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
				addProduction(left,fright,sright);
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
	gramatica->production_function->productions[size].rightsimbols[1] = rightside2;
	gramatica->production_function->size = size + 1;
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
	gramatica->production_function->productions=malloc(sizeof(production_struct)*MAXLENGHT);
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

void showProductionSymbols(){
	int i;
	printf("Producciones\n");
	for( i = 0; i < gramatica->production_function->size; i++){
		printf("%c -> %c%c\n", gramatica->production_function->productions[i].leftsimbol, gramatica->production_function->productions[i].rightsimbols[0],gramatica->production_function->productions[i].rightsimbols[1]);
	}
	printf("End\n\n");
}