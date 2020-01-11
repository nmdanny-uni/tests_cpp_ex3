#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../HashMap.hpp"

TEST_CASE("Sanity check") {
    REQUIRE(1 + 1 == 2);
}

TEST_CASE("HashMap initialization/destruction") {
    SECTION("Can default construct an empty hashmap") {
        const HashMap<std::string, int> map;
        REQUIRE(map.capacity() == 16);
        REQUIRE(map.size() == 0);
        REQUIRE(map.empty());

        REQUIRE(!map.containsKey("not in map"));
        REQUIRE(map.bucketIndex("not in map") == -1);
        REQUIRE(map.bucketSize("not in map") == 0);

        REQUIRE_THROWS(map.at("not in map"));
    }

    SECTION("Can insert elements to a hashmap") {
        HashMap<std::string, int> map;
        map["a"] = 1;
        map.insert("b", 2);
        map["c"] = 30;
        map["c"] = 3;
    }
}