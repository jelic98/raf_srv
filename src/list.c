#include "main.h"

xNode xListCreate(long num, long* factors) {
	root = (xNode*) malloc(sizeof(xNode));
		
	if(root) {
		root->num = num;
		root->next = NULL;
			
		int i, len = sizeof(factors) / sizeof(long);
	
		for(i = 0; i < len; i++) {
			root->factors[i] = factors[i];
		}
	}

	return root;
}

xNode xListPut(xNode* root, long num, long* factors, int* added) {
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

xNode xListGet(xNode* root, long num) {
	while(root) {
		if(root->num == num) {
			return root;
		}

		root = root->next;
	}

	return NULL;
}

xNode xListClear(xNode* root) {
	xNode* tmp = NULL;

	while(root) {	
		tmp = root->next;

		free(root);

		root->num = 0;
		root->next = NULL;

		memset(root->factors, 0, sizeof(root->factors));

		root = tmp;
	}

	return root;
}
