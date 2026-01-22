#pragma once
#include <iostream>
#include <concepts>
#include <vector>
#include <chrono>
#include <random>
#include "ITree.h"

template <std::integral T, std::derived_from<ITree<T>> Tree>
class TreeTest {

public:	
	
	static void test(size_t size) {

		//формируем данные для тестирования
		std::vector<T> data(size); //последовательные данные
		for (T i = 0; i != size; ++i) {
			data[i] = i;
		}
		std::cout << "Тестируем построение дерева \n";
		std::cout << "1. Заполнение отсортированными данными \n";
		Tree ord_tree = build_tree_test(data);
		std::cout << "-------------------------\n";
		
		std::cout << "\n2. Заполнение дерева в случайном порядке \n";		
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(data.begin(), data.end(), g); //перемешали
		//числа по порядку
		Tree rnd_tree = build_tree_test(data);
		std::cout << "-------------------------\n";
		
		std::cout << "Тестируем обходы дерева \n";
		std::cout << "1. Заполненное отсортированными данными \n";
		test_all_traversals(ord_tree);
		std::cout << "\n2. Заполненное в случайном порядке \n";
		test_all_traversals(rnd_tree);
		std::cout << "-------------------------\n";

		std::cout << "Тестируем поиск/удаление \n";
		
		std::cout << "1. Заполненное отсортированными данными \n";
		std::vector<T> items_sort{ T(size / 2), T(size - 1)};
		std::cout << "Удаляем существующие элементы \n";
		for (auto& item : items_sort) {
			test_remove(item, ord_tree);
		}
		std::cout << "Проверяем те же элементы (должны не найтись) \n";
		for (auto& item : items_sort) {
			test_remove(item, ord_tree);
		}

		std::cout << "\n2. Заполненное в случайном порядке \n";
		std::vector<T> items_rnd{ data[size / 2], data[size - 1] };
		std::cout << "Удаляем существующие элементы \n";
		for (auto& item : items_rnd) {
			test_remove(item, rnd_tree);
		}
		std::cout << "Проверяем те же элементы (должны не найтись) \n";
		for (auto& item : items_rnd) {
			test_remove(item, rnd_tree);
		}
		std::cout << "-------------------------\n";
	}
	


private:	
	//тестирование построение дерева
	static Tree build_tree_test(std::vector<T>& data) {
		
		Tree tree;		
		auto start_time = std::chrono::high_resolution_clock::now();
		for (auto& key : data) {
			tree.insert(key);
		}
		auto end_time = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			end_time - start_time);
		std::cout << " Дерево построено за " << duration.count() << " ms\n";
		std::cout << "Размер: " << tree.size() << ", Высота: " << tree.height() << "\n";
		return tree;
	}	

	//тестирование построение дерева
	static void test_all_traversals(Tree& tree) {		
		
		std::cout << "In-order \n";
		auto start_time = std::chrono::high_resolution_clock::now();
		auto inorder = tree.inorder();
		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			end_time - start_time);
		std::cout << "Inorder выполнен за  " << duration.count() << " ms\n\n";
		
		std::cout << "Pre-order: ";
		start_time = std::chrono::high_resolution_clock::now();
		auto preorder = tree.preorder();
		end_time = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			end_time - start_time);
		std::cout << " preorder выполнен за  " << duration.count() << " ms\n\n";

		std::cout << "Post-order: ";
		start_time = std::chrono::high_resolution_clock::now();
		auto postorder = tree.postorder();
		end_time = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			end_time - start_time);
		std::cout << " postorder выполнен за  " << duration.count() << " ms\n\n";
		
		std::cout << "Level-order: ";
		start_time = std::chrono::high_resolution_clock::now();
		auto levelorder = tree.level_order();
		end_time = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			end_time - start_time);
		std::cout << " level-order выполнен за  " << duration.count() << " ms\n\n";		
		
	}

	//тестирование поиска и удаления
	static void test_remove(T item, Tree& tree) {
		
		bool contains = true;
		auto start_time = std::chrono::high_resolution_clock::now();
		if (tree.contains(item)) {
			tree.remove(item);
		}
		else contains = false;

		auto end_time = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			end_time - start_time);
		if (contains) {
			std::cout << " Элемент "<< item << " удален за " << duration.count() << " ms\n";
			std::cout << "Новый размер: " << tree.size() << " Высота: " << tree.height() << "\n";
		}
		else {
			std::cout << " Элемент " << item << " не найден. Поиск за " << duration.count() << " ms\n";
		}
	}
		
};
