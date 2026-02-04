#pragma once
#include "BSTree.h"  // Предполагается, что у вас класс называется BSTree, а не BST
#include <random>
#include <memory>
#include <concepts>

// Концепт для проверки параметра вероятности
template<double P>
concept ValidProbability = (P >= 0.0 && P <= 1.0);

enum class ProbabilityStrategy { //стратегии для вероятностей
    FIXED,           // Фиксированная вероятность
    INVERSE_N,       // 1/n
    INVERSE_SQRT_N,  // 1/√n
    INVERSE_LOG_N    // 1/log n
};

//собственно шаблонный класс рандомизированного дерева
template<std::totally_ordered T,
    ProbabilityStrategy Strategy = ProbabilityStrategy::FIXED,
    double Param = 0.1>  // Для FIXED - значение вероятности
requires(Strategy != ProbabilityStrategy::FIXED || ValidProbability<Param>)

class RandomizedBSTree : public BSTree<T> {
public:
    RandomizedBSTree() = default;

    // Наследуем конструкторы
    using BSTree<T>::BSTree;

    void insert(const T& key) override {
        // Пробуем вставить
        bool inserted = try_insert(key);

        if (inserted && should_splay_to_root(this->node_count - 1)) {
            // Перемещаем в корень
            this->root = splay_to_root(std::move(this->root), key);
        }        
    }   

protected:
    // Попытка вставки (возвращает true, если элемент был добавлен)
    bool try_insert(const T& key) {
        if (this->insert_impl(key)) {
            ++this->node_count;
            return true;
        }
        return false;
    }
    
    //определяем, надо ли поворачивать
    bool should_splay_to_root(size_t old_size) const {
        
        if (old_size == 0) return false;

        double probability = compute_probability(old_size);
        if (probability == 0.0) return false;
        if (probability == 1.0) return true;

        static std::mt19937 rng(std::random_device{}());
        static std::uniform_real_distribution<double> dist(0.0, 1.0);

        return dist(rng) < probability;
    }
    //определение вероятности
    double compute_probability(size_t n) const {
        if constexpr (Strategy == ProbabilityStrategy::FIXED) {
            return Param;
        }
        else if constexpr (Strategy == ProbabilityStrategy::INVERSE_N) {
            return 1.0 / (n + 1);
        }
        else if constexpr (Strategy == ProbabilityStrategy::INVERSE_SQRT_N) {
            return 1.0 / std::sqrt(n + 1);
        }
        else if constexpr (Strategy == ProbabilityStrategy::INVERSE_LOG_N) {
            return 1.0 / std::log2(n + 2);
        };        
        return 0;
    }

    // Перемещение узла с ключом key в корень
    std::unique_ptr<typename BSTree<T>::Node> splay_to_root(
        std::unique_ptr<typename BSTree<T>::Node> root, const T& key) {
        
        if (!root || root->key == key) return root;

        std::unique_ptr<typename BSTree<T>::Node> dummy_left, dummy_right;
        typename BSTree<T>::Node* left_tree_max = nullptr;
        typename BSTree<T>::Node* right_tree_min = nullptr;

        // Временные узлы для сборки дерева
        auto left_dummy = std::make_unique<typename BSTree<T>::Node>(T{});
        auto right_dummy = std::make_unique<typename BSTree<T>::Node>(T{});
        typename BSTree<T>::Node* left = left_dummy.get();
        typename BSTree<T>::Node* right = right_dummy.get();

        auto current = std::move(root);

        while (current) {
            if (key < current->key) {
                if (current->left && key < current->left->key) {
                    // Zig-Zig: правый поворот
                    current = rotate_right(std::move(current));
                }
                if (!current->left) break;

                // Присоединяем current к правому дереву
                right->left = std::move(current);
                right = right->left.get();
                current = std::move(right->left);
            }
            else if (key > current->key) {
                if (current->right && key > current->right->key) {
                    // Zag-Zag: левый поворот
                    current = rotate_left(std::move(current));
                }
                if (!current->right) break;

                // Присоединяем current к левому дереву
                left->right = std::move(current);
                left = left->right.get();
                current = std::move(left->right);
            }
            else {
                break;
            }
        }

        // Собираем дерево
        left->right = current ? std::move(current->left) : nullptr;
        right->left = current ? std::move(current->right) : nullptr;

        if (current) {
            current->left = std::move(left_dummy->right);
            current->right = std::move(right_dummy->left);
        }

        return current ? std::move(current) : std::move(current ? std::move(current) : std::move(left_dummy->right));
    }

    // Правый поворот
    std::unique_ptr<typename BSTree<T>::Node>
        rotate_right(std::unique_ptr<typename BSTree<T>::Node> x) {
        
        if (!x || !x->left) return x;
        auto y = std::move(x->left);
        x->left = std::move(y->right);
        y->right = std::move(x);
        return y;
    }

    // Левый поворот
    std::unique_ptr<typename BSTree<T>::Node>
        rotate_left(std::unique_ptr<typename BSTree<T>::Node> x) {        
        
        if (!x || !x->right) return x;
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        y->left = std::move(x);
        return y;
    }
};