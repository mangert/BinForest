#pragma once
#include <concepts>
#include <vector>
#include <functional>

template <std::totally_ordered T>
class ITree {
public:
    virtual ~ITree() = default;

    // Основные операции
    virtual void insert(const T& key) = 0;
    virtual bool contains(const T& key) const = 0;
    virtual void remove(const T& key) = 0;
    virtual void clear() = 0;

    // Состояние
    virtual bool is_empty() const = 0;
    virtual size_t size() const = 0;    

    // Обходы
    virtual std::vector<T> inorder() const = 0;
    virtual std::vector<T> preorder() const = 0;
    virtual std::vector<T> postorder() const = 0;
    virtual std::vector<T> level_order() const = 0;

    // Характеристики
    virtual int height() const = 0;
    //virtual bool is_balanced() const = 0;

    // Опционально: визуализация
    //virtual void print(std::ostream& os) const = 0;
};

template <std::totally_ordered T>
std::ostream& operator<<(std::ostream& os, const ITree<T>& tree) {
    tree.print(os);
    return os;
}