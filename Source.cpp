#include "trim_view.h"
#include <iostream>
#include <vector>

int main() {
	std::vector<int> numbers{ 1,3,4,3,1,6,7,6,9,9,5 };
	auto odd_numbers = [](auto n) {return n % 2 != 0; };
	for (auto elem : std::ranges::views::filter(numbers, odd_numbers))
		std::cout << elem << " ";
}