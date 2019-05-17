#ifndef RBTREE_H
#define RBTREE_H

#include <stdint.h>

// Цвет узла К/Ч дерева
typedef enum { RED, BLACK } color_t; 

typedef struct RBNode {			// Узел К/Ч дерева
	uint8_t value : 6;			// Поле (6 бит), для хранения букв латинского алфавита в верхнем и нижнем регистре
	color_t color;				// Цвета узла: RED, BLACK	

	struct RBNode* parent;		// Предок узла
	struct RBNode* left;		// Левый потомок узла
	struct RBNode* right;		// Правый потомок узла
} RBNode;

typedef struct RBTree {			// Структура К/Ч дерева
	struct RBNode* root;		// Корень дерева
	struct RBNode* NIL;			// Ограничитель (нулевой элемент)
} RBTree;

// Инициализация дерева
void initTree(RBTree* T);	
// Удаление дерева
void deleteTree(RBTree* T);								

// Добавление нового элемента
void add(RBTree* T, uint8_t value);						

// Удаление минимального элемента
void deleteMin(RBTree* T);		
// Удаление максимального элемента
void deleteMax(RBTree* T);								

// Прямой обход дерева
void preOrderVisit(RBTree* T, RBNode* root);	
// Симметричный обход дерева
void inOrderVisit(RBTree* T, RBNode* root);			
// Обратный отход дерева
void postOrderVisit(RBTree* T, RBNode* root);		

// Чтение дерева из файла
void readFile(const char* file_name, RBTree* T);	

#endif