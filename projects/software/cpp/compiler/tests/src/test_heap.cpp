#include <algorithm>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

#include "heap.hpp"
#include "catch.hpp"

TEST_CASE("Allocating the first valid free block", "[alloc]") {
    Heap heap(0, 100);
    heap.alloc(20);
    auto a = heap.alloc(30);
    heap.alloc(40);
    heap.dealloc(a);
    REQUIRE(heap.alloc(10) == 20);  //Must allocate on the first available free block
}

TEST_CASE("Allocating above the available capacity", "[alloc]") {
    Heap heap(0, 30);
    heap.alloc(10);
    auto a = heap.alloc(10);
    heap.alloc(10);
    heap.dealloc(a);

    //Must throw an exception even if the total available size is 20
    REQUIRE_THROWS_AS(heap.alloc(20), std::length_error);
}

TEST_CASE("Merging right free block", "[alloc]") {
    // Verify that a freed block is merged with a previously freed
    // contiguous block that is found RIGHT AFTER it
    Heap heap(0, 40);
    heap.alloc(10);
    auto a = heap.alloc(10);
    auto b = heap.alloc(10);
    heap.alloc(10);

    heap.dealloc(b);
    heap.dealloc(a);    //Must combine the space occupied by a and b
    REQUIRE(heap.alloc(15) == a);
}

TEST_CASE("Merging left free block", "[alloc]") {
    // Verify that a freed block is merged with a previously freed
    // contiguous block that is found RIGHT BEFORE it
    Heap heap(0, 40);
    heap.alloc(10);
    auto a = heap.alloc(10);
    auto b = heap.alloc(10);
    heap.alloc(10);

    heap.dealloc(a);
    heap.dealloc(b);    //Must combine the space occupied by a and b
    REQUIRE(heap.alloc(15) == a);
}

TEST_CASE("Merging both left and right free blocks", "[alloc]") {
    // Verify that a freed block is merged with previously freed
    // contiguous blocks that are just BEFORE and AFTER it.
    Heap heap(0, 40);
    auto a = heap.alloc(10);
    auto b = heap.alloc(10);
    auto c = heap.alloc(10);
    heap.alloc(10);

    heap.dealloc(a);
    heap.dealloc(c);
    heap.dealloc(b);    //Must combine the space occupied by a, b and c
    REQUIRE(heap.alloc(15) == a);
}

TEST_CASE("Deallocting randomly", "[alloc]") {
    Heap heap(10, 100);
    std::vector<uint16_t> addresses;
    std::vector<uint16_t> sizes {10, 20, 30, 40};
    for(auto s : sizes)
        addresses.push_back(heap.alloc(s));

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(addresses.begin(), addresses.end(), g);
    for(auto addr : addresses)
        heap.dealloc(addr);

    REQUIRE(heap.alloc(100) == 10); // Verify the heap is fully reusable after the deallocations
}
