#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "./TPE/structs.h"

#define MAXLENGHT 15

void generateASDR();
gramatica_struct * gramatica;
production_struct * getProductionsNonTerminal(production_struct * productions, char nonTerminal , int * quant);

char leftPart=0;

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
	addInitialSymbols("A");

	addProductionSymbols("{A -> aB|c, B -> aA|b}");
//	addProductionSymbols("{ A -> bB|C, B -> bC|bB, C->c, D->B}");
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
	//makeDotFile(file);
	generateASDR();
	//Grafico
//	if ( (pid = fork()) < 0 ){
//			perror("fork");
//			exit(1);
//	}
//	if (pid == 0){
//			system("/usr/local/bin/dot -Tpng file.dot -o graph.png");
//			exit(1);
//	}	
}

/*Conversion*/
void generateASDR() {

	FILE * file;
	char * name = "ASDR.c";
	file = fopen(name, "w+");
	char * nonTerminals = gramatica->nonterminals->nonterminals;
	int i;
	production_struct ** productionsNT = malloc(sizeof(production_struct*) * gramatica->nonterminals->size);
	int * prodSize = malloc(sizeof(int) * gramatica->nonterminals->size);
//	int * nonTerminals = malloc(sizeof(int) * gramatica->nonterminals->size));
	
	for ( i = 0; i < gramatica->nonterminals->size; i++) {
		productionsNT[i] = getProductionsByTerminal(gramatica->production_function, nonTerminals[i], &(prodSize[i]));
	}

	//Empieza la creeacion del archivo
	if (file) {

		printf("------------generating headers---------------\n");
		fprintf(file, "#include <stdio.h>\n");
		fprintf(file, "#include <stdlib.h>\n");
		fprintf(file, "#include <string.h>\n");
		//fprintf(file, "#include \"include/Production.h\"\n");
		//fprintf(file, "#include \"include/utils.h\"\n");
		fprintf(file, "#define TRUE 1\n");
		fprintf(file, "#define FALSE 0\n");

		int k,j;
		for (k = 0; k < gramatica->nonterminals->size; k++) {
			fprintf(file, "production_struct * prods%d;\n", k);
			fprintf(file, "int prods%dquant;\n", k);
			fprintf(file, "int pn%d(int *, char * w);\n",k);
		}
		fprintf(file, "int procesar(production_struct p, char * w, int * t);\n");
		fprintf(file, "void init();\n");

		printf("------------generating main---------------\n");

		fprintf(file, "\nint main(int argc, char *argv[]) {\n");
		fprintf(file, "\tif(argc!=2) {\n");
		fprintf(file, "\t\tprintf(\"Mal uso del programa, se debería usar : %%s string \\n \", argv[0]);\n");
		fprintf(file, "\t\texit(1);\n");
		fprintf(file, "\t} else {\n");
		fprintf(file, "\t\tprintf(\"Procesando la cadena %%s\\n\", argv[1]);\n\t}\n");
		fprintf(file, "\tchar * w = argv[1];\n");
		fprintf(file, "\tinit();\n");
		fprintf(file, "\tint t = 0;\n");
		fprintf(file, "\tif(!pn0(&t,w) && (t==strlen(w))){\n");
		fprintf(file, "\t\tprintf(\"(The chain of used derivations is upside down)\\n\");\n");
		fprintf(file, "\t\tprintf(\"The string has been accepted \\n\");\n");
		fprintf(file, "\t} else {\n");
		fprintf(file, "\t\tprintf(\"The string has NOT been accepted\\n\");\n");
		fprintf(file, "\t}\n");
		fprintf(file, "\treturn 0;\n");
		fprintf(file, "}\n");
		/*main*/

//		printf("------------generating init---------------\n");

		/*init*/

		fprintf(file, "\n/*initializes all the structures*/\n");
		fprintf(file, "void init(){\n");

		for (k = 0; k < gramatica->nonterminals->size; k++) {
			production_struct * ps = productionsNT[k];
			fprintf(file, "\tprods%d = malloc(sizeof(production_struct *) * %d);\n",k, prodSize[k]);
			fprintf(file, "\tprods%dquant = %d;\n", k, prodSize[k]);
			int h;
			for (h = 0; h < prodSize[h]; h++) {
				production_struct p = ps[h];
				fprintf(file, "\tprods%d[%d] = newProduction(\'%c\', \"%s\");\n", k, h, p.leftsimbol, p.rightsimbols);
			}
		}
		fprintf(file, "}\n");
		/*init*/
		
		printf("------------generating procesar---------------\n");

		/*procesar*/
		fprintf(file, "\n/*returns TRUE if there is an error*/\n");
		fprintf(file, "int procesar(production_struct p, char * w, int * t) {\n");
		fprintf(file, "\tint i;\n");
		fprintf(file, "\tint n = p.size; /*number of symbols in the right side of the production*/\n");
		fprintf(file, "\tfor (i = 1; i < n; i++) {\n");
		fprintf(file, "\t\tchar comp = getProductionComponent(p, i);\n");
		fprintf(file, "\t\tif (isTerminal(comp)) {\n");
		fprintf(file, "\t\t\tif (w[*t] == comp) {\n");
		fprintf(file, "\t\t\t\t(*t) += 1;\n");
		fprintf(file, "\t\t\t} else {\n");
		fprintf(file, "\t\t\t\treturn TRUE;/*error*/\n\t\t\t}\n");
		fprintf(file, "\t\t} else {\n");
		fprintf(file, "\t\t\tint error= FALSE;\n");
		fprintf(file, "\t\t\tswitch(comp){\n");

		for (j = 0; j < gramatica->nonterminals->size; j++) {
			fprintf(file, "\t\t\tcase '%c':\n", nonTerminals[j]);
			fprintf(file, "\t\t\t\terror = pn%d(t,w);\n", j);
			fprintf(file, "\t\t\t\tbreak;\n");
		}
		fprintf(file, "\t\t\t}\n");
		fprintf(file, "\t\t\tif(error){\n");
		fprintf(file, "\t\t\t\treturn TRUE;\n");
		fprintf(file, "\t\t\t}\n\t\t}\n\t}\n");
		fprintf(file, "\treturn FALSE;\n}\n");


		//TODO: debug
		printf("------------procedures generator---------------\n");

		/*procedures for each non terminal*/
		/*corresponding to S*/
		for (j = 0; j < gramatica->nonterminals->size; j++) {
			fprintf(file, "\nint pn%d(int *t, char * w) {\n", j);
			fprintf(file, "\tint j;\n");
			fprintf(file, "\tint n = prods%dquant; /*quantity of productions with this termina */\n", j);
			fprintf(file, "\tint error = TRUE;\n");
			fprintf(file, "\tfor(j = 0; j < n && error; j++) {\n");
			fprintf(file, "\t\terror = procesar(prods%d[j], w, t);\n", j);
			fprintf(file, "\t\tif(!error){\n");
			fprintf(file, "\t\t\tprintProduction(prods%d[j]);\n", j);
			fprintf(file, "\t\t}\n\t}\n");
			fprintf(file, "\treturn error;\n}\n");
		}

		fprintf(file, "\treturn error;\n}\n");
		//TODO: debug
		
		
		printf("------------writing file---------------\n");

		fclose(file);
	}
}


production_struct * getProductionsNonTerminal(production_struct * productions, char nonTerminal , int * quant){
	production_struct * ans = malloc(sizeof(production_struct *));
	int i,k=0;
	for(i=0; i<gramatica->production_function->size;i++){
		production_struct p = gramatica->production_function->productions[i];
		char first = p.leftsimbol;
		if (first == nonTerminal){
			ans[k++] = p;
		}
	}
	(*quant) = k;
	return ans;
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
			
				if ( (fright >= 'a' && fright <= 'z') && sright != 0 && resp == -1){
					resp = 1;
				}else if( (fright >= 'A' && fright <= 'Z') && sright != 0 && resp == -1){
					resp = 0;
				}else if( (fright >= 'A' && fright <= 'Z') && sright != 0 && resp == 1){
						resp = -1;
						return resp;
				}else if( (fright >= 'a' && fright <= 'z') && sright != 0 && resp == 0){
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
	char * aux = malloc(gramatica->production_function->size * sizeof(char));
	int auxSize = 0;
	//Crear el archivo.
	file=fopen("file.dot","w");
	fseek(file, 0, SEEK_SET);
	//Insertar lineas.
	fputs("digraph{\n",file);
	//Inserta no terminales
	i=0;
	//Inserto Inicial
	fputs("node[shape=circle] ",file);
	fputc( gramatica->initial,file);
	fputs(" [label=\"",file);
	fputc((char)i+'0',file);
	fputs("\"];\n",file);
	i++;
		
	for (j = 0; j < gramatica->production_function->size; j++){
		char leftValue = gramatica->production_function->productions[j].leftsimbol;
		if ( leftValue != 0 &&  gramatica->production_function->productions[j].rightsimbols[0] != '\\' && leftValue != gramatica->initial && !isContained(leftValue,aux,auxSize)){
		aux[auxSize] = leftValue;
		auxSize++;
		fputs("node[shape=circle] ",file);
		fputc( leftValue,file);
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
		if(gramatica->production_function->productions[i].leftsimbol != 0){
			printf("%c -> %c%c\n", gramatica->production_function->productions[i].leftsimbol, gramatica->production_function->productions[i].rightsimbols[0],gramatica->production_function->productions[i].rightsimbols[1]);
		}
	}
	printf("End\n\n");
}