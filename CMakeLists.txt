project(TESTERS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# "test_my_impl" runs the tester on your own implementation
add_executable(test_hashmap test_hashmap.cpp catch.hpp ../HashMap.hpp)
