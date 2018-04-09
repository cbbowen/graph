
// Older versions of libstdc++ don't have `std::uncaught_exceptions`
#ifndef __has_include
#	define __has_include(...) false
#endif
#if __has_include(<cxxabi.h>)
#	include <cxxabi.h>
#endif
#if defined(__GLIBCXX__) && (__GLIBCXX__ <= 20150426)
#	include <exception>
namespace std {
	int uncaught_exceptions() noexcept {
		// This obviously doesn't handle multiple exceptions correctly, but that should never happen in this library given the limited usage of exceptions (only checking preconditions in debug builds).
		return uncaught_exception() ? 1 : 0;
	}
}
#endif

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
