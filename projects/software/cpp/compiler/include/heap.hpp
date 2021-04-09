#ifndef __HEAP_H__
#define __HEAP_H__

#include <cstdint>
#include <list>
#include <unordered_map>

struct Block {
    uint16_t addr;
    uint16_t numSlots;
};

class Heap {
    const uint16_t baseAddr;
    const uint16_t capacity;
    std::unordered_map<uint16_t, uint16_t> alloctedBlocks;
    std::list<Block> freeBlocks;

    public:
    Heap(uint16_t baseAddr, uint16_t capacity);
    uint16_t alloc(uint16_t numSlots);
    void dealloc(uint16_t addr);
};

#endif
