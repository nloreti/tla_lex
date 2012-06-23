int isTerminal(char c){
	return islower(c);
}

int isNonTerminal(char c){
	return isupper(c);
}

char getProductionComponent(production_struct production, int i){
	if(i < 0 ||i >= production.size){
		return -1;
	}
	return (i == 0)? production->left : production->right[i-1];
}

production_struct newProduction(char left, char * right){
	production_struct p = malloc(sizeof(production_struct));
	p->left = left;
	p->right = right;
	p.size= strlen(right)+1;
	return p;
}
