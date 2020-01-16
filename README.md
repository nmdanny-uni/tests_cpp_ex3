# Introduction

There are two types of tests included:
- C++ based tests(using the Catch2 unit testing library), designed to be ran via CLion. These test your HashMap implementation.

- A python tester(using pytest) 

It is extremely likely you'll face compilation issues  with the C++ tester - these are parts of
 the tests!
The tests ensure your implementation lines up with the API in terms of proper signatures - const correctness, proper
iterator implementation, proper operator/function overloads, etc...

Unfortunately, in some of these cases, the error messages you'll get are very opaque and confusing - it is your job to
debug and understand what went wrong. Try looking at the source of the error in the test code, to understand what the
tester is trying to do and where your implementation is lacking.


## Requirements
The tests are designed to be ran via a HUJI system.

Your executable should be located at `YOUR_PROJECT_ROOT/cmake-build-debug` and named `SpamDetector`,
if it isn't - make sure 

## Installation

1. Clone this repository into your project root(you may need to provide your CSE credentials for this):

```shell script
cd YOUR_PROJECT_ROOT
git clone https://github.cs.huji.ac.il/danielkerbel/tests_cpp_ex3 tests
```

2. Add the line `add_subdirectory(tests)` to the main `CMakeLists.txt` file(the one created by CLion
for your project's root, not the one within the `tests` folder). 
After adding the line, it should look like this:
    ```cmake
    cmake_minimum_required(VERSION 3.10)
    project(cpp_ex3)
    
    set(CMAKE_CXX_STANDARD 14)
    
    find_package(Boost COMPONENTS filesystem REQUIRED)
    include_directories(${Boost_INCLUDE_DIR})
    
    add_executable(SpamDetector SpamDetector.cpp HashMap.hpp)
    target_link_libraries(SpamDetector ${Boost_LIBRARIES})
    target_compile_options(SpamDetector PRIVATE -Wall -Wextra -Wvla -Werror)
    
    add_subdirectory(tests)
    
    ```
    (Note this also makes compilation warnings treated as errors, which you should probably do anyway)

3. Create a `Catch` run configuration in CLion, using `test_hashmap` as the **Target:**.

4. Create a `python tests | pytest` run configuration, using `tester` as **Target: Module Name**
   If using CLion, you'll probably need to configure the python interpreter as follows:
   `Python Interpreter: Use specified Interpreter: Python 3.7`
  

# Running

- Run the above c++ configuration, optionally with Valgrind memcheck.(`Run | Run ... | all in
 test_hashmap | Run with Valgrind Memcheck` ) This will automatically
 compile your project along with the tester.
 
- Similarly you can run the python tester. Alternatively, you can type in the terminal: 
  `python3 -m pytest -vvs tester.py`
  Note that this doesn't automatically re-compile your project. 

