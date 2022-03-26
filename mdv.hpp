#ifndef MDV_HPP
#define MDV_HPP

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

/* ========================================================================== */
/* ============================ HELPER STRUCTURE ============================ */
/* ========================================================================== */
template <typename T>
struct __MDV_Node {
	int height;
	int leaves;
	T key;
	struct __MDV_Node<T>* left;
	struct __MDV_Node<T>* right;
};

/* --------------------------- UTILITY FUNCTIONS ---------------------------- */
int max(int a, int b)
{
	return (a > b) ? a : b;
}

template <typename T>
void __print(__MDV_Node<T>* tree)
{
	if (tree == NULL)
		return;
	printf("leaves: %d  height: %d  key: %d\n", tree->leaves,
		tree->height, tree->key);
	__print(tree->left);
	__print(tree->right);
}

template <typename T>
__MDV_Node<T>* init_node(T key)
{
	__MDV_Node<T>* ret = (__MDV_Node<T>*) malloc(sizeof(__MDV_Node<T>));
	ret->key = key;
	ret->height = 1;
	ret->leaves = 1;
	ret->left = NULL;
	ret->right = NULL;

	return ret;
}

template <typename T>
void delete_node(__MDV_Node<T>* tree)
{
	if (tree == NULL)
		return;

	delete_node(tree->left);
	delete_node(tree->right);
	tree->height = 0;
	tree->leaves = 0;
	tree->left = NULL;
	tree->right = NULL;
	free(tree);
}

template <typename T>
int height(__MDV_Node<T>* n)
{
	if (n == NULL)
		return 0;

	return n->height;
}

template <typename T>
int balance(__MDV_Node<T>* n)
{
	if (n == NULL)
		return 0;

	return height(n->right) - height(n->left);
}

/* ------------------------------- BALANCING -------------------------------- */
template <typename T>
__MDV_Node<T>* rotate_right(__MDV_Node<T>* x)
{
	/* Variables for simplification */
	__MDV_Node<T>* left = x->left;
	__MDV_Node<T>* left_right = x->left->right;

	/* Rotate */
	x->left = left_right;
	left->right = x;

	/* Now just update heights and leaf counts */
	x->height = max(height(x->left), height(x->right)) + 1;
	left->height = max(height(left->left), height(left->right)) + 1;
	x->leaves = x->left->leaves + x->right->leaves;
	left->leaves = left->left->leaves + left->right->leaves;

	return left;
}

template <typename T>
__MDV_Node<T>* rotate_left(__MDV_Node<T>* x)
{
	/* Variables for simplification */
	__MDV_Node<T>* right = x->right;
	__MDV_Node<T>* right_left = x->right->left;

	/* Rotate */
	x->right = right_left;
	right->left = x;

	/* Now just update heights and leaf counts */
	x->height = max(height(x->left), height(x->right)) + 1;
	right->height = max(height(right->left), height(right->right)) + 1;
	x->leaves = x->left->leaves + x->right->leaves;
	right->leaves = right->left->leaves + right->right->leaves;

	return right;
}

/* ---------------------------- BASIC OPERATIONS ---------------------------- */
template <typename T>
T& __lookup(__MDV_Node<T>* x, int idx)
{
	/* Return found key */
	if (x->left == NULL)
		return x->key;

	/* If at least on is present, then both are present */
	/* Recurse into correct half, remeber about index subtraction! */
	if (idx < x->left->leaves)
		return __lookup(x->left, idx);
	else
		return __lookup(x->right, idx - x->left->leaves);
}

template <typename T>
int __find(__MDV_Node<T>* x, T val, int idx)
{
	/* We are at the bottom */
	if (x->left == NULL) {
		if (x->key == val)
			return idx;
		else
			return -1;
	}

	/* Look for the element in subtrees */
	int left = __find(x->left, val, idx);
	int right = __find(x->right, val, idx + x->left->leaves);

	if (left != -1)
		return left;
	else if (right != -1)
		return right;
	else
		return -1;
}

template <typename T>
__MDV_Node<T>* __insert(__MDV_Node<T>* x, int idx, T val, bool root=false)
{
	/* We reached the bottom */
	if (x->left == NULL) {
		__MDV_Node<T>* ret = init_node(T());
		ret->height = 2;
		ret->leaves = 2;
		if (idx == 0) {
			ret->left = init_node(val);
			ret->right = x;
		}
		else {
			ret->right = init_node(val);
			ret->left = x;
		}
		return ret;
	}

	/* Recurse like in lookup */
	if (idx < x->left->leaves)
		x->left = __insert(x->left, idx, val);
	else
		x->right = __insert(x->right, idx - x->left->leaves, val);

	/* Rebalance to preserve AVL property */
	int bf = balance(x);
	if (bf > 1) {		/* Tree is right-heavy */
		int bf_right = balance(x->right);
		if (bf_right > 0) {	/* Right-right rotation */
			return rotate_left(x);
		}
		else {			/* Right-left rotation */
			x->right = rotate_right(x->right);
			return rotate_left(x);
		}
	}
	else if (bf < -1) {	/* Tree is left-heavy */
		int bf_left = balance(x->left);
		if (bf_left > 0) {	/* Left-right rotation */
			x->left = rotate_left(x->left);
			return rotate_right(x);
		}
		else {			/* Left-left rotation */
			return rotate_right(x);
		}
	}

	/* Set values */
	x->height = max(height(x->left), height(x->right)) + 1;
	x->leaves = x->left->leaves + x->right->leaves;
	return x;
}

template <typename T>
__MDV_Node<T>* __erase(__MDV_Node<T>* x, int idx)
{
	/* We reached the bottom */
	if (x->left == NULL) {
		delete_node(x);
		return NULL;
	}

	/* Recurse like in lookup */
	if (idx < x->left->leaves)
		x->left = __erase(x->left, idx);
	else
		x->right = __erase(x->right, idx - x->left->leaves);

	/* If current node has only one child, remove it */
	if (x->right == NULL) {
		__MDV_Node<T>* ret = x->left;
		free(x);
		return ret;
	}
	if (x->left == NULL) {
		__MDV_Node<T>* ret = x->right;
		free(x);
		return ret;
	}

	/* Rebalance to preserve AVL property */
	int bf = balance(x);
	if (bf > 1) {		/* Tree is right-heavy */
		int bf_right = balance(x->right);
		if (bf_right > 0) {	/* Right-right rotation */
			return rotate_left(x);
		}
		else {			/* Right-left rotation */
			x->right = rotate_right(x->right);
			return rotate_left(x);
		}
	}
	else if (bf < -1) {	/* Tree is left-heavy */
		int bf_left = balance(x->left);
		if (bf_left > 0) {	/* Left-right rotation */
			x->left = rotate_left(x->left);
			return rotate_right(x);
		}
		else {			/* Left-left rotation */
			return rotate_right(x);
		}
	}

	/* Set values */
	x->height = max(height(x->left), height(x->right)) + 1;
	x->leaves = x->left->leaves + x->right->leaves;
	return x;
}

/* ========================================================================== */
/* ============================== PROPER CLASS ============================== */
/* ========================================================================== */
template <typename T>
class MDV {
	__MDV_Node<T>* tree;
	int len;

public:
	MDV();
	~MDV();

	void print();
	int length();
	void insert(int idx, T val);
	void push_back(T val);
	void push_front(T val);
	void erase(int idx);
	int find(T val);
	T& operator[](int idx);
};

/* ---------------------- INITIALIZATION & DESTRUCTION ---------------------- */
template <typename T>
MDV<T>::MDV()
{
	/* Tree will be initialized on first insert */
	tree = NULL;
	len = 0;
}

template <typename T>
MDV<T>::~MDV()
{
	delete_node(tree);
	len = 0;
}

/* ------------------------------- INTERFACE -------------------------------- */
template <typename T>
void MDV<T>::print()
{
	__print(tree);
}

template <typename T>
int MDV<T>::length()
{
	return len;
}

template <typename T>
void MDV<T>::insert(int idx, T val)
{
	if (len == 0) {
		tree = init_node(val);
		len++;
		return;
	}

	if (idx < 0 || idx > len)
		throw std::out_of_range("MDV::insert -> invalid index");

	tree = __insert(tree, idx, val, true);
	len++;
}

template <typename T>
void MDV<T>::push_back(T val)
{
	if (len == 0) {
		tree = init_node(val);
		len++;
		return;
	}

	tree = __insert(tree, len, val, true);
	len++;
}

template <typename T>
void MDV<T>::push_front(T val)
{
	if (len == 0) {
		tree = init_node(val);
		len++;
		return;
	}

	tree = __insert(tree, 0, val, true);
	len++;
}

template <typename T>
void MDV<T>::erase(int idx)
{
	if (idx < 0 || idx > len)
		throw std::out_of_range("MDV::erase -> invalid index");

	tree = __erase(tree, idx);
	len--;
}

template <typename T>
int MDV<T>::find(T val)
{
	return __find(tree, val, 0);
}

template <typename T>
T& MDV<T>::operator[](int idx)
{
	if (idx < 0 || idx >= len)
		throw std::out_of_range("MDV::operator[] -> invalid index");

	return __lookup(tree, idx);
}

#endif
