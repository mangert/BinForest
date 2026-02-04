#include <iostream>
#include "BSTree.h"
#include "TreeTest.h"
#include "AVLTree.h"
#include "Treap.h"
#include "OptimalBST.h"
#include "RandomizedBST.h"
#include "SplayTree.h"
#include <cassert>
#include <set>
#include <chrono>
#include <vector>
#include <random>
#include <numeric>

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
    bool found = tree.find_and_splay(4);
    std::cout << "   Найден: " << (found ? "да" : "нет")
        << ", новый корень: " << tree.preorder()[0] << std::endl;

    // 3. Поиск несуществующего элемента
    std::cout << "\n3. Поиск несуществующего элемента 10:" << std::endl;
    found = tree.find_and_splay(10);
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
            tree.find_and_splay(value);
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

    /*std::cout << "\nТестирование функционала деревьев\n\n";
    std::cout << "-------- Часть 1: n= " << n << "\n";
    std::cout << "****** Splay **********************\n";

    TreeTest<int, SplayTree<int>>::comprehensive_test(n);*/



}