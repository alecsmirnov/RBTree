#ifndef RBTREE_H
#define RBTREE_H

#include <stdint.h>

// ���� ���� �/� ������
typedef enum { RED, BLACK } color_t; 

typedef struct RBNode {			// ���� �/� ������
	uint8_t value : 6;			// ���� (6 ���), ��� �������� ���� ���������� �������� � ������� � ������ ��������
	color_t color;				// ����� ����: RED, BLACK	

	struct RBNode* parent;		// ������ ����
	struct RBNode* left;		// ����� ������� ����
	struct RBNode* right;		// ������ ������� ����
} RBNode;

typedef struct RBTree {			// ��������� �/� ������
	struct RBNode* root;		// ������ ������
	struct RBNode* NIL;			// ������������ (������� �������)
} RBTree;

// ������������� ������
void initTree(RBTree* T);	
// �������� ������
void deleteTree(RBTree* T);								

// ���������� ������ ��������
void add(RBTree* T, uint8_t value);						

// �������� ������������ ��������
void deleteMin(RBTree* T);		
// �������� ������������� ��������
void deleteMax(RBTree* T);								

// ������ ����� ������
void preOrderVisit(RBTree* T, RBNode* root);	
// ������������ ����� ������
void inOrderVisit(RBTree* T, RBNode* root);			
// �������� ����� ������
void postOrderVisit(RBTree* T, RBNode* root);		

// ������ ������ �� �����
void readFile(const char* file_name, RBTree* T);	

#endif