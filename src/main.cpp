#include <iostream>
#include "BSTree.h"
#include "TreeTest.h"
#include "AVLTree.h"
#include "Treap.h"
#include <cassert>
#include <set>

int main() {
	setlocale(LC_ALL, "Russian");

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