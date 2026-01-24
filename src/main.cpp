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

void test_avl_balance() {
    AVLTree<int> tree;

    // Вставляем в порядке, который создаёт несбалансированное BST
    // но AVL должно балансировать
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);  // Должен вызвать левую ротацию

    // После балансировки должно быть:
    //     2
    //    / \
    //   1   3

    assert(tree.size() == 3);
    assert(tree.height() == 1);  // Высота сбалансированного дерева из 3 узлов = 1

    auto inorder = tree.inorder();
    //assert(inorder == std::vector<int>{1, 2, 3});
    std::cout << "case 2" << std::endl;
    // Тест Right-Left case
    AVLTree<int> tree2;
    tree2.insert(3);
    tree2.insert(1);
    tree2.insert(2);  // Должен вызвать большую правую ротацию

    std::cout << "AVL balance tests passed!\n";
}
void test_big_rotations() {
    // Создаём Right-Left случай:
    //      x (10)
    //       \
    //        y (30)  ← баланс +1
    //       /
    //      z (20)

    auto x = std::make_unique<AVLTree<int>::Node>(10);
    auto y = std::make_unique<AVLTree<int>::Node>(30);
    auto z = std::make_unique<AVLTree<int>::Node>(20);

    y->left = std::move(z);
    y->upd_height();

    x->right = std::move(y);
    x->upd_height();

    // Проверяем баланс-факторы
    assert(x->balance_factor() == -2);  // Right heavy
    // assert(x->right->balanceFactor() == 1); // Left heavy

    // Большой левый поворот
    auto newRoot = AVLTree<int>::big_rotate_left(std::move(x));

    // Должно получиться:
    //       20
    //      /  \
    //     10   30

    assert(newRoot->key == 20);
    assert(newRoot->left->key == 10);
    assert(newRoot->right->key == 30);
    assert(std::abs(newRoot->balance_factor()) <= 1);

    std::cout << "Big left rotation test passed!\n";
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
    test_big_rotations();  
    test_avl_balance();
    
}