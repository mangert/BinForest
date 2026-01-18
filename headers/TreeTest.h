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
	
	static void test(size_t size, T max = 1000) {

		//числа в случайном порядке
		std::vector<T> data = gen_data(size, max);
		Tree rnd_tree = build_tree_test(data);
		std::cout << "-------------------------\n";
		std::sort(data.begin(), data.end());
		//числа по порядку
		Tree ord_tree = build_tree_test(data);
		std::cout << "-------------------------\n";
	}
	


private:

	static Tree build_tree_test(std::vector<T>& data) {
		
		Tree tree;		
		auto start_time = std::chrono::high_resolution_clock::now();
		for (auto& key : data) {
			tree.insert(key);
		}
		auto end_time = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			end_time - start_time);
		std::cout << " | Time: " << duration.count() << " ms\n";
		return tree;
	}
	
	static std::vector<T> gen_data(size_t size, T max) {
		
		std::random_device rd;
		std::mt19937 gen(rd()); 
		std::uniform_int_distribution<T> distrib(0, max);
		std::vector<T> result(size);

		for (size_t i = 0; i != size; ++i) {
			result[i] = static_cast<T>((distrib(gen)));
		};
		return result;		
	}	
};
