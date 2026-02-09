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

int main() {
	setlocale(LC_ALL, "Russian");
    //первая домашка
    /*
    {
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

        TreeTest<int, Treap<int>>::comprehensive_test(n);
    }
    */
    
    //вторая домашка
    {
        int n = 10000; //количество элементов для тестов

        std::cout << "\nТестирование функционала деревьев\n\n";
        std::cout << "-------- Часть 3: n= " << n << "\n";
        std::cout << "****** OptimalBST **********************\n";

        TreeTest<int, OptimalBST<int>>::comprehensive_test(n);

        std::cout << "****** Randomize BST *********************\n";

        std::cout << "========= Strategy: Fixed, 10% =========== \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::FIXED, 0.1>>::comprehensive_test(n);

        std::cout << "========= Strategy: Inverse Log N ========= \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_LOG_N>>::comprehensive_test(n);

        std::cout << "========= Strategy: Inverse N ============= \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_N>>::comprehensive_test(n);

        std::cout << "========= Strategy: Inverse Squrt N ======== \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_SQRT_N>>::comprehensive_test(n);

        std::cout << "****** Splay Tree ***********************\n";

        TreeTest<int, SplayTree<int>>::comprehensive_test(n);

        std::cout << "****** Treap (из прошлого ДЗ, для сравнения)\n";

        TreeTest<int, Treap<int>>::comprehensive_test(n);

        std::cout << "****** Optimal Treap *******************\n";

        TreeTest<int, OptimalTreap<int>>::comprehensive_test(n);

        n = 100000;
        std::cout << "-------- Часть 3b: n= " << n << "\n";
        std::cout << "(OBST не участвует в связи с ограничениями по памяти)\n";

        std::cout << "****** Randomize BST *********************\n";

        std::cout << "========= Strategy: Fixed, 10% =========== \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::FIXED, 0.1>>::comprehensive_test(n);

        std::cout << "========= Strategy: Inverse Log N ========= \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_LOG_N>>::comprehensive_test(n);

        std::cout << "========= Strategy: Inverse N ============= \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_N>>::comprehensive_test(n);

        std::cout << "========= Strategy: Inverse Squrt N ======== \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_SQRT_N>>::comprehensive_test(n);

        std::cout << "****** Splay Tree ***********************\n";

        TreeTest<int, SplayTree<int>>::comprehensive_test(n);

        std::cout << "****** Treap (из прошлого ДЗ, для сравнения)\n";

        TreeTest<int, Treap<int>>::comprehensive_test(n);

        std::cout << "****** Optimal Treap *******************\n";

        TreeTest<int, OptimalTreap<int>>::comprehensive_test(n);

        n = 1000000;
        std::cout << "-------- Часть 3c: n= " << n << "\n";
        std::cout << "(OBST не участвует в связи с ограничениями по памяти)\n";

        std::cout << "****** Randomize BST *********************\n";
        std::cout << "(Strategy: Fixed 10% не участвует в связи с длительностью удаления элементов \
        на отсортированных данных)\n";

        std::cout << "========= Strategy: Inverse Log N ========= \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_LOG_N>>::comprehensive_test(n);

        std::cout << "========= Strategy: Inverse N ============= \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_N>>::comprehensive_test(n);

        std::cout << "========= Strategy: Inverse Squrt N ======== \n";

        TreeTest<int, RandomizedBSTree<int, ProbabilityStrategy::INVERSE_SQRT_N>>::comprehensive_test(n);

        std::cout << "****** Splay Tree ***********************\n";

        TreeTest<int, SplayTree<int>>::comprehensive_test(n);

        std::cout << "****** Treap (из прошлого ДЗ, для сравнения)\n";

        TreeTest<int, Treap<int>>::comprehensive_test(n);

        std::cout << "****** Optimal Treap *******************\n";

        TreeTest<int, OptimalTreap<int>>::comprehensive_test(n);
    }
}