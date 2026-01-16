#pragma once
#include <iostream>
#include <memory>
#include <concepts>
#include "ITree.h"

template <std::totally_ordered T> 
class BSTree : public ITree<T> {

protected:
	struct Node {
		T key;
		std::unique_ptr<Node> left;
		std::unique_ptr<Node> right;

		//Конструкторы
		explicit Node(const T& k)
			: key(k), left(nullptr), right(nullptr) {}			
		
		//Копирование
		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;

		//Перемещение
		Node(Node&& other) noexcept = default;
		Node& operator=(Node&& other) noexcept = default;
		
		~Node() = default;
			
	};

public:
	//--------- конструкторы и операторы присваивания -------//
	
	BSTree() = default;  // пустое дерево

	BSTree(T key) : root(std::make_unique<Node>(Node(key))) {};	
	
	// Конструктор копирования
	BSTree(const BSTree& other) : root(clone(other.root.get())) {};	

	// Конструктор перемещения
	BSTree(BSTree&& other) noexcept
		: root(std::move(other.root)) {
		other.root = nullptr;		
	}

	virtual ~BSTree() = default;

	// Оператор копирующего присваивания
	BSTree& operator=(const BSTree& other) {
		if (this != &other) {
			root = clone(other.root.get());
		}
		return *this;
	};

	// Оператор перемещающего присваивания
	BSTree& operator=(BSTree&& other) noexcept {
		clear();
		root = std::move(other.root);		
		other.root = nullptr;		
		return *this;
	};
	
	//--------- Основные операции -------//
	//вставка (итеративно)
	void insert(const T& key) override {
		
		if (!root) {
			root = std::make_unique<Node>(key);
			return;
		}

		Node* current = root.get();
		while (true) {
			if (key < current->key) {
				if (!current->left) {
					current->left = std::make_unique<Node>(key);
					return;
				}
				current = current->left.get();
			}
			else {
				if (!current->right) {
					current->right = std::make_unique<Node>(key);
					return;
				}
				current = current->right.get();
			}
		}	
	};	
	
	//поиск элемента
	bool contains(const T& key) const override {
		// Ранний выход для пустого дерева
		if (!root) return false;

		const Node* current = root.get();

		if (key == current->key) return true;

		while (current) {
			if (key < current->key) {
				current = current->left.get();
			}
			else if (key > current->key) {
				current = current->right.get();
			}
			else {
				return true;
			}
		}
		return false;
	};
	
	//удаление элемента
	void remove(const T& key) override {
		root = remove_impl(root, key);
	}
	
	//очистка дерева
	void clear() override {
		root.reset();
	}

	//--------- Состояние -------//
	//проверка на пустоту
	bool is_empty() const {
		return !root;
	}
	
	using FuncPtr = void(*)(Node*);
	void visit(Node* node, FuncPtr action) {
		if (!node) return;		
		visit(node->left.get(), action);
		visit(node->right.get(), action);
		//std::cout << node->key << " ** "; //debug
		action(node);
		
	};
protected:

	//служебная функция копирования
	static std::unique_ptr<Node> clone(const Node* node) {
		if (!node) return nullptr;
		auto new_node = std::make_unique<Node>(node->key);
		new_node->left = clone(node->left.get());
		new_node->right = clone(node->right.get());
		return new_node;
	}
	//служебная функция поиска минимального ключа в поддереве
	static Node* find_min(Node* node) {
		while (node->left) {
			node = node->left.get();
		}
		return node;
	}
	
	//рекурсивная реализация удаления
	static void remove_impl(std::unique_ptr<Node>& node, const T& key) {
		if (!node) return;

		if (key < node->key) {
			remove_impl(node->left, key);
		}
		else if (key > node->key) {
			remove_impl(node->right, key);
		}
		else {
			// Узел для удаления найден
			if (!node->left) {
				node = std::move(node->right);
			}
			else if (!node->right) {
				node = std::move(node->left);
			}
			else {
				// Два ребенка
				Node* min = find_min(node->right.get());
				node->key = min->key;
				remove_impl(node->right, node->key);
			}
		}
	}

	
protected:
	std::unique_ptr<Node> root = nullptr;
};
