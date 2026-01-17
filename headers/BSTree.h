#pragma once
#include <iostream>
#include <memory>
#include <concepts>
#include "ITree.h"

template <std::totally_ordered T> 
class BSTree : public ITree<T> {

protected:
	
	struct Node { //структура для узла 
		T key;
		std::unique_ptr<Node> left;
		std::unique_ptr<Node> right;

		//Конструкторы и присваивание
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

	BSTree(T key) : root(std::make_unique<Node>(Node(key))), node_count(1) {};	
	
	// Конструктор копирования
	BSTree(const BSTree& other) : root(clone(other.root.get())), node_count(other.node_count) {};	

	// Конструктор перемещения
	BSTree(BSTree&& other) noexcept
		: root(std::move(other.root)), node_count(other.node_count) {
		other.root = nullptr;		
	}

	virtual ~BSTree() = default;

	// Оператор копирующего присваивания
	BSTree& operator=(const BSTree& other) {
		if (this != &other) {
			root = clone(other.root.get());
			node_count = other.node_count;
		}
		return *this;
	};

	// Оператор перемещающего присваивания
	BSTree& operator=(BSTree&& other) noexcept {
		clear();
		root = std::move(other.root);
		node_count = other.node_count;
		other.root = nullptr;
		other.node_count = 0;
		return *this;
	};
	
	//--------- Основные операции -------//
	//вставка (итеративно)
	void insert(const T& key) override {

		if (!root) {
			root = std::make_unique<Node>(key);
			++node_count;
			return;
		}

		Node* current = root.get();
		while (true) {
			if (key < current->key) {
				if (!current->left) {
					current->left = std::make_unique<Node>(key);
					++node_count;
					return;
				}
				current = current->left.get();
			}
			else if (key > current->key) {
				if (!current->right) {
					current->right = std::make_unique<Node>(key);
					++node_count;
					return;
				}
				current = current->right.get();
			}
			else {	
				return;
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
		if (remove_impl(root, key))
			--node_count;
	}
	
	//очистка дерева
	void clear() override {
		root.reset();
		node_count = 0;
	}

	//--------- Состояние -------//
	//проверка на пустоту
	bool is_empty() const override{
		return !root;
	};
	
	//размер
	size_t size() const override {
		return node_count;
	};

	//высота	
	int height() const override {
		return calc_height(root.get());
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
	static bool remove_impl(std::unique_ptr<Node>& node, const T& key) {
		if (!node) return false;

		if (key < node->key) {
			return remove_impl(node->left, key);
		}
		else if (key > node->key) {
			return remove_impl(node->right, key);
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
				return remove_impl(node->right, node->key);
			}
			return true;
		}		
	}

	//рекурсивный расчет высоты
	static int calc_height(const Node* node) {
		return node ? 1 + std::max(calc_height(node->left.get()),
			calc_height(node->right.get()))	: -1;
	}

	
protected:
	std::unique_ptr<Node> root = nullptr;
	size_t node_count = 0;
};
