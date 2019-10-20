#include "main.h"

xNode* xListCreate(long num, long* factors) {
	xNode* root = (xNode*) calloc(1, sizeof(xNode));
		
	if(root) {
		root->num = num;
		root->next = NULL;
			
		int i;
	
		for(i = 0; i < factors[i]; i++) {
			root->factors[i] = factors[i];
		}
	}

	return root;
}

xNode* xListPut(xNode* root, long num, long* factors, int* added) {
	if(!root) {
		*added = 1;
		return xListCreate(num, factors);
	}

	xNode* tmp = root;

	while(tmp->next) {
		if(tmp->num == num) {
			*added = 0;
			return root;
		}

		tmp = tmp->next;
	}

	if(tmp->num == num) {
		*added = 0;
		return root;
	}

	tmp->next = xListCreate(num, factors);

	*added = 1;
	return root;
}

xNode* xListGet(xNode* root, long num) {
	while(root) {
		if(root->num == num) {
			return root;
		}

		root = root->next;
	}

	return NULL;
}

xNode* xListClear(xNode* root) {
	xNode* tmp = NULL;

	while(root) {	
		tmp = root->next;

		root->num = 0;
		root->next = NULL;

		memset(root->factors, 0, sizeof(root->factors));
		free(root->factors);
		free(root);

		root = tmp;
	}

	return root;
}
