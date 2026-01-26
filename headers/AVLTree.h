#pragma once
#include <iostream>
#include <memory>
#include <concepts>
#include "ITree.h"
#include <stack>
#include <queue>
#include <numeric>
#include <algorithm>

template <std::totally_ordered T>
class AVLTree : public ITree<T> {

public:

	struct Node { //структура для узла 
		T key;
		std::unique_ptr<Node> left;
		std::unique_ptr<Node> right;
		int height;

		//Конструкторы и присваивание
		explicit Node(const T& k)
			: key(k), left(nullptr), right(nullptr), height(0) {
		}

		//Копирование
		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;

		//Перемещение
		Node(Node&& other) noexcept = default;
		Node& operator=(Node&& other) noexcept = default;

		~Node() = default;

		//служебная функция обновления высоты узла
		void upd_height() {						

			static int counter = 0;
			counter++;
			std::cout << "upd_height call #" << counter << " for node " << key << "\n";

			
			int left_height = left ? left->height : -1;
			int right_height = right ? right->height : -1;
			height = 1 + std::max(left_height, right_height);
		}

		// Баланс-фактор
		int balance_factor() const {
			int left_height = left ? left->height : -1;
			int right_height = right ? right->height : -1;
			return left_height - right_height;
		}

		void debug_print() const {
			std::cout << "Node " << key
				<< " [height=" << height
				<< ", left=" << (left ? std::to_string(left->key) : "null")
				<< ", right=" << (right ? std::to_string(right->key) : "null")
				<< ", balance=" << balance_factor() << "]\n";
		}
	};

public:
	//--------- конструкторы и операторы присваивания -------//

	AVLTree() = default;  // пустое дерево

	AVLTree(T key) : root(std::make_unique<Node>(Node(key))), node_count(1) {};

	// Конструктор копирования
	AVLTree(const AVLTree& other) : root(clone(other.root.get())), node_count(other.node_count) {};

	// Конструктор перемещения
	AVLTree(AVLTree&& other) noexcept
		: root(std::move(other.root)), node_count(other.node_count) {
		other.root = nullptr;
	}

	virtual ~AVLTree() {
		clear();
	};

	// Оператор копирующего присваивания
	AVLTree& operator=(const AVLTree& other) {
		if (this != &other) {
			root = clone(other.root.get());
			node_count = other.node_count;
		}
		return *this;
	};

	// Оператор перемещающего присваивания
	AVLTree& operator=(AVLTree&& other) noexcept {
		clear();
		root = std::move(other.root);
		node_count = other.node_count;
		other.root = nullptr;
		other.node_count = 0;
		return *this;
	};

	//--------- Основные операции -------//
	//вставка (рекурсивно)
	void insert(const T& key) override {
		std::cout << "Ins " << key << " size before" << size() << std::endl;
		bool height_changed = false;
		root = insert_impl(std::move(root), key, height_changed);
		std::cout << "Ins " << key << " size after" << size() << std::endl;
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
		//TODO
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
	bool empty() const override {
		return !root;
	};

	// --------- Публичные методы обходов --------- //
	std::vector<T> inorder() const override {
		std::vector<T> result;
		if (root) {
			result.reserve(node_count);
			inorder_impl([&](const T& key) { result.emplace_back(key); });
		}
		return result;
	}

	std::vector<T> preorder() const override {
		std::vector<T> result;
		if (root) {
			result.reserve(node_count);
			preorder_impl([&](const T& key) { result.emplace_back(key); });
		}
		return result;
	}

	std::vector<T> postorder() const override {
		std::vector<T> result;
		if (root) {
			result.reserve(node_count);
			postorder_impl([&](const T& key) { result.emplace_back(key); });
		}
		return result;
	}

	std::vector<T> level_order() const override {
		std::vector<T> result;
		if (root) {
			result.reserve(node_count);
			level_order_impl([&](const T& key) { result.emplace_back(key); });
		}
		return result;
	}

	// --------- Visitor методы  --------- //	
	void visit_inorder(std::function<void(const T&)> visitor) const override {
		if (visitor) inorder_impl(visitor);
	}
	void visit_preorder(std::function<void(const T&)> visitor) const  override {
		if (visitor) preorder_impl(visitor);
	}
	void visit_postorder(std::function<void(const T&)> visitor) const override {
		if (visitor) postorder_impl(visitor);
	}
	void visit_level_order(std::function<void(const T&)> visitor) const override {
		if (visitor) level_order_impl(visitor);
	}

	//--------- Метрики -------//
	//размер
	size_t size() const override {
		return node_count;
	};

	//высота	
	int height() const override {

		if (!root) return -1;
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

	//--------- Печать -------//
	void print(std::ostream& os = std::cout) const override {

		if (!root) {
			os << "[empty]\n";
			return;
		}

		std::queue<const Node*> q;
		q.push(root.get());
		int level = 0;

		while (!q.empty()) {
			int level_size = q.size();
			os << "Level " << level << ": ";

			for (int i = 0; i < level_size; ++i) {
				const Node* current = q.front();
				q.pop();

				os << current->key;

				// Показываем связи
				if (current->left || current->right) {
					os << "[";
					if (current->left) os << "L:" << current->left->key;
					if (current->left && current->right) os << ",";
					if (current->right) os << "R:" << current->right->key;
					os << "]";
				}
				os << "  ";

				if (current->left) q.push(current->left.get());
				if (current->right) q.push(current->right.get());
			}
			os << "\n";
			level++;
		}
	}


public:
	
	// --------- Вращения --------- //
	// Функции принимают владение узлом, возвращают новый корень поддерева
	static std::unique_ptr<Node> small_rotate_left(std::unique_ptr<Node> x) {

		auto y = std::move(x->right); // Забираем правое поддерево
		x->right = std::move(y->left);   // Перемещаем левое поддерево y в правое x
		y->left = std::move(x);          // x становится левым ребёнком y

		// Обновляем высоты
		// Сначала обновляем бывший x (теперь y->left)
		if (y->left) {
			y->left->upd_height();
		}

		// Потом новый корень y
		y->upd_height();

		return y;  // Возвращаем новый корень
	}

	static std::unique_ptr<Node> small_rotate_right(std::unique_ptr<Node> x) {

		auto y = std::move(x->left);    // Забираем левое поддерево
		x->left = std::move(y->right);   // Перемещаем правое поддерево y в левое x
		y->right = std::move(x);

		// Обновляем высоты
		// Обновляем высоту бывшего x (теперь y->right)
		if (y->right) {
			y->right->upd_height();
		}

		// Потом новый корень y
		y->upd_height();

		return y;  // Возвращаем новый корень
	}
	
	static std::unique_ptr<Node> big_rotate_left(std::unique_ptr<Node> x) {	
		
		x->right = small_rotate_right(std::move(x->right));   
		
		//x->upd_height();    
		
		return small_rotate_left(std::move(x));		
	}

	static std::unique_ptr<Node> big_rotate_right(std::unique_ptr<Node> x) {

		x->left = small_rotate_left(std::move(x->left));
		
		//x->upd_height();

		return small_rotate_right(std::move(x));;
	}

	//Балансировка
	static std::unique_ptr<Node> balance(std::unique_ptr<Node> node) {
		if (!node) return nullptr;
		
		int bf = node->balance_factor();

		if (bf > 1) {  // Левый перевес
			if (node->left) {
				node->left->upd_height();
			}

			int left_bf = node->left ? node->left->balance_factor() : 0;

			if (left_bf >= 0) {
				return small_rotate_right(std::move(node));
			}
			else {
				return big_rotate_right(std::move(node));
			}
		}

		if (bf < -1) {  // Правый перевес
			if (node->right) {
				node->right->upd_height();
			}

			int right_bf = node->right ? node->right->balance_factor() : 0;

			if (right_bf <= 0) {
				return small_rotate_left(std::move(node));
			}
			else {
				return big_rotate_left(std::move(node));
			}
		}

		return node;
	}
	
	// --------- Шаблонные реализации обходов --------- //
	template<typename Action>
	void inorder_impl(Action&& action) const {
		if (!root) return;

		std::stack<const Node*> stack;
		const Node* current = root.get();

		while (current || !stack.empty()) {
			while (current) {
				stack.push(current);
				current = current->left.get();
			}

			current = stack.top();
			stack.pop();
			action(current->key);  // Вызываем action

			current = current->right.get();
		}
	}

	template<typename Action>
	void preorder_impl(Action&& action) const {
		if (!root) return;

		std::stack<const Node*> stack;
		stack.push(root.get());

		while (!stack.empty()) {
			const Node* current = stack.top();
			stack.pop();
			action(current->key);

			if (current->right) stack.push(current->right.get());
			if (current->left) stack.push(current->left.get());
		}
	}

	template<typename Action>
	void postorder_impl(Action&& action) const {
		if (!root) return;

		// Два стека
		std::stack<const Node*> stack1, stack2;
		stack1.push(root.get());

		while (!stack1.empty()) {
			const Node* current = stack1.top();
			stack1.pop();
			stack2.push(current);

			if (current->left) stack1.push(current->left.get());
			if (current->right) stack1.push(current->right.get());
		}

		while (!stack2.empty()) {
			action(stack2.top()->key);
			stack2.pop();
		}
	}

	template<typename Action>
	void level_order_impl(Action&& action) const {
		if (!root) return;

		std::vector<const Node*> current_lvl;
		current_lvl.push_back(root.get());

		while (!current_lvl.empty()) {
			std::vector<const Node*> next_lvl;
			next_lvl.reserve(current_lvl.size() * 2);  // Оптимизация!

			for (const Node* node : current_lvl) {
				action(node->key);

				if (node->left) next_lvl.push_back(node->left.get());
				if (node->right) next_lvl.push_back(node->right.get());
			}

			current_lvl = std::move(next_lvl);
		}
	}

	//-------------- Общие служебные функции ---------//
	
	//служебная функция копирования дерева (рекурсивная)
	static std::unique_ptr<Node> clone(const Node* source) {
		if (!source) return nullptr;

		auto new_node = std::make_unique<Node>(source->key);
		new_node->n_height = source->n_height;  // копируем высоту

		new_node->left = clone(source->left.get());
		new_node->right = clone(source->right.get());

		return new_node;
	}

	//служебная функция рекурсивной вставки
	std::unique_ptr<Node> insert_impl(std::unique_ptr<Node> node, const T& key,
		bool& height_changed) {
		if (!node) {
			height_changed = true;
			++node_count;
			return std::make_unique<Node>(key);
		}

		bool child_height_changed = false;
		if (key < node->key) {
			node->left = insert_impl(std::move(node->left), key, child_height_changed);
		}
		else if (key > node->key) {
			node->right = insert_impl(std::move(node->right), key, child_height_changed);
		}
		else {
			height_changed = false;  // Дубликат
			return node;
		}

		if (!child_height_changed) {
			// Высота поддерева не изменилась - пропускаем балансировку
			height_changed = false;
			return node;
		}

		// Сохраняем старую высоту
		int old_height = node->height;
		node->upd_height();

		if (node->height == old_height) {
			// Высота не изменилась
			height_changed = false;
			return node;
		}

		// Высота изменилась - проверяем баланс
		height_changed = true;
		return balance(std::move(node));
	}

	//служебная функция удаления узла
	static void remove_node(std::stack<std::unique_ptr<Node>*>& path) {
		//TODO		
	}
	


public:
	std::unique_ptr<Node> root = nullptr;
	size_t node_count = 0;
};

