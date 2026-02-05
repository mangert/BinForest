#pragma once
#include <iostream>
#include <memory>
#include <concepts>
#include "BSTree.h"
#include <stack>
#include <queue>
#include <numeric>
#include <algorithm>


template<std::totally_ordered T>
class SplayTree : public BSTree<T> {
public:
    // Наследуем конструкторы
    using BSTree<T>::BSTree;

    // Вставка элемента
    void insert(const T& key) override {
        if (!this->root) {
            // Пустое дерево
            this->root = std::make_unique<typename BSTree<T>::Node>(key);
            this->node_count = 1;
            return;
        }

        // 1. Сначала делаем splay - поднимаем ближайший элемент в корень
        this->root = splay(std::move(this->root), key);

        // 2. Если ключ уже существует
        if (this->root->key == key) {
            return;  // Дубликат
        }

        // 3. Создаём новый узел
        auto new_node = std::make_unique<typename BSTree<T>::Node>(key);

        // 4. Вставляем новый узел в корень
        if (key < this->root->key) {
            // Ключ меньше корня
            auto tmp = std::move(this->root->left);
            new_node->right = std::move(this->root);            
            new_node->left = std::move(tmp);
            this->root = std::move(new_node);
        }
        else {
            // Ключ больше корня
            auto tmp = std::move(this->root->right);
            new_node->left = std::move(this->root);
            new_node->right = std::move(tmp);
            this->root = std::move(new_node);
        }

        ++this->node_count;        
    }    

    //удаление элемента
    void remove(const T& key) override {
        if (!this->root) return;

        // 1. Сначала делаем splay (даже если ключа нет)
        this->root = splay(std::move(this->root), key);

        // 2. Если ключ не найден после splay - ничего не делаем
        if (this->root->key != key) {
            return;  // Ключа нет, но дерево уже изменилось (splay ближайшего)
        }

        // 3. Ключ найден (он в корне после splay)
        // Удаляем корень

        if (!this->root->left) {
            // Нет левого поддерева
            this->root = std::move(this->root->right);
        }
        else if (!this->root->right) {
            // Нет правого поддерева
            this->root = std::move(this->root->left);
        }
        else {
            // Есть оба поддерева
            // Сохраняем правое поддерево
            auto right_subtree = std::move(this->root->right);

            // Делаем splay в левом поддереве, чтобы максимальный элемент стал корнем
            this->root = splay(std::move(this->root->left), key);
            // Теперь в корне левого поддерева максимальный элемент

            // Присоединяем правое поддерево
            this->root->right = std::move(right_subtree);
        }

        --this->node_count;
    }

    //функция splay-поиска
    bool find_and_splay(const T& key) {
        this->root = splay(std::move(this->root), key);
        return this->root && this->root->key == key;
    }

protected:    
    // Итеративный splay
    std::unique_ptr<typename BSTree<T>::Node> splay(
        std::unique_ptr<typename BSTree<T>::Node> root, const T& key) {
        if (!root) return nullptr;

        auto left_dummy = std::make_unique<typename BSTree<T>::Node>(T{});
        auto right_dummy = std::make_unique<typename BSTree<T>::Node>(T{});
        typename BSTree<T>::Node* left_tail = left_dummy.get();
        typename BSTree<T>::Node* right_tail = right_dummy.get();

        std::unique_ptr<typename BSTree<T>::Node> current = std::move(root);

        while (true) {
            if (key < current->key) {
                if (!current->left) break;

                if (key < current->left->key) {
                    // ZIG-ZIG: делаем правый поворот
                    auto child = std::move(current->left);
                    current->left = std::move(child->right);
                    child->right = std::move(current);
                    current = std::move(child);

                    if (!current->left) break;
                }

                // Подвешиваем current в правое дерево
                right_tail->left = std::move(current);
                right_tail = right_tail->left.get();
                current = std::move(right_tail->left);

            }
            else if (key > current->key) {
                if (!current->right) break;

                if (key > current->right->key) {
                    // ZAG-ZAG: делаем левый поворот
                    auto child = std::move(current->right);
                    current->right = std::move(child->left);
                    child->left = std::move(current);
                    current = std::move(child);

                    if (!current->right) break;
                }

                // Подвешиваем current в левое дерево
                left_tail->right = std::move(current);
                left_tail = left_tail->right.get();
                current = std::move(left_tail->right);

            }
            else {
                break;
            }
        }

        // Сборка
        left_tail->right = std::move(current->left);
        right_tail->left = std::move(current->right);

        current->left = std::move(left_dummy->right);
        current->right = std::move(right_dummy->left);

        return current;
    }
};