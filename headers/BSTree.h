#pragma once
#include <iostream>
#include <memory>
#include <concepts>
#include "ITree.h"
#include <stack>
#include <queue>

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

	virtual ~BSTree() {
		clear();		
	};

	// Оператор копирующего присваивания
	BSTree& operator=(const BSTree& other) {
		if (this != &other) {
			root = clone(other.root.get());
			node_count = other.node_count;
		}
		std::cout << "Copied!" << std::endl;
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
		if (!root) return;

		// Стек указателей на unique_ptr вдоль пути поиска
		std::stack<std::unique_ptr<Node>*> path;
		path.push(&root);

		// Поиск узла для удаления
		while (!path.empty()) {
			auto current_ptr = path.top();
			Node* current = current_ptr->get();

			if (!current) {
				// Дошли до nullptr - ключ не найден
				path.pop();
				return;
			}

			if (key < current->key) {				
				path.push(&((*current_ptr)->left));
			}
			else if (key > current->key) {				
				path.push(&((*current_ptr)->right));
			}
			else {
				// Нашли узел для удаления
				remove_node(path);
				--node_count;
				return;
			}
		}
	}
	
	//очистка дерева (итеративно)
	void clear() override {
		if (!root) return;

		std::stack<std::unique_ptr<Node>> node_stack;
		node_stack.push(std::move(root));  // Перемещаем владение в стек

		while (!node_stack.empty()) {
			auto node = std::move(node_stack.top());
			node_stack.pop();

			// Перемещаем детей в стек перед удалением node
			if (node->left) {
				node_stack.push(std::move(node->left));
			}
			if (node->right) {
				node_stack.push(std::move(node->right));
			}
		}

		node_count = 0;		
	}

	//--------- Состояние -------//
	//проверка на пустоту
	bool is_empty() const override{
		return !root;
	};

	//--------- Обходы -------//
	std::vector<T> inorder() const override {
		std::vector<T> result;
		if (!root) return result;
		result.reserve(node_count);

		std::stack<const Node*> stack;
		const Node* current = root.get();

		while (current || !stack.empty()) {
			// Добираемся до самого левого узла
			while (current) {
				stack.push(current);
				current = current->left.get();
			}

			// Обрабатываем узел
			current = stack.top();
			stack.pop();
			result.emplace_back(current->key);

			// Переходим к правому поддереву
			current = current->right.get();
		}

		return result;
	}

	std::vector<T> preorder() const override {
		std::vector<T> result;
		if (!root) return result;
		result.reserve(node_count);

		std::stack<const Node*> stack;
		stack.push(root.get());

		while (!stack.empty()) {
			const Node* current = stack.top();
			stack.pop();
			result.emplace_back(current->key);

			//сначала кладем правого, потом левого			
			if (current->right) {
				stack.push(current->right.get());
			}
			if (current->left) {
				stack.push(current->left.get());
			}
		}

		return result;
	}

	std::vector<T> postorder() const override {
		std::vector<T> result;
		if (!root) return result;
		result.reserve(node_count);

		std::stack<const Node*> stack;
		stack.push(root.get());

		while (!stack.empty()) {
			const Node* current = stack.top();
			stack.pop();
			result.emplace_back(current->key);

			if (current->left) stack.push(current->left.get());
			if (current->right) stack.push(current->right.get());
		}

		std::reverse(result.begin(), result.end());
		return result;
	}

	std::vector<T> level_order() const override {
		std::vector<T> result;
		if (!root) return result;
		result.reserve(node_count);

		// Используем вектор
		std::vector<const Node*> current_lvl;
		current_lvl.push_back(root.get());

		while (!current_lvl.empty()) {
			std::vector<const Node*> next_lvl;

			for (const Node* node : current_lvl) {
				result.emplace_back(node->key);

				if (node->left) next_lvl.push_back(node->left.get());
				if (node->right) next_lvl.push_back(node->right.get());
			}

			// Освобождаем память предыдущего уровня
			current_lvl = std::move(next_lvl);
		}

		return result;
	}
	
	//--------- Метрики -------//
	//размер
	size_t size() const override {
		return node_count;
	};
	
	//высота	
	int height() const override {
		if (!root) return -1;  // или 0, в зависимости от определения

		std::queue<const Node*> q;
		q.push(root.get());
		int height = -1;

		while (!q.empty()) {
			int levelSize = q.size();
			height++;

			for (int i = 0; i < levelSize; i++) {
				const Node* current = q.front();
				q.pop();

				if (current->left) q.push(current->left.get());
				if (current->right) q.push(current->right.get());
			}
		}
		return height;
	}	
	
	
protected:

	//служебная функция копирования дерева (итеративная)
	static std::unique_ptr<Node> clone(const Node* source_root) {
		
		if (!source_root) return nullptr;

		auto new_root = std::make_unique<Node>(source_root->key);

		std::queue<const Node*> src;  // Оригинал
		std::queue<Node*> dst;        // Копия

		src.push(source_root);
		dst.push(new_root.get());

		while (!src.empty()) {
			const Node* src_node = src.front();
			Node* dst_node = dst.front();
			src.pop();
			dst.pop();

			// Левый ребeнок
			if (src_node->left) {
				dst_node->left = std::make_unique<Node>(src_node->left->key);
				src.push(src_node->left.get());
				dst.push(dst_node->left.get());
			}

			// Правый ребeнок
			if (src_node->right) {
				dst_node->right = std::make_unique<Node>(src_node->right->key);
				src.push(src_node->right.get());
				dst.push(dst_node->right.get());
			}
		}

		return new_root;
	}

	//служебная функция удаления узла
	static void remove_node(std::stack<std::unique_ptr<Node>*>& path) {
		// path.top() указывает на unique_ptr удаляемого узла

		auto node_ptr = path.top();  // Указатель на unique_ptr<Node> удаляемого узла
		path.pop();                 // Убираем удаляемый узел из пути

		Node* node = node_ptr->get(); // Сырой указатель на удаляемый узел

		//У узла нет левого ребенка
		if (!node->left) {
			*node_ptr = std::move(node->right);  // Заменяем на правого ребенка
			return;
		}

		//У узла нет правого ребенка
		if (!node->right) {
			*node_ptr = std::move(node->left);   // Заменяем на левого ребенка
			return;
		}

		//У узла есть оба ребенка
		remove_2children_node(node_ptr);
	}

	//вспомогательная функция удаления узла с двумя детьми
	static void remove_2children_node(std::unique_ptr<Node>* nodePtr) {
		Node* node = nodePtr->get();

		// Находим преемника (минимальный в правом поддереве)
		// и его родителя
		std::unique_ptr<Node>* successorParentPtr = &((*nodePtr)->right);
		std::unique_ptr<Node>* successorPtr = &((*nodePtr)->right);

		// Идём влево, пока можно
		while ((*successorPtr)->left) {
			successorParentPtr = successorPtr;
			successorPtr = &((*successorPtr)->left);
		}
		
		// Копируем ключ преемника в удаляемый узел
		node->key = (*successorPtr)->key;

		// Удаляем преемника из дерева
		// У преемника может быть правый ребёнок (или не быть)
		if (successorParentPtr == successorPtr) {
			// Преемник - прямой правый ребёнок удаляемого узла
			*successorPtr = std::move((*successorPtr)->right);
		}
		else {
			// Преемник где-то глубже в левой ветке
			*successorPtr = std::move((*successorPtr)->right);
		}
	}	
	
	
protected:
	std::unique_ptr<Node> root = nullptr;
	size_t node_count = 0;
};
