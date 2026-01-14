#include <iostream>
#include "BSTree.h"

int main() {
	std::cout << "Hello" << std::endl;

	BSTree<int> BSTree;
	BSTree.insert(10);
	BSTree.insert(12);
	BSTree.insert(5);
	BSTree.insert(13);
	BSTree.insert(2);
	BSTree.insert(29);
	BSTree.clear();
	std::cout << "---";
}