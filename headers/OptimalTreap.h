#pragma once
#include <iostream>
#include <memory>
#include <concepts>
#include "ITree.h"
#include <stack>
#include <queue>
#include <numeric>
#include <algorithm>
#include <random>

enum class RebuildStrategy {
	NEVER,           // Только ручная перестройка
	OPERATION_COUNT, // По количеству операций
	HEIGHT_THRESHOLD, // По превышению высоты
	HYBRID           // И то, и другое
};


template <std::totally_ordered T,
	RebuildStrategy Strategy = RebuildStrategy::OPERATION_COUNT>
class OptimalTreap : public ITree<T> {

public:
	struct Node { //структура для узла 
		T key;	
		
		std::unique_ptr<Node> left;
		std::unique_ptr<Node> right;

		float base_priority;
		double priority;
		size_t access_count = 0;   // Счётчик обращений	

		
		//Конструкторы и присваивание
		explicit Node(const T& k)
			: key(k), base_priority(1.0 + (std::hash<T>{}(key) % 1000) / 10000.0), 
			left(nullptr), right(nullptr) {
			
			update_priority();
		}

		//Копирование
		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;

		//Перемещение
		Node(Node&& other) noexcept = default;
		Node& operator=(Node&& other) noexcept = default;

		~Node() = default;

		// Обновляем приоритет при изменении access_count
		void update_priority() {
			// Формула: чем больше доступов, тем выше приоритет (ближе к корню)
			priority = base_priority + std::log(1.0 + access_count);
		}

		// Увеличиваем счётчик и обновляем приоритет
		void record_access() {
			++access_count;
			update_priority();
		}

		// Для отладки
#ifdef DEBUG
		void debug_print() const {
			std::cout << "Node " << key
				<< " [priority=" << priority
				<< ", left=" << (left ? std::to_string(left->key) : "null")
				<< ", right=" << (right ? std::to_string(right->key) : "null")
				<< "]\n";
		}
#endif // DEBUG
	};

public:
	//--------- конструкторы и операторы присваивания -------//

	OptimalTreap() = default;  // пустое дерево

	OptimalTreap(T key) : root(std::make_unique<Node>(Node(key))), node_count(1) {};

	// Конструктор копирования
	OptimalTreap(const OptimalTreap& other) : root(clone(other.root.get())), node_count(other.node_count) {};

	// Конструктор перемещения
	OptimalTreap(OptimalTreap&& other) noexcept
		: root(std::move(other.root)), node_count(other.node_count) {
		other.root = nullptr;
	}

	virtual ~OptimalTreap() {
		clear();
	};

	// Оператор копирующего присваивания
	OptimalTreap& operator=(const OptimalTreap& other) {
		if (this != &other) {
			root = clone(other.root.get());
			node_count = other.node_count;
		}
		return *this;
	};

	// Оператор перемещающего присваивания
	OptimalTreap& operator=(OptimalTreap&& other) noexcept {
		clear();
		root = std::move(other.root);
		node_count = other.node_count;
		other.root = nullptr;
		other.node_count = 0;
		return *this;
	};

	//--------- Основные операции -------//
	//вставка
	void insert(const T& key) override {
		// Сначала проверяем, есть ли уже
		Node* existing = find_node(key);  // Нужен не-const метод find_node

		if (existing) {
			// Уже есть — просто обновляем статистику
			existing->record_access();
			// Можно локально перебалансировать
			//root = splay_by_priority(std::move(root), key);
		}
		else {
			// Нет — вставляем
			root = insert_impl(std::move(root), key);			
		}
		++operations_since_rebuild;
		check_and_rebuild();
	};

	//поиск элемента
	bool contains(const T& key) const override {
		const Node* node = find_node(key);
		return node != nullptr;
	}

	// Non-const версия с обновлением статистики
	bool find_and_update(const T& key) {
		
		Node* node = find_node(key);
		if (node) {
			node->record_access();			
			++operations_since_rebuild;
			check_and_rebuild();
			return true;
		}
		return false;
	}	

	//удаление элемента
	void remove(const T& key) override {
		root = remove_impl(std::move(root), key);
		++operations_since_rebuild;
		check_and_rebuild();
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

	
	void test_rebuild() {
		std::cout << "=== Тест перестройки оптимального дерева ===\n";

		// 1. Создаём дерево с 7 элементами
		clear();
		std::vector<int> keys = { 50, 30, 70, 20, 40, 60, 80 };
		for (int key : keys) {
			insert(key);
		}

		std::cout << "Исходное дерево (после вставки):\n";
		print();
		std::cout << "Высота: " << height() << ", Размер: " << size() << "\n\n";

		// 2. Имитируем поиски (увеличиваем access_count для некоторых ключей)
		std::cout << "Имитируем частые обращения к ключам 30, 70, 20...\n";

		// Частые обращения к 30
		for (int i = 0; i < 5; i++) {
			find_and_update(30);
		}

		// Частые обращения к 70  
		for (int i = 0; i < 3; i++) {
			find_and_update(70);
		}

		// Одно обращение к 20
		find_and_update(20);

		// 3. Покажем статистику до перестройки
		std::cout << "\nСтатистика до перестройки:\n";
		std::cout << "Key\tAccess\tPriority\n";
		std::cout << "------------------------\n";

		auto show_stats = [](Node* node) {
			if (node) {
				std::cout << node->key << "\t"
					<< node->access_count << "\t"
					<< node->priority << "\n";
			}
			};

		// Простой обход для вывода статистики
		std::function<void(Node*)> print_stats = [&](Node* node) {
			if (!node) return;
			print_stats(node->left.get());
			show_stats(node);
			print_stats(node->right.get());
			};

		print_stats(root.get());

		// 4. Принудительная перестройка
		std::cout << "\nВыполняем перестройку...\n";
		rebuild_optimal();

		// 5. Покажем результат
		std::cout << "\nДерево после перестройки:\n";
		print();
		std::cout << "Высота: " << height() << "\n";

		// 6. Проверим, что дерево осталось корректным BST
		std::cout << "\nПроверка корректности BST (inorder обход):\n";
		auto inorder_result = inorder();
		for (int key : inorder_result) {
			std::cout << key << " ";
		}
		std::cout << "\n";

		// Проверим отсортированность
		bool is_sorted = std::is_sorted(inorder_result.begin(), inorder_result.end());
		std::cout << "Inorder отсортирован: " << (is_sorted ? "ДА" : "НЕТ") << "\n";

		// 7. Покажем новую статистику
		std::cout << "\nСтатистика после перестройки:\n";
		std::cout << "Key\tAccess\tPriority\n";
		std::cout << "------------------------\n";
		print_stats(root.get());

		std::cout << "=== Тест завершён ===\n\n";
	}


protected:
	//======== вспомогательные функции split и merge =====/
	static std::pair<std::unique_ptr<Node>, std::unique_ptr<Node>>
		split(std::unique_ptr<Node> node, const T& key) {

		if (!node) {
			return { nullptr, nullptr };
		}

		if (node->key < key) { // Разделяем правое поддерево			

			auto [left_of_right, right] = split(std::move(node->right), key);

			// Присоединяем left_of_right как правое поддерево текущего узла
			node->right = std::move(left_of_right);

			// Текущий узел (с поддеревьями) + то, что получилось справа
			return { std::move(node), std::move(right) };

		}
		else { // Разделяем левое поддерево

			auto [left, right_of_left] = split(std::move(node->left), key);

			// Присоединяем right_of_left как левое поддерево текущего узла
			node->left = std::move(right_of_left);

			// То, что получилось слева + текущий узел (с поддеревьями)
			return { std::move(left), std::move(node) };
		}
	}

	static std::unique_ptr<Node> merge(std::unique_ptr<Node> left,
		std::unique_ptr<Node> right) {
		// Базовые случаи
		if (!left) return right;
		if (!right) return left;

		// Сравниваем приоритеты (свойство кучи)
		if (left->priority > right->priority) { // left становится корнем, его приоритет больше

			// Рекурсивно мержим правое поддерево left с right
			left->right = merge(std::move(left->right), std::move(right));
			return left;  // left - новый корень

		}
		else { // right становится корнем, его приоритет больше или равен

			// Рекурсивно мержим left с левым поддеревом right
			right->left = merge(std::move(left), std::move(right->left));
			return right;  // right - новый корень
		}
	}

	//-------------Вспомогательные функции для поиска -------//
	
	Node* find_node(const T& key) {
		Node* current = root.get();
		while (current) {
			if (key < current->key) {
				current = current->left.get();
			}
			else if (key > current->key) {
				current = current->right.get();
			}
			else {
				current->record_access();
				return current;  // Нашли
			}
		}
		return nullptr;  // Не нашли
	}

	Node* find_node(const T& key) const {
		Node* current = root.get();
		while (current) {
			if (key < current->key) {
				current = current->left.get();
			}
			else if (key > current->key) {
				current = current->right.get();
			}
			else {
				return current;  // Нашли
			}
		}
		return nullptr;  // Не нашли
	}

	//-------------Перестройка дерева -------//
	//основная функция перестройки
	void rebuild_optimal() {
		if (!root) return;

		// 1. Собираем все узлы в вектор unique_ptr (inorder - уже отсортировано!)
		std::vector<std::unique_ptr<Node>> all_nodes;
		all_nodes.reserve(node_count);
		collect_all_nodes_inorder(all_nodes, std::move(root));

		// 2. НЕ нужно сортировать! Узлы уже в порядке возрастания ключей
		// 3. Строим декартово дерево
		root = build_cartesian_from_sorted(std::move(all_nodes));
	}

	//вспомогательная функция для сбора дерева в вектор
	void collect_all_nodes_inorder(std::vector<std::unique_ptr<Node>>& result,
		std::unique_ptr<Node>&& subtree) {
		if (!subtree) return;

		// 1. Сначала сохраняем правое поддерево (чтобы не потерять после перемещения subtree)
		auto right_subtree = std::move(subtree->right);

		// 2. Рекурсивно обрабатываем левое поддерево
		collect_all_nodes_inorder(result, std::move(subtree->left));

		// 3. Теперь subtree->left уже перемещён, можно безопасно переместить сам узел
		//    Отсоединяем оставшееся правое поддерево (оно сохранено в right_subtree)
		//    и добавляем узел в результат
		result.push_back(std::move(subtree));

		// 4. Обрабатываем правое поддерево
		collect_all_nodes_inorder(result, std::move(right_subtree));
	}

	//вспомогательная функция построения дерева из вектора
	std::unique_ptr<Node> build_cartesian_from_sorted(
		std::vector<std::unique_ptr<Node>>&& sorted_nodes) {

		if (sorted_nodes.empty()) return nullptr;

		// 1. Находим узел с максимальным приоритетом
		int max_prio_idx = 0;
		for (size_t i = 1; i < sorted_nodes.size(); ++i) {			
			if (sorted_nodes[i]->priority > sorted_nodes[max_prio_idx]->priority) {
				max_prio_idx = i;
			}
		}

		// 2. Этот узел становится корнем
		std::unique_ptr<Node> root = std::move(sorted_nodes[max_prio_idx]);

		// 3. Разделяем оставшиеся узлы на левые и правые (по ключу)
		std::vector<std::unique_ptr<Node>> left_nodes;
		std::vector<std::unique_ptr<Node>> right_nodes;

		for (size_t i = 0; i < sorted_nodes.size(); ++i) {
			if (i == max_prio_idx) continue;  // Пропускаем корень

			if (sorted_nodes[i]->key < root->key) {
				left_nodes.push_back(std::move(sorted_nodes[i]));
			}
			else {
				right_nodes.push_back(std::move(sorted_nodes[i]));
			}
		}

		// 4. Рекурсивно строим левое и правое поддеревья
		root->left = build_cartesian_from_sorted(std::move(left_nodes));
		root->right = build_cartesian_from_sorted(std::move(right_nodes));

		return root;
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
			next_lvl.reserve(current_lvl.size() * 2);  // Оптимизация

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
		new_node->priority = source->priority;  // копируем приоритет

		new_node->left = clone(source->left.get());
		new_node->right = clone(source->right.get());

		return new_node;
	}

	std::unique_ptr<Node> insert_impl(std::unique_ptr<Node> node, const T& key) {
		// Если дерево пустое - просто создаем новый узел
		if (!node) {
			++node_count;
			return std::make_unique<Node>(key);
		}

		// Разделяем
		auto [left, right] = split(std::move(node), key);

		// Создаем и объединяем
		auto new_node = std::make_unique<Node>(key);
		++node_count;

		// Объединяем левое дерево с новым элементом и потом все вместе с правым деревом
		return merge(merge(std::move(left), std::move(new_node)), std::move(right));
	}

	//--------------Удаление --------------------//

	//Специализации функции получения следующего ключа
	// Для integral
	template<typename U = T>
		requires std::integral<U>
	static U get_next_key(const U& key) {
		if (key == std::numeric_limits<U>::max()) return key;
		return key + 1;
	}

	// Для std::string
	template<typename U = T>
		requires std::same_as<U, std::string>
	static U get_next_key(const U& key) {
		if (key.empty()) return "\x00";

		std::string result = key;
		// Находим последний символ, который можно увеличить
		for (auto it = result.rbegin(); it != result.rend(); ++it) {
			if (*it != std::numeric_limits<unsigned char>::max()) {
				++(*it);
				// Обнуляем все символы справа
				for (auto reset_it = result.rbegin(); reset_it != it; ++reset_it) {
					*reset_it = '\x00';
				}
				return result;
			}
		}

		// Все символы на максимуме - добавляем нулевой байт
		return key + '\x00';
	}

	// Для остальных типов - static_assert
	template<typename U = T>
	static U get_next_key(const U& key) {
		static_assert(
			std::is_integral_v<U> || std::is_same_v<U, std::string>,
			"Treap requires get_next_key() for type T. "
			"Either use int/long/std::string, or specialize get_next_key() for your type."
			);
		return key;
	}

	//собственно реализация удаления
	std::unique_ptr<Node> remove_impl(std::unique_ptr<Node> node, const T& key) {
		// 1. Разделяем на < key и >= key
		auto [left, middle_right] = split(std::move(node), key);

		if (!middle_right) {
			// key не найден
			return std::move(left);
		}

		// Если key не максимальное значение в диапазоне
		auto next_key = get_next_key(key);
		if (next_key != key) {
			// Разделяем middle_right на < next_key и >= next_key
			// Поскольку middle_right содержит >= key, то < next_key будет только == key
			auto [middle, right] = split(std::move(middle_right), next_key);

			if (middle) {
				--node_count;
				// middle должен содержать key
			}

			return merge(std::move(left), std::move(right));
		}

		// 3. key максимальный - особый случай		
		// key должен быть корнем middle_right (как наименьший в >= key)
		if (middle_right->key == key) {
			// Нашли! Удаляем корень
			--node_count;
			auto right = std::move(middle_right->right);
			// middle_right (содержащий key) уничтожится
			return merge(std::move(left), std::move(right));
		}
		else {
			// key нет, хотя он максимальный
			return merge(std::move(left), std::move(middle_right));
		}
	}

	void check_and_rebuild() {
		if constexpr (Strategy == RebuildStrategy::NEVER) {
			return;
		}

		bool should_rebuild = false;

		if constexpr (Strategy == RebuildStrategy::OPERATION_COUNT ||
			Strategy == RebuildStrategy::HYBRID) {
			if (operations_since_rebuild >= REBUILD_INTERVAL) {
				should_rebuild = true;
			}
		}

		if constexpr (Strategy == RebuildStrategy::HEIGHT_THRESHOLD ||
			Strategy == RebuildStrategy::HYBRID) {
			double max_allowed_height = HEIGHT_FACTOR * std::log2(node_count + 1);
			if (height() > max_allowed_height) {
				should_rebuild = true;
			}
		}

		if (should_rebuild) {
			rebuild_optimal();
			operations_since_rebuild = 0;
		}
	}


protected:
	std::unique_ptr<Node> root = nullptr;
	size_t node_count = 0;

	// Данные для стратегий
	size_t operations_since_rebuild = 0;
	static constexpr size_t REBUILD_INTERVAL = 1000;
	static constexpr double HEIGHT_FACTOR = 2.0; // 2 * log2(n)
};