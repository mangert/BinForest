#pragma once
#include <iostream>
#include <memory>
#include <concepts>

template <typename T>
requires std::totally_ordered<T> //принимаем только типы, у которых есть оператор <
class BSTree {

protected:
	struct Node {
		T key;
		Node* left;
		Node* right;	
	
		Node() : left(nullptr), right(nullptr) {};
		Node(T key) : key(key), left(nullptr), right(nullptr) {};
		Node(const Node& other) = delete;
		Node(Node&& other) = delete;
		Node& operator=(Node&&) = delete;
		Node& operator=(const Node&) = delete;

		~Node() = default;
	};

public:
	//  онструкторы
	BSTree() = default;  // пустое дерево

	BSTree(T key) : root(new Node(key)) {};

	
	//  онструктор копировани€
	BSTree(const BSTree& other) {
		//TODO
	};

	//  онструктор перемещени€
	BSTree(BSTree&& other) noexcept
		: root(other.root) {
		other.root = nullptr;		
	}

	virtual ~BSTree() {
		clear();
	};

	// ќператор копирующего присваивани€
	BSTree& operator=(const BSTree& other) {
		clear();
		//TODO
		return *this;
	};

	// ќператор перемещающего присваивани€
	BSTree& operator=(BSTree&& other) noexcept {
		clear();
		root = other.root;		
		other.root = nullptr;		
		return *this;
	};

	void insert(T& key) {
		Node* new_node = new Node(key);
		if (!root) {
			root = new_node;
			return;
		};
		_insert(key, root, new_node);	
	};

	void _insert(T key, Node* node, Node* new_node) {

		if (!node) {
			node = new_node;
			return;
		};

		if (key < node->key) {
			if (!node->left)
				node->left = new_node;
			else
				_insert(key, node->left, new_node);
		}
		else {
			if (!node->right)
				node->right = new_node;
			else
				_insert(key, node->right, new_node);
		};

	};
	
	bool search(T& key) const { return true; }
	void remove(T key) {}
	
	void clear(){
		//auto delete_node = [](Node* node) { delete node; node = nullptr; };
		auto print = [](Node* node) { std::cout << node->key << " ** "; };
		visit(root, print);
		std::cout << "== stop";

	}
	using FuncPtr = void(*)(Node*);
	void visit(Node* node, FuncPtr action) {
		if (!node) return;		
		visit(node->left, action);
		visit(node->right, action);
		//std::cout << node->key << " ** "; //debug
		action(node);
		
	};
	
protected:
	Node* root = nullptr;
};
