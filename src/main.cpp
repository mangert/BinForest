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
void test_avl_removal() {
    std::cout << "\n=== Testing AVL removal ===\n";

    AVLTree<int> tree;

    // Вставляем элементы
    for (int i = 0; i < 10; i++) {
        tree.insert(i);
    }

    std::cout << "Size after insertions: " << tree.size() << std::endl;
    std::cout << "Height after insertions: " << tree.height() << std::endl;

    // Удаляем корень
    tree.remove(4);
    std::cout << "Size after removing 4: " << tree.size() << std::endl;

    // Проверяем, что элемент удален
    assert(!tree.contains(4));

    // Удаляем лист
    tree.remove(9);
    std::cout << "Size after removing 9: " << tree.size() << std::endl;
    assert(!tree.contains(9));

    // Удаляем узел с одним ребенком
    tree.remove(8);
    std::cout << "Size after removing 8: " << tree.size() << std::endl;
    assert(!tree.contains(8));

    // Удаляем все элементы
    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 8 && i != 9) {
            tree.remove(i);
        }
    }

    std::cout << "Size after removing all: " << tree.size() << std::endl;
    assert(tree.empty());

    // Тест на удаление из пустого дерева
    tree.remove(100);  // Не должно падать

    // Тест на удаление несуществующего элемента
    tree.insert(1);
    tree.remove(2);  // Не должно влиять на дерево
    assert(tree.contains(1));
    assert(tree.size() == 1);

    std::cout << "✓ All removal tests passed!\n";
}

void test_avl_removal_with_balance() {
    std::cout << "\n=== Testing AVL removal with rebalancing ===\n";

    AVLTree<int> tree;

    // Создаем дерево, которое потребует балансировки при удалении
    //       4
    //      / \
    //     2   6
    //    / \ / \
    //   1  3 5  7

    tree.insert(4);
    tree.insert(2);
    tree.insert(6);
    tree.insert(1);
    tree.insert(3);
    tree.insert(5);
    tree.insert(7);

    std::cout << "Initial tree (level order): ";
    auto level = tree.level_order();
    for (int val : level) std::cout << val << " ";
    std::cout << std::endl;

    // Удаляем корень - должен найтись преемник и дерево останется сбалансированным
    tree.remove(4);

    std::cout << "After removing 4: ";
    level = tree.level_order();
    for (int val : level) std::cout << val << " ";
    std::cout << std::endl;

    // Проверяем, что дерево остается сбалансированным
    assert(tree.height() <= 2);  // Для 6 узлов высота должна быть <= 2

    // Проверяем BST свойство
    auto inorder = tree.inorder();
    for (size_t i = 1; i < inorder.size(); i++) {
        assert(inorder[i - 1] < inorder[i]);
    }

    std::cout << "✓ Removal with rebalancing test passed!\n";
}
/*
void test_avl_balance_properties() {
    std::cout << "\n=== Testing AVL balance properties ===\n";

    AVLTree<int> tree;

    // Вставляем последовательные числа - худший случай для BST
    // но AVL должно поддерживать баланс
    for (int i = 0; i < 20; i++) {
        tree.insert(i);
    }

    // Проверяем высоту - для 20 элементов максимальная высота AVL дерева
    // должна быть не более ~1.44*log2(20) ≈ 6.2
    int h = tree.height();
    int max_avl_height = static_cast<int>(1.44 * log2(tree.size() + 2) - 1);

    std::cout << "Size: " << tree.size() << std::endl;
    std::cout << "Height: " << h << std::endl;
    std::cout << "Max theoretical AVL height: " << max_avl_height << std::endl;

    if (h <= max_avl_height) {
        std::cout << "✓ Height is within AVL bounds!\n";
    }
    else {
        std::cout << "✗ Height exceeds AVL bounds!\n";
    }

    // Проверяем BST свойство
    auto inorder = tree.inorder();
    bool is_bst = true;
    for (size_t i = 1; i < inorder.size(); i++) {
        if (inorder[i] <= inorder[i - 1]) {
            is_bst = false;
            break;
        }
    }
    std::cout << (is_bst ? "✓ BST property maintained\n" : "✗ BST property violated\n");

    // Проверяем дубликаты
    tree.insert(5);  // Дубликат
    std::cout << "Size after inserting duplicate: " << tree.size()
        << " (should be " << inorder.size() << ")\n";
}

void test_avl_height_updates() {
    AVLTree<int> tree;

    std::cout << "\n=== Testing height updates ===\n";

    // Вставляем 1, 2, 3 - должно вызвать ротацию
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);

    // Проверяем структуру
    auto inorder = tree.inorder();
    assert(inorder.size() == 3);
    assert(inorder[0] == 1);
    assert(inorder[1] == 2);
    assert(inorder[2] == 3);

    // Проверяем высоту
    assert(tree.height() == 1);  // Для 3 узлов идеально сбалансированное дерево

    // Проверяем баланс
    auto level_order = tree.level_order();
    std::cout << "Level order after inserting 1,2,3: ";
    for (int val : level_order) std::cout << val << " ";
    std::cout << std::endl;

    std::cout << "Height updates test passed!\n";
}

void test_avl_comprehensive() {
    AVLTree<int> tree;

    // Тест последовательной вставки возрастающих значений
    for (int i = 0; i < 10; i++) {
        tree.insert(i);
        std::cout << "After inserting " << i
            << ", height = " << tree.height()
            << ", size = " << tree.size() << std::endl;

        // Проверяем балансировку
        auto inorder = tree.inorder();
        for (size_t j = 1; j < inorder.size(); j++) {
            assert(inorder[j - 1] < inorder[j]);  // BST свойство
        }

        // Проверяем высоту - для AVL дерева высота должна быть O(log n)
        int max_height = static_cast<int>(1.44 * log2(tree.size() + 2) - 1);
        assert(tree.height() <= max_height);
    }

    std::cout << "Comprehensive AVL test passed!\n";
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
}*/

int main() {
	setlocale(LC_ALL, "Russian");
    std::cout << "Двоичное дерево поиска" << std::endl;
    std::cout << "Пример\n";
    //example_tree();

    std::cout << "\nТестирование функционала\n";

    //TreeTest<int, BSTree<int>>::comprehensive_test(100000);
    //testRotations();
    //test_big_rotations();  
    /*test_avl_balance();
    AVLTree<int> tree;
    size_t n = 100;
    for (size_t i = 0; i != n; i++) {
        tree.insert(i);
    };
    std::cout << tree.size() << std::endl;
    tree.insert(59);
    std::cout << tree.size() << std::endl;
    tree.insert(2);
    std::cout << "Этот тест добавила" << std::endl;

    test_avl_comprehensive();
    std::cout << "И вот этот тест добавила" << std::endl;
    test_avl_height_updates();
    std::cout << "А теперь еще и вот этот" << std::endl;
    test_avl_balance_properties();*/
    test_avl_removal();
    test_avl_removal_with_balance();
    
}