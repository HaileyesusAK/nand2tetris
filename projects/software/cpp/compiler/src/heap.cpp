#include <algorithm>
#include <cstdint>
#include <iterator>
#include <list>
#include <stdexcept>
#include <unordered_map>

#include "heap.hpp"
Heap::Heap() : baseAddr(0), capacity(0) {}
Heap::Heap(uint16_t _baseAddr, uint16_t _capacity) : baseAddr(_baseAddr), capacity(_capacity){
    freeBlocks.push_back({baseAddr, capacity});
}

uint16_t Heap::alloc(uint16_t count) {
    auto cond = [&count] (const Block& b) { return b.numSlots >= count; };
    auto pos = std::find_if(freeBlocks.begin(), freeBlocks.end(), cond);
    if(pos == freeBlocks.end())
        throw std::length_error("No enough free space");

    auto addr = pos->addr;
    alloctedBlocks[addr] = count;

    if(pos->numSlots == count)
        freeBlocks.erase(pos);
    else {
        pos->addr += count;
        pos->numSlots -= count;
    }
    return addr;
}

void Heap::dealloc(uint16_t addr) {
    if(alloctedBlocks.count(addr)) {
        auto cond = [&addr] (const Block& b) { return b.addr >= addr; };
        auto pos = std::find_if(freeBlocks.begin(), freeBlocks.end(), cond);
        auto allocSize = alloctedBlocks[addr];

        if(addr + allocSize == pos->addr) { //The freed block can be merged with the free block RIGHT AFTER it
            pos->addr = addr;
            pos->numSlots += allocSize;
        }
        else {
            pos = freeBlocks.insert(pos, {addr, allocSize});
        }

        // Check if the newly freed block can be merged with the free block RIGHT BEFORE it
        if(pos != freeBlocks.begin()) {
            auto prev = std::prev(pos, 1);
            if(prev->addr + prev->numSlots == pos->addr) {
                prev->numSlots += pos->numSlots;
                freeBlocks.erase(pos);
            }
        }

        alloctedBlocks.erase(addr);
    }
}
