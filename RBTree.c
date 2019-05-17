#include "RBTree.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// Сдвиги для упаковки символов (смещение их номеров к началу с 0-го)
#define LOWERCASE_ENCODE -97		// Размещение букв нижнего регистра на позициях 0 - 25
#define UPPERCASE_ENCODE -39		// Размещение букв верхнего регистра на позициях 26 - 53

// Сдвиги для распаковки символов (смещение их номеров к прежним ASCII-кодам)
#define LOWERCASE_DECODE 97			// Получение буквы нижнего регистра по таблице ASCII
#define UPPERCASE_DECODE 39			// Получение буквы верхнего регистра по таблице ASCII

#define UPPERCASE 26				// Нижняя граница расположения букв верхнего регистра 

#define CORRECT_READING 1			// Прочитан один элемент файла

// Проверка дерева на пустоту
static bool isEmpty(const RBTree* T) {
	return T->root == NULL;
}

// Проверка: является ли узел правым сыном
static bool isRightChild(const RBNode* node) {
	return node == node->parent->right;
}

// Проверка: является ли узел листом
static bool isLeaf(const RBTree* T, const RBNode* node) {
	return node->left == T->NIL && node->right == T->NIL;
}

// Разместить заглавные и строчные буквы латинского алфавита в 6-и битах памяти
static uint8_t letterCompose(uint8_t value) {		
	return value + (islower(value) ? LOWERCASE_ENCODE : UPPERCASE_ENCODE);
}

// Распаковка букв из 6-бит памяти
static uint8_t letterUnpack(uint8_t value) {		
	return value + (value < UPPERCASE ? LOWERCASE_DECODE : UPPERCASE_DECODE);
}

// Удаление узлов К/Ч дерева
static void freeNods(RBNode* node, RBNode* NIL) {		
	if (node && node != NIL) {
		freeNods(node->left, NIL);
		freeNods(node->right, NIL);

		free(node);
	}
}

// Левый поворот К/Ч дерева
static RBNode* leftRotate(RBNode* x) {	
	RBNode* y = x->right;

	// Превращение левого поддерева "y" в правое поддерево "x"
	x->right = y->left;				
	if (y->left)
		y->left->parent = x;

	// Передача родителя "x" узлу "y"
	y->parent = x->parent;	
	!isRightChild(x) ? (x->parent->left = y) : (x->parent->right = y);

	// Размещение "х" в качестве левого дочернего узла "y"
	y->left = x;			
	x->parent = y;

	return y;
}

// Правый поворот К/Ч дерева
static RBNode* rightRotate(RBNode* y) {	
	RBNode* x = y->left;

	// Превращение правого поддерева "x" в левое поддерево "y"	
	y->left = x->right;		
	if (x->right)
		x->right->parent = y;

	// Передача родителя "y" узлу "x"
	x->parent = y->parent;	
	isRightChild(y) ? (y->parent->right = x) : (y->parent->left = x);

	// Размещение "y" в качестве правого дочернего узла "x"
	x->right = y;			
	y->parent = x;

	return x;
}

// Получить дядю узла
RBNode* getUncle(const RBNode* node) {
	return isRightChild(node->parent) ? node->parent->parent->left :
		node->parent->parent->right;
}

// Поддержание свойств К/Ч дерева при вставке нового элемента
static void insertFixup(RBTree* T, RBNode* node) {		
	while (node->parent != T->NIL) {
		RBNode* p = node->parent;

		if (node->color == RED && p->color == RED) {
			RBNode* u = getUncle(node);

			// Узел, родитель и дядя -- красного цвета
			if (u->color == RED) {		
				p->color = u->color = BLACK;
				p->parent->color = RED;
				p = p->parent;
			}
			else {				
				// Узел и родитель -- узлы красного цвета, Дядя -- узел чёрного цвета
				bool pirc = isRightChild(p);

				// Вставляемый элемент - младший потомок && отец – старший потомок || 
				// Вставляемый элемент - старший потомок && отец - младший потомок
				if (isRightChild(node) != pirc)	
					p = pirc ? rightRotate(p) : leftRotate(p);

				// Вставляемый элемент и его отец -- младшие или старшие потомки
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

// Сделать корнем элемент, не имеющий входных дуг. Используется после поворотов дерева
static void setRoot(RBTree* T) {
	while (T->root->parent != T->NIL)
		T->root = T->root->parent;
}

// Инициализировать К/Ч дерево
void initTree(RBTree* T) {
	T->root = NULL;

	T->NIL = (RBNode*)malloc(sizeof(RBNode));
	T->NIL->parent = T->NIL->left = T->NIL->right = T->NIL;
	T->NIL->color = BLACK;
}

// Удаление К/Ч дерева и ограничителя
void deleteTree(RBTree* T) {
	freeNods(T->root, T->NIL);
	free(T->NIL);

	T->root = T->NIL = NULL;
}

// Добавление нового элемента
void add(RBTree* T, uint8_t value) {
	RBNode* t = (RBNode*)malloc(sizeof(RBNode));
	t->value = letterCompose(value);
	t->left = t->right = T->NIL;

	// Если дерево пустое, добавляемый элемент -- корень
	if (isEmpty(T)) {
		T->root = t;
		T->root->color = BLACK;
		T->root->parent = T->NIL;
	}
	else {
		// Если дерево не пустое, вставка нового элемента по св-вам бинарного дерева
		RBNode* parent = NULL;
		for (RBNode* ptr = T->root; ptr != T->NIL; ) {
			parent = ptr;
			ptr = t->value < ptr->value ? ptr->left : ptr->right;
		}

		t->value < parent->value ? (parent->left = t) : (parent->right = t);

		t->color = RED;
		t->parent = parent;
	}

	// Обеспечение К/Ч свойств дерева
	insertFixup(T, t);
}

// Удаление минимального элемента
void deleteMin(RBTree* T) {
	RBNode* p = T->root;

	// Поиск минимального элемента
	while (p->left->left != T->NIL)
		p = p->left;

	if (p->left->color == BLACK)
		if (p->right->color == BLACK)
			if (isLeaf(T, p->left) && isLeaf(T, p->right)) {
				// Удаляемый элемент и его брат -- чёрные листья
				p->color = BLACK;
				p->right->color = RED;
			}
			else {
				// Брат удаляемого элемента имеет левого красного потомка
				if (p->right->left->color == RED)
					rightRotate(p->right);

				// Брат удаляемого элемента имеет правого красного потомка
				if (p->right->right->color == RED)
					leftRotate(p);

				p->parent->color = BLACK;
				p->color = p->parent->right->color = RED;
			}
		else {
			// Удаляемый элемент имеет красного брата
			leftRotate(p);

			p->parent->color = BLACK;
			p->color = RED;

			if (p->right != T->NIL) {
				// Удаляемый элемент имеет красного брата, который имеет левого потомка
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

// Удаление максимального элемента
void deleteMax(RBTree* T) {
	RBNode* p = T->root;

	// Поиск максимального элемента
	while (p->right->right != T->NIL)
		p = p->right;

	if (p->right->color == BLACK)
		if (p->left->color == BLACK)
			// Удаляемый элемент и его брат -- чёрные листья
			if (isLeaf(T, p->right) && isLeaf(T, p->left)) {
				p->color = BLACK;
				p->left->color = RED;
			}
			else {
				// Брат удаляемого элемента имеет правого красного потомка	
				if (p->left->right->color == RED)
					leftRotate(p->left);

				// Брат удаляемого элемента имеет левого красного потомка
				if (p->left->left->color == RED)
					rightRotate(p);

				p->parent->color = BLACK;
				p->color = p->parent->left->color = RED;
			}
		else {
			// Удаляемый элемент имеет красного брата		
			rightRotate(p);

			p->parent->color = BLACK;
			p->color = RED;

			if (p->left != T->NIL) {
				// Удаляемый элемент имеет красного брата, который имеет правого потомка
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

// Прямой обход дерева
void preOrderVisit(RBTree* T, RBNode* root) {
	if (root != T->NIL) {
		root->color == BLACK ? printf("[%c] ", letterUnpack(root->value)) :
							   printf("(%c) ", letterUnpack(root->value));

		preOrderVisit(T, root->left);
		preOrderVisit(T, root->right);
	}
}

// Симметричный обход дерева 
void inOrderVisit(RBTree* T, RBNode* root) {
	if (root != T->NIL) {
		inOrderVisit(T, root->left);

		root->color == BLACK ? printf("[%c] ", letterUnpack(root->value)) :
							   printf("(%c) ", letterUnpack(root->value));

		inOrderVisit(T, root->right);
	}
}

// Обратный отход дерева
void postOrderVisit(RBTree* T, RBNode* root) {
	if (root != T->NIL) {
		postOrderVisit(T, root->left);
		postOrderVisit(T, root->right);

		root->color == BLACK ? printf("[%c] ", letterUnpack(root->value)) :
							   printf("(%c) ", letterUnpack(root->value));
	}
}

// Чтение К/Ч дерева из файла
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

	// Проверка на корректный ввод данных
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
