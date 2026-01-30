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
    OptimalBST(const std::vector<T>& sorted_keys,
        const std::vector<double>& key_probs,
        const std::vector<double>& gap_probs = {}) {

        // Проверки корректности входных данных
        if (sorted_keys.empty()) return;

        if (sorted_keys.size() != key_probs.size()) {
            throw std::invalid_argument("Keys and probabilities must have same size");
        }

        // Проверка сортированности
        for (size_t i = 1; i < sorted_keys.size(); ++i) {
            if (sorted_keys[i - 1] >= sorted_keys[i]) {
                throw std::invalid_argument("Keys must be sorted in ascending order");
            }
        }

        // 1. Вызываем алгоритм ДП
        build_optimal_bst(sorted_keys, key_probs, gap_probs);
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
    void build_optimal_bst(const std::vector<T>& keys,
        const std::vector<double>& p,
        const std::vector<double>& q = {}) {
        size_t n = keys.size();

        if (n == 0) {
            root = nullptr;
            node_count = 0;
            return;
        }

        // ВРЕМЕННАЯ ЗАГЛУШКА: строим просто сбалансированное дерево
        // ЗАМЕНИТЕ на реальный алгоритм ДП позже
        root = build_balanced(keys, 0, n - 1);
        node_count = n;
    }

    std::unique_ptr<Node> build_balanced(const std::vector<T>& keys, int start, int end) {
        if (start > end) return nullptr;

        int mid = start + (end - start) / 2;
        auto node = std::make_unique<Node>(keys[mid]);
        node->left = build_balanced(keys, start, mid - 1);
        node->right = build_balanced(keys, mid + 1, end);
        return node;
    }

    double compute_expected_cost(const Node* node, int depth) const {
        if (!node) return 0.0;

        // Заглушка: считаем просто сумму глубин
        return depth +
            compute_expected_cost(node->left.get(), depth + 1) +
            compute_expected_cost(node->right.get(), depth + 1);
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
};

