#include <iostream>
#include "BSTree.h"
#include "TreeTest.h"
#include "AVLTree.h"
#include "Treap.h"
#include "OptimalBST.h"
#include <cassert>
#include <set>

#include <vector>
#include <random>
#include <numeric>


void test_various_distributions() {
    std::cout << "=== Тест 1: Вырожденный случай ===" << std::endl;
    {
        std::vector<int> keys = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        std::vector<double> probs = { 0.9, 0.02, 0.02, 0.02, 0.02,
                                     0.02, 0.02, 0.02, 0.02, 0.02 };

        OptimalBST<int> tree(keys, probs);
        std::cout << "Размер: " << tree.size() << std::endl;
        std::cout << "Высота: " << tree.height() << std::endl;
        std::cout << "Ожидаемая стоимость: " << tree.get_expected_cost()
            << std::endl;

        auto inorder = tree.inorder();
        std::cout << "Inorder (первые 5): ";
        for (size_t i = 0; i < std::min(inorder.size(), size_t(5)); ++i) {
            std::cout << inorder[i] << " ";
        }
        std::cout << "...\n" << std::endl;
    }

    std::cout << "=== Тест 2: Равномерное распределение ===" << std::endl;
    {
        std::vector<int> keys = { 5, 15, 25, 35, 45, 55, 65, 75, 85, 95 };
        std::vector<double> probs(10, 0.1);

        OptimalBST<int> tree(keys, probs);
        std::cout << "Высота сбалансированного дерева: " << tree.height()
            << " (ожидается ~3)" << std::endl;
        std::cout << "Ожидаемая стоимость: " << tree.get_expected_cost()
            << std::endl << std::endl;
    }

    std::cout << "=== Тест 3: Нормальное распределение ===" << std::endl;
    {
        std::vector<int> keys = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
        std::vector<double> probs = { 0.02, 0.05, 0.08, 0.12, 0.16,
                                     0.16, 0.12, 0.08, 0.05, 0.02 };

        OptimalBST<int> tree(keys, probs);
        std::cout << "Корень дерева (первый элемент preorder): ";
        auto preorder = tree.preorder();
        if (!preorder.empty()) {
            std::cout << preorder[0] << " (ожидается ~50 или 60)" << std::endl;
        }
        std::cout << "Ожидаемая стоимость: " << tree.get_expected_cost()
            << std::endl << std::endl;
    }

    std::cout << "=== Тест 4: Случайные ключи с Зипфом ===" << std::endl;
    {
        std::vector<int> keys;
        std::vector<double> probs;
        std::mt19937 gen(42);
        std::uniform_int_distribution<> dis(0, 1000);

        for (int i = 0; i < 20; ++i) {
            keys.push_back(dis(gen));
            probs.push_back(1.0 / (i + 1));
        }

        OptimalBST<int> tree(keys, probs);
        std::cout << "Размер: " << tree.size() << std::endl;
        std::cout << "Высота: " << tree.height() << std::endl;
        std::cout << "Содержит ли 42? " << (tree.contains(42) ? "да" : "нет")
            << std::endl << std::endl;
    }

    std::cout << "=== Тест 5: Буквы алфавита ===" << std::endl;
    {
        std::vector<char> keys = { 'e', 't', 'a', 'o', 'i', 'n', 's', 'h', 'r', 'd' };
        std::vector<double> probs = { 12.7, 9.1, 8.2, 7.5, 7.0, 6.7, 6.3, 6.1, 6.0, 4.3 };

        // Отладочный вывод до нормализации
        std::cout << "До нормализации:" << std::endl;
        for (size_t i = 0; i < keys.size(); ++i) {
            std::cout << keys[i] << ": " << probs[i] << "% ";
        }
        std::cout << "\nСумма: " << std::accumulate(probs.begin(), probs.end(), 0.0)
            << "%" << std::endl;

        OptimalBST<char> tree(keys, probs);

        // Отладочный вывод после нормализации (если есть доступ)
        std::cout << "Корень: " << tree.preorder()[0] << std::endl;
        std::cout << "Весь preorder: ";
        for (char c : tree.preorder()) std::cout << c << " ";
        std::cout << std::endl;

        // Также можно вывести inorder
        std::cout << "Inorder (должен быть отсортирован): ";
        for (char c : tree.inorder()) std::cout << c << " ";
        std::cout << std::endl;
    }

    std::cout << "=== Простой детерминированный тест ===" << std::endl;
    {
        // 3 ключа, где разница очевидна
        std::vector<char> keys = { 'a', 'b', 'c' };
        std::vector<double> probs = { 0.7, 0.2, 0.1 }; // 'a' явно самый частый

        OptimalBST<char> tree(keys, probs);
        auto preorder = tree.preorder();

        std::cout << "Ключи: a(0.7), b(0.2), c(0.1)" << std::endl;
        std::cout << "Preorder: ";
        for (char c : preorder) std::cout << c << " ";
        std::cout << std::endl;

        if (preorder[0] == 'a') {
            std::cout << "✓ Корень 'a' - правильно!" << std::endl;
        }
        else {
            std::cout << "✗ Ошибка! Корень должен быть 'a', а получился '"
                << preorder[0] << "'" << std::endl;

            // Выведем дерево для анализа
            std::cout << "Структура дерева:" << std::endl;
            tree.print();
        }
    }
}

void test_correctness() {
    std::cout << "\n=== Проверка корректности ===" << std::endl;

    // Простой пример для ручной проверки
    std::vector<int> keys = { 10, 20, 30 };
    std::vector<double> probs = { 0.5, 0.3, 0.2 };  // 10 самый частый

    OptimalBST<int> tree(keys, probs);

    std::cout << "Ключи: 10(0.5), 20(0.3), 30(0.2)" << std::endl;
    std::cout << "Preorder (корень первый): ";
    for (int k : tree.preorder()) std::cout << k << " ";
    std::cout << std::endl;

    // Ожидаем: корень 10, т.к. он самый частый
    if (!tree.preorder().empty() && tree.preorder()[0] == 10) {
        std::cout << "✓ Корень верный (10)" << std::endl;
    }
    else {
        std::cout << "✗ Ожидался корень 10" << std::endl;
    }

    // Проверка поиска
    std::cout << "\nПроверка поиска:" << std::endl;
    std::cout << "contains(10): " << tree.contains(10) << std::endl;
    std::cout << "contains(20): " << tree.contains(20) << std::endl;
    std::cout << "contains(15): " << tree.contains(15) << std::endl;
}

void test_simple_obst() {
    // Ключи и их вероятности
    std::vector<int> keys = { 10, 20, 30, 40 };

    // Вероятности (не нормализованы - алгоритм сам нормализует)
    std::vector<double> probs = { 0.4, 0.1, 0.2, 0.3 };
    // После нормализации: ~0.4, 0.1, 0.2, 0.3 (сумма 1.0)

    // Строим OBST
    OptimalBST<int> tree(keys, probs);

    std::cout << "Tree size: " << tree.size() << std::endl;
    std::cout << "Tree height: " << tree.height() << std::endl;
    std::cout << "Expected search cost: "
        << tree.get_expected_cost() << std::endl;

    // Проверяем поиск
    std::cout << "Contains 20: " << tree.contains(20) << std::endl;
    std::cout << "Contains 25: " << tree.contains(25) << std::endl;

    // Обходы
    auto inorder = tree.inorder();
    std::cout << "Inorder: ";
    for (int key : inorder) std::cout << key << " ";
    std::cout << std::endl;

    // Печатаем дерево
    tree.print();
}

int main() {
	setlocale(LC_ALL, "Russian");
    /*
    int n = 1000000; //количество элементов для тестов

    std::cout << "\nТестирование функционала деревьев\n\n";
    std::cout << "-------- Часть 1: n= " << n << "\n";
    std::cout << "****** BSTree **********************\n";
    
    TreeTest<int, BSTree<int>>::comprehensive_test(n);    
    

    std::cout << "****** AVLTree *********************\n";

    TreeTest<int, AVLTree<int>>::comprehensive_test(n);
    

    std::cout << "****** Treap ***********************\n";

    TreeTest<int, Treap<int>>::comprehensive_test(n);

    n = n * 10;
    std::cout << "-------- Часть 2: n= " << n << "\n";
    std::cout << "(BSTree не участвует в связи с длительным временем)\n";
    
    std::cout << "****** AVLTree *********************\n";

    TreeTest<int, AVLTree<int>>::comprehensive_test(n);


    std::cout << "****** Treap ***********************\n";

    TreeTest<int, Treap<int>>::comprehensive_test(n);*/

    test_simple_obst();
    test_various_distributions();
    test_correctness();

}