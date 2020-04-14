#include <iostream>
#include "include/seq_list.hpp"

int main()
{
	ds::seq_list<int> list(3, 2);

	std::cout << list[0] << std::endl;

	return 0;
}
