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
    SECTION("Can default construct an empty hashmap and do basic operations on it ") {
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

        // can insert via [] operator
        map["a"] = 10;
        // or insert explicitly via 'insert', which should succeed
        REQUIRE(map.insert("b", 20));
        map["c"] = 30;

        REQUIRE(map.containsKey("a"));
        // can use the [] operator to read
        REQUIRE(map["a"] == 10);
        REQUIRE(map["b"] == 20);
        REQUIRE(map["c"] == 30);

        REQUIRE(map.size() == 3);
        REQUIRE(!map.empty());

        // IMPORTANT: All sections nested within this section, are independent of each other,
        // but they all begin in the same state, where the map contains the entries
        // (a,10), (b,20), (c,30)

        SECTION("Can't use insert() when key already exists") {
            REQUIRE(!map.insert("a", 5));
            // inserting can't overwrite values
            REQUIRE(map["a"] == 10);
        }

        SECTION("Can mutate previously inserted elements") {
            // we have the same 3 elements we began with
            REQUIRE(map.size() == 3);

            // can mutate with [] operator
            map["a"] = 1;
            map["c"] = 3;
            // can mutate with 'at' method
            map.at("b") = 2;

            REQUIRE(map["a"] == 1);
            REQUIRE(map["b"] == 2);
            REQUIRE(map["c"] == 3);

            // these mutations didn't change the map's size
            REQUIRE(map.size() == 3);
            // in other sections, "a", "b", "c" are unaffected(they're still 10,20,30)
        }

        SECTION("Can erase elements") {
            // we have the same 3 elements we began with
            REQUIRE(map.size() == 3);

            // can first erase the value
            REQUIRE(map.erase("a"));

            // can't erase it twice
            REQUIRE(!map.erase("a"));

            REQUIRE(map.size() == 2);
            REQUIRE(!map.containsKey("a"));
            REQUIRE_THROWS(map.at("a"));
            // in other sections, "a" wasn't removed (sections are independent of each other)
        }

        SECTION("Can't erase non-existent elements") {
            REQUIRE(!map.erase("not in map"));
            REQUIRE(map.size() == 3);
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

    SECTION("Creating hashmap with vectors of mismatching sizes throws an exception") {
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

        // 1 million insertions
        // this shouldn't take more than a few seconds on Aquarium PCs.
        // If this is too slow, your implementation might be too inefficient (maybe its resizing too often/not enough,
        //  or doing something terribly complicated).
        int iterCount = 1000000;

        // first step, adding random elements to both maps
        for (int i=0; i < iterCount; ++i)
        {
            int key = intGen(rng);
            int val = intGen(rng);
            stdMap[key] = val;
            myMap[key] = val;
            // if one of these fail, your map's behavior is incorrect
            REQUIRE(myMap.at(key) == val);
            REQUIRE(stdMap.size() == myMap.size());

            // sanity check, std::unordered_map is obviously correct
            REQUIRE(stdMap.at(key) == val);
        }

        REQUIRE(myMap.size() == stdMap.size());
        int its = 0;
        for (const auto& kvp : myMap)
        {
            ++its;
            // if this fails, your map doesn't contain the same elements as the ones in the standard map
            // (even though at a previous point in time, this key-value was in both of your maps)
            // this probably indicates
            REQUIRE(stdMap.at(kvp.first) == kvp.second);
            REQUIRE(myMap.at(kvp.first) == kvp.second);
        }
        // if this fails, you have an issue with your iterator: you didn't iterate over the entire map
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
            // sanity check, can't fail (std erase returns number of elements erased)
            REQUIRE(stdMap.erase(key) == 1);

            // if this fails, either the previous iterator somehow iterated over a non-existent key, or,
            // erasing a previous key somehow caused this key to be erased.
            REQUIRE(myMap.erase(key));

            // if we erased one element from your map, and one from the standard map, both started with equal sizes
            // and thus should end with equal sizes
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

TEST_CASE("HashMap iterator tests")
{
        SECTION("Can iterate over the hashmap") {
            const HashMap<std::string, int> map({"a", "b", "c"},
                                          {10, 20, 30});

            std::unordered_map<std::string, int> stdMap;
            // note that this is an inefficient way of iterating, because we're making unnecessary copies
            for (std::pair<std::string, int> pair: map)
            {
                stdMap[pair.first] = pair.second;
            }

            REQUIRE(stdMap["a"] == 10);
            REQUIRE(stdMap["b"] == 20);
            REQUIRE(stdMap["c"] == 30);

            // if you implemented the iterator correctly, you should also be able to iterate via const references
            // (the preferred way to iterate):
            for (const std::pair<std::string, int>& pair: map)
            {
                stdMap[pair.first] = 0;
            }

            REQUIRE(stdMap["a"] == 0);
            REQUIRE(stdMap["b"] == 0);
            REQUIRE(stdMap["c"] == 0);
    }

    SECTION("Can iterate over an empty hashmap")
    {
            HashMap<int, int> myMap;
            int iters = 0;
            for (const auto& kvp: myMap)
            {
                ++iters;
            }
            REQUIRE(iters == 0);
    }

    SECTION("Can create and iterate over large hashmap that underwent several resizes")
        {
            HashMap<int, int> myMap;

            // first, filling our map with 1024 key-value pairs
            for (int i=1; i <= 1024; ++i)
            {
                myMap[i] = 1;
                // if this fails, you probably have an issue with your resizing code
                REQUIRE(myMap.size() == i);
                REQUIRE(myMap.at(i) == 1);
            }

            int iterCount = 0;
            for(const std::pair<int, int>& kvp: myMap)
            {
                ++iterCount;
                // if this fails, somehow you iterate over values that aren't in the map
                REQUIRE(myMap.at(kvp.first) == kvp.second);
            }
            // if this fails, you didn't iterate over the entire map, probably an issue with the iterator
            // implementation
            REQUIRE(iterCount == myMap.size());
        }
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
        HashMap<int, int> nonConst({1,2}, {5,5});

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