#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../HashMap.hpp"
#include <set>
#include <random>
#include <unordered_map>

TEST_CASE("Sanity check, ensure you configured the tests correctly") {
    REQUIRE(1 + 1 == 2);
}

TEST_CASE("Basic HashMap tests") {
    // IMPORTANT: each section is independently from other sections, and has no way to affect other sections.
    // For example, clearing the map in one section, only affects that section
    SECTION("Can default construct an empty hashmap and do stuff on it") {
        const HashMap<std::string, int> map;
        REQUIRE(map.capacity() == 16);
        REQUIRE(map.size() == 0);
        REQUIRE(map.empty());

        REQUIRE(!map.containsKey("not in map"));
        REQUIRE_THROWS(map.bucketIndex("not in map"));
        REQUIRE_THROWS(map.bucketSize("not in map"));

        REQUIRE_THROWS(map.at("not in map"));
    }

    SECTION("Can insert elements to a hashmap") {
        HashMap<std::string, int> map;
        map["a"] = 10;
        REQUIRE(map.insert("b", 20));
        map["c"] = 30;

        REQUIRE(map.containsKey("a"));
        REQUIRE(map["a"] == 10);
        REQUIRE(map["b"] == 20);
        REQUIRE(map["c"] == 30);
        REQUIRE(map.size() == 3);
        REQUIRE(!map.empty());

        SECTION("Can't use insert() when key already exists") {
            REQUIRE(!map.insert("a", 5));
            REQUIRE(map["a"] == 10);
        }

        SECTION("Can mutate previously inserted elements") {
            map["a"] = 1;
            map.at("b") = 2;
            map["c"] = 3;
            REQUIRE(map["a"] == 1);
            REQUIRE(map["b"] == 2);
            REQUIRE(map["c"] == 3);
            REQUIRE(map.size() == 3);
        }

        SECTION("Can erase elements") {
            REQUIRE(map.size() == 3);
            REQUIRE(map.insert("d", 5));
            REQUIRE(map.erase("d"));
            REQUIRE(map.size() == 3);
            REQUIRE(!map.containsKey("d"));
            REQUIRE_THROWS(map.at("d"));
        }

        SECTION("Can't erase non-existent elements") {
            REQUIRE(!map.erase("not in map"));
            REQUIRE(map.size() == 3);
        }

        SECTION("Can iterate over the hashmap") {
            using Catch::Matchers::UnorderedEquals;
            std::unordered_map<std::string, int> stdMap;
            for (const std::pair<std::string, int>& pair: map)
            {
                stdMap[pair.first] = pair.second;
            }


            REQUIRE(stdMap["a"] == 10);
            REQUIRE(stdMap["b"] == 20);
            REQUIRE(stdMap["c"] == 30);
        }

        SECTION("Can create and iterate over large hashmap that underwent several resizes")
        {
            HashMap<int, int> myMap;
            std::unordered_map<int, int> stdMap;
            for (int i=1; i <= 1024; ++i)
            {
                myMap[i] = 1;
                stdMap[i] = 1;
                REQUIRE(myMap.size() == stdMap.size());
            }

            int iterCount = 0;
            for(const auto& kvp: myMap)
            {
                ++iterCount;
                REQUIRE(stdMap[kvp.first] == kvp.second);
            }
            REQUIRE(iterCount == myMap.size());
        }

        SECTION("Can clear the hashmap") {
            int oldCapacity = map.capacity();
            map.clear();
            REQUIRE(map.empty());
            REQUIRE(map.size() == 0);
            REQUIRE(!map.containsKey("a"));
            REQUIRE(!map.containsKey("b"));
            REQUIRE(!map.containsKey("c"));
            // clear doesn't change capacity
            REQUIRE(map.capacity() == oldCapacity);
        }

    }

    SECTION("HashMap has copy constructor") {
        HashMap<std::string, int> orig;
        orig["a"] = 1;
        orig["b"] = 2;
        orig["c"] = 3;
        const HashMap<std::string, int> copy(orig);

        SECTION("Const correctness: 'at', 'size', 'containsKey', and operator[] work for const maps") {


            REQUIRE(copy.size() == 3);

            REQUIRE(copy.containsKey("a"));
            REQUIRE(copy.at("a") == 1);
            REQUIRE(copy["a"] == 1);

            REQUIRE(copy.containsKey("b"));
            REQUIRE(copy.at("b") == 2);
            REQUIRE(copy["b"] == 2);

            REQUIRE(copy.containsKey("c"));
            REQUIRE(copy.at("c") == 3);
            REQUIRE(copy["c"] == 3);
        }

    }

    SECTION("Creating hashmap with mismatching keys and values vector throws an exception") {
        REQUIRE_THROWS(HashMap<std::string, int>({"a", "b"}, {1}));
    }

    SECTION("Can create hashmap from keys and values vectors") {
        const HashMap<std::string, int> map(
            {"a", "b", "c", "d", "e", "e"},
            {1, 2, 3, 4, 50, 5}
            // since 'e' appears twice, its later occurrence should overwrite the former
        );

        REQUIRE(map.size() == 5);
        REQUIRE(map.at("a") == 1);
        REQUIRE(map.at("b") == 2);
        REQUIRE(map.at("c") == 3);
        REQUIRE(map.at("d") == 4);
        REQUIRE(map.at("e") == 5);


        SECTION("Map equality works") {

            // map equals itself
            REQUIRE(map == map);
            REQUIRE(!(map != map));

            const HashMap<std::string, int> map2(
                {"a", "b", "c", "d", "e"},
                {1, 2, 3, 4, 5}
            );

            REQUIRE(map == map2);
            REQUIRE(!(map != map2));

            const HashMap<std::string, int> map3(
                {"a", "b"}, {1,2}
            );

            REQUIRE(!(map == map3));
            REQUIRE(map != map3);
        }
    }

}

TEST_CASE("HashMap tests for large inputs") {
    SECTION("Behaves similarly to a std::unordered_map")
    {
        std::unordered_map<int, int> stdMap;
        HashMap<int, int> myMap;

        // stuff for generating numbers between 0 to 100k
        std::mt19937 rng;
        uint32_t seed = 1337;
        rng.seed(seed);
        std::uniform_int_distribution<int> intGen(0, 100000);

        int iterCount = 100000;

        // first step, adding random elements to both maps
        for (int i=0; i < iterCount; ++i)
        {
            int key = intGen(rng);
            int val = intGen(rng);
            stdMap[key] = val;
            myMap[key] = val;
            REQUIRE(myMap.at(key) == val);
            REQUIRE(stdMap.at(key) == val);
            REQUIRE(stdMap.size() == myMap.size());
        }

        // checking that both maps are equal
        REQUIRE(myMap.size() == stdMap.size());
        int its = 0;
        for (const auto& kvp : myMap)
        {
            ++its;
            REQUIRE(stdMap.at(kvp.first) == kvp.second);
            REQUIRE(myMap.at(kvp.first) == kvp.second);
            //CHECK(stdMap.load_factor() == myMap.getLoadFactor());
        }
        REQUIRE(its == myMap.size());

        // second step, randomly deleting 20% of the keys
        std::uniform_int_distribution<int> shouldErase(1, 5);
        // due to iterator invalidation, we'll perform the deletion in bulk - we'll use one loop to determine which
        // keys to delete, and another loop(on the vector rather than map) to delete the items
        std::vector<int> keysToDelete;
        for (const auto& kvp: myMap)
        {
            if (shouldErase(rng) == 1)
            {
                keysToDelete.push_back(kvp.first);
            }
        }
        for (int key: keysToDelete)
        {
            REQUIRE(stdMap.erase(key) == 1);
            REQUIRE(myMap.erase(key));
            REQUIRE(myMap.size() == stdMap.size());
        }

        // checking that both maps are equal after deletion
        REQUIRE(myMap.size() == stdMap.size());
        for (const auto& kvp : myMap)
        {
            REQUIRE(stdMap.count(kvp.first) == 1);
            REQUIRE(stdMap[kvp.first] == kvp.second);
        }
    }
}

TEST_CASE("Advanced HashMap iterator usage")
{
    SECTION("Usage as an Input Iterator(subset of Forward Iterator): use std::accumulate to sum all keys in the map") {
        HashMap<int, int> myMap;
        myMap[1] = 1;
        myMap[2] = 1;
        myMap[3] = 1;
        myMap[4] = 1;

        // summing all keys of the map
        // also, i'm using here a lambda function, which is similar to what we've seen in Java/Python.
        // (std::accumulate is like 'reduce' in Python, or 'Stream.reduce' in Java)
        int keySum = std::accumulate(myMap.begin(), myMap.end(), 0,
                                     [](int sum, std::pair<int, int> kvp) -> int {
                                         return sum + kvp.first;
                                     });
        REQUIRE(keySum == 1 + 2 + 3 + 4);
    }

    SECTION("Usage as a Forward Iterator: use std::is_permutation to find a permutation of elements") {
        HashMap<int, int> myMap;
        myMap[1] = 2;
        myMap[2] = 1;

        std::vector<std::pair<int,int>> perm{{1,2}, {2,1}};

        /* this is a silly example, because we don't have a well defined order in a HashMap. But no matter how your map
         * is implemented, iterating it must give the pairs [(1,2), (2,1)] or [(2,1), (1,2)], thus, it must contain the above
         * permutation.
         * If you don't understand, see the documentation about std::is_permutation. The most important thing is that it
         * uses ForwardIt.
         */
        bool has_perm = std::is_permutation(myMap.begin(), myMap.end(), perm.cbegin());

        REQUIRE(has_perm);
    }

    SECTION("Const correctness: Can use iterators on constant map") {
        // using ctor that takes keys and values vectors
        HashMap<int, int> nonConst({1,2}, {5,5});

        // using copy ctor
        const HashMap<int, int> constMap(nonConst);

        int keySum = 0, valueSum = 0;
        for (const std::pair<int, int>& kvp: constMap)
        {
            keySum += kvp.first;
            valueSum += kvp.second;
        }
        REQUIRE(keySum == 3);
        REQUIRE(valueSum == 10);
    }
}