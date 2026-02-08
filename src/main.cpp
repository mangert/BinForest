#include <iostream>
#include "BSTree.h"
#include "TreeTest.h"
#include "AVLTree.h"
#include "Treap.h"
#include "OptimalBST.h"
#include "RandomizedBST.h"
#include "SplayTree.h"
#include "OptimalTreap.h"
#include <cassert>
#include <set>
#include <chrono>
#include <vector>
#include <random>
#include <numeric>

void quick_debug_test() {
    std::cout << "\n=== БЫСТРАЯ ДИАГНОСТИКА ===\n";

    OptimalTreap<int> tree;

    // Вставляем 3 элемента
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);

    std::cout << "Дерево после вставки:\n";
    tree.print();

    // Много раз ищем 20
    std::cout << "\nИщем 20 много раз...\n";
    for (int i = 0; i < 50; i++) {
        tree.find_and_update(20);
    }

    std::cout << "\nДерево после 50 поисков 20:\n";
    tree.print();

    // Проверим приоритеты
    std::cout << "\nПриоритеты:\n";
    // ... код для вывода приоритетов всех узлов

    std::cout << "20 должен быть КОРНЕМ! Если нет — проблема!\n";
    std::cout << "=== ДИАГНОСТИКА ЗАВЕРШЕНА ===\n\n";
}

template<typename T>
int find_depth(const OptimalTreap<T>& tree, const T& key) {
    // Простой поиск с подсчётом глубины
    auto* node = tree.root.get();
    int depth = 0;

    while (node) {
        if (key == node->key) {
            return depth;
        }
        else if (key < node->key) {
            node = node->left.get();
        }
        else {
            node = node->right.get();
        }
        ++depth;
    }
    return -1;  // Ключ не найден
}
void test_real_optimality() {
    std::cout << "\n=== Корректный тест оптимальности ===\n";

    constexpr int SIZE = 1000;
    constexpr int GROUP_A_START = 0;    // Группа A
    constexpr int GROUP_B_START = 500;  // Группа B
    constexpr int GROUP_SIZE = 100;
    constexpr int TOTAL_OPS = 100000;

    // 1. Два дерева с одинаковыми данными
    OptimalTreap<int> skewed_tree;
    OptimalTreap<int> uniform_tree;

    for (int i = 0; i < SIZE; ++i) {
        skewed_tree.insert(i);
        uniform_tree.insert(i);
    }

    // 2. Измерим ИСХОДНЫЕ глубины
    auto measure_group_depth = [&](const auto& tree, int start) {
        double sum = 0;
        int found = 0;
        for (int i = 0; i < GROUP_SIZE; ++i) {
            int depth = find_depth(tree, start + i);
            if (depth >= 0) {
                sum += depth;
                found++;
            }
        }
        return sum / found;
        };

    double initial_depth_A = measure_group_depth(skewed_tree, GROUP_A_START);
    double initial_depth_B = measure_group_depth(skewed_tree, GROUP_B_START);

    std::cout << "Исходные глубины:\n";
    std::cout << "Группа A (" << GROUP_A_START << "-" << GROUP_A_START + GROUP_SIZE - 1
        << "): " << initial_depth_A << "\n";
    std::cout << "Группа B (" << GROUP_B_START << "-" << GROUP_B_START + GROUP_SIZE - 1
        << "): " << initial_depth_B << "\n";
    std::cout << "Разница: " << (initial_depth_B - initial_depth_A) << "\n\n";

    // 3. Skewed доступ: 90% к группе A, 10% к группе B
    std::cout << "Skewed доступ (90% A, 10% B):\n";
    for (int i = 0; i < TOTAL_OPS; ++i) {
        if (rand() % 100 < 90) {
            // Частый доступ к группе A
            int key = GROUP_A_START + rand() % GROUP_SIZE;
            skewed_tree.find_and_update(key);
        }
        else {
            // Редкий доступ к группе B
            int key = GROUP_B_START + rand() % GROUP_SIZE;
            skewed_tree.find_and_update(key);
        }
    }

    // 4. Uniform доступ
    std::cout << "Uniform доступ:\n";
    for (int i = 0; i < TOTAL_OPS; ++i) {
        int key = rand() % SIZE;
        uniform_tree.find_and_update(key);
    }

    // 5. Измерим ФИНАЛЬНЫЕ глубины
    double skewed_final_A = measure_group_depth(skewed_tree, GROUP_A_START);
    double skewed_final_B = measure_group_depth(skewed_tree, GROUP_B_START);
    double uniform_final_A = measure_group_depth(uniform_tree, GROUP_A_START);
    double uniform_final_B = measure_group_depth(uniform_tree, GROUP_B_START);

    // 6. Вычислим ИЗМЕНЕНИЯ
    double skewed_improvement_A = initial_depth_A - skewed_final_A;
    double skewed_improvement_B = initial_depth_B - skewed_final_B;
    double uniform_improvement_A = initial_depth_A - uniform_final_A;
    double uniform_improvement_B = initial_depth_B - uniform_final_B;

    std::cout << "\nРЕЗУЛЬТАТЫ:\n";
    std::cout << "=================================\n";
    std::cout << "Skewed дерево:\n";
    std::cout << "  Группа A: " << initial_depth_A << " → " << skewed_final_A
        << " (Δ=" << skewed_improvement_A << ")\n";
    std::cout << "  Группа B: " << initial_depth_B << " → " << skewed_final_B
        << " (Δ=" << skewed_improvement_B << ")\n";
    std::cout << "  Преимущество A над B: "
        << (skewed_improvement_A - skewed_improvement_B) << "\n\n";

    std::cout << "Uniform дерево:\n";
    std::cout << "  Группа A: " << initial_depth_A << " → " << uniform_final_A
        << " (Δ=" << uniform_improvement_A << ")\n";
    std::cout << "  Группа B: " << initial_depth_B << " → " << uniform_final_B
        << " (Δ=" << uniform_improvement_B << ")\n";
    std::cout << "  Преимущество A над B: "
        << (uniform_improvement_A - uniform_improvement_B) << "\n\n";

    // 7. Ключевой показатель
    double skewed_advantage = skewed_improvement_A - skewed_improvement_B;
    double uniform_advantage = uniform_improvement_A - uniform_improvement_B;

    std::cout << "ВЫВОД:\n";
    if (skewed_advantage > uniform_advantage * 1.5) {
        std::cout << "✓ OptimalTreap УСПЕШНО адаптируется! "
            << "Skewed преимущество в "
            << (skewed_advantage / uniform_advantage)
            << " раз больше!\n";
    }
    else if (skewed_advantage > uniform_advantage) {
        std::cout << "OptimalTreap немного помогает\n";
    }
    else {
        std::cout << "OptimalTreap не показывает преимущества\n";
    }

    std::cout << "=== Тест завершён ===\n\n";
}

void test_tree_structure() {
    OptimalTreap<int> tree;

    // Вставка
    for (int i = 1; i <= 10; i++) tree.insert(i);

    std::cout << "Дерево после вставки:\n";
    tree.print();

    // Много поисков
    for (int i = 0; i < 20; i++) tree.find_and_update(5);
    for (int i = 0; i < 10; i++) tree.find_and_update(3);

    std::cout << "\nПосле 20 поисков 5 и 10 поисков 3:\n";
    tree.print();

    // Проверка: где теперь 5 и 3?
    std::cout << "\nКорень дерева: " << tree.root->key
        << " (приоритет: " << tree.root->priority << ")\n";

    // Найдём 5 в дереве
    auto find_path = [&](int key) {
        std::vector<int> path;
        auto* node = tree.root.get();
        while (node) {
            path.push_back(node->key);
            if (key == node->key) break;
            if (key < node->key) node = node->left.get();
            else node = node->right.get();
        }
        return path;
        };

    std::cout << "Путь к 5: ";
    for (int x : find_path(5)) std::cout << x << " -> ";
    std::cout << "\nПуть к 3: ";
    for (int x : find_path(3)) std::cout << x << " -> ";
    std::cout << "\n";
}

void test_small_tree() {
    std::cout << "=== Тест OptimalTreap на маленьком дереве ===\n";

    OptimalTreap<int> tree;

    // Вставляем 10 элементов
    for (int i = 1; i <= 10; i++) {
        tree.insert(i);
    }

    std::cout << "После вставки 1..10:\n";
    std::cout << "Высота: " << tree.height() << "\n";

    // Покажем структуру
    std::cout << "Inorder: ";
    auto inorder = tree.inorder();
    for (int x : inorder) std::cout << x << " ";
    std::cout << "\n";

    // Ищем элемент 5 много раз
    std::cout << "\nИщем элемент 5 10 раз...\n";
    for (int i = 0; i < 10; i++) {
        tree.find_and_update(5);
    }

    // Ищем элемент 3 несколько раз
    for (int i = 0; i < 5; i++) {
        tree.find_and_update(3);
    }

    // Посмотрим, как изменилось дерево
    std::cout << "После частых поисков 5 и 3:\n";
    std::cout << "Высота: " << tree.height() << "\n";

    // Проверим, стал ли 5 ближе к корню
    std::cout << "\nПроверяем, поднялись ли часто используемые элементы:\n";

    // Простая проверка: найдём глубину элементов
    auto check_depth = [&](int key) {
        // Функция для поиска глубины
        std::function<int(typename OptimalTreap<int>::Node*, int, int)> find_depth =
            [&](typename OptimalTreap<int>::Node* node, int k, int depth) -> int {
            if (!node) return -1;
            if (node->key == k) return depth;
            if (k < node->key) return find_depth(node->left.get(), k, depth + 1);
            return find_depth(node->right.get(), k, depth + 1);
            };

        return find_depth(tree.root.get(), key, 0);
        };

    std::cout << "Глубина 5: " << check_depth(5) << "\n";
    std::cout << "Глубина 3: " << check_depth(3) << "\n";
    std::cout << "Глубина 1: " << check_depth(1) << " (редко используется)\n";

    // Покажем приоритеты
    std::cout << "\nПриоритеты элементов:\n";
    std::cout << "Key\tAccess\tPriority\n";
    std::cout << "----------------------\n";

    // Обход для вывода
    std::function<void(typename OptimalTreap<int>::Node*)> print_stats =
        [&](typename OptimalTreap<int>::Node* node) {
        if (!node) return;
        print_stats(node->left.get());
        std::cout << node->key << "\t"
            << node->access_count << "\t"
            << node->priority << "\n";
        print_stats(node->right.get());
        };

    print_stats(tree.root.get());

    std::cout << "=== Тест завершён ===\n\n";
}

void test_splay_basic() {
    std::cout << "=== БАЗОВЫЙ ТЕСТ SplayTree ===" << std::endl;

    SplayTree<int> tree;

    // 1. Вставка элементов
    std::cout << "\n1. Вставка элементов [5, 3, 7, 2, 4, 6, 8]:" << std::endl;
    for (int key : {5, 3, 7, 2, 4, 6, 8}) {
        tree.insert(key);
        std::cout << "   Вставили " << key << ", корень: "
            << (tree.empty() ? -1 : tree.preorder()[0]) << std::endl;
    }

    std::cout << "\n   Итоговое дерево (preorder): ";
    for (int key : tree.preorder()) std::cout << key << " ";
    std::cout << "\n   Высота: " << tree.height() << ", Размер: " << tree.size() << std::endl;

    // 2. Поиск существующего элемента
    std::cout << "\n2. Поиск существующего элемента 4 (должен стать корнем):" << std::endl;
    bool found = tree.find_and_update(4);
    std::cout << "   Найден: " << (found ? "да" : "нет")
        << ", новый корень: " << tree.preorder()[0] << std::endl;

    // 3. Поиск несуществующего элемента
    std::cout << "\n3. Поиск несуществующего элемента 10:" << std::endl;
    found = tree.find_and_update(10);
    std::cout << "   Найден: " << (found ? "да" : "нет")
        << ", новый корень (ближайший к 10): " << tree.preorder()[0] << std::endl;

    // 4. Вставка существующего элемента (дубликат)
    std::cout << "\n4. Попытка вставить существующий элемент 7:" << std::endl;
    int old_root = tree.preorder()[0];
    tree.insert(7);
    std::cout << "   Корень до/после: " << old_root << " -> " << tree.preorder()[0] << std::endl;

    // 5. Удаление элемента
    std::cout << "\n5. Удаление элемента 5:" << std::endl;
    std::cout << "   Дерево до удаления (inorder): ";
    for (int key : tree.inorder()) std::cout << key << " ";
    std::cout << "\n   Корень до удаления: " << tree.preorder()[0] << std::endl;

    tree.remove(5);

    std::cout << "   Дерево после удаления (inorder): ";
    for (int key : tree.inorder()) std::cout << key << " ";
    std::cout << "\n   Корень после удаления: " << tree.preorder()[0] << std::endl;
    std::cout << "   Размер после удаления: " << tree.size() << std::endl;

    // 6. Удаление несуществующего элемента
    std::cout << "\n6. Попытка удалить несуществующий элемент 100:" << std::endl;
    std::cout << "   Корень до удаления: " << tree.preorder()[0] << std::endl;
    tree.remove(100);
    std::cout << "   Корень после удаления (ближайший к 100): " << tree.preorder()[0] << std::endl;

    // 7. Проверка свойств BST
    std::cout << "\n7. Проверка свойств BST:" << std::endl;
    auto inorder = tree.inorder();
    bool is_sorted = std::is_sorted(inorder.begin(), inorder.end());
    std::cout << "   Inorder отсортирован: " << (is_sorted ? "да" : "нет") << std::endl;
    std::cout << "   Элементы: ";
    for (int key : inorder) std::cout << key << " " <<  std::endl;

    // 8. Const contains (без splay)
    std::cout << "\n8. Const contains (не меняет дерево):" << std::endl;
    const SplayTree<int>& const_tree = tree;
    std::cout << "   Корень до вызова: " << tree.preorder()[0] << std::endl;
    bool const_found = const_tree.contains(6);
    std::cout << "   Найден 6: " << (const_found ? "да" : "нет")
        << ", корень после: " << tree.preorder()[0] << " (не должен измениться)" << std::endl;

    std::cout << "\n=== ТЕСТ ЗАВЕРШЕН ===" << std::endl;
}

void test_splay_sequence() {
    std::cout << "\n\n=== ТЕСТ ПОСЛЕДОВАТЕЛЬНОСТИ ОПЕРАЦИЙ ===" << std::endl;

    SplayTree<int> tree;

    std::cout << "Последовательность операций и корень после каждой:" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    const std::vector<std::pair<std::string, int>> operations = {
        {"insert(10)", 10},
        {"insert(5)", 5},
        {"insert(15)", 15},
        {"find(5)", 5},
        {"insert(7)", 7},
        {"find(12)", 12},  // Не существует, ближайший 10 или 15
        {"insert(3)", 3},
        {"remove(7)", 7},
        {"find(10)", 10},
        {"remove(100)", 100}  // Не существует
    };

    for (const auto& [op, value] : operations) {
        if (op.find("insert") != std::string::npos) {
            tree.insert(value);
        }
        else if (op.find("find") != std::string::npos) {
            tree.find_and_update(value);
        }
        else if (op.find("remove") != std::string::npos) {
            tree.remove(value);
        }

        std::cout << op << " -> корень: ";
        if (tree.empty()) {
            std::cout << "[empty]";
        }
        else {
            std::cout << tree.preorder()[0];
        }
        std::cout << ", размер: " << tree.size() << std::endl;
    }
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
    
    //TreeTest<int, OptimalBST<int>>::comprehensive_test(10000);

    /*
    final_verification();
    test_3_elements_debug();
    test_letter_analysis();
    
    test_obvious_case();
        
    test_uniform();

    test_dp_manual();*/

    /*int n = 10000; //количество элементов для тестов

    std::cout << "\nТестирование функционала деревьев\n\n";
    std::cout << "-------- Часть 1: n= " << n << "\n";
    std::cout << "****** BSTree **********************\n";

    TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::FIXED, 0.25>>::comprehensive_test(n);
    std::cout << "****** BSTree **********************\n";
    TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_LOG_N>>::comprehensive_test(n);
    std::cout << "****** BSTree **********************\n";
    TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_N>>::comprehensive_test(n);
    std::cout << "****** BSTree **********************\n";
    TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_SQRT_N>>::comprehensive_test(n);*/
    //test_splay_basic();
    //test_splay_sequence();

    //int n = 100000; //количество элементов для тестов
    
    // Добавь в твой тест    
    /*
    SplayTree<int> tree;
    for (int n : {100, 500, 1000, 5000, 10000}) {        
        std::vector<int> random(n);

        for (size_t i = 0; i < n; i++) {            
            random[i] = static_cast<int>(i);
        }
        std::shuffle(random.begin(), random.end(),
            std::mt19937{ std::random_device{}() });
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i != n; ++i) {
            tree.insert(random[i]);        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "n=" << n << ", time=" << std::chrono::duration<double>(end - start).count() << "s" << std::endl;
    }
    */
    /*std::cout << "\nТестирование функционала деревьев\n\n";
    std::cout << "-------- Часть 1: n= " << n << "\n";
    std::cout << "****** Splay **********************\n";

    TreeTest<int, SplayTree<int>>::comprehensive_test(n);*/

    int n = 1000000; //количество элементов для тестов
    TreeTest<int, OptimalTreap<int>>::comprehensive_test(n);   
    //test_tree_structure();
    
    //quick_debug_test();
    //test_real_optimality();
}