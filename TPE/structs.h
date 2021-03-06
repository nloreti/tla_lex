#define BLOCK 10
#include <stdio.h>

//GRAMATICA

typedef struct{
	char leftsimbol;
	char* rightsimbols;
	int rightsimbolslenght;
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

/*
//AUTOMATA

typedef struct{
	char label;
	char* name;
}node_struct;

typedef struct{
	node_struct** nodes;
	int size;
}nodes_struct;

	
typedef struct{
	char * nodes;
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
	statechange_struct ** delta;
	int size;
	}delta_struct;
	
typedef struct{
	nodes_struct * nodes;
	finals_struct * finals;
	language_struct * language;
	char initial;
	delta_struct * delta;	
}automata_struct;

*/
//Gramatica Funciones
int createGramatica();
int initializeFunctions();
int initializeNonTerminals();
int initializeTerminals();


int addTerminalSymbols(char *);
int addNonTerminalSymbols(char *);
void eliminateUnreach();
void eliminateInproductive();
int isContained(char, char * , int);
void left_eliminateUnitaries();
void left_normalize();
int addProductions(char, char, char);
int addProductionSymbols(char *);

void rotate_productions();
void right_normalize();
int isLeft();

int addTerminals(char *);
int addNonTermianls(char *);
int addProduction(char *);

int addTerminal(char c);
int addNonTerminal(char c);
int addInitialSymbol(char c);
void addInitialSymbols(char * c);

void getProductionLeftPart(char c);
int getProduction(char* c,int lenght);
void showProductionSymbols();
void showInitialSymbol();
void showNonTerminalSymbols();
void showTerminalSymbols();

void reportError(char* msg,char* error);
void makeDotFile(FILE * file);
/*
//Automata Funciones
int createAutomata();
int initializeNodes();
int initilizeFinals();
int initializeLanguage();
int initializeDelta();
int insertFinal(char c);
int getLabel(char c);
int insertNode(char* name, int lenght);
void showNodes();
void showFinals();
int getTransitionLeftPart(char* c);
int getTransitionRightPart(char* c);
int getLabelTransition(char c);
void showTransitions();

int createNewTransition(char leftLabel,char rightLabel, char transition);
int addLabel(char c);
int labelExist(char c);
int addLabelToLenguage(char c);
void showLenguage();


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
void gr_init();
*/
