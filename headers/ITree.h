#pragma once
#include <functional>
#include <vector>
#include <concepts>
#include <memory>

template <typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a == b } -> std::convertible_to<bool>;
};

template <Comparable T>
class ITree {
public:
    virtual ~ITree() = default;

    // === Основные операции ===
    virtual void insert(const T& key) = 0;
    virtual bool contains(const T& key) const = 0;
    virtual void remove(const T& key) = 0;
    virtual void clear() = 0;

    // === Состояние дерева ===
    virtual bool empty() const = 0;
    virtual std::size_t size() const = 0;

    // === Обходы дерева ===

    // Посетитель (visitor) - более гибкий вариант
    template <typename Visitor>
    void inorderVisit(Visitor&& visitor) const {
        std::vector<T> elements = inorder();
        for (const auto& elem : elements) {
            visitor(elem);
        }
    }

    // Стандартные обходы (возвращают вектор элементов)
    virtual std::vector<T> inorder() const = 0;     // Лево-Узел-Право (отсортировано для BST)
    virtual std::vector<T> preorder() const = 0;    // Узел-Лево-Право
    virtual std::vector<T> postorder() const = 0;   // Лево-Право-Узел
    virtual std::vector<T> levelOrder() const = 0;  // По уровням (BFS)

    // === Информация о дереве ===
    virtual int height() const = 0;
    virtual bool isBalanced() const = 0;  // Для BST просто проверка, для AVL - баланс

    // === Специальные операции (для декартова дерева) ===
    // Можно вынести в отдельный интерфейс ITreap, если нужны операции по индексу

    // === Поддержка range-based for ===
    // Можно добавить итераторы, но для начала достаточно методов выше

protected:
    // Вспомогательные protected методы, если нужны общие утилиты
    virtual void printTree(std::ostream& os, const std::string& prefix = "", bool isLeft = false) const {
        // Базовая реализация может быть пустой
        // Каждое дерево реализует свою визуализацию
    }

    // Дружественная функция для вывода
    friend std::ostream& operator<<(std::ostream& os, const ITree& tree) {
        tree.printTree(os);
        return os;
    }
};

// Алиас для удобства
template <Comparable T>
using TreePtr = std::unique_ptr<ITree<T>>;