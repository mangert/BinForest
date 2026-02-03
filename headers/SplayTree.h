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

    // Переопределяем методы, требующие splay
    void insert(const T& key) override {
        
        bool inserted = this->insert_impl(key);
        if (inserted) {
            ++this->node_count;            
            this->root = splay(std::move(this->root), key);
        }
    }    

    void remove(const T& key) override {
        if (!this->root) return;

        // Splay узел с ключом (или ближайший)
        this->root = splay(std::move(this->root), key);

        if (this->root->key == key) {
            // Удаляем корень
            if (!this->root->left) {
                this->root = std::move(this->root->right);
            }
            else {
                auto right = std::move(this->root->right);
                this->root = splay(std::move(this->root->left), key);
                this->root->right = std::move(right);
            }
            --this->node_count;
        }
    }

    bool find_and_splay(const T& key) {
        return true;
    }

protected:
    // Итеративный splay
    std::unique_ptr<typename BSTree<T>::Node>
        splay(std::unique_ptr<typename BSTree<T>::Node> root, const T& key) const {
        // Итеративная реализация
        // Может быть const, т.к. не меняет сам объект SplayTree,
        // только структуру дерева (что технически меняет состояние,
        // но формально root - mutable?)
        return nullptr;
    }
};