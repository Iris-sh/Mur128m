#include <vector>
#include <cstdint>
#include <cstddef>
struct Mem_block {
    bool filled;
    unsigned __int128 begin_addr;
    unsigned __int128 block_size;
    std::vector<std::uint8_t> content;
    bool executed;
    Mem_block(unsigned __int128 begin_addr, unsigned __int128 block_size, std::vector<std::uint8_t> content) {
        filled = true;
        executed = true;
        this->begin_addr = begin_addr;
        this->block_size = block_size;
        this->content = content;
    }
};
struct Memory_info{
    std::vector<Mem_block> block;
};
