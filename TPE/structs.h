//GRAFMATICA
typedef struct{
	char leftsimbol;
	char rightsimbols[2];
}production_struct;

typedef struct{
	production_struct* productions;
	int size;
}production_function_struct;

typedef struct{
	char * nonterminals;
	int size;
}nonterminals_struct;

typedef struct{
	char * terminal;
	int size;
}terminals_struct;


typedef struct{
	production_function_struct * production_function;
	nonterminals_struct * nonterminals;
	terminals_struct * terminals;
	char initial;
}gramatica_struct;

//AUTOMATA
typedef struct{
	char * nodos;
	int size;
}nonfinals_struct;
	
typedef struct{
	char * nodos;
	int size;
}finals_struct;

typedef struct{
	char * language;
	int size;
}language_struct;

typedef struct{
	char initialnode;
	char finalnode;
	char transition;
}statechange_struct;


typedef struct{
	nonfinals_struct * nonfinals;
	finals_struct * finals;
	language_struct * language;
	char initial;
	statechange_struct ** delta;	
}automata;


//Gramatica Funciones
int createGramatica();
int initializeFunctions();
int initializeNonTerminals();
int initializeTerminals();

int addTerminal(char);
int addTerminalSymbols(char *);

int addNonTerminal(char);
int addNonTerminalSymbols(char *);


void eliminateUnreach();
void eliminateInproductive();
int isContained(char, char * , int);

void left_eliminateUnitaries();
void left_normalize();
int addProduction(char, char, char);
int addProductionSymbols(char *);

void rotate_productions();
void right_normalize();
int isLeft();

int addTerminals(char *);
int addNonTermianls(char *);
int addProduction(char *);

//Automata Funciones
automata * createAutomata();

void reportError(char* msg,char* error);

void showTerminalSymbols();
void showNonTerminalSymbols();
void showProductionSymbols();
void showInitialSymbol();

void addInitialSymbol(char * c);
void makeDotFile(FILE * file);

