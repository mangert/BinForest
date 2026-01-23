#include <iostream>
#include "BSTree.h"
#include "TreeTest.h"
#include <cassert>
#include "AVLTree.h"

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
void testRotations() {
    AVLTree<int> tree;

    // Создаём несбалансированное дерево вручную
    //     3
    //      \
    //       5
    //        \
    //         7

    auto node3 = std::make_unique<AVLTree<int>::Node>(3);
    auto node5 = std::make_unique<AVLTree<int>::Node>(5);
    auto node7 = std::make_unique<AVLTree<int>::Node>(7);

    node5->right = std::move(node7);
    node5->upd_height();

    node3->right = std::move(node5);
    node3->upd_height();

    // Проверяем баланс-фактор
    assert(node3->balance_factor() == -2);  // Несбалансировано вправо

    // Делаем левый поворот
    auto newRoot = AVLTree<int>::small_rotate_left(std::move(node3));

    // Теперь должно быть:
    //       5
    //      / \
    //     3   7

    assert(newRoot->key == 5);
    assert(newRoot->left->key == 3);
    assert(newRoot->right->key == 7);
    assert(std::abs(newRoot->balance_factor()) <= 1);  // Сбалансировано

    std::cout << "Left rotation test passed!\n";
}

int main() {
	setlocale(LC_ALL, "Russian");
    std::cout << "Двоичное дерево поиска" << std::endl;
    std::cout << "Пример\n";
    //example_tree();

    std::cout << "\nТестирование функционала\n";

    //TreeTest<int, BSTree<int>>::comprehensive_test(100000);
    testRotations();
    
    
    
}