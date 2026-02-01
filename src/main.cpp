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

void final_verification() {
    std::cout << "=== ФИНАЛЬНАЯ ПРОВЕРКА ===" << std::endl;

    // Тест 1: Должен выбрать 20
    {
        std::vector<int> keys = { 10, 20, 30 };
        std::vector<double> probs = { 0.4, 0.3, 0.3 };

        OptimalBST<int> tree(keys, probs);
        std::cout << "Тест {0.4, 0.3, 0.3}: корень = "
            << tree.preorder()[0] << " (ожидается 20)" << std::endl;
    }

    // Тест 2: Теперь 10 явный фаворит
    {
        std::vector<int> keys = { 10, 20, 30 };
        std::vector<double> probs = { 0.7, 0.2, 0.1 };

        OptimalBST<int> tree(keys, probs);
        std::cout << "Тест {0.7, 0.2, 0.1}: корень = "
            << tree.preorder()[0] << " (ожидается 10)" << std::endl;
    }

    // Тест 3: Все равны
    {
        std::vector<int> keys = { 10, 20, 30 };
        std::vector<double> probs = { 0.333, 0.333, 0.334 };

        OptimalBST<int> tree(keys, probs);
        std::cout << "Тест равных: корень = "
            << tree.preorder()[0] << " (может быть 20 или 10)" << std::endl;
    }
}

void test_3_elements_debug() {
    std::vector<int> keys = { 10, 20, 30 };
    std::vector<double> probs = { 0.4, 0.3, 0.3 }; // 10 самый вероятный

    OptimalBST<int> tree(keys, probs);

    std::cout << "Тест: keys={10,20,30}, probs={0.4,0.3,0.3}\n";
    std::cout << "Корень: " << tree.preorder()[0] << std::endl;

    // Вручную:
    // Если корень 10: cost = (0.4*1) + (0.3*2) + (0.3*2) = 1.6
    // Если корень 20: cost = (0.4*2) + (0.3*1) + (0.3*2) = 1.7
    // Если корень 30: cost = (0.4*2) + (0.3*2) + (0.3*1) = 1.7
    // Оптимально: корень 10
}

double compute_expected_cost_manual(const std::vector<double>& probs, int root_idx) {
    // Ручной расчет стоимости для заданного корня
    // Упрощенный: считаем, что дерево идеально сбалансировано относительно этого корня
    double cost = 0;

    for (size_t i = 0; i < probs.size(); ++i) {
        if (i == root_idx) {
            cost += probs[i] * 1; // Корень на глубине 1
        }
        else if (i < root_idx) {
            cost += probs[i] * 2; // Левое поддерево, глубина ~2
        }
        else {
            cost += probs[i] * 2; // Правое поддерево, глубина ~2
        }
    }

    return cost;
}

void test_letter_analysis() {
    std::vector<char> keys = { 'a', 'd', 'e', 'h', 'i', 'n', 'o', 'r', 's', 't' };
    std::vector<double> probs = { 0.111, 0.058, 0.172, 0.083, 0.095,
                                 0.091, 0.101, 0.081, 0.085, 0.123 };
    

    std::cout << "Анализ оптимальности корней:\n";

    // Проверяем несколько кандидатов в корни
    std::vector<int> candidates = { 2, 6, 9 }; // e, o, t (индексы 0-based)

    for (int root_idx : candidates) {
        double cost = compute_expected_cost_manual(probs, root_idx);
        std::cout << "Корень " << keys[root_idx] << " (" << probs[root_idx]
            << "): ожидаемая стоимость ~" << cost << std::endl;
    }

    OptimalBST<char> tree(keys, probs);

    // Отладочный вывод после нормализации (если есть доступ)
    std::cout << "Корень: " << tree.preorder()[0] << std::endl;
    std::cout << "Весь preorder: ";
    for (char c : tree.preorder()) std::cout << c << " ";
    std::cout << std::endl;
}

void test_obvious_case() {
    // 3 элемента, средний имеет намного большую вероятность
    std::vector<int> keys = { 10, 20, 30 };
    std::vector<double> probs = { 0.1, 0.8, 0.1 }; // 20 - явный фаворит

    OptimalBST<int> tree(keys, probs);

    std::cout << "Тест 1: Явный фаворит (0.1, 0.8, 0.1)\n";
    std::cout << "Preorder: ";
    for (int k : tree.preorder()) std::cout << k << " ";
    std::cout << "\nКорень: " << tree.preorder()[0]
        << " (ожидается 20)\n" << std::endl;

    assert(tree.preorder()[0] == 20);
}

void test_uniform() {
    std::vector<int> keys = { 10, 20, 30, 40, 50, 60, 70 };
    std::vector<double> probs(7, 1.0 / 7.0);

    OptimalBST<int> tree(keys, probs);

    std::cout << "Тест 2: Равномерное распределение\n";
    std::cout << "Высота: " << tree.height()
        << " (для 7 элементов сбалансированное: 3)\n";
    std::cout << "Корень: " << tree.preorder()[0]
        << " (ожидается 40 - середина)\n" << std::endl;
}


void test_dp_manual() {
    // Маленький пример для ручной проверки
    std::vector<int> keys = { 10, 20, 30 };
    std::vector<double> probs = { 0.3, 0.4, 0.3 }; // 20 немного вероятнее

    OptimalBST<int> tree(keys, probs);

    std::cout << "Тест 3: Проверка ДП (0.3, 0.4, 0.3)\n";
    std::cout << "Корень: " << tree.preorder()[0] << std::endl;

    // Рассчитаем вручную:
    // Если корень 10: стоимость = 0.3*1 + 0.4*2 + 0.3*2 = 1.7
    // Если корень 20: стоимость = 0.3*2 + 0.4*1 + 0.3*2 = 1.6  
    // Если корень 30: стоимость = 0.3*2 + 0.4*2 + 0.3*1 = 1.7
    // Оптимально: корень 20 с стоимостью 1.6

    if (tree.preorder()[0] == 20) {
        std::cout << "✓ Алгоритм выбрал оптимальный корень 20\n" << std::endl;
    }
}


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
    /*
    test_simple_obst();
    test_various_distributions();
    test_correctness();*/
    
    TreeTest<int, OptimalBST<int>>::comprehensive_test(10);

    /*
    final_verification();
    test_3_elements_debug();
    test_letter_analysis();
    
    test_obvious_case();
        
    test_uniform();

    test_dp_manual();*/

}