#include <stdio.h>

#include "RBTree.h"

int main(int argc, char* argv[]) {
	RBTree T;
	readFile("tree.txt", &T);

	printf("Preoder visit:\t\t");
	preOrderVisit(&T, T.root);
	printf("\nInorder visit:\t\t");
	inOrderVisit(&T, T.root);
	printf("\nPostorder visit:\t");
	postOrderVisit(&T, T.root);

	deleteMin(&T);

	printf("\n\nPreoder visit:\t\t");
	preOrderVisit(&T, T.root);
	printf("\nInorder visit:\t\t");
	inOrderVisit(&T, T.root);
	printf("\nPostorder visit:\t");
	postOrderVisit(&T, T.root);
	puts("");

	deleteTree(&T);

	return 0;
}