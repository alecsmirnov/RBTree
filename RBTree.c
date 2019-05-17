#include "RBTree.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// ������ ��� �������� �������� (�������� �� ������� � ������ � 0-��)
#define LOWERCASE_ENCODE -97		// ���������� ���� ������� �������� �� �������� 0 - 25
#define UPPERCASE_ENCODE -39		// ���������� ���� �������� �������� �� �������� 26 - 53

// ������ ��� ���������� �������� (�������� �� ������� � ������� ASCII-�����)
#define LOWERCASE_DECODE 97			// ��������� ����� ������� �������� �� ������� ASCII
#define UPPERCASE_DECODE 39			// ��������� ����� �������� �������� �� ������� ASCII

#define UPPERCASE 26				// ������ ������� ������������ ���� �������� �������� 

#define CORRECT_READING 1			// �������� ���� ������� �����

// �������� ������ �� �������
bool isEmpty(const RBTree* T) {
	return T->root == NULL;
}

// ��������: �������� �� ���� ������ �����
bool isRightChild(const RBNode* node) {
	return node == node->parent->right;
}

// ��������: �������� �� ���� ������
bool isLeaf(const RBTree* T, const RBNode* node) {
	return node->left == T->NIL && node->right == T->NIL;
}

// ���������� ��������� � �������� ����� ���������� �������� � 6-� ����� ������
static uint8_t letterCompose(uint8_t value) {		
	return value + (islower(value) ? LOWERCASE_ENCODE : UPPERCASE_ENCODE);
}

// ���������� ���� �� 6-��� ������
static uint8_t letterUnpack(uint8_t value) {		
	return value + (value < UPPERCASE ? LOWERCASE_DECODE : UPPERCASE_DECODE);
}

// �������� ����� �/� ������
static void freeNods(RBNode* node, RBNode* NIL) {		
	if (node && node != NIL) {
		freeNods(node->left, NIL);
		freeNods(node->right, NIL);

		free(node);
	}
}

// ����� ������� �/� ������
static RBNode* leftRotate(RBNode* x) {	
	RBNode* y = x->right;

	// ����������� ������ ��������� "y" � ������ ��������� "x"
	x->right = y->left;				
	if (y->left)
		y->left->parent = x;

	// �������� �������� "x" ���� "y"
	y->parent = x->parent;	
	!isRightChild(x) ? (x->parent->left = y) : (x->parent->right = y);

	// ���������� "�" � �������� ������ ��������� ���� "y"
	y->left = x;			
	x->parent = y;

	return y;
}

// ������ ������� �/� ������
static RBNode* rightRotate(RBNode* y) {	
	RBNode* x = y->left;

	// ����������� ������� ��������� "x" � ����� ��������� "y"	
	y->left = x->right;		
	if (x->right)
		x->right->parent = y;

	// �������� �������� "y" ���� "x"
	x->parent = y->parent;	
	isRightChild(y) ? (y->parent->right = x) : (y->parent->left = x);

	// ���������� "y" � �������� ������� ��������� ���� "x"
	x->right = y;			
	y->parent = x;

	return x;
}

// �������� ���� ����
RBNode* getUncle(const RBNode* node) {
	return isRightChild(node->parent) ? node->parent->parent->left :
		node->parent->parent->right;
}

// ����������� ������� �/� ������ ��� ������� ������ ��������
static void insertFixup(RBTree* T, RBNode* node) {		
	while (node->parent != T->NIL) {
		RBNode* p = node->parent;

		if (node->color == RED && p->color == RED) {
			RBNode* u = getUncle(node);

			// ����, �������� � ���� -- �������� �����
			if (u->color == RED) {		
				p->color = u->color = BLACK;
				p->parent->color = RED;
				p = p->parent;
			}
			else {				
				// ���� � �������� -- ���� �������� �����, ���� -- ���� ������� �����
				bool pirc = isRightChild(p);

				// ����������� ������� - ������� ������� && ���� � ������� ������� || 
				// ����������� ������� - ������� ������� && ���� - ������� �������
				if (isRightChild(node) != pirc)	
					p = pirc ? rightRotate(p) : leftRotate(p);

				// ����������� ������� � ��� ���� -- ������� ��� ������� �������
				p = pirc ? leftRotate(p->parent) : rightRotate(p->parent); p->color = BLACK;

				p->left->color = p->right->color = RED;
			}
		}

		node = p;
	}

	T->root = node;

	if (T->root->color == RED)
		T->root->color = BLACK;
}

// ������� ������ �������, �� ������� ������� ���. ������������ ����� ��������� ������
void setRoot(RBTree* T) {
	while (T->root->parent != T->NIL)
		T->root = T->root->parent;
}

// ���������������� �/� ������
void initTree(RBTree* T) {
	T->root = NULL;

	T->NIL = (RBNode*)malloc(sizeof(RBNode));
	T->NIL->parent = T->NIL->left = T->NIL->right = T->NIL;
	T->NIL->color = BLACK;
}

// �������� �/� ������ � ������������
void deleteTree(RBTree* T) {
	freeNods(T->root, T->NIL);
	free(T->NIL);

	T->root = T->NIL = NULL;
}

// ���������� ������ ��������
void add(RBTree* T, uint8_t value) {
	RBNode* t = (RBNode*)malloc(sizeof(RBNode));
	t->value = letterCompose(value);
	t->left = t->right = T->NIL;

	// ���� ������ ������, ����������� ������� -- ������
	if (isEmpty(T)) {
		T->root = t;
		T->root->color = BLACK;
		T->root->parent = T->NIL;
	}
	else {
		// ���� ������ �� ������, ������� ������ �������� �� ��-��� ��������� ������
		RBNode* parent = NULL;
		for (RBNode* ptr = T->root; ptr != T->NIL; ) {
			parent = ptr;
			ptr = t->value < ptr->value ? ptr->left : ptr->right;
		}

		t->value < parent->value ? (parent->left = t) : (parent->right = t);

		t->color = RED;
		t->parent = parent;
	}

	// ����������� �/� ������� ������
	insertFixup(T, t);
}

// �������� ������������ ��������
void deleteMin(RBTree* T) {
	RBNode* p = T->root;

	// ����� ������������ ��������
	while (p->left->left != T->NIL)
		p = p->left;

	if (p->left->color == BLACK)
		if (p->right->color == BLACK)
			if (isLeaf(T, p->left) && isLeaf(T, p->right)) {
				// ��������� ������� � ��� ���� -- ������ ������
				p->color = BLACK;
				p->right->color = RED;
			}
			else {
				// ���� ���������� �������� ����� ������ �������� �������
				if (p->right->left->color == RED)
					rightRotate(p->right);

				// ���� ���������� �������� ����� ������� �������� �������
				if (p->right->right->color == RED)
					leftRotate(p);

				p->parent->color = BLACK;
				p->color = p->parent->right->color = RED;
			}
		else {
			// ��������� ������� ����� �������� �����
			leftRotate(p);

			p->parent->color = BLACK;
			p->color = RED;

			if (p->right != T->NIL) {
				// ��������� ������� ����� �������� �����, ������� ����� ������ �������
				leftRotate(p);
				p->parent->color = RED;
				p->color = p->parent->right->color = BLACK;
			}
		}

	RBNode* temp = p->left->right;

	free(p->left);
	p->left = temp;
	p->left->color = BLACK;

	setRoot(T);
}

// �������� ������������� ��������
void deleteMax(RBTree* T) {
	RBNode* p = T->root;

	// ����� ������������� ��������
	while (p->right->right != T->NIL)
		p = p->right;

	if (p->right->color == BLACK)
		if (p->left->color == BLACK)
			// ��������� ������� � ��� ���� -- ������ ������
			if (isLeaf(T, p->right) && isLeaf(T, p->left)) {
				p->color = BLACK;
				p->left->color = RED;
			}
			else {
				// ���� ���������� �������� ����� ������� �������� �������	
				if (p->left->right->color == RED)
					leftRotate(p->left);

				// ���� ���������� �������� ����� ������ �������� �������
				if (p->left->left->color == RED)
					rightRotate(p);

				p->parent->color = BLACK;
				p->color = p->parent->left->color = RED;
			}
		else {
			// ��������� ������� ����� �������� �����		
			rightRotate(p);

			p->parent->color = BLACK;
			p->color = RED;

			if (p->left != T->NIL) {
				// ��������� ������� ����� �������� �����, ������� ����� ������� �������
				rightRotate(p);
				p->parent->color = RED;
				p->color = p->parent->left->color = BLACK;
			}
		}

	RBNode* temp = p->right->left;

	free(p->right);
	p->right = temp;
	p->right->color = BLACK;

	setRoot(T);
}

// ������ ����� ������
void preOrderVisit(RBTree* T, RBNode* root) {
	if (root != T->NIL) {
		root->color == BLACK ? printf("[%c] ", letterUnpack(root->value)) :
							   printf("(%c) ", letterUnpack(root->value));

		preOrderVisit(T, root->left);
		preOrderVisit(T, root->right);
	}
}

// ������������ ����� ������ 
void inOrderVisit(RBTree* T, RBNode* root) {
	if (root != T->NIL) {
		inOrderVisit(T, root->left);

		root->color == BLACK ? printf("[%c] ", letterUnpack(root->value)) :
							   printf("(%c) ", letterUnpack(root->value));

		inOrderVisit(T, root->right);
	}
}

// �������� ����� ������
void postOrderVisit(RBTree* T, RBNode* root) {
	if (root != T->NIL) {
		postOrderVisit(T, root->left);
		postOrderVisit(T, root->right);

		root->color == BLACK ? printf("[%c] ", letterUnpack(root->value)) :
							   printf("(%c) ", letterUnpack(root->value));
	}
}

// ������ �/� ������ �� �����
void readFile(const char* file_name, RBTree* T) {
	FILE* fp = NULL;
	if ((fp = fopen(file_name, "r")) == NULL) {
		fprintf(stderr, "Unable to open input file!\n");
		exit(EXIT_FAILURE);
	}

	initTree(T);

	char ch;
	while (fscanf(fp, "%c", &ch) == CORRECT_READING)
		isalpha(ch) ? add(T, ch) : (ch = EOF);

	// �������� �� ���������� ���� ������
	if (ch == EOF) {
		deleteTree(T);

		fprintf(stderr, "File reading error!\n");
		exit(EXIT_FAILURE);
	}

	if (fclose(fp) != 0) {
		fprintf(stderr, "File closing error!\n");
		exit(EXIT_FAILURE);
	}
}