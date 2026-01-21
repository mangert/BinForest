#include <iostream>
#include "BSTree.h"
#include "TreeTest.h"
#include <cassert>

void testPrinting() {
    BSTree<int> tree;

    // Построим сбалансированное дерево
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);

    std::cout << "=== Simple print ===\n";
    tree.print_simple(std::cout);
    std::cout << "\n\n";

    std::cout << "=== Detailed print ===\n";
    tree.print_detailed(std::cout);
    std::cout << "\n";

    std::cout << "=== ASCII tree ===\n";
    tree.print(std::cout);

    // Для интерфейса ITree
    std::cout << "\n=== Via ITree interface ===\n";
    const ITree<int>& itree = tree;
    itree.print(std::cout);
}

void testAllTraversals() {
    std::cout << "=== Testing all traversals ===\n";

    BSTree<int> tree;
    //       5
    //      / \
    //     3   7
    //    / \ / \
    //   2  4 6  8
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);

    std::cout << "Tree structure:\n";
    std::cout << "       5\n";
    std::cout << "      / \\\n";
    std::cout << "     3   7\n";
    std::cout << "    / \\ / \\\n";
    std::cout << "   2  4 6  8\n\n";

    auto inorder = tree.inorder();
    std::cout << "In-order (sorted): ";
    for (int x : inorder) std::cout << x << " ";
    std::cout << "\n";  // 2 3 4 5 6 7 8

    auto preorder = tree.preorder();
    std::cout << "Pre-order: ";
    for (int x : preorder) std::cout << x << " ";
    std::cout << "\n";  // 5 3 2 4 7 6 8

    auto postorder = tree.postorder();
    std::cout << "Post-order: ";
    for (int x : postorder) std::cout << x << " ";
    std::cout << "\n";  // 2 4 3 6 8 7 5

    auto levelorder = tree.level_order();
    std::cout << "Level-order: ";
    for (int x : levelorder) std::cout << x << " ";
    std::cout << "\n";  // 5 3 7 2 4 6 8

    // Тест на вырожденном дереве
    std::cout << "\n=== Testing degenerate tree ===\n";

    BSTree<int> degenerate;
    for (int i = 0; i < 1000; i++) {
        degenerate.insert(i);
    }

    std::cout << "Size: " << degenerate.size()
        << ", Height: " << degenerate.height() << "\n";

    auto deg_inorder = degenerate.inorder();
    std::cout << "In-order size: " << deg_inorder.size() << "\n";

    auto deg_preorder = degenerate.preorder();
    std::cout << "Pre-order size: " << deg_preorder.size() << "\n";

    auto deg_postorder = degenerate.postorder();
    std::cout << "Post-order size: " << deg_postorder.size() << "\n";

    auto deg_levelorder = degenerate.level_order();
    std::cout << "Level-order size: " << deg_levelorder.size() << "\n";

    std::cout << "\n=== All traversal tests passed ===\n";
}


int main() {
	setlocale(LC_ALL, "Russian");
    BSTree<int> tree;
    /*
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    assert(tree.size() == 3);

    // Дубликаты
    tree.insert(5);  // Не должен добавиться
    tree.insert(3);  // Не должен добавиться
    tree.insert(7);  // Не должен добавиться
    assert(tree.size() == 3);  // Размер не изменился

    // Проверка структуры
    assert(tree.contains(5));
    assert(tree.contains(3));
    assert(tree.contains(7));

    // Удаление и повторная вставка
    tree.remove(5);
    assert(tree.size() == 2);
    tree.insert(5);  // Теперь можно добавить
    assert(tree.size() == 3);
	*/
    
    /*TreeTest<int, BSTree<int>>::test(10000000);
    std::cout << "**********************\n";*/
    testAllTraversals();
    testPrinting();
    
}