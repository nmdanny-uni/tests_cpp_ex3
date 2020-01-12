project(TESTERS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# "test_my_impl" runs the tester on your own implementation
add_executable(test_hashmap test_hashmap.cpp catch.hpp ../HashMap.hpp)

if(MINGW)
    find_program(HAS_LLD lld)
    if (HAS_LLD)
        # see https://github.com/catchorg/Catch2/issues/1205
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
    endif()
endif()
