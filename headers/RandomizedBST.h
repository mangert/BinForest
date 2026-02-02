#pragma once
#include "BSTree.h"  // Предполагается, что у вас класс называется BSTree, а не BST
#include <random>
#include <memory>

template <std::totally_ordered T>
class RandomizedBSTree : public BSTree<T> {
public:
    RandomizedBSTree() = default;

    // Наследуем конструкторы
    using BSTree<T>::BSTree;

    void insert(const T& key) override {
        // Пробуем вставить
        bool inserted = try_insert(key);

        if (inserted && should_splay_to_root()) {
            // Перемещаем в корень
            this->root = splay_to_root(std::move(this->root), key);
        }
        //std::cout << "Insert " << key << " ********\n";
        //this->print();
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

    // Генератор случайных чисел
    static std::mt19937& get_rng() {
        static std::mt19937 rng(std::random_device{}());
        return rng;
    }

    // Вероятность перемещения в корень
    static bool should_splay_to_root() {
        static std::uniform_real_distribution<double> dist(0.0, 1.0);
        // Можно сделать вероятность 1/(n+1), но для простоты фиксированная
        return dist(get_rng()) < 0.25f; // 25%
        
    }

    // Перемещение узла с ключом key в корень
    std::unique_ptr<typename BSTree<T>::Node>
        splay_to_root(std::unique_ptr<typename BSTree<T>::Node> node, const T& key) {
        if (!node || node->key == key) {
            return node;
        }

        // Ключ в левом поддереве
        if (key < node->key) {
            if (!node->left) return node;

            // Zig-Zig (левый-левый)
            if (key < node->left->key) {
                node->left->left = splay_to_root(std::move(node->left->left), key);
                node = rotate_right(std::move(node));
            }
            // Zig-Zag (левый-правый)
            else if (key > node->left->key) {
                node->left->right = splay_to_root(std::move(node->left->right), key);
                if (node->left->right) {
                    node->left = rotate_left(std::move(node->left));
                }
            }

            return node->left ? rotate_right(std::move(node)) : std::move(node);
        }
        // Ключ в правом поддереве
        else {
            if (!node->right) return node;

            // Zag-Zag (правый-правый)
            if (key > node->right->key) {
                node->right->right = splay_to_root(std::move(node->right->right), key);
                node = rotate_left(std::move(node));
            }
            // Zag-Zig (правый-левый)
            else if (key < node->right->key) {
                node->right->left = splay_to_root(std::move(node->right->left), key);
                if (node->right->left) {
                    node->right = rotate_right(std::move(node->right));
                }
            }

            return node->right ? rotate_left(std::move(node)) : std::move(node);
        }
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