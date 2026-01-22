#include <iostream>
#include "BSTree.h"
#include "TreeTest.h"
#include <cassert>

void example_tree() { //пример дерева (маленькое, чтобы напечатать можно было)
    
    BSTree<int> tree;

    // Пример дерева
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);

    tree.print();
    
}

int main() {
	setlocale(LC_ALL, "Russian");
    std::cout << "Двоичное дерево поиска" << std::endl;
    std::cout << "Пример\n";
    example_tree();

    std::cout << "\nТестирование функционала\n";

    TreeTest<int, BSTree<int>>::test(100000);
    
    
}