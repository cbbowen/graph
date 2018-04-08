
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <iostream>
struct wait_on_end {
	~wait_on_end() {
		std::cout << "Press ENTER to continue." << std::endl;
		std::cin.get();
	}
};
static wait_on_end _;
