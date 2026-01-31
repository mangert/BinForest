#pragma once
#include <iostream>
#include <memory>
#include <concepts>
#include "ITree.h"
#include <numeric>
#include <algorithm>
#include <stack>
#include <queue>
#include <vector>
#include <stdexcept>

template <std::totally_ordered T>
class OptimalBST : public ITree<T> {

protected:
    struct Node {
        T key;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        explicit Node(const T& k)
            : key(k), left(nullptr), right(nullptr) {
        }

        // Запрещаем копирование
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        // Разрешаем перемещение
        Node(Node&&) noexcept = default;
        Node& operator=(Node&&) noexcept = default;

        ~Node() = default;
    };

public:
    //--------- Конструкторы -------//

    // Основной конструктор для построения оптимального дерева
    OptimalBST(const std::vector<T>& keys,
        const std::vector<double>& key_probs) {

        if (keys.empty()) return;

        if (keys.size() != key_probs.size()) {
            throw std::invalid_argument(
                "Keys and probabilities must have same size");
        }

        // Копируем данные (чтобы не менять входные параметры)
        std::vector<T> sorted_keys = keys;
        std::vector<double> sorted_probs = key_probs;

        // 1. Сортируем если нужно
        if (!is_sorted(sorted_keys)) {
            sort_keys_with_probs(sorted_keys, sorted_probs);
        }

        // 2. Нормализуем вероятности
        normalize_probabilities(sorted_probs);
        
        std::cout << "\nВероятности после нормализации:" << std::endl;
        for (size_t i = 0; i < sorted_probs.size(); ++i) {
            std::cout << "p[" << i << "] = " << sorted_probs[i] << std::endl;
        }

        // 3. Создаем q = нули (упрощенный вариант)
        std::vector<double> q(sorted_keys.size() + 1, 0.0);

        // 4. Строим дерево
        build_optimal_bst(sorted_keys, sorted_probs, q);
    }

    // Конструктор копирования
    OptimalBST(const OptimalBST& other)
        : root(clone(other.root.get())), node_count(other.node_count) {
    }

    // Конструктор перемещения
    OptimalBST(OptimalBST&& other) noexcept
        : root(std::move(other.root)), node_count(other.node_count) {
        other.node_count = 0;
    }

    virtual ~OptimalBST() {
        clear();
    }

    // Оператор копирующего присваивания
    OptimalBST& operator=(const OptimalBST& other) {
        if (this != &other) {
            root = clone(other.root.get());
            node_count = other.node_count;
        }
        return *this;
    }

    // Оператор перемещающего присваивания
    OptimalBST& operator=(OptimalBST&& other) noexcept {
        if (this != &other) {
            clear();
            root = std::move(other.root);
            node_count = other.node_count;
            other.node_count = 0;
        }
        return *this;
    }

    //--------- Основные операции -------//

    void insert(const T& key) override {
        // Optimal BST не поддерживает динамическую вставку
        // Можно либо игнорировать, либо бросать исключение
        // throw std::runtime_error("OptimalBST doesn't support insert()");
    }

    bool contains(const T& key) const override {
        const Node* current = root.get();
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
    }

    void remove(const T& key) override {
        // Optimal BST не поддерживает удаление
        // throw std::runtime_error("OptimalBST doesn't support remove()");
    }

    void clear() override {
        if (!root) return;

        std::stack<std::unique_ptr<Node>> node_stack;
        node_stack.push(std::move(root));

        while (!node_stack.empty()) {
            auto node = std::move(node_stack.top());
            node_stack.pop();

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

    bool empty() const override {
        return !root;
    }

    size_t size() const override {
        return node_count;
    }

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

    // --------- Visitor методы --------- //    

    void visit_inorder(std::function<void(const T&)> visitor) const override {
        if (visitor) inorder_impl(visitor);
    }

    void visit_preorder(std::function<void(const T&)> visitor) const override {
        if (visitor) preorder_impl(visitor);
    }

    void visit_postorder(std::function<void(const T&)> visitor) const override {
        if (visitor) postorder_impl(visitor);
    }

    void visit_level_order(std::function<void(const T&)> visitor) const override {
        if (visitor) level_order_impl(visitor);
    }

    //--------- Метрики -------//

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

    //--------- Дополнительные методы для OBST ---------//

    double expected_search_cost() const {
        // Заглушка - нужно будет реализовать с учетом вероятностей
        return compute_expected_cost(root.get(), 1);
    }

    double get_expected_cost() const {        
        return expected_cost_;
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

protected:
    //вспомогательные функции для построения дерева    
    
    // Функция для сортировки ключей с вероятностями
    static void sort_keys_with_probs(std::vector<T>& keys,
		std::vector<double>& probs) {
		
        std::vector<std::pair<T, double>> pairs;
		pairs.reserve(keys.size());
        
        for (size_t i = 0; i < keys.size(); ++i) {
            pairs.emplace_back(keys[i], probs[i]);
		}

		std::sort(pairs.begin(), pairs.end());

	    for (size_t i = 0; i < pairs.size(); ++i) {
		    keys[i] = pairs[i].first;
			probs[i] = pairs[i].second;
		}
	}

    // Функция для проверки, отсортированы ли ключи
    static bool is_sorted(const std::vector<T>& keys) {
        for (size_t i = 1; i < keys.size(); ++i) {
            if (keys[i] < keys[i - 1]) {  // Используем < для strict ordering
                return false;
            }
        }
        return true;
    }

    // Функция для нормализации вероятностей
    static void normalize_probabilities(std::vector<double>& probs) {
        double sum = std::accumulate(probs.begin(), probs.end(), 0.0);

        if (std::abs(sum - 1.0) > 1e-9) {
            // Если сумма близка к 0, делаем равномерное распределение
            if (sum < 1e-9) {
                std::fill(probs.begin(), probs.end(), 1.0 / probs.size());
            }
            else {
                for (auto& prob : probs) {
                    prob /= sum;
                }
            }
        }
    }

//функция построения дерева    
    void build_optimal_bst(const std::vector<T>& keys,
        const std::vector<double>& p,
        const std::vector<double>& q) {

        size_t n = keys.size();
        if (n == 0) {
            root = nullptr;
            node_count = 0;
            expected_cost_ = 0.0;
            return;
        }

        // Проверка размеров (для безопасности)
        if (p.size() != n || q.size() != n + 1) {
            throw std::invalid_argument("Invalid probability arrays size");
        }

#ifdef DEBUG // Проверка сортированности (assert для отладки)
        assert(is_sorted(keys) && "Keys must be sorted in build_optimal_bst");
#endif // DEBUG
            
        // Таблицы ДП
        std::vector<std::vector<double>> e(n + 2,
            std::vector<double>(n + 1, 0.0));
        std::vector<std::vector<double>> w(n + 2,
            std::vector<double>(n + 1, 0.0));
        std::vector<std::vector<int>> root_table(n + 1,
            std::vector<int>(n + 1, 0));

        constexpr double INF = std::numeric_limits<double>::max();

        // 1. Инициализация (q[i-1] = 0 в упрощенном варианте)
        for (int i = 1; i <= n + 1; ++i) {
            e[i][i - 1] = q[i - 1];
            w[i][i - 1] = q[i - 1];
        }

        // 2. Основной цикл ДП
        for (int length = 1; length <= n; ++length) {
            for (int i = 1; i <= n - length + 1; ++i) {
                int j = i + length - 1;

                // w[i][j] = сумма вероятностей от i до j
                w[i][j] = w[i][j - 1] + p[j - 1]; // p[j-1] т.к. p 0-based

                // Границы поиска корня с оптимизацией Кнута
                int left = (i <= j - 1) ? root_table[i][j - 1] : i;
                int right = (i + 1 <= j) ? root_table[i + 1][j] : j;

                // Ищем оптимальный корень
                e[i][j] = INF;
                for (int r = left; r <= right; ++r) {
                    double cost = e[i][r - 1] + e[r + 1][j] + w[i][j];
                    if (cost < e[i][j]) {
                        e[i][j] = cost;
                        root_table[i][j] = r;
                    }
                }
            }
        }

        std::cout << "\nТаблица root:" << std::endl;
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= n; ++j) {
                if (i <= j) {
                    std::cout << root_table[i][j] << " ";
                }
                else {
                    std::cout << "  ";
                }
            }
            std::cout << std::endl;
        }

        // 3. Сохраняем минимальную ожидаемую стоимость
        expected_cost_ = e[1][n];

        // 4. Строим дерево
        root = build_tree_from_roots(keys, root_table, 1, n);
        node_count = n;
    }

    //вспомогательная рекурсивная функция построения поддерева
    std::unique_ptr<Node> build_tree_from_roots(
        const std::vector<T>& keys,
        const std::vector<std::vector<int>>& root_table,
        int i, int j) {

        if (i > j) return nullptr;

        int root_idx = root_table[i][j];  // 1-based индекс в keys
        // keys 0-based, поэтому keys[root_idx-1]
        auto node = std::make_unique<Node>(keys[root_idx - 1]);

        // Рекурсивно строим левое и правое поддеревья
        node->left = build_tree_from_roots(keys, root_table, i, root_idx - 1);
        node->right = build_tree_from_roots(keys, root_table, root_idx + 1, j);

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

protected:
	std::unique_ptr<Node> root = nullptr;
	size_t node_count = 0;
    double expected_cost_ = 0.0;
};

