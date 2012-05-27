
#define BLOCK 10


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


//Gramatica Funciones
int createGramatica();
int initializeFunctions();
int initializeNonTerminals();
int initializeTerminals();

int addTerminal(char c);
int addNonTerminal(char c);
int addInitialSymbol(char c);
int addProduction(char c);
void getProductionLeftPart(char c);
int getProduction(char* c,int lenght);
void showProductionSymbols();
void showInitialSymbol();
void showNonTerminalSymbols();
void showTerminalSymbols();

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
void reportError(char* msg,char* error);
int createNewTransition(char leftLabel,char rightLabel, char transition);



