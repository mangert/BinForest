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

        // Фиктивные узлы для левого и правого деревьев
        auto left_dummy = std::make_unique<typename BSTree<T>::Node>(T{});
        auto right_dummy = std::make_unique<typename BSTree<T>::Node>(T{});
        typename BSTree<T>::Node* left_max = left_dummy.get();
        typename BSTree<T>::Node* right_min = right_dummy.get();

        std::unique_ptr<typename BSTree<T>::Node> current = std::move(root);

        // Пока не найдём ключ или не дойдём до листа
        while (current && current->key != key) {
            if (key < current->key) {
                // Если нет левого ребёнка - ключ не найден
                if (!current->left) {
                    break;
                }

                // ZIG-ZIG (key в левом-левом поддереве)
                if (key < current->left->key) {
                    // Первый правый поворот
                    current = rotate_right(std::move(current));
                    if (!current->left) break;

                    // Второй правый поворот
                    current = rotate_right(std::move(current));
                    if (!current->left) break;
                }
                // ZIG (key в левом поддереве, но не в левом-левом)
                else {
                    // Подготавливаем ZIG: перемещаем current в правое дерево
                    right_min->left = std::move(current);
                    right_min = right_min->left.get();
                    current = std::move(right_min->left);
                    if (!current) break;

                    // На следующей итерации будет ZIG (правый поворот)
                    // или ZIG-ZIG/ZIG-ZAG в зависимости от нового current
                }
            }
            else { // key > current->key (равенство исключено условием цикла)
                // Симметрично для правого поддерева
                if (!current->right) {
                    break;
                }

                // ZAG-ZAG (key в правом-правом поддереве)
                if (key > current->right->key) {
                    current = rotate_left(std::move(current));
                    if (!current->right) break;
                    current = rotate_left(std::move(current));
                    if (!current->right) break;
                }
                // ZAG (key в правом поддереве, но не в правом-правом)
                else {
                    left_max->right = std::move(current);
                    left_max = left_max->right.get();
                    current = std::move(left_max->right);
                    if (!current) break;
                }
            }
        }

        // Сборка финального дерева
        // current - либо узел с key, либо ближайший к нему

        if (current) {
            // Присоединяем поддеревья current к временным деревьям
            left_max->right = std::move(current->left);
            right_min->left = std::move(current->right);

            // Собираем: left_dummy.right <- current -> right_dummy.left
            current->left = std::move(left_dummy->right);
            current->right = std::move(right_dummy->left);
            return current;
        }

        // Ключ не найден - возвращаем ближайший элемент
        if (left_dummy->right) {
            return std::move(left_dummy->right);
        }
        return std::move(right_dummy->left);
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