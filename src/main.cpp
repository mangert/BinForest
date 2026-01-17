#include <iostream>
#include "BSTree.h"
#include "TreeTest.h"
#include <cassert>

int main() {
	setlocale(LC_ALL, "Russian");
    /*BSTree<int> tree;
    
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
    
    TreeTest<int, BSTree<int>>::test(10000);
    std::cout << "**********************\n";

	
}