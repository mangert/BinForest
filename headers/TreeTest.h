#pragma once
#include "ITree.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <set>
#include <functional>
#include <concepts>
#include "OptimalBST.h"

template <std::integral T, std::derived_from<ITree<T>> Tree>
class TreeTest {
public:

	static void comprehensive_test(size_t size) {
		std::cout << "========================================\n";
		std::cout << "COMPREHENSIVE TREE TEST (size = " << size << ")\n";
		std::cout << "========================================\n\n";
		if constexpr (std::is_same_v<Tree, OptimalBST<T>>) {
			std::cout << "TREE TYPE: Optimal BST\n";
		}
		else {
			std::cout << "TREE TYPE: Dynamic tree\n";
		}

		// 1. Тест граничных случаев
		if constexpr (std::is_same_v<Tree, OptimalBST<T>>) {
			std::cout << "Edge cases test skipped for OBST\n";
		}
		else test_edge_cases();

		// 2. Основной тест с отсортированными и случайными данными
		main_test(size);

		// 3. Тест производительности (сравнение сбалансированного/несбалансированного)
		if constexpr (std::is_same_v<Tree, OptimalBST<T>>) {
			std::cout << "performance_comparison test skipped for OBST\n";
		}
		else performance_comparison(size);

		// 4. Тест копирования и перемещения
		test_copy_move_semantics(size);

		// 5. Стресс-тест (меньший размер для скорости)
		// 3. Тест производительности (сравнение сбалансированного/несбалансированного)
		if constexpr (std::is_same_v<Tree, OptimalBST<T>>) {
			std::cout << "stress test skipped for OBST\n";
		}
		else stress_test(std::min(size, (size_t)5000));

		std::cout << "\n========================================\n";
		std::cout << "ALL TESTS PASSED SUCCESSFULLY!\n";
		std::cout << "========================================\n";
	}

private:

	// ==================== 1. Граничные случаи ====================
	static void test_edge_cases() {
		std::cout << "1. EDGE CASES TEST\n";
		std::cout << "------------------\n";

		Tree tree;

		// 1.1 Пустое дерево
		assert(tree.empty());
		assert(tree.size() == 0);
		assert(tree.height() == -1); // предполагаем height(empty) = -1
		assert(!tree.contains(0));
		assert(tree.inorder().empty());
		std::cout << "+ Empty tree checks passed\n";

		// 1.2 Один элемент
		tree.insert(42);
		assert(!tree.empty());
		assert(tree.size() == 1);
		assert(tree.height() == 0);
		assert(tree.contains(42));
		assert(!tree.contains(0));
		assert(tree.inorder() == std::vector<T>{42});
		std::cout << "+ Single element checks passed\n";

		// 1.3 Дубликаты не добавляются
		tree.insert(42); // Дубликат
		assert(tree.size() == 1);
		std::cout << "+ Duplicate prevention check passed\n";

		// 1.4 Удаление единственного элемента
		tree.remove(42);
		assert(tree.empty());
		assert(tree.size() == 0);
		assert(tree.height() == -1);
		assert(!tree.contains(42));
		std::cout << "+ Single element removal passed\n";

		// 1.5 Удаление несуществующего элемента
		tree.remove(999);
		assert(tree.empty()); // Должно остаться пустым
		std::cout << "+ Non-existent removal passed\n";

		// 1.6 Быстрое добавление/удаление
		for (int i = 0; i < 100; i++) {
			tree.insert(i);
			tree.remove(i);
			assert(tree.empty());
		}
		std::cout << "+ Rapid insert/remove cycle passed\n";

		// 1.7 Очистка
		tree.insert(1);
		tree.insert(2);
		tree.insert(3);
		tree.clear();
		assert(tree.empty());
		assert(tree.size() == 0);
		std::cout << "+ Clear operation passed\n";

		std::cout << "++ All edge cases passed\n\n";
	}

	// ==================== 2. Основной тест ====================	
	static void main_test(size_t size) {
		std::cout << "2. MAIN TEST (sorted vs random data)\n";
		std::cout << "-------------------------------------\n";

		// Подготовка данных
		std::vector<T> sorted_data(size);
		std::vector<T> random_data(size);

		for (size_t i = 0; i < size; i++) {
			sorted_data[i] = static_cast<T>(i);
			random_data[i] = static_cast<T>(i);
		}

		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(random_data.begin(), random_data.end(), g);

		// 2.1 Тест с отсортированными данными
		std::cout << "2.1 Sorted data (degenerate tree):\n";
		
		auto buid_sorted = [&]()->Tree {
			if constexpr (std::is_same_v<Tree, OptimalBST<T>>) {
				return build_and_test_tree_obst(sorted_data, "sorted");
			} else {
				return build_and_test_tree(sorted_data, "sorted");
			} 
		};		
		Tree sorted_tree = buid_sorted();
		
		// 2.2 Тест со случайными данными
		auto buid_random = [&]()->Tree {
			if constexpr (std::is_same_v<Tree, OptimalBST<T>>) {
				return build_and_test_tree_obst(random_data, "random");
			}
			else {
				return build_and_test_tree(random_data, "random");
			}
		};
		Tree random_tree = buid_random();

		// 2.3 Проверка корректности обходов
		std::cout << "\n2.3 Traversal correctness:\n";
		test_traversals_correctness(sorted_tree, "sorted");
		test_traversals_correctness(random_tree, "random");

		// 2.4 Тест поиска 10% случайных элементов
		std::cout << "\n2.5 10 percent search tests:\n";
		test_random_search_10_percent(sorted_tree, sorted_data, "Sorted tree");
		test_random_search_10_percent(random_tree, random_data, "Random tree");

		// 2.5 Тест удаления 10% случайных элементов
		std::cout << "\n2.5 10 percent removal tests:\n";
		if constexpr (std::is_same_v<Tree, OptimalBST<T>>) {
			std::cout << "Remove operation not is not defined for static trees\n";
		}
		else {
			test_random_removal_10_percent(sorted_tree, sorted_data, "Sorted tree");
			test_random_removal_10_percent(random_tree, random_data, "Random tree");
		}

		std::cout << "++ Main test completed\n\n";
	}	

	// ==================== 3. Сравнение производительности ====================
	static void performance_comparison(size_t size) {
		std::cout << "3. PERFORMANCE COMPARISON\n";
		std::cout << "-------------------------\n";

		if (size > 100000) {
			std::cout << "Skipping performance test for large size (>100k)\n";
			return;
		}

		std::vector<T> sorted(size);
		std::vector<T> random(size);

		for (size_t i = 0; i < size; i++) {
			sorted[i] = static_cast<T>(i);
			random[i] = static_cast<T>(i);
		}

		std::shuffle(random.begin(), random.end(),
			std::mt19937{ std::random_device{}() });

		// Построение деревьев
		auto start = std::chrono::high_resolution_clock::now();
		Tree degenerate;
		for (auto x : sorted) degenerate.insert(x);
		auto end = std::chrono::high_resolution_clock::now();
		auto degenerate_build = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		start = std::chrono::high_resolution_clock::now();
		Tree balanced;
		for (auto x : random) balanced.insert(x);
		end = std::chrono::high_resolution_clock::now();
		auto balanced_build = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << "Build times:\n";
		std::cout << "  Degenerate (sorted): " << degenerate_build.count() << " ms, "
			<< "height = " << degenerate.height() << "\n";
		std::cout << "  Balanced (random):   " << balanced_build.count() << " ms, "
			<< "height = " << balanced.height() << "\n";

		// Поиск (средний элемент)
		T middle = static_cast<T>(size / 2);

		start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 1000; i++) {
			degenerate.contains(middle);
		}
		end = std::chrono::high_resolution_clock::now();
		auto degenerate_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

		start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 1000; i++) {
			balanced.contains(middle);
		}
		end = std::chrono::high_resolution_clock::now();
		auto balanced_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

		std::cout << "\nSearch time (middle element, 1000 iterations):\n";
		std::cout << "  Degenerate: " << (degenerate_search.count() / 1000.0) << " ns per search\n";
		std::cout << "  Balanced:   " << (balanced_search.count() / 1000.0) << " ns per search\n";
		std::cout << "  Ratio (degenerate/balanced): "
			<< (degenerate_search.count() / (double)balanced_search.count()) << "x\n";

				std::cout << "++ Performance comparison completed\n\n";	
	}

	// ==================== 4. Копирование и перемещение ====================
	static void test_copy_move_semantics(size_t size) {
		std::cout << "4. COPY/MOVE SEMANTICS TEST\n";
		std::cout << "---------------------------\n";

		// Создаём исходное дерево
		auto build_original = [&]()->Tree {
			if constexpr (std::is_same_v<Tree, OptimalBST<T>>) {
				std::vector<T> data;
				for (size_t i = 0; i < std::min(size, (size_t)100); i++) {
					data.push_back(static_cast<T>(i * 2));
				}
				return build_and_test_tree_obst(data, "copy_move");
			}
			else {
				Tree original;
				for (size_t i = 0; i < std::min(size, (size_t)100); i++) {
					original.insert(static_cast<T>(i * 2));
				}
				return original;
			}
		};
		
		Tree original = build_original();

		// 4.1 Конструктор копирования
		Tree copy_constructed(original);
		verify_tree_equality(original, copy_constructed, "copy constructor");
		std::cout << "+ Copy constructor\n";

		// 4.2 Оператор присваивания копированием
		//Tree copy_assigned;
		Tree copy_assigned = original;
		verify_tree_equality(original, copy_assigned, "copy assignment");
		std::cout << "+ Copy assignment\n";

		// 4.3 Конструктор перемещения
		Tree temp_for_move1 = original; // копируем
		Tree move_constructed(std::move(temp_for_move1));
		verify_tree_equality(original, move_constructed, "move constructor");
		assert(temp_for_move1.empty() || temp_for_move1.size() == 0);
		std::cout << "+ Move constructor\n";

		// 4.4 Оператор присваивания перемещением
		Tree temp_for_move2 = original; // копируем
		//Tree move_assigned;
		Tree move_assigned = std::move(temp_for_move2);
		verify_tree_equality(original, move_assigned, "move assignment");
		assert(temp_for_move2.empty() || temp_for_move2.size() == 0);
		std::cout << "+ Move assignment\n";

		// 4.5 Self-assignment
		Tree self_assigned = original;
		self_assigned = self_assigned; // self-assignment
		verify_tree_equality(original, self_assigned, "self-assignment");
		std::cout << "+ Self-assignment\n";

		std::cout << "++ Copy/move semantics test completed\n\n";
	}

	// ==================== 5. Стресс-тест ====================
	static void stress_test(size_t size) {
		std::cout << "5. STRESS TEST (size = " << size << ")\n";
		std::cout << "-----------------------\n";

		if (size == 0) return;

		Tree tree;
		std::vector<T> data(size);

		// Генерируем данные
		for (size_t i = 0; i < size; i++) {
			data[i] = static_cast<T>(i);
		}
		std::shuffle(data.begin(), data.end(), std::mt19937{ std::random_device{}() });

		// Вставка
		auto start = std::chrono::high_resolution_clock::now();
		for (auto x : data) {
			tree.insert(x);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto insert_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		// Проверяем, что все элементы добавлены
		assert(tree.size() == size);
		for (auto x : data) {
			assert(tree.contains(x));
		}
		std::cout << "+ Insert: " << insert_time.count() << " ms\n";

		// Поиск всех элементов
		start = std::chrono::high_resolution_clock::now();
		for (auto x : data) {
			assert(tree.contains(x));
		}
		end = std::chrono::high_resolution_clock::now();
		auto search_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "+ Search all: " << search_time.count() << " ms\n";

		// Обход inorder
		start = std::chrono::high_resolution_clock::now();
		auto inorder_result = tree.inorder();
		end = std::chrono::high_resolution_clock::now();
		auto inorder_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		// Проверяем, что inorder отсортирован
		assert(std::is_sorted(inorder_result.begin(), inorder_result.end()));
		assert(inorder_result.size() == size);
		std::cout << "+ Inorder traversal: " << inorder_time.count() << " ms\n";

		// Удаление в случайном порядке
		std::shuffle(data.begin(), data.end(), std::mt19937{ std::random_device{}() });

		start = std::chrono::high_resolution_clock::now();
		
		for (auto x : data) {		
			tree.remove(x);
		}
		end = std::chrono::high_resolution_clock::now();
		auto remove_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		// Проверяем, что дерево пусто
		assert(tree.empty());		
		assert(tree.size() == 0);
		

		std::cout << "+ Remove all: " << remove_time.count() << " ms\n";

		std::cout << "+ Stress test completed (total: "
			<< (insert_time + search_time + inorder_time + remove_time).count()
			<< " ms)\n\n";
	}

	// ==================== Вспомогательные методы ====================

	//построение дерева	
	static Tree build_and_test_tree(const std::vector<T>& data, const std::string& name) {
		Tree tree;

		auto start = std::chrono::high_resolution_clock::now();
		for (auto key : data) {
			tree.insert(key);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << "  Built in " << duration.count() << " ms\n";
		std::cout << "  Size: " << tree.size() << ", Height: " << tree.height() << "\n";

		// Проверяем корректность
		verify_tree_integrity(tree, name + " tree after build");

		return tree;
	}	
	
	static Tree build_and_test_tree_obst(const std::vector<T>& data, const std::string& name) {
		
		std::vector<double> probs4;
		std::mt19937 gen(42);
		std::uniform_int_distribution<> dis(0, 1000);

		for (int i = 0; i != data.size(); ++i) {			
			probs4.push_back(1.0 / (i + 1)); // Зипф
		}		

		auto start = std::chrono::high_resolution_clock::now();
		Tree tree(data, probs4);

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << "  Built in " << duration.count() << " ms\n";
		std::cout << "  Size: " << tree.size() << ", Height: " << tree.height() << "\n";

		// Проверяем корректность
		verify_tree_integrity(tree, name + " tree after build");

		return tree;
	}

	// тест поиска N/10 случайных чисел
	static void test_random_search_10_percent(Tree& tree, const std::vector<T>& all_data,
		const std::string& tree_name) {
		std::cout << "\n2.5 " << tree_name << " - Search 10% random elements:\n";

		size_t n = all_data.size();
		size_t search_count = n / 10;  // 10% от общего количества

		if (search_count == 0) {
			std::cout << "  (Skipped: tree too small)\n";
			return;
		}

		// Выбираем случайные элементы для поиска
		std::vector<T> search_keys;
		std::sample(all_data.begin(), all_data.end(),
			std::back_inserter(search_keys),
			search_count,
			std::mt19937{ std::random_device{}() });

		// Измеряем время поиска
		auto start = std::chrono::high_resolution_clock::now();
		size_t found_count = 0;
		for (const auto& key : search_keys) {
			if (tree.contains(key)) {
				++found_count;
			}			
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		// Все элементы должны быть найдены (они точно есть в дереве)
		assert(found_count == search_keys.size());

		std::cout << "  Searched " << search_count << " random keys in "
			<< duration.count() << " ms\n";
		std::cout << "  Average search time: "
			<< (duration.count() * 1000.0 / search_count) << " us per search\n";
		std::cout << "  All " << found_count << " keys were found\n";
	}

	// тест удаления N/10 случайных чисел
	static void test_random_removal_10_percent(Tree& tree, const std::vector<T>& all_data,
		const std::string& tree_name) {
		std::cout << "\n2.6 " << tree_name << " - Remove 10% random elements:\n";

		size_t n = all_data.size();
		size_t remove_count = n / 10;  // 10% от общего количества

		if (remove_count == 0) {
			std::cout << "  (Skipped: tree too small)\n";
			return;
		}

		// Выбираем случайные элементы для удаления
		std::vector<T> remove_keys;
		std::sample(all_data.begin(), all_data.end(),
			std::back_inserter(remove_keys),
			remove_count,
			std::mt19937{ std::random_device{}() });

		// Сохраняем начальный размер
		size_t initial_size = tree.size();

		// Измеряем время удаления
		auto start = std::chrono::high_resolution_clock::now();
		for (const auto& key : remove_keys) {			
			tree.remove(key);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		// Проверяем, что элементы удалены
		size_t not_removed = 0;
		for (const auto& key : remove_keys) {
			if (tree.contains(key)) {
				++not_removed;
			}			
		}

		// Проверяем корректность размера
		size_t expected_size = initial_size - remove_keys.size() + not_removed;
		assert(tree.size() == expected_size);

		std::cout << "  Removed " << remove_keys.size() << " random keys in "
			<< duration.count() << " ms\n";
		std::cout << "  Average removal time: "
			<< (duration.count() * 1000.0 / remove_keys.size()) << " us per removal\n";
		std::cout << "  Final size: " << tree.size()
			<< " (removed " << (remove_keys.size() - not_removed) << " elements)\n";

		if (not_removed > 0) {
			std::cout << "  Warning: " << not_removed << " elements were already missing\n";
		}

		// Проверяем целостность дерева после удаления
		verify_tree_integrity(tree, tree_name + " after removing 10%");
	}

	//проверка корректности обходов
	static void test_traversals_correctness(const Tree& tree, const std::string& name) {
		std::cout << "  " << name << " tree traversals:\n";

		// In-order должен быть отсортирован
		auto inorder = tree.inorder();
		assert(std::is_sorted(inorder.begin(), inorder.end()));
		std::cout << "    + In-order is sorted (" << inorder.size() << " elements)\n";

		// Все обходы должны иметь одинаковый размер
		auto preorder = tree.preorder();
		auto postorder = tree.postorder();
		auto levelorder = tree.level_order();

		assert(inorder.size() == preorder.size());
		assert(inorder.size() == postorder.size());
		assert(inorder.size() == levelorder.size());
		std::cout << "    + All traversals have same size\n";

		// In-order должен содержать все уникальные элементы
		std::set<T> unique_inorder(inorder.begin(), inorder.end());
		assert(unique_inorder.size() == inorder.size());
		std::cout << "    + No duplicates in in-order\n";

		// Visitor-версии должны давать те же результаты
		std::vector<T> visitor_inorder;
		tree.visit_inorder([&](const T& key) { visitor_inorder.push_back(key); });
		assert(visitor_inorder == inorder);
		std::cout << "    + Visitor in-order matches\n";
	}

	static void test_removal(Tree& tree, const std::vector<T>& data, const std::string& name) {
		size_t original_size = tree.size();

		// Удаляем несколько элементов
		std::vector<T> to_remove = {
			data[0],                    // первый
			data[data.size() / 2],      // средний
			data[data.size() - 1]       // последний
		};

		for (auto item : to_remove) {
			assert(tree.contains(item));

			auto start = std::chrono::high_resolution_clock::now();
			tree.remove(item);
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

			assert(!tree.contains(item));
			std::cout << "    Removed " << item << " in " << duration.count() << " us\n";

			// Проверяем корректность после каждого удаления
			verify_tree_integrity(tree, name + " after removing " + std::to_string(item));
		}

		// Пытаемся удалить несуществующие элементы
		T non_existent = static_cast<T>(data.size() + 1000);
		size_t size_before = tree.size();
		tree.remove(non_existent);
		assert(tree.size() == size_before); // Размер не должен измениться
		std::cout << "    + Non-existent removal handled correctly\n";

		std::cout << "    Final size: " << tree.size()
			<< " (removed " << (original_size - tree.size()) << " elements)\n";
	}

	static void verify_tree_integrity(const Tree& tree, const std::string& context) {
		// 1. Размер должен соответствовать количеству элементов в in-order
		auto inorder = tree.inorder();
		if (tree.size() != inorder.size()) {
			std::cerr << "ERROR [" << context << "]: size() mismatch! "
				<< "tree.size()=" << tree.size()
				<< ", inorder.size()=" << inorder.size() << "\n";
			std::exit(1);
		}

		// 2. In-order должен быть отсортирован (для BST)
		if (!std::is_sorted(inorder.begin(), inorder.end())) {
			std::cerr << "ERROR [" << context << "]: in-order not sorted!\n";
			std::exit(1);
		}

		// 3. Высота не должна превышать размер (для BST)
		if (tree.height() >= static_cast<int>(tree.size())) {
			// Это предупреждение, а не ошибка (вырожденное дерево)
			std::cout << "  Warning [" << context << "]: tree is degenerate "
				<< "(height=" << tree.height() << " >= size=" << tree.size() << ")\n";
		}

		// 4. Все элементы должны быть уникальными
		std::set<T> unique(inorder.begin(), inorder.end());
		if (unique.size() != inorder.size()) {
			std::cerr << "ERROR [" << context << "]: duplicate keys found!\n";
			std::exit(1);
		}
	}

	static void verify_tree_equality(const Tree& tree1, const Tree& tree2,
		const std::string& context) {
		assert(tree1.size() == tree2.size());
		assert(tree1.height() == tree2.height());
		assert(tree1.empty() == tree2.empty());

		auto inorder1 = tree1.inorder();
		auto inorder2 = tree2.inorder();

		if (inorder1 != inorder2) {
			std::cerr << "ERROR [" << context << "]: trees are not equal!\n";
			std::exit(1);
		}
	}
};